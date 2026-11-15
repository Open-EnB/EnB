// Earth & Beyond client (reconstruction): app\client\camctl.h, the camera control gadget (notes in camctl.cpp). The client
// holds no string of a header; the name follows camctl.cpp's __FILE__ (interim, D137).
#ifndef CAMCTL_H  // NAME 1 [camctl.h]: after camctl.cpp's __FILE__
#define CAMCTL_H

#include "gadget.h"
#include "trect.h"
#include "rect.h"

// What the gadget drives (its +0x40): slot +0x00 gets the pointer's push out of the inner rectangle, slot +0x04 the release
class CamCtlControlClass	// NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(float x, float y, unsigned int keys) = 0;	// +0x00 (-1, 0 or 1 each way; the Shift / Ctrl / Alt bits 0x100 / 0x200 / 0x400)  // NAME 1 [x, y, keys]: what 0x65BC20 passes
	virtual void Unknown_04() = 0;										// +0x04 (0x65BC00: the gadget's slot +0x50)
};

// The camera control gadget (0x68 bytes, table 0xAF98C4): a rectangle whose band between the outer and the inner rectangle
// pushes the camera; nothing in the client constructs it
class CamCtlGadgetClass : public RectangleGadgetClass	// NAME 3: class-from-file (D24) for app\Client\camctl.cpp
{
public:
	CamCtlGadgetClass(CamCtlControlClass *control, const Rect &outer, const Rect &inner);	// 0x65B930 (the asserts' outer / inner)  // NAME 3 [outer, inner]: the asserts'; NAME 1 [control]: what it holds
	// (no destructor of its own: the implicit one 0x65BBE0, deleting 0x65BB90)
	virtual bool Unknown_34(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x34 0x65BC20 (the pointer in the band: the control's slot +0x00)  // NAME 1 [event, x, y, mask]: GadgetClass's
	virtual void Unknown_50() { Control->Unknown_04(); }	// +0x50 0x65BC00

	CamCtlControlClass *Control;						// +0x40 the constructor's  // NAME 1: what it holds
	bool Inside;										// +0x44 the pointer in the band (slot +0x68 on entry, +0x50 / +0x6C on leaving)  // NAME 2: what it holds
	RectClass Outer;									// +0x48 outer / 640 and 480: Left, Top, then (Width + 1), (Height + 1) in Right / Bottom  // NAME 3: the asserts' outer
	RectClass Inner;									// +0x58 inner, the same way  // NAME 3: the asserts' inner
};

#endif // CAMCTL_H
