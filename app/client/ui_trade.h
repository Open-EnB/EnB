// Earth & Beyond client (reconstruction): app\client\ui_trade.h, the trade window (the client's ui_trade.cpp per its
// asserts, 'e:\build\beta\G\app\Client\ui_trade.cpp' at 0xB8BF74; this header's name is provisional, NAME 1, D137).
// Laid out from the constructor 0x650560, the destructor 0x6507C0 and the functions 0x651390-0x653A80 (TR1, 2026-11-09;
// O23, 2026-11-14).
#ifndef UI_TRADE_H  // NAME 1 [ui_trade.h]: provisional header name for ui_trade.cpp
#define UI_TRADE_H

#include <vector>
#include <string>
#include "pdaint.h"			// (before sclient.h: its PDAInterfaceClass; ui_inv.h's UIInvClass)
#include "sclient.h"
#include "..\shared\money.h"
#include "..\..\wwlib\ref_ptr.h"

class ButtonGadgetClass;		// unknown_675120.h's buttons (+0xD4)
class ProxyGadgetClass;			// gd_proxy.h's highlights (+0x108)
class Unknown669FB0Class;		// gd_inv_slider.h's (+0x160)
class SlotDataGadgetClass;		// gd_slot_data.h's slot gadgets (the base's +0x88)
class RenderObjClass;
class TextureClass;
class SlotDataClass;			// slot_data.h's (the base's +0x98)
class InvDataClass;				// inv_data.h's (the base's 0x59FE10; +0x150 the three inventories)

// The trade window (0x190 bytes, tables 0xAF8F70 / 0xAF8F64 / 0xAF8F58; 'fa_ce02a.ini'; ui_talkt.cpp makes it)
class UITradeClass : public UIInvClass  // NAME 3: class-from-file (D24) for ui_trade.cpp
{
public:
	UITradeClass(SClient *client);						// 0x650560 (G_ASSERT 'PDA' l.106)
	virtual ~UITradeClass();							// 0x6507C0 (deleting 0x650790)
	virtual void Unknown_54();							// +0x54 0x650A40 (FAIL_IFs l.137-199: the model, the highlights, the buttons, the slider)
	virtual void Unknown_58();							// +0x58 0x651390 (FAIL_IF '!PDA' l.247: the cargo window opened with it)
	virtual void Unknown_5C();							// +0x5C 0x651470 (the box closed, the cargo window closed, the shown slots emptied)
	virtual void Unknown_60();							// +0x60 0x651510 (every frame: the credits, the box's answer, the buttons)
	virtual bool Unknown_68(int key);					// +0x68 0x651830 (G_ASSERT 'false' l.466: the buttons' keys)  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_B0();							// +0xB0 0x6538E0 (every button's slot from UnknownA8 on, through +0x17C)
	virtual void Unknown_C8();							// +0xC8 0x651DB0 (the selection's highlight: +0x108's textures)
	virtual void Unknown_CC();							// +0xCC 0x651C30 (the categories again, then the base's)
	virtual void Unknown_D0();							// +0xD0 0x651D10 (the categories again, then the base's +0xCC)
	bool Unknown_651EC0();								// 0x651EC0 (the categories from the inventory's item types; inline in 0x651C30 / 0x651D10)
	void Unknown_651F60(int index);						// 0x651F60 (the category: clamped to the shown ones, -2 none)  // NAME 1 [index]: neutral
	void Unknown_652030();								// 0x652030 (the buy-back category -3: every item the three inventories hold)
	void Unknown_652360();								// 0x652360 (FAIL_IFs '!bttn' l.674, '!font' l.680: the six category buttons)
	void Unknown_652450(bool force);					// 0x652450 (FAIL_IF '!font' l.699: the credits text)  // NAME 1 [force]: neutral
	void Unknown_652670();								// 0x652670 (+0x150's three inventories, made once: types 1, 2, 3)
	void Unknown_652890();								// 0x652890 (FAIL_IFs '!bttn' l.729, '!sd_render' l.732, '!trade_render' l.735: the purchase asked, +0x174 1)
	void Unknown_652BA0(bool arg, bool significant);	// 0x652BA0 (the loss warning box, +0x174 3 with arg, else 2)  // NAME 1 [arg, significant]: neutral
	void Unknown_652C60();								// 0x652C60 (the whole stack's sale asked, +0x174 4)
	void Unknown_652E70();								// 0x652E70 (the 'BUY_CONFIRMATION' box closed, +0x174 0)
	bool Unknown_652EB0();								// 0x652EB0 (the cargo window's slot holds an item, or +0x174 is 1)
	void Unknown_652FF0();								// 0x652FF0 (the box's answer: by +0x174)
	void Unknown_6531F0(bool arg);						// 0x6531F0 (the sale's loss warning)  // NAME 1 [arg]: neutral
	void Unknown_653450();								// 0x653450 (FAIL_IFs '!button' l.911, '!sd_render' l.914, '!trade_render' l.917)
	void Unknown_6539A0();								// 0x6539A0 (the sort toggled; no caller: 0x651830 inlines it)
	void Unknown_653A80();								// 0x653A80 (the sort button's text and tooltip)
	RenderObjClass *Get_Unknown0D0() { return(InterfaceMesh.Peek()); }	// (inline: 0x652450's register)  // NAME 1: neutral (D10)
	void Close_Box();									// (inline, ui_trade.cpp: 0x652E70's body)  // NAME 1: describes what it does
	int Get_Shown(unsigned int index) { int slot = -1; if (index < Unknown17C.size()) slot = Unknown17C[index]; return(slot); }	// (inline in 0x6538E0)  // NAME 1: neutral (D10)
	int Find_Shown(int slot) { int pos = 0; for (int i = 0; i < (int)Unknown17C.size(); i++) { if (Unknown17C[i] == slot) { pos = i; break; } } return(pos); }	// (inline in 0x651DB0)  // NAME 1: neutral (D10)

