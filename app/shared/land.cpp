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
#include "ini.h"
#include "targa.h"
#include "ffactory.h"
#include "wwfile.h"
#include "assetmgr.h"
#include "textureloader.h"
#include "mesh.h"
#include "meshmdl.h"
#include "ww3d.h"
#include "pot.h"
#include "camera.h"
#include "matrix4.h"
#include "aabox.h"
#include "matrix3d.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void Unknown_466E70(HeightFieldClass *field, float detail_u, float detail_v);	// NAME 1: unknown; news the global object (0x90 bytes, 0xBDC0D8) Unknown_466F10 deletes; called by Load_Assets only
void Unknown_466F10(void);
// NAME 1: unknown; the object Unknown_466E70 makes (0x90 bytes, vtable 0xAE8974, ctor 0x466BE0): Update_Patches draws it with the land's scale while its +0x11 is set
class LandUnknown466BE0Class
{
public:
	virtual ~LandUnknown466BE0Class(void);
	void							Unknown_466F40(CameraClass &camera, int lod, const Matrix4 &scale);	// NAME 1: unknown
	char							m_Unknown04[0xD];
	bool							m_Unknown11;				// NAME 1: set by the ctor
};
extern LandUnknown466BE0Class *Unknown_BDC0D8;	// NAME 1: unknown
// NAME 2: the distance (in field cells) of level 1 per lod setting, doubled per level (0xB7188C)
int LandLodDistances[6] = { 8, 16, 24, 32, 48, 64 };
	// NAME 1: unknown; deletes a global object (0xBDC0D8) and clears it; called by the land's dtor only

LandClass::LandClass(void) :
	LightmapSerial(0),
	Reciprocals(NULL),
	HasSea(false),
	CameraCellX(-999999),
	CameraCellY(-999999),
	CameraCellZ(0),
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
		StripBuffers[i] = NULL;
		EdgeBuffers[i] = NULL;
	}
	for (i = 0; i < MAX_SEAS; i++) {
		SeaMaterials[i] = NULL;
		SeaTextures[i][0] = NULL;
		SeaTextures[i][1] = NULL;
		SeaDetailScales[i][0] = 1.0f;
		SeaDetailScales[i][1] = 1.0f;
	}
}

