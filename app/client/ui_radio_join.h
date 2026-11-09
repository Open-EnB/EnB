// Earth & Beyond client (reconstruction): app\client\ui_radio_join.h, the private channel's join window (its model
// 'cht03', 'cht03.ini': 'Join Private Channel'). No string names the file: ui_radio_join.cpp and this header are
// provisional names (NAME 1, D137), after ui_radio_main.cpp's 'cht01' window in the client (CH1).
#ifndef UI_RADIO_JOIN_H  // NAME 1 [ui_radio_join.h]: provisional header name, after its title 'Join Private Channel'
#define UI_RADIO_JOIN_H

#include <string>
#include "ui_chat_panel_base.h"
#include "fontmanager.h"

class TextGadgetClass;

// The private channel's join window (0xBC bytes, tables 0xAF6C5C / 0xAF6C50 / 0xAF6C44)
class UIRadioJoinClass : public ChatPanelUIBase	// NAME 1: neutral, after its title 'Join Private Channel'
{
public:
	UIRadioJoinClass(SClient *client, int state);		// 0x5EAB40 (deleting 0x5EAB90)  // NAME 1 [client, state]: what the members hold
	virtual ~UIRadioJoinClass();						// 0x5EABC0
	virtual void Unknown_54();							// +0x54 0x5EAC50 ('cht03': the buttons 'EXIT', 'TEXT_FIELD', 'BUTTON01', 'BUTTON03', the text box 'TEXTFIELD_Cht03')
	virtual void Unknown_58();							// +0x58 0x5EADD0
	virtual void Unknown_5C();							// +0x5C 0x5EAEA0
	virtual void Unknown_60();							// +0x60 0x5EAEC0 (every frame: the text box; enter joins)
	virtual bool Unknown_68(int key);					// +0x68 0x5EAF30 (1 exit; 2 the text box; 3 cancel; 4 join)  // NAME 1 [key]: InterfaceClass's
	void Unknown_5EB0B0(RefCountPtr<FontSys> font, const char *text);	// 0x5EB0B0  // NAME 1 [font, text]: neutral
	void Unknown_5EB140();								// 0x5EB140 (the title and the message per state)
	void Unknown_5EB2F0();								// 0x5EB2F0 (chat request 7 for the channel left, 6 for the one typed)

	int State;											// +0xA0 (0 join, 1 no such channel)  // NAME 1: neutral (D10)
	TextGadgetClass *Entry;								// +0xA4 'TEXTFIELD_Cht03' (id 6)  // NAME 1: neutral (D10)
	bool UnknownA8;										// +0xA8 (set with the text box's focus)
	GadgetClass *Buttons[4];							// +0xAC 'EXIT', 'TEXT_FIELD', 'BUTTON01', 'BUTTON03'  // NAME 2: what they are
};

#endif // UI_RADIO_JOIN_H
