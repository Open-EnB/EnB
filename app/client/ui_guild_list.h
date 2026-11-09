// Earth & Beyond client (reconstruction): app\client\ui_guild_list.h, the guild list window (ui_guild_list.cpp's; both
// names provisional, NAME 1). Laid out from the constructor 0x81E030 and the destructor 0x81E1E0 (GL1, 2026-11-09).
#ifndef UI_GUILD_LIST_H  // NAME 1 [ui_guild_list.h]: provisional header name for ui_guild_list.cpp
#define UI_GUILD_LIST_H

#include <string>
#include <vector>
#include "ui_chat_panel.h"

class GadgetClass;			// gadget.cpp's
class GadgetSlider;			// gd_slider.h's
class FontSys;				// fontmanager.h's

// The guild list window ('UI_GUILD_LIST', 'tsi36a': 0xE8 bytes, constructor 0x81E030 over ui_chat_panel_base.cpp's
// 0x56D280, destructor 0x81E1E0, tables 0xB08304 / 0xB082F8 / 0xB082EC); the guild window's +0xBC
class UIGuildListClass : public ChatPanelUIBase	// NAME 1: neutral, after its tooltip section 'UI_GUILD_LIST'
{
public:
	UIGuildListClass(SClient *client);					// 0x81E030  // NAME 1 [client]: ChatPanelUIBase's
	virtual ~UIGuildListClass();						// 0x81E1E0 (deleting 0x81E1B0)
	virtual void Unknown_54();							// +0x54 0x81E3E0 ('tsi36a', 'fa_tsi36a.ini': the two buttons, the nine rows, the slider)
	virtual void Unknown_58();							// +0x58 0x81E5C0 (activated: the lines emptied, the buttons on)
	virtual void Unknown_5C();							// +0x5C 0x81E770 (deactivated: the buttons off, the lines emptied)
	virtual void Unknown_60();							// +0x60 0x81E860 (every frame: the slider)
	virtual bool Unknown_68(int key);					// +0x68 0x81E920 (the buttons 1-2: Unknown0A8 set)  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// +0x7C 0x81E970  // NAME 1 [view, arg]: InterfaceClass's
	void Unknown_81E9A0(int count);						// 0x81E9A0 (the slider over count lines, 9 shown)  // NAME 1 [count]: neutral
	void Unknown_81EA70();								// 0x81EA70 (Top from the slider's value; the rows refreshed when it moved)
	void Unknown_81EB10();								// 0x81EB10 (the rows from Top)
	void Unknown_81EB90();								// 0x81EB90 (the lines emptied)
	void Unknown_81EC50(bool on);						// 0x81EC50 (the buttons' slot +0x40)  // NAME 1 [on]: neutral
	void Unknown_81EC90(std::vector<std::string> lines);	// 0x81EC90 (the guild window's 0x81C060: lines added)  // NAME 1 [lines]: neutral
	void Unknown_81EEA0(std::vector<std::string> lines);	// 0x81EEA0 (the guild window's 0x81C1F0: the lines replaced, the buttons on)  // NAME 1 [lines]: neutral

	GadgetSlider *Slider;								// +0xA0  // NAME 1: as ui_friends.h's
	int Top;											// +0xA4 (the first row's index: the slider's value)  // NAME 1: as ui_friends.h's
	int Unknown0A8;										// +0xA8 (1 once a button is pressed; the guild window's slot +0x60: 1 or 2 closes it)
	std::vector<std::string> Unknown0AC;				// +0xAC the lines
	GadgetClass *Unknown0BC[2];							// +0xBC the buttons ('exit', 'button01')
	RefCountPtr<FontSys> Unknown0C4[9];					// +0xC4 the rows ('name%02d_TSI36a')
};

#endif // UI_GUILD_LIST_H
