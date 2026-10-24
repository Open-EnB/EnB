/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\ (file name unknown: no assert names it; neutral
**	name, D10)
**
**	The terrain's height field, from a land ini's [HEIGHTMAP] section: HEIGHT_MAP (an ASCII PGM),
**	HSCALE (the side length in world units), VSCALE (the height of the PGM's maxval), WRAPPING,
**	FLIP_PGM. The field is centred on the origin: vertex (i, j) is at x = (i - Size / 2) * Spacing,
**	y = (j - Size / 2) * Spacing.
**
**	The client: ctor 0x4634A0, Load 0x4634F0, the PGM reader 0x4637C0, the ray and box collision
**	code 0x463F00-0x465D30 (VIDTEST's 2002 copy: 0x4023D0, 0x402420, 0x4026B0, Sample 0x402AE0).
**	The client doesn't name the class (D24): HeightFieldClass is descriptive. VIDTEST's object is
**	0x98 bytes: +0 heights, +8 maxval (16 bits), +0x0C/+0x10 spacing, +0x14 height scale,
**	+0x18 wrapping, +0x1C log2(size), +0x20/+0x24 size, +0x28/+0x2C size - 1, +0x30 size / 2,
**	+0x34 the PGM name (100).
**
**	NOT THE CLIENT'S LAYOUT: functional (D42); the heights are floats here.
*/

#if defined(_MSC_VER)
#pragma once
#endif

// NAME 2 [heightfield.h]: describes the class; no assert or __FILE__ names the file
#ifndef __HEIGHTFIELD_H
#define __HEIGHTFIELD_H

#include "vector3.h"

class INIClass;

// NAME 2: after land.ini's [HEIGHTMAP] / HEIGHT_MAP; the client names no class
class HeightFieldClass
{
public:
	HeightFieldClass(void);
	~HeightFieldClass(void);

// NAME 2: methods and members describe what they do / hold (VIDTEST 0x402420, 0x4026B0, 0x402AE0)
	static HeightFieldClass *	Load(const char *ini_name);
	bool						Load_PGM(const char *filename, bool flip);

	int						Get_Size(void) const					{ return Width; }
	float						Get_Spacing(void) const				{ return Spacing; }
	float						Get_Extent(void) const				{ return Spacing * Width; }
	float						Get_Vertex_Height(int i, int j) const;
	Vector3					Get_Vertex(int i, int j) const;
	Vector3					Get_Normal(int i, int j) const;
	float						Get_Height(float x, float y) const;	// bilinear, world coordinates
	void						Sample(Vector3 *out, float u, float v) const;	// u, v in [0, 1]

	const char *			Get_Name(void) const					{ return Name; }

protected:
	float *					Heights;
	unsigned short			MaxValue;
	float						Spacing;
	float						HeightScale;
	bool						Wrapping;
	int						Width;
	int						Height;
	char						Name[100];
};

#endif //__HEIGHTFIELD_H
