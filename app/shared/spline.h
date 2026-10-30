// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\spline.h (its __FILE__ in the client's asserts)
**
**	The minimum of E&B's spline.h that spln_db.cpp's evaluation helpers need (0x485620, 0x485790):
**	a resizable float array (Resize 0x483820 in spline.cpp, assert 'size > 0') and a colour array of
**	three of them. The asserts sit on the client's lines: 'index < Size' on line 48 (the array's
**	Set) and on line 177 (the colour array's Set). The rest of spline.h (the client's
**	spline.cpp 0x483xxx) is not rebuilt; the classes' names and the unknown first members are ours.
*/



#if defined(_MSC_VER)
#pragma once
#endif

// NAME 2 [spline.h]: the client's __FILE__
#ifndef __SPLINE_H
#define __SPLINE_H

#include "vector3.h"
#include "unknown_debug.h"

// NAME 1: a float array of a set size (0x10 bytes: +0x04 the values, +0x08 their count, +0x0C the last index)
class SplineFloatArrayClass
{
public:
// NAME 2: after what it does (0x483820: deletes the values, news size of them set to value)
	void						Resize(unsigned int size, float value);














	void						Set(int index, float value)
	{
		G_ASSERT(index < Size);
		Data[index] = value;
	}

// NAME 1: members after their offsets (Resize 0x483820); +0x00 unknown
	int						m_Unknown00;
	float *					Data;
	int						Size;
	int						Last;
};
















































































































// NAME 1: three float arrays (red, green, blue) and a count (0x38 bytes: +0x04, +0x14, +0x24, +0x34)
class SplineColorArrayClass
{
public:
// NAME 2: after what it does (inlined in 0x485790)
	void						Set(int index, const Vector3 &color)
	{
		G_ASSERT(index < Size);
		Red.Set(index, color.X);
		Green.Set(index, color.Y);
		Blue.Set(index, color.Z);
	}

// NAME 1: members after their offsets (0x485790); +0x00 unknown
	int						m_Unknown00;
	SplineFloatArrayClass	Red;
	SplineFloatArrayClass	Green;
	SplineFloatArrayClass	Blue;
	int						Size;
};

#endif //__SPLINE_H
