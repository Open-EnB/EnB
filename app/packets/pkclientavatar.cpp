// Earth & Beyond client (reconstruction): app\packets\pkclientavatar.cpp. Notes at the end of the file.
#include "pkclientavatar.h"
#include <stdio.h>


ClientAvatarPacketClass::ClientAvatarPacketClass(RawPacketClass *raw)
{
	raw->Get(&AvatarID, sizeof(AvatarID));
}

RawPacketClass *ClientAvatarPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(55, Get_Data_Size());
	raw->Add(&AvatarID, sizeof(AvatarID));
	return(raw);
}

int ClientAvatarPacketClass::Get_Data_Size()
{
	return(sizeof(AvatarID));
}

int ClientAvatarPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {AvatarID=%d}", "Client_Avatar", AvatarID));
}


/*
**	Reconstruction notes (pkclientavatar.cpp)
**
**	* Client 0x877370-0x877610 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
