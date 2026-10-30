// Earth & Beyond client (reconstruction)
/*
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
**	VIDTEST draws its land with this class (D101), its trees too since 2026-10-28 (Create_Decorations, the patch records).
*/



#if defined(_MSC_VER)
#pragma once
#endif

// NAME 2 [land.h]: after LandClass; no assert or __FILE__ names the file
#ifndef __LAND_H
#define __LAND_H

#include <vector>
#include <string>
#include <deque>
#include "rendobj.h"
#include "ref_ptr.h"
#include "vector3.h"
#include "vector2i.h"
#include "shader.h"
#include "matrix3d.h"
#include "land_lighting.h"

class HeightFieldClass;
class TextureClass;
class DX8VertexBufferClass;
class IndexBufferClass;
class Vector2;
class VertexMaterialClass;
class LandClass;
class INIClass;
class OBBoxClass;
class MaterialPassClass;
class CameraClass;

// NAME 2: one decoration, 0x38 bytes (Create_Decorations 0x479D00's local, the .dcb reader 0x4795D0); a tree's
// prototype, its rotation about z and place on the ground, and its scale
struct LandDecorationStruct
{
	int							Proto;			// +0x00, an index into the prototype names (a byte in the .dcb)
	Matrix3D						Transform;		// +0x04
	float							Scale;			// +0x34
};

// LandLightingStruct: land_lighting.h

// NAME 1: the land's patch (0x30 bytes, new'd per grid cell by Load_Assets 0x4691D0; ctor 0x474530, dtor 0x474560)
class LandPatchClass
{
public:
	LandPatchClass(LandClass *land);
	~LandPatchClass(void);

// NAME 2: after what they do (0x4746C0, called by Load_Assets per patch; 0x474790)
	void							Init(int size, int x, int y, float detail_u, float detail_v);
	void							Build_Vertices(int x, int y, float detail_u, float detail_v);
	void							Draw(void);			// NAME 2: the patch's four quarters of its level's indices (0x4745D0, called by Render 0x474B30)


// NAME 1: members after their offsets (ctor 0x474530)
	DX8VertexBufferClass *	VertexBuffer;		// +0x00, FVF 0x202 (xyz, two uv sets), released by the dtor
	LandClass *					Land;					// +0x04
	int							VertexCount;		// +0x08, Size squared
	int							Size;					// +0x0C, vertices a side
	int							Level;					// +0x10, indexes the land's EdgeCounts (0x4745D0)
	int							Quarters;				// +0x14, bits 0-3: which quarters Draw draws
	float							X;						// NAME 2: +0x18, the patch's corner in world units (Update_Patches 0x46B3C0)
	float							Y;						// +0x1C
	float							Z;						// +0x20, 0
	bool							Visible;				// NAME 2: +0x24, not culled by the camera (Update_Patches)
	float							MidHeight;			// +0x28, (lowest + highest) / 2, in PGM units
	bool							Visited;				// NAME 2: +0x2C, Limit_Levels' mark (0x46AA40)
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
	bool							Visible;				// NAME 2: +0x20, the patch's Visible (Update_Patches 0x46B3C0)
	int							Level;					// NAME 2: +0x24, the patch's Level (Update_Patches)
};

// What the land's vector at +0x1218 holds (added by 0x75F8B0 through 0x4786A0): WW3D2's TexProjectClass. Render 0x474B30
// calls 0x93D6C0, TexProjectClass::Peek_Material_Pass inside texproject.cpp's client range (0x93D670 .. 0x93D6D0
// Set_Perspective_Projection), and reads ProjectorClass's WorldBoundingVolume at +0x8C; the RefCountClass the vector's
// RefCountPtr releases sits at +0xCC, TexProjectClass's CullableClass base (dtor 0x46A7F1). ww3d2/texproject.h
class TexProjectClass;

