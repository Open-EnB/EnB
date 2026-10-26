// Earth & Beyond client (reconstruction): app\packets\pkturn.cpp. Notes at the end of the file.
#include "pkturn.h"
#include <stdio.h>


TurnPacketClass::TurnPacketClass(int game_id, float intensity) : GameID(game_id), Intensity(intensity)
{
}

TurnPacketClass::TurnPacketClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&Intensity, sizeof(Intensity));
}

RawPacketClass *TurnPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(18, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&Intensity, sizeof(Intensity));
	return(raw);
}

int TurnPacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(Intensity));
}

int TurnPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d Intensity=%f}", "Turn", GameID, Intensity));
}


/*
**	Reconstruction notes (pkturn.cpp)
**
**	* Client 0x8AB560-0x8AB7D0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (builder B,
**	  2026-10-25): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
