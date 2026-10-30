// Earth & Beyond client (reconstruction): app\packets\pkdebug.cpp (the client's __FILE__).
// DEBUG 26: a game id, a debug command (an index into DebugLabels) and its argument.
// Notes at the end of the file.
#include "pkdebug.h"
#include "..\shared\unknown_debug.h"
#include <stdio.h>

// The debug commands' names (Print's Command)
static char *DebugLabels[] = {	// 0xBB3A74  // NAME 2: what it holds
	"Create_Ship",
	"Create_Planet",
	"Create_Nebula",
	"Create_Stargate",
	"Create_Drones",
	"Create_Star_Base",
	"Create_Planet_Cargo_Transport",
	"Create_Planet_Personel_Transport",
	"Create_Planet_Rover",
	"Create_Planet_Ore",
	"Refuel_Ship",
	"Inc_Pos_Updates",
	"Dec_Pos_Updates",
	"Next_Hull",
	"Prev_Hull",
	"Create_Component",
	"Toggle_Autolevel",
	"Inc_Max_Pitch",
	"Dec_Max_Pitch",
	"Inc_Zband_Width",
	"Dec_Zband_Width",
	"Next_Sector",
	"Toggle_Rotate_Interp",
	"Fill_Inventory",
	"Kill_Server",
	"Toggle_Reporters",
	"Test_Script",
	"Script_Interactive",
	"Create_Test_Component",
	"Give_money_100000",
	"Toggle_invulnerability,Buff_skills",
	"Set_debug_mode",
	"Apply_damage",
	"Snoop",
	"Set_super_client"
};

static const int label_count = sizeof(DebugLabels) / sizeof(DebugLabels[0]);	// NAME 5: the client's assert 'DebugIdx < label_count'

DebugPacketClass::DebugPacketClass(int game_id, int debug_idx, int user_data) :
	GameID(game_id),
	DebugIdx(debug_idx),
	UserData(user_data)
{
}

DebugPacketClass::DebugPacketClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&DebugIdx, sizeof(DebugIdx));
	raw->Get(&UserData, sizeof(UserData));
}

RawPacketClass *DebugPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(26, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&DebugIdx, sizeof(DebugIdx));
	raw->Add(&UserData, sizeof(UserData));
	return(raw);
}

int DebugPacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(DebugIdx) + sizeof(UserData));
}





int DebugPacketClass::Print(char *buffer, int)
{
	char *command = "unknown";
	G_ASSERT(DebugIdx >= 0 && DebugIdx < label_count);
	if (DebugIdx >= 0 && DebugIdx < label_count) {
		command = DebugLabels[DebugIdx];
	}
	return(sprintf(buffer, "%s {GameID=%d Command=%s UserData=%d}", "Debug", GameID, command, UserData));
}


/*
**	Reconstruction notes (pkdebug.cpp)
**
**	* Client 0x8802C0-0x880590 (0x880290 is the previous object's guarded static, 0xC416FC; this one's
**	  is 0x880590). DEBUG 26 written by hand (builder P4, 2026-10-26); Print's assert is on the client's
**	  line 84. The destructor the scalar deleting one calls (0x7280E0) is a copy from an earlier object.
**	  The label 'Toggle_invulnerability,Buff_skills' is one string in the client.
*/