// NAME 5: "Land.Peek() == (LandClass*)obj" (client assert)
class LandClass : public RenderObjClass
{
	friend class LandPatchClass;
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
// NAME 2: writes an 8-bit grey TGA, 255 where the height field is at or above the sea level (0x46BCC0; nothing calls it)
	bool							Save_Sea_Mask(const char *filename);
// NAME 2: each patch's position, level of detail (lod: an index into the distance table 0xB7188C), edge quarters and visibility (0x46B3C0, called by 0x50F1B0)
	void							Update_Patches(CameraClass &camera, int lod);
// NAME 2: what they set (0x478640 / 0x478670)
	void							Set_Strip_Buffer(int level);
	void							Set_Edge_Buffer(int level);
// NAME 1: the vector at +0x1218 (0x4786A0 push_back, 0x4788B0 clear; called by 0x75F8B0)
	void							Add_Unknown1218(const RefCountPtr<TexProjectClass> &object);
	void							Clear_Unknown1218(void);
// NAME 2: VIDTEST's: it writes +0x136 (0x40AB10); none in the client
	void							Set_Dirty(void)							{ Dirty = true; }
// NAME 2: the decorations (decoration.ini, the zone's land_%04d.dcb; mainview's loader 0x5137F0 reads the .dcb and builds
// them, or makes them from decoration.ini when there is none): the prototype names of an ini section (0x478970, nothing
// calls it), the .dcb's reader 0x4795D0 and writer 0x479A50 (nothing calls it)
	void							Load_Prototype_Names(std::vector<std::string> &names, INIClass &ini, const char *section);
	void							Load_Decorations(std::vector<std::string> &names, std::vector<LandDecorationStruct> &decorations, const char *filename);
// NAME 2: the same as an ini ([Prototypes] NUM_PROTOS, PROTO%d; [DECO%d] DecID, M (the matrix, column by column), Scale):
// the reader 0x478B50 and the writer 0x4791D0 (nothing calls either)
	void							Load_Decorations_Ini(std::vector<std::string> &names, std::vector<LandDecorationStruct> &decorations, const char *filename);
	void							Save_Decorations_Ini(std::vector<std::string> &names, std::vector<LandDecorationStruct> &decorations, const char *filename);
	void							Save_Decorations(std::vector<std::string> &names, std::vector<LandDecorationStruct> &decorations, const char *filename);
// NAME 2: VIDTEST's Land::Create_Decorations (0x415C40 ~ 0x479D00): decoration.ini [set]'s NUM_DECOS trees at random on the
// land; the patch records' tree and shadow buffers from the decorations (0x47A2F0 = VIDTEST 0x416170)
	void							Create_Decorations(int set);
	void							Build_Decorations(std::vector<std::string> &names, std::vector<LandDecorationStruct> &decorations);
// NAME 2: the sea's (mainview's Render_Planet_Mode 0x50E6E0 reads +0xE8 and +0x160: the camera under the sea level)
	bool							Has_Sea(void) const					{ return HasSea; }
	float							Get_Sea_Level(void) const				{ return SeaLevel; }

protected:
// NAME 2: after what they do (0x46C730, 0x46CAE0 (not rebuilt), 0x46D410, 0x46D6B0, 0x46D8B0, 0x46D880)
	void							Prepare_Lighting(void);
	void							Compute_Lighting(void);
	void							Build_Index_Buffers(int size);	// NAME 2: the strip and edge index buffers of each level for patches of size vertices a side (0x478240)
	void							Apply_Colors(void);		// NAME 2: the lit rectangle times the colour map, plus +0x1214 when there is one (0x46D410)
	void							Upload_Lightmap(Vector2i &min, Vector2i &max);
	void							Swap_Lightmaps(void);
// NAME 2: limits the patches' levels outwards from the camera's patch, one level per patch (0x46AA40, called by Update_Patches 0x46B3C0)
	void							Limit_Levels(int x, int y);
// NAME 2: the height map's horizon tables (+0x1E4..+0x1F8, the normals +0x1FC) in a .pld beside the .pgm (0x46EB60, 0x46ECF0), computed from the ini (0x46E670, not rebuilt)
	bool							Load_PLD(const char *filename);
	void							Save_PLD(const char *filename);
	void							Compute_Horizons(INIClass &ini);
// NAME 2: the reciprocal table and the shared horizon table (0x46EF60, called by Compute_Horizons 0x46E670)
	void							Init_Horizon_Tables(void);
// NAME 1: unknown; the steps of Compute_Horizons 0x46E670 (not rebuilt): the shadow casters into the height field (0x46E120),
// the horizons (0x46F120), three work buffers (0x473290) and their use (0x470F80)
	void							Unknown_46E120(std::vector< RefCountPtr<RenderObjClass> > &casters, unsigned char *mask);
	void							Unknown_46F120(void);
	void							Unknown_473290(unsigned char **a, unsigned char **b, unsigned char **c);
	void							Unknown_470F80(unsigned char *a, unsigned char *b, unsigned char *c);
// NAME 1: unknown; the sky's share per vertex in R, G, B from eight horizon codes, the normal's index and the colour (0x472F80, called by 0x470F80)
	void							Unknown_472F80(unsigned char *red, unsigned char *green, unsigned char *blue, unsigned char *horizons, unsigned char normal, const Vector3 &color);
// NAME 1: unknown; one height-field row each into Unknown_473290's buffers, with VScale / Spacing (0x4733C0: 4 bytes a vertex, 0x473780 and 0x473BC0: 3)
	void							Unknown_4733C0(int row, float ratio, unsigned short *out);
	void							Unknown_473780(int row, float ratio, unsigned char *out);
	void							Unknown_473BC0(int row, float ratio, unsigned char *out);
	static void					Lightmap_Thread(void *land);
// NAME 2: a one-line inline setter: its extra inline depth is what keeps the record's operator= out of line inside the inlined
// Update_Lightmap_Full (0x46DA70 / 0x46DDD0) while the direct copies in the same functions inline (F5, 2026-10-27)
	void							Set_Lighting(const LandLightingStruct &lighting)
	{
		Lighting = lighting;
	}

protected:
// NAME 2: members after what the ctor 0x468E90, the dtor 0x46A480 and Load_Assets 0x4691D0 do with them; m_UnknownXX: use not known
	int							LightmapSerial;						// +0x8C, counts the lightmap swaps (0x46D8B0)
	HeightFieldClass *		Field;									// +0x90, HeightFieldClass::Load; never set by the ctor
	IndexBufferClass *		StripBuffers[MAX_LEVELS];			// NAME 2: a level's triangle strip over a patch (Build_Index_Buffers 0x478240)
	int							StripCounts[MAX_LEVELS];			// NAME 2: its index count less 2
	IndexBufferClass *		EdgeBuffers[MAX_LEVELS];			// NAME 2: a level's edge fans, four sides (none at level 0)
	int							EdgeCounts[MAX_LEVELS];			// NAME 2: their index count: LandPatchClass::Draw draws a quarter (one side) per bit
	float *						Reciprocals;							// NAME 2: +0xE4, 1 / n for n = 1 .. 2 Size - 1 (0x46EF60)
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
	int							CameraCellZ;							// NAME 2: +0x114, the camera's cell (patch units) at the last Update_Patches 0x46B3C0
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
	TextureClass *				SeaTextures[MAX_SEAS][2];			// NAME 2: +0x140, the sea swatch's textures, pass x stage (Load_Assets 0x4691D0)
	ShaderClass					SeaShaders[MAX_SEAS];				// +0x150
	VertexMaterialClass *	SeaMaterials[MAX_SEAS];				// NAME 2: +0x158, the swatch's material per pass
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
// NAME 2: the 0x2D-byte lighting record (VIDTEST's LandLightingStruct, its names), one struct member: its generated operator= 0x4744C0 is what the
// inlined Update_Lightmap_Full calls in 0x46DA70 / 0x46DDD0; the defaults are its constructor's, inlined into the land's (0x468E90)
	LandLightingStruct		Lighting;								// +0x198, 0x2D bytes; its operator= 0x4744C0
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
// NAME 2: what Prepare_Lighting 0x46C730 makes of the lighting record
	Vector3						Normals[256];						// +0x1FC, a normal table: NormalColors[i] lights Normals[i]
	unsigned short				HorizonLow;							// +0xDFC, (SunAngle - ShadowBias) in 0..65535 = 0..pi/2
	unsigned short				HorizonHigh;						// +0xDFE, (SunAngle + ShadowBias)
	unsigned int				SkyColorDX8;						// +0xE00, DX8Wrapper::Convert_Color(SkyColor, 0)
	bool							SunFromWest;						// +0xE04, SunDirection.X < 0
	float							ShadowStep;							// +0xE08, 1 / (2 ceil(ShadowBias in horizon units))
	unsigned int				NormalColors[256];				// +0xE0C
	unsigned int *				m_Unknown120C;							// Field Count dwords
	unsigned char *			Colors;									// +0x1210, [LAND] COLOR_MAP, RGB per vertex
	unsigned char *			m_Unknown1214;							// one byte per vertex from the colour map
	std::vector< RefCountPtr<TexProjectClass> >	m_Unknown1218;
};

#endif //__LAND_H
