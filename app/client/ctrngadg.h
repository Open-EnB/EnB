// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ctrngadg.h (name assumed from ctrngadg.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\client\ctrngadg.cpp' (0xB76294); the client holds no string of this header)
**
**	The cockpit's full-screen gadget (UICpitClass::Unknown_54 0x57DD20 makes it with 0, 0, 640, 480 and its
**	client, 0x57E014): a deactivation gadget whose pointer drags turn the camera (camera_manager.cpp's
**	0x79B370-0x79B410) and whose wheel zooms it. 0x50 bytes; constructor 0x50A850, table 0xAECDE4 (29 slots:
**	GadgetClass's one-line slots 0x50A8C0-0x50AAC0 are emitted in this object), deleting destructor 0x50AAE0,
**	destructor 0x50AB10.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CTRNGADG_H  // NAME 3 [ctrngadg.h]: header name assumed from the client's ctrngadg.cpp
#define CTRNGADG_H

#include "gadget.h"

class LineDisplayClientClass;

class CtrnGadgetClass : public InterfaceDeactivationGadgetClass  // NAME 3: class-from-file (D24) for app\client\ctrngadg.cpp
{
public:
	CtrnGadgetClass(int x, int y, int width, int height, LineDisplayClientClass *client);	// 0x50A850 (mask 0x1080)  // NAME 1 [x, y, width, height, client]: what the cockpit passes (0, 0, 640, 480)
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x2C 0x50ABC0 (assert 'false' line 162: the wheel; drags turn the camera)  // NAME 1 [event, x, y, mask]: GadgetClass's
	virtual bool Unknown_30(float x, float y);							// +0x30 0x50AB30 (the base's hit test; with +0x44 set, Alt off and Shift or Ctrl down)  // NAME 1 [x, y]: GadgetClass's

	LineDisplayClientClass *Client;										// +0x40 (slot +0x2C: its slot +0x18's)  // NAME 1: neutral (D10), as InterfaceClass's +0x40
	bool Unknown44;														// +0x44 (the constructor sets it; slot +0x30 tests it)
	float Unknown48;													// +0x48 (slot +0x2C: the last x)
	float Unknown4C;													// +0x4C (slot +0x2C: the last y)
};

#endif // CTRNGADG_H
