/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_85d1a0.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x85D1A0-0x85D480 rebuilt so far (opcode 73); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_85D1A0_H  // NAME 1 [pkunknown_85d1a0]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_85D1A0_H

#include "pk.h"

// opcode 73 (0x0049, ?): vtable 0xB08D98, constructor 0x85D270, 0x68 bytes
class Packet_0049Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x0049
{
public:
	Packet_0049Class(RawPacketClass *raw);	// 0x85D270
	virtual RawPacketClass *Build();	// +0x04 0x85D350
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
