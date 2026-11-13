// Earth & Beyond client (reconstruction): app\client\ui_option_check.h, the options' check pages (the model 'opt03a'
// with 'fa_opt03a-chat.ini' or 'fa_opt03a-camera.ini'; the tooltips' section 'UI_OPTION_CHECK'). No string names the
// file: ui_option_check.cpp and this header are provisional names (NAME 1, D137), after ui_option_channel.cpp (OP1).
#ifndef UI_OPTION_CHECK_H  // NAME 1 [ui_option_check.h]: provisional header name
#define UI_OPTION_CHECK_H

#include "ui_chat_panel_base.h"

class CheckBoxGadgetClass;	// ui_manu.cpp's check button (0x7E8030: checked or not; +0x64)

// The check pages (0xE4 bytes, tables 0xAF428C / 0xAF4280 / 0xAF4274): the chat messages (page 0) or the cameras (page 1)
class UIOptionCheckClass : public ChatPanelUIBase	// NAME 1: neutral, after its tooltips' section 'UI_OPTION_CHECK'
{
public:
	UIOptionCheckClass(SClient *client, int page, bool mode);	// 0x5C9910 (deleting 0x5C9990)  // NAME 1 [client, page, mode]: what it does with them
	virtual ~UIOptionCheckClass();						// 0x5C99C0
	virtual void Unknown_54();							// +0x54 0x5C9A50 ('opt03a': the buttons 'exit', 'ok', 'cancel', the 12 checks, Escape)
	virtual void Unknown_58();							// +0x58 0x5C9C00 (the checks from the options, then the base's, when not Active)
	virtual void Unknown_5C();							// +0x5C 0x5C9CE0 (InterfaceClass's when Active)
	virtual bool Unknown_68(int key);					// +0x68 0x5C9D00 (1 exit; 2 ok: 0x5C9F20; 3 cancel; 5-16 the checks)  // NAME 1 [key]: InterfaceClass's
	void Unknown_5C9E40(bool on);						// 0x5C9E40 (every gadget's slot +0x40; nothing calls it)  // NAME 1 [on]: GadgetClass's
	void Unknown_5C9EB0();								// 0x5C9EB0 (the checks from the options; inlined into 0x5C9C00)
	void Unknown_5C9F20();								// 0x5C9F20 (the checks into the options, saved; page 0: the chat panel's 0x5689D0)

	int Page;											// +0xA0 (the constructor's; 0 chat, 1 camera: the ini, the texts and the options)  // NAME 1: neutral (D10)
	bool Mode;											// +0xA4 (the constructor's; false: ok and cancel go back to the chat panel's page 1)  // NAME 1: neutral (D10)
	GadgetClass *Buttons[3];							// +0xA8 'exit', 'ok', 'cancel'  // NAME 2: what they are
	CheckBoxGadgetClass *Checks[12];					// +0xB4 'check00'-'check11', one per option  // NAME 2: what they are
};

#endif // UI_OPTION_CHECK_H
