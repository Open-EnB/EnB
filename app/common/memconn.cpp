// Earth & Beyond client (reconstruction): app\common\memconn.cpp. Notes at the end of the file.
#include "memconn.h"
#include "..\packets\pk.h"
#include "..\shared\unknown_debug.h"
#include "..\shared\auxdata\unknown_root.h"

MemoryConnectionClass::MemoryConnectionClass() :
	SendPipe(0), RecvPipe(0)
{
}


MemoryConnectionClass::~MemoryConnectionClass()
{
	Release();
}

bool MemoryConnectionClass::Send_Message(RawPacketClass *packet)
{

	G_ASSERT(0);
	return(false);
}

// The packet built and queued on the send pipe, due after its delay
bool MemoryConnectionClass::Send_Packet(PacketClass *vp)
{

	G_ASSERT(vp);
	if (vp == 0) {
		return(false);
	}

	G_ASSERT(SendPipe);
	if (SendPipe == 0) {
		return(false);
	}
	if (!Is_Connected()) {
		return(false);
	}
	MemoryMessageClass message; message.Packet = vp->Build(); message.Time = Unknown_Get_Ticks() + SendPipe->Delay;
	SendPipe->Messages.push_back(message);
	return(true);
}

// The receive pipe's first message once it is due, as a packet
PacketClass *MemoryConnectionClass::Receive()
{
	if (RecvPipe != 0 && RecvPipe->Messages.size() != 0) {
		MemoryMessageClass &message = RecvPipe->Messages.front();
		if (message.Time <= Unknown_Get_Ticks()) {
			message.Packet->Data.Seek(0);
			PacketClass *packet = Unknown_869790(message.Packet);
			packet->Connection = this;
			delete message.Packet;
			RecvPipe->Messages.pop_front();
			return(packet);
		}
	}
	return(0);
}

bool MemoryConnectionClass::Is_Connected()
{
	return(SendPipe != 0 && SendPipe->RefCount > 1);
}

bool MemoryConnectionClass::Is_Dead()
{
	return(RecvPipe == 0 || (RecvPipe->Messages.size() == 0 && RecvPipe->RefCount == 1));
}

bool MemoryConnectionClass::Disconnect()
{
	Release();
	return(true);
}

void MemoryConnectionClass::Clear_Pipe(MemoryPipeClass *pipe)
{
	for (std::list<MemoryMessageClass>::iterator it = pipe->Messages.begin(); it != pipe->Messages.end(); ++it) {
		delete it->Packet;
	}
	pipe->Messages.clear();
}

// Each pipe is freed with its last connection
// (Clear_Pipe's loop takes `++it`: `it++` spends the inline budget, so the two copies here inline less.)
void MemoryConnectionClass::Release()
{
	if (SendPipe != 0) {
		SendPipe->RefCount--;
		if (SendPipe->RefCount == 0) {
			Clear_Pipe(SendPipe);
			delete SendPipe;
		}
		SendPipe = 0;
	}
	if (RecvPipe != 0) {
		RecvPipe->RefCount--;
		if (RecvPipe->RefCount == 0) {
			Clear_Pipe(RecvPipe);
			delete RecvPipe;
		}
		RecvPipe = 0;
	}
}

// Two connections joined by two pipes: what one sends, the other receives
void MemoryConnectionClass::Create_Pair(MemoryConnectionClass *&c1, MemoryConnectionClass *&c2)
{
	c1 = new MemoryConnectionClass;
	c2 = new MemoryConnectionClass;































	G_ASSERT(c1 && c2);

	MemoryPipeClass *p1 = new MemoryPipeClass;
	MemoryPipeClass *p2 = new MemoryPipeClass;

	G_ASSERT(p1 && p2);
	p1->RefCount = 2;
	p2->RefCount = 2;
	c1->SendPipe = p1;
	c2->RecvPipe = p1;
	c1->RecvPipe = p2;
	c2->SendPipe = p2;
}

