// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_8667a0.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8667A0-0x866A90 rebuilt so far (opcode 116); the file's other classes
**	are still declared in pk_classes.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_8667A0_H  // NAME 1 [pkunknown_8667a0]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_8667A0_H

#include "pk.h"

// opcode 116 (0x0074, ?): vtable 0xB095F4, constructor 0x866870, 0x68 bytes
class Packet_0074Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x0074
{
public:
	Packet_0074Class(RawPacketClass *raw);	// 0x866870
	virtual RawPacketClass *Build();	// +0x04 0x866930
	virtual bool Process_Packet(void *client);	// +0x08 0x868910 (clientstub.cpp l.245)
	virtual int Get_Data_Size();	// +0x0C 0x866A40
	virtual int Print(char *buffer, int len);	// +0x10 0x866A60
	virtual int Get_Type();	// +0x28 0x866A20  // NAME 2: what it returns (the opcode Build sends), as MessageLinePacketClass's

	int Unknown0C;	// +0x0C
	char Unknown10[65];	// +0x10
	char Unknown51[0x7];	// +0x51
	__int64 Unknown58;	// +0x58
	int Unknown60;	// +0x60
	int Unknown64;	// +0x64
};

#endif
