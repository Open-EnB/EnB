// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\linebox.h (PROVISIONAL, after linebox.cpp's __FILE__
**	'e:\build\beta\G\app\client\linebox.cpp' 0xB92BAC; the client holds no string of this header)
**
**	linebox.cpp's LineBoxClass: a box drawn as twelve Line3DClass edges, either around a target render
**	object (its bounding sphere) or around its own position and extent; the edges keep their width and a
**	minimum size on screen by scaling with the camera distance. 0xAC bytes, vtable 0xAFF5C4 (0xAFF5AC /
**	0xAFF5A8 for the other bases). Nothing in the client makes one (the constructor has no callers).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef LINEBOX_H  // NAME 3 [linebox.h]: the header of linebox.cpp
#define LINEBOX_H

#include "..\..\ww3d2\rendobj.h"
#include "vector3.h"

class Line3DClass;
class SceneClass;

class LineBoxClass : public RenderObjClass  // NAME 2 [LineBoxClass]: linebox.cpp's class (the file name, Westwood's Class suffix)
{
public:
	LineBoxClass(float r, float g, float b, float width, float min_size);		// 0x6DFD80  // NAME 1 [r, g, b, width, min_size]: what it does with them
	LineBoxClass(const LineBoxClass &src);										// 0x6DFFD0: G_ASSERT(0)
	LineBoxClass &operator=(const LineBoxClass &that);							// 0x6E00C0: G_ASSERT(0)
	virtual ~LineBoxClass();													// 0x6E0100

	virtual RenderObjClass *Clone() const;										// 0x6E0240
	virtual int Class_ID() const { return 0x10007; }							// 0x6DFF40  // NAME 2: E&B's class id
	virtual int Get_Num_Polys() const;											// 0x6E0C20
	virtual void Render(RenderInfoClass &rinfo);								// 0x6E0350
	virtual void On_Frame_Update() { Invalidate_Cached_Bounding_Volumes(); }	// 0x6DFF60
	virtual void Notify_Added(SceneClass *scene);								// 0x6E0910: registered for frame updates
	virtual void Notify_Removed(SceneClass *scene);								// 0x6E0940
	virtual void Get_Obj_Space_Bounding_Sphere(SphereClass &sphere) const;		// 0x6E0970
	virtual void Get_Obj_Space_Bounding_Box(AABoxClass &box) const;				// 0x6E0A30
	virtual void Update_Cached_Bounding_Volumes() const;						// 0x6E0AC0

	virtual int Get_Num_Lines() const { return NumLines; }						// 0x6DFF80 (slot 113)  // NAME 2: the assert's 'NumLines'
	virtual Line3DClass *Get_Line(int index);									// 0x6E0C50 (slot 114: add-refed)  // NAME 1: what it returns

	void Set_Target(RenderObjClass *target);									// 0x6E0CD0: drawn around target's bounds  // NAME 1: what it does
	void Set_Box(Vector3 max, Vector3 min);										// 0x6E0D40: no target, centred between the corners  // NAME 1 [Set_Box, max, min]: what it does

protected:
	Line3DClass **Lines;														// +0x8C (NumLines of them)  // NAME 2: the asserts' 'NumLines'
	RenderObjClass *Target;														// +0x90  // NAME 1: what it holds
	Vector3 Extent;																// +0x94 (half the box, without a target)  // NAME 1: what it holds
	int NumLines;																// +0xA0 (12)  // NAME 2: the assert 'index < NumLines'
	float Width;																// +0xA4 (times the camera distance)  // NAME 1: what Render does with it
	float MinSize;																// +0xA8 (with a target: the box's least half size, times half the distance)  // NAME 1: what Render does with it
};

#endif
