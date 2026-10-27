/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\ (file name unknown: no assert names it; land.h is
**	a proposal)
**
**	The terrain: a render object built from a land ini (the client: land%04d.ini, mainview.cpp;
**	VIDTEST: land.ini) over a HeightFieldClass (+0x90), with a colour map lit into lightmap textures
**	by the sun (a worker thread, +0x194), a detail map, an optional sea, a grid of patches drawn at a
**	level of detail chosen per patch, and decorations.
**
**	THE CLIENT'S LAYOUT (0x1228 bytes, as VIDTEST's 2002 copy), from the constructor 0x468E90, the
**	destructor 0x46A480 and Load_Assets 0x4691D0. The client's vtable (0xAE8994) overrides the
**	destructor, Clone, Class_ID, Render and Cast_Ray / Cast_AABox / Cast_OBBox only: Get_Num_Polys
**	and Get_Cost stay RenderObjClass's, so the land costs nothing in the predictive LOD budget.
**
**	Names: the client's "Land.Peek() == (LandClass*)obj" names the class LandClass; its profile label
**	"Land::Update_Lightmap_ManualThreaded" names that method. The rest are descriptive (D10), with
**	their evidence; members whose use isn't known yet are m_UnknownXX.
**
**	The client: Create 0x46A3A0, ctor 0x468E90, Load_Assets 0x4691D0, dtor 0x46A480 (scalar deleting
**	0x4691A0), the small virtuals 0x46C600-0x46C6E0, Update_Patches 0x46B3C0, the lightmap 0x46D900 /
**	0x46DA70, the tessellation 0x46DDD0-0x473BC0, the patch 0x474530 / 0x474560, the LOD record
**	0x474990 / 0x4749D0, Render 0x474B30, the decorations 0x478B50-0x47A2F0. VIDTEST's 2002 copy:
**	docs/vidtest.md 4.2, 4.3.
**
**	VIDTEST draws with programs\vidtest\vidland.cpp (a functional stand-in) until this class can draw.
*/

#if defined(_MSC_VER)
#pragma once
#endif

// NAME 2 [land.h]: after LandClass; no assert or __FILE__ names the file
#ifndef __LAND_H
#define __LAND_H

#include <vector>
#include "rendobj.h"
#include "ref_ptr.h"
#include "vector3.h"
#include "vector2i.h"
#include "shader.h"

class HeightFieldClass;
class TextureClass;
class IndexBufferClass;
class Vector2;
class VertexMaterialClass;
class LandClass;

// NAME 2: VIDTEST's name for the 0x30-byte lighting record its Update_Lighting 0x4250B0 hands the land (esp+0x74)
struct LandLightingStruct
{
	Vector3		LightColor;		// clamped LightColor x LightIntensity
	Vector3		SunDirection;	// towards the sun
	float		SunAngle;		// pi/2 - elevation
	float		ShadowBias;
	Vector3		SkyColor;		// clamped SkyColor x SkyIntensity
	bool		Valid;
};

// NAME 1: the land's patch (0x30 bytes, new'd per grid cell by Load_Assets 0x4691D0; ctor 0x474530, dtor 0x474560)
class LandPatchClass
{
public:
	LandPatchClass(LandClass *land);
	~LandPatchClass(void);

// NAME 1: members after their offsets (ctor 0x474530)
	RefCountClass *			m_Unknown00;		// released by the dtor
	LandClass *					Land;					// +0x04
	int							m_Unknown08;
	int							m_Unknown0C;
	int							m_Unknown10;
	int							m_Unknown14;
	int							m_Unknown18;
	int							m_Unknown1C;
	int							m_Unknown20;
	int							m_Unknown24;
	int							m_Unknown28;
	int							m_Unknown2C;
};

// NAME 1: one record per patch, 0x28 bytes (new[]'d by Load_Assets 0x4691D0; ctor 0x474990, dtor 0x4749D0)
class LandPatchRecordClass
{
public:
	LandPatchRecordClass(void);
	~LandPatchRecordClass(void);

// NAME 1: members after their offsets (ctor 0x474990)
	RefCountPtr<RefCountClass>	m_Unknown00;
	RefCountPtr<RefCountClass>	m_Unknown04;
	RefCountPtr<RefCountClass>	m_Unknown08;
	RefCountPtr<RefCountClass>	m_Unknown0C;
	int							m_Unknown10;
	int							m_Unknown14;
	int							m_Unknown18;
	int							m_Unknown1C;
	bool							m_Unknown20;
	int							m_Unknown24;
};

