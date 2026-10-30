/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkmove.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x89D040-0x89D2E0 (opcode 20).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKMOVE_H  // NAME 1 [pkmove]: provisional, pk + its class's name
#define PKMOVE_H

#include "pk.h"

// opcode 20 (0x0014, MOVE): vtable 0xB0B344, constructor 0x89D110, 0x14 bytes
class MovePacketClass : public PacketClass  // NAME 2: the reference decoder's opcode name MOVE (the Net-7 opcode table; no structure check)
{
public:
	MovePacketClass(int game_id, char move_type);	// 0x89D070
	MovePacketClass(RawPacketClass *raw);	// 0x89D110
	virtual RawPacketClass *Build();	// +0x04 0x89D1B0
	virtual bool Process_Packet(void *client);	// +0x08 0x867C20 (clientstub.cpp l.38)
	virtual int Get_Data_Size();	// +0x0C 0x89D280
	virtual int Print(char *buffer, int len);	// +0x10 0x89D2A0
	virtual int Get_Priority() { return(127); }	// +0x1C 0x89D0C0

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	char MoveType;	// +0x10 (MoveTypeNames' index)  // NAME 3: the client's Print text 'MoveType='
};

#endif
