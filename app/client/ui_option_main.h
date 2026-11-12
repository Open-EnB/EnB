// Earth & Beyond client (reconstruction): app\client\ui_option_main.h, the options' main page (its model 'opt01a',
// 'fa_opt01a.ini'; the tooltips' section 'UI_OPTION_MAIN'). No string names the file: ui_option_main.cpp and this
// header are provisional names (NAME 1, D137: P-M1-2), after ui_option_font.cpp in the client (CP1).
#ifndef UI_OPTION_MAIN_H  // NAME 1 [ui_option_main.h]: provisional header name
#define UI_OPTION_MAIN_H

#include "ui_chat_panel_base.h"

// The main page (0xC0 bytes, tables 0xAF46B4 / 0xAF46A8 / 0xAF469C): the chat panel's request 1 (the HUD's options button)
class UIOptionMainClass : public ChatPanelUIBase	// NAME 1: neutral, after its tooltips' section 'UI_OPTION_MAIN'
{
public:
	UIOptionMainClass(SClient *client);					// 0x5CE240 (deleting 0x5CE290)
	virtual ~UIOptionMainClass();						// 0x5CE2C0
	virtual void Unknown_54();							// +0x54 0x5CE350 ('opt01a': the buttons 'exit', 'bttn00'-'bttn06', the texts, Escape)
	virtual void Unknown_58();							// +0x58 0x5CE460 (the base's when not Active)
	virtual void Unknown_5C();							// +0x5C 0x5CE480 (InterfaceClass's when Active)
	virtual bool Unknown_68(int key);					// +0x68 0x5CE4A0 (1 exit: the options saved; 2-8 the chat panel's pages 4, 8, 7, 5, 6, 11, 12)  // NAME 1 [key]: InterfaceClass's
	void Unknown_5CE670(bool on);						// 0x5CE670 (every button's slot +0x40; nothing calls it)  // NAME 1 [on]: GadgetClass's

	GadgetClass *Buttons[8];							// +0xA0 'exit', then the pages: camera, control, interface, graphics, sound, credits, fonts  // NAME 2: what they are
};

#endif // UI_OPTION_MAIN_H
