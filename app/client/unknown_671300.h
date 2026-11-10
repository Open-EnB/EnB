// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Interim header (D137): the client holds no string of the header that declares this gadget. Its code, 0x671300-0x671790,
**	opens the object of gd_text_editor.cpp (see the notes there), but ui_avatr.cpp, ui_cpit.cpp, gd_buffgadg.h and others
**	use the class without the editor.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UNKNOWN_671300_H  // NAME 1 [unknown_671300.h]: neutral (D137)
#define UNKNOWN_671300_H

#include "unknown_675120.h"
#include "..\shared\uvcontrl.h"

// The gadget of constructor 0x671300 (0x80 bytes, table 0xAFB278): a button gadget that notes the buttons pressed on it and
// scrolls its texture
class Unknown_671300Class : public ButtonGadgetClass	// NAME 1: neutral (D10)
{
public:
	Unknown_671300Class(RenderObjClass *robj, int unknown1, const LayerClass &layer, int unknown2, int unknown3, int unknown4);	// 0x671300 (ButtonGadgetClass 0x675240 with 0x77)  // NAME 1 [robj, unknown1, layer, unknown2, unknown3, unknown4]: neutral, the base's
	Unknown_671300Class(RenderObjClass *robj, int unknown1, int unknown2, int unknown3, int unknown4);	// 0x671410 (ButtonGadgetClass 0x675310 with 0x77)  // NAME 1 [robj, unknown1, unknown2, unknown3, unknown4]: neutral, the base's
	virtual ~Unknown_671300Class();						// +0x00 0x6714F0 (deleting 0x6713E0)
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x2C 0x671570  // NAME 1 [event, x, y, mask]: the base's
	virtual void Unknown_A4(float speed);				// +0xA4 0x671650  // NAME 1 [speed]: what 0x6716B0 does with it
	virtual void Unknown_A8();							// +0xA8 0x671670
	virtual void Unknown_AC();							// +0xAC 0x6716B0 (the buff gadget's 0x665F20: every frame)
	void Unknown_671630();								// 0x671630 (the buttons' flags off)
	void Unknown_671740(bool on, float speed);			// 0x671740 (ui_cpit.cpp's 0x583510: 1, 1.0f; 0, 0)  // NAME 1 [on, speed]: neutral

	bool Unknown64;										// +0x64 (the left button held)
	bool Unknown65;										// +0x65 (the right button held)
	bool Unknown66;										// +0x66
	UVControllerClass *Unknown68;						// +0x68 (0x4A1240's)
	float Unknown6C;									// +0x6C (its offset: the seconds run)
	float Unknown70;									// +0x70 (the seconds' scale)
	bool Unknown74;										// +0x74 (it runs)
	int Unknown78;										// +0x78
	int Unknown7C;										// +0x7C
};

#endif
