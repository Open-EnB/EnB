// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\gd_resize.h (name assumed from gd_resize.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\Client\gd_resize.cpp'; the client holds no string of this header)
**
**	gd_resize.cpp's drag handle: a model gadget that follows the pointer between two corners (table 0xAFA954,
**	0x8C bytes; constructor 0x66BA50, destructor 0x66BAF0). Its base is the model gadget that stays out of the
**	scene (constructor 0x675120, table 0xAFB870: unknown_675120.h).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef GD_RESIZE_H  // NAME 3 [gd_resize.h]: header name assumed from the client's gd_resize.cpp
#define GD_RESIZE_H

#include "gd_robj.h"
#include "vector2.h"
#include "..\..\wwlib\ref_ptr.h"

#include "unknown_675120.h"

class ResizeGadgetClass : public Unknown_675120Class  // NAME 3 [ResizeGadgetClass]: class-from-file (D24) for app\Client\gd_resize.cpp
{
public:
	ResizeGadgetClass(const RefCountPtr<RenderObjClass> &robj);		// 0x66BA50 (mask 0x77)  // NAME 1 [robj]: neutral
	virtual ~ResizeGadgetClass();										// +0x00 0x66BAF0 (deleting 0x66BAC0)
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x2C 0x66BB10 (FAIL_IF '!Peek_Render_Obj()' l.64: the drag)  // NAME 1 [event, x, y, mask]: the base's
	virtual void Unknown_40(bool on);									// +0x40 0x66BD60 (hidden while off)  // NAME 1 [on]: the base's
	void Unknown_66BDA0();												// 0x66BDA0 (+0x60 from the model's position)

	Vector2 Unknown60;													// +0x60: the origin (the model's x, y; 0x66BDA0)  // NAME 1: neutral (D10)
	Vector2 Unknown68;													// +0x68: the pointer from the origin, clamped to +0x70 .. +0x78  // NAME 1: neutral (D10)
	Vector2 Unknown70;													// +0x70: the low corner  // NAME 1: neutral (D10)
	Vector2 Unknown78;													// +0x78: the high corner  // NAME 1: neutral (D10)
	Vector2 Unknown80;													// +0x80: the model's position less the pointer at the press  // NAME 1: neutral (D10)
	bool Unknown88;														// +0x88: dragging (set at the press, cleared at the release)  // NAME 1: neutral (D10)
};

#endif
