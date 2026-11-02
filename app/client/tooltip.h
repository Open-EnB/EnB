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
#include "..\..\wwmath\vector2.h"

class UIViewClass;
class FontStyleClass;
class GadgetClass;
class DynamicWindowClass;
class DestroyGadgetEvent;

class UIViewUnknown30Class	// NAME 1: neutral (D10)
{
public:
	enum { MAX_LINES = 20 };	// NAME 2 [MAX_LINES]: tooltip.cpp's assert 'lines > MAX_LINES' (0x6ABC60); the constructor makes 20 fonts

	UIViewUnknown30Class(UIViewClass *view);			// 0x6AB320 (Init 0x65A820 passes the view)  // NAME 1 [view]: what Init passes
	~UIViewUnknown30Class();							// 0x6AB4D0
	void Unknown_6AB5B0();								// 0x6AB5B0 (UIViewClass's +0x14 0x65B470: every frame; the gadget under the pointer)
	bool Unknown_6AB850();								// 0x6AB850 (FAIL_IF '!Hides': one hide less, true when none is left)
	bool Unknown_6AB890();								// 0x6AB890 (one hide more, the gadget dropped; true for the first)
	void Unknown_6AB8C0();								// 0x6AB8C0 (the delay restarted, +0x0D set; inlined in 0x6AB930)
	void Unknown_6AB8F0(DestroyGadgetEvent *event, GadgetClass *gadget);	// 0x6AB8F0 (FAIL_IF 'CurGadget != gadget': the gadget destroyed)  // NAME 2 [gadget]: the FAIL_IF's; NAME 1 [event]: the callback's
	void Unknown_6AB930(GadgetClass *gadget);			// 0x6AB930 (the gadget under the pointer changed; the destructor passes NULL)  // NAME 1 [gadget]: what 0x6AB5B0 passes
	void Unknown_6ABC10();								// 0x6ABC10 (the window hidden; inline, in 0x6AB5B0 / 0x6AB930)
	void Unknown_6ABC60();								// 0x6ABC60 (the window shown: the text split into Lines, FAIL_IF 'lines > MAX_LINES')
	void Unknown_6ABE90();								// 0x6ABE90 (the window placed: at the pointer, or under the gadget's box)

	UIViewClass *View;									// +0x00  // NAME 1 [View]: what the constructor stores
	GadgetClass *CurGadget;								// +0x04 (0x6AB8F0's assert 'CurGadget != gadget')
	unsigned int Unknown08;								// +0x08 (Unknown_Get_Ticks plus the delay: when the window shows)
	bool Unknown0C;										// +0x0C (shown)
	bool Unknown0D;										// +0x0D (0x6AB8C0; the held-button check skipped)
	bool Unknown0E;										// +0x0E (the client's DebugMode while Alt (0x12) is down: the gadget's name for the text, Packet_0068 when it goes)
	bool Unknown0F;										// +0x0F (set: the delay not scaled by 'Interface_Tooltip_Delay')
	int Hides;											// +0x10 (0x6AB850's assert '!Hides'; 0x6AB890 counts it up)
	RefCountPtr<DynamicWindowClass> Window;				// +0x14 (DynamicWindowClass "TOOLTIPS", 20; level 2)  // NAME 1 [Window]
	Vector2 Unknown18;									// +0x18 (the pointer where the gadget was found)
	FontStyleClass *Lines[MAX_LINES];					// +0x20 (TOOLTIP_FONT each)  // NAME 2 [Lines]: 'lines > MAX_LINES'
};

#endif
