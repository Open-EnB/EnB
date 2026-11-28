// Earth & Beyond client (reconstruction): app\packets\gameedit\pkgeadd.cpp. Notes at the end of the file.
#include "pkgeadd.h"
#include <stdio.h>

GameEditorAddPacketClass::GameEditorAddPacketClass(int game_id, int type, float radius) :
	GameID(game_id),
	Type(type),
	Radius(radius)
{
}

GameEditorAddPacketClass::GameEditorAddPacketClass(PacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&Type, sizeof(Type));
	raw->Get(&Radius, sizeof(Radius));
}

PacketClass *GameEditorAddPacketClass::Encode()
{
	PacketClass *raw = new PacketClass(30003, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&Type, sizeof(Type));
	raw->Add(&Radius, sizeof(Radius));
	return(raw);
}

int GameEditorAddPacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(Type) + sizeof(Radius));
}

int GameEditorAddPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d Type=%d Radius=%f}", "GameEditorAdd", GameID, Type, Radius));
}

bool GameEditorAddPacketClass::Process_Packet(void *)
{
	return(true);
}


/*
**	Reconstruction notes (pkgeadd.cpp)
**
**	* Client 0x85D7B0-0x85DA70 (0x85D780 is the previous object's guarded static, 0xC416FC; this one's is
**	  0x85DA70): the send-only game-editor packet 30003 (not in the packet switch: its Process_Packet
**	  returns true here), written by hand (builder P5, 2026-10-27).
**	* The file name is by convention from its class GameEditorAddPacketClass (thunk order) and its place in retail's object order (before pkgeaset), NAME 3; no assert names it.
*/
