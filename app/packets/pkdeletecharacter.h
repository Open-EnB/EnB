// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkdeletecharacter.h (the name by convention: see the .cpp)
**
**	The packet classes of client 0x85D1A0-0x85D480 rebuilt so far (opcode 73); the file's other classes
**	are still declared in pk_classes.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKDELETECHARACTER_H  // NAME 3 [pkdeletecharacter.h]: by convention from its class DeleteCharacterPacketClass (thunk order) and its place in retail's object order (after pkcreatecharacter)
#define PKDELETECHARACTER_H

#include "pk.h"

// opcode 73 (0x0049, ?): vtable 0xB08D98, constructor 0x85D270, 0x68 bytes
class DeleteCharacterPacketClass : public UnknownPacketBaseClass  // NAME 5: thunk order A, binary.md 5.11
{
public:
	DeleteCharacterPacketClass(__int64 unknown10, int unknown18, const char *unknown1c, __int64 unknown60);	// 0x85D1A0 (Unknown1C strcpy'd)
	DeleteCharacterPacketClass(PacketClass *raw);	// 0x85D270
	virtual PacketClass *Encode();	// +0x04 0x85D350  // NAME 5: thunk order confirmed, binary.md 5.11
	virtual bool Process_Packet(void *client);	// +0x08 0x8680A0 (clientstub.cpp l.110)
	virtual int Get_Data_Size();	// +0x0C 0x85D430
	virtual int Print(char *buffer, int len);	// +0x10 0x85D450

	char Unknown0C[0x4];	// +0x0C
	__int64 Unknown10;	// +0x10
	int Unknown18;	// +0x18
	char Unknown1C[65];	// +0x1C
	char Unknown5D[0x3];	// +0x5D
	__int64 Unknown60;	// +0x60
};

#endif
