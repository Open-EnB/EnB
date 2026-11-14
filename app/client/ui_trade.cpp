// Earth & Beyond client (reconstruction): app\Client\ui_trade.cpp. Notes at the end of the file.
#define __UI_TRADE_MEMBERS	// sclient.h's ViewUnknown34Class::Unknown_5896C0
#include <stdio.h>
#include <string.h>
#include "ui_trade.h"
#include "client.h"
#include "gadget.h"
#include "gd_robj.h"
#include "unknown_675120.h"
#include "fontmanager.h"
#include "gd_slot_data.h"
#include "inv_trade_render.h"
#include "option.h"
#include "slot_data.h"
#include "inv_data.h"
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\auxdata\lookup.h"
#include "..\shared\auxdata\delta_money.h"
#include "..\shared\g_assert.h"

extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)
#include "..\common\itemtype.h"	// Unknown_BAE854 (0xBAE854): the categories' names ('Devices', ...)
bool operator!=(const MoneyClass &a, const MoneyClass &b);	// money.cpp's (out of line, ??9@YA_NABVMoneyClass@@0@Z)  // NAME 1 [a, b]: neutral
bool operator<(const MoneyClass &a, const MoneyClass &b);	// money.cpp's (out of line, ??M@YA_NABVMoneyClass@@0@Z)  // NAME 1 [a, b]: neutral
MoneyClass operator*(const MoneyClass &a, float b);			// money.cpp's (out of line, ??D@YA?AVMoneyClass@@ABV0@M@Z)  // NAME 1 [a, b]: neutral
MoneyClass operator*(float a, const MoneyClass &b);			// money.cpp's (out of line, ??D@YA?AVMoneyClass@@MABV0@@Z)  // NAME 1 [a, b]: neutral
MoneyClass operator+(const MoneyClass &a, const MoneyClass &b);	// money.cpp's (out of line, ??H@YA?AVMoneyClass@@ABV0@0@Z)  // NAME 1 [a, b]: neutral
struct InvDataPeekItemStruct	// NAME 1: neutral (D10): as far as this file reads it (ui_mfd_extended_context_harvestable.h's +0x14 Name)
{
	char Unknown00[0x0C];
	int Unknown0C;										// +0x0C (13: a kind the sale warns about)  // NAME 1: neutral (D10)
	char Unknown10[0x14 - 0x10];
	std::string Name;									// +0x14 (0x653450 sorts by it)  // NAME 2: as ui_mfd_extended_context_harvestable.h's
	char Unknown24[0x50 - 0x24]; int Unknown50;		// +0x50 (the stack count: 0x651510 enables the stack button over 1)  // NAME 1: neutral (D10)
};
// The money field's lookup through two inline layers (as ui_ptrade.cpp's): the client calls the Cast out of line
template<class C> inline C *Unknown_Find_Valid(DeltaCompoundClass *root, const char *name) { NameClass n(name); C *t = Unknown_Find_Field<C>(root, n); if (t && t->Unknown70) return(t); return(NULL); }	// NAME 1: neutral (D10)	// money.cpp's (out of line, ??9@YA_NABVMoneyClass@@0@Z)  // NAME 1 [a, b]: neutral

// The client at InterfaceClass +0x40, as this file calls it (as ui_manu.cpp declares it)
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (SClient's returns itself)  // NAME 3: ui_manu.cpp's FAIL_IF's
};
#include <hash_set.h>
#include "texture.h"
#include "gd_proxy.h"
#include "gd_slider.h"
#include "..\audio\soundlib.h"
#include "local_interface_sort.h"
#include "ui_line_display.h"

inline float Get_Slider_Value(GadgetSlider *slider) { return(slider->Value); }	// (0x651510 reads it through st0: an accessor's return)  // NAME 1: neutral (D10)








































// NOT EXACT: register-only (explain.py): the client loads client->PDAInterface into ebx and schedules PDA's assignment around it
// @0x650560
UITradeClass::UITradeClass(SClient *client) : UIInvClass(client, 7), Unknown11C(0), Unknown138(0), Unknown13C(-2), Unknown140(-1),
	Slider(NULL), Unknown170(-1), Unknown174(0), Unknown178(0)
{
	memset(Unknown0D4, 0, sizeof(Unknown0D4));
	memset(Unknown108, 0, sizeof(Unknown108));
	memset(Unknown120, 0, sizeof(Unknown120));
	Unknown_BE67BC->Load("fa_ce02a.ini");
	PDA = Create_Peek(client->PDAInterface);
	G_ASSERT(PDA);
}

