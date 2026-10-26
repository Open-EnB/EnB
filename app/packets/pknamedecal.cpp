// Earth & Beyond client (reconstruction): app\packets\pknamedecal.cpp. Notes at the end of the file.
#include "pknamedecal.h"
#include <stdio.h>


NameDecalPacketClass::NameDecalPacketClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(Name, sizeof(Name));
	raw->Get(RGB, sizeof(RGB));
}

RawPacketClass *NameDecalPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(178, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(Name, sizeof(Name));
	raw->Add(RGB, sizeof(RGB));
	return(raw);
}

int NameDecalPacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(Name) + sizeof(RGB));
}

int NameDecalPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d Name=%s RGB=%f %f %f}", "NameDecal", GameID, Name, RGB[0], RGB[1], RGB[2]));
}


/*
**	Reconstruction notes (pknamedecal.cpp)
**
**	* Client 0x8805C0-0x8812D0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (builder B,
**	  2026-10-25): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
**	* Not rebuilt yet: opcode 16 (declared in pk_classes.h).
*/
