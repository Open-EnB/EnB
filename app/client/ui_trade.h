// Earth & Beyond client (reconstruction): app\client\ui_trade.h, the trade window (the client's ui_trade.cpp per its
// asserts, 'e:\build\beta\G\app\Client\ui_trade.cpp' at 0xB8BF74; this header's name is provisional, NAME 1, D137).
// Laid out from the constructor 0x650560 and the functions 0x651F60-0x653450 (TR1, 2026-11-09).
#ifndef UI_TRADE_H  // NAME 1 [ui_trade.h]: provisional header name for ui_trade.cpp
#define UI_TRADE_H

#include <vector>
#include <string>
#include "pdaint.h"			// (before sclient.h: its PDAInterfaceClass; ui_inv.h's UIInvClass)
#include "sclient.h"
#include "..\shared\money.h"
#include "..\..\wwlib\ref_ptr.h"

class ButtonGadgetClass;		// unknown_675120.h's buttons (+0xD4)
class SlotDataGadgetClass;		// gd_slot_data.h's slot gadgets (the base's +0x88)
class RenderObjClass;
class SlotDataClass;			// slot_data.h's (the base's +0x98)
class InvDataClass;				// client_item_property_builder.h's (the base's 0x59FE10)

// The trade window (0x190 bytes, tables 0xAF8F70 / 0xAF8F64 / 0xAF8F58; 'fa_ce02a.ini'; ui_talkt.cpp makes it)
class UITradeClass : public UIInvClass  // NAME 3: class-from-file (D24) for ui_trade.cpp
{
public:
	UITradeClass(SClient *client);						// 0x650560
	void Unknown_651F60(int index);						// 0x651F60 (the category: clamped to the shown ones, -2 none)  // NAME 1 [index]: neutral
	void Unknown_652360();								// 0x652360 (FAIL_IFs '!bttn' l.674, '!font' l.680: the six category buttons)
	void Unknown_652450(bool force);					// 0x652450 (FAIL_IF '!font' l.699: the credits text)  // NAME 1 [force]: neutral
	void Unknown_652890();								// 0x652890 (FAIL_IFs '!bttn' l.729, '!sd_render' l.732, '!trade_render' l.735: the purchase asked, +0x174 1)
	void Unknown_652BA0(bool arg, bool significant);	// 0x652BA0 (the loss warning box, +0x174 3 with arg, else 2)  // NAME 1 [arg, significant]: neutral
	void Unknown_652C60();								// 0x652C60 (the whole stack's sale asked, +0x174 4)
	void Unknown_652E70();								// 0x652E70 (the 'BUY_CONFIRMATION' box closed, +0x174 0)
	bool Unknown_652EB0();								// 0x652EB0 (the cargo window's slot holds an item, or +0x174 is 1)
	void Unknown_652FF0();								// 0x652FF0 (the box's answer: by +0x174)
	void Unknown_6531F0(bool arg);						// 0x6531F0 (the sale's loss warning)  // NAME 1 [arg]: neutral
	void Unknown_653450();
	RenderObjClass *Get_Unknown0D0() { return(Unknown0D0); }	// (inline: 0x652450's register)  // NAME 1: neutral (D10)								// 0x653450 (FAIL_IFs '!button' l.911, '!sd_render' l.914, '!trade_render' l.917)

	RenderObjClass *Unknown0D0;							// +0xD0 (the window's model: the styles' object)  // NAME 1: neutral (D10)
	ButtonGadgetClass *Unknown0D4[13];					// +0xD4 (0..5 the category buttons)  // NAME 1: neutral (D10)
	int Unknown108[5];									// +0x108
	int Unknown11C;										// +0x11C
	int Unknown120[6];									// +0x120 (each button's category: the index into 0xBAE854)  // NAME 1: neutral (D10)
	int Unknown138;										// +0x138 (the buttons shown)  // NAME 1: neutral (D10)
	int Unknown13C;										// +0x13C (the selected button: -2 none, -3)  // NAME 1: neutral (D10)
	int Unknown140;										// +0x140 (-1)
	int Unknown144;										// +0x144
	MoneyClass Money;									// +0x148 (the credits shown; 0xFFFFFFFF without the field)  // NAME 2: what it holds
	std::vector<int> Unknown150;						// +0x150  // NAME 1: neutral (D10)
	int Unknown160;										// +0x160
	RefCountPtr<PDAInterfaceClass> PDA;					// +0x164 (0x653B90's, the client's +0x1284; G_ASSERT 'PDA' l.106)  // NAME 3: the assert's
	int Unknown168;										// +0x168
	int Unknown16C;										// +0x16C
	int Unknown170;										// +0x170 (-1)
	int Unknown174;										// +0x174 (the box shown: 0 none, 2 / 3 a loss warning, 4 the stack)  // NAME 1: neutral (D10)
	int Unknown178;										// +0x178
	std::vector<int> Unknown17C;						// +0x17C  // NAME 1: neutral (D10)
	int Unknown18C;										// +0x18C
};

#endif // UI_TRADE_H