// @0x6507C0
UITradeClass::~UITradeClass()
{
	SlotDataInterface::Unknown_BC();
	Unknown_BE67C4->Remove(InterfaceMesh.Peek());
	Unknown_BE67BC->Unload("fa_ce02a.ini");
	Slider->Unknown_66E460(RefCountPtr<RenderObjClass>(), 0, 0);
	GadgetClass::Destroy(Slider);
	Slider = NULL;
	if (Client->As_Space_Client()->LineDisplay) {
		Client->As_Space_Client()->LineDisplay->Flush_Lines();
	}
}

























































































































// @0x651390
void UITradeClass::Unknown_58()
{
	InterfaceSortScopeClass scope(this);
	FAIL_IF(!PDA) return;
	PDA->Unknown_695DC0(true);
	UIInvClass::Unknown_58();
	Unknown_652450(true);
	Unknown_651F60(0);
	Unknown178 = 3;
	Unknown140 = -1;
}

// @0x651470
void UITradeClass::Unknown_5C()
{
	Unknown13C = -2;
	Close_Box();
	PDA->Unknown_695DC0(false);
	Unknown17C.clear();
	UIInvClass::Unknown_5C();
}
// NOT EXACT: register (the box's +0x34 through edx, -1 kept in ebx; explain.py names a relocation: symbols.json's stale return type for 0x695680)
// @0x651510
void UITradeClass::Unknown_60()
{
	Unknown_652450(false);
	Unknown_652FF0();
	bool can_sell = false;
	bool can_sell_stack = false;
	RefCountPtr<UIPdaCargoClass> cargo = PDA->Unknown_695680();
	if (cargo) {
		if (cargo->UnknownB4 != Unknown140) {
			Close_Box();
			Unknown140 = cargo->UnknownB4;
		}
		if (cargo && cargo->UnknownB4 != -1) {
			InvDataPeekClass *peek = cargo->Unknown_59FE10()->Unknown_688830(); int count = peek->Get_Stack_Count(cargo->UnknownB4);
			if (count > 0) {
				can_sell = true;
			} else {
				cargo->UnknownB4 = -1;
				cargo->Unknown_C8();
			}
			if (count > 1) can_sell_stack = true;
		}
	}
	if (Unknown178 > 0 || (cargo && cargo->Unknown_59FE10()->Unknown_18())) {
		InvDataClass *inv = Unknown_59FE10();
		float value = Get_Slider_Value(Slider);
		if (Unknown13C == -3) {
			Unknown_652030();
		} else {
			inv->Unknown_688870((Unknown120[Unknown13C] == -1) ? 0 : (1 << Unknown120[Unknown13C]), 0);
		}
		Slider->Unknown_A4(value, 0);
		Unknown178--;
	}
	Unknown0D4[8]->Unknown_40(can_sell);
	Unknown0D4[9]->Unknown_40(can_sell_stack);
	int slot = UnknownB4;
	bool can_buy = false;
	bool can_buy_stack = false;
	if (slot >= 0) {
		InvDataPeekItemStruct *item = Unknown_59FE10()->Unknown_688830()->Get_Item(slot, false);
		if (item) {
			can_buy = true;
			if (item->Unknown50 > 1) can_buy_stack = true;
		}
	}
	if (Unknown0D4[6]->Unknown_60() != can_buy) Unknown0D4[6]->Unknown_40(can_buy);
	if (Unknown0D4[7]->Unknown_60() != can_buy_stack) Unknown0D4[7]->Unknown_40(can_buy_stack);
	if (!PDA->Unknown_695CE0()) Unknown_5C();
	UIInvClass::Unknown_60();
}





















































































