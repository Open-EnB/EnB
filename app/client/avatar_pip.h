// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\avatar_pip.h (name assumed from avatar_pip.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\Client\avatar_pip.cpp'; the client holds no string of this header)
**
**	The avatar picture-in-picture: an avatar (a CharacterViewAvatarClass) or another render object drawn in its
**	own 3-D scene (a SimpleSceneClass, a camera, a point light and a 0xC4-byte render object that carries the
**	model) into the current view's PIP sort list, placed over a viewport render object ('vp': the mesh's
**	AVATAR_YOU / AVATAR_THEM). No virtual functions: 0x3C bytes, constructor 0x7E31B0, destructor 0x7E3410.
**	SClient's +0xCC / +0xD0 (PlayerPIP / OtherPIP), ui_chat_panel.cpp's +0x110, ui_ptrade.cpp's.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __AVATAR_PIP_H  // NAME 3 [avatar_pip.h]: header name assumed from the client's avatar_pip.cpp
#define __AVATAR_PIP_H

#include "always.h"
#include "ref_ptr.h"
#include "vector2.h"
#include "vector3.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\shared\ro_proxy.h"

class SClient;
class UIViewClass;
class CameraClass;
class SimpleSceneClass;
class LightClass;
class CharacterViewAvatarClass;

class AvatarPipClass  // NAME 3: class-from-file (D24) for app\client\avatar_pip.cpp
{
public:
	AvatarPipClass();									// 0x7E31B0
	~AvatarPipClass();									// 0x7E3410

	void Unknown_7E35D0();								// 0x7E35D0 (the model off, then everything released; 0x7E36B0 inlines it)
	bool Unknown_7E36B0(SClient *sclient, const RefCountPtr<RenderObjClass> &vp, bool arg);	// 0x7E36B0 (FAIL_IF '!vp', '!sclient', '!Client->CurrentView', '!PIPView': the scene, the camera)  // NAME 2 [sclient, vp]: the FAIL_IFs'; NAME 1 [arg]: neutral
	bool Unknown_7E3A20(const char *name, unsigned id, CharacterViewAvatarClass *avatar);	// 0x7E3A20 (FAIL_IF '!Client', '!aux', '!npc_id' 'PIPAvatarID': the avatar of id, or the given one)  // NAME 1 [name, id, avatar]: what it does with them
	void Unknown_7E4350();								// 0x7E4350 (the model off: the light and the carrier out of the scene, the scene out of PIPView's sort list)
	void Unknown_7E43E0(const RefCountPtr<CharacterViewAvatarClass> &avatar);	// 0x7E43E0 (the avatar as the model)  // NAME 1 [avatar]: what it holds
	void Unknown_7E4520(const RefCountPtr<RenderObjClass> &model);	// 0x7E4520 (the model onto the carrier, the scene into PIPView's sort list at the vp's sort level + 1)  // NAME 1 [model]: what it holds
	void Unknown_7E4740(bool force);					// 0x7E4740 (the camera's viewport and view plane from the vp's position and size, unless unchanged)  // NAME 1 [force]: what it does

	SClient *Client;									// +0x00 (0x7E36B0's FAIL_IF '!Client->CurrentView')  // NAME 3: the FAIL_IF's
	UIViewClass *PIPView;								// +0x04 (the current view's slot +0x20)  // NAME 3: the FAIL_IF's
	RefCountPtr<CameraClass> Camera;					// +0x08  // NAME 1: neutral (D10): what it holds
	RefCountPtr<SimpleSceneClass> Scene;				// +0x0C  // NAME 1: neutral (D10): what it holds
	RefCountPtr<RenderObjClass> Unknown010;				// +0x10 the vp (0x7E36B0)
	Vector2 Unknown014;									// +0x14 the vp's size (its bounding box's extent * 2)
	Vector3 Unknown01C;									// +0x1C the vp's position
	RefCountPtr<LightClass> Unknown028;					// +0x28 the point light (the constructor's)
	RefCountPtr<CharacterViewAvatarClass> Unknown02C;	// +0x2C the avatar (0x7E43E0; ui_chat_panel.cpp tests its +0x204)
	RefCountPtr<RenderObjClass> Unknown030;				// +0x30 the model (0x7E4520)
	RefCountPtr<ProxyRObj> Unknown034;			// +0x34 the model's carrier (the constructor's)
	bool Unknown038;									// +0x38 (0x7E36B0's arg; 0x7E3A20 hands it to 0x786410)
};

#endif // __AVATAR_PIP_H