// NOT EXACT: inline-budget (ours inlines LandPatchClass's ctor and Init, the client calls 0x474530 / 0x4746C0; without them the frame slots differ); 7 forms (attempts.csv 0x4691D0)
bool LandClass::Load_Assets(const char *ini_name, bool flag)
{
	Set_Force_Visible(true);
	INIClass ini;
	ini.Load(ini_name);
	unsigned int max_size = DX8Wrapper::Get_Current_Caps()->Get_DX8_Caps().MaxTextureWidth;
	Field = HeightFieldClass::Load(ini_name, max_size);
	if (Field == NULL) {
		return false;
	}
	HasSea = ini.Get_Bool("GENERAL", "HASSEA", false);
	char color_map[100];
	char detail_map[100];
	ini.Get_String("LAND", "COLOR_MAP", "", color_map, sizeof(color_map));
	ini.Get_String("LAND", "DETAIL_MAP", "", detail_map, sizeof(detail_map));
	float detail_u = ini.Get_Float("LAND", "XDETAIL_SCALE", 1.0f);
	float detail_v = ini.Get_Float("LAND", "YDETAIL_SCALE", 1.0f);
	bool flip_tga = ini.Get_Bool("LAND", "FLIP_TGA", false);
	GridWidth = ini.Get_Int("LAND", "Patch_Grid_Width", 16);
	LightmapChunk = ini.Get_Int("LAND", "LIGHTMAP_CHUNK", 8);
	GridMask = GridWidth - 1;
	GridLog2 = Find_POT_Log2(GridWidth);
	HalfGridWidth = (unsigned int)GridWidth >> 1;
	PatchCount = GridWidth * GridWidth;
	PatchLog2 = Field->Get_Log2() - GridLog2;
	PatchSize = 1 << PatchLog2;
	PatchVerts = PatchSize + 1;
	Reciprocals = new float[Field->Get_Size() * 2 - 1];
	m_Unknown1E4 = new unsigned char[Field->Get_Count()];
	m_Unknown1E8 = new unsigned short[Field->Get_Count()];
	m_Unknown1EC = new unsigned short[Field->Get_Count()];
	m_Unknown1F0 = new unsigned char[Field->Get_Count()];
	m_Unknown1F4 = new unsigned char[Field->Get_Count()];
	m_Unknown1F8 = new unsigned char[Field->Get_Count()];
	m_Unknown120C = new unsigned int[Field->Get_Count()];
	PatchRecords = new LandPatchRecordClass[PatchCount];
	Patches = new LandPatchClass *[PatchCount];
	int i;
	for (i = 0; i < PatchCount; i++) {
		Patches[i] = NULL;
	}

	int width, height, bits;
	bool windowed;
	WW3D::Get_Device_Resolution(width, height, bits, windowed);
	Is16Bit = bits <= 16;
	WW3DFormat format = Is16Bit ? WW3D_FORMAT_A4R4G4B4 : WW3D_FORMAT_A8R8G8B8;
	m_Unknown120 = new TextureClass(Field->Get_Size(), Field->Get_Size(), format, MIP_LEVELS_1, TextureClass::POOL_SYSTEMMEM);
	for (i = 0; i < 2; i++) {
		LightmapTextures[i] = new TextureClass(Field->Get_Size(), Field->Get_Size(), format, MIP_LEVELS_1, TextureClass::POOL_DEFAULT);
		LightmapTextures[i]->Set_Mag_Filter(TextureClass::FILTER_TYPE_BEST);
		LightmapTextures[i]->Set_Min_Filter(TextureClass::FILTER_TYPE_BEST);
		LightmapTextures[i]->Set_Mip_Mapping(TextureClass::FILTER_TYPE_NONE);
	}

	Targa targa;
	if (targa.Open(color_map, TGA_READMODE) != 0) {
		return false;
	}
	if (!flip_tga) {
		targa.Header.ImageDescriptor ^= 0x20;
	}
	if (targa.Load(color_map, TGAF_IMAGE, false) != 0) {
		return false;
	}
	int pixels = targa.Header.Width * targa.Header.Height;
	unsigned char *image = (unsigned char *)targa.GetImage();
	Colors = new unsigned char[pixels * 3];
	switch (targa.Header.PixelDepth) {
		case 24:
			memcpy(Colors, image, pixels * 3);
			break;
		case 32:
			m_Unknown1214 = new unsigned char[pixels];
			for (int y = 0; y < targa.Header.Height; y++) {
				for (int x = 0; x < targa.Header.Width; x++) {
					int n = y * targa.Header.Width + x;
					Colors[n * 3] = image[n * 4];
					Colors[n * 3 + 1] = image[n * 4 + 1];
					Colors[n * 3 + 2] = image[n * 4 + 2];
					m_Unknown1214[n] = image[n * 4 + 3];
				}
			}
			break;
	}

	// a colour map larger than a texture may be: average it down to max_size a side
	if (max_size < (unsigned int)targa.Header.Width) {
		unsigned char *colors = new unsigned char[max_size * max_size * 3];
		unsigned char *extra = NULL;
		if (m_Unknown1214 != NULL) {
			extra = new unsigned char[max_size * max_size];
		}
		unsigned int step = (unsigned int)targa.Header.Width / max_size;
		unsigned int area = step * step;
		for (unsigned int j = 0; j < max_size; j++) {
			for (unsigned int i = 0; i < max_size; i++) {
				unsigned int r = 0;
				unsigned int g = 0;
				unsigned int b = 0;
				unsigned int a = 0;
				for (unsigned int y = 0; y < step; y++) {
					for (unsigned int x = 0; x < step; x++) {
						int n = (j * step + y) * targa.Header.Width + i * step + x;
						r += Colors[n * 3];
						g += Colors[n * 3 + 1];
						b += Colors[n * 3 + 2];
						if (m_Unknown1214 != NULL) {
							a += m_Unknown1214[n];
						}
					}
				}
				r /= area;
				g /= area;
				b /= area;
				a /= area;
				int k = j * max_size + i;
				colors[k * 3] = (unsigned char)r;
				colors[k * 3 + 1] = (unsigned char)g;
				colors[k * 3 + 2] = (unsigned char)b;
				if (m_Unknown1214 != NULL) {
					extra[k] = (unsigned char)a;
				}
			}
		}
		delete [] Colors;
		Colors = colors;
		if (m_Unknown1214 != NULL) {
			delete [] m_Unknown1214;
			m_Unknown1214 = extra;
		}
	}

	DetailTexture = WW3DAssetManager::Get_Instance()->Get_Texture(detail_map);
	if (!DetailTexture->Is_Initialized()) {
		TextureLoader::Request_Foreground_Loading(DetailTexture);
	}
	DetailTexture->Set_Mag_Filter(TextureClass::FILTER_TYPE_BEST);
	DetailTexture->Set_Min_Filter(TextureClass::FILTER_TYPE_BEST);
	DetailTexture->Set_Mip_Mapping(TextureClass::FILTER_TYPE_BEST);

	Material = new VertexMaterialClass;
	Material->Set_Lighting(false);
	Shader.Set_Primary_Gradient(ShaderClass::GRADIENT_DISABLE);
	Shader.Set_Secondary_Gradient(ShaderClass::SECONDARY_GRADIENT_DISABLE);
	Shader.Set_Texturing(ShaderClass::TEXTURING_ENABLE);
	Shader.Set_Post_Detail_Color_Func(flag ? ShaderClass::DETAILCOLOR_SCALE2X : ShaderClass::DETAILCOLOR_SCALE);
	Shader.Set_Fog_Func(ShaderClass::FOG_ENABLE);
	Shader.Set_Cull_Mode(ShaderClass::CULL_MODE_ENABLE);

	i = PatchCount;
	while (i--) {
		Patches[i] = new LandPatchClass(this);
	}
	Build_Index_Buffers(PatchVerts);
	for (unsigned int x = 0; x < (unsigned int)GridWidth; x++) {
		for (unsigned int y = 0; y < (unsigned int)GridWidth; y++) {
			Patches[y * GridWidth + x]->Init(PatchVerts, x * PatchSize, y * PatchSize, detail_u, detail_v);
		}
	}

	// the horizons: from the height map's .pld when it is newer than the map, the colour map and the ini
	char name[100];
	strcpy(name, Field->Get_Name());
	_strlwr(name);
	char *ext = strchr(name, '.');
	file_auto_ptr pgm(_TheFileFactory, Field->Get_Name());
	if (ext != NULL && ext[1] == 'p' && ext[2] == 'g' && ext[3] == 'm' && pgm->Is_Available()) {
		ext[2] = 'l';
		ext[3] = 'd';
		bool newer = false;
		file_auto_ptr pld(_TheFileFactory, name);
		if (pld->Is_Available()) {
			unsigned long time = pld->Get_Date_Time();
			newer = time > pgm->Get_Date_Time();
			file_auto_ptr colors(_TheFileFactory, color_map);
			if (colors->Is_Available() && colors->Get_Date_Time() >= time) {
				newer = false;
			}
			file_auto_ptr ini_file(_TheFileFactory, ini_name);
			if (ini_file->Is_Available() && ini_file->Get_Date_Time() >= time) {
				newer = false;
			}
		}
		bool loaded = false;
		if (newer) {
			loaded = Load_PLD(pld->File_Name());
		}
		if (!loaded) {
			Compute_Horizons(ini);
			Save_PLD(pld->File_Name());
		}
	}

	if (HasSea) {
		char swatch[100];
		ini.Get_String("SEA", "SWATCH", "water_swatch", swatch, sizeof(swatch));
		SeaLevel = ini.Get_Float("SEA", "SEA_LEVEL", 0.0f);
		SeaDetailScales[0][0] = ini.Get_Float("SEA", "DETAIL_SCALE", 1.0f);
		SeaDetailScales[0][1] = ini.Get_Float("SEA", "DETAIL_SCALE2", 1.0f);
		SeaDetailScales[1][0] = ini.Get_Float("SEA", "DETAIL_SCALE3", 1.0f);
		SeaDetailScales[1][1] = ini.Get_Float("SEA", "DETAIL_SCALE4", 1.0f);
		RenderObjClass *sea = WW3DAssetManager::Get_Instance()->Create_Render_Obj(swatch);
		if (sea != NULL) {
			if (sea->Class_ID() == RenderObjClass::CLASSID_MESH) {
				MeshModelClass *model = ((MeshClass *)sea)->Get_Model();
				SeaCount = model->Get_Pass_Count();
				if (SeaCount > MAX_SEAS) {
					SeaCount = MAX_SEAS;
				}
				for (int pass = 0; pass < SeaCount; pass++) {
					SeaTextures[pass][0] = model->Get_Single_Texture(pass, 0);
					SeaTextures[pass][1] = model->Get_Single_Texture(pass, 1);
					SeaShaders[pass] = model->Get_Single_Shader(pass);
					SeaMaterials[pass] = model->Get_Single_Material(pass);
				}
			}
			sea->Release_Ref();
		}
		SeaShaders[0].Set_Fog_Func(ShaderClass::FOG_ENABLE);
		SeaShaders[0].Set_Cull_Mode(ShaderClass::CULL_MODE_DISABLE);
		SeaShaders[1].Set_Fog_Func(ShaderClass::FOG_ENABLE);
		SeaShaders[1].Set_Cull_Mode(ShaderClass::CULL_MODE_DISABLE);
	}
	Unknown_466E70(Field, detail_u, detail_v);
	return true;
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
			REF_PTR_RELEASE(SeaMaterials[i]);
			REF_PTR_RELEASE(SeaTextures[i][0]);
			REF_PTR_RELEASE(SeaTextures[i][1]);
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
		REF_PTR_RELEASE(StripBuffers[i]);
		if (i != 0) {
			REF_PTR_RELEASE(EdgeBuffers[i]);
		}
	}
	if (Reciprocals != NULL) delete [] Reciprocals;
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

