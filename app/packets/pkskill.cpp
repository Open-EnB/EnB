// Earth & Beyond client (reconstruction): app\packets\pkskill.cpp. Notes at the end of the file.
#include "pkskill.h"
#include <stdio.h>


SkillPacketClass::SkillPacketClass(int game_id, int action, int skill_index) : GameID(game_id), Action(action), SkillIndex(skill_index)
{
}

SkillPacketClass::SkillPacketClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&Action, sizeof(Action));
	raw->Get(&SkillIndex, sizeof(SkillIndex));
}

RawPacketClass *SkillPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(87, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&Action, sizeof(Action));
	raw->Add(&SkillIndex, sizeof(SkillIndex));
	return(raw);
}

int SkillPacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(Action) + sizeof(SkillIndex));
}

int SkillPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "Skill {GameID=%d Action=%d, SkillIndex=%d}", GameID, Action, SkillIndex));
}

SkillPacket_88Class::SkillPacket_88Class(int game_id, int action, int ability_index) : GameID(game_id), Action(action), AbilityIndex(ability_index)
{
}

SkillPacket_88Class::SkillPacket_88Class(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&Action, sizeof(Action));
	raw->Get(&AbilityIndex, sizeof(AbilityIndex));
}

RawPacketClass *SkillPacket_88Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(88, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&Action, sizeof(Action));
	raw->Add(&AbilityIndex, sizeof(AbilityIndex));
	return(raw);
}

int SkillPacket_88Class::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(Action) + sizeof(AbilityIndex));
}

int SkillPacket_88Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "Skill {GameID=%d Action=%d, AbilityIndex=%d}", GameID, Action, AbilityIndex));
}


/*
**	Reconstruction notes (pkskill.cpp)
**
**	* Client 0x8A49F0-0x8A4EF0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (builder B,
**	  2026-10-25): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
