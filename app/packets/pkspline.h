/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkspline.h (the name provisional: see the .cpp)
**
**	The packet class of client 0x8A57A0-0x8A5A80 (opcode 56).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKSPLINE_H  // NAME 1 [pkspline]: provisional, pk + its class's name
#define PKSPLINE_H

#include "pk.h"

class SplineEntryClass;

// opcode 56 (0x0038, ?): vtable 0xB0BB3C, constructor 0x8A5870, 0x14 bytes
class SplinePacketClass : public PacketClass  // NAME 3: the client's Print text 'Spline {'
{
public:
	SplinePacketClass(int count, SplineEntryClass **entries);	// 0x8A57A0
	SplinePacketClass(RawPacketClass *raw);	// 0x8A5870
	virtual ~SplinePacketClass();	// +0x00 0x8A5840 / 0x8A5930
	virtual RawPacketClass *Build();	// +0x04 0x8A5960
	virtual bool Process_Packet(void *client);	// +0x08 0x737C10 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8A5A30
	virtual int Print(char *buffer, int len);	// +0x10 0x8A5A50

	int Blocksize;	// +0x0C (4 bytes as they are)  // NAME 3: the client's Print text 'Blocksize='
	char *Block;	// +0x10 (new'd; SplineDatabaseClass::Pack's entries)  // NAME 2: what it holds
};

#endif
