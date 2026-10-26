/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_8a5510.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8A5510-0x8A5770 rebuilt so far (opcode 232); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_8A5510_H  // NAME 1 [pkunknown_8a5510]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_8A5510_H

#include "pk.h"

// opcode 232 (0x00E8, ?): vtable 0xB0BB08, constructor 0x8A55A0, 0x14 bytes
class Packet_00E8Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x00E8
{
public:
	Packet_00E8Class(int unknown0_c);	// 0x8A5510
	Packet_00E8Class(RawPacketClass *raw);	// 0x8A55A0
	virtual RawPacketClass *Build();	// +0x04 0x8A5630
	virtual bool Process_Packet(void *client);	// +0x08 0x869600 (clientstub.cpp l.452)
	virtual int Get_Data_Size();	// +0x0C 0x8A5720
	virtual int Print(char *buffer, int len);	// +0x10 0x8A5740
	virtual int Get_Type();	// +0x28 0x8A5700  // NAME 2: what it returns (the opcode Build sends), as MessageLinePacketClass's

	int Unknown0C;	// +0x0C
	int Unknown10;	// +0x10
};

#endif
