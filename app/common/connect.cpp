// Earth & Beyond client (reconstruction): app\common\connect.cpp (PARTIAL). Notes at the end of the file.
#include "connect.h"
#include "..\packets\pk.h"
#include "..\shared\unknown_debug.h"
#include <string.h>


ConnectionClass::ConnectionClass()
{
	Unknown04 = true;
}


ConnectionClass::~ConnectionClass()
{
}

// The header Receive reads (0xBE8EA0).
static PacketHeaderClass Header;

// (not rebuilt here: the factories, ConnectionManager and the wrappers, client 0x821000-0x8232E0; spacing kept: the next assert is on the client's line 1104)


































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































StreamConnectionClass::StreamConnectionClass() : Ring(100000)
{
	ReadBuffer = new char[0x1000];
}

// NOT EXACT: 0x8233E0 numbers its unwind states one lower (the ring's destructor runs in state 0); the code is the client's
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
**	  constructor, destructor (NOT EXACT, above), Write_Message 0x823460 (also inlined in Send_Packet
**	  0x8234D0 and Send_Message 0x823570, out of line because it is a member), Message_Ready
**	  0x8235E0, Receive 0x8236D0, Fill 0x823840 (defined after Receive, inlined there: its assert is
**	  on line 1138, after Receive's 1104) and Is_Dead 0x8238C0 (Message_Ready inlined).
**	* Receive's assert names the size it wants `readbytes` and what the ring gave `bytes`
**	  (`cmp ebp, eax`: the wanted size first).
**	* The header Receive reads is a static (0xBE8EA0); Message_Ready reads into a local.
**	* Not rebuilt: everything from 0x821000 to 0x8232E0 (Create_Connection and its helpers, the
**	  forwarding, peek, logging and statistics wrappers, ConnectionManager): docs/kernel-map.md 4.
*/
