// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	gd_uiback.cpp's classes (client 0x673EE0-0x674F80). The client holds no string of this header; the name follows the
**	file's __FILE__ (D24). Notes at the end of gd_uiback.cpp.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef GD_UIBACK_H  // NAME 3 [GD_UIBACK_H]: header name from gd_uiback.cpp's __FILE__ (D24)
#define GD_UIBACK_H

#include "unknown_671300.h"
#include "unknown_674260.h"
#include "vector2.h"
#include "vector3.h"
#include "..\..\wwlib\refcount.h"

// The cockpit's two thrust buttons (ui_cpit.cpp's 'THROTTLE' and 'REVERSE'; 0x88 bytes, table 0xAFB498)
class Unknown_673EE0Class : public Unknown_671300Class	// NAME 1: neutral (D10)
{
public:
	Unknown_673EE0Class(RenderObjClass *robj, int unknown1, const LayerClass &layer, int unknown2, int unknown3);	// 0x673EE0 (Unknown_671300Class with 3)  // NAME 1 [robj, unknown1, layer, unknown2, unknown3]: neutral, the base's
	virtual ~Unknown_673EE0Class();						// +0x00 0x673F60 (deleting 0x673F30)
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x2C 0x673F80  // NAME 1 [event, x, y, mask]: the base's
	bool Unknown_6740E0(float seconds);					// 0x6740E0 (ui_cpit.cpp's 0x583790 passes 1.0f)  // NAME 1 [seconds]: what it is compared with
	void Unknown_674120(bool on);						// 0x674120 (+0x80 = 1 or 0)  // NAME 1 [on]: neutral
	void Unknown_674160(bool on);						// 0x674160 (+0x80 = 2 or 0)  // NAME 1 [on]: neutral

	int Unknown80;										// +0x80 (0, 1 counts the time held, 2)
	float Unknown84;									// +0x84 (the seconds held, while +0x80 is 1)
};

// What MovableUIBackGadget moves: refcounted, table 0xAFB794 (slots +0x08 to +0x10 pure), through RefCountPtr
class Unknown_AFB794Class : public RefCountClass	// NAME 1: neutral (D10)
{
public:
	virtual void Set_Position(const Vector3 &position) = 0;	// +0x08  // NAME 2: 0x6745F0 forwards to the model's  // NAME 1 [position]: neutral
	virtual Vector3 Get_Position() = 0;						// +0x0C  // NAME 2: 0x674610 forwards to the model's
	virtual RenderObjClass *Peek_Object() = 0;				// +0x10  // NAME 2: 0x674640 returns its model
};

// The gadget that drags its target with the pointer (RTTI 'MovableUIBackGadget'; 0xA4 bytes, table 0xAFB6B8)
class MovableUIBackGadget : public Unknown_674260Class	// NAME 5: the client's RTTI .?AVMovableUIBackGadget@@ (the typeid 0x6748F0 passes)
{
public:
	MovableUIBackGadget(const RefCountPtr<RenderObjClass> &model, const Vector3 &offset);	// 0x674310 (the target: the model itself)  // NAME 1 [model, offset]: what it does with them
	MovableUIBackGadget(const RefCountPtr<RenderObjClass> &model, const RefCountPtr<Unknown_AFB794Class> &target);	// 0x674740  // NAME 1 [model, target]: what it does with them
	virtual ~MovableUIBackGadget();						// +0x00 0x674860 (deleting 0x674710)
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x2C 0x6748F0  // NAME 1 [event, x, y, mask]: the base's

	RefCountPtr<Unknown_AFB794Class> Unknown60;			// +0x60 (the target)
	bool Unknown64;										// +0x64 (it moved on the last event)
	Vector3 Unknown68;									// +0x68 (where the button went down)
	Vector3 Unknown74;									// +0x74 (the target's position then)
	Vector3 Unknown80;									// +0x80 (the offset out of the limits)
	Vector2 Unknown8C;									// +0x8C (the limits' least: -320, -240)
	Vector2 Unknown94;									// +0x94 (the limits' most: 320, 240)
	Vector2 Unknown9C;									// +0x9C (the target's bounding box's half size)
};

// The event 0x6748F0 emits when the target has moved (RTTI 'MovableUIBackGadgetMoved')
class MovableUIBackGadgetMoved							// NAME 5: the client's RTTI .?AVMovableUIBackGadgetMoved@@
{
};

// The gadget over a window's back: answers the pointer between the bones 'back_tl' and 'back_br' (0x68 bytes, table 0xAFB7AC)
class UIBackGadgetClass : public Unknown_674260Class	// NAME 3: class-from-file (D24) for gd_uiback.cpp
{
public:
	UIBackGadgetClass(const RefCountPtr<RenderObjClass> &model);	// 0x674C10  // NAME 1 [model]: what ui_starmap.cpp passes
	virtual ~UIBackGadgetClass();						// +0x00 0x674D50 (deleting 0x674D20)
	virtual bool Unknown_30(float x, float y);			// +0x30 0x674DE0  // NAME 1 [x, y]: the base's
	void Unknown_674D70();								// 0x674D70 (FAIL_IF '!robj': the bones' indices; inlined in the constructor)

	int Unknown60;										// +0x60 ('back_tl')
	int Unknown64;										// +0x64 ('back_br')
};

#endif
