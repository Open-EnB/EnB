// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkskill.h (the name provisional: see the .cpp)
**
**	The packet classes of client 0x8A49F0-0x8A4EF0 rebuilt so far (opcode 87, 88); the file's other classes
**	are still declared in pk_classes.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKSKILL_H  // NAME 1 [pkskill]: provisional, pk + its first named class's name
#define PKSKILL_H

#include "pk.h"

// opcode 87 (0x0057, SKILL_UP): vtable 0xB0BA48, constructor 0x8A4A70, 0x18 bytes
class SkillPacketClass : public PacketClass  // NAME 3: the client's Print text 'Skill {' and the PacketClass convention
{
public:
	SkillPacketClass(int game_id, int action, int skill_index);	// 0x8A49F0
	SkillPacketClass(RawPacketClass *raw);	// 0x8A4A70
	virtual RawPacketClass *Build();	// +0x04 0x8A4B30
	virtual bool Process_Packet(void *client);	// +0x08 0x867E30 (clientstub.cpp l.71)
	virtual int Get_Data_Size();	// +0x0C 0x8A4C10
	virtual int Print(char *buffer, int len);	// +0x10 0x8A4C30

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	int Action;	// +0x10  // NAME 3: the client's Print text 'Action='
	int SkillIndex;	// +0x14  // NAME 3: the client's Print text 'SkillIndex='
};

// opcode 88 (0x0058, SKILL_ABILITY): vtable 0xB0BA78, constructor 0x8A4CF0, 0x18 bytes
class SkillPacket_88Class : public PacketClass  // NAME 3: the client's Print text 'Skill {' and the PacketClass convention; opcode added (another opcode's class has the name)
{
public:
	SkillPacket_88Class(int game_id, int action, int ability_index);	// 0x8A4C70
	SkillPacket_88Class(RawPacketClass *raw);	// 0x8A4CF0
	virtual RawPacketClass *Build();	// +0x04 0x8A4DB0
	virtual bool Process_Packet(void *client);	// +0x08 0x867E60 (clientstub.cpp l.74)
	virtual int Get_Data_Size();	// +0x0C 0x8A4E90
	virtual int Print(char *buffer, int len);	// +0x10 0x8A4EB0

	int GameID;	// +0x0C  // NAME 3: the client's Print text 'GameID='
	int Action;	// +0x10  // NAME 3: the client's Print text 'Action='
	int AbilityIndex;	// +0x14  // NAME 3: the client's Print text 'AbilityIndex='
};

#endif
