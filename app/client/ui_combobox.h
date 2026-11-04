// Earth & Beyond client (reconstruction): app\client\ui_combobox.h, the combo box (the client's ui_combobox.cpp per
// its __FILE__; this header's name is provisional). Moved out of ui_combobox.cpp with ui_chat_panel.h's stand-in
// UIChatComboClass, the same class (its Unknown_57AD50; P-TG4-1, 2026-11-03).
#ifndef UI_COMBOBOX_H  // NAME 3 [ui_combobox.h]: header name by convention (D24) from the client's ui_combobox.cpp
#define UI_COMBOBOX_H

#include <string>
#include <vector>
#include "sclient.h"

class GadgetClass;				// gadget.cpp's (ui_combobox.cpp declares it as far as it calls it)
class HAnimComboClass;			// hanim.h
class UIComboBoxSliderClass;	// gd_slider.cpp's slider, as ui_combobox.cpp calls it
class UIComboBoxUnknownBCClass;	// ui_combobox.cpp's

// The combo box, as far as ui_combobox.cpp's functions so far and the chat panel's combos (Combo[]) use it
class UIComboBoxClass : public InterfaceClass	// NAME 3: class-from-file (D24)
{
public:
	void Unknown_57A330();								// 0x57A330 (the drop-down's opening / closing per frame)
	void Unknown_57AAB0();								// 0x57AAB0 (the rows from the top item)
	void Unknown_57AC70(int index);						// 0x57AC70 (select by index)  // NAME 1 [index]: neutral
	void Unknown_57AD50(const char *text);				// 0x57AD50 (select by text: the first item it begins)  // NAME 1 [text]: neutral
	void Unknown_57B440();								// 0x57B440 (ui_chat_panel.cpp: slot +0x7C, before each combo's slot +0x7C)

	const char *Unknown_Item_Text(unsigned index) { return(index < Items.size() ? Items[index].c_str() : ""); }	// (inline: 0x57AC70, 0x57AD50)  // NAME 1: neutral (D10)
	void Unknown_Show_Selected();	// (inline: 0x57AC70, 0x57AD50; ui_combobox.cpp defines it: it reads the text field's gadget)  // NAME 1: neutral (D10)

	UIMeshClass *Unknown084;							// +0x84 (the box's mesh)
	char Unknown088[0xA0 - 0x88];						// +0x88
	HAnimComboClass *Unknown0A0;						// +0xA0 (the drop-down's animation)
	char Unknown0A4[0xB8 - 0xA4];						// +0xA4
	int Unknown0B8;										// +0xB8 (the rows)
	UIComboBoxUnknownBCClass *Unknown0BC;				// +0xBC
	int Unknown0C0;										// +0xC0
	int Unknown0C4;										// +0xC4
	int Unknown0C8;										// +0xC8 (the drop-down: 0 up, 1 opening, 2 closing, 3 down)
	float Unknown0CC;									// +0xCC (the rows shown)
	char Unknown0D0[0xD4 - 0xD0];						// +0xD0
	GadgetClass *Unknown0D4;							// +0xD4 (the text field)
	UIComboBoxSliderClass *Unknown0D8;					// +0xD8
	unsigned int Unknown0DC;							// +0xDC (the top item)
	int Selected;										// +0xE0  // NAME 2: what 0x57AC70 / 0x57AD50 set
	std::vector<std::string> Items;						// +0xE4  // NAME 2: what the rows show
	char Unknown0F4[0xF8 - 0xF4];						// +0xF4
	GadgetClass **Unknown0F8;							// +0xF8 (the rows)
	char Unknown0FC[0x10C - 0xFC];						// +0xFC
	volatile int Unknown10C;							// +0x10C (set: the text field is left alone; volatile: the client reads it after the store to Selected, 0x57AC70 / 0x57AD50)
};

#endif
