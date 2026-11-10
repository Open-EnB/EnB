// Earth & Beyond client (reconstruction): app\client\inv_data.cpp, an inventory's slot data
// (the client's file per its asserts, 'e:\build\beta\G\app\Client\inv_data.cpp' at 0xB8FDC4). Notes at the end.
#include "inv_data.h"
#include "sclient.h"
#include "co.h"
#include "..\shared\g_assert.h"

// The item 0x68AAA0 returns, as far as this file reads it (as ui_inv.cpp declares it)
struct InvDataPeekItemStruct	// NAME 1: neutral (D10)
{
	char Unknown00[0x0C];
	int Unknown0C;										// +0x0C (the item's type: a bit of +0x15C's mask)  // NAME 1: neutral (D10)
};


// 0x688410
InvDataClass::InvDataClass(int inv_type, SClient *client) :
	SlotDataClass(client),
	Unknown50(inv_type),
	Unknown54(client, Unknown_689100(inv_type, 0), inv_type == 2),
	UnknownD8(client, Unknown_689100(inv_type, 1), false),
	Unknown15C((unsigned int)-1),
	Unknown160(0)
{
}


// 0x6886D0
InvDataClass::~InvDataClass()
{
}

// 0x688830
InvDataPeekClass *InvDataClass::Unknown_688830()
{
	return(&Unknown54);
}

// 0x688850
InvDataPeekClass *InvDataClass::Unknown_688850()
{
	return(&UnknownD8);
}


// 0x688870
void InvDataClass::Unknown_688870(unsigned int mask, const std::hash_set<int> *ids)
{
	Unknown15C = mask;
	if (ids != NULL) {
		Unknown164 = *ids;
		if (Unknown164.size() == 0) {
			Unknown164.insert(-1);
		}
	} else {
		Unknown164.clear();
	}
	Unknown_6A7860();
}


































// 0x6889F0
DeltaCompoundClass *InvDataClass::Unknown_1C()
{
	G_ASSERT(!Peek_Object());
	DeltaCompoundClass *root = NULL;
	ClientObjectClass *object = NULL;
	switch (Unknown50) {
		case 3:
		case 19:
		case 20:
			root = (DeltaCompoundClass *)Get_Client()->AuxData;
			break;
		case 1:
		case 2:
			object = Get_Client()->Get_Ship();
			break;
		case 4:
			root = (DeltaCompoundClass *)Get_Client()->AuxData;
			break;
		case 5:
		case 6:
		case 18:
			object = Get_Client()->Get_Target();
			break;
		case 12:
		case 13:
		case 14:
			object = Get_Client()->Unknown_74BF80();
			break;
		case 16:
			object = Get_Client()->Get_Ship();
			break;
		case 17:
			object = Get_Client()->Find_Object(Get_Client()->Unknown1134);
			break;













		default:
			G_ASSERT(false);
			return(NULL);
	}
	if (object != NULL && (root = (DeltaCompoundClass *)object->AuxData) != NULL) {
		Unknown_08(object);
	}
	Unknown54.Attach(root);
	UnknownD8.Attach(root);
	return(root);
}

// 0x688B50
bool InvDataClass::Unknown_18()
{
	if (Unknown54.Changed()) {
		return(true);
	}
	return(UnknownD8.Changed() ? true : false);
}

// NOT EXACT: functional from the pseudocode (D64)
// 0x688B90
bool InvDataClass::Unknown_20()
{
	if (!Unknown_688F70()) {
		return(false);
	}
	SlotDataClass::Unknown_24();
	Unknown160 = 0;
	bool filtered = (Unknown164.size() != 0);
	int count = Unknown54.Count;
	int index = 0;
	for (int slot = 0; slot < count; slot++) {
		int id = Unknown54.Get_Item_Template_ID(slot);
		bool show = false;
		if (id != -2) {
			if (Unknown15C == (unsigned int)-1) {
				show = true;
			}
			if (id != -1) {
				InvDataPeekItemStruct *item = Unknown54.Get_Item(slot, true);
				if (Unknown54.Status != 2 && item != NULL) {
					unsigned int bit = (item->Unknown0C == -1) ? 0 : (1 << item->Unknown0C);
					Unknown160 |= bit;
					if ((Unknown15C & bit) == bit) {
						if (filtered) {
							show = (Unknown164.find(id) != Unknown164.end());
						} else {
							show = true;
						}
					}
				}
			}
		}
		if (show) {
			Unknown40.push_back(slot);
			Unknown30.push_back(index);
			index++;
		} else {
			Unknown30.push_back(-1);
		}
	}
	return(true);
}


// 0x688EF0
void InvDataClass::Unknown_24()
{
	Unknown160 = 0;
	Unknown17C.clear();
	DeltaCompoundClass *root = Peek_Aux_Data();
	Unknown54.Attach(root);
	UnknownD8.Attach(root);
	SlotDataClass::Unknown_24();
}


// 0x688F70
bool InvDataClass::Unknown_688F70()
{
	int count = Unknown54.Count;
	std::vector<bool> used;
	for (int slot = 0; slot < count; slot++) {
		used.push_back(Unknown54.Get_Item_Template_ID(slot) != -2);
	}
	if (Unknown17C != used) {
		Unknown17C.swap(used);
		return(true);
	}
	return(false);
}














































































// 0x689100
const char *InvDataClass::Unknown_689100(int inv_type, int list)
{
	switch (list) {
		case 0:
			switch (inv_type) {
				case 1: return("Inventory.Cargo");
				case 2: return("Inventory.Equipped");
				case 3: return("SecureInventory");
				case 4: return("VendorInventory");
				case 5: return("Equipped");
				case 6: return("Cargo");
				case 12: return("ManufactureTarget");
				case 13: return("ManufactureComponents");
				case 14: return("ManufactureOverride");
				case 16:
				case 17: return("Inventory.PlayerTrade");
				case 18: return("HarvestableResources");
				case 19: return("RewardInventory");
				case 20: return("OverflowInventory");
			}
			break;
		case 1:
			switch (inv_type) {
				case 2: return("Inventory.Ammo");
				case 1: case 3: case 4: case 5: case 6: case 12: case 13: case 14: case 16: case 17: case 18: case 19: case 20:
					return("");
			}
			break;
	}
	G_ASSERT(false);
	return("");
}

/*
**	Notes.
**	Object 0x688410-0x6894FF: the constructor first, 0x689100 (the lists' names) the last function of the file's own;
**	the SGI hash_set's and vector<bool>'s template members follow it (0x689270, 0x689290, 0x6894E0 on). Asserts:
**	'!Peek_Object()' l.97 and 'false' l.143 in 0x6889F0, 'false' l.343 in 0x689100.
**	Each inventory type reads its aux tree's root from a different place (0x6889F0): the player's own (SClient +0x12C0)
**	for the vault, vendor, reward and overflow lists, else the object's (+0x88) for the ship, the target, the
**	manufacturing object or the trade partner, which the slot data then watches.
*/
