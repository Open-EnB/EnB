// Earth & Beyond client (reconstruction): app\shared\ terrain (file name unknown). Notes at the end.
#define ENABLE_WWPROFILE
#include "land.h"
#include "heightfield.h"
#include "texture.h"
#include "vertmaterial.h"
#include "dx8wrapper.h"
#include "dx8indexbuffer.h"
#include "vector2.h"
#include "surfaceclass.h"
#include "dx8vertexbuffer.h"
#include <windows.h>
#include "wwprofile.h"
#include "wwmath.h"
#include <math.h>

void Unknown_466F10(void);	// NAME 1: unknown; deletes a global object (0xBDC0D8) and clears it; called by the land's dtor only

LandClass::LandClass(void) :
	LightmapSerial(0),
	m_UnknownE4(NULL),
	HasSea(false),
	CameraCellX(-999999),
	CameraCellY(-999999),
	m_Unknown114(0),
	m_Unknown120(NULL),
	DetailTexture(NULL),
	Material(NULL),
	Patches(NULL),
	Is16Bit(false),
	Flag(false),
	Dirty(true),
	FrontLightmap(0),
	BackLightmap(1),
	SeaCount(0),
	m_Unknown17C(ShaderClass::_PresetOpaqueShader),
	m_Unknown184(ShaderClass::_PresetOpaqueShader),
	PatchRecords(NULL),
	m_Unknown190(false),
	m_Unknown191(false),
	m_Unknown192(true),
	Thread(NULL),
	LightColor(0.5f, 0.5f, 0.5f),
	SunDirection(0.0f, 0.0f, 1.0f),
	SunAngle(0.0f),
	ShadowBias(0.05f),
	SkyColor(0.1f, 0.1f, 0.5f),
	LightingValid(true),
	UpdateMin(0, 0),
	UpdateMax(0, 0),
	CursorX(0),
	CursorY(0),
	ThreadDone(false),
	m_Unknown1E4(NULL),
	m_Unknown1E8(NULL),
	m_Unknown1EC(NULL),
	m_Unknown1F0(NULL),
	m_Unknown1F4(NULL),
	m_Unknown1F8(NULL),
	HorizonLow(0),
	HorizonHigh(0),
	SkyColorDX8(0),
	SunFromWest(true),
	ShadowStep(0),
	m_Unknown120C(NULL),
	Colors(NULL),
	m_Unknown1214(NULL)
{
	LightmapTextures[0] = LightmapTextures[1] = NULL;
	int i;
	for (i = 0; i < MAX_LEVELS; i++) {
		m_Unknown94[i] = NULL;
		m_UnknownBC[i] = NULL;
	}
	for (i = 0; i < MAX_SEAS; i++) {
		m_Unknown158[i] = NULL;
		m_Unknown140[i][0] = NULL;
		m_Unknown140[i][1] = NULL;
		SeaDetailScales[i][0] = 1.0f;
		SeaDetailScales[i][1] = 1.0f;
	}
}

