// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pknote.h (the name provisional: see the .cpp)
**
**	The packet class of client 0x89EF20-0x89F1C0 (opcode 108).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKNOTE_H  // NAME 1 [pknote]: provisional, pk + its Print text 'Note:'
#define PKNOTE_H

#include "pk.h"

// opcode 108 (0x006C, ?): vtable 0xB0B504, constructor 0x89EFC0, 0x28 bytes
class Packet_006CClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x006C
{
public:
	Packet_006CClass(__int64 unknown10, __int64 unknown18);	// 0x89EF20
	Packet_006CClass(RawPacketClass *raw);	// 0x89EFC0
	virtual RawPacketClass *Build();	// +0x04 0x89F0A0
	virtual bool Process_Packet(void *client);	// +0x08 0x868970 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x89F170
	virtual int Print(char *buffer, int len);	// +0x10 0x89F190

	__int64 Unknown10;	// +0x10
	__int64 Unknown18;	// +0x18
	int Unknown20;	// +0x20 (zeroed; not sent)
	int Unknown24;	// +0x24 (zeroed; not sent)
};

#endif
