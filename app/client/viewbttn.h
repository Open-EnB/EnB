// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\viewbttn.h (name assumed from viewbttn.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\client\viewbttn.cpp' (0xB91D5C); the client holds no string of this header)
**
**	The main view's full-screen mouse gadget (UICpitClass::Unknown_54 0x57DD20 makes it with 0, 0, 640, 480 and
**	the main view): a right drag turns and tilts the avatar towards the pointer, with the option
**	'Cpit_ThrottleRightMouse' the pointer inside the screen's middle moves it forward; the pointer over an object
**	targets it (the avatar: the camera block). 0x5C bytes; constructor 0x6B56A0, table 0xAFE0C4 (30 slots),
**	deleting destructor 0x6B57D0, destructor 0x6B5800.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef VIEWBTTN_H  // NAME 3 [viewbttn.h]: header name assumed from the client's viewbttn.cpp
#define VIEWBTTN_H

#include "gadget.h"

class MainViewClass;

// The drag's pointer on a grid (table 0xAFE0B8, 2 slots, 0x14 bytes; no destructor: the gadget's destructor frees it
// with a plain delete): slot +0x00 tells whether the pointer moved to another cell
class UnknownAFE0B8Class	// NAME 1: neutral (D10)
{
public:
	UnknownAFE0B8Class() : Unknown04(-1), Unknown08(-1), Unknown0C(0.03125f), Unknown10(0.03125f) {}
	virtual bool Unknown_00(unsigned time, float x, float y);	// +0x00 0x6B5ED0 (G_ASSERTs 'x >= 0.0f && x <= 1.0f' l.389, 'y >= ...' l.390: the cell of x / +0x0C, y / +0x10)  // NAME 1 [time, x, y]: what the gadget passes (the client's +0x10BC)
	virtual void Unknown_04();							// +0x04 0x6B5FB0 (no cell)

	int Unknown04;										// +0x04 (the cell's column, -1 none)
	int Unknown08;										// +0x08 (the cell's row, -1 none)
	float Unknown0C;									// +0x0C (1/32: a cell's width)
	float Unknown10;									// +0x10 (1/32: a cell's height)
};

class ViewButtonClass : public RectangleGadgetClass	// NAME 3: class-from-file (D24) for viewbttn.cpp
{
public:
	ViewButtonClass(int x, int y, int width, int height, MainViewClass *view);	// 0x6B56A0 (mask 0xFF)  // NAME 1 [x, y, width, height, view]: what the cockpit passes (0, 0, 640, 480)
	virtual ~ViewButtonClass();							// +0x00 0x6B5800 (the option saved; 0x6B57D0)
	virtual GadgetClass *Unknown_18() { return(this); }	// +0x18 0x6B57B0
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x2C 0x6B58A0 (FAIL_IF '!targeting' l.237)  // NAME 1 [event, x, y, mask]: GadgetClass's
	virtual bool Unknown_34(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x34 0x6B5E40 (the base's; unanswered, mask 0x44 to slot +0x2C)  // NAME 1 [event, x, y, mask]: GadgetClass's

	bool Unknown_6B5E90();								// 0x6B5E90 (no caller: no throttle, or the cockpit's +0x144 set)

	int Unknown40;										// +0x40 (the constructor leaves it)
	MainViewClass *View;								// +0x44 (its client +0x4C, radar +0x80, cockpit +0x1D0)  // NAME 2: what it holds
	bool ThrottleRightMouse;							// +0x48  // NAME 3: the option 'Cpit_ThrottleRightMouse' it holds
	bool Unknown49;										// +0x49 (moving forward; the cockpit's 0x584A60 reads it)
	bool Unknown4A;										// +0x4A (turning)
	UnknownAFE0B8Class *Unknown4C;						// +0x4C
	int Unknown50;										// +0x50
	int Unknown54;										// +0x54
	float Unknown58;									// +0x58 (1)
};

UnknownAFE0B8Class *Unknown_6B5FD0();					// 0x6B5FD0 (no caller: a new UnknownAFE0B8Class)

#endif // VIEWBTTN_H
