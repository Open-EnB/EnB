// Earth & Beyond client (reconstruction): app\packets\pkoption.cpp. Notes at the end of the file.
#include "pkoption.h"
#include <stdio.h>


OptionPacketClass::OptionPacketClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&OptionType, sizeof(OptionType));
	raw->Get(&OptionVar, sizeof(OptionVar));
}

RawPacketClass *OptionPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(46, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&OptionType, sizeof(OptionType));
	raw->Add(&OptionVar, sizeof(OptionVar));
	return(raw);
}

int OptionPacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(OptionType) + sizeof(OptionVar));
}

int OptionPacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "%s {GameID=%d OptionType=%d OptionVar=%d}", "Option", GameID, OptionType, OptionVar));
}


/*
**	Reconstruction notes (pkoption.cpp)
**
**	* Client 0x89F490-0x89F710 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
