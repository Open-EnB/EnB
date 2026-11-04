// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ship_pip.h (name assumed from ship_pip.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\Client\ship_pip.cpp'; the client holds no string of this header)
**
**	The ship picture-in-picture gadget: a model shown in its own 3-D scene (a SimpleSceneClass, a camera,
**	two lights and the 'BONE_HOV01A' float parent) drawn into a view's PIP sort list. No RTTI: table 0xB07DF4
**	(37 slots, GadgetClass's 29 and 8 of its own), constructor 0x80B2E0, destructor 0x80B520 (deleting
**	0x80B4F0), 0xF4 bytes. ui_avatr.cpp's constructor 0x553AF0 makes one (type 1) at its +0x94.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __SHIP_PIP_H  // NAME 3 [ship_pip.h]: header name assumed from the client's ship_pip.cpp
#define __SHIP_PIP_H

#include "always.h"
#include "gadget.h"
#include "ref_ptr.h"
#include "vector2.h"
#include "vector3.h"
#include "matrix3d.h"

class ViewClass;
class UIViewClass;
class CameraClass;
class SimpleSceneClass;
class LightClass;
class RenderObjClass;
class LoginUnknown18Class;

class ShipPipClass : public GadgetClass  // NAME 3: class-from-file (D24) for app\client\ship_pip.cpp
{
public:
	ShipPipClass(int type);												// 0x80B2E0 (0 or 1, else G_ASSERT 'false'; ui_avatr.cpp passes 1)  // NAME 1 [type]: neutral
	virtual ~ShipPipClass();											// +0x00 0x80B520 (deleting 0x80B4F0; slot +0x80 inline)
	// +0x04 GadgetClass's 0x65EF90; +0x08..+0x28 GadgetClass's inline defaults
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x2C 0x80D5E0 (the drags: turn, pan, zoom; the software mouse's cursor)  // NAME 1 [event, x, y, mask]: GadgetClass's
	virtual bool Unknown_30(float x, float y);							// +0x30 0x80D540 (the pointer over the picture: +0xA4 / +0xA8 from x * 640, y * 480)  // NAME 1 [x, y]: GadgetClass's
	// +0x34 GadgetClass's 0x65F500; +0x38 / +0x3C defaults; +0x40 GadgetClass's 0x65F640; +0x44..+0x70 GadgetClass's
	virtual void Unknown_74(int race);									// +0x74 0x80BAB0 (the two lights for the type, coloured by race: ui_avatr.cpp passes its race)  // NAME 1 [race]: what ui_avatr.cpp passes (its +0x108)
	virtual void Unknown_78();											// +0x78 0x80CE10 (the frame: the float parent turns, the camera orbits / zooms / pans by +0xB8..+0xC8)
	virtual void Unknown_7C(const RefCountPtr<RenderObjClass> &model);	// +0x7C 0x80C640 (the model onto the float parent, the camera reset, the scene into PIPView's sort list)  // NAME 1 [model]: what it holds
	virtual void Unknown_80();											// +0x80 0x80C5C0 (the model off: the lights and the scene out)
	virtual void Unknown_84(const RefCountPtr<RenderObjClass> &model);	// +0x84 0x80CA30 (the model replaced, the camera kept within the new distances)  // NAME 1 [model]: as slot +0x7C's
	virtual void Unknown_88();											// +0x88 0x80E260 (the pan offsets +0xCC / +0xD0 reset)
	virtual void Unknown_8C();											// +0x8C 0x80E350 (the camera matrix +0x6C reset: Look_At)
	virtual void Unknown_90();											// +0x90 0x80E530 (the camera's transform from +0x6C and the pan; the lights' directions follow)

	void Unknown_80C180();												// 0x80C180 (ui_avatr.cpp's destructor: slot +0x80, +0xD8 / FloatParent / Scene / Camera released, the size cleared)
	bool Unknown_80C220(ViewClass *view);								// 0x80C220 (FAIL_IF '!PIPView': view's slot +0x1C; the scene, the camera, 'BONE_HOV01A'; G_ASSERT 'FloatParent')  // NAME 1 [view]: what ui_avatr.cpp passes (its CharacterViewClass)
	void Unknown_80C540(const Vector2 &size, const Vector3 &position, unsigned int sort_level);	// 0x80C540 (ui_avatr.cpp: (640, 480), (0, 0, 0), 0x1C; +0x64 / +0x68 the top left in 640x480)  // NAME 1 [size, position, sort_level]: what it does with them
	void Unknown_80E020(float x, float y);								// 0x80E020 (the pan +0xCC / +0xD0 moved and clamped: type 0 70 / 50, type 1 2 / 4.75)  // NAME 1 [x, y]: neutral
	void Unknown_80E190(float distance);								// 0x80E190 (the zoom: +0xB4 moved within +0xB0..+0xAC, the camera along its z)  // NAME 1 [distance]: neutral
	float Unknown_80E7A0();												// 0x80E7A0 (the pan / zoom speed: 60, or type 1 +0xB4 * 0.6)
	LoginUnknown18Class *Unknown_80E7D0();								// 0x80E7D0 (the software mouse: PIPView's task's +0x18, or slot +0x30's +0x4C's +0x1274)

	int Type;															// +0x30 (the constructor's: 0 or 1)  // NAME 1: neutral (D10)
	UIViewClass *PIPView;												// +0x34 (0x80C220: the view's slot +0x1C)  // NAME 3: the FAIL_IF's
	RefCountPtr<CameraClass> Camera;									// +0x38 (0x80C220; slot +0x90 sets its transform)  // NAME 1: neutral (D10): what it holds
	RefCountPtr<SimpleSceneClass> Scene;								// +0x3C (0x80C220; PIPView's 0x65B7A0 / 0x65B7E0)  // NAME 1: neutral (D10): what it holds
	float Unknown040;													// +0x40 the width (0x80C540; slot +0x30)
	float Unknown044;													// +0x44 the height
	unsigned int Unknown048;											// +0x48 the sort level (0x80C540; PIPView's 0x65B7A0)
	RefCountPtr<LightClass> Unknown04C;									// +0x4C the first light (slot +0x74)
	RefCountPtr<LightClass> Unknown050;									// +0x50 the second light (slot +0x74)
	RefCountPtr<RenderObjClass> Unknown054;								// +0x54 the model (slots +0x7C / +0x84; ui_avatr.cpp calls its slot +0x1C4 every frame: the CharacterView's AvatarRObj)
	Vector3 Unknown058;													// +0x58 the model's bounding sphere's centre
	Vector2 Unknown064;													// +0x64 the top left in 640x480 (0x80C540: Set)
	Matrix3D Unknown06C;												// +0x6C the camera before the pan (identity; slot +0x8C's Look_At)
	int Unknown09C;														// +0x9C the drag's last x (slot +0x2C)
	int Unknown0A0;														// +0xA0 the drag's last y (slot +0x2C)
	int Unknown0A4;														// +0xA4 the pointer's x in the picture (slot +0x30)
	int Unknown0A8;														// +0xA8 the pointer's y in the picture (slot +0x30)
	float Unknown0AC;													// +0xAC the farthest distance (the model's radius * +0xF0)
	float Unknown0B0;													// +0xB0 the nearest distance (the radius * +0xEC)
	float Unknown0B4;													// +0xB4 the distance
	float Unknown0B8;													// +0xB8 turn speed x (slot +0x78)
	float Unknown0BC;													// +0xBC turn speed y (slot +0x78)
	float Unknown0C0;													// +0xC0 pan speed x (slot +0x78)
	float Unknown0C4;													// +0xC4 pan speed y (slot +0x78)
	float Unknown0C8;													// +0xC8 zoom speed (slot +0x78)
	float Unknown0CC;													// +0xCC the pan x (0x80E020)
	float Unknown0D0;													// +0xD0 the pan y (0x80E020)
	RefCountPtr<RenderObjClass> FloatParent;							// +0xD4 'BONE_HOV01A' (0x80C220's G_ASSERT; the model on its bone 'NewShipScale')  // NAME 3: the assert's
	RefCountPtr<RenderObjClass> Unknown0D8;								// +0xD8 (only released: 0x80C180, 0x80C220, the destructor; its type is not known)
	bool Unknown0DC;													// +0xDC (slots +0x88 / +0x8C, type 1: the near view)
	float Unknown0E0;													// +0xE0 the turn's momentum x (slot +0x2C; slot +0x78 decays it by 0.99)
	float Unknown0E4;													// +0xE4 the turn's momentum y
	bool Unknown0E8;													// +0xE8 (true; slot +0x2C clears it while dragging, slot +0x78 does nothing without it)
	float Unknown0EC;													// +0xEC the nearest distance in radii (type 0 0.5, type 1 0.75)
	float Unknown0F0;													// +0xF0 the farthest distance in radii (type 0 2.1, type 1 3.0)
};

#endif // __SHIP_PIP_H
