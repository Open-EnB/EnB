// Earth & Beyond client (reconstruction): app\packets\pkmanufactureitemcatagorypacket.cpp. Notes at the end of the file.
#include "pkmanufactureitemcatagorypacket.h"
#include <stdio.h>

ManufactureItemCatagoryPacketClass::ManufactureItemCatagoryPacketClass(int player, int mode) : player(player), Mode(mode)
{
}

ManufactureItemCatagoryPacketClass::ManufactureItemCatagoryPacketClass(PacketClass *raw) : player(0), Mode(0)
{
	raw->Get(player);
	raw->Get(Mode);
}

PacketClass *ManufactureItemCatagoryPacketClass::Build()
{
	PacketClass *raw = new PacketClass(121, Get_Data_Size());
	raw->Add(player);
	raw->Add(Mode);
	return(raw);
}

int ManufactureItemCatagoryPacketClass::Get_Data_Size()
{
	return(sizeof(player) + sizeof(Mode));
}

int ManufactureItemCatagoryPacketClass::Print(char *buffer, int)
{
	char mode[128];
	return(sprintf(buffer, "Manufacture Item Category request {player=%d} Mode = %s", player, Value_To_String(mode, sizeof(mode) - 1, (UnknownB013ECEnum)Mode)));
}


ManufactureSetItemCategoryPacketClass::ManufactureSetItemCategoryPacketClass(int player, int category) : player(player), Category(category)
{
}

ManufactureSetItemCategoryPacketClass::ManufactureSetItemCategoryPacketClass(PacketClass *raw) : player(0), Category(-1)
{
	raw->Get(player);
	raw->Get(Category);
}

PacketClass *ManufactureSetItemCategoryPacketClass::Build()
{
	PacketClass *raw = new PacketClass(122, Get_Data_Size());
	raw->Add(player);
	raw->Add(Category);
	return(raw);
}

int ManufactureSetItemCategoryPacketClass::Get_Data_Size()
{
	return(sizeof(player) + sizeof(Category));
}

int ManufactureSetItemCategoryPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "Manufacture Item Category request {player=%d} Category = %d", player, Category));
}

ManufactureSetItemIDPacketClass::ManufactureSetItemIDPacketClass(int player, int item_id) : player(player), ItemID(item_id)
{
}

ManufactureSetItemIDPacketClass::ManufactureSetItemIDPacketClass(PacketClass *raw) : player(0), ItemID(-1)
{
	raw->Get(player);
	raw->Get(ItemID);
}

PacketClass *ManufactureSetItemIDPacketClass::Build()
{
	PacketClass *raw = new PacketClass(123, Get_Data_Size());
	raw->Add(player);
	raw->Add(ItemID);
	return(raw);
}

int ManufactureSetItemIDPacketClass::Get_Data_Size()
{
	return(sizeof(player) + sizeof(ItemID));
}

int ManufactureSetItemIDPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "Manufacture Set ItemID {player=%d} ItemID = %d", player, ItemID));
}

RefinerySetItemIDPacketClass::RefinerySetItemIDPacketClass(int player, int item_id) : player(player), ItemID(item_id)
{
}

RefinerySetItemIDPacketClass::RefinerySetItemIDPacketClass(PacketClass *raw) : player(0), ItemID(-1)
{
	raw->Get(&player, sizeof(player));
	raw->Get(&ItemID, sizeof(ItemID));
}

PacketClass *RefinerySetItemIDPacketClass::Build()
{
	PacketClass *raw = new PacketClass(124, Get_Data_Size());
	raw->Add(&player, sizeof(player));
	raw->Add(&ItemID, sizeof(ItemID));
	return(raw);
}

int RefinerySetItemIDPacketClass::Get_Data_Size()
{
	return(sizeof(player) + sizeof(ItemID));
}

int RefinerySetItemIDPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "Refinery Set ItemID {player=%d} ItemID = %d", player, ItemID));
}

