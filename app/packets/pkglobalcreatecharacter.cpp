// Earth & Beyond client (reconstruction): app\packets\pkglobalcreatecharacter.cpp. Notes at the end of the file.
#include "pkglobalcreatecharacter.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// ShipCustDescClass's assignment inlined: the 0x1A name bytes at +0x14 in one loop (shipcust_desc.h's
// written-out operator=, as 0x8A0380 / 0x89FD80)
GlobalCreateCharacterPacketClass::GlobalCreateCharacterPacketClass(int galaxy, int slot, const char *name, const AvatarDescClass *desc, const ShipCustDescClass *ship, unsigned char unknown) :
	Galaxy(galaxy),
	Slot(slot),
	Unknown14(unknown)
{
	strncpy(Name, name, sizeof(Name) - 1);
	Name[sizeof(Name) - 1] = 0;
	if (desc != NULL) {
		Desc = *desc;
	}
	Ship = *ship;
}

GlobalCreateCharacterPacketClass::GlobalCreateCharacterPacketClass(RawPacketClass *raw) :
	Galaxy(0),
	Slot(0),
	Unknown14(0)
{
	raw->Get(Galaxy);
	raw->Get(Slot);
	raw->Get(Unknown14);
	raw->Get(Name, sizeof(Name));
	raw->Get(&Desc);
	raw->Get(&Ship);
	for (unsigned int i = 0; i < strlen(Name); i++) {
		Name[i] = (char)tolower(Name[i]);
	}
}

GlobalCreateCharacterPacketClass::~GlobalCreateCharacterPacketClass()
{
}

RawPacketClass *GlobalCreateCharacterPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(114, Get_Data_Size());
	raw->Add(Galaxy);
	raw->Add(Slot);
	raw->Add(Unknown14);
	raw->Add(Name, sizeof(Name));
	raw->Add(Desc);
	raw->Add(Ship);
	return(raw);
}

// The description counted as 0x10C bytes (its wire size), only the ship asked, as in pkcreatecharacter.cpp
int GlobalCreateCharacterPacketClass::Get_Data_Size()
{
	return(((Unknown_PacketDataClass &)Ship).Get_Size() + 0x159);
}

int GlobalCreateCharacterPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "Global Create Character"));
}


/*
**	Reconstruction notes (pkglobalcreatecharacter.cpp)
**
**	* Client 0x864090-0x864780 (each object ends with <string>'s guarded static 0xC416FC): GLOBAL_CREATE_CHARACTER
**	  114, written by hand (builder P9, 2026-10-27): the galaxy, the slot, a byte, a 0x41-byte name, an
**	  AvatarDescClass and a ShipCustDescClass, as opcode 72 (pkcreatecharacter.cpp). The destructor is declared
**	  (the client's 0x864590 stores the class's own vtable first).
**	* The file name is provisional (no assert names it).
*/
