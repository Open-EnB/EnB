// Earth & Beyond client (reconstruction): app\client\gd_inv_slider.h, the inventory windows' slider (the client's
// gd_inv_slider.cpp per its asserts, 'e:\build\beta\G\app\Client\gd_inv_slider.cpp' at 0xB8D080; this header's name is
// provisional, NAME 1, D137). Laid out from the constructor 0x669FB0 and 0x66A300 (PW1, 2026-11-09); not rebuilt yet.
#ifndef GD_INV_SLIDER_H  // NAME 1 [gd_inv_slider.h]: provisional header name for gd_inv_slider.cpp
#define GD_INV_SLIDER_H

#include "gd_slider.h"		// GadgetSlider

class UIInvClass;			// ui_inv.h's inventory window
class SlotDataClass;		// slot_data.h's

// The inventory slider (0x13C bytes, table 0xAFA3F4: the destructor 0x66A160 and slot +0xA4 0x66A2B0 its own; the cargo,
// vault, trade and inventory windows make it)
class Unknown669FB0Class : public GadgetSlider	// NAME 1: neutral (D10)
{
public:
	Unknown669FB0Class(UIInvClass *inventory, LayerClass *layer);	// 0x669FB0 (FAIL_IFs '!Inventory' l.34, '!SlotData' l.37)  // NAME 3 [inventory]: the FAIL_IF's; NAME 1 [layer]: GadgetSlider's
	virtual ~Unknown669FB0Class();						// +0x00 0x66A190 (deleting 0x66A160)
	virtual void Unknown_A4(float val, bool arg2);		// +0xA4 0x66A2B0  // NAME 1 [val, arg2]: GadgetSlider's
	void Unknown_66A300(int a, int b);					// 0x66A300 (FAIL_IFs '!Inventory' l.90, '!Is_Attached()' l.91: +0x130 / +0x134 by the slider's direction)  // NAME 1 [a, b]: neutral

	UIInvClass *Inventory;								// +0x128 (FAIL_IF '!Inventory')  // NAME 3: the FAIL_IF's
	SlotDataClass *SlotData;							// +0x12C (referenced: the inventory's +0x98; FAIL_IF '!SlotData')  // NAME 3: the FAIL_IF's
	int Unknown130;										// +0x130
	int Unknown134;										// +0x134
	int Unknown138;										// +0x138
};

#endif // GD_INV_SLIDER_H
