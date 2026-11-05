// Earth & Beyond client (reconstruction): app\client\ui_buff.h, the buff display (the client's ui_buff.cpp per its
// asserts, 'e:\build\beta\G\app\Client\ui_buff.cpp'; this header's name is provisional, NAME 1). Laid out from the
// constructor 0x5608D0 and the destructor 0x560A10.
#ifndef UI_BUFF_H  // NAME 1 [ui_buff.h]: provisional header name for ui_buff.cpp
#define UI_BUFF_H

#include "sclient.h"

class UIBuffGadgetClass;	// gd_buffgadg.cpp's gadget (ui_buff.cpp declares it as far as it calls it)
struct UIBuffInfoStruct;	// gd_buffgadg.cpp's buff values (the same)

// The buff display (0xEC bytes, tables 0xAEFDD0 / 0xAEFDC4 / 0xAEFDB8; the main view's +0x1D4, 0x50C690 makes it, and
// 0x69B5E0): a 'TSI02A' mesh, the strings of 'fc_buff.ini' and a gadget per slot of the ship's BuffArray
class UIBuffClass : public Unknown5BB320Class	// NAME 1: from the client's file ui_buff.cpp
{
public:
	enum { MAX_BUFFS = 15 };	// NAME 1: the ship's BuffArray's count (the loops' 15)

	UIBuffClass(UIViewClass *view, const LayerClass &layer);	// 0x5608D0 (0xEC bytes; slot 0x5609E0)  // NAME 1 [view, layer]: the base 0x5BB250's (0x50C690 passes the main view)
	virtual ~UIBuffClass();								// 0x560A10
	virtual void Unknown_58();							// 0x560B30 (the base's, once: Unknown_AC)
	virtual void Unknown_5C();							// 0x560B60 (the base's, once)
	virtual void Unknown_60();							// 0x560B90 (0x561030, then the base's)

	void Unknown_560BC0(const UIBuffInfoStruct &info, int slot);	// 0x560BC0 (the slot's gadget updated)  // NAME 1 [info]: neutral; NAME 3 [slot]: the asserts' 'BuffTable[slot]'
	void Unknown_560D20(const char *type, const UIBuffInfoStruct &info, int slot);	// 0x560D20 (the slot's gadget made: 'buff%02d')  // NAME 1 [type, info]: neutral; NAME 3 [slot]: the asserts'
	void Unknown_560FA0(int slot);						// 0x560FA0 (the slot's gadget taken off; inlined into the others)  // NAME 3 [slot]: the asserts'
	void Unknown_561B40(bool good);					// 0x561B40 (click1c.wav for a good buff, else cost.wav; inlined into the others)  // NAME 1 [good]: what it tells (BuffDefStruct::IsGoodBuff)
	void Unknown_561030();								// 0x561030 (the ship's BuffArray to the slots; 'frame' hidden without buffs)

	int UnknownAC;										// +0xAC: 1 while slot +0x58 has run (slot +0x5C clears it)
	UIBuffGadgetClass *BuffTable[MAX_BUFFS];			// +0xB0  // NAME 3: the asserts' 'BuffTable[slot]'
};

#endif // UI_BUFF_H
