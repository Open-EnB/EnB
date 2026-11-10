// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Interim location (D137): app\client\gd_dynamicelement.h. The client holds no string that names this
**	gadget's file; its code (0x6686F0-0x66884F) sits between gd_buffgadg.cpp's and gd_dynamicwindow.cpp's in
**	the alphabetical link order, so the name is chosen to sort there.
**
**	A gadget over one element of a dynamic window: the pointer is over it while it is inside the element's
**	bounds (table 0xAF9F48, 0x34 bytes; constructor 0x6686F0, destructor 0x668750, deleting 0x668720). The
**	toggle buttons (dynamic_window_toggle.cpp) and the item examination window's rows make one over themselves.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef GD_DYNAMICELEMENT_H  // NAME 1 [gd_dynamicelement.h]: interim header name (D137), see above
#define GD_DYNAMICELEMENT_H

#include "aabox.h"
#include "gadget.h"

class DynamicWindowElementClass;

class DynamicElementGadgetClass : public GadgetClass  // NAME 1 [DynamicElementGadgetClass]: what it is (no RTTI, no string)
{
public:
	DynamicElementGadgetClass(DynamicWindowElementClass *element);		// 0x6686F0 (mask 0x55)  // NAME 1 [element]: what it holds
	virtual ~DynamicElementGadgetClass() {}								// +0x00 0x668750 (deleting 0x668720)
	virtual bool Unknown_30(float x, float y);							// +0x30 0x668770 (the pointer inside the element's bounds)
	virtual void Unknown_64(AABoxClass &box);							// +0x64 0x668840 (the element's bounds)

protected:
	DynamicWindowElementClass *Element;									// +0x30  // NAME 1: what it holds
};

#endif