	RefCountPtr<RenderObjClass> InterfaceMesh;			// +0xD0 (the window's model 'ce02a': the styles' object; FAIL_IF '!InterfaceMesh' l.137)  // NAME 3: the FAIL_IF's
	ButtonGadgetClass *Unknown0D4[13];					// +0xD4 (0..5 the category buttons, 6..9 buy, buy all, sell, sell all, 11 the sort)  // NAME 1: neutral (D10)
	ProxyGadgetClass *Unknown108[5];					// +0x108 (the slots' 'HIGHLIGHT%02d' proxies)  // NAME 1: neutral (D10)
	int Unknown11C;										// +0x11C (the inventory's item types last seen)  // NAME 1: neutral (D10)
	int Unknown120[6];									// +0x120 (each button's category: the index into 0xBAE854)  // NAME 1: neutral (D10)
	int Unknown138;										// +0x138 (the buttons shown)  // NAME 1: neutral (D10)
	int Unknown13C;										// +0x13C (the selected button: -2 none, -3 the buy-back)  // NAME 1: neutral (D10)
	int Unknown140;										// +0x140 (the cargo window's slot last seen, -1)  // NAME 1: neutral (D10)
	int Unknown144;										// +0x144
	MoneyClass Money;									// +0x148 (the credits shown; 0xFFFFFFFF without the field)  // NAME 2: what it holds
	std::vector<RefCountPtr<InvDataClass> > Unknown150;	// +0x150 (0x652670's three inventories)  // NAME 1: neutral (D10)
	Unknown669FB0Class *Slider;								// +0x160 (destroyed by the destructor)  // NAME 2: as the other windows'
	RefCountPtr<PDAInterfaceClass> PDA;					// +0x164 (0x653B90's, the client's +0x1284; G_ASSERT 'PDA' l.106)  // NAME 3: the assert's
	RefCountPtr<TextureClass> StandardBttnTexture;		// +0x168 (G_ASSERT 'StandardBttnTexture' l.190)  // NAME 3: the assert's
	RefCountPtr<TextureClass> SelectedBttnTexture;		// +0x16C ('I_blank_SEL.tga'; G_ASSERT 'SelectedBttnTexture' l.194)  // NAME 3: the assert's
	int Unknown170;										// +0x170 (the highlighted proxy, -1)  // NAME 1: neutral (D10)
	int Unknown174;										// +0x174 (the box shown: 0 none, 2 / 3 a loss warning, 4 the stack)  // NAME 1: neutral (D10)
	int Unknown178;										// +0x178 (frames the shown slots are rebuilt for: 3 on opening)  // NAME 1: neutral (D10)
	std::vector<int> Unknown17C;						// +0x17C (the slots shown, in order)  // NAME 1: neutral (D10)
	int Unknown18C;										// +0x18C
};

#endif // UI_TRADE_H
