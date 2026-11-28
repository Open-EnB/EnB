// Earth & Beyond client (reconstruction): app\packets\gameedit\pkgerem.cpp. Notes at the end of the file.
#include "pkgerem.h"
#include <stdio.h>

GameEditorRemovePacketClass::GameEditorRemovePacketClass(int game_id) : GameID(game_id)
{
}

GameEditorRemovePacketClass::GameEditorRemovePacketClass(PacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
}

PacketClass *GameEditorRemovePacketClass::Build()
{
	PacketClass *raw = new PacketClass(30004, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	return(raw);
}

int GameEditorRemovePacketClass::Get_Data_Size()
{
	return(sizeof(GameID));
}

int GameEditorRemovePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d}", "GameEditorRemove", GameID));
}

bool GameEditorRemovePacketClass::Process_Packet(void *)
{
	return(true);
}


/*
**	Reconstruction notes (pkgerem.cpp)
**
**	* Client 0x861B30-0x861D80 (0x861B00 is the previous object's guarded static, 0xC416FC; this one's is
**	  0x861D80): the send-only game-editor packet 30004 (not in the packet switch: its Process_Packet
**	  returns true here), written by hand (builder P5, 2026-10-27).
**	* The file name is by convention from its class GameEditorRemovePacketClass (thunk order) and its place in retail's object order (between pkgeplan and pkgesave), NAME 3; no assert names it.
*/
