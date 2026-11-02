// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\tooltip.h (name assumed from tooltip.cpp, whose __FILE__
**	is 0xB91938; the client holds no string of this header)
**
**	UIViewUnknown30Class: the gadgets' tooltip window, UIViewClass +0x30 (uiview.cpp's Init 0x65A820 makes
**	it, Shutdown deletes it). 0x70 bytes; constructor 0x6AB320 ('TOOLTIPS', 'TOOLTIP_FONT'), destructor
**	0x6AB4D0. Its code is tooltip.cpp's (0x6AB320-0x6ABE90: the EH handlers 0xAC14A9-0xAC1533 run in one
**	block; the asserts '!Hides' line 181, 'CurGadget != gadget' line 210, 'lines > MAX_LINES' line 331).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef TOOLTIP_H  // NAME 3 [tooltip.h]: header name assumed from the client's tooltip.cpp
#define TOOLTIP_H

#include "..\..\wwlib\ref_ptr.h"

class UIViewClass;
class GadgetClass;
class DynamicWindowClass;
class FontStyleClass;

class UIViewUnknown30Class	// NAME 1: neutral (D10)
{
public:
	enum { MAX_LINES = 20 };	// NAME 2 [MAX_LINES]: tooltip.cpp's assert 'lines > MAX_LINES' (0x6ABC60); the constructor makes 20 fonts

	UIViewUnknown30Class(UIViewClass *view);			// 0x6AB320 (Init 0x65A820 passes the view)  // NAME 1 [view]: what Init passes
	~UIViewUnknown30Class();							// 0x6AB4D0
	void Unknown_6AB5B0();								// 0x6AB5B0 (UIViewClass's +0x14 0x65B470)
	void Unknown_6AB930(bool unknown);					// 0x6AB930 (the destructor passes false)  // NAME 1 [unknown]: neutral

	UIViewClass *View;									// +0x00  // NAME 1 [View]: what the constructor stores
	GadgetClass *CurGadget;								// +0x04 (0x6AB8F0's assert 'CurGadget != gadget')
	int Unknown08;										// +0x08 (0x6AB5B0 / 0x6AB930: a tick count)
	bool Unknown0C;										// +0x0C
	bool Unknown0D;										// +0x0D
	bool Unknown0E;										// +0x0E
	bool Unknown0F;										// +0x0F
	int Hides;											// +0x10 (0x6AB850's assert '!Hides'; 0x6AB890 counts it up)
	RefCountPtr<DynamicWindowClass> Window;				// +0x14 (DynamicWindowClass "TOOLTIPS", 20; level 2)  // NAME 1 [Window]
	float Unknown18;									// +0x18
	float Unknown1C;									// +0x1C
	FontStyleClass *Lines[MAX_LINES];					// +0x20 (TOOLTIP_FONT each)  // NAME 2 [Lines]: 'lines > MAX_LINES'
};

#endif