// NOT EXACT: inline budget (explain.py): ours inlines 0x653A80 inside the inlined 0x6539A0 (case 17); the client calls it out of line
// @0x651830
bool UITradeClass::Unknown_68(int key)
{
	switch (key) {
		case 5:
		case 25:
			return(true);
		case 12:
			if (Unknown0D4[6]->Unknown_60()) {
				SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
				Close_Box();
				Unknown_5A02E0(false);
				return(true);
			}
			return(true);
		case 13:
			if (Unknown0D4[7]->Unknown_60()) {
				SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
				Close_Box();
				Unknown_652890();
				return(true);
			}
			return(true);
		case 14:
			if (Unknown0D4[8]->Unknown_60()) {
				SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
				Close_Box();
				Unknown_6531F0(false);
				return(true);
			}
			return(true);
		case 15:
			if (Unknown0D4[9]->Unknown_60()) {
				SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
				Close_Box();
				Unknown_652C60();
				return(true);
			}
			return(true);
		case 16:
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			Unknown_652030();

			break;
		case 17:
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			Unknown_6539A0();

			break;
		case 18:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Close_Box();
			Unknown_5C();
			return(true);
		default:
			if (key >= 6 && key <= 11) {
				Close_Box();
				Unknown_651F60(key - 6);
				SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
				return(true);
			}
			if (key >= 20 && key <= 24) {
				G_ASSERT(false);
				return(true);
			}
			return(UIInvClass::Unknown_68(key));
	}
	Close_Box(); return(true);
}

// @0x651C30
void UITradeClass::Unknown_CC()
{
	if (Unknown_651EC0() && Unknown138 > 0) {
		if (Unknown13C == -2) {
			Unknown_651F60(0);
		} else {
			Unknown_651F60(Unknown13C);
		}
	}
	UIInvClass::Unknown_CC();
}

// @0x651D10
void UITradeClass::Unknown_D0()
{
	Unknown_651EC0();
	UIInvClass::Unknown_CC();
}

// @0x651DB0
void UITradeClass::Unknown_C8()
{
	if ((unsigned int)Unknown170 < 5) Unknown108[Unknown170]->Unknown_90(StandardBttnTexture.Peek());
	int slot = UnknownB4;
	bool changed = (Unknown170 != slot);
	Unknown170 = slot;
	if (slot != -1) {
		Unknown170 = Find_Shown(slot) - UnknownA8;
	}
	if ((unsigned int)Unknown170 < 5) {
		Unknown108[Unknown170]->Unknown_90(SelectedBttnTexture.Peek());
		if (changed) Close_Box();
	}
}

// @0x651EC0
bool UITradeClass::Unknown_651EC0()
{
	int types = Unknown_59FE10()->Unknown160;
	if (types != Unknown11C) {
		Unknown11C = types;
		memset(Unknown120, 0, sizeof(Unknown120));
		Unknown138 = 0;
		for (int i = 0; i < 19; i++) {
			if (types & 1) {
				Unknown120[Unknown138] = i;
				Unknown138++;
			}
			if (Unknown138 >= 6) break;
			types >>= 1;
		}
		Unknown_652360();
		return(true);
	}
	return(false);
}





































































// NOT EXACT: 0x651F60's -1 call pushes a zeroed edx twice, ours shares the other call's push 0 (register residue)
// @0x651F60
void UITradeClass::Unknown_651F60(int index)
{
	InvDataClass *inv = Unknown_59FE10();
	if (Unknown138 > 0) {
		if (index == -2) {
			inv->Unknown_688870(0, 0);
			Unknown13C = -2;
		} else {
			if (index >= Unknown138) index = Unknown138 - 1;
			if (index < 0) index = 0;
			Unknown13C = index;
			if (Unknown120[index] == -1) {
				inv->Unknown_688870(0, 0);
			} else {
				inv->Unknown_688870(1 << Unknown120[index], 0);
			}
		}
	} else {
		Unknown13C = -2;
	}
	Unknown_652360();
	UnknownB4 = -1;
	Unknown_C8();
	Unknown170 = -1;
	Unknown_653450();
}

// @0x652030
void UITradeClass::Unknown_652030()
{
	if (Unknown13C != -3) {
		UnknownB4 = -1;
		Unknown_C8();
		Unknown170 = -1;
	}
	Unknown13C = -3;
	Unknown_652360();
	Unknown_652670();
	std::hash_set<int> ids(100);
	for (std::vector<RefCountPtr<InvDataClass> >::iterator it = Unknown150.begin(); it != Unknown150.end(); it++) {
		RefCountPtr<InvDataClass> inv = *it;
		inv->Unknown_6A7860();
		InvDataPeekClass *items = inv->Unknown_688830();
		InvDataPeekClass *ammo = inv->Unknown_688850();
		int count = inv->Unknown_6A7990();
		for (int i = 0; i < count; i++) {
			int slot = inv->Unknown_6A7A00(i);
			int id = items->Get_Item_Template_ID(slot);
			if (id != -1 && id != -2) ids.insert(id);
			int ammo_id = ammo->Get_Item_Template_ID(slot);
			if (ammo_id != -1 && ammo_id != -2) ids.insert(ammo_id);
		}
	}
	InvDataClass *own = Unknown_59FE10(); own->Unknown_688870(-1, &ids);
	Unknown_653450();
}




