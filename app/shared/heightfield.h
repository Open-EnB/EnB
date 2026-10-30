// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\ (file name unknown: no assert names it; neutral
**	name, D10)
**
**	The terrain's height field, from a land ini's [HEIGHTMAP] section: HEIGHT_MAP (an ASCII PGM),
**	HSCALE (the side length in world units), VSCALE (the height of the PGM's maxval), WRAPPING,
**	FLIP_PGM. The field is centred on the origin: vertex (i, j) is at x = (i - Size / 2) * Spacing,
**	y = (j - Size / 2) * Spacing.
**
**	The client: ctor 0x4634A0, dtor 0x463790, Load 0x4634F0 (static, called by LandClass::Load_Assets
**	0x4691D0 with a maximum size from the graphics options), the PGM reader 0x4637C0, Sample 0x463BF0,
**	the vertex accessors 0x463C80-0x463E70, the height at a position 0x464600 (two triangles per
**	cell), the ray and box collision code 0x463F00-0x465D30 (not rebuilt). VIDTEST's 2002 copy:
**	0x4023D0, 0x402420, 0x4026B0, Sample 0x402AE0. The client doesn't name the class (D24):
**	HeightFieldClass is descriptive. The object is 0x98 bytes; the heights are 16-bit, in a buffer
**	aligned to 64 bytes (+0x04 is the allocation), indexed (j & Mask) << Log2 | (i & Mask).
*/



#if defined(_MSC_VER)
#pragma once
#endif

// NAME 2 [heightfield.h]: describes the class; no assert or __FILE__ names the file
#ifndef __HEIGHTFIELD_H
#define __HEIGHTFIELD_H

#include "vector3.h"
#include <vector>

class Vector2;
class RayCollisionTestClass;
class AABoxCollisionTestClass;
class OBBoxCollisionTestClass;
class OBBoxClass;

// NAME 2: after land.ini's [HEIGHTMAP] / HEIGHT_MAP; the client names no class
class HeightFieldClass
{
public:
	HeightFieldClass(void);
	~HeightFieldClass(void);

// NAME 2: methods after what they do (the client's 0x4634F0, 0x4637C0, 0x463BF0, 0x463C80-0x463E70, 0x464600)
	static HeightFieldClass *	Load(const char *ini_name, unsigned int max_size);
	bool						Load_PGM(const char *filename, bool flip, unsigned int max_size);
	Vector3					Sample(const Vector2 &uv);
	float						Get_Height(unsigned int i, unsigned int j);
	float						Get_Height_2(unsigned int i, unsigned int j);	// NAME 1: the same code at a second address (0x463CD0)
	void						Set_Height(float height, unsigned int i, unsigned int j);
	void						Set_Height_2(float height, unsigned int i, unsigned int j);	// NAME 1: the same code at a second address (0x463D90)
	void						World_To_Grid(const Vector2 &pos, float *i, float *j);
	void						Get_Vertex(unsigned int i, unsigned int j, Vector3 *out);
	void						Get_Downhill(Vector3 *direction, const Vector2 &pos);	// NAME 2: towards the cell's lowest corner below the point (0x463F00)
	void						Get_Normal(Vector3 *normal, const Vector2 &pos);	// NAME 2: the triangle's normal (0x464290)
	float						Get_Height(const Vector3 &pos);	// NAME 2: x and y of a position (0x464600; its callers pass a Vector3, 0x70E3B0 reads its z after)
// NAME 2: after LandClass's Cast_Ray / Cast_AABox / Cast_OBBox (0x46C680-0x46C6E0), which forward to them (0x4647C0, 0x465030, 0x4654E0; not rebuilt)
	bool						Cast_Ray(RayCollisionTestClass &raytest);
	bool						Cast_AABox(AABoxCollisionTestClass &boxtest);
	bool						Cast_OBBox(OBBoxCollisionTestClass &boxtest);
// NAME 2: the triangles (three corners each) of the cells under a box, for the land's render 0x474B30 (0x465D30)
	void						Get_Triangles(const OBBoxClass &box, std::vector<Vector3> &verts);

// NAME 2: ours (VIDTEST's land, functional): the grid clamped at the edges, world coordinates
	int						Get_Size(void) const					{ return Size; }
	float						Get_Spacing(void) const				{ return XSpacing; }
	float						Get_Extent(void) const				{ return XSpacing * Size; }
	float						Get_Vertex_Height(int i, int j) const;
	Vector3					Get_Vertex(int i, int j) const;
	Vector3					Get_Normal(int i, int j) const;
	float						Get_Height(float x, float y) const;	// bilinear, world coordinates
	void						Sample(Vector3 *out, float u, float v) const;	// u, v in [0, 1], bilinear
	const char *			Get_Name(void) const					{ return Name; }
	unsigned int			Get_Log2(void) const					{ return Log2; }	// NAME 2: the land's Load_Assets reads them (0x4691D0)
	unsigned int			Get_Count(void) const				{ return Count; }
	float						Get_YSpacing(void) const			{ return YSpacing; }	// NAME 2: the land's Update_Patches reads them (0x46B3C0)
	unsigned short			Get_Max_Value(void) const			{ return MaxValue; }
	unsigned int			Get_Mask(void) const					{ return Mask; }	// NAME 2: the land's horizon scanners read them (0x4733C0-0x473BC0)
	unsigned int			Get_Count_Mask(void) const			{ return CountMask; }
	bool						Is_Wrapping(void) const				{ return Wrapping; }
	unsigned short *		Get_Heights(void) const				{ return Heights; }	// NAME 2: the land's Compute_Horizons saves and restores them (0x46E670)
	float						Get_VScale(void) const				{ return VScale; }	// NAME 2: the land's patches read it (0x474790)

protected:
// NAME 2: members after what Load and the PGM reader store
	unsigned short *		Heights;				// +0x00, Buffer aligned to 64 bytes
	void *					Buffer;				// +0x04
	unsigned short			MaxValue;			// +0x08, the largest height read
	float						XSpacing;			// +0x0C, HSCALE / size
	float						YSpacing;			// +0x10
	float						VScale;				// +0x14, VSCALE / MaxValue
	bool						Wrapping;			// +0x18
	unsigned int			Log2;					// +0x1C
	unsigned int			Size;					// +0x20
	unsigned int			Count;				// +0x24, Size * Size
	unsigned int			Mask;					// +0x28, Size - 1
	unsigned int			CountMask;			// +0x2C, Count - 1
	unsigned int			HalfSize;			// +0x30
	char						Name[100];			// +0x34
};

#endif //__HEIGHTFIELD_H