LandClass::~LandClass(void)
{
	if (Thread != NULL) {
		WaitForSingleObject(Thread, INFINITE);
	}
	REF_PTR_RELEASE(LightmapTextures[0]);
	REF_PTR_RELEASE(LightmapTextures[1]);
	REF_PTR_RELEASE(m_Unknown120);
	REF_PTR_RELEASE(DetailTexture);
	REF_PTR_RELEASE(Material);
	m_Unknown178 = 0;
	m_Unknown180 = 0;
	m_Unknown188 = 0;
	int i;
	if (HasSea) {
		for (i = 0; i < MAX_SEAS; i++) {
			REF_PTR_RELEASE(m_Unknown158[i]);
			REF_PTR_RELEASE(m_Unknown140[i][0]);
			REF_PTR_RELEASE(m_Unknown140[i][1]);
		}
	}
	if (Patches != NULL) {
		i = PatchCount;
		while (i--) {
			if (Patches[i] != NULL) {
				delete Patches[i];
				Patches[i] = NULL;
			}
		}
	}
	if (PatchRecords != NULL) {
		delete [] PatchRecords;
		PatchRecords = NULL;
	}
	for (i = MAX_LEVELS; i--; ) {
		REF_PTR_RELEASE(m_Unknown94[i]);
		if (i != 0) {
			REF_PTR_RELEASE(m_UnknownBC[i]);
		}
	}
	if (m_UnknownE4 != NULL) delete [] m_UnknownE4;
	if (m_Unknown1E4 != NULL) delete [] m_Unknown1E4;
	if (m_Unknown1E8 != NULL) delete [] m_Unknown1E8;
	if (m_Unknown1EC != NULL) delete [] m_Unknown1EC;
	if (m_Unknown1F0 != NULL) delete [] m_Unknown1F0;
	if (m_Unknown1F4 != NULL) delete [] m_Unknown1F4;
	if (m_Unknown1F8 != NULL) delete [] m_Unknown1F8;
	if (m_Unknown120C != NULL) delete [] m_Unknown120C;
	if (Colors != NULL) delete [] Colors;
	if (m_Unknown1214 != NULL) delete [] m_Unknown1214;
	if (Patches != NULL) delete [] Patches;
	if (Field != NULL) delete Field;
	Unknown_466F10();
}

LandClass *LandClass::Create(const char *ini_name, bool flag)
{
	LandClass *land = new LandClass;
	if (!land->Load_Assets(ini_name, flag)) {
		REF_PTR_RELEASE(land);
	}
	land->Flag = flag;
	return land;
}

RenderObjClass *LandClass::Clone(void) const
{
	return NULL;
}

int LandClass::Class_ID(void) const
{
	return CLASSID_LAND;
}

bool LandClass::Cast_Ray(RayCollisionTestClass &raytest)
{
	return Field->Cast_Ray(raytest);
}

bool LandClass::Cast_AABox(AABoxCollisionTestClass &boxtest)
{
	return Field->Cast_AABox(boxtest);
}

bool LandClass::Cast_OBBox(OBBoxCollisionTestClass &boxtest)
{
	return Field->Cast_OBBox(boxtest);
}

void LandClass::Lightmap_Thread(void *land)
{
	((LandClass *)land)->Compute_Lighting();
	((LandClass *)land)->Apply_Colors();
}

void LandClass::Swap_Lightmaps(void)
{
	LightmapSerial++;
	LightmapTextures[BackLightmap]->Clean();
	BackLightmap ^= 1;
	FrontLightmap ^= 1;
}

void LandClass::Update_Lightmap_Full(const LandLightingStruct &lighting)
{
	if (Thread != NULL) {
		WaitForSingleObject(Thread, INFINITE);
		Thread = NULL;
	}
	Set_Lighting(lighting);
	Prepare_Lighting();
	Vector2i &min = UpdateMin;
	Vector2i &max = UpdateMax;
	min.I = 0;
	min.J = 0;
	int size = Field->Get_Size();
	max.I = size;
	max.J = size;
	Compute_Lighting();
	Apply_Colors();
	Upload_Lightmap(min, max);
	Swap_Lightmaps();
}

// NOT EXACT: the client calls the lighting record's operator= (0x4744C0) in the inlined Update_Lightmap_Full: the record is one struct member there (land.h)
void LandClass::Update_Lightmap_ManualThreaded(const LandLightingStruct &lighting)
{
	WWPROFILE("Land::Update_Lightmap_ManualThreaded");
	int size = Field->Get_Size();
	if (Thread != NULL) {
		WaitForSingleObject(Thread, INFINITE);
		Thread = NULL;
	}
	if (LightmapTextures[FrontLightmap]->Is_Dirty()) {
		Update_Lightmap_Full(lighting);
	} else if (CursorY == size) {
		Swap_Lightmaps();
		CursorY = 0;
		CursorX = 0;
		Set_Lighting(lighting);
		Prepare_Lighting();
	} else {
		UpdateMin.I = CursorX;
		UpdateMin.J = CursorY;
		UpdateMax.J = CursorY + LightmapChunk;
		UpdateMax.I = LightmapChunk + CursorX;
		Compute_Lighting();
		Apply_Colors();
		Upload_Lightmap(UpdateMin, UpdateMax);
		CursorX += LightmapChunk;
		if (CursorX == size) {
			CursorX = 0;
			CursorY += LightmapChunk;
		}
	}
}

