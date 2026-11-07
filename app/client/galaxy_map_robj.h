// Earth & Beyond client (reconstruction): app\Client\galaxy_map_robj.h (notes at the end)
/*
**	The galaxy map's render objects: a system's marker (a textured quad with the system's name)
**	and the line between two sectors. galaxy_map_robj.cpp's classes.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __GALAXY_MAP_ROBJ_H
#define __GALAXY_MAP_ROBJ_H

#include <vector>
#include "vector2.h"
#include "vector3.h"
#include "..\common\galaxy_map.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\ww3d2\dynamesh.h"
#include "..\..\ww3d2\segline.h"

class INIClass;
class FontSys;
class TextureClass;
class VertexMaterialClass;

// A system's marker on the galaxy map. 0x120 bytes, table 0xB05A44
class GalaxyMapRObjClass : public DynamicMeshClass  // NAME 1: what it is
{
public:
	GalaxyMapRObjClass(const RefCountPtr<GalaxyMapSystemClass> &system);	// 0x7B2B30  // NAME 2 [system]: 0x7AA420 passes its data
	virtual ~GalaxyMapRObjClass();										// 0x7B3100

	virtual void Render(RenderInfoClass &rinfo);						// +0x24 0x7B32E0
	virtual void Add(SceneClass *scene);								// +0x34 0x7B3260
	virtual void Remove(void);											// +0x38 0x7B32B0
	virtual void Set_Position(const Vector3 &v);						// +0x54 0x7B31F0

	void Unknown_7B3440(std::vector<Vector3> &points);	// 0x7B3440  // NAME 1 [points]: what it walks
	void Unknown_7B3690(bool on);						// 0x7B3690  // NAME 1 [on]: neutral
	void Unknown_7B3700(bool on);						// 0x7B3700  // NAME 1 [on]: neutral
	void Unknown_7B3770(const Vector2 &min, const Vector2 &max, bool resize);	// 0x7B3770: the quad from min to max  // NAME 1 [min, max, resize]: what it does with them
	void Unknown_7B3C60();								// 0x7B3C60 (no caller; inlined in 0x7B31F0 and 0x7B3770): the name placed at the position plus NameOffset
	void Unknown_7B3CD0();								// 0x7B3CD0 (no caller): the blinking colour

	struct UVFlagsStruct  // NAME 1: what it holds (initialised first, before Name: a member with its own constructor)
	{
		UVFlagsStruct(int swap) { SwapUV = swap; FlipU = true; FlipV = true; }
		unsigned char SwapUV : 1;						// bit 0: the system's HashID & 1  // NAME 1: what 0x7B3770 does with it
		unsigned char FlipU : 1;						// bit 1: 0x7B3440, the farthest point's X > 0  // NAME 1: what 0x7B3770 does with it
		unsigned char FlipV : 1;						// bit 2: 0x7B3440, its Y < 0  // NAME 1: what 0x7B3770 does with it
	} UVFlags;											// +0xF4  // NAME 1: what it holds
	RefCountPtr<FontSys> Name;							// +0xF8: 'GALAXY_MAP_SYSTEM_NAME' with the system's name  // NAME 1: what it shows
	RefCountPtr<TextureClass> Texture;					// +0xFC: sm_system00.tga  // NAME 1: what it holds
	RefCountPtr<VertexMaterialClass> Material;			// +0x100  // NAME 1: what it holds
	Vector3 Color;										// +0x104: the system's +0x20, the material's emissive  // NAME 1: what it holds
	Vector2 NameOffset;									// +0x110: from the quad and Placement (0x7B3770)  // NAME 1: what it holds
	unsigned int Blink;									// +0x118: non-zero while blinking, the time  // NAME 1: what it does
	int Placement;										// +0x11C: the system's +0x3C, 0..3: where the name goes  // NAME 1: what it decides
};

// The line between two sectors. 0xFC bytes, table 0xB05CAC
class GalaxyMapLineRObjClass : public SegmentedLineClass  // NAME 1: what it is
{
public:
	GalaxyMapLineRObjClass();								// 0x7B3E30
	virtual ~GalaxyMapLineRObjClass();						// 0x7B3ED0

	virtual void Render(RenderInfoClass &rinfo);			// +0x24 0x7B43F0

	bool Unknown_7B3F00(INIClass &ini, const char *name, const Vector3 &color, RefCountPtr<RenderObjClass> from, RefCountPtr<RenderObjClass> to);	// 0x7B3F00  // NAME 1 [ini, name, color, from, to]: what 0x7ACEF0 passes, the FAIL_IFs' 'from' / 'to'

	Vector3 Color;											// +0xE8  // NAME 1: what it holds
	bool Hidden;											// +0xF4: fades out while set (0x7AD090, 0x7AD850)  // NAME 1: what it does
	float Fade;												// +0xF8: 0..1  // NAME 1: what it holds
};

#endif

/*
**	Reconstruction notes
**
**	The header's name follows galaxy_map_robj.cpp's __FILE__ (D24 provisional, D137). Layouts from the
**	constructors 0x7B2B30 / 0x7B3E30 and the tables 0xB05A44 (DynamicMeshClass's Clone and Class_ID
**	inherited) and 0xB05CAC (SegmentedLineClass's).
*/
