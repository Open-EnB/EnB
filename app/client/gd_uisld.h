// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	gd_uisld.cpp's class (client 0x675370-0x675FEF). The client holds no string of this header; the name follows the
**	file's __FILE__ (D24). Notes at the end of gd_uisld.cpp.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef GD_UISLD_H  // NAME 3 [GD_UISLD_H]: header name from gd_uisld.cpp's __FILE__ (D24)
#define GD_UISLD_H

#include "unknown_675120.h"
#include "vector2.h"
#include "vector3.h"
#include "matrix3d.h"

class RenderObjClass;
class LayerClass;

// The windows' scroll bar: a 'bar_*' model with a 'pull_*' bone the pointer drags along it (0xD0 bytes, table 0xAFBA80;
// ui_ptmis.cpp, ui_ptnew.cpp, ui_ptppl.cpp, ui_string_list.cpp, ui_treem.cpp and ui_commands.cpp make it)
class UISliderGadget : public Unknown_675120Class	// NAME 3: class-from-file (D24) for app\Client\gd_uisld.cpp, as gd_uiback.cpp's UIBackGadget (RTTI)
{
public:
	UISliderGadget(RenderObjClass *screen, const char *pull, const char *bar, const LayerClass *layer, bool horizontal, float extra);	// 0x675370 (the gadget's model the screen's 'bar_*')  // NAME 1 [screen, pull, bar, layer, horizontal, extra]: what the pages pass and what 0x6755E0 does with them
	UISliderGadget(RenderObjClass *screen, const char *pull, const char *bar, bool horizontal, float extra);	// 0x6754F0 (no layer)  // NAME 1 [screen, pull, bar, horizontal, extra]: as above
	virtual ~UISliderGadget();							// +0x00 0x675810 (the pull's bone put back and released; deleting 0x6754C0)
	virtual RenderObjClass *Unknown_24(Vector3 *position, float *frame, bool indicator);	// +0x24 0x675C10 (FAIL_IF '!retval': the pull while it is held, else the bar)
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x2C 0x675A90 (pressed, dragged, released)
	virtual bool Unknown_30(float x, float y);			// +0x30 0x675900 (the pointer on the pull, else the base's)
	virtual void Unknown_40(bool on);					// +0x40 0x6758E0 (the base's)
	virtual void Unknown_8C(bool on);					// +0x8C 0x675CA0 (the pull hidden with the bar)
	virtual void Unknown_A4(int top);					// +0xA4 0x675D30 (the first row: clamped, the pull's bone moved; 'SR_rollover14b.wav' when it changes)  // NAME 1 [top]: what it sets
	virtual int Unknown_A8() { return(Unknown0B4); }	// +0xA8 0x675460 (the first row shown)
	virtual int Unknown_AC() { return(Unknown0AC); }	// +0xAC 0x675480
	virtual int Unknown_B0() { return(Unknown0B0); }	// +0xB0 0x6754A0
	void Unknown_675CF0(int count, int a, int b);		// 0x675CF0 (+0xAC, +0xB0, then slot +0xA4)  // NAME 1 [count, a, b]: neutral

	void Unknown_6755E0(const char *pull, const char *bar, bool horizontal, float extra);	// 0x6755E0 (the constructors': the pull's bone captured, the track's length)  // NAME 1 [pull, bar, horizontal, extra]: as the constructors'
	void Unknown_675F00(Vector2 point);					// 0x675F00 (the row under a screen point)  // NAME 1 [point]: neutral

	RenderObjClass *Pull;								// +0x60 (the screen's 'pull_*' sub-object, referenced)  // NAME 2: the sub-object's name
	int PullBone;										// +0x64 (the screen's bone of that name, captured)  // NAME 2: as Pull
	Vector3 Unknown068;									// +0x68 (the pull's place at row 0, from the bar's centre)
	Matrix3D PullTransform;								// +0x74 (the bone's transform before the capture; the destructor puts it back)  // NAME 2: as Pull
	RenderObjClass *Screen;								// +0xA4 (referenced)  // NAME 1: what the pages pass
	bool IsHorizontal;									// +0xA8  // NAME 2: which axis the pull moves on (GadgetSlider's IsHorizontal)
	bool Unknown0A9;									// +0xA9 (pressed: slot +0x2C)
	int Unknown0AC;										// +0xAC (slot +0xAC; the row at the bar's start)
	int Unknown0B0;										// +0xB0 (slot +0xB0; the row at the bar's end)
	int Unknown0B4;										// +0xB4 (slot +0xA8: the row shown first)
	float Length;										// +0xB8 (the pull's track along the bar)  // NAME 2: what 0x6755E0 computes
	bool Unknown0BC;									// +0xBC (the pointer on the pull: slot +0x30)
	Vector2 Unknown0C0;									// +0xC0 (the pointer from the pull: slot +0x30)
	Vector2 Unknown0C8;									// +0xC8 (the pointer in screen units, 640 x 480 from the centre: slot +0x2C passes it whole)
};

#endif
