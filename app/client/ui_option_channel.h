// Earth & Beyond client (reconstruction): app\client\ui_option_channel.h, the options' channels page (its model
// 'opt06a', 'fa_opt06a.ini'; the tooltips' section 'UI_OPTION_CHANNEL'). No string names the file: ui_option_channel.cpp
// and this header are provisional names (NAME 1, D137), after ui_option_account.cpp in the client (OP1).
#ifndef UI_OPTION_CHANNEL_H  // NAME 1 [ui_option_channel.h]: provisional header name
#define UI_OPTION_CHANNEL_H

#include <string>
#include "ui_chat_panel_base.h"

class CheckBoxGadgetClass;	// ui_manu.cpp's check button (0x7E8030: checked or not; +0x64)

// The channels page (0x104 bytes, tables 0xAF412C / 0xAF4120 / 0xAF4114)
class UIOptionChannelClass : public ChatPanelUIBase	// NAME 1: neutral, after its tooltips' section 'UI_OPTION_CHANNEL'
{
public:
	UIOptionChannelClass(SClient *client, bool mode);	// 0x5C8DB0 (deleting 0x5C8E30)  // NAME 1 [client, mode]: what it does with them
	virtual ~UIOptionChannelClass();					// 0x5C8E60
	virtual void Unknown_54();							// +0x54 0x5C8EF0 ('opt06a': the buttons 'exit', 'ok', 'cancel', the 21 checks, Escape)
	virtual void Unknown_58();							// +0x58 0x5C9070 (the checks from the options, then the base's, when not Active)
	virtual void Unknown_5C();							// +0x5C 0x5C9150 (InterfaceClass's when Active)
	virtual bool Unknown_68(int key);					// +0x68 0x5C9170 (1 exit; 2 ok: 0x5C93B0; 3 cancel; 5-25 the checks)  // NAME 1 [key]: InterfaceClass's
	void Unknown_5C92D0(bool on);						// 0x5C92D0 (every gadget's slot +0x40; nothing calls it)  // NAME 1 [on]: GadgetClass's
	void Unknown_5C9340();								// 0x5C9340 (the checks from the options; inlined into 0x5C9070)
	void Unknown_5C93B0();								// 0x5C93B0 (the checks into the options; a changed channel joined or left)
	void Unknown_5C9580(std::string channel);			// 0x5C9580 (chat request 6 with the channel and " ")  // NAME 1 [channel]: what 0x5C93B0 passes
	void Unknown_5C9740(std::string channel);			// 0x5C9740 (chat request 7 with the channel)  // NAME 1 [channel]: what 0x5C93B0 passes

	bool Mode;											// +0xA0 (the constructor's; false: ok and cancel go back to the chat panel's page 1)  // NAME 1: neutral (D10)
	GadgetClass *Buttons[3];							// +0xA4 'exit', 'ok', 'cancel'  // NAME 2: what they are
	CheckBoxGadgetClass *Checks[21];					// +0xB0 'check00'-'check20', one per channel option  // NAME 2: what they are
};

#endif // UI_OPTION_CHANNEL_H