// NOT EXACT: as Update_Lightmap_ManualThreaded (the record's operator= 0x4744C0 in the inlined Update_Lightmap_Full)
void LandClass::Update_Lightmap_Threaded(const LandLightingStruct &lighting)
{
	if (LightmapTextures[FrontLightmap]->Is_Dirty()) {
		Update_Lightmap_Full(lighting);
		return;
	}
	if (!ThreadDone) {
		if (Thread == NULL) {
			Set_Lighting(lighting);
			Prepare_Lighting();
			UpdateMin.I = 0;
			UpdateMin.J = 0;
			int size = Field->Get_Size();
			UpdateMax.I = size;
			UpdateMax.J = size;
			Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Lightmap_Thread, this, 0, NULL);
		}
		if (WaitForSingleObject(Thread, 0) == WAIT_OBJECT_0) {
			Thread = NULL;
			ThreadDone = true;
		}
	} else {
		int y = CursorY;
		int size = Field->Get_Size();
		if (y == size) {
			Swap_Lightmaps();
			CursorY = 0;
			CursorX = 0;
			ThreadDone = false;
			return;
		}
		Vector2i min;
		Vector2i max;
		min.I = CursorX;
		min.J = y;
		max.I = min.I + LightmapChunk;
		max.J = LightmapChunk + y;
		Upload_Lightmap(min, max);
		CursorX += LightmapChunk;
		if (CursorX == size) {
			CursorX = 0;
			CursorY += LightmapChunk;
		}
	}
}

Vector3 LandClass::Get_Color(const Vector2 &pos)
{
	float i, j;
	Field->World_To_Grid(pos, &i, &j);
	unsigned char *color = &Colors[((int)j * Field->Get_Size() + (int)i) * 3];
	return Vector3(color[2] * (1.0f / 255.0f), color[1] * (1.0f / 255.0f), color[0] * (1.0f / 255.0f));
}

void LandClass::Set_Color(const Vector2 &pos, const Vector3 &color)
{
	float i, j;
	Field->World_To_Grid(pos, &i, &j);
	int index = ((int)j * Field->Get_Size() + (int)i) * 3;
	Colors[index++] = (unsigned char)(color.Z * 255.0f);
	Colors[index++] = (unsigned char)(color.Y * 255.0f);
	Colors[index] = (unsigned char)(color.X * 255.0f);
}

LandPatchClass::LandPatchClass(LandClass *land) :
	VertexBuffer(NULL),
	Land(land),
	Level(0),
	Quarters(0),
	m_Unknown18(0),
	m_Unknown1C(0),
	m_Unknown20(0)
{
}

LandPatchRecordClass::LandPatchRecordClass(void) :
	m_Unknown10(0),
	m_Unknown14(0),
	m_Unknown18(0),
	m_Unknown1C(0),
	m_Unknown20(false),
	m_Unknown24(0)
{
}

void LandClass::Set_Index_Buffer_94(int level)
{
	DX8Wrapper::Set_Index_Buffer(m_Unknown94[level], 0);
}

void LandClass::Set_Index_Buffer_BC(int level)
{
	DX8Wrapper::Set_Index_Buffer(m_UnknownBC[level], 0);
}

void LandClass::Add_Unknown1218(const RefCountPtr<LandUnknownClass> &object)
{
	m_Unknown1218.push_back(object);
}

