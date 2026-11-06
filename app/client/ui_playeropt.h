// Earth & Beyond client (reconstruction): app\client\ui_playeropt.h, the player options window (the client's
// ui_playeropt.cpp per its assert; this header's name is provisional, NAME 1). Laid out from the constructor 0x5D8C40
// and the destructor 0x5D8E90 (CP1, 2026-11-06); declared as far as the rebuilt functions use it.
#ifndef UI_PLAYEROPT_H  // NAME 1 [ui_playeropt.h]: provisional header name for ui_playeropt.cpp
#define UI_PLAYEROPT_H

#include <string>
#include <vector>
#include <list>
#include "ui_chat_panel.h"

class GadgetClass;			// gadget.cpp's
class GadgetSlider;			// gd_slider.h's
class FontSys;				// fontmanager.h's
class ClientObjectClass;	// co.h's
class UIPTradeCreditsClass;	// sclient.h's: the text entry (ui_ptrade.cpp's credits entry; here 'Ignore Player')

// ui_playeropt.cpp's player options window: the ignore list (0x114 bytes; constructor 0x5D8C40 over
// ui_chat_panel_base.cpp's 0x56D280, destructor 0x5D8E90, tables 0xAF58B0 / 0xAF58A4 / 0xAF5898)
class UIPlayerOptClass : public UIChatPanelBaseClass	// NAME 3: class-from-file (D24) for app\client\ui_playeropt.cpp
{
public:
	UIPlayerOptClass(SClient *client);					// 0x5D8C40  // NAME 1 [client]: UIChatPanelBaseClass's
	virtual ~UIPlayerOptClass();						// 0x5D8E90 (deleting 0x5D8E60)
	virtual void Unknown_54();							// +0x54 0x5D9100 ('tsi27a', 'fa_tsi27a.ini': the buttons, the rows, the slider, the 'Ignore Player' entry)
	virtual void Unknown_58();							// +0x58 0x5D9410 (activated: both lists emptied, request 0x17 sent, the buttons on)
	virtual void Unknown_5C();							// +0x5C 0x5D9610 (deactivated: the buttons off, both lists emptied)
	virtual void Unknown_60();							// +0x60 0x5D9730 (every frame)
	virtual bool Unknown_68(int key);					// +0x68 0x5D9870 (the buttons 1-4, the rows 6-12)  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// +0x7C 0x5D99F0 (the entry moved too)  // NAME 1 [view, arg]: InterfaceClass's
	void Unknown_5D9A40(int count);						// 0x5D9A40 (the slider over count rows, 7 shown)  // NAME 1 [count]: neutral
	void Unknown_5D9AC0();								// 0x5D9AC0 (Top from the slider's value; the rows refreshed when it moved)
	void Unknown_5D9B10();								// 0x5D9B10 (the rows from Top)
	void Unknown_5D9BB0();								// 0x5D9BB0 (the names emptied)
	void Unknown_5D9C70();								// 0x5D9C70 (the list emptied)
	void Unknown_5D9D10();								// 0x5D9D10 (the names rebuilt from the list, sorted; the selection cleared)
	void Unknown_5D9FD0();								// 0x5D9FD0 (the rows under the pointer or selected lit)
	void Unknown_5DA060(int index);						// 0x5DA060 (a row clicked)  // NAME 1 [index]: neutral
	void Unknown_5DA0D0();								// 0x5DA0D0 (the entry answered: request 10 with the name typed)
	void Unknown_5DA2B0(bool on);						// 0x5DA2B0 (the buttons and rows' slot +0x40; [2] off while nothing is selected)  // NAME 1 [on]: neutral
	void Unknown_5DA340();								// 0x5DA340 (FAIL_IF 'Selection < 0 || Selection >= list_size', line 395: request 11 with the selected name)
	void Unknown_5DA570();								// 0x5DA570 (request 0x17)
	void Unknown_5DA600(std::vector<std::string> names);	// 0x5DA600 (the list replaced)  // NAME 1 [names]: neutral
	void Unknown_5DA780(std::string name);				// 0x5DA780 (a name added)  // NAME 1 [name]: neutral
	void Unknown_5DA8C0(std::string name);				// 0x5DA8C0 (a name removed)  // NAME 1 [name]: neutral
	void Unknown_5DAAD0();								// 0x5DAAD0 ([3] on with a player target)
	void Unknown_5DAB50();								// 0x5DAB50 (request 10 with the target's name, when the target is a player)
	ClientObjectClass *Unknown_5DAD50();				// 0x5DAD50 (the main view's target when it is a player, else NULL)

	GadgetSlider *Slider;								// +0xA0 (0xE)  // NAME 1: neutral
	int Top;											// +0xA4 (the first row's index: the slider's value)  // NAME 1: as ui_friends.h's
	int Selection;										// +0xA8 (-1 from the constructor)  // NAME 3: the assert's
	RefCountPtr<UIPTradeCreditsClass> Unknown0AC;		// +0xAC ('Ignore Player')
	std::vector<std::string> Unknown0B0;				// +0xB0 the names, sorted (the assert's 'list_size')
	std::list<std::string> Unknown0C0;					// +0xC0 the names as received
	GadgetClass *Unknown0CC[4];							// +0xCC the buttons ('exit', 'button01' .. 'button03', ids 1-4)
	GadgetClass *Unknown0DC[7];							// +0xDC the rows ('name%02d', ids 6-12)
	RefCountPtr<FontSys> Unknown0F8[7];					// +0xF8 the rows' names ('name%02d_tsi27a')
};

#endif // UI_PLAYEROPT_H
