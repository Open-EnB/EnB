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


InventorySortPacketClass::InventorySortPacketClass(int id, InventoryEnum target_inv, int sort1, int sort2, int sort3, bool reverse) :
	ID(id),
	TargetInv(target_inv),
	Sort1(sort1),
	Sort2(sort2),
	Sort3(sort3),
	Reverse(reverse)
{
}

InventorySortPacketClass::InventorySortPacketClass(RawPacketClass *raw)
{
	raw->Get(ID);
	int value;
	raw->Get(value);
	TargetInv = (InventoryEnum)value;
	raw->Get(Sort1);
	raw->Get(Sort2);
	raw->Get(Sort3);
	raw->Get(Reverse);
}

RawPacketClass *InventorySortPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(40, Get_Data_Size());
	raw->Add(ID);
	int value = TargetInv;
	raw->Add(value);
	raw->Add(Sort1);
	raw->Add(Sort2);
	raw->Add(Sort3);
	raw->Add(Reverse);
	return(raw);
}

int InventorySortPacketClass::Get_Data_Size()
{
	return(sizeof(ID) + sizeof(int) + sizeof(Sort1) + sizeof(Sort2) + sizeof(Sort3) + sizeof(Reverse));
}

int InventorySortPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {ID=%d TargetInv=%d Sort1=%d Sort2=%d Sort3=%d Reverse=%d}", "InventorySort", ID, TargetInv, Sort1, Sort2, Sort3, Reverse));
}


/*
**	Reconstruction notes (pkinventorymove.cpp)
**
**	* Client 0x894CA0-0x8952D0 (0x894C70 is the previous object's guarded static, 0xC416FC; this one's
**	  is 0x8952D0). INVENTORY_MOVE 39 written by hand (builder P4, 2026-10-26); every field is a
**	  network-order int (RawPacketClass::Get(int &) / Add(const int &)); the two inventories are read
**	  into an int and stored (an enum). The destructor the scalar deleting one calls (0x690F80) is a
**	  copy from an earlier object (COMDAT).
**	* INVENTORY_SORT 40 likewise (its Reverse a bool: Get(bool &) / Add(const bool &)).
**	* The file name is provisional (no assert names it).
*/
