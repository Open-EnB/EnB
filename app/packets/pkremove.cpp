// Earth & Beyond client (reconstruction): app\packets\pkremove.cpp. Notes at the end of the file.
#include "pkremove.h"
#include <stdio.h>


RemovePacketClass::RemovePacketClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
}

RawPacketClass *RemovePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(7, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	return(raw);
}

int RemovePacketClass::Get_Data_Size()
{
	return(sizeof(GameID));
}

int RemovePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d}", "Remove", GameID));
}


/*
**	Reconstruction notes (pkremove.cpp)
**
**	* Client 0x8A1660-0x8A1920 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (builder B,
**	  2026-10-25): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
