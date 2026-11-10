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
#include "item\client_item_property_builder.h"
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\auxdata\lookup.h"
#include "..\shared\auxdata\delta_money.h"
#include "..\shared\g_assert.h"

extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)
extern const char *Unknown_BAE854[];		// 0xBAE854: the categories' names ('Devices', ...)  // NAME 1: neutral (D10)
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
			font->Change_Text(Money.Unknown_4F00C0(true).c_str(), 0);
		}
		return;
	}
	Money = MoneyClass((unsigned int)-1);
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
		_snprintf(text, sizeof(text), "Confirm purchase of %d %s for %s credits?", count, name.c_str(), total.Unknown_4F00C0(true).c_str());
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
	RefCountPtr<Unknown5CF9B0Class> cargo = PDA->Unknown_695630();
	if (cargo) {
		int slot = cargo->UnknownB4;
		InvDataPeekClass *peek = cargo->Unknown_59FE10()->Unknown_688830();
		MoneyClass price = peek->Get_Price(slot);
		unsigned int count = peek->Get_Stack_Count(slot);
		MoneyClass extra;
		total = price * (float)count + extra;
	}
	char text[128];
	_snprintf(text, sizeof(text), "Are you sure you want to sell the whole stack for %s credits?", total.Unknown_4F00C0(true).c_str());
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
		RefCountPtr<Unknown5CF9B0Class> cargo = PDA->Unknown_695630();
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
				RefCountPtr<Unknown5CF9B0Class> cargo = PDA->Unknown_695630();
				if (cargo) cargo->Unknown_5A03B0(false);
				break;
			}
			case 3: {
				RefCountPtr<Unknown5CF9B0Class> cargo = PDA->Unknown_695630();
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
	RefCountPtr<Unknown5CF9B0Class> cargo = PDA->Unknown_695630();
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

/*
** Reconstruction notes (TR1, 2026-11-09)
** The trade window's functions in the client's order, as far as rebuilt: 0x651F60, 0x652360, 0x652450, 0x652890, 0x652BA0,
** 0x652C60, 0x652E70, 0x652EB0, 0x652FF0, 0x6531F0. The constructor 0x650560 (G_ASSERT 'PDA' l.106), Init 0x650A40
** (l.137-199), 0x651390 (l.247), 0x651830 (l.466), the hash-set users 0x652030 / 0x6522D0 / 0x652670, 0x653450
** (l.911-917) and the rest of 0x650490-0x653AE5 are not rebuilt yet; their lines are kept blank.
** Forms: the box is reached through Get_View()->Get_Unknown34() (the client reads it into eax first); 0x652EB0 stays
** out of line in 0x652FF0 / 0x6531F0 only with its item through named locals (a shorter source is inlined); 0x652FF0
** closes the box on its own when 0x652EB0 fails; 0x652C60 sets +0x174 before its total.
*/
