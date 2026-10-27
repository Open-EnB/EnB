// Earth & Beyond client (reconstruction): app\shared\ terrain (file name unknown). Notes at the end.
#define ENABLE_WWPROFILE
#include "land.h"
#include "heightfield.h"
#include "texture.h"
#include "vertmaterial.h"
#include "dx8wrapper.h"
#include "dx8indexbuffer.h"
#include "vector2.h"
#include <windows.h>
#include "wwprofile.h"

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
	UpdateX0(0),
	UpdateY0(0),
	UpdateX1(0),
	UpdateY1(0),
	CursorX(0),
	CursorY(0),
	ThreadDone(false),
	m_Unknown1E4(NULL),
	m_Unknown1E8(NULL),
	m_Unknown1EC(NULL),
	m_Unknown1F0(NULL),
	m_Unknown1F4(NULL),
	m_Unknown1F8(NULL),
	m_UnknownDFC(0),
	m_UnknownDFE(0),
	m_UnknownE00(0),
	m_UnknownE04(true),
	m_UnknownE08(0),
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
	((LandClass *)land)->Compute_Shadows();
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
	int *min = &UpdateX0;
	int *max = &UpdateX1;
	min[0] = 0;
	min[1] = 0;
	int size = Field->Get_Size();
	max[0] = size;
	max[1] = size;
	Compute_Lighting();
	Compute_Shadows();
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
		UpdateX0 = CursorX;
		UpdateY0 = CursorY;
		UpdateY1 = CursorY + LightmapChunk;
		UpdateX1 = LightmapChunk + CursorX;
		Compute_Lighting();
		Compute_Shadows();
		Upload_Lightmap(&UpdateX0, &UpdateX1);
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
			UpdateX0 = 0;
			UpdateY0 = 0;
			int size = Field->Get_Size();
			UpdateX1 = size;
			UpdateY1 = size;
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
		int min[2];
		int max[2];
		min[0] = CursorX;
		min[1] = y;
		max[0] = min[0] + LightmapChunk;
		max[1] = LightmapChunk + y;
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
	m_Unknown00(NULL),
	Land(land),
	m_Unknown10(0),
	m_Unknown14(0),
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
