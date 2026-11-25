// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Interim placement (D137): smouse.cpp's software mouse, out of tsk_login.h: the client's drag_mouse.obj has no <string>
**	initialiser, so it never saw tsk_login.h (tsk_game.h). tsk_login.h includes this header where the class stood.
*/
#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __SMOUSE_H	// NAME 1 [smouse.h]: neutral, what it declares (D10)
#define __SMOUSE_H


class RenderObjClass;
class SMouseDragClass;  // smouse.cpp's: what +0x2C holds (0x685190's class)
class LoginUnknown18Class	// NAME 1: neutral (D10): what LoginTaskClass +0x18 points to (smouse.cpp's software mouse)
{
public:
	LoginUnknown18Class(int unknown);					// 0x514FE0 (0x30 bytes)
	~LoginUnknown18Class();								// 0x515030
	RenderObjClass *Unknown_515080(int cursor);			// 0x515080: a Bitmap2DObjClass of the cursor's texture (0x515120 passes the cursor; ret 4, unread: +0x0C is)  // NAME 1 [cursor]: what 0x515120 passes
	int Unknown_515120(int cursor);						// 0x515120 (the constructor passes 1): the cursor set, the old one returned  // NAME 1 [cursor]: an index into mousetbl.cpp's table 0xBE7C70
	int Unknown_515320();								// 0x515320: the cursor (+0x0C)
	void *Unknown_515340(void *scene);					// 0x515340: the scene set, the old one returned  // NAME 1 [scene]: what the callers pass (the view's slot +0x3C)
	void Unknown_5153B0();								// 0x5153B0: hidden
	void Unknown_515400();								// 0x515400: shown again
	void Unknown_515440();								// 0x515440: every LoginTaskClass::Run
	bool Unknown_515510(SMouseDragClass *drag);			// 0x515510 (FAIL_IF '!drag')  // NAME 3 [drag]: the FAIL_IF's
	bool Unknown_5155A0(SMouseDragClass *drag);			// 0x5155A0 (FAIL_IF 'DragObject != drag')  // NAME 3 [drag]: the FAIL_IF's

	float Get_X() { return(Unknown14); }				// (inline: gutter_radar.cpp's 0x7B6E40 reads it through the FPU)  // NAME 2: what it does
	float Get_Y() { return(Unknown18); }				// (as Get_X)  // NAME 2: what it does

	int Unknown00;										// +0x00 (the constructor's argument)
	RenderObjClass *Unknown04;							// +0x04: the cursor's model
	void *Unknown08;									// +0x08: the scene it is in
	int Unknown0C;										// +0x0C: the cursor (-1: none)
	int Unknown10;										// +0x10 (-1)
	float Unknown14;									// +0x14: the pointer's x (0..1)
	float Unknown18;									// +0x18: the pointer's y
	float Unknown1C;									// +0x1C: the hot spot's x offset (-1/640 per pixel)
	float Unknown20;									// +0x20: the hot spot's y offset (-1/480 per pixel)
	bool Unknown24;										// +0x24: hidden
	int Unknown28;										// +0x28
	SMouseDragClass *DragObject;						// +0x2C  // NAME 3: the FAIL_IFs'
};

#endif
