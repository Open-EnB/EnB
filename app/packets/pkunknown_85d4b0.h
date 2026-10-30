/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_85d4b0.h (the name provisional: see the .cpp)
**
**	The packet class of client 0x85D4B0-0x85D780 (opcode 77).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_85D4B0_H  // NAME 1 [pkunknown_85d4b0]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_85D4B0_H

#include "pk.h"

// opcode 77 (0x004D, ?): vtable 0xB08DC8, constructor 0x85D570, 0x60 bytes
class Packet_004DClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x004D (Print 'Registration_Result')
{
public:
	Packet_004DClass(int unknown0c, const char *unknown10, __int64 unknown58);	// 0x85D4B0
	Packet_004DClass(RawPacketClass *raw);	// 0x85D570
	virtual RawPacketClass *Build();	// +0x04 0x85D650
	virtual bool Process_Packet(void *client);	// +0x08 0x8689A0 (clientstub.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x85D730
	virtual int Print(char *buffer, int len);	// +0x10 0x85D750

	int Unknown0C;	// +0x0C (network order)
	char Unknown10[0x41];	// +0x10 (strcpy'd; sent as its 0x41 bytes)
	__int64 Unknown58;	// +0x58 (network order)
};

#endif
