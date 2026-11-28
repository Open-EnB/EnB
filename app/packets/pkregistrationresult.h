// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkregistrationresult.h (the name by convention: see the .cpp)
**
**	The packet class of client 0x85D4B0-0x85D780 (opcode 77).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKREGISTRATIONRESULT_H  // NAME 3 [pkregistrationresult.h]: by convention from its class RegistrationResultPacketClass (thunk order) and its place in retail's object order (after pkdeletecharacter, before gameedit)
#define PKREGISTRATIONRESULT_H

#include "pk.h"

// opcode 77 (0x004D, ?): vtable 0xB08DC8, constructor 0x85D570, 0x60 bytes
class RegistrationResultPacketClass : public UnknownPacketBaseClass  // NAME 5: thunk order A, binary.md 5.11
{
public:
	RegistrationResultPacketClass(int unknown0c, const char *unknown10, __int64 unknown58);	// 0x85D4B0
	RegistrationResultPacketClass(PacketClass *raw);	// 0x85D570
	virtual PacketClass *Build();	// +0x04 0x85D650
	virtual bool Process_Packet(void *client);	// +0x08 0x8689A0 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x85D730
	virtual int Print(char *buffer, int len);	// +0x10 0x85D750

	int Unknown0C;	// +0x0C (network order)
	char Unknown10[0x41];	// +0x10 (strcpy'd; sent as its 0x41 bytes)
	__int64 Unknown58;	// +0x58 (network order)
};

#endif
