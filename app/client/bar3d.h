// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\bar3d.h (PROVISIONAL, after bar3d.cpp's __FILE__
**	'e:\build\beta\G\app\Client\bar3d.cpp' 0xB92054; the client holds no string of this header)
**
**	bar3d.cpp's Bar3DClass: a bar floating over a target render object (a DynamicMeshClass of eight
**	vertices in two tri strips of two vertex materials, size 0x124, vtable 0xAFE4A0, 0xAFE488 / 0xAFE484
**	for the other bases). It takes its bounding volumes from the target, grows with the camera distance
**	(never below its own size) and moves its fill edge (vertices 2-5) with an AccelClass percent.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef BAR3D_H  // NAME 3 [bar3d.h]: the header of bar3d.cpp
#define BAR3D_H

#include "..\..\ww3d2\dynamesh.h"
#include "..\shared\accel.h"

class SceneClass;

class Bar3DClass : public DynamicMeshClass  // NAME 2 [Bar3DClass]: bar3d.cpp's class (the file name, Westwood's Class suffix, Rect3DClass's precedent)
{
public:
	Bar3DClass(float r0, float g0, float b0, float a0, float r1, float g1, float b1, float a1,
		float height, float width, float height_scale, float width_scale, float offset, float offset_scale,
		float percent, float rate);								// 0x6B7DE0: the filled part's emissive color and opacity, the empty part's, the minimum sizes, their growth with the distance, the height over the target  // NAME 2 [all parameters]: what it does with them
	Bar3DClass(const Bar3DClass &src);							// 0x6B8570
	Bar3DClass &operator=(const Bar3DClass &that);				// 0x6B8650: G_ASSERT(0)
	virtual ~Bar3DClass();										// 0x6B87E0

	virtual RenderObjClass *Clone() const;						// 0x6B8870
	virtual int Class_ID() const { return 0x10005; }			// 0x6B84C0  // NAME 2: E&B's class id
	virtual void Render(RenderInfoClass &rinfo);				// 0x6B88F0
	virtual void On_Frame_Update();								// 0x6B8C10
	virtual void Notify_Added(SceneClass *scene);				// 0x6B8C80: registered for frame updates
	virtual void Notify_Removed(SceneClass *scene);				// 0x6B8CB0
	virtual void Get_Obj_Space_Bounding_Sphere(SphereClass &sphere) const { Target->Get_Obj_Space_Bounding_Sphere(sphere); }	// 0x6B84E0
	virtual void Get_Obj_Space_Bounding_Box(AABoxClass &box) const { Target->Get_Obj_Space_Bounding_Box(box); }				// 0x6B8510

	void Set_Target(RenderObjClass *target);					// 0x6B8D70  // NAME 2: what it does
	void Set_Colors(float r0, float g0, float b0, float a0, float r1, float g1, float b1, float a1);	// 0x6B8DE0: the two vertex materials  // NAME 2: what it does

protected:
	virtual void Update_Cached_Bounding_Volumes() const;		// 0x6B8CE0: the target's

	RenderObjClass *Target;										// +0xF4  // NAME 2: what it is
	float Offset;												// +0xF8: the minimum height over the target's bounding sphere  // NAME 2: what it is
	float OffsetScale;											// +0xFC: per unit of camera distance  // NAME 2: what it is
	float Height;												// +0x100: the minimum height  // NAME 2: what it is
	float Width;												// +0x104: the minimum width  // NAME 2: what it is
	float HeightScale;											// +0x108: per unit of camera distance  // NAME 2: what it is
	float WidthScale;											// +0x10C: per unit of camera distance  // NAME 2: what it is
	float Rate;													// +0x110: the constructor's rate / 1000 (per millisecond)  // NAME 2: what it is
	AccelClass Percent;											// +0x114  // NAME 2: what it holds
};

#endif
