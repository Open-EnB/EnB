/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkcreatecharacter.h (the name provisional: see the .cpp)
**
**	The packet class of client 0x85C5F0-0x85D170 (opcode 72).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKCREATECHARACTER_H  // NAME 2 [pkcreatecharacter]: provisional, pk + its class's Print text 'Create Character'
#define PKCREATECHARACTER_H

#include "pk.h"
#include "..\shared\avatar_desc.h"
#include "..\shared\shipcust_desc.h"

// opcode 72 (0x0048, ?): vtable 0xB08D64, constructor 0x85C5F0, 0x2E0 bytes
class Packet_0048Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x0048 (its Print text is 'Create Character': P-P7-2)
{
public:
	Packet_0048Class(RawPacketClass *raw);	// 0x85C5F0
	virtual ~Packet_0048Class();	// +0x00 0x85C7D0 / 0x85CF80
	Packet_0048Class(int unknown0C, int unknown10, const char *name, __int64 unknown60, const AvatarDescClass *desc, const ShipCustDescClass &ship, bool unknown14);	// 0x85C800
	virtual RawPacketClass *Build();	// +0x04 0x85D000
	virtual bool Process_Packet(void *client);	// +0x08 0x868040 (clientstub.cpp l.104)
	virtual int Get_Data_Size();	// +0x0C 0x85D110
	virtual int Print(char *buffer, int len);	// +0x10 0x85D140
	virtual bool Unknown_28(void *a);	// +0x28 0x868070 (clientstub.cpp l.107)  // NAME 1: neutral (D10)

	int Unknown0C;	// +0x0C  // NAME 1: neutral (D10)
	int Unknown10;	// +0x10  // NAME 1: neutral (D10)
	int Unknown14;	// +0x14 (the value constructor's bool)  // NAME 1: neutral (D10)
	char Name[0x41];	// +0x18 (0x41 bytes on the wire)  // NAME 1: what the value constructor copies into it (a string)
	__int64 Unknown60;	// +0x60  // NAME 1: neutral (D10)
	AvatarDescClass Desc;	// +0x68  // NAME 2: what it holds
	ShipCustDescClass Ship;	// +0x174  // NAME 2: what it holds
	int Unknown2D4;	// +0x2D4 (0; not in the message)  // NAME 1: neutral (D10)
	int Unknown2D8;	// +0x2D8 (0)  // NAME 1: neutral (D10)
	int Unknown2DC;	// +0x2DC (0)  // NAME 1: neutral (D10)
};

#endif
