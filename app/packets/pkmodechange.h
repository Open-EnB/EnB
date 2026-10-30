/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkmodechange.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x89CAF0-0x89CD80 (opcode 76).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKMODECHANGE_H  // NAME 1 [pkmodechange]: provisional, pk + its class's name
#define PKMODECHANGE_H

#include "pk.h"

// opcode 76 (0x004C, ?): vtable 0xB0B2E4, constructor 0x89CBB0, 0x14 bytes
class ModeChangePacketClass : public PacketClass
{
public:
	ModeChangePacketClass(int unknown0c, int mode);	// 0x89CAF0
	ModeChangePacketClass(RawPacketClass *raw);	// 0x89CBB0
	virtual RawPacketClass *Build();	// +0x04 0x89CC50
	virtual bool Process_Packet(void *client);	// +0x08 0x738100 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C PKMODECHANGE_HDS
	virtual int Print(char *buffer, int len);	// +0x10 0x89CD40

	int Unknown0C;	// +0x0C (4 bytes as they are)
	int Mode;	// +0x10 (4 bytes as they are: an index into 0xBB56B0's names)  // NAME 3: the client's Print text 'Mode='
};

#endif
