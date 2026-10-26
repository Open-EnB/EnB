// Earth & Beyond client (reconstruction): app\common\connect.cpp (PARTIAL). Notes at the end of the file.
#include "connect.h"
#include "..\packets\pk.h"
#include "..\shared\unknown_debug.h"
#include <string.h>
#include "..\..\wwlib\nstrdup.h"

ConnectionClass::ConnectionClass()
{
	Unknown04 = true;
}


ConnectionClass::~ConnectionClass()
{
}

// The header Receive reads (0xBE8EA0).
static PacketHeaderClass Header;

// (not rebuilt here: the factories 0x821000-0x821FF0, ConnectionManager's Accept_New 0x822300, Add_Connection 0x822CC0 and helpers, the wrappers to 0x8232E0; spacing kept: the asserts are on the client's lines)
















































































































































































































































































































































































































































































// ConnectionManagerClass (vtable 0xB085D0, 0x28 bytes): connections polled round robin
ConnectionManagerClass::ConnectionManagerClass() :
	RoundRobin(0),
	Current(0),
	Unknown1C(false),
	Name(0),
	Listener(0)
{
}

ConnectionManagerClass::~ConnectionManagerClass()
{
	for (std::vector<ConnectionEntryClass>::iterator it = Connections.begin(); it != Connections.end(); it++) {
		it->Connection->Disconnect();
		delete it->Connection;
	}
	Connections.clear();
	if (Listener != 0) {
		if (Listener->Is_Listening()) {
			Listener->Stop();
		}
		delete Listener;
		Listener = 0;
	}
	Listener = 0;
	delete [] Name;
	Name = 0;
}

unsigned short ConnectionManagerClass::Get_Port()
{
	if (Listener != 0) {
		return(Listener->Get_Port());
	}
	return(0);
}

void ConnectionManagerClass::Set_Name(const char *name)
{
	FAIL_IF(!name) return;
	if (Name != 0) {
		delete [] Name;
		Name = 0;
	}
	Name = nstrdup(name);
}

void ConnectionManagerClass::Set_Listener(ConnectionListenerClass *listener)
{
	if (Listener != 0) {
		if (Listener->Is_Listening()) {
			Listener->Stop();
		}
		delete Listener;
		Listener = 0;
	}
	Listener = listener;
}

































































PacketClass *ConnectionManagerClass::Peek(bool next)
{
	int count = Connections.size();
	for (int i = 0; i < count; i++) {
		int index;
		if (next) index = (RoundRobin + 1 + i) % count;
		else index = (RoundRobin + i) % count;
		ConnectionEntryClass &entry = Connections[index];
		if (!entry.Marked && !entry.Connection->Is_Dead()) {
			PacketClass *packet = entry.Connection->Peek();
			if (packet != 0) {
				RoundRobin = index;
				Current = entry.Connection;
				return(packet);
			}
		}
	}
	return(0);
}



PacketClass *ConnectionManagerClass::Get_Packet(bool next)
{
	PacketClass *retval = Peek(next);
	if (retval == 0) {
		return(0);
	}
	PacketClass *testpacket = Connections[RoundRobin].Connection->Receive();
	G_ASSERT(retval == testpacket);
	return(retval);
}

PacketClass *ConnectionManagerClass::Receive()
{
	if (RoundRobin < (int)Connections.size()) {
		ConnectionEntryClass &entry = Connections[RoundRobin];
		if (entry.Connection == Current) return(entry.Connection->Receive());
	}
	return(0);
}

int ConnectionManagerClass::Broadcast(PacketClass *packet)
{
	int count = 0;
	for (std::vector<ConnectionEntryClass>::iterator it = Connections.begin(); it != Connections.end(); it++) {
		if (it->Connection != 0 && it->Connection->Is_Connected()) {
			it->Connection->Send_Packet(packet);
			count++;
		}
	}
	return(count);
}

bool ConnectionManagerClass::Mark(PacketClass *packet)
{
	int count = Connections.size();
	for (int i = 0; i < count; i++) {
		ConnectionEntryClass &entry = Connections[i];
		if (entry.Connection->Peek() == packet) {
			entry.Marked = true;
			return(true);
		}
	}
	return(false);
}

void ConnectionManagerClass::Clear_Marks()
{
	for (std::vector<ConnectionEntryClass>::iterator it = Connections.begin(); it != Connections.end(); it++) {
		it->Marked = false;
	}
}




























bool ConnectionManagerClass::Set_Unknown05(ConnectionClass *connection, bool on)
{
	for (std::vector<ConnectionEntryClass>::iterator it = Connections.begin(); it != Connections.end(); it++) {
		if (it->Connection == connection) {
			bool old = it->Unknown05;
			it->Unknown05 = on;
			return(old);
		}
	}
	G_ASSERT(false);
	return(false);
}


bool ConnectionManagerClass::Remove_Connection(ConnectionClass *connection)
{
	FAIL_IF(!connection) return(false);
	for (std::vector<ConnectionEntryClass>::iterator it = Connections.begin(); it != Connections.end(); it++) {
		if (it->Connection == connection) {
			if (it - Connections.begin() < RoundRobin) {
				RoundRobin--;
			}
			Connections.erase(it);
			return(true);
		}
	}
	return(false);
}





