// @0x652360
void UITradeClass::Unknown_652360()
{
	for (int i = 0; i < 6; i++) {
		bool enable = true;
		bool selected = (Unknown13C == -2);
		if (i == Unknown13C) {
			selected = true;
		} else if (i >= Unknown138) {
			enable = false;
		}
		ButtonGadgetClass *bttn = Unknown0D4[i];
		FAIL_IF(!bttn) continue;
		bttn->Unknown_40(enable);



		FontSys *font = bttn->Unknown34;
		FAIL_IF(!font) continue;
		font->Change_Text(Unknown_BAE854[Unknown120[i]], 0);
		font->Unknown_6CE910(selected);
	}
}




// @0x652450
void UITradeClass::Unknown_652450(bool force)
{
	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
	DeltaTemplateClass<MoneyClass> *t = Unknown_Find_Valid<DeltaTemplateClass<MoneyClass> >(aux, "Money");
	if (t) {
		MoneyClass money = t->Value;
		if (operator!=(Money, money) || force) {
			Money = money;
			FontSys *font = Unknown_BE67C4->Get_Style("credits00_ce02a", Get_Unknown0D0());
			FAIL_IF(!font) return;
			font->Change_Text(Money.Amount_As_Text(true).c_str(), 0);
		}
		return;
	}
	Money = MoneyClass((unsigned int)-1);
}

// @0x652670
void UITradeClass::Unknown_652670()
{
	SClient *client = Client->As_Space_Client();
	if (Unknown150.empty()) {
		Unknown150.push_back(Create_Get(new InvDataClass(1, client)));
		Unknown150.push_back(Create_Get(new InvDataClass(2, client)));
		Unknown150.push_back(Create_Get(new InvDataClass(3, client)));
	}
}







// @0x652890
void UITradeClass::Unknown_652890()
{
	if ((unsigned int)Unknown170 < 5) {
		SlotDataGadgetClass *bttn = Buttons[Unknown170];
		FAIL_IF(!bttn) return;

		SlotDataRenderClass *sd_render = bttn->SlotRender;
		FAIL_IF(!sd_render) return;

		InvTradeRenderClass *trade_render = (InvTradeRenderClass *)sd_render->Unknown_24();
		FAIL_IF(!trade_render) return;
		InvDataPeekClass *peek = trade_render->Peek_Inv_Data()->Unknown_688830();
		int slot = trade_render->Unknown20;
		MoneyClass price = peek->Get_Price(slot);
		std::string name("");
		int count;
		Unknown_5A00C0(&count, &name);
		MoneyClass total = (float)count * price;
		char text[128];
		_snprintf(text, sizeof(text), "Confirm purchase of %d %s for %s credits?", count, name.c_str(), total.Amount_As_Text(true).c_str());
		Get_View()->Get_Unknown34()->Unknown_588A70("BUY_CONFIRMATION", false, text, 320, false, false);
		Unknown174 = 1;
	}
}











// @0x652BA0
void UITradeClass::Unknown_652BA0(bool arg, bool significant)
{
	char text[128];
	if (significant) {
		_snprintf(text, sizeof(text), "You will take significant losses on this sale. Do you wish to proceed?");
	} else {
		_snprintf(text, sizeof(text), "You will take a loss on this sale. Do you wish to proceed?");
	}
	if (arg) {
		Unknown174 = 3;
	} else {
		Unknown174 = 2;
	}
	Get_View()->Get_Unknown34()->Unknown_588A70("BUY_CONFIRMATION", false, text, 320, false, false);
}




// @0x652C60
void UITradeClass::Unknown_652C60()
{
	Unknown174 = 4;
	MoneyClass total;
	RefCountPtr<UIPdaCargoClass> cargo = PDA->Unknown_695630();
	if (cargo) {
		int slot = cargo->UnknownB4;
		InvDataPeekClass *peek = cargo->Unknown_59FE10()->Unknown_688830();
		MoneyClass price = peek->Get_Price(slot);
		unsigned int count = peek->Get_Stack_Count(slot);
		MoneyClass extra;
		total = price * (float)count + extra;
	}
	char text[128];
	_snprintf(text, sizeof(text), "Are you sure you want to sell the whole stack for %s credits?", total.Amount_As_Text(true).c_str());
	Get_View()->Get_Unknown34()->Unknown_588A70("BUY_CONFIRMATION", false, text, 320, false, false);
}














