// Earth & Beyond client (reconstruction): app\Client\ui_trade.cpp. Notes at the end of the file.
#include <stdio.h>
#include <string.h>
#include "ui_trade.h"
#include "client.h"
#include "gadget.h"
#include "gd_robj.h"
#include "unknown_675120.h"
#include "fontmanager.h"
#include "item\client_item_property_builder.h"
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\auxdata\lookup.h"
#include "..\shared\auxdata\delta_money.h"
#include "..\shared\g_assert.h"

extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)
extern const char *Unknown_BAE854[];		// 0xBAE854: the categories' names ('Devices', ...)  // NAME 1: neutral (D10)
bool operator!=(const MoneyClass &a, const MoneyClass &b);
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
	View->Unknown34->Unknown_588A70("BUY_CONFIRMATION", false, text, 320, false, false);
}

/*
** Reconstruction notes (TR1, 2026-11-09)
** The trade window's functions in the client's order, as far as rebuilt: 0x651F60, 0x652360, 0x652450, 0x652BA0. The
** constructor 0x650560 (G_ASSERT 'PDA' l.106), Init 0x650A40 (l.137-199), 0x651390 (l.247), 0x651830 (l.466) and the
** others of 0x650490-0x653AE5 are not rebuilt yet; their lines are kept blank.
*/
