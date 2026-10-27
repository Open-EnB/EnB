// Earth & Beyond client (reconstruction): app\packets\pkunknown_861b30.cpp. Notes at the end of the file.
#include "pkunknown_861b30.h"
#include <stdio.h>

Packet_7534Class::Packet_7534Class(int game_id) : GameID(game_id)
{
}

Packet_7534Class::Packet_7534Class(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
}

RawPacketClass *Packet_7534Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(30004, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	return(raw);
}

int Packet_7534Class::Get_Data_Size()
{
	return(sizeof(GameID));
}

int Packet_7534Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d}", "GameEditorRemove", GameID));
}

bool Packet_7534Class::Process_Packet(void *)
{
	return(true);
}


/*
**	Reconstruction notes (pkunknown_861b30.cpp)
**
**	* Client 0x861B30-0x861D80 (0x861B00 is the previous object's guarded static, 0xC416FC; this one's is
**	  0x861D80): the send-only game-editor packet 30004 (not in the packet switch: its Process_Packet
**	  returns true here), written by hand (builder P5, 2026-10-27).
**	* The file name is provisional (no assert names it).
*/