// @0x652E70
void UITradeClass::Unknown_652E70()
{
	if (View && View->Unknown34) View->Unknown34->Unknown_5896C0("BUY_CONFIRMATION");
	Unknown174 = 0;
}




// @0x652EB0
bool UITradeClass::Unknown_652EB0()
{
	if (Unknown174 != 1) {
		RefCountPtr<UIPdaCargoClass> cargo = PDA->Unknown_695630();
		if (!cargo) return(false);
		int slot = cargo->UnknownB4;
		if (slot == -1) return(false);
		InvDataClass *inv = cargo->Unknown_59FE10();
		InvDataPeekClass *peek = inv->Unknown_688830();
		InvDataPeekItemStruct *item = peek->Get_Item(slot, false);
		if (item == NULL) return(false);
	}
	return(true);
}





// @0x652FF0
void UITradeClass::Unknown_652FF0()
{
	if (!Get_View()->Get_Unknown34()->Is_Active()) return;
	if (!Unknown_652EB0()) {
		Unknown_652E70();
		return;
	}
	int button = Get_View()->Get_Unknown34()->Unknown_5890B0("BUY_CONFIRMATION");
	if (button == 0) return;
	if (button == 1) {
		switch (Unknown174) {
			case 1:
				Unknown_5A02E0(true);
				break;
			case 2: {
				RefCountPtr<UIPdaCargoClass> cargo = PDA->Unknown_695630();
				if (cargo) cargo->Unknown_5A03B0(false);
				break;
			}
			case 3: {
				RefCountPtr<UIPdaCargoClass> cargo = PDA->Unknown_695630();
				if (cargo) cargo->Unknown_5A03B0(true);
				break;
			}
			case 4:
				Unknown_652E70();
				Unknown_6531F0(true);
				return;
		}
	}
	Unknown_652E70();
}



// @0x6531F0
void UITradeClass::Unknown_6531F0(bool arg)
{
	if (!Unknown_652EB0()) return;
	RefCountPtr<UIPdaCargoClass> cargo = PDA->Unknown_695630();
	if (cargo) {
		int slot = cargo->UnknownB4;
		InvDataPeekClass *peek = cargo->Unknown_59FE10()->Unknown_688830();
		MoneyClass price = peek->Get_Price(slot);
		float value = peek->Get_Ave_Cost(slot);
		if (peek->Get_Item(slot, false)->Unknown0C == 13 && operator<(price, MoneyClass((__int64)value))) {
			bool significant = operator<(price, MoneyClass((__int64)(value * 0.5f)));
			Unknown_652BA0(arg, significant);
		} else {
			cargo->Unknown_5A03B0(arg);
		}
	}
}










// NOT EXACT: inline budget, as TR1's stash (docs/attempts/0x653450): ours inlines the string assigns the client calls out of line
// @0x653450
void UITradeClass::Unknown_653450()
{
	SlotDataGadgetClass *button = Buttons[0];
	FAIL_IF(!button) return;

	SlotDataRenderClass *sd_render = button->SlotRender;
	FAIL_IF(!sd_render) return;

	InvTradeRenderClass *trade_render = (InvTradeRenderClass *)sd_render->Unknown_24();
	FAIL_IF(!trade_render) return;
	InvDataPeekClass *peek = trade_render->Peek_Inv_Data()->Unknown_688830();
	Unknown17C.clear();
	bool by_name = OptionClass::Instance->Get_Bool("Trade_Sort_By_Name");
	for (int i = 0; i < SlotData->Unknown_6A7990(); i++) {
		int slot = SlotData->Unknown_6A7A00(i);
		MoneyClass price = peek->Get_Price(slot);
		std::string name;
		name = "";
		InvDataPeekItemStruct *item = peek->Get_Item(slot, true);
		if (item) name = item->Name;
		std::vector<int>::iterator it;
		for (it = Unknown17C.begin(); it != Unknown17C.end(); it++) {
			if (!by_name) {
				MoneyClass other = peek->Get_Price(*it);
				if (operator<(price, other)) break;
			} else {
				std::string other("");
				InvDataPeekItemStruct *other_item = peek->Get_Item(*it, true);
				if (other_item) other = other_item->Name;
				if (name.compare(other) < 0) break;
			}
		}
		Unknown17C.insert(it, 1, slot);
	}
	Unknown_B0();
}

