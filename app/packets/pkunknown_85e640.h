/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_85e640.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x85E640-0x85F220 rebuilt so far (opcode 30006); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_85E640_H  // NAME 1 [pkunknown_85e640]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_85E640_H

#include "pk.h"

// opcode 30006 (0x7536, ?): vtable 0xB08EE8, constructor 0x85EA90, 0x10 bytes
class Packet_7536Class : public PacketClass  // NAME 1: neutral (D10): opcode 0x7536
{
public:
	Packet_7536Class(int game_id);	// 0x85EA00
	Packet_7536Class(RawPacketClass *raw);	// 0x85EA90
	virtual RawPacketClass *Build();	// +0x04 0x85EB20
	virtual bool Process_Packet(void *client);	// +0x08 0x868130 (clientstub.cpp l.119)
	virtual int Get_Data_Size();	// +0x0C 0x85EBE0
	virtual int Print(char *buffer, int len);	// +0x10 0x85EC00

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
};

#endif
