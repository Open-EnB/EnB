/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_860d30.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x860D30-0x861730 rebuilt so far (opcode 30025); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_860D30_H  // NAME 1 [pkunknown_860d30]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_860D30_H

#include "pk.h"

// opcode 30025 (0x7549, ?): vtable 0xB091E8, constructor 0x861260, 0x10 bytes
class Packet_7549Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x7549
{
public:
	Packet_7549Class(int unit_type);	// 0x8611F0
	Packet_7549Class(RawPacketClass *raw);	// 0x861260
	virtual RawPacketClass *Build();	// +0x04 0x861310
	virtual bool Process_Packet(void *client);	// +0x08 0x868310 (clientstub.cpp l.149)
	virtual int Get_Data_Size();	// +0x0C 0x8613D0
	virtual int Print(char *buffer, int len);	// +0x10 0x8613F0

	int UnitType;	// +0x0C  // NAME 3: the client's Print text 'UnitType='
};

#endif
