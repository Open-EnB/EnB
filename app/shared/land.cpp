// Earth & Beyond client (reconstruction): app\shared\ terrain (file name unknown). Notes at the end.
#include "land.h"
#include "heightfield.h"
#include "texture.h"
#include "vertmaterial.h"
#include <windows.h>

void Unknown_466F10(void);	// NAME 1: unknown; deletes a global object (0xBDC0D8) and clears it; called by the land's dtor only

LandClass::LandClass(void) :
	m_Unknown8C(0),
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
	m_Unknown137(false),
	m_Unknown138(true),
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
	m_Unknown1E0(false),
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
