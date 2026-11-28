// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkmoderequest.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x89CDB0-0x89D040 (opcode 75).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKMODEREQUEST_H  // NAME 1 [pkmoderequest]: provisional, pk + its Print text 'Mode Request'
#define PKMODEREQUEST_H

#include "pk.h"

// opcode 75 (0x004B, ?): vtable 0xB0B314, constructor 0x89CE70, 0x14 bytes
class ModeRequestPacketClass : public UnknownPacketBaseClass  // NAME 5: thunk order A, binary.md 5.11
{
public:
	ModeRequestPacketClass(int unknown0c, int mode);	// 0x89CDB0
	ModeRequestPacketClass(PacketClass *raw);	// 0x89CE70
	virtual PacketClass *Encode();	// +0x04 0x89CF10  // NAME 5: thunk order confirmed, binary.md 5.11
	virtual bool Process_Packet(void *client);	// +0x08 0x738100 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C PKMODEREQUEST_HDS
	virtual int Print(char *buffer, int len);	// +0x10 0x89D000
	virtual int Get_Priority() { return(0x7F); }	// +0x1C 0x89CE00

	int Unknown0C;	// +0x0C (4 bytes as they are)
	int Mode;	// +0x10 (4 bytes as they are: an index into 0xBB56E8's names)  // NAME 3: the client's Print text 'Mode='
};

#endif
