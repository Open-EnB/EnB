// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\user_click_indicator.h (name assumed from user_click_indicator.cpp)
**
**	The indicator of a clicked object (0x3C bytes): the model hud04a in the view's scene, its four corner bones
**	T / R / B / L pushed out to the object's bounding sphere and the whole spun about Z each frame. Its object is
**	0x6B3A90-0x6B4470 (user_click_indicator.cpp).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __USER_CLICK_INDICATOR_H  // NAME 3 [user_click_indicator.h]: header name assumed from the client's user_click_indicator.cpp
#define __USER_CLICK_INDICATOR_H

#include "ref_ptr.h"

class SceneClass;
class SphereClass;
class UIViewClass;
class RenderObjClass;
class MaterialInfoClass;
class VertexMaterialClass;

class UserClickIndicatorClass  // NAME 3: class-from-file (D24)
{
public:
	UserClickIndicatorClass();							// 0x6B3A90
	~UserClickIndicatorClass();							// 0x6B3C20
	bool Unknown_6B3D00(UIViewClass *view);				// 0x6B3D00 (into the view's scene, sized to the view plane)  // NAME 1 [view]: gutter_data.cpp's 0x7B6040's FAIL_IF
	bool Unknown_6B3ED0(const RefCountPtr<SceneClass> &scene);	// 0x6B3ED0 (into a scene, hidden)  // NAME 2 [scene]: the FAIL_IF's '!scene'
	void Unknown_6B3F50(const SphereClass &sphere);		// 0x6B3F50 (each frame)  // NAME 1 [sphere]: gadget.cpp's
	void Unknown_6B4400();								// 0x6B4400 (out of its scene)

	struct BoneStruct  // NAME 1: neutral (D10)
	{
		int Bone;										// +0x00 (Get_Bone_Index of the name, captured)  // NAME 1: what it holds
		MaterialInfoClass *MaterialInfo;				// +0x04 (the sub-object's Get_Material_Info)  // NAME 1: what it holds
		VertexMaterialClass *VertexMaterial;			// +0x08 (its Get_Vertex_Material(0))  // NAME 1: what it holds
	};

	RenderObjClass *Indicator;							// +0x00 (hud04a)  // NAME 3: the FAIL_IF's '!Indicator'
	BoneStruct Bones[4];								// +0x04 (T, R, B, L)  // NAME 1: what it holds
	float Size;											// +0x34 (the bones' offset: grows or shrinks to 0.8 of the sphere's radius)  // NAME 1: what it holds
	bool Unknown38;										// +0x38 (set while Size changes; the spin waits for it)  // NAME 1: neutral (D10)
};

#endif //__USER_CLICK_INDICATOR_H