// @0x6538E0
void UITradeClass::Unknown_B0()
{
	if (Is_Active()) {
		if (UnknownA8 == -1) UnknownA8 = 0;
		int count = Buttons.size();
		for (int i = 0; i < count; i++) {
			Unknown_B4(Get_Shown(i + UnknownA8), i);
		}
		Unknown_C8();
	}
}

// @0x6539A0
void UITradeClass::Unknown_6539A0()
{
	BoolOptionStruct *option = OptionClass::Instance->Find_Bool("Trade_Sort_By_Name");
	option->Value = !option->Value;
	Unknown_653A80();
	if (Unknown13C == -3) {
		UnknownB4 = -1;
		Unknown_C8();
		Unknown170 = -1;
		Unknown_652030();
	} else {
		Unknown_651F60(Unknown13C);
	}
}

// @0x653A80
void UITradeClass::Unknown_653A80()
{
	ButtonGadgetClass *sort = Unknown0D4[11];
	if (sort) {
		if (OptionClass::Instance->Get_Bool("Trade_Sort_By_Name")) {
			sort->Unknown34->Change_Text("Price sort", 0);
			GadgetClass::Set_Tooltip(sort, "UI_TRADE", "SORT_PRICE");
			return;
		}
		sort->Unknown34->Change_Text("Name sort", 0);
		GadgetClass::Set_Tooltip(sort, "UI_TRADE", "SORT_NAME");
	}
}

/*
** Reconstruction notes (TR1, 2026-11-09; O23, 2026-11-14)
** The trade window, a UIInvClass of kind 7 (tables 0xAF8F70 / 0xAF8F64 / 0xAF8F58): its own slots +0x00, +0x54 Init,
** +0x58 0x651390, +0x5C 0x651470, +0x60 0x651510, +0x68 0x651830, +0xB0 0x6538E0, +0xC8 0x651DB0, +0xCC 0x651C30,
** +0xD0 0x651D10. The functions in the client's order: the constructor 0x650560, the destructor 0x6507C0 (deleting
** 0x650790), Init 0x650A40, 0x651390-0x651EC0, 0x651F60, 0x652030 (and 0x6522D0, its hash_set's out-of-line
** destructor for the unwind), 0x652360, 0x652450, 0x652670, 0x652890-0x6531F0, 0x653450, 0x6538E0, 0x6539A0 (no
** caller: 0x651830 inlines it), 0x653A80; then the instances 0x653B10-0x654850 (Create_Peek / Create_Get, the vectors'
** and the hash_set's members, the thunks 0x654840 / 0x654850 for the second and third tables).
** Init 0x650A40 (FAIL_IFs l.137-199) is not written yet; its lines are kept blank.
** Evidence: the asserts' names ('PDA', '!InterfaceMesh', 'StandardBttnTexture', 'SelectedBttnTexture'); the buttons'
** sub-objects from the table 0xB8BE98 ('category00'..'category05', 'buy', 'buy_stack', 'sell', 'sellall', 'compare',
** 'sort', 'exit'); +0x150 holds three InvDataClass (0x688410 with types 1-3), +0x160 is gd_inv_slider.cpp's slider.
** Forms: the box is reached through Get_View()->Get_Unknown34() (the client reads it into eax first); 0x652EB0 stays
** out of line in 0x652FF0 / 0x6531F0 only with its item through named locals (a shorter source is inlined); 0x652FF0
** closes the box on its own when 0x652EB0 fails; 0x652C60 sets +0x174 before its total. Close_Box is 0x652E70's body
** inline (eight inlined copies in 0x651830); 0x651830's cases 16 and 17 share the switch's tail (the box closed after
** it), the disabled buttons return at once; 0x651EC0 tests the change first and returns false last; 0x651DB0 takes the
** change before the search and the search keeps its own result; 0x6538E0 reads +0x17C through a local with -1 first;
** 0x652030 and 0x651510 take the inventory into a local before the call; 0x651510 reads the slider through an
** accessor (fld / fstp).
*/
