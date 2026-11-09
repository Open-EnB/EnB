// Earth & Beyond client (reconstruction): app\client\ui_trade.h, the trade window (the client's ui_trade.cpp per its
// asserts, 'e:\build\beta\G\app\Client\ui_trade.cpp' at 0xB8BF74; this header's name is provisional, NAME 1, D137).
// Laid out from the constructor 0x650560 and the functions 0x651F60-0x653450 (TR1, 2026-11-09).
#ifndef UI_TRADE_H  // NAME 1 [ui_trade.h]: provisional header name for ui_trade.cpp
#define UI_TRADE_H

#include <vector>
#include <string>
#include "pdaint.h"			// (before sclient.h: its PDAInterfaceClass)
#include "sclient.h"
#include "..\shared\money.h"
#include "..\..\wwlib\ref_ptr.h"

class ButtonGadgetClass;		// unknown_675120.h's buttons (+0xD4)
class SlotDataGadgetClass;		// gd_slot_data.h's slot gadgets (the base's +0x88)
class RenderObjClass;
class SlotDataClass;			// slot_data.h's (the base's +0x98)
class InvDataClass;				// client_item_property_builder.h's (the base's 0x59FE10)

// ui_inv.cpp's slot window base (0xD0 bytes, constructor 0x59F3D0 over the slot windows' base 0x62DE80; tables 0xAF252C /
// 0xAF2520 / 0xAF2514), as far as ui_trade.cpp calls it
class Unknown59F3D0Class : public InterfaceClass	// NAME 1: neutral (D10)
{
public:
	Unknown59F3D0Class(SClient *client, int kind);		// 0x59F3D0 (the trade window passes 7 into +0xBC)  // NAME 1 [client, kind]: neutral
	virtual ~Unknown59F3D0Class();						// 0x59F940 (ui_mount.cpp's destructor calls it)
	virtual void Unknown_54();							// 0x59F470 (ui_mount.cpp's 0x5C74A0 calls it)
	virtual void Unknown_58();							// 0x62E7C0 (0x62DE80's class's; ui_mount.cpp's 0x5C7570 calls it)
	virtual void Unknown_5C();							// 0x59FD70 (ui_mount.cpp's 0x5C7600 calls it)
	virtual void Unknown_88(); virtual void Unknown_8C(); virtual void Unknown_90(); virtual void Unknown_94();
	virtual void Unknown_98(); virtual void Unknown_9C(); virtual void Unknown_A0(); virtual void Unknown_A4();
	virtual void Unknown_A8(); virtual void Unknown_AC(RenderObjClass *robj, int arg); virtual void Unknown_B0(); virtual void Unknown_B4();	// NAME 1 [robj, arg]: Unknown_AC's, neutral (ui_mount.cpp passes a button's sub-object and 0)
	virtual void Unknown_B8(); virtual void Unknown_BC(); virtual void Unknown_C0(); virtual void Unknown_C4();
	virtual void Unknown_C8();							// +0xC8 (the trade window calls it after setting +0xB4 to -1)
	virtual void Unknown_CC(); virtual void Unknown_D0();	// +0xCC, +0xD0 (the table's last two: 0x62E580, 0x62E5B0; ui_mount.cpp's own begin at +0xD4)
	InvDataClass *Unknown_59FE10();						// 0x59FE10 (SlotData's slot +0x10)
	void Unknown_5A00C0(int *count, std::string *name);	// 0x5A00C0 (the trade window's 0x652890: the count and the name of the buy)  // NAME 1 [count, name]: what 0x652890 prints
	void Unknown_5A02E0(bool arg);						// 0x5A02E0 (the trade window's 0x652FF0 passes true)  // NAME 1 [arg]: neutral

	char Unknown84[0x88 - 0x84];						// +0x84
	std::vector<SlotDataGadgetClass *> Unknown88;		// +0x88 (the slots' gadgets: the trade window's 0x652890 reads 0..4 as '!bttn')  // NAME 1: neutral (D10)
	SlotDataClass *SlotData;							// +0x98 (0x62DE80's class's FAIL_IF '!SlotData')  // NAME 3: the FAIL_IF's
	char Unknown9C[0xB4 - 0x9C];						// +0x9C
	int UnknownB4;										// +0xB4 (the trade window sets -1 before slot +0xC8)
	char UnknownB8[0xD0 - 0xB8];						// +0xB8 (+0xBC the constructor's kind)
};

// The trade window (0x190 bytes, tables 0xAF8F70 / 0xAF8F64 / 0xAF8F58; 'fa_ce02a.ini'; ui_talkt.cpp makes it)
class UITradeClass : public Unknown59F3D0Class  // NAME 3: class-from-file (D24) for ui_trade.cpp
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
