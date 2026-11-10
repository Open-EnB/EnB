// Earth & Beyond client (reconstruction): app\client\ui_radio_entry.h, the private channel's password window (its
// model 'cht04', 'cht04.ini': 'Enter Password', 'Create Password'). The file name is the client's (ui_radio_entry.cpp's
// assert '!PasswordGadget || !ConfirmGadget'); the client holds no string of this header (NAME 3, D137).
#ifndef UI_RADIO_ENTRY_H  // NAME 3 [ui_radio_entry.h]: header name assumed from the client's ui_radio_entry.cpp
#define UI_RADIO_ENTRY_H

#include <string>
#include "ui_chat_panel_base.h"
#include "fontmanager.h"

class TextGadgetClass;

// The private channel's password window (0xC4 bytes, tables 0xAF683C / 0xAF6830 / 0xAF6824)
class UIRadioEntryClass : public ChatPanelUIBase	// NAME 3: class-from-file (D24) for app\client\ui_radio_entry.cpp
{
public:
	UIRadioEntryClass(SClient *client, int state);		// 0x5E6290 (deleting 0x5E6300)  // NAME 1 [client, state]: what the members hold
	virtual ~UIRadioEntryClass();						// 0x5E6330
	virtual void Unknown_54();							// +0x54 0x5E63C0 ('cht04': the buttons 'EXIT', 'TEXT_FIELD00', 'TEXT_FIELD01', 'BUTTON02', 'BUTTON03', the text boxes)
	virtual void Unknown_58();							// +0x58 0x5E6610
	virtual void Unknown_5C();							// +0x5C 0x5E66E0
	virtual void Unknown_60();							// +0x60 0x5E6700 (every frame: the text boxes; enter moves on, then sends)
	virtual bool Unknown_68(int key);					// +0x68 0x5E6840 (1 exit; 2, 3 the text boxes; 4 cancel; 5 OK: 0x5E6A20)  // NAME 1 [key]: InterfaceClass's
	void Unknown_5E6A20(int key);						// 0x5E6A20 (cancel or OK per state)  // NAME 1 [key]: what 0x5E6840 passes
	void Unknown_5E6B10(RefCountPtr<FontSys> font, const char *text);	// 0x5E6B10  // NAME 1 [font, text]: neutral
	void Unknown_5E6BA0();								// 0x5E6BA0 (the title, the message and the boxes' labels per state)
	void Unknown_5E6EC0();								// 0x5E6EC0 (chat request 6: the channel and the password typed)
	void Unknown_5E70A0();								// 0x5E70A0 (chat request 22: the new password, both boxes equal)

	int State;											// +0xA0 (0 the channel needs a password, 1 the password was wrong, 2 create one)  // NAME 1: neutral (D10)
	TextGadgetClass *PasswordGadget;					// +0xA4 'TEXTFIELD00_Cht04' (id 7)  // NAME 5: the assert's
	TextGadgetClass *ConfirmGadget;						// +0xA8 'TEXTFIELD01_Cht04' (id 8, state 2 only)  // NAME 5: the assert's
	bool UnknownAC;										// +0xAC (set with a text box's focus)
	GadgetClass *Buttons[5];							// +0xB0 'EXIT', 'TEXT_FIELD00', 'TEXT_FIELD01', 'BUTTON02', 'BUTTON03'  // NAME 2: what they are
};

#endif // UI_RADIO_ENTRY_H
