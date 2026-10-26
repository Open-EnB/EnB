/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkmissionforfeit.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x89C640-0x89CAC0 rebuilt so far (opcode 134, 135); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKMISSIONFORFEIT_H  // NAME 1 [pkmissionforfeit]: provisional, pk + its first named class's name
#define PKMISSIONFORFEIT_H

#include "pk.h"

// opcode 134 (0x0086, MISSION_FORFEIT): vtable 0xB0B284, constructor 0x89C6C0, 0x14 bytes
class MissionForfeitPacketClass : public PacketClass  // NAME 2: the reference decoder's opcode name MISSION_FORFEIT (the Net-7 opcode table; no structure check)
{
public:
	MissionForfeitPacketClass(int playerid, int id);	// 0x89C640
	MissionForfeitPacketClass(RawPacketClass *raw);	// 0x89C6C0
	virtual RawPacketClass *Build();	// +0x04 0x89C760
	virtual bool Process_Packet(void *client);	// +0x08 0x8687C0 (clientstub.cpp l.224)
	virtual int Get_Data_Size();	// +0x0C 0x89C830
	virtual int Print(char *buffer, int len);	// +0x10 0x89C850

	int playerid;	// +0x0C  // NAME 3: the client's Print text 'playerid='
	int ID;	// +0x10  // NAME 3: the client's Print text 'ID='
};

// opcode 135 (0x0087, MISSION_DISMISSAL): vtable 0xB0B2B4, constructor 0x89C900, 0x14 bytes
class MissionDismissalPacketClass : public PacketClass  // NAME 2: the reference decoder's opcode name MISSION_DISMISSAL (the Net-7 opcode table; no structure check)
{
public:
	MissionDismissalPacketClass(int playerid, int id);	// 0x89C880
	MissionDismissalPacketClass(RawPacketClass *raw);	// 0x89C900
	virtual RawPacketClass *Build();	// +0x04 0x89C9A0
	virtual bool Process_Packet(void *client);	// +0x08 0x8687F0 (clientstub.cpp l.227)
	virtual int Get_Data_Size();	// +0x0C 0x89CA70
	virtual int Print(char *buffer, int len);	// +0x10 0x89CA90

	int playerid;	// +0x0C  // NAME 3: the client's Print text 'playerid='
	int ID;	// +0x10  // NAME 3: the client's Print text 'ID='
};

#endif
