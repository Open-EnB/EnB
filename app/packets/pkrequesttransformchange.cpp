// Earth & Beyond client (reconstruction): app\packets\pkrequesttransformchange.cpp. Notes at the end of the file.
#include "pkrequesttransformchange.h"
#include <stdio.h>


RequestTransformChangePacketClass::RequestTransformChangePacketClass(RawPacketClass *raw)
{
	raw->Get(&PlayerID, sizeof(PlayerID));
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(Unknown14, sizeof(Unknown14));
	raw->Get(Unknown20, sizeof(Unknown20));
	raw->Get(Unknown30, sizeof(Unknown30));
}

RawPacketClass *RequestTransformChangePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(67, Get_Data_Size());
	raw->Add(&PlayerID, sizeof(PlayerID));
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(Unknown14, sizeof(Unknown14));
	raw->Add(Unknown20, sizeof(Unknown20));
	raw->Add(Unknown30, sizeof(Unknown30));
	return(raw);
}

int RequestTransformChangePacketClass::Get_Data_Size()
{
	return(sizeof(PlayerID) + sizeof(GameID) + sizeof(Unknown14) + sizeof(Unknown20) + sizeof(Unknown30));
}

int RequestTransformChangePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {PlayerID=%d GameID=%d Position={%f %f %f} Orientation={%f %f %f %f} Velocity={%f %f %f}}", "Request_Transform_Change", PlayerID, GameID, Unknown14[0], Unknown14[1], Unknown14[2], Unknown20[0], Unknown20[1], Unknown20[2], Unknown20[3], Unknown30[0], Unknown30[1], Unknown30[2]));
}


/*
**	Reconstruction notes (pkrequesttransformchange.cpp)
**
**	* Client 0x8A27B0-0x8A2B40 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (builder B,
**	  2026-10-25): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
