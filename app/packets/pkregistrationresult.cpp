// Earth & Beyond client (reconstruction): app\packets\pkregistrationresult.cpp. Notes at the end of the file.
#include "pkregistrationresult.h"
#include <stdio.h>
#include <string.h>
#include "..\shared\auth_product.h"
RegistrationResultPacketClass::RegistrationResultPacketClass(int unknown0c, const char *unknown10, __int64 unknown58) :
	Unknown0C(unknown0c),
	Unknown58(unknown58)
{
	strcpy(Unknown10, unknown10);
}

RegistrationResultPacketClass::RegistrationResultPacketClass(PacketClass *raw)
{
	int value;
	raw->Get(value);
	raw->Get(Unknown10, sizeof(Unknown10));
	raw->Get(Unknown58);
	Unknown0C = value;
}

PacketClass *RegistrationResultPacketClass::Build()
{
	PacketClass *raw = new PacketClass(77, Get_Data_Size());
	int value = Unknown0C;
	raw->Add(value);
	raw->Add(Unknown10, sizeof(Unknown10));
	raw->Add(Unknown58);
	return(raw);
}

int RegistrationResultPacketClass::Get_Data_Size()
{
	return(sizeof(Unknown0C) + sizeof(Unknown10) + sizeof(Unknown58));
}

int RegistrationResultPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "Registration Result"));
}


/*
**	Reconstruction notes (pkregistrationresult.cpp)
**
**	* Client 0x85D4B0-0x85D780 (0x85D480 is the previous object's guarded static, 0xC416FC; this one's is
**	  0x85D780): opcode 77, written by hand (builder P5, 2026-10-27), the value constructor first. The raw
**	  constructor reads the first int into a local (the raw pointer's dead slot) and stores it last.
**	* The file name is by convention from its class RegistrationResultPacketClass (thunk order) and its place in retail's object order (after pkdeletecharacter, before gameedit), NAME 3; no assert names it.
*/
