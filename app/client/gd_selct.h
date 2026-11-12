// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\gd_selct.h (name assumed from gd_selct.cpp, whose __FILE__ is
**	'e:\build\beta\G\app\Client\gd_selct.cpp'; the client holds no string of this header)
**
**	The quantity selector (inv_transaction.cpp makes one for a stack): a bar at the pointer from 0 to a maximum,
**	the number typed or picked with the pointer, shown in GD_SELECT_FONT; a click or Enter takes it, the second
**	button or Esc takes 0. Over gadget.h's InterfaceDeactivationGadgetClass (its rectangle reused in pixels from the
**	screen's centre). Table 0xAFADD4 (29 slots: +0x2C, +0x30 and +0x40 its own), constructor 0x66CC00, 0x6C bytes.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef GD_SELCT_H  // NAME 3 [gd_selct.h]: header name assumed from the client's gd_selct.cpp
#define GD_SELCT_H

#include "gadget.h"

class UIViewClass;
class StrEditClass;
class Rect3DClass;
class FontSys;

class SelectGadgetClass : public InterfaceDeactivationGadgetClass  // NAME 2: class-from-file (D24) for app\Client\gd_selct.cpp, after its GD_SELECT_FONT
{
public:
	SelectGadgetClass(UIViewClass *view, int max);		// 0x66CC00 (the base's 0, 0, 1, 1, 0x1FF; the edit, the four rectangles and the text made)  // NAME 1 [view]: what inv_transaction.cpp passes; NAME 1 [max]: what it holds
	virtual ~SelectGadgetClass();						// +0x00 0x66D080 (deleting 0x66D050)
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x2C 0x66D1E0 (the buttons and keys; the bar and the number follow the pointer or the typed number)  // NAME 1 [event, x, y, mask]: GadgetClass's
	virtual bool Unknown_30(float x, float y);			// +0x30 0x66D1B0 (every point while shown and not finished)  // NAME 1 [x, y]: GadgetClass's
	virtual void Unknown_40(bool on);					// +0x40 0x66D520 (G_ASSERT 'font' l.369: on, at the pointer, the number reset to Max; off, the models out of the scene)  // NAME 1 [on]: GadgetClass's

	bool Finished;										// +0x40 (a click or key let go: inv_transaction.cpp reads Value then)  // NAME 1: what slot +0x2C does with it
	bool Armed;											// +0x41 (the button or key that finishes went down on this gadget)  // NAME 1: what slot +0x2C does with it
	int Max;											// +0x44 (inv_transaction.cpp's stack count)  // NAME 1: what slot +0x40 does with it
	int Value;											// +0x48 (the number picked; 0 when cancelled)  // NAME 1: what inv_transaction.cpp reads
	StrEditClass *Edit;									// +0x4C (10 digits, flags 0xC4)  // NAME 1: its type
	FontSys *Text;										// +0x50 (GD_SELECT_FONT: the number)  // NAME 1: what it shows
	float TextX;										// +0x54 (the number's x: right of the bar)  // NAME 1: what slot +0x40 puts there
	Rect3DClass *Bar;									// +0x58 (green: the part picked)  // NAME 1: what slot +0x2C sizes
	Rect3DClass *Frame;									// +0x5C (light green, a pixel larger: the outline)  // NAME 1: what slot +0x40 puts there
	Rect3DClass *Back;									// +0x60 (dark: the whole range)  // NAME 1: what slot +0x40 puts there
	Rect3DClass *TextBack;								// +0x64 (dark: behind the number)  // NAME 1: what slot +0x40 puts there
	UIViewClass *View;									// +0x68 (slot +0x44 the mouse, +0x50 the scene)  // NAME 1: its type
};

#endif
