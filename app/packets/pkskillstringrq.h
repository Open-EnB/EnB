/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkskillstringrq.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8A2530-0x8A2780 rebuilt so far (opcode 81); the file's other classes
**	are still declared in pk_classes.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKSKILLSTRINGRQ_H  // NAME 1 [pkskillstringrq]: provisional, pk + its first named class's name
#define PKSKILLSTRINGRQ_H

#include "pk.h"

// opcode 81 (0x0051, SKILL_STRING_RQ): vtable 0xB0B7D8, constructor 0x8A25B0, 0x14 bytes
class SkillStringRqPacketClass : public PacketClass  // NAME 2: the reference decoder's opcode name SKILL_STRING_RQ (the Net-7 opcode table; no structure check)
{
public:
	SkillStringRqPacketClass(int player_id, int request);	// 0x8A2530
	SkillStringRqPacketClass(RawPacketClass *raw);	// 0x8A25B0
	virtual RawPacketClass *Build();	// +0x04 0x8A2650
	virtual bool Process_Packet(void *client);	// +0x08 0x867FE0 (clientstub.cpp l.98)
	virtual int Get_Data_Size();	// +0x0C 0x8A2720
	virtual int Print(char *buffer, int len);	// +0x10 0x8A2740

	int PlayerID;	// +0x0C  // NAME 3: the client's Print text 'PlayerID='
	int Request;	// +0x10  // NAME 3: the client's Print text 'Request='
};

#endif
