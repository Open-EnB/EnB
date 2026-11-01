// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\rect3d.h (PROVISIONAL, after rect3d.cpp's __FILE__
**	'e:\build\beta\G\app\Client\rect3d.cpp' 0xB92CE8; the client holds no string of this header)
**
**	rect3d.cpp's Rect3DClass: a flat four-vertex DynamicMeshClass (one tri strip, two polygons) in one
**	color, the size of DynamicMeshClass (0xF4), vtable 0xAFFB08 (0xAFFAF0 / 0xAFFAEC for the other bases).
**	fontsys_cursor.cpp's TextCursorClass derives from it; the HUD's 0x66CC00 makes four.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef RECT3D_H  // NAME 3 [rect3d.h]: the header of rect3d.cpp
#define RECT3D_H

#include "..\..\ww3d2\dynamesh.h"
#include "vector4.h"

class SceneClass;

class Rect3DClass : public DynamicMeshClass  // NAME 2 [Rect3DClass]: rect3d.cpp's class (the file name, Westwood's Class suffix)
{
public:
	Rect3DClass(const Vector4 &color, const Vector4 &rect);		// 0x6E2290: rect is x, y, width, height (y down by the height)  // NAME 2 [color, rect]: what it does with them
	Rect3DClass(const Rect3DClass &src);						// 0x6E26B0: G_ASSERT(0)
	Rect3DClass &operator=(const Rect3DClass &that);			// 0x6E2740: G_ASSERT(0)
	virtual ~Rect3DClass();										// 0x6E28F0

	virtual RenderObjClass *Clone() const;						// 0x6E2920
	virtual int Class_ID() const { return 0x1000A; }			// 0x685840 (the copy fontsys_cursor.obj emitted first)  // NAME 2: E&B's class id
	virtual void Notify_Added(SceneClass *scene);				// 0x6E29C0: registered for frame updates
	virtual void Notify_Removed(SceneClass *scene);				// 0x6E29F0

	void Set_Color(const Vector4 &color);						// 0x6E2A20: the four vertices' color  // NAME 2: what it does
	void Set_Rect(const Vector4 &rect);							// 0x6E2A90: the four vertices moved to rect  // NAME 2: what it does
};

#endif
