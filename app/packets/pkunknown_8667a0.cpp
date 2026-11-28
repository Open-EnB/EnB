// Earth & Beyond client (reconstruction): app\packets\pkunknown_8667a0.cpp. Notes at the end of the file.
#include "pkunknown_8667a0.h"
#include <stdio.h>
#include <string.h>
#include "..\shared\auth_product.h"

GlobalPlayerStatusPacketClass::GlobalPlayerStatusPacketClass(int unknown0c, const char *unknown10, __int64 unknown58, int unknown60, int unknown64) :
	Unknown0C(unknown0c), Unknown58(unknown58), Unknown60(unknown60), Unknown64(unknown64)
{
	strncpy(Unknown10, unknown10, sizeof(Unknown10) - 1);
	Unknown10[sizeof(Unknown10) - 1] = 0;
}

GlobalPlayerStatusPacketClass::GlobalPlayerStatusPacketClass(RawPacketClass *raw)
{
	raw->Get(Unknown0C);
	raw->Get(Unknown10, sizeof(Unknown10));
	raw->Get(Unknown58);
	raw->Get(Unknown60);
	raw->Get(Unknown64);
}

RawPacketClass *GlobalPlayerStatusPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(Get_Type(), Get_Data_Size());
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
**	Reconstruction notes (pkunknown_8667a0.cpp)
**
**	* Client 0x8667A0-0x866A90 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, neutral (D10): the object's first address).
*/
