// Earth & Beyond client (reconstruction): app\packets\pkcreatecharacter.cpp. Notes at the end of the file.
#include "pkcreatecharacter.h"
#include <stdio.h>
#include <string.h>

CreateCharacterPacketClass::CreateCharacterPacketClass(RawPacketClass *raw) :
	Unknown2D4(0),
	Unknown2D8(0),
	Unknown2DC(0)
{
	raw->Get(Unknown0C);
	raw->Get(Unknown10);
	raw->Get(Unknown14);
	raw->Get(Name, sizeof(Name));
	raw->Get(Unknown60);
	raw->Get((Unknown_PacketDataClass *)&Desc);
	raw->Get((Unknown_PacketDataClass *)&Ship);
}

// The client calls AvatarDescClass's assignment out of line (0x85CB00, emitted in this object), inlines
// ShipCustDescClass's and stores the three ints after the members' constructors (assigned in the body)
CreateCharacterPacketClass::CreateCharacterPacketClass(int unknown0C, int unknown10, const char *name, __int64 unknown60, const AvatarDescClass *desc, const ShipCustDescClass &ship, bool unknown14) :
	Unknown2D4(0),
	Unknown2D8(0),
	Unknown2DC(0)
{
	Unknown0C = unknown0C;
	Unknown10 = unknown10;
	Unknown14 = unknown14;
	strcpy(Name, name);
	Unknown60 = unknown60;
	if (desc != NULL) {
		Desc = *desc;
	}
	Ship = ship;
}

CreateCharacterPacketClass::~CreateCharacterPacketClass()
{
}

RawPacketClass *CreateCharacterPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(72, Get_Data_Size());
	raw->Add(Unknown0C);
	raw->Add(Unknown10);
	raw->Add(Unknown14);
	raw->Add(Name, sizeof(Name));
	raw->Add(Unknown60);
	raw->Add((Unknown_PacketDataClass &)Desc);
	raw->Add((Unknown_PacketDataClass &)Ship);
	return(raw);
}

int CreateCharacterPacketClass::Get_Data_Size()
{
	return(((Unknown_PacketDataClass &)Ship).Get_Size() + 0x161);
}

int CreateCharacterPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "Create Character"));
}


/*
**	Reconstruction notes (pkcreatecharacter.cpp)
**
**	* Client 0x85C5F0-0x85D170 (0x85C5C0 is the previous object's guarded static, 0xC416FC; this one's is
**	  0x85D170): opcode 72, written by hand (builder P7, 2026-10-27): three ints, a 0x41-byte name, an
**	  __int64, an AvatarDescClass and a ShipCustDescClass. Get_Data_Size counts the description as 0x10C
**	  bytes (0x161 with the rest) and asks only the ship for its size, as the client does.
**	* The file name is provisional (no assert names it).
*/
