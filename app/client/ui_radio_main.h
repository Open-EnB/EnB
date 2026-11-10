// Earth & Beyond client (reconstruction): app\client\ui_radio_main.h, the chat channel window (its model 'cht01',
// 'cht01.ini'; the tooltips' section 'UI_RADIO_MAIN'). No string names the file: ui_radio_main.cpp and this header are
// provisional names (NAME 1, D137), after ui_radio_entry.cpp's password window in the client (CH1).
#ifndef UI_RADIO_MAIN_H  // NAME 1 [ui_radio_main.h]: provisional header name, after the tooltips' section 'UI_RADIO_MAIN'
#define UI_RADIO_MAIN_H

#include <string>
#include "ui_chat_panel_base.h"
#include "fontmanager.h"

// The chat channel window (0xC4 bytes, tables 0xAF6AFC / 0xAF6AF0 / 0xAF6AE4)
class UIRadioMainClass : public ChatPanelUIBase	// NAME 1: neutral, after its tooltips' section 'UI_RADIO_MAIN'
{
public:
	UIRadioMainClass(SClient *client, int state, std::string channel);	// 0x5E9410 (deleting 0x5E95D0)  // NAME 1 [client, state, channel]: what the members hold
	virtual ~UIRadioMainClass();						// 0x5E9600
	virtual void Unknown_54();							// +0x54 0x5E96F0 ('cht01': the buttons 'EXIT', 'BUTTON01'-'BUTTON03', then the texts)
	virtual void Unknown_58();							// +0x58 0x5E97C0
	virtual void Unknown_5C();							// +0x5C 0x5E9860
	virtual bool Unknown_68(int key);					// +0x68 0x5E9890 (1 exit; 2-4 the buttons: 0x5E9910)  // NAME 1 [key]: InterfaceClass's
	void Unknown_5E9910(int key);						// 0x5E9910 (a button per state: leave, create, the other windows)  // NAME 1 [key]: what 0x5E9890 passes
	void Unknown_5E9AD0(RefCountPtr<FontSys> font, const char *text, int id);	// 0x5E9AD0 (the text, and button id's tooltip, 'EMPTY' for none)  // NAME 1 [font, text, id]: neutral
	void Unknown_5E9BB0();								// 0x5E9BB0 (the title, the message and the buttons' texts per state)
	void Unknown_5EA260(bool on);						// 0x5EA260 (the buttons' slot +0x40; 'BUTTON01' / 'BUTTON03' hidden when unused)  // NAME 1 [on]: GadgetClass's
	void Unknown_5EA370();								// 0x5EA370 (chat request 4: a new channel)
	void Unknown_5EA550();								// 0x5EA550 (chat request 21 with 'none' and the channel)
	void Unknown_5EA7F0();								// 0x5EA7F0 (chat request 7: the channel left)

	int State;											// +0xA0 (0 the channel or none, 1 join, 2 none to join, 3 created)  // NAME 1: neutral (D10)
	std::string Channel;								// +0xA4  // NAME 1: what 0x5E9BB0 prints with it
	GadgetClass *Buttons[4];							// +0xB4 'EXIT', 'BUTTON01'-'BUTTON03'  // NAME 2: what they are
};

#endif // UI_RADIO_MAIN_H
