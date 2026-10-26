/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkoption.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x89F490-0x89F710 rebuilt so far (opcode 46); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKOPTION_H  // NAME 1 [pkoption]: provisional, pk + its first named class's name
#define PKOPTION_H

#include "pk.h"

// opcode 46 (0x002E, OPTION): vtable 0xB0B564, constructor 0x89F510, 0x18 bytes
class OptionPacketClass : public PacketClass  // NAME 2: the reference decoder's opcode name OPTION (the Net-7 opcode table; no structure check)
{
public:
	OptionPacketClass(RawPacketClass *raw);	// 0x89F510
	virtual RawPacketClass *Build();	// +0x04 0x89F5D0
	virtual bool Process_Packet(void *client);	// +0x08 0x868AC0 (clientstub.cpp l.272)
	virtual int Get_Data_Size();	// +0x0C 0x89F6B0
	virtual int Print(char *buffer, int len);	// +0x10 0x89F6D0

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	int OptionType;	// +0x10  // NAME 3: the client's Print text 'OptionType='
	bool OptionVar;	// +0x14  // NAME 3: the client's Print text 'OptionVar='
	char Unknown15[0x3];	// +0x15
};

#endif