Packet_007DClass::Packet_007DClass(int player, int cargo_slot_id, int vault_slot_id) : player(player), CargoSlotID(cargo_slot_id), VaultSlotID(vault_slot_id)
{
}

Packet_007DClass::Packet_007DClass(PacketClass *raw) : player(0), CargoSlotID(-1), VaultSlotID(-1)
{
	raw->Get(player);
	raw->Get(CargoSlotID);
	raw->Get(VaultSlotID);
}

PacketClass *Packet_007DClass::Build()
{
	PacketClass *raw = new PacketClass(125, Get_Data_Size());
	raw->Add(player);
	raw->Add(CargoSlotID);
	raw->Add(VaultSlotID);
	return(raw);
}

int Packet_007DClass::Get_Data_Size()
{
	return(sizeof(player) + sizeof(CargoSlotID) + sizeof(VaultSlotID));
}

int Packet_007DClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "Manufacture Set SlotID {player=%d} CargoSlotID = %d VaultSlotID = %d", player, CargoSlotID, VaultSlotID));
}

ManufactureActionPacketClass::ManufactureActionPacketClass(int player, int action_id) : player(player), ActionID(action_id)
{
}

ManufactureActionPacketClass::ManufactureActionPacketClass(PacketClass *raw) : player(0), ActionID(0)
{
	raw->Get(player);
	raw->Get(ActionID);
}

PacketClass *ManufactureActionPacketClass::Build()
{
	PacketClass *raw = new PacketClass(126, Get_Data_Size());
	raw->Add(player);
	raw->Add(ActionID);
	return(raw);
}

int ManufactureActionPacketClass::Get_Data_Size()
{
	return(sizeof(player) + sizeof(ActionID));
}

int ManufactureActionPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "Manufacture Action {player=%d} ActionID = %d", player, ActionID));
}

ManufactureSetPacketClass::ManufactureSetPacketClass(int manufacture_id) : ManufactureID(manufacture_id)
{
}

ManufactureSetPacketClass::ManufactureSetPacketClass(PacketClass *raw)
{
	ManufactureID = 0;
	raw->Get(ManufactureID);
}

PacketClass *ManufactureSetPacketClass::Build()
{
	PacketClass *raw = new PacketClass(127, Get_Data_Size());
	raw->Add(ManufactureID);
	return(raw);
}

int ManufactureSetPacketClass::Get_Data_Size()
{
	return(sizeof(ManufactureID));
}

int ManufactureSetPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "Manufacture Set ManufactureID = %d", ManufactureID));
}

ManufactureSetTechLevelFilterPacketClass::ManufactureSetTechLevelFilterPacketClass(int player, bool mode, unsigned int bit_field) :
	Player(player),
	Mode(mode),
	BitField(bit_field)
{
}

ManufactureSetTechLevelFilterPacketClass::ManufactureSetTechLevelFilterPacketClass(PacketClass *raw) :
	Player(0),
	Mode(true),
	BitField(0)
{
	raw->Get(Player);
	raw->Get(Mode);
	raw->Get((int &)BitField);
}

PacketClass *ManufactureSetTechLevelFilterPacketClass::Build()
{
	PacketClass *raw = new PacketClass(128, Get_Data_Size());
	raw->Add(Player);
	raw->Add(Mode);
	raw->Add((int)BitField);
	return(raw);
}

int ManufactureSetTechLevelFilterPacketClass::Get_Data_Size()
{
	return(sizeof(Player) + sizeof(Mode) + sizeof(BitField));
}

int ManufactureSetTechLevelFilterPacketClass::Print(char *buffer, int)
{
	char text[128];
	return(sprintf(buffer, "Manufacture Tech Level Filter request {player=%d} Mode = %s BitField = %u", Player, Value_To_String(text, sizeof(text) - 1, Mode), BitField));
}


/*
**	Reconstruction notes (pkmanufactureitemcatagorypacket.cpp)
**
**	* Client 0x89B340-0x89C610 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
**	* Opcodes 121 and 128 written by hand (builder P4, 2026-10-27).
*/
