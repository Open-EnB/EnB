// Earth & Beyond client (reconstruction): app\client\inv_trade_render.cpp, an inventory slot on the trade screen
// (the client's file per its asserts, 'e:\build\beta\G\app\Client\inv_trade_render.cpp' at 0xB905A8). Notes at the end.
#include <string>
#include <stdio.h>
#include "..\shared\g_assert.h"
#include "..\shared\money.h"
#include "inv_trade_render.h"
#include "rendobj.h"
#include "fontmanager.h"
#include "fontdatabase.h"

// inv_data.cpp's, as far as this file calls it
class Unknown688830Item { public: char Unknown00[0x14]; std::string Unknown14; };	// NAME 1: neutral (D10)
class Unknown688830Class { public: Unknown688830Item *Unknown_68AAA0(int slot, bool flag); int Unknown_689CA0(int slot); bool Unknown_689C60(int slot); MoneyClass Unknown_689DA0(int slot); };	// NAME 1: neutral (D10); NAME 1 [slot, flag]: neutral
class Unknown695E10Class { public: void Unknown_695E10(int id, int *owned, int *slots); };	// NAME 1: neutral (D10); NAME 1 [id, owned, slots]: neutral
class SClient { public: char Unknown00[0x1284]; Unknown695E10Class *Unknown1284; };	// sclient.cpp's, as far as this file reads it
class InvDataClass { public: Unknown688830Class *Unknown_688830(); char Unknown00[0x20]; SClient *Client; };	// NAME 3: Peek_Inv_Data, inv_data's __FILE__ 0xB90004
extern FontStringFilesClass *Unknown_BE67BC;	// NAME 1: neutral (D10)
extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)


// 0x68FB60 and 0x68FC80 stay at the end of inv_render.cpp for now: see the notes









































// NOT EXACT: 0x68FE10, 26 rows: register and block order (the item's name arm sits after the function's last jump in the client)
// 0x68FE10
void InvTradeRenderClass::Unknown_2C(bool force)
{
	InvRenderClass::Unknown_2C(force);
	int slot = Unknown20;
	bool hidden = true;	// NAME 1 [hidden, data, cost, stack, slots, owned, name, item]: neutral
	if (slot != -1) {
		hidden = false;
		Unknown688830Class *data = Peek_Inv_Data()->Unknown_688830();
		if (!force && !data->Unknown_689C60(slot)) {
			return;
		}
		if (CostText) {
			MoneyClass cost = data->Unknown_689DA0(slot);
			CostText->Unknown_6CE420(0, "Cost: %s each", cost.Unknown_4F00C0(true).c_str());
		}
		if (StackText) {
			int stack = Unknown_68D7E0(slot);
			if (stack > 1) {
				StackText->Unknown_6CE420(0, "%d per stack", stack);
			} else {
				StackText->Unknown_6CE420(0, "");
			}
		}
		if (OwnedTextValue) {
			int slots = 0;
			int owned = 0;
			Unknown695E10Class *counter = Peek_Inv_Data()->Client->Unknown1284;	// NAME 1 [counter]: neutral
			counter->Unknown_695E10(data->Unknown_689CA0(slot), &owned, &slots);
			if (slots > 1) {
				OwnedTextValue->Unknown_6CE420(0, "Owned: %d in %d slots", owned, slots);
			} else if (owned > 0) {
				OwnedTextValue->Unknown_6CE420(0, "Owned: %d", owned);
			} else {
				OwnedTextValue->Unknown_6CE420(0, "");
			}
		}


		if (DescriptionGroup) {
			std::string name;








			Unknown688830Item *item = data->Unknown_68AAA0(slot, true);
			name = !item ? (G_ASSERT(item), std::string("Item Unknown")) : item->Unknown14;
			DescriptionGroup->Unknown_6D0A70(Unknown58.c_str(), name.c_str());
			DescriptionGroup->Unknown_6D0A70(Unknown68.c_str(), "");
		}
	}
	if (force) {
		if (CostText) {
			CostText->Set_Hidden(hidden);
		}
		if (StackText) {
			StackText->Set_Hidden(hidden);
		}
		if (OwnedText) {
			OwnedText->Set_Hidden(hidden);
		}
		if (OwnedTextValue) {
			OwnedTextValue->Set_Hidden(hidden);
		}
		if (DescriptionGroup) {
			if (hidden) {
				DescriptionGroup->Unknown_6D0550(Unknown58.c_str());
				DescriptionGroup->Unknown_6D0550(Unknown68.c_str());
			} else {
				DescriptionGroup->Unknown_6D0390(Unknown58.c_str());
				DescriptionGroup->Unknown_6D0390(Unknown68.c_str());
			}
		}
	}
}





// 0x6902E0
void InvTradeRenderClass::Unknown_6902E0(RenderObjClass *robj, int index, const char *suffix)	// NAME 1 [index, suffix]: neutral
{
	FAIL_IF(!robj) return;
	char name[128];	// NAME 1 [name]: neutral
	sprintf(name, "WRAP_text%02d_%s", index, suffix);
	Unknown58 = name;
	sprintf(name, "WRAP_BODY%02d_%s", index, suffix);
	Unknown68 = name;




	DescriptionGroup = Unknown_BE67C4->Find(robj);
	G_ASSERT(DescriptionGroup);
	DescriptionGroup->Unknown_6D0A70(Unknown58.c_str(), "");
	sprintf(name, "cost%02d_%s", index, suffix);
	CostText = Create_Get(Unknown_BE67C4->Get_Style(name, robj));



	G_ASSERT(CostText);
	if (CostText) CostText->Set_Hidden(true);

	sprintf(name, "stack%02d_%s", index, suffix);
	StackText = Create_Get(Unknown_BE67C4->Get_Style(name, robj));
	G_ASSERT(StackText);
	if (StackText) StackText->Set_Hidden(true);

	sprintf(name, "owned_title%02d_%s", index, suffix);
	OwnedText = Create_Get(Unknown_BE67C4->Get_Style(name, robj));
	G_ASSERT(OwnedText);
	if (OwnedText) OwnedText->Set_Hidden(true);

	sprintf(name, "owned%02d_%s", index, suffix);
	OwnedTextValue = Create_Get(Unknown_BE67C4->Get_Style(name, robj));
	G_ASSERT(OwnedTextValue);
	if (OwnedTextValue) OwnedTextValue->Set_Hidden(true);
}

/*
**	Reconstruction notes. The client's file per its asserts ('e:\build\beta\G\app\Client\inv_trade_render.cpp' at 0xB905A8; lines
**	115-185), its object from 0x68FB60: inv_render.cpp's ends at 0x68FB4A, `<string>`'s initialiser. The class name is D24's, from the file.
**	The constructor 0x68FB60 and destructor 0x68FC80 belong here; moved, 0x68FAE0 (inv_render's header initialiser, credited by the object's range, D100)
**	falls out of that range and is lost outright: P-IR2-1, for the user (D66).
*/
