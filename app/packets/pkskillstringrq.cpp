// Earth & Beyond client (reconstruction): app\packets\pkskillstringrq.cpp. Notes at the end of the file.
#include "pkskillstringrq.h"
#include <stdio.h>


SkillStringRqPacketClass::SkillStringRqPacketClass(int player_id, int request) : PlayerID(player_id), Request(request)
{
}

SkillStringRqPacketClass::SkillStringRqPacketClass(RawPacketClass *raw)
{
	raw->Get(&PlayerID, sizeof(PlayerID));
	raw->Get(&Request, sizeof(Request));
}

RawPacketClass *SkillStringRqPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(81, Get_Data_Size());
	raw->Add(&PlayerID, sizeof(PlayerID));
	raw->Add(&Request, sizeof(Request));
	return(raw);
}

int SkillStringRqPacketClass::Get_Data_Size()
{
	return(sizeof(PlayerID) + sizeof(Request));
}

int SkillStringRqPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {PlayerID=%d Request=%d}", "Request_Skill_Strinsg", PlayerID, Request));
}


/*
**	Reconstruction notes (pkskillstringrq.cpp)
**
**	* Client 0x8A2530-0x8A2780 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (builder B,
**	  2026-10-25): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
