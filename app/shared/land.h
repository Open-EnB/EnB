/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\ (file name unknown: no assert names it; land.h is
**	a proposal)
**
**	The terrain: a render object built from a land ini (VIDTEST: land.ini; the client: land%04d.ini,
**	mainview.cpp) over a HeightFieldClass, with a colour map lit into a lightmap texture by the sun,
**	a detail map, patches of Patch_Grid_Width quads drawn at a level of detail chosen per patch,
**	and decorations (decoration.ini: trees) placed at random on it.
**
**	The client's vtable (0xAE8994) overrides the destructor, Clone, Class_ID, Render and Cast_Ray /
**	Cast_AABox / Cast_OBBox only: Get_Num_Polys and Get_Cost stay RenderObjClass's, so the land costs
**	nothing in the predictive LOD budget.
**
**	Names: the client's "Land.Peek() == (LandClass*)obj" names the class LandClass; its profile label
**	"Land::Update_Lightmap_ManualThreaded" names that method. The rest are descriptive (D10).
**
**	The client: ctor 0x468E90, Load_Assets 0x4691D0, dtor 0x46A480, Update_Patches 0x46B3C0, the
**	lightmap 0x46D900 / 0x46DA70, the tessellation 0x46DDD0-0x473BC0, Render 0x474B30, the
**	decorations 0x478B50 / 0x479D00 / 0x47A2F0 (VIDTEST's 2002 copy, docs/vidtest.md 4.2, 4.3:
**	0x405410, 0x405750, 0x4069D0, 0x4078F0, 0x40A9F0, 0x40AB10, 0x410B90, 0x414B50, 0x415C40,
**	0x416170). VIDTEST's object is 0x1228 bytes.
**
**	NOT THE CLIENT'S LAYOUT OR ALGORITHM: functional (D42), written for VIDTEST's land scene
**	(milestones.md M2.4). The lightmap follows VIDTEST's (colour x (light x max(0, n.sun) + sky
**	share x sky), shadowed by a horizon test along the sun's x direction); the patches are a simpler scheme
**	(fixed strides 1..16 per patch, skirts against cracks) than the client's tessellation.
*/

#if defined(_MSC_VER)
#pragma once
#endif

// NAME 2 [land.h]: after LandClass; no assert or __FILE__ names the file
#ifndef __LAND_H
#define __LAND_H

#include "rendobj.h"
#include "vector.h"
#include "vector3.h"
#include "sphere.h"
#include "aabox.h"
#include "shader.h"

class HeightFieldClass;
class TextureClass;
class VertexMaterialClass;
class DX8VertexBufferClass;
class DX8IndexBufferClass;
class CameraClass;

/*
** What Update_Lighting hands the land (VIDTEST 0x4250B0's record at esp+0x74, 0x30 bytes)
*/
// NAME 2: the record Update_Lighting hands the land (VIDTEST esp+0x74)
struct LandLightingStruct
{
	Vector3	LightColor;		// clamped LightColor x LightIntensity
	Vector3	SunDirection;	// towards the sun
	float		SunAngle;		// pi/2 - elevation
	float		ShadowBias;		// 0.005
	Vector3	SkyColor;		// clamped SkyColor x SkyIntensity
	bool		Valid;
};

// NAME 5: "Land.Peek() == (LandClass*)obj" (client assert)
class LandClass : public RenderObjClass
{
public:
	enum {
		MAX_LEVELS = 5,		// strides 1, 2, 4, 8, 16
	};

	LandClass(void);
	virtual ~LandClass(void);

// NAME 2: Create, Load_Assets, Create_Decorations, Update_Patches, Set_Dirty, Update_Lightmap_Full describe VIDTEST 0x4068F0, 0x405750, 0x415C40, 0x4078F0, +0x136, 0x40A9F0
	static LandClass *		Create(const char *ini_name, bool flag);
	bool							Load_Assets(const char *ini_name);
	bool							Create_Decorations(int set);

	void							Update_Patches(CameraClass &camera, int lod);
	void							Set_Dirty(void)								{ Dirty = true; }
	void							Update_Lightmap_Full(const LandLightingStruct &lighting);
// NAME 5: profile label "Land::Update_Lightmap_ManualThreaded" (client 0x46DAA2)
	void							Update_Lightmap_ManualThreaded(const LandLightingStruct &lighting);

	const HeightFieldClass *	Peek_Height_Field(void) const			{ return Field; }
	int							Get_Rendered_Polys(void) const			{ return CurrentPolys; }	// not Get_Num_Polys: the client's land adds nothing to the LOD budget

	// RenderObjClass
	virtual RenderObjClass *	Clone(void) const							{ return NULL; }
	virtual void				Render(RenderInfoClass &rinfo);
	virtual void				Get_Obj_Space_Bounding_Sphere(SphereClass &sphere) const;
	virtual void				Get_Obj_Space_Bounding_Box(AABoxClass &box) const;

protected:
	void							Build_Buffers(void);
	void							Build_Horizons(void);
	bool							Load_PLD(void);
	void							Compute_Lightmap(const LandLightingStruct &lighting, int first_row, int rows);
	void							Upload_Lightmap(void);
	void							Render_Decorations(RenderInfoClass &rinfo);

// NAME 2: members describe what they hold (VIDTEST offsets in land.h's header)
	HeightFieldClass *		Field;
	bool							HasSea;
	bool							Flag;
	bool							Dirty;
	int							PatchGridWidth;		// quads per patch side (16)
	int							LightmapChunk;
	int							PatchCount;				// patches per side
	float							DetailScaleX;
	float							DetailScaleY;

	unsigned char *			Colors;					// RGB per vertex (the colour map)
	unsigned int *				Lightmap;				// X8R8G8B8 per vertex
	unsigned short *			HorizonEast;			// horizon elevation towards +x (0..65535 = 0..pi/2)
	unsigned short *			HorizonWest;			// towards -x
	Vector3 *					Normals;					// per vertex
	unsigned char *			SkyWeights;				// per vertex R, G, B: the sky's share (.pld; NULL: from the horizons)
	int							LightmapRow;			// the time-sliced update's next row
	TextureClass *				LightmapTexture;
	TextureClass *				DetailTexture;
	VertexMaterialClass *	Material;
	ShaderClass					Shader;

	DynamicVectorClass<DX8VertexBufferClass *>	RowBuffers;	// one per patch row
	DX8IndexBufferClass *	IndexBuffer;
	int							LevelStart[MAX_LEVELS];
	int							LevelPolys[MAX_LEVELS];
	unsigned char *			PatchLevel;				// per patch: its level, 0xFF culled
	int							CurrentPolys;
	SphereClass					BoundSphere;
	AABoxClass					BoundBox;

	struct DecoBatchStruct
	{
		TextureClass *				Texture;
		ShaderClass					Shader;
		VertexMaterialClass *	Material;
		DX8VertexBufferClass *	VertexBuffer;
		DX8IndexBufferClass *	IndexBuffer;
		int							Vertices;
		int							Polys;
		bool							Shadow;					// the trees' shadows, drawn after them (VIDTEST 0x410B90) // NAME 2: ours
		int							Patch;					// drawn while this patch's level is below 3 (VIDTEST 0x410B90: +0x24 < 3) // NAME 2: ours
	};
	DynamicVectorClass<DecoBatchStruct *>	DecoBatches;	// the decorations, batched (VIDTEST 0x416170)
};

#endif //__LAND_H
