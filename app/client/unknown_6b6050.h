// Earth & Beyond client (reconstruction): the starbase interface's full-screen gadget (client 0x6B6050-0x6B619F).
// File and header names PROVISIONAL (D137): no string; between viewbttn.cpp (to 0x6B602D, no static initialiser
// of its own) and wnd_file.cpp (0x6B61B0); the object ends with <string>'s guarded static 0x6B6180.
#ifndef UNKNOWN_6B6050_H
#define UNKNOWN_6B6050_H

#include "gadget.h"

class SBaseViewClass;

// What 0x634140 news for the starbase interface's +0xB8 (0x44 bytes, constructor 0x6B6050, table 0xAFE174): a
// full-screen RectangleGadgetClass that keeps the view
class UIStarbaseGadget6B6050Class : public RectangleGadgetClass	// NAME 1: neutral (D10)
{
public:
	UIStarbaseGadget6B6050Class(int x, int y, int width, int height, SBaseViewClass *view);	// 0x6B6050 (0, 0, 640, 480, the view)  // NAME 1 [x, y, width, height, view]: what 0x634140 passes
	virtual ~UIStarbaseGadget6B6050Class();				// 0x6B60F0 (deleting 0x6B60C0)
	virtual GadgetClass *Unknown_1C();					// +0x1C 0x6B60A0 (itself)
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x2C 0x6B6110 (a mask bit of its own, no event)
	virtual bool Unknown_34(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x34 0x6B6130 (bits 0x44 the base left: slot +0x2C, not handled)

	SBaseViewClass *Unknown40;							// +0x40
};

#endif