// NAME 1: unknown; what the land's vector at +0x1218 holds: RefCountClass at +0xCC (dtor 0x46A7F1), added by 0x75F8B0 through 0x4786A0
class LandUnknownBaseClass
{
public:
	virtual ~LandUnknownBaseClass(void);
	char							m_Unknown04[0xC8];
};
class LandUnknownClass : public LandUnknownBaseClass, public RefCountClass	// NAME 1: as above
{
};

// NAME 5: "Land.Peek() == (LandClass*)obj" (client assert)
class LandClass : public RenderObjClass
{
public:
	enum {
		MAX_LEVELS = 5,		// NAME 2: the per-level arrays at +0x94..+0xE0 (5 each)
		MAX_SEAS = 2,			// NAME 2: the sea arrays at +0x140..+0x170 (2 each; Load_Assets clamps to 2)
	};

	LandClass(void);
	virtual ~LandClass(void);

// NAME 2: Create, Load_Assets describe VIDTEST 0x4068F0 / 0x405750 and the client's 0x46A3A0 / 0x4691D0 ("LoadAssets" in symbols.json)
	static LandClass *		Create(const char *ini_name, bool flag);
	bool							Load_Assets(const char *ini_name, bool flag);

	// RenderObjClass
	virtual RenderObjClass *	Clone(void) const;
	virtual int						Class_ID(void) const;
	virtual void					Render(RenderInfoClass &rinfo);
	virtual bool					Cast_Ray(RayCollisionTestClass &raytest);
	virtual bool					Cast_AABox(AABoxCollisionTestClass &boxtest);
	virtual bool					Cast_OBBox(OBBoxCollisionTestClass &boxtest);

// NAME 2: Update_Lightmap_Full and Update_Lightmap_Threaded describe 0x46D900 / 0x46DDD0 (VIDTEST 0x40A9F0)
	void							Update_Lightmap_Full(const LandLightingStruct &lighting);
// NAME 5: profile label "Land::Update_Lightmap_ManualThreaded" (client 0x46DAA2)
	void							Update_Lightmap_ManualThreaded(const LandLightingStruct &lighting);
	void							Update_Lightmap_Threaded(const LandLightingStruct &lighting);

// NAME 2: the colour map at a position (0x46BB20 / 0x46BBF0: Colors, B G R per vertex)
	Vector3						Get_Color(const Vector2 &pos);
	void							Set_Color(const Vector2 &pos, const Vector3 &color);
// NAME 1: what they set (0x478640 / 0x478670); the arrays' use isn't known yet
	void							Set_Index_Buffer_94(int level);
	void							Set_Index_Buffer_BC(int level);
// NAME 1: the vector at +0x1218 (0x4786A0 push_back, 0x4788B0 clear; called by 0x75F8B0)
	void							Add_Unknown1218(const RefCountPtr<LandUnknownClass> &object);
	void							Clear_Unknown1218(void);

protected:
// NAME 2: after what they do (0x46C730, 0x46CAE0, 0x46D410, 0x46D6B0: not rebuilt; 0x46D8B0, 0x46D880)
	void							Prepare_Lighting(void);
	void							Compute_Lighting(void);
	void							Compute_Shadows(void);
	void							Upload_Lightmap(Vector2i &min, Vector2i &max);
	void							Swap_Lightmaps(void);
	static void					Lightmap_Thread(void *land);
	void							Set_Lighting(const LandLightingStruct &lighting)
	{
		LightColor = lighting.LightColor;
		SunDirection = lighting.SunDirection;
		SunAngle = lighting.SunAngle;
		ShadowBias = lighting.ShadowBias;
		SkyColor = lighting.SkyColor;
		LightingValid = lighting.Valid;
	}

protected:
// NAME 2: members after what the ctor 0x468E90, the dtor 0x46A480 and Load_Assets 0x4691D0 do with them; m_UnknownXX: use not known
	int							LightmapSerial;						// +0x8C, counts the lightmap swaps (0x46D8B0)
	HeightFieldClass *		Field;									// +0x90, HeightFieldClass::Load; never set by the ctor
	IndexBufferClass *		m_Unknown94[MAX_LEVELS];			// released by the dtor; DX8Wrapper::Set_Index_Buffer (0x478640)
	int							m_UnknownA8[MAX_LEVELS];
	IndexBufferClass *		m_UnknownBC[MAX_LEVELS];			// released by the dtor, all but [0]; DX8Wrapper::Set_Index_Buffer (0x478670)
	int							m_UnknownD0[MAX_LEVELS];
	unsigned char *			m_UnknownE4;							// Field Size * 8 - 4 bytes
	bool							HasSea;									// +0xE8, [GENERAL] HASSEA
	int							PatchLog2;								// +0xEC, Field Log2 - GridLog2
	int							PatchSize;								// +0xF0, cells per patch side
	int							PatchVerts;								// +0xF4, PatchSize + 1
	int							GridLog2;								// +0xF8
	int							GridWidth;								// +0xFC, [LAND] Patch_Grid_Width (16)
	int							HalfGridWidth;							// +0x100
	int							PatchCount;								// +0x104, GridWidth squared
	int							GridMask;								// +0x108, GridWidth - 1
	int							CameraCellX;							// +0x10C, -999999: none yet
	int							CameraCellY;							// +0x110
	int							m_Unknown114;
	TextureClass *				LightmapTextures[2];					// +0x118
	TextureClass *				m_Unknown120;							// a third texture of the field's size
	TextureClass *				DetailTexture;							// +0x124, [LAND] DETAIL_MAP
	VertexMaterialClass *	Material;								// +0x128
	ShaderClass					Shader;									// +0x12C
	LandPatchClass **			Patches;									// +0x130, PatchCount
	bool							Is16Bit;									// +0x134, the device's depth is 16 bits or less
	bool							Flag;										// +0x135, Create's flag
	bool							Dirty;									// +0x136
	unsigned char				FrontLightmap;						// +0x137, LightmapTextures index shown (xor 1 per swap)
	unsigned char				BackLightmap;						// +0x138, the one being computed
	int							LightmapChunk;							// +0x13C, [LAND] LIGHTMAP_CHUNK (8)
	RefCountClass *			m_Unknown140[MAX_SEAS][2];			// released by the dtor when HasSea
	ShaderClass					SeaShaders[MAX_SEAS];				// +0x150
	RefCountClass *			m_Unknown158[MAX_SEAS];				// released by the dtor when HasSea
	float							SeaLevel;								// +0x160, [SEA] SEA_LEVEL
	float							SeaDetailScales[MAX_SEAS][2];		// +0x164, [SEA] DETAIL_SCALE..DETAIL_SCALE4
	int							SeaCount;								// +0x174
	RefCountPtr<RefCountClass>	m_Unknown178;
	ShaderClass					m_Unknown17C;
	RefCountPtr<RefCountClass>	m_Unknown180;
	ShaderClass					m_Unknown184;
	RefCountPtr<RefCountClass>	m_Unknown188;
	LandPatchRecordClass *	PatchRecords;							// +0x18C, PatchCount
	bool							m_Unknown190;
	bool							m_Unknown191;
	bool							m_Unknown192;
	void *						Thread;									// +0x194, a HANDLE the dtor waits for
// NAME 2: +0x198..+0x1C4 is the 0x30-byte lighting record Update_Lightmap 0x46D900 / 0x46DA70 copy in (VIDTEST's LandLightingStruct, its names)
	Vector3						LightColor;								// +0x198
	Vector3						SunDirection;							// +0x1A4
	float							SunAngle;								// +0x1B0
	float							ShadowBias;								// +0x1B4
	Vector3						SkyColor;								// +0x1B8
	bool							LightingValid;							// +0x1C4
// NAME 2: the lightmap rectangle being updated and the time-sliced update's cursor (0x46D900 / 0x46DA70)
	Vector2i						UpdateMin;								// +0x1C8, SurfaceClass::Copy's min (0x46D6B0)
	Vector2i						UpdateMax;								// +0x1D0
	int							CursorX;									// +0x1D8, steps of LightmapChunk
	int							CursorY;									// +0x1DC
	bool							ThreadDone;							// +0x1E0, Update_Lightmap_Threaded's worker has finished
	unsigned char *			m_Unknown1E4;							// Field Count bytes
	unsigned short *			m_Unknown1E8;							// Field Count shorts
	unsigned short *			m_Unknown1EC;							// Field Count shorts
	unsigned char *			m_Unknown1F0;							// Field Count bytes
	unsigned char *			m_Unknown1F4;							// Field Count bytes
	unsigned char *			m_Unknown1F8;							// Field Count bytes
	char							m_Unknown1FC[0xC00];
	short							m_UnknownDFC;
	short							m_UnknownDFE;
	int							m_UnknownE00;
	bool							m_UnknownE04;
	int							m_UnknownE08;
	char							m_UnknownE0C[0x400];
	unsigned int *				m_Unknown120C;							// Field Count dwords
	unsigned char *			Colors;									// +0x1210, [LAND] COLOR_MAP, RGB per vertex
	unsigned char *			m_Unknown1214;							// one byte per vertex from the colour map
	std::vector< RefCountPtr<LandUnknownClass> >	m_Unknown1218;
};

#endif //__LAND_H
