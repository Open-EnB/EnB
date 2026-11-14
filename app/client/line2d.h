// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\line2d.h (the inline setters' __FILE__ 0xB92B80).
**
**	Line2DClass: a screen-space line from one point to another (0..1 coordinates, y down), a quad of
**	Unknown6E1AB0Class turned to the line's angle, its width in pixels (0 keeps the width it has).
**	0xF4 bytes, vtable 0xAFF350 (0xAFF338 / 0xAFF334 for the other bases). Line2DClass's code is
**	line2d.cpp's (0x6DF890-0x6DFD80); ui_mount.cpp and intrlist.cpp make them.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef LINE2D_H  // NAME 3 [line2d.h]: the client's __FILE__
#define LINE2D_H

#include "unknown_6e1ab0.h"
#include "..\shared\g_assert.h"

class Line2DClass : public Unknown6E1AB0Class  // NAME 2 [Line2DClass]: the header's name, Westwood's Class suffix (as Rect3DClass)
{
public:
	Line2DClass(const Vector4 &color, const Vector2 &from, const Vector2 &to, float width);	// 0x6DF890  // NAME 1 [color, from, to, width]: what it does with them
	virtual ~Line2DClass() {}																// 0x6DFA30

	void Set(const Vector2 &from, const Vector2 &to, float width);							// 0x6DFA50  // NAME 1: what it does









	// A line is placed by its two points: its base's position and size setters assert.
	virtual void Set_Size(const Vector2 &size) { G_ASSERT(0); }							// 0x6DF980 (line 39)
	virtual void Set_Screen_Position(const Vector2 &position) { G_ASSERT(0); }				// 0x6DF9B0 (line 40)
};

#endif
