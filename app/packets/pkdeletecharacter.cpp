// Earth & Beyond client (reconstruction): app\packets\pkdeletecharacter.cpp. Notes at the end of the file.
#include "pkdeletecharacter.h"
#include <stdio.h>
#include <string.h>
#include "..\shared\auth_product.h"
DeleteCharacterPacketClass::DeleteCharacterPacketClass(__int64 unknown10, int unknown18, const char *unknown1c, __int64 unknown60) :
	Unknown10(unknown10),
	Unknown18(unknown18),
	Unknown60(unknown60)
{
	strcpy(Unknown1C, unknown1c);
}

DeleteCharacterPacketClass::DeleteCharacterPacketClass(PacketClass *raw)
{
	raw->Get(Unknown10);
	raw->Get(Unknown18);
	raw->Get(Unknown1C, sizeof(Unknown1C));
	raw->Get(Unknown60);
}

PacketClass *DeleteCharacterPacketClass::Build()
{
	PacketClass *raw = new PacketClass(73, Get_Data_Size());
	raw->Add(Unknown10);
	raw->Add(Unknown18);
	raw->Add(Unknown1C, sizeof(Unknown1C));
	raw->Add(Unknown60);
	return(raw);
}

int DeleteCharacterPacketClass::Get_Data_Size()
{
	return(sizeof(Unknown10) + sizeof(Unknown18) + sizeof(Unknown1C) + sizeof(Unknown60));
}

int DeleteCharacterPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "Delete Character"));
}


/*
**	Reconstruction notes (pkdeletecharacter.cpp)
**
**	* Client 0x85D1A0-0x85D480 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is by convention from its class DeleteCharacterPacketClass (thunk order) and its place in retail's object order (after pkcreatecharacter), NAME 3; no assert names it.
*/
