// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\gd_pip_scene.h (name assumed from gd_pip_scene.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\Client\gd_pip_scene.cpp'; the client holds no string of this header)
**
**	The gadget over a picture-in-picture scene's rectangle (pip_scene.h): GadgetClass with the scene (+0x30,
**	G_ASSERT 'PIPScene' l.50), where the pointer is in its viewport and whether it is inside. Table 0xAFA65C
**	(29 slots: GadgetClass's, +0x30 its own), constructor 0x66ACE0, destructor 0x66ADD0, 0x4C bytes. The base of
**	the map gadgets (gd_galaxy_map.cpp 0x668E30, gd_starmap.cpp 0x7B4700) and the ship's (0x66DA50).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef GD_PIP_SCENE_H  // NAME 3 [gd_pip_scene.h]: header name assumed from the client's gd_pip_scene.cpp
#define GD_PIP_SCENE_H

#include "always.h"
#include "ref_ptr.h"
#include "refcount.h"
#include "vector2.h"
#include "gadget.h"
#include "pip_scene.h"

class GadgetPipSceneClass : public GadgetClass  // NAME 3: class-from-file (D24) for app\client\gd_pip_scene.cpp
{
public:
	GadgetPipSceneClass(PipSceneClass *pip);			// 0x66ACE0 (G_ASSERT 'PIPScene' l.50; GadgetClass(0x1077))  // NAME 1 [pip]: what the callers pass
	virtual ~GadgetPipSceneClass();						// +0x00 0x66ADD0 (deleting 0x66ADA0)
	virtual bool Unknown_30(float x, float y);			// +0x30 0x66AE60 (FAIL_IF '!PIPScene' l.64: the pointer inside the scene's rectangle, Position set)  // NAME 1 [x, y]: GadgetClass's

	static bool Wrap_Pointer();							// 0x66B080 (the pointer at an edge of the window put to the opposite one; no caller)  // NAME 2: what it does
	void Set_Cursor(int cursor);						// 0x66B170 (FAIL_IF '!PIPScene' l.147, '!PIPScene->Peek_Client()', '!mouse': the software mouse's cursor)  // NAME 2: what it does; NAME 1 [cursor]: tsk_login.h's

	PipSceneClass *PIPScene;							// +0x30 (G_ASSERT 'PIPScene')  // NAME 3: the assert's
	RefCountPtr<RefCountClass> Unknown34;				// +0x34 (released by the destructor; nothing here sets it)
	Vector2 Position;									// +0x38 (the pointer in the viewport, in its pixels from the top left; 0x53F9E0 hands it to 0x7A72C0 by value)  // NAME 1: what slot +0x30 stores
	Vector2 WrapPosition;								// +0x40 (Position when the pointer was wrapped)  // NAME 1: what slot +0x30 stores
	bool WrapPointer;									// +0x48 (the derived gadgets set it: the pointer wraps at the window's edges, Position kept outside the rectangle too)  // NAME 1: what slot +0x30 does with it
	bool PointerInside;									// +0x49 (slot +0x30's answer; 0x693020 returns it)  // NAME 1: what slot +0x30 stores
};

#endif
