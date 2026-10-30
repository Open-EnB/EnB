// Earth & Beyond client (reconstruction): app\packets\pkunknown_85d7b0.cpp. Notes at the end of the file.
#include "pkunknown_85d7b0.h"
#include <stdio.h>

Packet_7533Class::Packet_7533Class(int game_id, int type, float radius) :
	GameID(game_id),
	Type(type),
	Radius(radius)
{
}

Packet_7533Class::Packet_7533Class(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&Type, sizeof(Type));
	raw->Get(&Radius, sizeof(Radius));
}

RawPacketClass *Packet_7533Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(30003, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&Type, sizeof(Type));
	raw->Add(&Radius, sizeof(Radius));
	return(raw);
}

int Packet_7533Class::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(Type) + sizeof(Radius));
}

int Packet_7533Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d Type=%d Radius=%f}", "GameEditorAdd", GameID, Type, Radius));
}

bool Packet_7533Class::Process_Packet(void *)
{
	return(true);
}


/*
**	Reconstruction notes (pkunknown_85d7b0.cpp)
**
**	* Client 0x85D7B0-0x85DA70 (0x85D780 is the previous object's guarded static, 0xC416FC; this one's is
**	  0x85DA70): the send-only game-editor packet 30003 (not in the packet switch: its Process_Packet
**	  returns true here), written by hand (builder P5, 2026-10-27).
**	* The file name is provisional (no assert names it).
*/
