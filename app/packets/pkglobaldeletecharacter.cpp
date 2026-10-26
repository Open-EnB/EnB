// Earth & Beyond client (reconstruction): app\packets\pkglobaldeletecharacter.cpp. Notes at the end of the file.
#include "pkglobaldeletecharacter.h"
#include <stdio.h>


GlobalDeleteCharacterPacketClass::GlobalDeleteCharacterPacketClass(RawPacketClass *raw)
{
	raw->Get(Unknown0C);
}

RawPacketClass *GlobalDeleteCharacterPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(113, Get_Data_Size());
	raw->Add(Unknown0C);
	return(raw);
}

int GlobalDeleteCharacterPacketClass::Get_Data_Size()
{
	return(sizeof(Unknown0C));
}

int GlobalDeleteCharacterPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "Global Delete Character"));
}


/*
**	Reconstruction notes (pkglobaldeletecharacter.cpp)
**
**	* Client 0x8647B0-0x8649E0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
