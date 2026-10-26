/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkrequesttransformchange.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8A27B0-0x8A2B40 rebuilt so far (opcode 67); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKREQUESTTRANSFORMCHANGE_H  // NAME 1 [pkrequesttransformchange]: provisional, pk + its first named class's name
#define PKREQUESTTRANSFORMCHANGE_H

#include "pk.h"

// opcode 67 (0x0043, REQUEST_TRANSFORM_CHANGE): vtable 0xB0B808, constructor 0x8A28C0, 0x3C bytes
class RequestTransformChangePacketClass : public PacketClass  // NAME 2: the reference decoder's opcode name REQUEST_TRANSFORM_CHANGE (the Net-7 opcode table; no structure check)
{
public:
	RequestTransformChangePacketClass(RawPacketClass *raw);	// 0x8A28C0
	virtual RawPacketClass *Build();	// +0x04 0x8A2990
	virtual bool Process_Packet(void *client);	// +0x08 0x867EC0 (clientstub.cpp l.80)
	virtual int Get_Data_Size();	// +0x0C 0x8A2A80
	virtual int Print(char *buffer, int len);	// +0x10 0x8A2AA0

	int PlayerID;	// +0x0C  // NAME 3: the client's Print text 'PlayerID='
	int GameID;	// +0x10  // NAME 3: the client's Print text 'GameID='
	float Unknown14[3];	// +0x14
	float Unknown20[4];	// +0x20
	float Unknown30[3];	// +0x30
};

#endif
