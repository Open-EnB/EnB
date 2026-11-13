// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\debris.h (PROVISIONAL, D137: debris.cpp's header, after its __FILE__
**	'e:\build\beta\G\app\client\debris.cpp' 0xB921B4; the client holds no string of this header)
**
**	debris.cpp's DebrisFieldClass: the space dust drawn around the camera (0x108 bytes, tables 0xAFE714 / 0xAFE6FC /
**	0xAFE6F8), one point group or one line group over share buffers of Count entries; debrsint.cpp's
**	DebrisInterfaceClass makes one per settings.ini Debris%02d section.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef DEBRIS_H  // NAME 3 [debris.h]: the header of debris.cpp
#define DEBRIS_H

#include "rendobj.h"
#include "vector3.h"
#include "vector4.h"
#include "sharebuf.h"
#include "pointgr.h"
#include "linegrp.h"

class TextureClass;

class DebrisFieldClass : public RenderObjClass	// NAME 3: the profile name's ('DebrisFieldClass::Render' 0xB921E4)
{
public:
	DebrisFieldClass(float spacing, float size, float min_dist, float max_dist, float r, float g, float b, float a, bool additive, TextureClass *texture, float fade_in, float fade_out, int primitive, unsigned rows_columns_log2, float blur_time);	// 0x6B8F20  // NAME 2 [all]: the Debris%02d keys they come from
	DebrisFieldClass(const DebrisFieldClass &src);		// 0x6B96A0: G_ASSERT(0)
	DebrisFieldClass &operator=(const DebrisFieldClass &that);	// 0x6B9850: G_ASSERT(0)
	virtual ~DebrisFieldClass();						// 0x6B9990 (deleting 0x6B9670)

	virtual RenderObjClass *Clone(void) const;			// 0x6B9B70
	virtual int Class_ID(void) const { return 0x10006; }	// 0x6B9620  // NAME 2: E&B's class id
	virtual int Get_Num_Polys(void) const { if (PointGroup) { return PointGroup->Get_Polygon_Count(); } return LineGroup->Get_Polygon_Count(); }	// 0x6B9640
	virtual void Render(RenderInfoClass &rinfo);		// 0x6B9BF0 (NOT EXACT)
	int Unknown_6BB440();								// 0x6BB440 (0 or 1: what debrsint.cpp's keys 9 / 10 scale, the alpha or the colour)
	void Unknown_6BB470();								// 0x6BB470 (the shader from Additive / UnknownBC; after +0xBC changes)
	void Unknown_6BB640(const Vector3 &offset);			// 0x6BB640 (the main view's wind, through debrsint.cpp's 0x53E470)  // NAME 1 [offset]: neutral
	void Set_Color(float r, float g, float b) { Color.X = r; Color.Y = g; Color.Z = b; } float Get_Spacing() { return(Spacing); } float Get_Size() { return(Size); } float Get_Min_Dist() { return(MinDist); } float Get_Max_Dist() { return(MaxDist); } const Vector3 &Get_Color() { return(Color); } float Get_Alpha() { return(Alpha); }	// (debrsint.cpp's 0x53E220: the floats through the FPU)  // NAME 2: what it sets

	float Spacing;										// +0x8C  // NAME 2: the key's
	float Size;											// +0x90  // NAME 2: the key's
	float MinDist;										// +0x94  // NAME 2: the key's
	float MaxDist;										// +0x98  // NAME 2: the key's
	Vector3 Color;										// +0x9C 'R' 'G' 'B'  // NAME 2: the keys'
	float Alpha;										// +0xA8 'A'  // NAME 2: the key's
	float FadeIn;										// +0xAC  // NAME 2: the key's
	float FadeOut;										// +0xB0  // NAME 2: the key's
	int UnknownB4;										// +0xB4 (the constructor: 10000; every share buffer's count)
	bool Additive;										// +0xB8  // NAME 2: the key's
	int UnknownBC;										// +0xBC (0, 1, 2: debrsint.cpp's key 11 cycles it; the constructor: 2)
	PointGroupClass *PointGroup;						// +0xC0 (primitives 0 / 1)  // NAME 2: its type
	LineGroupClass *LineGroup;							// +0xC4 (primitives 2 / 3)  // NAME 2: its type
	float UnknownC8;									// +0xC8 (0..1; the constructor: 1.0)
	ShareBufferClass<Vector3> *UnknownCC;				// +0xCC
	ShareBufferClass<Vector4> *UnknownD0;				// +0xD0
	int UnknownD4;										// +0xD4 (Render: the count filled)
	ShareBufferClass<unsigned char> *UnknownD8;			// +0xD8 (point groups)
	ShareBufferClass<Vector3> *UnknownDC;				// +0xDC (line groups)
	ShareBufferClass<Vector4> *UnknownE0;				// +0xE0 (line groups, not additive: 0x6BB470)
	ShareBufferClass<float> *UnknownE4;					// +0xE4 (line groups)
	Vector3 UnknownE8;									// +0xE8 (0x6BB640: the last offset's change)
	float BlurTime;										// +0xF4  // NAME 2: the key's
	Vector3 UnknownF8;									// +0xF8 (0x6BB640: the last offset)
	int Primitive;										// +0x104 (0, 1 points; 2, 3 lines)  // NAME 2: the key's
};

#endif
