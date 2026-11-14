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
#include "g_assert.h"
class INIClass;
// NAME 1: a float array of a set size (0x10 bytes: +0x00 the vtable, +0x04 the values, +0x08 their count, +0x0C the last index)
class SplineFloatArrayClass
{
public:
// NAME 2 [ctors, ~, Load, Save]: what they do; the ints are inline (unknown_480b10.cpp 0x480B10 news one: +0x04, +0x08 zeroed)
							SplineFloatArrayClass(void) : Data(NULL), Size(0) {}
							SplineFloatArrayClass(int size);	// 0x4837B0 (spline.cpp: Size = size, Last = size - 1, new float[size])
	virtual					~SplineFloatArrayClass(void);	// +0x00 0x4837F0 (spline.cpp; scalar deleting 0x4815E0)
// NAME 2: after what it does (0x483820: deletes the values, news size of them set to value)
	void						Resize(unsigned int size, float value);
	void						Load(INIClass &ini, const char *section, const char *prefix);	// 0x4838B0 (spline.cpp: assert 'Size >= 2')
	void						Save(INIClass &ini, const char *section, const char *prefix);	// 0x483AF0 (spline.cpp)
// NAME 1 [Evaluate, Get_Value, Find_Index]: what they do (spline.inl, assert line 33)
	virtual float				Evaluate(float t);	// +0x04 0x481520 (spline.inl)
	float						Get_Value(float t);	// 0x481960 (spline.inl: the value at t, 0..1, linear between the points)
	int							Find_Index(float &t);	// 0x4818C0 (spline.inl: the point before t; t becomes the fraction)
// NAME 1 [Read, Create]: what they do (0x483A20 has no caller: Load 0x4838B0 and Create 0x483BB0 inline it; Create has none)
	void						Read(INIClass &ini, const char *section, const char *prefix);	// 0x483A20 (spline.cpp: assert 'Size >= 2' line 67)
	static SplineFloatArrayClass *	Create(INIClass &ini, const char *section, const char *prefix);	// 0x483BB0 (spline.cpp: NULL when '%sC' is not above 0)

	void						Set(int index, float value)
	{
		G_ASSERT(index < Size);
		Data[index] = value;
	}

// NAME 1: members after their offsets (Resize 0x483820, Get_Value 0x481960); +0x00 the vtable 0xAE9FAC

	float *					Data;
	int						Size;
	int						Last;
};


// NAME 1: a float array read at t plus a phase (0x483CF0 adds the float at +0x10 to t; the class is ours, no caller, no vtable)
class SplineFloatArrayOffsetClass : public SplineFloatArrayClass
{
public:
	float						Get_Value(float t);	// 0x483CF0 (spline.cpp)
	float						Offset;
};







































































































// NAME 1: three float arrays (red, green, blue) and a count (0x3C bytes: +0x04, +0x14, +0x24, +0x34, +0x38)
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
// NAME 2 [ctor, ~, Load, Save, Evaluate, Get_Value]: what they do (unknown_480b10.cpp 0x480B10 news one)
							SplineColorArrayClass(void) : Red(0), Green(0), Blue(0), Size(0), Last(-1) {}
							SplineColorArrayClass(int size) : Red(size), Green(size), Blue(size), Size(size), Last(size - 1) {}	// inlined in 0x4842E0
	virtual					~SplineColorArrayClass(void) {}	// +0x00 0x481840 (scalar deleting 0x481810)
	void						Load(INIClass &ini, const char *section, const char *prefix);	// 0x483DB0 (spline.cpp)
	void						Save(INIClass &ini, const char *section, const char *prefix);	// 0x483EE0 (spline.cpp)
	virtual Vector3				Evaluate(float t);	// +0x04 0x481610 (spline.inl)
	Vector3						Get_Value(float t);	// inlined in 0x4811A0 (spline.inl)
	static SplineColorArrayClass *	Create(INIClass &ini, const char *section);	// 0x4842E0 (spline.cpp: 'XC', then X, Y, Z)
// NAME 1: members after their offsets (0x485790, 0x480B10); +0x00 the vtable 0xAE9FB8
	SplineFloatArrayClass	Red;
	SplineFloatArrayClass	Green;
	SplineFloatArrayClass	Blue;
	int						Size;
	int						Last;
};

#include "spline.inl"

#endif //__SPLINE_H
