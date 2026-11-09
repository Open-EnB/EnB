// Earth & Beyond client (reconstruction): app\client\ui_radio_admin.h, the private channel's admin window (its model
// 'cht05', 'cht05.ini'; the tooltips' section 'UI_RADIO_ADMIN'). No string names the file: ui_radio_admin.cpp and this
// header are provisional names (NAME 1, D137), after the tooltips' section (the windows' UI_<file> pattern; CW1).
#ifndef UI_RADIO_ADMIN_H  // NAME 1 [ui_radio_admin.h]: provisional header name, after the tooltips' section 'UI_RADIO_ADMIN'
#define UI_RADIO_ADMIN_H

#include <string>
#include <vector>
#include <list>
#include "ui_chat_panel_base.h"
#include "fontmanager.h"

class GadgetSlider;

// The private channel's admin window (0x11C bytes, tables 0xAF66DC / 0xAF66D0 / 0xAF66C4)
class UIRadioAdminClass : public ChatPanelUIBase	// NAME 1: neutral, after its tooltips' section 'UI_RADIO_ADMIN'
{
public:
	UIRadioAdminClass(SClient *client);					// 0x5E4160 (deleting 0x5E42D0)  // NAME 1 [client]: as the other windows'
	virtual ~UIRadioAdminClass();						// 0x5E4300
	virtual void Unknown_54();							// +0x54 0x5E4500 ('cht05': the buttons 'EXIT', 'BUTTON01'-'BUTTON06', the rows 'NAME00'-'NAME06')
	virtual void Unknown_58();							// +0x58 0x5E4720 (activated: the lists emptied, the members requested)
	virtual void Unknown_5C();							// +0x5C 0x5E49A0
	virtual void Unknown_60();							// +0x60 0x5E4AC0 (every frame: the slider, the rows under the pointer or selected lit)
	virtual bool Unknown_68(int key);					// +0x68 0x5E4B80 (1, 7 exit; 2-6 the requests: 0x5E5360; 9-15 the rows: 0x5E5290)  // NAME 1 [key]: InterfaceClass's
	void Unknown_5E4C60(int count);						// 0x5E4C60 (the slider over count rows, 7 shown)  // NAME 1 [count]: neutral
	void Unknown_5E4CD0();								// 0x5E4CD0 (Top from the slider's value; the rows refreshed when it moved)
	void Unknown_5E4D10();								// 0x5E4D10 (the rows from Top)
	void Unknown_5E4DB0();								// 0x5E4DB0 (Names emptied)
	void Unknown_5E4E70();								// 0x5E4E70 (Unknown0BC emptied)
	void Unknown_5E4F10();								// 0x5E4F10 (Names rebuilt from Unknown0BC, sorted)
	void Unknown_5E5200();								// 0x5E5200 (the rows under the pointer or selected lit)
	void Unknown_5E5290(int index);						// 0x5E5290 (a row clicked: BUTTON01-BUTTON05 on when the panel's +0x224 is set)  // NAME 1 [index]: neutral
	void Unknown_5E5360(int key);						// 0x5E5360 (chat requests 16, 17, 14, 20, 18 with the channel and the selected member)  // NAME 1 [key]: what 0x5E4B80 passes
	void Unknown_5E5A40(bool on);						// 0x5E5A40 (the buttons and rows' slot +0x40)  // NAME 1 [on]: GadgetClass's
	void Unknown_5E5B10();								// 0x5E5B10 (chat request 27: the channel's members)
	void Unknown_5E5D70(std::vector<std::string> names);	// 0x5E5D70 (cpacket.cpp: CLIENT_CHAT_LIST 164 type 2)  // NAME 1 [names]: as cpacket.cpp's
	void Unknown_5E5EF0(std::string name);				// 0x5E5EF0 (cpacket.cpp: CLIENT_CHAT_EVENT 165, event 7: one added)  // NAME 1 [name]: as cpacket.cpp's
	void Unknown_5E6030(std::string name);				// 0x5E6030 (cpacket.cpp: CLIENT_CHAT_EVENT 165, events 8, 16, 18: one removed)  // NAME 1 [name]: as cpacket.cpp's

	GadgetSlider *Slider;								// +0xA0  // NAME 1: neutral (D10)
	int Top;											// +0xA4 (the first row's index: the slider's value)  // NAME 1: neutral (D10)
	int Selection;										// +0xA8 (-1: none)  // NAME 1: neutral (D10)
	std::vector<std::string> Names;						// +0xAC (sorted: the rows)  // NAME 1: neutral (D10)
	std::list<std::string> Unknown0BC;					// +0xBC (as the server sent them)
	GadgetClass *Buttons[7];							// +0xC8 'EXIT', 'BUTTON01'-'BUTTON06'  // NAME 2: what they are
	GadgetClass *Rows[7];								// +0xE4 'NAME00'-'NAME06'  // NAME 1: neutral (D10)
	RefCountPtr<FontSys> Texts[7];						// +0x100 'name00_Cht05'-'name06_Cht05'  // NAME 1: neutral (D10)
};

#endif // UI_RADIO_ADMIN_H
