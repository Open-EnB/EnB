// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\bar2d.h (PROVISIONAL, after bar2d.cpp's __FILE__
**	'e:\build\beta\G\app\Client\bar2d.cpp' 0xB91FFC; the client holds no string of this header)
**
**	bar2d.cpp's Bar2DClass: a flat bar (a DynamicMeshClass of eight vertices in one tri strip, size 0x150,
**	vtable 0xAFE228, 0xAFE210 / 0xAFE20C for the other bases) whose fill edge (vertices 2-5) moves with
**	an AccelClass percent; the filled part shades from the first color to the second, the empty part from
**	the third to the fourth.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef BAR2D_H  // NAME 3 [bar2d.h]: the header of bar2d.cpp
#define BAR2D_H

#include "..\..\ww3d2\dynamesh.h"
#include "..\shared\accel.h"

class SceneClass;

class Bar2DClass : public DynamicMeshClass  // NAME 2 [Bar2DClass]: bar2d.cpp's class (the file name, Westwood's Class suffix, Rect3DClass's precedent)
{
public:
	Bar2DClass(const Vector4 &color0, const Vector4 &color1, const Vector4 &color2, const Vector4 &color3,
		float height, float width, float x, float y, float percent, float rate, int orientation);	// 0x6B6900: x, y in 0..1 of the screen; orientation 1-3 turns it by 180, 90, 270 degrees  // NAME 2 [color0..3, height, width, x, y, percent, rate, orientation]: what it does with them
	Bar2DClass(const Bar2DClass &src);							// 0x6B74D0: G_ASSERT(0)
	Bar2DClass &operator=(const Bar2DClass &that);				// 0x6B75C0: G_ASSERT(0)
	virtual ~Bar2DClass();										// 0x6B7770

	virtual RenderObjClass *Clone() const;						// 0x6B77A0
	virtual int Class_ID() const { return 0x10004; }			// 0x6B7480  // NAME 2: E&B's class id
	virtual void Render(RenderInfoClass &rinfo);				// 0x6B7820
	virtual void On_Frame_Update();								// 0x6B7A70
	virtual void Notify_Added(SceneClass *scene);				// 0x6B7AC0: registered for frame updates
	virtual void Notify_Removed(SceneClass *scene);				// 0x6B7AF0

	void Set_Percent(float percent);							// 0x6B7B20: moves there at Rate  // NAME 2: what it does
	void Set_Percent_Now(float percent);						// 0x6B7BE0: jumps there  // NAME 2: what it does
	void Set_Colors(const Vector4 &color0, const Vector4 &color1, const Vector4 &color2, const Vector4 &color3);	// 0x6B7CA0  // NAME 2: what it does

protected:
	float Rate;													// +0xF4: the constructor's rate / 1000 (per millisecond)  // NAME 2: what it is
	AccelClass Percent;											// +0xF8  // NAME 2: what it holds
	float Width;												// +0x108: twice the constructor's width  // NAME 2: what it is
	float Height;												// +0x10C: twice the constructor's height  // NAME 2: what it is
	Vector4 Color0;												// +0x110: the start of the filled part  // NAME 2: Set_Colors' order
	Vector4 Color1;												// +0x120: the fill edge of the filled part  // NAME 2: Set_Colors' order
	Vector4 Color2;												// +0x130: the fill edge of the empty part  // NAME 2: Set_Colors' order
	Vector4 Color3;												// +0x140: the end of the empty part  // NAME 2: Set_Colors' order
};

#endif
