// Earth & Beyond client (reconstruction): the starbase interface's full-screen gadget (client 0x6B6050-0x6B619F).
// File PROVISIONAL (D137): see unknown_6b6050.h.
#include <string>
#include "unknown_6b6050.h"

// @0x6B6050
UIStarbaseGadget6B6050Class::UIStarbaseGadget6B6050Class(int x, int y, int width, int height, SBaseViewClass *view) :
	RectangleGadgetClass(x, y, width, height, 0xFF),
	Unknown40(view)
{
}

// @0x6B60A0
GadgetClass *UIStarbaseGadget6B6050Class::Unknown_1C()
{
	return(this);
}

// @0x6B60F0
UIStarbaseGadget6B6050Class::~UIStarbaseGadget6B6050Class()
{
}

// @0x6B6110
bool UIStarbaseGadget6B6050Class::Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)
{
	if (mask & Mask) {
		return(true);
	}
	return(false);
}

// @0x6B6130
bool UIStarbaseGadget6B6050Class::Unknown_34(KernelEventClass *event, float x, float y, unsigned int mask)
{
	bool handled = GadgetClass::Unknown_34(event, x, y, mask);
	if (!handled && (mask & 0x44)) {
		Unknown_2C(event, x, y, mask & 0x44);
		handled = false;
	}
	return(handled);
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**
**	UIStarbaseGadget6B6050Class (unknown_6b6050.h): 0x6B6050-0x6B619F, table 0xAFE174. The constructor calls
**	RectangleGadgetClass's 0x660D30 with 0xFF as its fifth argument, then stores the table and the view; slot +0x1C
**	answers with the gadget itself; the deleting destructor 0x6B60C0 sits before the destructor 0x6B60F0 (which
**	stores the table and jumps to GadgetClass's); slot +0x2C tests the mask against GadgetClass's +0x10; slot +0x34
**	passes bits 0x44 the base declined to slot +0x2C and answers false. The object ends with <string>'s guarded
**	static 0x6B6180 (guard 0xC416FC, atexit 0x42B320), as every file including it. Moved here from ui_starbase.cpp
**	(declared) and the stand-in build's poc_world2.cpp (defined, until 2026-11-21), its interim places.
*/
