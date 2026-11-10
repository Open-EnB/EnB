// Earth & Beyond client (reconstruction): app\client\ui_radio_list.h, the chat channel list window (its model 'cht02',
// 'cht02.ini': 'Public Channels', 'Channel Members'). No string names the file: ui_radio_list.cpp and this header are
// provisional names (NAME 1, D137), after ui_radio_entry.cpp's password window before it in the client (CW1).
#ifndef UI_RADIO_LIST_H  // NAME 1 [ui_radio_list.h]: provisional header name, after the window's lists
#define UI_RADIO_LIST_H

#include <string>
#include <vector>
#include <list>
#include "ui_chat_panel_base.h"
#include "fontmanager.h"

class GadgetSlider;

// The chat channel list window (0x110 bytes, tables 0xAF699C / 0xAF6990 / 0xAF6984)
class UIRadioListClass : public ChatPanelUIBase	// NAME 1: neutral, after the window's lists
{
public:
	UIRadioListClass(SClient *client, int state);		// 0x5E7500 (deleting 0x5E7680)  // NAME 1 [client, state]: what the members hold
	virtual ~UIRadioListClass();						// 0x5E76B0
	virtual void Unknown_54();							// +0x54 0x5E78B0 ('cht02': the buttons 'EXIT', 'BUTTON01', 'BUTTON02', the rows 'NAME00'-'NAME06')
	virtual void Unknown_58();							// +0x58 0x5E7AD0 (activated: the lists emptied, the list requested)
	virtual void Unknown_5C();							// +0x5C 0x5E7C80
	virtual void Unknown_60();							// +0x60 0x5E7DA0 (every frame: the slider, the rows under the pointer or selected lit)
	virtual bool Unknown_68(int key);					// +0x68 0x5E7E60 (1 exit; 2, 3 the buttons: 0x5E85F0; 5-11 the rows: 0x5E8580)  // NAME 1 [key]: InterfaceClass's
	void Unknown_5E7F90(int count);						// 0x5E7F90 (the slider over count rows, 7 shown)  // NAME 1 [count]: neutral
	void Unknown_5E8000();								// 0x5E8000 (Top from the slider's value; the rows refreshed when it moved)
	void Unknown_5E8040();								// 0x5E8040 (the rows from Top)
	void Unknown_5E80E0();								// 0x5E80E0 (Names emptied)
	void Unknown_5E81A0();								// 0x5E81A0 (Unknown0C0 emptied)
	void Unknown_5E8240();								// 0x5E8240 (Names rebuilt from Unknown0C0, sorted)
	void Unknown_5E84F0();								// 0x5E84F0 (the rows under the pointer or selected lit)
	void Unknown_5E8580(int index);						// 0x5E8580 (a row clicked: BUTTON01 on)  // NAME 1 [index]: neutral
	void Unknown_5E85F0(int key);						// 0x5E85F0 (the buttons per state: join the selected channel, private, OK)  // NAME 1 [key]: what 0x5E7E60 passes
	void Unknown_5E8860(bool on);						// 0x5E8860 (the buttons and rows' slot +0x40)  // NAME 1 [on]: GadgetClass's
	void Unknown_5E8930();								// 0x5E8930 (chat request 25 or 27 per state)
	void Unknown_5E8C20();								// 0x5E8C20 (the title and the buttons' texts per state)
	void Unknown_5E8E30(int state, std::vector<std::string> names);	// 0x5E8E30 (cpacket.cpp: CLIENT_CHAT_LIST 164, the list for state)  // NAME 1 [state, names]: neutral
	void Unknown_5E8FF0(int state, std::string name);	// 0x5E8FF0 (cpacket.cpp: CLIENT_CHAT_EVENT 165, one added)  // NAME 1 [state, name]: neutral
	void Unknown_5E9160(int state, std::string name);	// 0x5E9160 (cpacket.cpp: CLIENT_CHAT_EVENT 165, one removed)  // NAME 1 [state, name]: neutral

	int State;											// +0xA0 (0 the public channels, 1 the channel's members)  // NAME 1: neutral (D10)
	GadgetSlider *Slider;								// +0xA4  // NAME 1: neutral (D10)
	int Top;											// +0xA8 (the first row's index: the slider's value)  // NAME 1: neutral (D10)
	int Selection;										// +0xAC (-1: none)  // NAME 1: neutral (D10)
	std::vector<std::string> Names;						// +0xB0 (sorted: the rows)  // NAME 1: neutral (D10)
	std::list<std::string> Unknown0C0;					// +0xC0 (as the server sent them)
	GadgetClass *Buttons[3];							// +0xCC 'EXIT', 'BUTTON01', 'BUTTON02'  // NAME 2: what they are
	GadgetClass *Rows[7];								// +0xD8 'NAME00'-'NAME06'  // NAME 1: neutral (D10)
	RefCountPtr<FontSys> Texts[7];						// +0xF4 'name00_Cht02'-'name06_Cht02'  // NAME 1: neutral (D10)
};

#endif // UI_RADIO_LIST_H
