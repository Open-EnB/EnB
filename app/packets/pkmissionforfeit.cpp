// Earth & Beyond client (reconstruction): app\packets\pkmissionforfeit.cpp. Notes at the end of the file.
#include "pkmissionforfeit.h"
#include <stdio.h>


MissionForfeitPacketClass::MissionForfeitPacketClass(int playerid, int id) : playerid(playerid), ID(id)
{
}

MissionForfeitPacketClass::MissionForfeitPacketClass(RawPacketClass *raw) : playerid(0), ID(-1)
{
	raw->Get(playerid);
	raw->Get(ID);
}

RawPacketClass *MissionForfeitPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(134, Get_Data_Size());
	raw->Add(playerid);
	raw->Add(ID);
	return(raw);
}

int MissionForfeitPacketClass::Get_Data_Size()
{
	return(sizeof(playerid) + sizeof(ID));
}

int MissionForfeitPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "Mission Forfeit request {playerid=%d} Internal Mission ID = %d", playerid, ID));
}

MissionDismissalPacketClass::MissionDismissalPacketClass(int playerid, int id) : playerid(playerid), ID(id)
{
}

MissionDismissalPacketClass::MissionDismissalPacketClass(RawPacketClass *raw) : playerid(0), ID(-1)
{
	raw->Get(playerid);
	raw->Get(ID);
}

RawPacketClass *MissionDismissalPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(135, Get_Data_Size());
	raw->Add(playerid);
	raw->Add(ID);
	return(raw);
}

int MissionDismissalPacketClass::Get_Data_Size()
{
	return(sizeof(playerid) + sizeof(ID));
}

int MissionDismissalPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "Mission Dismiss request {playerid=%d} Internal Mission ID = %d", playerid, ID));
}


/*
**	Reconstruction notes (pkmissionforfeit.cpp)
**
**	* Client 0x89C640-0x89CAC0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
