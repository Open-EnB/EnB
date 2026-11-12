// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\pip_scene.h (name assumed from pip_scene.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\client\pip_scene.cpp'; the client holds no string of this header)
**
**	A 3-D scene and its camera drawn into a rectangle of a view's PIP sort list: the base of the map
**	PIPs (0x6927C0) and of 0x541DE0's. No RTTI: table 0xB0665C (the destructor only), constructor
**	0x7BA9C0, destructor 0x7BAC10 (deleting 0x7BABE0), 0x34 bytes.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __PIP_SCENE_H  // NAME 3 [pip_scene.h]: header name assumed from the client's pip_scene.cpp
#define __PIP_SCENE_H

#include "always.h"
#include "ref_ptr.h"
#include "vector2.h"
#include "rect.h"

class SClient;
class SimpleSceneClass;
class CameraClass;
class RenderObjClass;
class UIViewClass;

class PipSceneClass  // NAME 3: class-from-file (D24) for app\client\pip_scene.cpp
{
public:
	PipSceneClass(SClient *client);						// 0x7BA9C0  // NAME 1 [client]: what it stores
	virtual ~PipSceneClass();							// +0x00 0x7BAC10 (deleting 0x7BABE0)

	bool Unknown_7BACD0(const RefCountPtr<RenderObjClass> &vp, bool arg);	// 0x7BACD0 (FAIL_IF '!vp')  // NAME 2 [vp]: the FAIL_IF's  // NAME 1 [arg]: neutral (true: the 'tl' / 'br' bones, false: the bounding box)
	bool Unknown_7BB080(UIViewClass *view, int arg1, int arg2);	// 0x7BB080 (FAIL_IF '!view'; UIViewClass::Unknown_65B7A0's poly budget, sort level)  // NAME 2 [view]: the FAIL_IF's  // NAME 1 [arg1, arg2]: neutral
	bool Unknown_7BB110();								// 0x7BB110 (true: it was in a view)

	SClient *Peek_Client() { return(Unknown04); }		// (inline: gd_pip_scene.cpp's FAIL_IF '!PIPScene->Peek_Client()' l.148)  // NAME 3: the FAIL_IF's
	Vector2 &Rect_Min() { return *(Vector2 *)&Rect.Left; }	// Rect's corners as Vector2s (0x692EF0, 0x7BACD0)  // NAME 1: neutral
	Vector2 &Rect_Max() { return *(Vector2 *)&Rect.Right; }

	SClient *Unknown04;									// +0x04 (0x7BA9C0's client)
	RefCountPtr<SimpleSceneClass> Scene;				// +0x08 (0x7BA9C0 news it; 0x53F920 adds to it)  // NAME 1: neutral (D10): what it holds
	RefCountPtr<CameraClass> Camera;					// +0x0C (0x53F770 sets its clip planes)  // NAME 1: neutral (D10): what it holds
	UIViewClass *View;									// +0x10 (0x7BB080's view)  // NAME 2: the FAIL_IF's '!view'
	RectClass Rect;										// +0x14 (0x692EF0 hands its corners to 0x7C3C60)  // NAME 1: neutral (D10): what it holds
	Vector2 ViewportSize;								// +0x24
	Vector2 Unknown2C;									// +0x2C (the rectangle's centre)
};

#endif //__PIP_SCENE_H
