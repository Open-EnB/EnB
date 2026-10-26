/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkwarpindex.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8B1AA0-0x8B2050 rebuilt so far (opcode 156); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKWARPINDEX_H  // NAME 1 [pkwarpindex]: provisional, pk + its first named class's name
#define PKWARPINDEX_H

#include "pk.h"

// opcode 156 (0x009C, WARP_INDEX): vtable 0xB0C360, constructor 0x8B1EB0, 0x10 bytes
class WarpIndexPacketClass : public PacketClass
{
public:
	WarpIndexPacketClass(int index);	// 0x8B1E20
	WarpIndexPacketClass(RawPacketClass *raw);	// 0x8B1EB0
	virtual RawPacketClass *Build();	// +0x04 0x8B1F40
	virtual bool Process_Packet(void *client);	// +0x08 0x73A7F0 (not rebuilt)
	virtual int Get_Data_Size();	// +0x0C 0x8B2000
	virtual int Print(char *buffer, int len);	// +0x10 0x8B2020

	int Index;	// +0x0C  // NAME 3: the client's Print text 'Index='
};

#endif
