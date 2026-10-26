/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_87acd0.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x87ACD0-0x87DE90 rebuilt so far (opcode 170); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_87ACD0_H  // NAME 1 [pkunknown_87acd0]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_87ACD0_H

#include "pk.h"

// opcode 170 (0x00AA, ?): vtable 0xB09F90, constructor 0x87C230, 0x14 bytes
class Packet_00AAClass : public PacketClass  // NAME 1: neutral (D10): opcode 0x00AA
{
public:
	Packet_00AAClass(int unknown0_c, int unknown10);	// 0x87C190
	Packet_00AAClass(RawPacketClass *raw);	// 0x87C230
	virtual RawPacketClass *Build();	// +0x04 0x87C2D0
	virtual bool Process_Packet(void *client);	// +0x08 0x868D30 (clientstub.cpp l.311)
	virtual int Get_Data_Size();	// +0x0C 0x87C3A0
	virtual int Print(char *buffer, int len);	// +0x10 0x87C3C0

	int Unknown0C;	// +0x0C
	int Unknown10;	// +0x10
};

#endif
