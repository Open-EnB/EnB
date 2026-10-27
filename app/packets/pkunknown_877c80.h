/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_877c80.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x877C50-0x878000 (opcode 99).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_877C80_H  // NAME 1 [pkunknown_877c80]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_877C80_H

#include "pk.h"

// opcode 99 (0x0063, ?): vtable 0xB09D7C, constructor 0x877D70, 0x18 bytes
class Packet_0063Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x0063 (its Print says 'Broadcast_Packet')
{
public:
	Packet_0063Class(char *text, int unknown14);	// 0x877C80
	Packet_0063Class(RawPacketClass *raw);	// 0x877D70
	virtual ~Packet_0063Class();	// +0x00 0x877D40 / 0x877E40
	virtual RawPacketClass *Build();	// +0x04 0x877E80
	virtual bool Process_Packet(void *client);	// +0x08 0x868610 (clientstub.cpp l.197)
	virtual int Get_Data_Size();	// +0x0C 0x877F60
	virtual int Print(char *buffer, int len);	// +0x10 0x877F80
	virtual int Get_Priority() { return(0); }	// +0x1C 0x877D20

	char *String;	// +0x0C (Size bytes, the text and its 0)  // NAME 3: the client's Print text 'String='
	short Size;	// +0x10  // NAME 3: the client's Print text 'Size='
	int Unknown14;	// +0x14 (in network order: Get(int &) / Add(const int &); not printed)
};

#endif