void LandClass::Clear_Unknown1218(void)
{
	m_Unknown1218.clear();
}

// NOT EXACT: frame (0x14, the client 0x20: it keeps this, the size and the buffer in frame slots); 2 forms tried
void LandClass::Upload_Lightmap(Vector2i &min, Vector2i &max)
{
	SurfaceClass *surface = m_Unknown120->Get_Surface_Level(0);
	if (Is16Bit) {
		unsigned char *bytes = (unsigned char *)m_Unknown120C;
		unsigned short *words = (unsigned short *)m_Unknown120C;
		int size = Field->Get_Size();
		for (int j = min.J; j < max.J; j++) {
			for (int i = min.I; i < max.I; i++) {
				int n = j * size + i;
				words[n] = (unsigned short)((((unsigned short)(bytes[n * 4 + 3] & 0xF0) << 4) | (unsigned short)(bytes[n * 4 + 2] & 0xF0)) << 4)
					| (unsigned short)(unsigned char)((bytes[n * 4 + 1] & 0xF0) | (bytes[n * 4] >> 4));
			}
		}
	}
	surface->Copy(min, max, (unsigned char *)m_Unknown120C);
	DX8Wrapper::_Update_Texture(m_Unknown120, LightmapTextures[BackLightmap]);
	REF_PTR_RELEASE(surface);
}

// NAME 2: the patch vertex, FVF 0x202 (0x4746C0)
struct LandVertexStruct
{
	float	X;
	float	Y;
	float	Z;
	float	U1;
	float	V1;
	float	U2;
	float	V2;
};

void LandPatchClass::Init(int size, int x, int y, float detail_u, float detail_v)
{
	Size = size;
	VertexCount = size * size;
	VertexBuffer = new DX8VertexBufferClass(0x202, VertexCount);
	Build_Vertices(x, y, detail_u, detail_v);
}

// NOT EXACT: frame (0x3C, the client 0x38); 3 forms tried (attempts.csv 0x474790)
void LandPatchClass::Build_Vertices(int x, int y, float detail_u, float detail_v)
{
	float lowest = 2.0f;
	float highest = -1.0f;
	float inv_size = 1.0f / (float)(unsigned int)Land->Field->Get_Size();
	float vscale = Land->Field->Get_VScale();
	VertexBufferClass::WriteLockClass lock(VertexBuffer);
	LandVertexStruct *vertex = (LandVertexStruct *)lock.Get_Vertex_Array();
	for (int j = 0; j < Size; j++) {
		int xi = x;
		for (int i = 0; i < Size; i++) {
			vertex->Y = (float)j;
			vertex->X = (float)i;
			float height = Land->Field->Get_Height_2(xi, j + y) * (1.0f / vscale);
			if (height <= lowest) lowest = height;
			if (highest <= height) highest = height;
			vertex->Z = height;
			vertex->V1 = (float)(j + y) * inv_size;
			vertex->V2 = vertex->V1 * detail_v;
			float u = (float)xi * inv_size;
			vertex->U1 = u;
			vertex->U2 = u * detail_u;
			vertex++;
			xi++;
		}
	}
	MidHeight = (highest + lowest) * 0.5f;
}

void LandPatchClass::Draw(void)
{
	int quarter = Land->m_UnknownD0[Level] >> 2;
	if (Quarters & 1) DX8Wrapper::Draw_Triangles(0, quarter / 3, 0, VertexCount);
	if (Quarters & 2) DX8Wrapper::Draw_Triangles(quarter, quarter / 3, 0, VertexCount);
	if (Quarters & 4) DX8Wrapper::Draw_Triangles(quarter * 2, quarter / 3, 0, VertexCount);
	if (Quarters & 8) DX8Wrapper::Draw_Triangles(quarter * 3, quarter / 3, 0, VertexCount);
}

