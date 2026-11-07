// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\gd_slot_data.h (name assumed from gd_slot_data.cpp, whose
**	__FILE__ is 0xB8D684; the client holds no string of this header)
**
**	gd_slot_data.cpp's gadgets: a slot on a screen, over a model gadget without a scene (Unknown_675120Class),
**	drawn by its SlotRender (slot_data_render.cpp) and reporting its events to its SlotIFace (ui_slot_data.cpp's
**	screen). SlotDataGadgetClass: table 0xAFB0CC, 0x6C bytes, constructor 0x670650; Unknown_670E30Class adds a
**	shortcut bar's hold on the slot: table 0xAFB1A4, 0x70 bytes, constructor 0x670E30. Notes in gd_slot_data.cpp.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef GD_SLOT_DATA_H  // NAME 3 [gd_slot_data.h]: header name assumed from the client's gd_slot_data.cpp
#define GD_SLOT_DATA_H

#include "gd_robj.h"
#include "unknown_675120.h"
#include "slot_data_render.h"
#include <string>

class SlotDataGadgetClass;
class Unknown_670E30Class;
class UIScutBarClass;

// What a slot reports to (ui_slot_data.cpp's screen, not in the tree yet): one slot used
class SlotDataIFaceClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(); virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C();
	virtual void Unknown_50(); virtual void Unknown_54(); virtual void Unknown_58(); virtual void Unknown_5C();
	virtual void Unknown_60(); virtual void Unknown_64(); virtual void Unknown_68(); virtual void Unknown_6C();
	virtual void Unknown_70(); virtual void Unknown_74(); virtual void Unknown_78(); virtual void Unknown_7C();
	virtual void Unknown_80(); virtual void Unknown_84(); virtual void Unknown_88(); virtual void Unknown_8C();
	virtual void Unknown_90(); virtual void Unknown_94();
	virtual void Unknown_98(SlotDataGadgetClass *gadget, KernelEventClass *event, unsigned int mask);	// +0x98 (0x670B20)  // NAME 1 [gadget, event, mask]: what it passes
};

class SlotDataGadgetClass : public Unknown_675120Class  // NAME 3: class-from-file (D24) for app\Client\gd_slot_data.cpp
{
public:
	SlotDataGadgetClass(SlotDataIFaceClass *slot_iface, SlotDataRenderClass *slot_render);	// 0x670650 (mask 0xE55)  // NAME 2 [slot_iface, slot_render]: the asserts' members; ui_slot_data.cpp's '!slot_render'
	virtual ~SlotDataGadgetClass();										// +0x00 0x6707B0 (deleting 0x670780)
	virtual int Unknown_10() { return((int)this); }						// +0x10 0x670740 (itself; the base's slot is an int)
	virtual void Unknown_20(int arg);									// +0x20 0x670990 (the tooltip from the render)  // NAME 1 [arg]: the base's
	virtual RenderObjClass *Unknown_24(Vector3 *position, float *frame, bool indicator);	// +0x24 0x670CF0
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x2C 0x670B20 (highlight, press, report to SlotIFace)
	virtual bool Unknown_30(float x, float y);							// +0x30 0x670AF0
	virtual void Unknown_40(bool on);									// +0x40 0x670AB0
	virtual void Unknown_90(class TextureClass *texture);				// +0x90 0x670D20 (G_ASSERT 'false')
	virtual Vector3 Unknown_9C();										// +0x9C 0x670D50 (the base's, darkened when the render says so)
	virtual Vector3 Unknown_A0();										// +0xA0 0x670DE0
	virtual Unknown_670E30Class *Unknown_A4() { return(NULL); }			// +0xA4 0x670760
	virtual void Unknown_A8(bool arg);									// +0xA8 0x670CB0 (the render's +0x2C)  // NAME 1 [arg]: neutral
	virtual void Unknown_AC(int slot);									// +0xAC 0x670890 (the render's +0x40, the tooltip)  // NAME 1 [slot]: as the render's

	int Unknown_670910();												// 0x670910 (the render's +0x20, or -1)
	int Unknown_670950();												// 0x670950 (the render's +0x08, or 0)
	void Unknown_670C90(bool on);										// 0x670C90 (Unknown68)  // NAME 1 [on]: neutral

	SlotDataIFaceClass *SlotIFace;										// +0x60  // NAME 3: the asserts'
	SlotDataRenderClass *SlotRender;									// +0x64 (referenced)  // NAME 3: the asserts'
	bool Unknown68;														// +0x68 (true; 0x670C90: the report to SlotIFace on or off)
};

class Unknown_670E30Class : public SlotDataGadgetClass  // NAME 1: neutral (D10)
{
public:
	Unknown_670E30Class(SlotDataIFaceClass *slot_iface, SlotDataRenderClass *slot_render, LayerClass *layer);	// 0x670E30  // NAME 1 [layer]: what slot +0x28 takes
	virtual ~Unknown_670E30Class() {}									// +0x00 0x670F70 (deleting 0x670F40)
	virtual void Unknown_04();											// +0x04 0x671050 (off its shortcut bar)
	virtual void Unknown_7C(int arg);									// +0x7C 0x671090 (ShortcutOwner set)  // NAME 1 [arg]: the base's
	virtual void Unknown_80(int arg);									// +0x80 0x671110 (ShortcutOwner cleared)  // NAME 1 [arg]: the base's
	virtual void Unknown_84(int arg);									// +0x84 0x671150 ('scut_gadg': its tooltip from the render)  // NAME 1 [arg]: the base's
	virtual bool Unknown_88() { return(ShortcutOwner != NULL); }		// +0x88 0x670F20
	virtual Unknown_670E30Class *Unknown_A4() { return(this); }			// +0xA4 0x670F00

	UIScutBarClass *ShortcutOwner;										// +0x6C  // NAME 3: the asserts'
};

#endif