// NOT EXACT: shape (2 rows: the client returns false through al = HasSea, ours xor al, al; 7 forms, attempts.csv 0x46BCC0)
bool LandClass::Save_Sea_Mask(const char *filename)
{
	if (HasSea) {
		unsigned int size = Field->Get_Size();
		unsigned char *mask = new unsigned char[size * size];
		unsigned int row = 0;
		for (unsigned int j = 0; j < size; j++) {
			for (unsigned int i = 0; i < size; i++) {
				if (Field->Get_Height(i, j) < SeaLevel) {
					mask[row + i] = 0;
				} else {
					mask[row + i] = 0xFF;
				}
			}
			row += size;
		}
		Targa targa;
		memset(&targa.Header, 0, sizeof(targa.Header));
		targa.Header.ImageType = 3;
		targa.Header.Width = (short)size;
		targa.Header.Height = (short)size;
		targa.Header.PixelDepth = 8;
		targa.SetImage((char *)mask);
		FileClass *file = _TheWritingFileFactory->Get_File(filename);
		if (file != NULL) {
			file->Create();
			file->Close();
			_TheWritingFileFactory->Return_File(file);
		}
		targa.Save(filename, TGAF_IMAGE, false);
		delete [] mask;
		return true;
	}
	return false;
}

// NOT EXACT: inline-budget (the client calls the first push_back out of line (0x46BF70) and inlines _Freefront in pop_front and the destructor; ours the reverse)
void LandClass::Limit_Levels(int x, int y)
{
	std::deque<Vector2i> queue;
	queue.push_back(Vector2i(x, y));
	Vector2i next;
	while (!queue.empty()) {
		Vector2i &cell = queue.front();
		LandPatchClass *patch = Patches[cell.J * GridWidth + cell.I];
		if (patch->Visited) {
			queue.pop_front();
		} else {
			patch->Visited = true;
			unsigned int level = Patches[cell.J * GridWidth + cell.I]->Level + 1;
			if (level > 4) {
				level = 4;
			}
			next.J = cell.J;
			next.I = cell.I + 1;
			if (next.I < GridWidth && !Patches[next.J * GridWidth + next.I]->Visited) {
				queue.push_back(next);
				patch = Patches[next.J * GridWidth + next.I];
				if ((unsigned int)patch->Level > level) {
					patch->Level = level;
				}
			}
			next.I = cell.I - 1;
			if (next.I >= 0 && !Patches[next.J * GridWidth + next.I]->Visited) {
				queue.push_back(next);
				patch = Patches[next.J * GridWidth + next.I];
				if ((unsigned int)patch->Level > level) {
					patch->Level = level;
				}
			}
			next.I = cell.I;
			next.J = cell.J + 1;
			if (next.J < GridWidth && !Patches[next.J * GridWidth + next.I]->Visited) {
				queue.push_back(next);
				patch = Patches[next.J * GridWidth + next.I];
				if ((unsigned int)patch->Level > level) {
					patch->Level = level;
				}
			}
			next.J = cell.J - 1;
			if (next.J >= 0 && !Patches[next.J * GridWidth + next.I]->Visited) {
				queue.push_back(next);
				patch = Patches[next.J * GridWidth + next.I];
				if ((unsigned int)patch->Level > level) {
					patch->Level = level;
				}
			}
			queue.pop_front();
		}
	}
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

// NOT EXACT: shape (written from the disassembly: the client's frame is 0x18 larger, it copies the camera position and ors the change tests; 5 forms, attempts.csv 0x46B3C0)
void LandClass::Update_Patches(CameraClass &camera, int lod)
{
	Vector3 pos = camera.Get_Position();
	float extent = (float)(unsigned int)PatchSize * Field->Get_Spacing();
	float half = extent * 0.5f;
	float inv_extent = 1.0f / extent;
	float inv_spacing = 1.0f / Field->Get_Spacing();
	int cell_x = (int)(inv_extent * pos.X);
	int cell_y = (int)(inv_extent * pos.Y);
	int cell_z = (int)(inv_extent * pos.Z);
	int i, j;
	if (Dirty == true || CameraCellX != cell_x || CameraCellZ != cell_z || CameraCellY != cell_y) {
		Dirty = false;
		for (i = 0; i < GridWidth; i++) {
			for (j = 0; j < GridWidth; j++) {
				LandPatchClass *patch = Patches[j * GridWidth + i];
				int x, y;
				if (Field->Is_Wrapping()) {
					x = ((i - cell_x) & GridMask) - HalfGridWidth + cell_x;
					y = ((j - cell_y) & GridMask) - HalfGridWidth + cell_y;
				} else {
					x = i - HalfGridWidth;
					y = j - HalfGridWidth;
				}
				patch->X = x * extent;
				patch->Y = y * extent;
				patch->Z = 0.0f;
				Vector3 center(patch->X + half, patch->Y + half, Field->Get_VScale() * patch->MidHeight);
				float distance = (pos - center).Length();
				int cells = (int)(distance * inv_spacing);
				int threshold = LandLodDistances[lod];
				int level;
				for (level = 0; level < 4; level++) {
					if (cells < threshold) {
						break;
					}
					threshold <<= 1;
				}
				patch->Level = level;
				patch->Quarters = 0;
				patch->Visited = false;
			}
		}
		Limit_Levels((HalfGridWidth + cell_x) & GridMask, (HalfGridWidth + cell_y) & GridMask);
		for (i = 0; i < GridWidth; i++) {
			for (j = 0; j < GridWidth; j++) {
				LandPatchClass *patch = Patches[j * GridWidth + i];
				int level = patch->Level;
				if (level != 0) {
					if (Patches[((i - 1) & GridMask) + j * GridWidth]->Level < level) {
						patch->Quarters |= 1;
					}
					if (Patches[((i + 1) & GridMask) + j * GridWidth]->Level < level) {
						patch->Quarters |= 2;
					}
					if (Patches[((j - 1) & GridMask) * GridWidth + i]->Level < level) {
						patch->Quarters |= 4;
					}
					if (Patches[((j + 1) & GridMask) * GridWidth + i]->Level < level) {
						patch->Quarters |= 8;
					}
				}
			}
		}
		CameraCellX = cell_x;
		CameraCellY = cell_y;
		CameraCellZ = cell_z;
	}
	for (i = 0; i < GridWidth; i++) {
		for (j = 0; j < GridWidth; j++) {
			int index = j * GridWidth + i;
			LandPatchClass *patch = Patches[index];
			LandPatchRecordClass *record = &PatchRecords[index];
			Vector3 min(patch->X, patch->Y, patch->Z);
			Vector3 size((float)(unsigned int)PatchSize * Field->Get_Spacing(), (float)(unsigned int)PatchSize * Field->Get_YSpacing(),
				(float)Field->Get_Max_Value() * Field->Get_VScale());
			Vector3 max = min + size;
			AABoxClass box(MinMaxAABoxClass(min, max));
			bool visible = !camera.Cull_Box(box);
			patch->Visible = visible;
			record->Visible = visible;
			record->Level = patch->Level;
		}
	}
	if (Unknown_BDC0D8->m_Unknown11) {
		Matrix4 scale(Vector4(Field->Get_Spacing(), 0.0f, 0.0f, 0.0f), Vector4(0.0f, Field->Get_YSpacing(), 0.0f, 0.0f),
			Vector4(0.0f, 0.0f, Field->Get_VScale(), 0.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f));
		Unknown_BDC0D8->Unknown_466F40(camera, lod, scale);
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
	X(0),
	Y(0),
	Z(0)
{
}

LandPatchRecordClass::LandPatchRecordClass(void) :
	m_Unknown10(0),
	m_Unknown14(0),
	m_Unknown18(0),
	m_Unknown1C(0),
	Visible(false),
	Level(0)
{
}

void LandClass::Set_Strip_Buffer(int level)
{
	DX8Wrapper::Set_Index_Buffer(StripBuffers[level], 0);
}

void LandClass::Set_Edge_Buffer(int level)
{
	DX8Wrapper::Set_Index_Buffer(EdgeBuffers[level], 0);
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
	int quarter = Land->EdgeCounts[Level] >> 2;
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

// NOT EXACT: order-only (the shadowed vertex: the client loads +0x1F0's factor first; 64 rows masked; attempts.csv 0x46CAE0). The lit vertices: the sun's normal colour plus the sky's; the shadowed: the sky's; between the horizons a blend
void LandClass::Compute_Lighting(void)
{
	unsigned int i, j, n, r, g, b;
	unsigned char *color;
	unsigned char *sky = (unsigned char *)&SkyColorDX8;
	if (LightingValid) {
		float step = ShadowStep;
		unsigned short low = HorizonLow;
		unsigned short high = HorizonHigh;
		unsigned short horizon;
		if (SunFromWest) {
		for (j = UpdateMin.J; j < (unsigned int)UpdateMax.J; j++) {
			for (i = UpdateMin.I; i < (unsigned int)UpdateMax.I; i++) {
				n = Field->Get_Size() * j + i;
				horizon = m_Unknown1E8[n];
				if (low > horizon) {
					m_Unknown120C[n] = NormalColors[m_Unknown1E4[n]];
					color = (unsigned char *)&m_Unknown120C[n];
					r = (unsigned char)((m_Unknown1F0[n] * sky[2]) >> 8) + color[2];
					g = (unsigned char)((m_Unknown1F4[n] * sky[1]) >> 8) + color[1];
					b = (unsigned char)((m_Unknown1F8[n] * sky[0]) >> 8) + color[0];
					if (r > 255) r = 255;
					if (g > 255) g = 255;
					if (b > 255) b = 255;
					color[0] = (unsigned char)b;
					color[1] = (unsigned char)g;
					color[2] = (unsigned char)r;
					color[3] = 0xFF;
				} else if (high > horizon) {
					float t = (float)(int)(high - horizon) * step;
					m_Unknown120C[n] = NormalColors[m_Unknown1E4[n]];
					color = (unsigned char *)&m_Unknown120C[n];
					r = (unsigned int)((float)(unsigned char)((m_Unknown1F0[n] * sky[2]) >> 8) + (float)color[2] * t);
					g = (unsigned int)((float)(unsigned char)((m_Unknown1F4[n] * sky[1]) >> 8) + (float)color[1] * t);
					b = (unsigned int)((float)(unsigned char)((m_Unknown1F8[n] * sky[0]) >> 8) + (float)color[0] * t);
					if (r > 255) r = 255;
					if (g > 255) g = 255;
					if (b > 255) b = 255;
					color[0] = (unsigned char)b;
					color[1] = (unsigned char)g;
					color[2] = (unsigned char)r;
					color[3] = 0xFF;
				} else {
					m_Unknown120C[n] = SkyColorDX8;
					color = (unsigned char *)&m_Unknown120C[n];
					unsigned int f1 = m_Unknown1F4[n];
					unsigned int f2 = m_Unknown1F8[n];
					color[2] = (unsigned char)((color[2] * m_Unknown1F0[n]) >> 8);
					color[3] = 0xFF;
					color[1] = (unsigned char)((color[1] * f1) >> 8);
					color[0] = (unsigned char)((color[0] * f2) >> 8);
				}
			}
		}
		} else {
		for (j = UpdateMin.J; j < (unsigned int)UpdateMax.J; j++) {
			for (i = UpdateMin.I; i < (unsigned int)UpdateMax.I; i++) {
				n = Field->Get_Size() * j + i;
				horizon = m_Unknown1EC[n];
				if (low > horizon) {
					m_Unknown120C[n] = NormalColors[m_Unknown1E4[n]];
					color = (unsigned char *)&m_Unknown120C[n];
					r = (unsigned char)((m_Unknown1F0[n] * sky[2]) >> 8) + color[2];
					g = (unsigned char)((m_Unknown1F4[n] * sky[1]) >> 8) + color[1];
					b = (unsigned char)((m_Unknown1F8[n] * sky[0]) >> 8) + color[0];
					if (r > 255) r = 255;
					if (g > 255) g = 255;
					if (b > 255) b = 255;
					color[0] = (unsigned char)b;
					color[1] = (unsigned char)g;
					color[2] = (unsigned char)r;
					color[3] = 0xFF;
				} else if (high > horizon) {
					float t = (float)(int)(high - horizon) * step;
					m_Unknown120C[n] = NormalColors[m_Unknown1E4[n]];
					color = (unsigned char *)&m_Unknown120C[n];
					r = (unsigned int)((float)(unsigned char)((m_Unknown1F0[n] * sky[2]) >> 8) + (float)color[2] * t);
					g = (unsigned int)((float)(unsigned char)((m_Unknown1F4[n] * sky[1]) >> 8) + (float)color[1] * t);
					b = (unsigned int)((float)(unsigned char)((m_Unknown1F8[n] * sky[0]) >> 8) + (float)color[0] * t);
					if (r > 255) r = 255;
					if (g > 255) g = 255;
					if (b > 255) b = 255;
					color[0] = (unsigned char)b;
					color[1] = (unsigned char)g;
					color[2] = (unsigned char)r;
					color[3] = 0xFF;
				} else {
					m_Unknown120C[n] = SkyColorDX8;
					color = (unsigned char *)&m_Unknown120C[n];
					unsigned int f1 = m_Unknown1F4[n];
					unsigned int f2 = m_Unknown1F8[n];
					color[2] = (unsigned char)((color[2] * m_Unknown1F0[n]) >> 8);
					color[3] = 0xFF;
					color[1] = (unsigned char)((color[1] * f1) >> 8);
					color[0] = (unsigned char)((color[0] * f2) >> 8);
				}
			}
		}
		}
	} else {
		for (j = UpdateMin.J; j < (unsigned int)UpdateMax.J; j++) {
			for (i = UpdateMin.I; i < (unsigned int)UpdateMax.I; i++) {
				n = Field->Get_Size() * j + i;
				m_Unknown120C[n] = SkyColorDX8;
				color = (unsigned char *)&m_Unknown120C[n];
				unsigned int f1 = m_Unknown1F4[n];
				unsigned int f2 = m_Unknown1F8[n];
				unsigned int f0 = m_Unknown1F0[n];
				unsigned int c2 = color[2];
				unsigned int c1 = color[1];
				unsigned int c0 = color[0];
				color[2] = (unsigned char)((f0 * c2) >> 8);
				color[1] = (unsigned char)((c1 * f1) >> 8);
				color[0] = (unsigned char)((c0 * f2) >> 8);
			}
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

// NOT EXACT: field-offset / order (the store order of the strip's index pairs; 2 forms, attempts.csv 0x478240)
void LandClass::Build_Index_Buffers(int size)
{
	int level = MAX_LEVELS - 1;
	do {
		int step = 1 << level;
		int quads = (size - 1) / step;
		StripCounts[level] = (quads + 2) * quads * 2 - 2;
		StripBuffers[level] = new DX8IndexBufferClass(StripCounts[level] + 2);
		{
			IndexBufferClass::WriteLockClass lock(StripBuffers[level]);
			unsigned short *index = lock.Get_Index_Array();
			bool forward = true;
			for (int row = step; row < size; row += step) {
				if (forward) {
					for (int col = 0; col < size - step; col += step) {
						*index++ = row * size + col;
						*index++ = col + (row - step) * size;
					}
					unsigned short last = (row + 1) * size - 1;
					*index++ = last;
					*index++ = (row - step + 1) * size - 1;
					*index++ = last;
					*index++ = last;
				} else {
					*index++ = (row - step + 1) * size - 1;
					*index++ = (row + 1) * size - 1;
					unsigned short corner = 0;
					for (int col = size - 1; col > 0; col -= step) {
						*index++ = (row - step) * size - step + col;
						corner = col + (row * size - step);
						*index++ = corner;
					}
					*index++ = corner;
					*index++ = corner;
				}
				forward = !forward;
			}
		}
		if (level != 0) {
			EdgeCounts[level] = quads * 12;
			EdgeBuffers[level] = new DX8IndexBufferClass(EdgeCounts[level]);
			IndexBufferClass::WriteLockClass lock(EdgeBuffers[level]);
			unsigned short *index = lock.Get_Index_Array();
			int half = step >> 1;
			int i;
			for (i = step; i <= size; i += step) {
				*index++ = (i - step) * size;
				*index++ = i * size;
				*index++ = (i - half) * size;
			}
			for (i = step; i < size; i += step) {
				*index++ = (i - step + 1) * size - 1;
				*index++ = (i - half + 1) * size - 1;
				*index++ = (i + 1) * size - 1;
			}
			for (i = step; i < size; i += step) {
				*index++ = i - step;
				*index++ = i - half;
				*index++ = i;
			}
			int last = (size - 1) * size;
			for (i = step; i < size; i += step) {
				*index++ = last - step + i;
				*index++ = i + last;
				*index++ = i + (last - half);
			}
		}
	} while (level-- != 0);
}

// NAME 2: the eight compass directions' unit vectors x (1 - 1/sqrt 2) mixes, per direction (0xAE97BC, 0xAE97DC)
static const float LandDirectionX[8] = { 0.70710677f, 0.29289323f, -0.29289323f, -0.70710677f, -0.70710677f, -0.29289323f, 0.29289323f, 0.70710677f };
static const float LandDirectionY[8] = { 0.29289323f, 0.70710677f, 0.70710677f, 0.29289323f, -0.29289323f, -0.70710677f, -0.70710677f, -0.29289323f };
// NAME 2: per direction and per horizon angle step (256), the sky area term and the sun term (0xBDC0E8)
Vector3 LandHorizonTable[8][256];

void LandClass::Init_Horizon_Tables(void)
{
	unsigned int i;
	for (i = 0; i < Field->Get_Size() * 2 - 1; i++) {
		Reciprocals[i] = 1.0f / (float)(i + 1);
	}
	for (i = 0; i < 256; i++) {
		unsigned int bits = ((i & 0xFF) + 0x770) << 19 | 0x7FFFF;
		static const float half_pi = WWMATH_PI / 2.0f;
		static const float one_over_two_pi = 1.0f / (2.0f * WWMATH_PI);
		float angle = (float)(half_pi - atan(*(float *)&bits));
		float s = (float)sin(angle);
		float area = (float)((angle - sin(angle + angle) * 0.5) * one_over_two_pi);
		for (unsigned int d = 0; d < 8; d++) {
			LandHorizonTable[d][i].Set(area * LandDirectionX[d], area * LandDirectionY[d], s * s * 0.125f);
		}
	}
}

// NOT EXACT: shape (21 masked rows: the mask test's operand order and the heights' restore; 5 forms, attempts.csv 0x46E670)
void LandClass::Compute_Horizons(INIClass &ini)
{
	std::vector< RefCountPtr<RenderObjClass> > casters;
	int index = 0;
	char section[128];
	sprintf(section, "SHADOW%d", 0);
	while (ini.Find_Section(section) != NULL) {
		char asset[128];
		ini.Get_String(section, "Asset", "", asset, sizeof(asset));
		RefCountPtr<RenderObjClass> caster = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(asset));
		if (caster != NULL) {
			float scale = ini.Get_Float(section, "Scale", 1.0f);
			caster->Scale(scale);
			Matrix3D tm(true);
			char text[256];
			ini.Get_String(section, "M", "", text, sizeof(text));
			char *token = strtok(text, ",");
			for (int col = 0; col < 4; col++) {
				for (int row = 0; row < 3; row++) {
					if (token != NULL) {
						tm[row][col] = (float)atof(token);
						token = strtok(NULL, ",");
					}
				}
			}
			caster->Set_Transform(tm);
			casters.push_back(caster);
		}
		index++;
		sprintf(section, "SHADOW%d", index);
	}
	unsigned short *heights = new unsigned short[Field->Get_Count()];
	unsigned char *mask = new unsigned char[Field->Get_Count()];
	unsigned short *field_heights = Field->Get_Heights();
	memcpy(heights, field_heights, Field->Get_Count() * 2);
	memset(mask, 0, Field->Get_Count());
	Unknown_46E120(casters, mask);
	Init_Horizon_Tables();
	Unknown_46F120();
	unsigned char *a;
	unsigned char *b;
	unsigned char *c;
	Unknown_473290(&a, &b, &c);
	Unknown_470F80(a, b, c);
	for (int i = 0; i < (int)Field->Get_Count(); i++) {
		if (mask[i] != 0) {
			m_Unknown1E8[i] = 0xFFFF;
			m_Unknown1EC[i] = 0xFFFF;
		}
	}
	memcpy(Field->Get_Heights(), heights, Field->Get_Count() * 2);
	delete [] a;
	delete [] b;
	delete [] c;
	delete [] heights;
	delete [] mask;
}

// NOT EXACT: order-only (3 masked rows: the client loads Spacing before VScale for VScale / Spacing; 6 forms, attempts.csv 0x473290)
void LandClass::Unknown_473290(unsigned char **a, unsigned char **b, unsigned char **c)
{
	int size = Field->Get_Size();
	*a = new unsigned char[Field->Get_Count() * 3];
	*b = new unsigned char[Field->Get_Count() * 3];
	*c = new unsigned char[Field->Get_Count() * 4];
	float ratio = Field->Get_VScale() / Field->Get_Spacing();
	unsigned char *a_row = *a;
	unsigned char *b_row = *b;
	unsigned char *c_row = *c;
	int row;
	for (row = 0; row < size; row++) {
		Unknown_4733C0(row, ratio, c_row);
		c_row += size * 4;
		Unknown_473780(row, ratio, b_row);
		b_row += size * 3;
	}
	for (row = 0; row < size; row++) {
		Unknown_473BC0(row, ratio, a_row);
		a_row += size * 3;
	}
}

bool LandClass::Load_PLD(const char *filename)
{
	FILE *file = fopen(filename, "rb");
	if (file != NULL) {
		unsigned int count = 0;
		if (fread(&count, 4, 1, file) == 1 && count == Field->Get_Count()
			&& fread(m_Unknown1E4, 1, count, file) == count
			&& fread(m_Unknown1E8, 2, count, file) == count
			&& fread(m_Unknown1EC, 2, count, file) == count
			&& fread(m_Unknown1F0, 1, count, file) == count
			&& fread(m_Unknown1F4, 1, count, file) == count
			&& fread(m_Unknown1F8, 1, count, file) == count
			&& fread(Normals, sizeof(Vector3), 256, file) == 256) {
			fclose(file);
			return true;
		}
		fclose(file);
	}
	return false;
}

void LandClass::Save_PLD(const char *filename)
{
	FILE *file = fopen(filename, "wb");
	if (file != NULL) {
		unsigned int count = Field->Get_Count();
		if (fwrite(&count, 4, 1, file) == 1
			&& fwrite(m_Unknown1E4, 1, count, file) == count
			&& fwrite(m_Unknown1E8, 2, count, file) == count
			&& fwrite(m_Unknown1EC, 2, count, file) == count
			&& fwrite(m_Unknown1F0, 1, count, file) == count
			&& fwrite(m_Unknown1F4, 1, count, file) == count
			&& fwrite(m_Unknown1F8, 1, count, file) == count) {
			fwrite(Normals, sizeof(Vector3), 256, file);
		}
	}
	fclose(file);
}

/*
**	Notes
**
**	The client's terrain, rebuilt on its own layout (land.h) from the constructor 0x468E90 and the
**	destructor 0x46A480 out: Create 0x46A3A0 (it stores the flag through a null pointer when
**	Load_Assets fails: REF_PTR_RELEASE clears the pointer first), the constructor, the destructor,
**	Clone 0x46C640, Class_ID 0x46C660 and the Cast_* virtuals 0x46C680-0x46C6E0. Load_Assets 0x4691D0
**	(functional: the height field, the colour map averaged down to the device's largest texture, the
**	lightmap textures, the patches, the horizon tables from a .pld beside the .pgm when it is newer
**	than the .pgm, the colour map and the ini (Load_PLD 0x46EB60 / Save_PLD 0x46ECF0), the sea swatch).
**	The rest of the window (Compute_Horizons 0x46E670, Update_Patches, the tessellation, Render
**	0x474B30, the decorations) is not rebuilt yet: VIDTEST draws with programs\vidtest\vidland.cpp,
**	the functional land this file held until 2026-10-27 (M2.4).
*/
