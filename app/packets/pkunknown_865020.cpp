// Earth & Beyond client (reconstruction): app\packets\pkunknown_865020.cpp. Notes at the end of the file.
#include "pkunknown_865020.h"
#include "..\shared\unknown_4a1590.h"
#include <stdio.h>
#include "..\shared\auth_product.h"

GlobalHelloPacketClass::GlobalHelloPacketClass(int unknown0c, int unknown10, int unknown14, const char *unknown18, int unknown60, int unknown5c, bool unknown64) :
	Unknown0C(unknown0c), Unknown10(unknown10), Unknown14(unknown14), Unknown5C(unknown5c), Unknown60(unknown60), Unknown64(0)
{
	Bounded_Strcpy(Unknown18, sizeof(Unknown18) - 1, unknown18);
	if (unknown64) {
		Unknown64 = 1;
	}
}

GlobalHelloPacketClass::GlobalHelloPacketClass(PacketClass *raw)
{
	raw->Get(Unknown0C);
	raw->Get(Unknown10);
	raw->Get(Unknown14);
	raw->Get(Unknown18, sizeof(Unknown18));
	raw->Get(Unknown5C);
	raw->Get(Unknown60);
	raw->Get(Unknown64);
}

PacketClass *GlobalHelloPacketClass::Build()
{
	PacketClass *raw = new PacketClass(Get_Type(), Get_Data_Size());
	raw->Add(Unknown0C);
	raw->Add(Unknown10);
	raw->Add(Unknown14);
	raw->Add(Unknown18, sizeof(Unknown18));
	raw->Add(Unknown5C);
	raw->Add(Unknown60);
	raw->Add(Unknown64);
	return(raw);
}

int GlobalHelloPacketClass::Get_Type()
{
	return(115);
}

int GlobalHelloPacketClass::Get_Data_Size()
{
	return(sizeof(Unknown0C) + sizeof(Unknown10) + sizeof(Unknown14) + sizeof(Unknown18) + sizeof(Unknown5C) + sizeof(Unknown60) + sizeof(Unknown64));
}

int GlobalHelloPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "Global Hello"));
}


/*
**	Reconstruction notes (pkunknown_865020.cpp)
**
**	* Client 0x865020-0x8653C0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, neutral (D10): the object's first address).
*/
