// Earth & Beyond client (reconstruction): app\packets\pkmove.cpp. Notes at the end of the file.
#include "pkmove.h"
#include <stdio.h>

static char *MoveTypeNames[] = {	// 0xBB56F4 (Print's MoveType; the client's own missing comma joins two names)  // NAME 2: what it holds
	"Move_Forward",
	"Move_Backward",
	"Move_Run",
	"Stop",
	"Debug_Move_Forward",
	"Debug_Move_Backward",
	"Debug_Stop"
	"Jump_Forward",
	"Jump_Backward",
	"Jump_To",
	"Autopilot",
	"Autofollow toggle",
	"Autolevel toggle",
	0
};

MovePacketClass::MovePacketClass(int game_id, char move_type) :
	GameID(game_id),
	MoveType(move_type)
{
}

MovePacketClass::MovePacketClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&MoveType, sizeof(MoveType));
}

RawPacketClass *MovePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(20, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&MoveType, sizeof(MoveType));
	return(raw);
}

int MovePacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(MoveType));
}

int MovePacketClass::Print(char *buffer, int)
{
	char *type = MoveTypeNames[MoveType];
	return(sprintf(buffer, "%s {GameID=%d MoveType=%s}", "Move", GameID, type));
}


/*
**	Reconstruction notes (pkmove.cpp)
**
**	* Client 0x89D040-0x89D2E0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Written by hand (builder P3, 2026-10-26), 5 of 5 exact in appfile.py
**	  match. MoveTypeNames is the client's table 0xBB56F4: its 'Debug_StopJump_Forward' is two names
**	  joined by a missing comma (kept). Print reads the name into a local before the sprintf. The
**	  destructor the scalar deleting one calls (0x581610) is a copy from an earlier object (COMDAT).
**	* The file name is provisional (no assert names it).
*/
