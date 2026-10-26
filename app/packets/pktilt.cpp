// Earth & Beyond client (reconstruction): app\packets\pktilt.cpp. Notes at the end of the file.
#include "pktilt.h"
#include <stdio.h>


TiltPacketClass::TiltPacketClass(int game_id, float intensity) : GameID(game_id), Intensity(intensity)
{
}

TiltPacketClass::TiltPacketClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&Intensity, sizeof(Intensity));
}

RawPacketClass *TiltPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(19, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&Intensity, sizeof(Intensity));
	return(raw);
}

int TiltPacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(Intensity));
}

int TiltPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d Intensity=%f}", "Tilt", GameID, Intensity));
}


/*
**	Reconstruction notes (pktilt.cpp)
**
**	* Client 0x8AAAB0-0x8AAD20 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (builder B,
**	  2026-10-25): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
