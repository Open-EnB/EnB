// Earth & Beyond client (reconstruction): app\client\unknown_6af220.h, the GM tools' player inventory window (its
// model 'tsi27a', 'fa_tsi27a.ini': 'Inventory', 'Remove', 'Remove All'). No string names the file (no asserts):
// unknown_6af220.cpp and this header are provisional names (NAME 1, D137).
#ifndef UNKNOWN_6AF220_H  // NAME 1 [unknown_6af220.h]: provisional header name
#define UNKNOWN_6AF220_H

#include <string>
#include <vector>
#include <map>
#include "ui_chat_panel_base.h"

class GadgetClass;			// gadget.cpp's
class GadgetSlider;			// gd_slider.h's
class FontSys;				// fontmanager.h's

// The player inventory window GM_EDIT_PLAYER_INVENTORY 234 opens (0x118 bytes; constructor 0x6AF220 over
// ui_chat_panel_base.cpp's 0x56D280, destructor 0x6AF810, tables 0xAFDC6C / 0xAFDC60 / 0xAFDC54)
class Unknown6AF220Class : public ChatPanelUIBase	// NAME 1: neutral (D10), as cpacket.cpp's handler 0x73F2E0 names it
{
public:
	Unknown6AF220Class(SClient *client);				// 0x6AF220  // NAME 1 [client]: ChatPanelUIBase's
	virtual ~Unknown6AF220Class();						// 0x6AF810 (deleting 0x6AF6A0)
	virtual void Unknown_54();							// +0x54 0x6AFB10 ('tsi27a', 'fa_tsi27a.ini': the four buttons, the seven rows, the title, the slider)
	virtual void Unknown_58();							// +0x58 0x6AFDE0 (activated: the list emptied, the buttons on)
	virtual void Unknown_5C();							// +0x5C 0x6AFF60 (deactivated: the buttons off, the list emptied)
	virtual void Unknown_60();							// +0x60 0x6B0040 (every frame: the slider, the rows lit)
	virtual bool Unknown_68(int key);					// +0x68 0x6B0090 (1 exit; 2 'Remove'; 3 'Remove All'; 6-12 the rows)  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// +0x7C 0x6B0290  // NAME 1 [view, arg]: InterfaceClass's
	void Unknown_6B02C0(int count);						// 0x6B02C0 (the slider over count rows, 7 shown; every caller expands it)  // NAME 1 [count]: neutral
	void Unknown_6B0340();								// 0x6B0340 (Top from the slider's value; the rows refreshed when it moved)
	bool Unknown_6B0390(unsigned int index);			// 0x6B0390 (no caller, expanded: the item at index is one of Names' keys, a heading)  // NAME 1 [index]: neutral
	void Unknown_6B0460();								// 0x6B0460 (the rows from Top: a heading shows Names' text and is off)
	void Unknown_6B05F0();								// 0x6B05F0 (no caller, expanded: the slider over the whole list, nothing selected, 'Remove' off)
	void Unknown_6B06C0();								// 0x6B06C0 (slot +0x60: the rows under the pointer, selected or headings lit)
	void Unknown_6B0820(int row);						// 0x6B0820 (no caller, slot +0x68 expands it: a row clicked; 'Remove' on unless it is a heading)  // NAME 1 [row]: neutral
	void Unknown_6B0940(bool on);						// 0x6B0940: the buttons and rows' slot +0x40; 'Remove' off while nothing is selected  // NAME 1 [on]: neutral
	void Unknown_6B09D0(int unknown, std::vector<std::string> *strings);	// 0x6B09D0 (cpacket.cpp's 0x73F2E0: the packet's +0x0C and its strings)  // NAME 1 [unknown, strings]: what the handler passes
	void Unknown_6B0C80();								// 0x6B0C80 (key 2: GM_DESTROY_ITEM for the selected item, the item taken off the list)
	void Unknown_6B0E90();								// 0x6B0E90 (key 3: GM_DESTROY_ITEM for all, the window closed)
	void Unknown_6B0F30(int unknown);					// 0x6B0F30 (no caller, expanded: 0x6B05F0, then the rows; its argument is not read: the client's 'ret 4')  // NAME 1 [unknown]: neutral
	void Unknown_6B1000();								// 0x6B1000 (no caller, expanded: the list emptied, +0x104 -1)

	std::map<std::string, std::string> Names;			// +0xA0 the headings' texts by their items ('InvShipEquipped' 'Equipped:', 'InvShipCargo' 'Cargo:', 'InvPlayerSecure' 'Vault:')  // NAME 1: neutral
	GadgetSlider *Slider;								// +0xB0 (the destructor's 0x66E460; 7 rows)  // NAME 1: neutral
	int Top;											// +0xB4 (the first row's index: the slider's value)  // NAME 1: neutral
	int Selected;										// +0xB8 (-1 from the constructor: the selected item's index)  // NAME 1: neutral
	GadgetClass *Buttons[4];							// +0xBC 'exit', 'button01' ('Remove'), 'button02' ('Remove All'), 'button03' (hidden)  // NAME 1: neutral
	GadgetClass *Rows[7];								// +0xCC the rows 'name%02d' (ids 6-12)  // NAME 1: neutral
	RefCountPtr<FontSys> RowTexts[7];					// +0xE8 the rows' texts 'name%02d_tsi27a' (eh vector constructor 0x553A80, destructor 0x51C630)  // NAME 1: neutral
	int Unknown104;										// +0x104 (0x6B09D0's first argument, GM_DESTROY_ITEM's +0x0C: the player)
	std::vector<std::string> Items;						// +0x108 the list (0x6B09D0 copies the packet's strings)  // NAME 1: neutral
};

#endif // UNKNOWN_6AF220_H
