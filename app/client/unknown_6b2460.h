// Earth & Beyond client (reconstruction): app\client\unknown_6b2460.h, the GM tools' mission log window (its
// model 'tsi27a', 'fa_tsi27a.ini': 'Mission Log', 'Reset', 'Reset All'). No string names the file (no asserts):
// unknown_6b2460.cpp and this header are provisional names (NAME 1, D137).
#ifndef UNKNOWN_6B2460_H  // NAME 1 [unknown_6b2460.h]: provisional header name
#define UNKNOWN_6B2460_H

#include <string>
#include <vector>
#include "ui_chat_panel_base.h"

class GadgetClass;			// gadget.cpp's
class GadgetSlider;			// gd_slider.h's
class FontSys;				// fontmanager.h's

// The mission log window GM_VIEW_MISSION_LOG 235 opens (0x108 bytes; constructor 0x6B2460 over
// ui_chat_panel_base.cpp's 0x56D280, destructor 0x6B25D0, tables 0xAFDDCC / 0xAFDDC0 / 0xAFDDB4)
class Unknown6B2460Class : public ChatPanelUIBase	// NAME 1: neutral (D10), as cpacket.cpp's handler 0x73F1F0 names it
{
public:
	Unknown6B2460Class(SClient *client);				// 0x6B2460  // NAME 1 [client]: ChatPanelUIBase's
	virtual ~Unknown6B2460Class();						// 0x6B25D0 (deleting 0x6B25A0)
	virtual void Unknown_54();							// +0x54 0x6B27E0 ('tsi27a', 'fa_tsi27a.ini': the four buttons, the seven rows, the title, the slider)
	virtual void Unknown_58();							// +0x58 0x6B2AB0 (activated: the list emptied, the buttons on)
	virtual void Unknown_5C();							// +0x5C 0x6B2C30 (deactivated: the buttons off, the list emptied)
	virtual void Unknown_60();							// +0x60 0x6B2D10 (every frame: the slider, the rows lit)
	virtual bool Unknown_68(int key);					// +0x68 0x6B2DF0 (1 exit; 2 'Reset'; 3 'Reset All'; 6-12 the rows)  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// +0x7C 0x6B2F30  // NAME 1 [view, arg]: InterfaceClass's
	void Unknown_6B2F60(int count);						// 0x6B2F60 (no caller, expanded: the slider over count rows, 7 shown)  // NAME 1 [count]: neutral
	void Unknown_6B2FE0();								// 0x6B2FE0 (no caller, expanded: Top from the slider's value; the rows refreshed when it moved)
	void Unknown_6B3030();								// 0x6B3030 (the rows from Top)
	void Unknown_6B30D0();								// 0x6B30D0 (no caller, expanded: the slider over the whole list, nothing selected, 'Reset' off)
	void Unknown_6B31A0();								// 0x6B31A0 (no caller, slot +0x60 expands it: the rows under the pointer or selected lit)
	void Unknown_6B3240(int row);						// 0x6B3240 (no caller, slot +0x68 expands it: a row clicked, 'Reset' on)  // NAME 1 [row]: neutral
	void Unknown_6B32B0(bool on);						// 0x6B32B0: the buttons and rows' slot +0x40; 'Reset' off while nothing is selected  // NAME 1 [on]: neutral
	void Unknown_6B3340(int unknown, std::vector<std::string> *strings);	// 0x6B3340 (cpacket.cpp's 0x73F1F0: the packet's +0x0C and its strings)  // NAME 1 [unknown, strings]: what the handler passes
	void Unknown_6B35F0();								// 0x6B35F0 (key 2: GM_RESET_MISSION for the selected mission, the mission taken off the list)
	void Unknown_6B3810();								// 0x6B3810 (key 3: GM_RESET_MISSION for all, the window closed)
	void Unknown_6B38B0(int unknown);					// 0x6B38B0 (no caller, expanded: 0x6B30D0, then the rows; its argument is not read: the client's 'ret 4')  // NAME 1 [unknown]: neutral
	void Unknown_6B3980();								// 0x6B3980 (no caller, expanded: the list emptied, +0xF4 -1)

	GadgetSlider *Slider;								// +0xA0 (the destructor's 0x66E460; 7 rows)  // NAME 1: neutral
	int Top;											// +0xA4 (the first row's index: the slider's value)  // NAME 1: neutral
	int Selected;										// +0xA8 (-1 from the constructor: Top plus the clicked row)  // NAME 1: neutral
	GadgetClass *Buttons[4];							// +0xAC 'exit', 'button01' ('Reset'), 'button02' ('Reset All'), 'button03' (hidden)  // NAME 1: neutral
	GadgetClass *Rows[7];								// +0xBC the rows 'name%02d' (ids 6-12)  // NAME 1: neutral
	RefCountPtr<FontSys> RowTexts[7];					// +0xD8 the rows' texts 'name%02d_tsi27a' (eh vector constructor 0x553A80, destructor 0x51C630)  // NAME 1: neutral
	int UnknownF4;										// +0xF4 (0x6B3340's first argument, GM_RESET_MISSION's first: the player)
	std::vector<std::string> Items;						// +0xF8 the list (0x6B3340 copies the packet's strings)  // NAME 1: neutral
};

#endif // UNKNOWN_6B2460_H
