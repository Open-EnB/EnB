// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\target_reticle.h (name assumed from target_reticle.cpp, whose __FILE__
**	is 'e:\build\beta\G\app\client\target_reticle.cpp'; the client holds no string of this header)
**
**	The four-cornered reticle (hud01a) drawn round a target on the screen, coloured from the object's radar data,
**	with the distance and shield text beside it. 0x44 bytes, no virtuals.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __TARGET_RETICLE_H  // NAME 3 [target_reticle.h]: header name assumed from the client's target_reticle.cpp
#define __TARGET_RETICLE_H

#include "ref_ptr.h"
#include "vector2.h"

class SClient;
class SceneClass;
class RenderObjClass;
class MaterialInfoClass;
class VertexMaterialClass;
class FontSys;
class ClientObjectClass;

class TargetReticleClass	// NAME 3: class-from-file (D24) for target_reticle.cpp
{
public:
	enum { CORNER_COUNT = 4 };	// NAME 2: UL, UR, BR, BL

	TargetReticleClass(SClient *client, const RefCountPtr<SceneClass> &scene, int sort_level, bool show_text);	// 0x7C5100  // NAME 2 [sort_level, show_text]: what it does with them
	~TargetReticleClass();											// 0x7C55A0

	void Update(const Vector2 &pos, float size);					// 0x7C5690 (gutter_radar.cpp's 0x7B7040): pos in 0..1 of the screen; size in the same units  // NAME 2: what it does
	void Set_Client_Object(ClientObjectClass *co);					// 0x7C6060: the object the reticle marks, or NULL to hide it  // NAME 2: what it does

private:
	struct CornerStruct	// NAME 2: one corner's bone and material (constructor 0x7C54E0, destructor 0x7C5500)
	{
		int BoneIndex;									// +0x00  // NAME 2: what it holds
		RefCountPtr<MaterialInfoClass> Material;		// +0x04  // NAME 2: what it holds
		RefCountPtr<VertexMaterialClass> VertexMaterial;	// +0x08  // NAME 2: what it holds
	};

	SClient *Client;									// +0x00  // NAME 2: what it holds
	RefCountPtr<SceneClass> Scene;						// +0x04  // NAME 2: what it holds
	RefCountPtr<RenderObjClass> TargetRObj;				// +0x08 (the FAIL_IF's name)
	CornerStruct Corners[CORNER_COUNT];					// +0x0C  // NAME 2: what it holds
	RefCountPtr<FontSys> Text;							// +0x3C  // NAME 2: what it holds
	ClientObjectClass *ClientObject;					// +0x40 (the FAIL_IF's name)
};

#endif //__TARGET_RETICLE_H
