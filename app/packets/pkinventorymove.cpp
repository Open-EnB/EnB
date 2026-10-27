// Earth & Beyond client (reconstruction): app\packets\pkinventorymove.cpp. Notes at the end of the file.
#include "pkinventorymove.h"
#include <stdio.h>

InventoryMovePacketClass::InventoryMovePacketClass(int id, InventoryEnum from_inv, int from_slot, InventoryEnum to_inv, int to_slot, int num) :
	ID(id),
	FromInv(from_inv),
	FromSlot(from_slot),
	ToInv(to_inv),
	ToSlot(to_slot),
	Num(num)
{
}

InventoryMovePacketClass::InventoryMovePacketClass(RawPacketClass *raw)
{
	raw->Get(ID);
	int value;
	raw->Get(value);
	FromInv = (InventoryEnum)value;
	raw->Get(FromSlot);
	raw->Get(value);
	ToInv = (InventoryEnum)value;
	raw->Get(ToSlot);
	raw->Get(Num);
}

RawPacketClass *InventoryMovePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(39, Get_Data_Size());
	raw->Add(ID);
	int value = FromInv;
	raw->Add(value);
	raw->Add(FromSlot);
	value = ToInv;
	raw->Add(value);
	raw->Add(ToSlot);
	raw->Add(Num);
	return(raw);
}

int InventoryMovePacketClass::Get_Data_Size()
{
	return(sizeof(ID) + sizeof(int) + sizeof(FromSlot) + sizeof(int) + sizeof(ToSlot) + sizeof(Num));
}

int InventoryMovePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {ID=%d FromInv=%d FromSlot=%d ToInv=%d ToSlot=%d Num=%d}", "Inventory", ID, FromInv, FromSlot, ToInv, ToSlot, Num));
}


/*
**	Reconstruction notes (pkinventorymove.cpp)
**
**	* Client 0x894CA0-0x8952D0 (0x894C70 is the previous object's guarded static, 0xC416FC; this one's
**	  is 0x8952D0). INVENTORY_MOVE 39 written by hand (builder P4, 2026-10-26); every field is a
**	  network-order int (RawPacketClass::Get(int &) / Add(const int &)); the two inventories are read
**	  into an int and stored (an enum). The destructor the scalar deleting one calls (0x690F80) is a
**	  copy from an earlier object (COMDAT).
**	* Not rebuilt yet: opcode 40 (declared in pk_classes.h).
**	* The file name is provisional (no assert names it).
*/
