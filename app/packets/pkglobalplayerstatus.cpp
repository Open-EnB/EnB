// Earth & Beyond client (reconstruction): app\packets\pkglobalplayerstatus.cpp. Notes at the end of the file.
#include "pkglobalplayerstatus.h"
#include <stdio.h>
#include <string.h>
#include "..\shared\auth_product.h"

GlobalPlayerStatusPacketClass::GlobalPlayerStatusPacketClass(int unknown0c, const char *unknown10, __int64 unknown58, int unknown60, int unknown64) :
	Unknown0C(unknown0c), Unknown58(unknown58), Unknown60(unknown60), Unknown64(unknown64)
{
	strncpy(Unknown10, unknown10, sizeof(Unknown10) - 1);
	Unknown10[sizeof(Unknown10) - 1] = 0;
}

GlobalPlayerStatusPacketClass::GlobalPlayerStatusPacketClass(PacketClass *raw)
{
	raw->Get(Unknown0C);
	raw->Get(Unknown10, sizeof(Unknown10));
	raw->Get(Unknown58);
	raw->Get(Unknown60);
	raw->Get(Unknown64);
}

PacketClass *GlobalPlayerStatusPacketClass::Build()
{
	PacketClass *raw = new PacketClass(Get_Type(), Get_Data_Size());
	raw->Add(Unknown0C);
	raw->Add(Unknown10, sizeof(Unknown10));
	raw->Add(Unknown58);
	raw->Add(Unknown60);
	raw->Add(Unknown64);
	return(raw);
}

int GlobalPlayerStatusPacketClass::Get_Type()
{
	return(116);
}

int GlobalPlayerStatusPacketClass::Get_Data_Size()
{
	return(sizeof(Unknown0C) + sizeof(Unknown10) + sizeof(Unknown58) + sizeof(Unknown60) + sizeof(Unknown64));
}

int GlobalPlayerStatusPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "GlobalPlayerStatus"));
}


/*
**	Reconstruction notes (pkglobalplayerstatus.cpp)
**
**	* Client 0x8667A0-0x866A90 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is by convention from its class GlobalPlayerStatusPacketClass (thunk order) and its place in retail's object order (after pkgloballogin), NAME 3; no assert names it.
*/