void MemoryConnectionClass::Set_Send_Delay(int delay)
{






	G_ASSERT(SendPipe);
	if (SendPipe != 0) {
		SendPipe->Delay = delay;
	}
}

void MemoryConnectionClass::Set_Recv_Delay(int delay)
{

	G_ASSERT(RecvPipe);
	if (RecvPipe != 0) {
		RecvPipe->Delay = delay;
	}
}

// Options 0 / 1 read the send / receive delay into *data, 2 / 3 set it from *data
// (the first case nested: the client keeps its null-data assert and jumps to it from the second)
bool MemoryConnectionClass::Unknown_30(int option, int *data)
{
	switch (option) {
		case 0:
		case 2:


			G_ASSERT(data);
			if (data != 0) {


				G_ASSERT(SendPipe);
				if (SendPipe == 0) return(false);
				break;
			}
			return(false);

		case 1:
		case 3:
			G_ASSERT(data);
			if (data == 0) {
				return(false);
			}
			G_ASSERT(RecvPipe);
			if (RecvPipe == 0) {
				return(false);
			}
			break;

		default:
			return(false);
	}

	switch (option) {
		case 0: *data = SendPipe->Delay; return(true);
		case 1: *data = RecvPipe->Delay; return(true);
		case 2: Set_Send_Delay(*data); return(true);
		case 3: Set_Recv_Delay(*data); return(true);
	}
	return(true);
}

// MemoryListenerClass
MemoryListenerClass::MemoryListenerClass() :
	PortNumber(0)
{
}

unsigned short MemoryListenerClass::Get_Port() { return(PortNumber); }
bool MemoryListenerClass::Is_Listening() { return(PortNumber != 0); }

MemoryListenerClass::~MemoryListenerClass()
{














	FAIL_IF(PortNumber != 0) Stop();
	Clear_Pending();
}

void MemoryListenerClass::Clear_Pending()
{
	for (std::list<ConnectionClass *>::iterator it = Pending.begin(); it != Pending.end(); ++it) {
		delete *it;
	}
	Pending.clear();
}

// A new pair: the server's end waits for Accept, the client gets the other
ConnectionClass *MemoryListenerClass::Connect()
{
	MemoryConnectionClass *c1 = 0;
	MemoryConnectionClass *c2 = 0;
	MemoryConnectionClass::Create_Pair(c1, c2);
	Pending.push_back(c2);
	return(c1);
}

ConnectionClass *MemoryListenerClass::Accept()
{
	if (Pending.size() == 0) {
		return(0);
	}
	ConnectionClass *connection = Pending.front();
	Pending.pop_front();
	return(connection);
}

bool MemoryListenerClass::Listen(unsigned short port)
{

















	FAIL_IF(port == 0) return(false);


	FAIL_IF(PortNumber != 0) return(false);
	MemoryConnectionFactoryClass factory;
	if (!factory.Register(port, this)) {
		return(false);
	}
	PortNumber = port;
	return(true);
}

bool MemoryListenerClass::Stop()
{








	FAIL_IF(PortNumber == 0) return(false);
	bool result = MemoryConnectionFactoryClass().Unregister(PortNumber);
	PortNumber = 0;
	return(result);
}

/*
**	Reconstruction notes (memconn.cpp; docs/kernel-map.md 4, step 8)
**
**	* The client's functions, in order: the memory connection 0x826480-0x826E30 (constructor,
**	  destructor, its slots, Clear_Pipe, Release, Create_Pair on lines 145 / 150, the delays on 167
**	  / 176, the option slot +0x30 on 191-207), the memory listener 0x826EB0-0x827380 (the
**	  destructor's FAIL_IF on 251, Listen on 302 / 305, Stop on 324) and the std::list helpers.
**	* The registry by port 0x827560-0x828A00 comes after memconn.cpp's template instances and
**	  Listen / Stop / the factory call it out of line: another file (unknown_memregistry.cpp).
*/
