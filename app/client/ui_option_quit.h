// Earth & Beyond client (reconstruction): app\client\ui_option_quit.h, the options' quit page (its model 'opt02a',
// 'fa_opt02a.ini'; the tooltips' section 'UI_OPTION_QUIT'). No string names the file: ui_option_quit.cpp and this
// header are provisional names (NAME 1, D137: P-M1-2), after ui_option_main.cpp in the client (CP1).
#ifndef UI_OPTION_QUIT_H  // NAME 1 [ui_option_quit.h]: provisional header name
#define UI_OPTION_QUIT_H

#include "ui_chat_panel_base.h"

// The quit page (0xB8 bytes, tables 0xAF4814 / 0xAF4808 / 0xAF47FC): the chat panel's request 2
class UIOptionQuitClass : public ChatPanelUIBase	// NAME 1: neutral, after its tooltips' section 'UI_OPTION_QUIT'
{
public:
	UIOptionQuitClass(SClient *client, bool mode);		// 0x5CE700 (deleting 0x5CE770)  // NAME 1 [client, mode]: what it does with them
	virtual ~UIOptionQuitClass();						// 0x5CE7A0
	virtual void Unknown_54();							// +0x54 0x5CE830 ('opt02a': the five buttons, the texts; Escape, Q, C, L)
	virtual void Unknown_58();							// +0x58 0x5CE9C0 (the base's, then 0x5CECB0, when not Active)
	virtual void Unknown_5C();							// +0x5C 0x5CEAE0 (InterfaceClass's when Active)
	virtual bool Unknown_68(int key);					// +0x68 0x5CEB00 (1 exit; 2 cancel: the chat panel's page 1 unless Mode; 3-5 the options saved and SClient's 0x74BB50: desktop, character select, login)  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// +0x7C 0x5CEC30 (InterfaceClass's and 0x5CECB0 when the view changes)  // NAME 1 [view, arg]: InterfaceClass's
	void Unknown_5CEC70(bool on);						// 0x5CEC70 (every button's slot +0x40; nothing calls it)  // NAME 1 [on]: GadgetClass's
	void Unknown_5CECB0();								// 0x5CECB0 (the text 'Note_opt02a' hidden by SClient's 0x744B60; inlined into 0x5CE9C0)

	GadgetClass *Buttons[5];							// +0xA0 'exit', 'bttn03' (cancel), 'bttn00' (quit), 'bttn01', 'bttn02'  // NAME 2: what they are
	bool Mode;											// +0xB4 (the constructor's; false: cancel goes back to the chat panel's page 1)  // NAME 1: neutral (D10)
};

#endif // UI_OPTION_QUIT_H
