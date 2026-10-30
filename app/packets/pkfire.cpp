// Earth & Beyond client (reconstruction): app\packets\pkfire.cpp. Notes at the end of the file.
#include "pkfire.h"
#include <stdio.h>

FirePacketClass::FirePacketClass(int game_id, char slot) :
	GameID(game_id),
	Slot(slot)
{
}

FirePacketClass::FirePacketClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&Slot, sizeof(Slot));
}

RawPacketClass *FirePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(Get_Type(), Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&Slot, sizeof(Slot));
	return(raw);
}

int FirePacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(Slot));
}

int FirePacketClass::Print(char *buffer, int)
{
	const char *name = Get_Name();
	return(sprintf(buffer, "%s {GameID=%d Slot=%d}", name, GameID, Slot));
}


/*
**	Reconstruction notes (pkfire.cpp)
**
**	* The client's object 0x887290-0x887560 (its Print 0x887500 ends with the guarded static 0xC416FC):
**	  opcode 21 written by hand (builder P10, 2026-10-27). No assert names the file: pk + the class's
**	  Get_Name 'Fire', provisional (D10).
**	* Build takes the opcode from the virtual Get_Type, so FireEquipped 22 (Get_Name 'FireEquipped', 0x1C
**	  bytes) shares the raw constructor, Build, Get_Data_Size and Print; its inline members are compiled
**	  in pk.cpp's object (0x86E5D0-0x86E630), where the packet switch constructs it.
**	* The scalar deleting destructor 0x887340 calls PacketClass's destructor copy 0x7242C0 (the linker's
**	  pick of the COMDAT): the implicit destructor.
*/