void LandClass::Prepare_Lighting(void)
{
	SkyColorDX8 = DX8Wrapper::Convert_Color(SkyColor, 0.0f);
	if (LightingValid) {
		static const float max_angle = WWMATH_PI / 2.0f;
		float scale = 65535.0f / max_angle;
		float angle = SunAngle;
		SunFromWest = SunDirection.X < 0.0f;
		float low = WWMath::Clamp(angle - ShadowBias, 0.0f, max_angle);
		float high = WWMath::Clamp(angle + ShadowBias, 0.0f, max_angle);
		HorizonLow = (unsigned short)(low * scale);
		HorizonHigh = (unsigned short)(high * scale);
		ShadowStep = (float)(1.0 / (ceil(scale * ShadowBias) * 2.0));
		for (int i = 0; i < 256; i++) {
			const Vector3 &n = Normals[i];
			float d = n.X * SunDirection.X + n.Z * SunDirection.Z + SunDirection.Y * n.Y;
			Vector3 color;
			if (d > 0.0f) {
				color = LightColor * d;
			} else {
				color.Set(0.0f, 0.0f, 0.0f);
			}
			NormalColors[i] = DX8Wrapper::Convert_Color(color, 0.0f);
		}
	}
}

// NOT EXACT: shape (the client steps a colour pointer by 3 and tests extra before its loads; the loop bodies match); 3 forms tried
void LandClass::Apply_Colors(void)
{
	unsigned char *lightmap = (unsigned char *)m_Unknown120C;
	unsigned char *colors = Colors;
	unsigned char *extra = m_Unknown1214;
	int size = Field->Get_Size();
	unsigned int i, j, value;
	if (extra == NULL) {
		for (j = UpdateMin.J; j < (unsigned int)UpdateMax.J; j++) {
			for (i = UpdateMin.I; i < (unsigned int)UpdateMax.I; i++) {
				unsigned char *light = &lightmap[(j * size + i) * 4];
				unsigned char *color = &colors[(j * size + i) * 3];
				value = (color[0] * light[0]) >> 8;
				if (value > 255) value = 255;
				light[0] = (unsigned char)value;
				value = (color[1] * light[1]) >> 8;
				if (value > 255) value = 255;
				light[1] = (unsigned char)value;
				value = (color[2] * light[2]) >> 8;
				if (value > 255) value = 255;
				light[2] = (unsigned char)value;
			}
		}
	} else {
		for (j = UpdateMin.J; j < (unsigned int)UpdateMax.J; j++) {
			for (i = UpdateMin.I; i < (unsigned int)UpdateMax.I; i++) {
				unsigned char *add = &extra[j * size + i];
				unsigned char *light = &lightmap[(j * size + i) * 4];
				unsigned char *color = &colors[(j * size + i) * 3];
				value = ((light[0] + add[0]) * color[0]) >> 8;
				if (value > 255) value = 255;
				light[0] = (unsigned char)value;
				value = ((light[1] + add[0]) * color[1]) >> 8;
				if (value > 255) value = 255;
				light[1] = (unsigned char)value;
				value = ((light[2] + add[0]) * color[2]) >> 8;
				if (value > 255) value = 255;
				light[2] = (unsigned char)value;
			}
		}
	}
}

/*
**	Notes
**
**	The client's terrain, rebuilt on its own layout (land.h) from the constructor 0x468E90 and the
**	destructor 0x46A480 out: Create 0x46A3A0 (it stores the flag through a null pointer when
**	Load_Assets fails: REF_PTR_RELEASE clears the pointer first), the constructor, the destructor,
**	Clone 0x46C640, Class_ID 0x46C660 and the Cast_* virtuals 0x46C680-0x46C6E0. The rest of the
**	window (Load_Assets 0x4691D0, the patches, the lightmap, the tessellation, Render 0x474B30, the
**	decorations) is not rebuilt yet: VIDTEST draws with programs\vidtest\vidland.cpp, the functional
**	land this file held until 2026-10-27 (M2.4).
*/