bool ConnectionManagerClass::Destroy_Connection(ConnectionClass *connection)
{
	FAIL_IF(!connection) return(false);
	for (std::vector<ConnectionEntryClass>::iterator it = Connections.begin(); it != Connections.end(); it++) {
		if (it->Connection == connection) {
			if (it - Connections.begin() < RoundRobin) {
				RoundRobin--;
			}
			Connections.erase(it);
			connection->Disconnect();
			delete connection;
			return(true);
		}
	}
	return(false);
}

bool ConnectionManagerClass::Destroy_All()
{
	for (std::vector<ConnectionEntryClass>::iterator it = Connections.begin(); it != Connections.end(); it++) {
		it->Connection->Disconnect();
		delete it->Connection;
	}
	Connections.clear();
	return(true);
}





bool ConnectionManagerClass::Is_Managed(ConnectionClass *connection)
{
	for (std::vector<ConnectionEntryClass>::iterator it = Connections.begin(); it != Connections.end(); it++) {
		if (it->Connection == connection) {
			return(true);
		}
	}
	return(false);
}

























































































































































































































StreamConnectionClass::StreamConnectionClass() : Ring(100000)
{
	ReadBuffer = new char[0x1000];
}

// 0x8233E0: exact since pk.h holds the full packet base and <string> (before, its unwind states were one lower; cause not isolated)
StreamConnectionClass::~StreamConnectionClass()
{
	delete ReadBuffer;
}


bool StreamConnectionClass::Write_Message(RawPacketClass *packet)
{
	if (Transport_Write(&packet->Header, 4) != 4) {
		return(false);
	}
	int bytes = packet->Header.Size - 4;
	if (bytes) {
		char *data = packet->Data.Buffer;
		if (Transport_Write(data, bytes) != bytes) {
			return(false);
		}
	}
	return(true);
}


bool StreamConnectionClass::Send_Packet(PacketClass *packet)
{
	RawPacketClass *raw = packet->Build();
	bool ok = Write_Message(raw);
	delete raw;
	return(ok);
}


bool StreamConnectionClass::Send_Message(RawPacketClass *packet)
{
	return(Write_Message(packet));
}


bool StreamConnectionClass::Message_Ready()
{
	Fill();
	PacketHeaderClass header;
	unsigned int got = Ring.Read(&header, sizeof(header));
	if (got < sizeof(header)) {
		Ring.Unread(got);
		return(false);
	}
	int bytes = header.Size - 4;
	if (bytes < 0) {
		Ring.Unread(got);
		return(false);
	}
	if (bytes > Ring.Get_Used()) {
		Ring.Unread(got);
		return(false);
	}
	Ring.Unread(got);
	return(true);
}


PacketClass *StreamConnectionClass::Receive()
{
	if (Ring.Get_Free() > 0x80) {
		Fill();
	}
	int got = Ring.Read(&Header, sizeof(Header));
	if (got != sizeof(Header)) {
		Ring.Unread(got);
		return(0);
	}
	int readbytes = Header.Size - 4;
	if (readbytes < 0) {
		Disconnect();
		return(0);
	}
	if (readbytes > Ring.Get_Used()) {
		Ring.Unread(sizeof(Header));
		return(0);
	}
	char *buffer = new char[Header.Size];
	*(PacketHeaderClass *)buffer = Header;
	int bytes = Ring.Read(buffer + sizeof(Header), readbytes);
	G_ASSERT(readbytes == bytes);
	PacketClass *packet = Unknown_Create_Packet(buffer);
	delete buffer;
	if (packet) {
		packet->Connection = this;
	}
	return(packet);
}

// (spacing kept: the next assert is on the client's line 1138)















void StreamConnectionClass::Fill()
{
	int len = Ring.Get_Free();
	if (len >= 0x1000) {
		len = 0x1000;
	}
	int bytes_from_transport = Transport_Read(ReadBuffer, len);
	if (bytes_from_transport > 0) {
		int bytes_written = Ring.Write(ReadBuffer, bytes_from_transport);
		G_ASSERT(bytes_written == bytes_from_transport);
	}
}


bool StreamConnectionClass::Is_Dead()
{
	return(!Is_Connected() && !Message_Ready());
}


/*
**	Reconstruction notes (connect.cpp, PARTIAL)
**
**	* Rebuilt: the connection base's constructor 0x820F90 and destructor 0x820FE0 (the scalar
**	  deleting destructor 0x820FB0 is the compiler's), and the stream connection 0x823320-0x8238C0:
**	  constructor, destructor (see above), Write_Message 0x823460 (also inlined in Send_Packet
**	  0x8234D0 and Send_Message 0x823570, out of line because it is a member), Message_Ready
**	  0x8235E0, Receive 0x8236D0, Fill 0x823840 (defined after Receive, inlined there: its assert is
**	  on line 1138, after Receive's 1104) and Is_Dead 0x8238C0 (Message_Ready inlined).
**	* Receive's assert names the size it wants `readbytes` and what the ring gave `bytes`
**	  (`cmp ebp, eax`: the wanted size first).
**	* The header Receive reads is a static (0xBE8EA0); Message_Ready reads into a local.
**	* Not rebuilt: everything from 0x821000 to 0x8232E0 (Create_Connection and its helpers, the
**	  forwarding, peek, logging and statistics wrappers, ConnectionManager): docs/kernel-map.md 4.
*/
