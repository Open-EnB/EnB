// Earth & Beyond client (reconstruction): app\packets\pkmanufactureitemcatagorypacket.cpp. Notes at the end of the file.
#include "pkmanufactureitemcatagorypacket.h"
#include <stdio.h>

ManufactureItemCatagoryPacketClass::ManufactureItemCatagoryPacketClass(int player, int mode) : player(player), Mode(mode)
{
}

ManufactureItemCatagoryPacketClass::ManufactureItemCatagoryPacketClass(RawPacketClass *raw) : player(0), Mode(0)
{
	raw->Get(player);
	raw->Get(Mode);
}

RawPacketClass *ManufactureItemCatagoryPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(121, Get_Data_Size());
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
	return(sprintf(buffer, "Manufacture Item Category request {player=%d} Mode = %s", player, Manufacturing_Mode_Name(mode, sizeof(mode) - 1, Mode)));
}


ManufactureItemCatagoryPacket_122Class::ManufactureItemCatagoryPacket_122Class(int player, int category) : player(player), Category(category)
{
}

ManufactureItemCatagoryPacket_122Class::ManufactureItemCatagoryPacket_122Class(RawPacketClass *raw) : player(0), Category(-1)
{
	raw->Get(player);
	raw->Get(Category);
}

RawPacketClass *ManufactureItemCatagoryPacket_122Class::Build()
{
	RawPacketClass *raw = new RawPacketClass(122, Get_Data_Size());
	raw->Add(player);
	raw->Add(Category);
	return(raw);
}

int ManufactureItemCatagoryPacket_122Class::Get_Data_Size()
{
	return(sizeof(player) + sizeof(Category));
}

int ManufactureItemCatagoryPacket_122Class::Print(char *buffer, int)
{
	return(sprintf(buffer, "Manufacture Item Category request {player=%d} Category = %d", player, Category));
}

ManufactureItemIdPacketClass::ManufactureItemIdPacketClass(int player, int item_id) : player(player), ItemID(item_id)
{
}

ManufactureItemIdPacketClass::ManufactureItemIdPacketClass(RawPacketClass *raw) : player(0), ItemID(-1)
{
	raw->Get(player);
	raw->Get(ItemID);
}

RawPacketClass *ManufactureItemIdPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(123, Get_Data_Size());
	raw->Add(player);
	raw->Add(ItemID);
	return(raw);
}

int ManufactureItemIdPacketClass::Get_Data_Size()
{
	return(sizeof(player) + sizeof(ItemID));
}

int ManufactureItemIdPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "Manufacture Set ItemID {player=%d} ItemID = %d", player, ItemID));
}

RefineryItemIdPacketClass::RefineryItemIdPacketClass(int player, int item_id) : player(player), ItemID(item_id)
{
}

RefineryItemIdPacketClass::RefineryItemIdPacketClass(RawPacketClass *raw) : player(0), ItemID(-1)
{
	raw->Get(&player, sizeof(player));
	raw->Get(&ItemID, sizeof(ItemID));
}

RawPacketClass *RefineryItemIdPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(124, Get_Data_Size());
	raw->Add(&player, sizeof(player));
	raw->Add(&ItemID, sizeof(ItemID));
	return(raw);
}

int RefineryItemIdPacketClass::Get_Data_Size()
{
	return(sizeof(player) + sizeof(ItemID));
}

int RefineryItemIdPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "Refinery Set ItemID {player=%d} ItemID = %d", player, ItemID));
}

Packet_007DClass::Packet_007DClass(int player, int cargo_slot_id, int vault_slot_id) : player(player), CargoSlotID(cargo_slot_id), VaultSlotID(vault_slot_id)
{
}

Packet_007DClass::Packet_007DClass(RawPacketClass *raw) : player(0), CargoSlotID(-1), VaultSlotID(-1)
{
	raw->Get(player);
	raw->Get(CargoSlotID);
	raw->Get(VaultSlotID);
}

RawPacketClass *Packet_007DClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(125, Get_Data_Size());
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

ManufactureActionPacketClass::ManufactureActionPacketClass(RawPacketClass *raw) : player(0), ActionID(0)
{
	raw->Get(player);
	raw->Get(ActionID);
}

RawPacketClass *ManufactureActionPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(126, Get_Data_Size());
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

ManufactureSetPacketClass::ManufactureSetPacketClass(RawPacketClass *raw)
{
	ManufactureID = 0;
	raw->Get(ManufactureID);
}

RawPacketClass *ManufactureSetPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(127, Get_Data_Size());
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


/*
**	Reconstruction notes (pkmanufactureitemcatagorypacket.cpp)
**
**	* Client 0x89B340-0x89C610 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
**	* Not rebuilt yet: opcode 121, 128 (declared in pk_classes.h).
*/
