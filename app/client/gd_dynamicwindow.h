// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\gd_dynamicwindow.h (name assumed from gd_dynamicwindow.cpp,
**	whose __FILE__ is 0xB8CF58; the client holds no string of this header)
**
**	gd_dynamicwindow.cpp's gadget: a button gadget over a dynamic window's model (FAIL_IF '!DynamicObject'
**	l.61). Table 0xAF9FD4, 0x6C bytes; constructors 0x668860 / 0x6689C0, destructor 0x668B20 (deleting
**	0x668990). The message box (unknown_msgbox.cpp) makes one over its window and one per option.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef GD_DYNAMICWINDOW_H  // NAME 3 [gd_dynamicwindow.h]: header name assumed from the client's gd_dynamicwindow.cpp
#define GD_DYNAMICWINDOW_H

#include "unknown_675120.h"
#include "..\..\wwlib\ref_ptr.h"

class DynamicWindowClass;

class DynamicWindowGadgetClass : public ButtonGadgetClass  // NAME 3 [DynamicWindowGadgetClass]: class-from-file (D24) for app\Client\gd_dynamicwindow.cpp
{
public:
	DynamicWindowGadgetClass(const RefCountPtr<DynamicWindowClass> &object, unsigned int mask, unsigned int flags, bool highlight);	// 0x668860  // NAME 1 [object, mask, flags, highlight]: the base's and what it does
	DynamicWindowGadgetClass(DynamicWindowClass *object, unsigned int mask, unsigned int flags, bool highlight);	// 0x6689C0 (the message box passes 0x55, 3)  // NAME 1 [object, mask, flags, highlight]: the base's and what it does
	virtual ~DynamicWindowGadgetClass();								// +0x00 0x668B20 (deleting 0x668990)
	virtual RenderObjClass *Unknown_24(Vector3 *position, float *frame, bool indicator) { return(NULL); }	// +0x24 0x668970
	virtual bool Unknown_30(float x, float y);							// +0x30 0x668C80 (FAIL_IF '!DynamicObject': the pointer inside the window's bounds)

	void Set_Dynamic_Object(const RefCountPtr<DynamicWindowClass> &object);	// 0x668BE0 (the window's model becomes the gadget's)  // NAME 1 [Set_Dynamic_Object, object]: the FAIL_IF's DynamicObject

	RefCountPtr<DynamicWindowClass> DynamicObject;						// +0x64 (FAIL_IF '!DynamicObject' l.61)  // NAME 3: the FAIL_IF's
	bool Highlight;														// +0x68 (0.6 grey, white while the pointer is over it)  // NAME 1: what it does
};

#endif
