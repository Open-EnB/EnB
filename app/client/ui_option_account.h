// Earth & Beyond client (reconstruction): app\client\ui_option_account.h, the options' account page (its model
// 'opt05a', 'fa_opt05a.ini'; the tooltips' section 'UI_OPTION_ACCOUNT'). No string names the file: ui_option_account.cpp
// and this header are provisional names (NAME 1, D137), between ui_mount.cpp and the 'opt06a' page in the client (MT1).
#ifndef UI_OPTION_ACCOUNT_H  // NAME 1 [ui_option_account.h]: provisional header name
#define UI_OPTION_ACCOUNT_H

#include "ui_chat_panel_base.h"

// The account page (0xAC bytes, tables 0xAF3FCC / 0xAF3FC0 / 0xAF3FB4)
class UIOptionAccountClass : public ChatPanelUIBase	// NAME 1: neutral, after its tooltips' section 'UI_OPTION_ACCOUNT'
{
public:
	UIOptionAccountClass(SClient *client, int mode);	// 0x5C8870 (deleting 0x5C88C0)  // NAME 1 [client, mode]: what it does with them
	virtual ~UIOptionAccountClass();					// 0x5C88F0
	virtual void Unknown_54();							// +0x54 0x5C8980 ('opt05a': the buttons 'exit' and 'ok', the texts, Escape)
	virtual void Unknown_58();							// +0x58 0x5C8A90 (the base's when not Active)
	virtual void Unknown_5C();							// +0x5C 0x5C8AB0 (InterfaceClass's when Active)
	virtual bool Unknown_68(int key);					// +0x68 0x5C8AD0 (1 exit; 2 ok: the chat panel's page 5 in mode 1, else 1)  // NAME 1 [key]: InterfaceClass's
	void Unknown_5C8B90();								// 0x5C8B90 (mode 1: 'title_opt05a' and the message to run EBConfig)

	GadgetClass *Buttons[2];							// +0xA0 'exit', 'ok'  // NAME 2: what they are
	int Mode;											// +0xA8 (the constructor's; 1: the screen resolution notice)  // NAME 1: neutral (D10)
};

#endif // UI_OPTION_ACCOUNT_H
