// Earth & Beyond client (reconstruction): app\packets\pkequipuse.cpp. Notes at the end of the file.
#include "pkequipuse.h"
#include <stdio.h>


EquipUsePacketClass::EquipUsePacketClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&Inventory, sizeof(Inventory));
	raw->Get(&Slot, sizeof(Slot));
}

RawPacketClass *EquipUsePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(93, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&Inventory, sizeof(Inventory));
	raw->Add(&Slot, sizeof(Slot));
	return(raw);
}

int EquipUsePacketClass::Get_Data_Size()
{
	return(sizeof(GameID) + sizeof(Inventory) + sizeof(Slot));
}

int EquipUsePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {GameID=%d Inventory=%d Slot=%d}", "Use inventory item", GameID, Inventory, Slot));
}


/*
**	Reconstruction notes (pkequipuse.cpp)
**
**	* Client 0x8AE810-0x8AEAB0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (builder B,
**	  2026-10-25): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
