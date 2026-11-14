// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (PROVISIONAL). The class's own object (0x6E1AB0-0x6E2290) links after
**	local_interface_sort.obj (its <string> guard 0x6E1A80) and before rect3d.obj (0x6E2290), so its file name
**	sorts between the two; the client holds no string of it. Its functions are in unknown_6e1ab0.cpp.
**
**	Unknown6E1AB0Class: a flat DynamicMeshClass (DynamicMeshClass(4, 8)) of one color at a screen position
**	and size, the size of DynamicMeshClass (0xF4), vtable 0xAFF894 (0xAFF87C / 0xAFF878 for the other bases).
**	Line2DClass (line2d.h) derives from it and disables the two setters of position and size.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UNKNOWN_6E1AB0_H  // NAME 1 [unknown_6e1ab0.h]: neutral (D10): the object's file is not known
#define UNKNOWN_6E1AB0_H

#include "..\..\ww3d2\dynamesh.h"
#include "vector2.h"
#include "vector4.h"

class Unknown6E1AB0Class : public DynamicMeshClass  // NAME 1 [Unknown6E1AB0Class]: neutral (D10)
{
public:
	Unknown6E1AB0Class(const Vector4 &color, const Vector2 &position, const Vector2 &size);	// 0x6E1AB0  // NAME 1 [color, position, size]: what Line2DClass passes (its color, (0, 0), (0.1, 0.1))
	virtual ~Unknown6E1AB0Class() {}														// 0x6DFA10 (inline: line2d.obj's copy, which 0x6E1F30 calls)

	virtual int Class_ID() const { return 0x10009; }										// 0x6DF960 (line2d.obj's copy, shared by both tables)  // NAME 2: E&B's class id
	virtual void Set_Color(const Vector4 &color);											// 0x6E21D0 (slot 122: the four vertices' color)  // NAME 1: what it does
	virtual void Set_Size(const Vector2 &size);											// 0x6E20A0 (slot 123: vertices 1-3 moved by the size, clamped to 0..1)  // NAME 1: what it does
	virtual void Set_Screen_Position(const Vector2 &position);								// 0x6E1F60 (slot 124)  // NAME 1: what it does
};

#endif
