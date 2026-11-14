// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\gauge3d.h (PROVISIONAL, after gauge3d.cpp's __FILE__
**	'e:\build\beta\G\app\Client\gauge3d.cpp' 0xB928A4; the client holds no string of this header)
**
**	gauge3d.cpp's Gauge3DClass: a flat four-vertex DynamicMeshClass (as Rect3DClass) whose filled part
**	follows an AccelClass value 0..1 from one edge, in a color that can fade from one color to another.
**	0x154 bytes, vtable 0xAFEF20 (0xAFEF08 / 0xAFEF04 for the other bases). Nothing in the client
**	makes one (no callers of 0x6D9360).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef GAUGE3D_H  // NAME 3 [gauge3d.h]: the header of gauge3d.cpp
#define GAUGE3D_H

#include "..\..\ww3d2\dynamesh.h"
#include "vector2.h"
#include "vector4.h"
#include "..\shared\accel.h"

class SceneClass;
class RenderInfoClass;

class Gauge3DClass : public DynamicMeshClass  // NAME 2 [Gauge3DClass]: gauge3d.cpp's class (the file name, Westwood's Class suffix, as Rect3DClass)
{
public:
	// rect is x, y, width, height (y down by the height); value 0..1; speed per second; direction
	// the edge the gauge fills from (0 left, 1 right, 2 bottom, 3 top)
	Gauge3DClass(const Vector4 &color0, const Vector4 &color1, const Vector4 &color2, const Vector4 &rect,
		float value, float speed, int flags, int direction);	// 0x6D9360  // NAME 2 [all parameters]: what it does with them
	Gauge3DClass(const Gauge3DClass &src);						// 0x6D9980: G_ASSERT(0)
	Gauge3DClass &operator=(const Gauge3DClass &that);			// 0x6D9A50: G_ASSERT(0)
	virtual ~Gauge3DClass();									// 0x6D9C00

	virtual RenderObjClass *Clone() const;						// 0x6D9C30
	virtual int Class_ID() const { return 0x1000B; }			// 0x6D9930  // NAME 2: E&B's class id
	virtual void Render(RenderInfoClass &rinfo);				// 0x6D9CB0
	virtual void On_Frame_Update();								// 0x6DA1E0
	virtual void Notify_Added(SceneClass *scene);				// 0x6DA260: registered for frame updates
	virtual void Notify_Removed(SceneClass *scene);				// 0x6DA2A0

	void Set_Value(float value);								// 0x6DA2D0: at once  // NAME 2: what it does
	void Set_Target(float value);								// 0x6DA360: moving at the speed  // NAME 2: what it does
	void Set_Colors(const Vector4 &color0, const Vector4 &color1);	// 0x6DA3E0  // NAME 2: what it does

	enum {
		FLAG_FADE = 0x01,			// NAME 1: the color fades from Color0 to Color1 with the value
		FLAG_KEEP_EMPTY = 0x10,		// NAME 1: the empty edge's two vertices keep their color
	};

protected:
	int Direction;					// +0xF4  // NAME 2: the edge the gauge fills from
	int Flags;						// +0xF8  // NAME 2: FLAG_*
	bool Dirty;						// +0xFC  // NAME 2: the vertices need moving
	float Speed;					// +0x100: per millisecond  // NAME 2: the AccelClass rate
	AccelClass Value;				// +0x104  // NAME 2: the gauge's value 0..1
	Vector2 Position;				// +0x114  // NAME 2: the rectangle's left, top (y down by the height)
	Vector2 Size;					// +0x11C  // NAME 2: width, height; the copy constructor copies only Size
	Vector4 Color0;					// +0x124  // NAME 1: the color at value 0
	Vector4 Color1;					// +0x134  // NAME 1: the color at value 1
	Vector4 Color2;					// +0x144  // NAME 1: stored, never read
};

#endif
