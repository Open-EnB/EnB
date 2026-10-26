/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkturn.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8AB560-0x8AB7D0 rebuilt so far (opcode 18); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKTURN_H  // NAME 1 [pkturn]: provisional, pk + its first named class's name
#define PKTURN_H

#include "pk.h"

// opcode 18 (0x0012, TURN): vtable 0xB0C048, constructor 0x8AB600, 0x14 bytes
class TurnPacketClass : public PacketClass  // NAME 2: the reference decoder's opcode name TURN (the Net-7 opcode table; no structure check)
{
public:
	TurnPacketClass(int game_id, float intensity);	// 0x8AB560
	TurnPacketClass(RawPacketClass *raw);	// 0x8AB600
	virtual RawPacketClass *Build();	// +0x04 0x8AB6A0
	virtual bool Process_Packet(void *client);	// +0x08 0x867BC0 (clientstub.cpp l.32)
	virtual int Get_Data_Size();	// +0x0C 0x8AB770
	virtual int Print(char *buffer, int len);	// +0x10 0x8AB790
	virtual int Get_Priority() { return(127); }	// +0x1C 0x8AB5B0

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	float Intensity;	// +0x10  // NAME 3: the client's Print text 'Intensity='
};

#endif
