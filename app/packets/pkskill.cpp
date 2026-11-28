// Earth & Beyond client (reconstruction): app\packets\pkskill.cpp. Notes at the end of the file.
#include "pkskill.h"
#include <stdio.h>


SkillPacketClass::SkillPacketClass(int game_id, int action, int skill_index) : GameID(game_id), Action(action), SkillIndex(skill_index)
{
}

SkillPacketClass::SkillPacketClass(PacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&Action, sizeof(Action));
	raw->Get(&SkillIndex, sizeof(SkillIndex));
}

PacketClass *SkillPacketClass::Encode()
{
	PacketClass *raw = new PacketClass(87, Get_Data_Size());
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

SkillAbilityPacketClass::SkillAbilityPacketClass(int game_id, int action, int ability_index) : GameID(game_id), Action(action), AbilityIndex(ability_index)
{
}

SkillAbilityPacketClass::SkillAbilityPacketClass(PacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&Action, sizeof(Action));
	raw->Get(&AbilityIndex, sizeof(AbilityIndex));
}

PacketClass *SkillAbilityPacketClass::Encode()
{
	PacketClass *raw = new PacketClass(88, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&Action, sizeof(Action));
	raw->Add(&AbilityIndex, sizeof(AbilityIndex));
	return(raw);
}

int SkillAbilityPacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(Action) + sizeof(AbilityIndex));
}

int SkillAbilityPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "Skill {GameID=%d Action=%d, AbilityIndex=%d}", GameID, Action, AbilityIndex));
}


/*
**	Reconstruction notes (pkskill.cpp)
**
**	* Client 0x8A49F0-0x8A4EF0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
