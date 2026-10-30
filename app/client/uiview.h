/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ (uiview.h: PROVISIONAL, after uiview.cpp's __FILE__)
**
**	What uiview.cpp's UIViewClass (sclient.h) makes and calls, as far as uiview.cpp and mainview.cpp use it.
*/

#if defined(_MSC_VER)
#pragma once
#endif

// NAME 3 [uiview.h]: the header of uiview.cpp (its __FILE__ 'e:\build\beta\G\app\Client\uiview.cpp')
#ifndef __UIVIEW_H
#define __UIVIEW_H

#include "..\..\ww3d2\scene.h"
#include "ref_ptr.h"

class CameraClass;
class UIViewClass;

// the views' 2-D scene (UIViewClass +0x1C; 0xA0 bytes, constructor 0x6DA4D0)
class ViewUnknown1CClass : public SimpleSceneClass	// NAME 1: neutral (D10)
{
public:
	ViewUnknown1CClass();								// 0x6DA4D0
	bool Unknown_6DA6F0(int a, int b, int c, int d);	// 0x6DA6F0 (UIViewClass's 0x65B7A0)  // NAME 1 [a, b, c, d]: neutral
	bool Unknown_6DA730(int a);							// 0x6DA730 (UIViewClass's 0x65B7E0)  // NAME 1 [a]: neutral
	void Unknown_6DA760(RefCountPtr<CameraClass> &camera);	// 0x6DA760 (UIViewClass::Render and the main view's 0x50D520 pass the +0x20 camera)  // NAME 1 [camera]: what they pass

	char Unknown98[0xA0 - 0x98];						// +0x98
};

// gadget_list_manager.cpp's manager (UIViewClass +0x2C; 0x4C bytes, constructor 0x6611B0, destructor 0x661430)
class GadgetListManagerClass	// NAME 3: class-from-file (D24) for app\client\gadget_list_manager.cpp
{
public:
	GadgetListManagerClass();							// 0x6611B0
	~GadgetListManagerClass();							// 0x661430

	char Unknown00[0x4C];
};

// UIViewClass +0x30 (0x70 bytes, constructor 0x6AB320, destructor 0x6AB4D0)
class UIViewUnknown30Class	// NAME 1: neutral (D10)
{
public:
	UIViewUnknown30Class(UIViewClass *view);			// 0x6AB320 (Init 0x65A820 passes the view)  // NAME 1 [view]: what Init passes
	~UIViewUnknown30Class();							// 0x6AB4D0
	void Unknown_6AB5B0();								// 0x6AB5B0 (UIViewClass's +0x14 0x65B470)

	char Unknown00[0x70];
};

#endif //__UIVIEW_H
