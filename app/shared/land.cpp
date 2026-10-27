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
#include "matpass.h"
#include "obbox.h"
#include "rinfo.h"
#include "dx8fvf.h"
#include "vector.h"
#include "landdmap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

// NAME 1: unknown; draws triangles of the land under a projector (0x477E20, its vector by value)
void Unknown_477E20(std::vector<Vector3> verts);

// NAME 2: a quad's four corners at a height into the sea's vertex list (Render 0x474B30 inlines the four Adds)
static inline void Add_Sea_Quad(DynamicVectorClass<Vector3> &quads, float x, float y, float dx, float dy, float z)
{
	quads.Add(Vector3(x, y, z));
	quads.Add(Vector3(x + dx, y, z));
	quads.Add(Vector3(x + dx, y + dy, z));
	quads.Add(Vector3(x, y + dy, z));
}

// NOT EXACT: written from the pseudocode (0x474B30); not run yet (VIDTEST draws with vidland.cpp)
void LandClass::Render(RenderInfoClass &rinfo)
{
	DX8Wrapper::Set_Texture(0, LightmapTextures[FrontLightmap]);
	DX8Wrapper::Set_Texture(1, DetailTexture);
	DX8Wrapper::Set_Shader(Shader);
	DX8Wrapper::Set_Material(Material);
	static DynamicVectorClass<Vector3> sea_quads(10);
	sea_quads.Delete_All();
	bool companion = Unknown_BDC0D8->m_Unknown11;
	float extent_x = Field->Get_Spacing() * (float)(unsigned int)PatchSize;
	float extent_y = (float)(unsigned int)PatchSize * Field->Get_YSpacing();
	Unknown_BDC0D8->Unknown_466F60(rinfo);
	int level, i;
	for (level = 0; level < MAX_LEVELS; level++) {
		LandPatchClass **patch = Patches;
		for (i = PatchCount; i != 0; i--, patch++) {
			if (!(*patch)->Visible || (*patch)->Level != level) {
				continue;
			}
			if (!companion) {
				Matrix3D tm(Vector3(Field->Get_Spacing(), 0.0f, 0.0f), Vector3(0.0f, Field->Get_YSpacing(), 0.0f),
					Vector3(0.0f, 0.0f, Field->Get_VScale()), Vector3((*patch)->X, (*patch)->Y, (*patch)->Z));
				DX8Wrapper::Set_Transform(D3DTS_WORLD, tm);
				Set_Strip_Buffer(level);
				DX8Wrapper::Set_Vertex_Buffer((*patch)->VertexBuffer);
				DX8Wrapper::Draw_Strip(0, (unsigned short)StripCounts[level], 0, (unsigned short)(*patch)->VertexCount);
				if (level != 0 && (*patch)->Quarters != 0) {
					Set_Edge_Buffer(level);
					(*patch)->Draw();
				}
			}
			if (HasSea) {
				Add_Sea_Quad(sea_quads, (*patch)->X, (*patch)->Y, extent_x, extent_y, SeaLevel);
			}
		}
	}

	if (HasSea) {
		// ten rings of sea quads around the field, and the corners
		float half = (float)(Field->Get_Size() >> 1) * Field->Get_Spacing();
		int ring, c;
		for (ring = 0; ring < 10; ring++) {
			for (c = 0; c < GridWidth; c++) {
				Add_Sea_Quad(sea_quads, c * extent_x - half, ring * extent_y + half, extent_x, extent_y, SeaLevel);
				Add_Sea_Quad(sea_quads, ring * extent_x + half, c * extent_y - half, extent_x, extent_y, SeaLevel);
				Add_Sea_Quad(sea_quads, c * extent_x - half, -half - (ring + 1) * extent_y, extent_x, extent_y, SeaLevel);
				Add_Sea_Quad(sea_quads, -half - (ring + 1) * extent_x, c * extent_y - half, extent_x, extent_y, SeaLevel);
			}
			for (c = 0; c < 10; c++) {
				Add_Sea_Quad(sea_quads, (c - 10) * extent_x - half, ring * extent_y + half, extent_x, extent_y, SeaLevel);
				Add_Sea_Quad(sea_quads, ring * extent_x + half, c * extent_y + half, extent_x, extent_y, SeaLevel);
				Add_Sea_Quad(sea_quads, c * extent_x + half, -half - (ring + 1) * extent_y, extent_x, extent_y, SeaLevel);
				Add_Sea_Quad(sea_quads, -half - (ring + 1) * extent_x, -half - (c + 1) * extent_y, extent_x, extent_y, SeaLevel);
			}
		}
		int count = sea_quads.Count();
		int triangles = count / 2;
		DynamicIBAccessClass ib(BUFFER_TYPE_DYNAMIC_DX8, (unsigned short)(triangles * 3));
		{
			DynamicIBAccessClass::WriteLockClass lock(&ib);
			unsigned short *index = lock.Get_Index_Array();
			for (i = 0; i < count / 4; i++) {
				unsigned short base = (unsigned short)(i * 4);
				*index++ = base;
				*index++ = base + 1;
				*index++ = base + 2;
				*index++ = base;
				*index++ = base + 2;
				*index++ = base + 3;
			}
		}
		DX8Wrapper::Set_Index_Buffer(ib, 0);
		DX8Wrapper::Set_World_Identity();
		for (int sea = 0; sea < SeaCount; sea++) {
			DynamicVBAccessClass vb(BUFFER_TYPE_DYNAMIC_DX8, dynamic_fvf_type, (unsigned short)count);
			{
				DynamicVBAccessClass::WriteLockClass lock(&vb);
				VertexFormatXYZNDUV2 *vertex = lock.Get_Formatted_Vertex_Array();
				float scale = 1.0f / (float)Field->Get_Size();
				for (i = 0; i < count; i++, vertex++) {
					const Vector3 &pos = sea_quads[i];
					vertex->x = pos.X;
					vertex->y = pos.Y;
					vertex->z = pos.Z;
					vertex->nx = 0.0f;
					vertex->ny = 0.0f;
					vertex->nz = 1.0f;
					float u, v;
					Field->World_To_Grid(Vector2(pos.X, pos.Y), &u, &v);
					u *= scale;
					v *= scale;
					vertex->u1 = u * SeaDetailScales[sea][0];
					vertex->v1 = v * SeaDetailScales[sea][0];
					vertex->u2 = SeaDetailScales[sea][1] * u;
					vertex->v2 = SeaDetailScales[sea][1] * v;
				}
			}
			DX8Wrapper::Set_Vertex_Buffer(vb);
			DX8Wrapper::Set_Texture(0, SeaTextures[sea][0]);
			DX8Wrapper::Set_Texture(1, SeaTextures[sea][1]);
			DX8Wrapper::Set_Shader(SeaShaders[sea]);
			DX8Wrapper::Set_Material(SeaMaterials[sea]);
			DX8Wrapper::Draw_Triangles(0, (unsigned short)triangles, 0, (unsigned short)count);
		}
	}

	// the projectors on the land (+0x1218): their boxes' triangles drawn with their material passes
	static std::vector<Vector3> projected;
	DX8Wrapper::Set_World_Identity();
	for (std::vector< RefCountPtr<LandUnknownClass> >::iterator it = m_Unknown1218.begin(); it != m_Unknown1218.end(); it++) {
		LandUnknownClass *projector = (*it).Peek();
		projected.erase(projected.begin(), projected.end());
		Field->Get_Triangles(projector->Get_Box(), projected);
		projector->Peek_Material_Pass()->Install_Materials();
		Unknown_477E20(projected);
	}

	// the patch records' extra buffers (+0x00 / +0x04 and +0x08 / +0x0C), up to level 2
	if (m_Unknown192) {
		if (m_Unknown190) {
			DX8Wrapper::Set_Texture(0, (TextureClass *)m_Unknown178.Peek());
			DX8Wrapper::Set_Texture(1, NULL);
			DX8Wrapper::Set_Shader(m_Unknown17C);
			DX8Wrapper::Set_Material((VertexMaterialClass *)m_Unknown180.Peek());
			for (i = 0; i < PatchCount; i++) {
				LandPatchRecordClass &record = PatchRecords[i];
				if (record.Visible && record.m_Unknown10 > 0 && (unsigned int)record.Level < 3) {
					DX8Wrapper::Set_Vertex_Buffer((VertexBufferClass *)record.m_Unknown00.Peek());
					DX8Wrapper::Set_Index_Buffer((IndexBufferClass *)record.m_Unknown04.Peek(), 0);
					DX8Wrapper::Draw_Triangles(0, (unsigned short)(record.m_Unknown14 / 3), 0, (unsigned short)record.m_Unknown10);
				}
			}
		}
		if (m_Unknown191) {
			DX8Wrapper::Set_Texture(0, (TextureClass *)m_Unknown178.Peek());
			DX8Wrapper::Set_Texture(1, NULL);
			DX8Wrapper::Set_Shader(m_Unknown184);
			DX8Wrapper::Set_Material((VertexMaterialClass *)m_Unknown188.Peek());
			DX8Wrapper::Set_DX8_Render_State(D3DRS_ZBIAS, 1);
			for (i = 0; i < PatchCount; i++) {
				LandPatchRecordClass &record = PatchRecords[i];
				if (record.Visible && record.m_Unknown18 > 0 && (unsigned int)record.Level < 3) {
					DX8Wrapper::Set_Vertex_Buffer((VertexBufferClass *)record.m_Unknown08.Peek());
					DX8Wrapper::Set_Index_Buffer((IndexBufferClass *)record.m_Unknown0C.Peek(), 0);
					DX8Wrapper::Draw_Triangles(0, (unsigned short)(record.m_Unknown1C / 3), 0, (unsigned short)record.m_Unknown18);
				}
			}
			DX8Wrapper::Set_DX8_Render_State(D3DRS_ZBIAS, 0);
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

// NOT EXACT: functional, from the disassembly of 0x46E120
void LandClass::Unknown_46E120(std::vector< RefCountPtr<RenderObjClass> > &casters, unsigned char *mask)
{
	if (Field->Is_Wrapping()) {
		return;
	}
	int size = Field->Get_Size();
	for (int k = 0; k < (int)casters.size(); k++) {
		RenderObjClass *caster = casters[k].Peek();
		const AABoxClass &box = caster->Get_Bounding_Box();
		float top = 0.0f;
		Vector3 corner = box.Center - box.Extent;
		if (corner.Z >= top) {
			top = corner.Z;
		}
		float x0, y0;
		Field->World_To_Grid(Vector2(corner.X, corner.Y), &x0, &y0);
		corner = box.Center + box.Extent;
		if (top <= corner.Z) {
			top = corner.Z;
		}
		float x1, y1;
		Field->World_To_Grid(Vector2(corner.X, corner.Y), &x1, &y1);
		if (x1 < x0 || y1 < y0) {
			continue;
		}
		int ix0 = (int)floor(x0);
		int ix1 = (int)ceil(x1);
		int iy0 = (int)floor(y0);
		int iy1 = (int)ceil(y1);
		if (ix0 < 0) {
			ix0 = 0;
		}
		if (ix1 >= size) {
			ix1 = size - 1;
		}
		if (iy0 < 0) {
			iy0 = 0;
		}
		if (iy1 >= size) {
			iy1 = size - 1;
		}
		for (int y = iy0; y <= iy1; y++) {
			unsigned char *row = mask + y * size;
			for (int x = ix0; x <= ix1; x++) {
				Vector3 vertex(0.0f, 0.0f, 0.0f);
				Field->Get_Vertex(x, y, &vertex);
				if (vertex.Z < top) {
					// a ray straight down from the casters' top onto the vertex: a hit raises the vertex to it
					LineSegClass ray(Vector3(vertex.X, vertex.Y, top), vertex);
					CastResultStruct result;
					result.ComputeContactPoint = true;
					RayCollisionTestClass raytest(ray, &result);
					caster->Cast_Ray(raytest);
					if (result.Fraction < 1.0f) {
						Field->Set_Height_2(result.ContactPoint.Z > vertex.Z ? result.ContactPoint.Z : vertex.Z, x, y);
						row[x] = 1;
					}
				}
			}
		}
	}
}

// NAME 1: unknown; the 256 unit normals Unknown_46F120 copies into Normals (0xAE8BBC .. 0xAE97BC, .rdata, 6-digit literals):
// index = azimuth * 16 + elevation, (cos a sqrt(1 - z^2), sin a sqrt(1 - z^2), z) with a = azimuth x 22.5 degrees, z = elevation / 16
static const float LandNormalTable[256][3] = {
	// azimuth 0 x 22.5 degrees, Z = 0 .. 15/16
	{ 1.0f, 0.0f, 0.0f }, { 0.998045f, 0.0f, 0.0625f }, { 0.992157f, 0.0f, 0.125f }, { 0.982265f, 0.0f, 0.1875f },
	{ 0.968246f, 0.0f, 0.25f }, { 0.949918f, 0.0f, 0.3125f }, { 0.927025f, 0.0f, 0.375f }, { 0.899218f, 0.0f, 0.4375f },
	{ 0.866025f, 0.0f, 0.5f }, { 0.826797f, 0.0f, 0.5625f }, { 0.780625f, 0.0f, 0.625f }, { 0.726184f, 0.0f, 0.6875f },
	{ 0.661438f, 0.0f, 0.75f }, { 0.582961f, 0.0f, 0.8125f }, { 0.484123f, 0.0f, 0.875f }, { 0.347985f, 0.0f, 0.9375f },
	// azimuth 1 x 22.5 degrees, Z = 0 .. 15/16
	{ 0.92388f, 0.382683f, 0.0f }, { 0.922073f, 0.381935f, 0.0625f }, { 0.916633f, 0.379682f, 0.125f }, { 0.907494f, 0.375896f, 0.1875f },
	{ 0.894543f, 0.370532f, 0.25f }, { 0.87761f, 0.363518f, 0.3125f }, { 0.856459f, 0.354757f, 0.375f }, { 0.830769f, 0.344116f, 0.4375f },
	{ 0.800103f, 0.331414f, 0.5f }, { 0.763861f, 0.316402f, 0.5625f }, { 0.721203f, 0.298732f, 0.625f }, { 0.670907f, 0.277899f, 0.6875f },
	{ 0.611089f, 0.253121f, 0.75f }, { 0.538586f, 0.22309f, 0.8125f }, { 0.447271f, 0.185266f, 0.875f }, { 0.321496f, 0.133168f, 0.9375f },
	// azimuth 2 x 22.5 degrees, Z = 0 .. 15/16
	{ 0.707107f, 0.707107f, 0.0f }, { 0.705724f, 0.705724f, 0.0625f }, { 0.701561f, 0.701561f, 0.125f }, { 0.694566f, 0.694566f, 0.1875f },
	{ 0.684653f, 0.684653f, 0.25f }, { 0.671693f, 0.671693f, 0.3125f }, { 0.655506f, 0.655506f, 0.375f }, { 0.635843f, 0.635843f, 0.4375f },
	{ 0.612372f, 0.612372f, 0.5f }, { 0.584634f, 0.584634f, 0.5625f }, { 0.551985f, 0.551985f, 0.625f }, { 0.51349f, 0.51349f, 0.6875f },
	{ 0.467707f, 0.467707f, 0.75f }, { 0.412216f, 0.412216f, 0.8125f }, { 0.342327f, 0.342327f, 0.875f }, { 0.246063f, 0.246063f, 0.9375f },
	// azimuth 3 x 22.5 degrees, Z = 0 .. 15/16
	{ 0.382683f, 0.92388f, 0.0f }, { 0.381935f, 0.922073f, 0.0625f }, { 0.379682f, 0.916633f, 0.125f }, { 0.375896f, 0.907494f, 0.1875f },
	{ 0.370532f, 0.894543f, 0.25f }, { 0.363518f, 0.87761f, 0.3125f }, { 0.354757f, 0.856459f, 0.375f }, { 0.344116f, 0.830769f, 0.4375f },
	{ 0.331414f, 0.800103f, 0.5f }, { 0.316402f, 0.763861f, 0.5625f }, { 0.298732f, 0.721203f, 0.625f }, { 0.277899f, 0.670907f, 0.6875f },
	{ 0.253121f, 0.611089f, 0.75f }, { 0.22309f, 0.538586f, 0.8125f }, { 0.185266f, 0.447271f, 0.875f }, { 0.133168f, 0.321496f, 0.9375f },
	// azimuth 4 x 22.5 degrees, Z = 0 .. 15/16
	{ 0.0f, 1.0f, 0.0f }, { 0.0f, 0.998045f, 0.0625f }, { 0.0f, 0.992157f, 0.125f }, { 0.0f, 0.982265f, 0.1875f },
	{ 0.0f, 0.968246f, 0.25f }, { 0.0f, 0.949918f, 0.3125f }, { 0.0f, 0.927025f, 0.375f }, { 0.0f, 0.899218f, 0.4375f },
	{ 0.0f, 0.866025f, 0.5f }, { 0.0f, 0.826797f, 0.5625f }, { 0.0f, 0.780625f, 0.625f }, { 0.0f, 0.726184f, 0.6875f },
	{ 0.0f, 0.661438f, 0.75f }, { 0.0f, 0.582961f, 0.8125f }, { 0.0f, 0.484123f, 0.875f }, { 0.0f, 0.347985f, 0.9375f },
	// azimuth 5 x 22.5 degrees, Z = 0 .. 15/16
	{ -0.382683f, 0.92388f, 0.0f }, { -0.381935f, 0.922073f, 0.0625f }, { -0.379682f, 0.916633f, 0.125f }, { -0.375896f, 0.907494f, 0.1875f },
	{ -0.370532f, 0.894543f, 0.25f }, { -0.363518f, 0.87761f, 0.3125f }, { -0.354757f, 0.856459f, 0.375f }, { -0.344116f, 0.830769f, 0.4375f },
	{ -0.331414f, 0.800103f, 0.5f }, { -0.316402f, 0.763861f, 0.5625f }, { -0.298732f, 0.721203f, 0.625f }, { -0.277899f, 0.670907f, 0.6875f },
	{ -0.253121f, 0.611089f, 0.75f }, { -0.22309f, 0.538586f, 0.8125f }, { -0.185266f, 0.447271f, 0.875f }, { -0.133168f, 0.321496f, 0.9375f },
	// azimuth 6 x 22.5 degrees, Z = 0 .. 15/16
	{ -0.707107f, 0.707107f, 0.0f }, { -0.705724f, 0.705724f, 0.0625f }, { -0.701561f, 0.701561f, 0.125f }, { -0.694566f, 0.694566f, 0.1875f },
	{ -0.684653f, 0.684653f, 0.25f }, { -0.671693f, 0.671693f, 0.3125f }, { -0.655506f, 0.655505f, 0.375f }, { -0.635843f, 0.635843f, 0.4375f },
	{ -0.612372f, 0.612372f, 0.5f }, { -0.584634f, 0.584634f, 0.5625f }, { -0.551985f, 0.551985f, 0.625f }, { -0.51349f, 0.51349f, 0.6875f },
	{ -0.467707f, 0.467707f, 0.75f }, { -0.412216f, 0.412216f, 0.8125f }, { -0.342327f, 0.342327f, 0.875f }, { -0.246063f, 0.246063f, 0.9375f },
	// azimuth 7 x 22.5 degrees, Z = 0 .. 15/16
	{ -0.92388f, 0.382683f, 0.0f }, { -0.922073f, 0.381935f, 0.0625f }, { -0.916633f, 0.379682f, 0.125f }, { -0.907494f, 0.375896f, 0.1875f },
	{ -0.894543f, 0.370532f, 0.25f }, { -0.87761f, 0.363518f, 0.3125f }, { -0.856459f, 0.354757f, 0.375f }, { -0.83077f, 0.344116f, 0.4375f },
	{ -0.800103f, 0.331414f, 0.5f }, { -0.763861f, 0.316402f, 0.5625f }, { -0.721203f, 0.298732f, 0.625f }, { -0.670907f, 0.277899f, 0.6875f },
	{ -0.611089f, 0.253121f, 0.75f }, { -0.538586f, 0.22309f, 0.8125f }, { -0.447271f, 0.185266f, 0.875f }, { -0.321496f, 0.133168f, 0.9375f },
	// azimuth 8 x 22.5 degrees, Z = 0 .. 15/16
	{ -1.0f, 0.0f, 0.0f }, { -0.998045f, 0.0f, 0.0625f }, { -0.992157f, 0.0f, 0.125f }, { -0.982265f, 0.0f, 0.1875f },
	{ -0.968246f, 0.0f, 0.25f }, { -0.949918f, 0.0f, 0.3125f }, { -0.927025f, 0.0f, 0.375f }, { -0.899218f, 0.0f, 0.4375f },
	{ -0.866025f, 0.0f, 0.5f }, { -0.826797f, 0.0f, 0.5625f }, { -0.780625f, 0.0f, 0.625f }, { -0.726184f, 0.0f, 0.6875f },
	{ -0.661438f, 0.0f, 0.75f }, { -0.582961f, 0.0f, 0.8125f }, { -0.484123f, 0.0f, 0.875f }, { -0.347985f, 0.0f, 0.9375f },
	// azimuth 9 x 22.5 degrees, Z = 0 .. 15/16
	{ -0.92388f, -0.382684f, 0.0f }, { -0.922073f, -0.381935f, 0.0625f }, { -0.916633f, -0.379682f, 0.125f }, { -0.907494f, -0.375896f, 0.1875f },
	{ -0.894542f, -0.370532f, 0.25f }, { -0.87761f, -0.363518f, 0.3125f }, { -0.856459f, -0.354757f, 0.375f }, { -0.830769f, -0.344116f, 0.4375f },
	{ -0.800103f, -0.331414f, 0.5f }, { -0.763861f, -0.316402f, 0.5625f }, { -0.721203f, -0.298732f, 0.625f }, { -0.670907f, -0.277899f, 0.6875f },
	{ -0.611089f, -0.253121f, 0.75f }, { -0.538586f, -0.22309f, 0.8125f }, { -0.447271f, -0.185266f, 0.875f }, { -0.321496f, -0.133168f, 0.9375f },
	// azimuth 10 x 22.5 degrees, Z = 0 .. 15/16
	{ -0.707107f, -0.707107f, 0.0f }, { -0.705724f, -0.705724f, 0.0625f }, { -0.701561f, -0.701561f, 0.125f }, { -0.694566f, -0.694566f, 0.1875f },
	{ -0.684653f, -0.684653f, 0.25f }, { -0.671693f, -0.671693f, 0.3125f }, { -0.655505f, -0.655506f, 0.375f }, { -0.635843f, -0.635844f, 0.4375f },
	{ -0.612372f, -0.612373f, 0.5f }, { -0.584634f, -0.584634f, 0.5625f }, { -0.551985f, -0.551985f, 0.625f }, { -0.51349f, -0.51349f, 0.6875f },
	{ -0.467707f, -0.467707f, 0.75f }, { -0.412216f, -0.412216f, 0.8125f }, { -0.342327f, -0.342327f, 0.875f }, { -0.246063f, -0.246063f, 0.9375f },
	// azimuth 11 x 22.5 degrees, Z = 0 .. 15/16
	{ -0.382683f, -0.92388f, 0.0f }, { -0.381935f, -0.922073f, 0.0625f }, { -0.379682f, -0.916633f, 0.125f }, { -0.375896f, -0.907494f, 0.1875f },
	{ -0.370532f, -0.894543f, 0.25f }, { -0.363518f, -0.87761f, 0.3125f }, { -0.354757f, -0.856459f, 0.375f }, { -0.344116f, -0.83077f, 0.4375f },
	{ -0.331413f, -0.800103f, 0.5f }, { -0.316402f, -0.763861f, 0.5625f }, { -0.298732f, -0.721203f, 0.625f }, { -0.277899f, -0.670907f, 0.6875f },
	{ -0.253121f, -0.611089f, 0.75f }, { -0.22309f, -0.538586f, 0.8125f }, { -0.185266f, -0.447271f, 0.875f }, { -0.133168f, -0.321496f, 0.9375f },
	// azimuth 12 x 22.5 degrees, Z = 0 .. 15/16
	{ 0.0f, -1.0f, 0.0f }, { 0.0f, -0.998045f, 0.0625f }, { 0.0f, -0.992157f, 0.125f }, { 0.0f, -0.982265f, 0.1875f },
	{ 0.0f, -0.968246f, 0.25f }, { 0.0f, -0.949918f, 0.3125f }, { 0.0f, -0.927025f, 0.375f }, { 0.0f, -0.899218f, 0.4375f },
	{ 0.0f, -0.866025f, 0.5f }, { 0.0f, -0.826797f, 0.5625f }, { 0.0f, -0.780625f, 0.625f }, { 0.0f, -0.726184f, 0.6875f },
	{ 0.0f, -0.661438f, 0.75f }, { 0.0f, -0.582961f, 0.8125f }, { 0.0f, -0.484123f, 0.875f }, { 0.0f, -0.347985f, 0.9375f },
	// azimuth 13 x 22.5 degrees, Z = 0 .. 15/16
	{ 0.382684f, -0.92388f, 0.0f }, { 0.381935f, -0.922073f, 0.0625f }, { 0.379682f, -0.916633f, 0.125f }, { 0.375897f, -0.907494f, 0.1875f },
	{ 0.370532f, -0.894542f, 0.25f }, { 0.363518f, -0.87761f, 0.3125f }, { 0.354757f, -0.856459f, 0.375f }, { 0.344116f, -0.830769f, 0.4375f },
	{ 0.331414f, -0.800103f, 0.5f }, { 0.316402f, -0.763861f, 0.5625f }, { 0.298732f, -0.721203f, 0.625f }, { 0.277899f, -0.670907f, 0.6875f },
	{ 0.253121f, -0.611089f, 0.75f }, { 0.22309f, -0.538586f, 0.8125f }, { 0.185266f, -0.447271f, 0.875f }, { 0.133168f, -0.321496f, 0.9375f },
	// azimuth 14 x 22.5 degrees, Z = 0 .. 15/16
	{ 0.707107f, -0.707107f, 0.0f }, { 0.705724f, -0.705724f, 0.0625f }, { 0.701561f, -0.701561f, 0.125f }, { 0.694566f, -0.694566f, 0.1875f },
	{ 0.684653f, -0.684653f, 0.25f }, { 0.671693f, -0.671693f, 0.3125f }, { 0.655506f, -0.655505f, 0.375f }, { 0.635844f, -0.635843f, 0.4375f },
	{ 0.612373f, -0.612372f, 0.5f }, { 0.584634f, -0.584634f, 0.5625f }, { 0.551985f, -0.551985f, 0.625f }, { 0.51349f, -0.51349f, 0.6875f },
	{ 0.467707f, -0.467707f, 0.75f }, { 0.412216f, -0.412216f, 0.8125f }, { 0.342327f, -0.342327f, 0.875f }, { 0.246063f, -0.246063f, 0.9375f },
	// azimuth 15 x 22.5 degrees, Z = 0 .. 15/16
	{ 0.92388f, -0.382683f, 0.0f }, { 0.922073f, -0.381935f, 0.0625f }, { 0.916633f, -0.379682f, 0.125f }, { 0.907494f, -0.375896f, 0.1875f },
	{ 0.894543f, -0.370531f, 0.25f }, { 0.87761f, -0.363518f, 0.3125f }, { 0.856459f, -0.354757f, 0.375f }, { 0.83077f, -0.344116f, 0.4375f },
	{ 0.800103f, -0.331413f, 0.5f }, { 0.763861f, -0.316402f, 0.5625f }, { 0.721203f, -0.298732f, 0.625f }, { 0.670907f, -0.277899f, 0.6875f },
	{ 0.611089f, -0.253121f, 0.75f }, { 0.538586f, -0.22309f, 0.8125f }, { 0.447271f, -0.185266f, 0.875f }, { 0.321496f, -0.133168f, 0.9375f }
};

// NAME 1: unknown; the index of the table normal nearest a vertex normal (largest dot product, first wins ties).
// The client inlines it eight times; it adds the dot product's terms Z first: n.Z * N.Z + n.X * N.X + n.Y * N.Y
static inline unsigned char Land_Nearest_Normal(const Vector3 *normals, const Vector3 &n)
{
	unsigned char best_index = 0;
	float best = -1.0f;
	for (int k = 0; k < 256; k++) {
		float dot = n.Z * normals[k].Z + n.X * normals[k].X + n.Y * normals[k].Y;
		if (dot > best) {
			best = dot;
			best_index = (unsigned char)k;
		}
	}
	return best_index;
}

// NOT EXACT: functional, from the disassembly and pseudocode of 0x46F120
void LandClass::Unknown_46F120(void)
{
	int i;
	for (i = 0; i < 256; i++) {
		Normals[i].Set(LandNormalTable[i][0], LandNormalTable[i][1], LandNormalTable[i][2]);
	}

	// Per height-field vertex: the normal of the triangles around it (vectors to its neighbours, cross products of neighbour pairs
	// around it, the client's -1 / +1 neighbours wrap through Get_Height_2's mask), matched against Normals into m_Unknown1E4.
	// Only even grid vertices (0 .. Size) are visited: the four corners get all eight triangles, the edge vertices one normal per
	// half (left / right, top / bottom) and the interior ones one per quadrant, written to the 1 or 2 x 2 bytes beside them.
	float spacing = Field->Get_Spacing();
	int size = Field->Get_Size();
	float neg = -spacing;
	int index = 0;
	int x;
	int y;
	Vector3 n;

	// the four corners (x, y in { 0, Size }): written at (0, 0), (Size - 1, 0), (0, Size - 1), (Size - 1, Size - 1)
	for (y = 0; y < size + 1; y += size) {
		for (x = 0; x < size + 1; x += size) {
			float h00 = Field->Get_Height_2(x - 1, y - 1);
			float h10 = Field->Get_Height_2(x, y - 1);
			float h20 = Field->Get_Height_2(x + 1, y - 1);
			float h01 = Field->Get_Height_2(x - 1, y);
			float h = Field->Get_Height_2(x, y);
			float h21 = Field->Get_Height_2(x + 1, y);
			float h02 = Field->Get_Height_2(x - 1, y + 1);
			float h12 = Field->Get_Height_2(x, y + 1);
			float h22 = Field->Get_Height_2(x + 1, y + 1);
			Vector3 v00(neg, neg, h00 - h);
			Vector3 v10(0.0f, neg, h10 - h);
			Vector3 v20(spacing, neg, h20 - h);
			Vector3 v01(neg, 0.0f, h01 - h);
			Vector3 v21(spacing, 0.0f, h21 - h);
			Vector3 v02(neg, spacing, h02 - h);
			Vector3 v12(0.0f, spacing, h12 - h);
			Vector3 v22(spacing, spacing, h22 - h);
			// the client's sum order: the first two terms swapped against the ring
			n = Vector3::Cross_Product(v10, v20) + Vector3::Cross_Product(v00, v10) + Vector3::Cross_Product(v20, v21)
				+ Vector3::Cross_Product(v21, v22) + Vector3::Cross_Product(v22, v12) + Vector3::Cross_Product(v12, v02)
				+ Vector3::Cross_Product(v02, v01) + Vector3::Cross_Product(v01, v00);
			n.Normalize();
			m_Unknown1E4[index] = Land_Nearest_Normal(Normals, n);
			index += size - 1;
		}
		index = (size - 1) * size;
	}

	// the top and bottom rows (y in { 0, Size }), even x in 2 .. Size - 2: the left half at x - 1, the right half at x
	index = 1;
	for (y = 0; y < size + 1; y += size) {
		for (x = 2; x <= size - 2; x += 2) {
			float h00 = Field->Get_Height_2(x - 1, y - 1);
			float h20 = Field->Get_Height_2(x + 1, y - 1);
			float h01 = Field->Get_Height_2(x - 1, y);
			float h = Field->Get_Height_2(x, y);
			float h21 = Field->Get_Height_2(x + 1, y);
			float h02 = Field->Get_Height_2(x - 1, y + 1);
			float h22 = Field->Get_Height_2(x + 1, y + 1);
			Vector3 v00(neg, neg, h00 - h);
			Vector3 v20(spacing, neg, h20 - h);
			Vector3 v01(neg, 0.0f, h01 - h);
			Vector3 v21(spacing, 0.0f, h21 - h);
			Vector3 v02(neg, spacing, h02 - h);
			Vector3 v22(spacing, spacing, h22 - h);
			n = Vector3::Cross_Product(v01, v00) + Vector3::Cross_Product(v02, v01);
			n.Normalize();
			m_Unknown1E4[index] = Land_Nearest_Normal(Normals, n);
			n = Vector3::Cross_Product(v20, v21) + Vector3::Cross_Product(v21, v22);
			n.Normalize();
			m_Unknown1E4[index + 1] = Land_Nearest_Normal(Normals, n);
			index += 2;
		}
		index = (size - 1) * size + 1;
	}

	// even rows y in 2 .. Size - 2
	index = size;
	for (y = 2; y <= size - 2; y += 2) {
		// the left and right columns (x in { 0, Size }): the top half at row y - 1, the bottom half at row y
		for (x = 0; x < size + 1; x += size) {
			float h00 = Field->Get_Height_2(x - 1, y - 1);
			float h10 = Field->Get_Height_2(x, y - 1);
			float h20 = Field->Get_Height_2(x + 1, y - 1);
			float h = Field->Get_Height_2(x, y);
			float h02 = Field->Get_Height_2(x - 1, y + 1);
			float h12 = Field->Get_Height_2(x, y + 1);
			float h22 = Field->Get_Height_2(x + 1, y + 1);
			Vector3 v00(neg, neg, h00 - h);
			Vector3 v10(0.0f, neg, h10 - h);
			Vector3 v20(spacing, neg, h20 - h);
			Vector3 v02(neg, spacing, h02 - h);
			Vector3 v12(0.0f, spacing, h12 - h);
			Vector3 v22(spacing, spacing, h22 - h);
			n = Vector3::Cross_Product(v10, v20) + Vector3::Cross_Product(v00, v10);
			n.Normalize();
			m_Unknown1E4[index] = Land_Nearest_Normal(Normals, n);
			n = Vector3::Cross_Product(v12, v02) + Vector3::Cross_Product(v22, v12);
			n.Normalize();
			m_Unknown1E4[index + size] = Land_Nearest_Normal(Normals, n);
			index += size - 1;
		}
		index += 3 - size * 2;
		// the interior even x in 2 .. Size - 2: one quadrant each at (x - 1, y - 1), (x, y - 1), (x - 1, y), (x, y)
		for (x = 2; x <= size - 2; x += 2) {
			float h00 = Field->Get_Height_2(x - 1, y - 1);
			float h10 = Field->Get_Height_2(x, y - 1);
			float h20 = Field->Get_Height_2(x + 1, y - 1);
			float h01 = Field->Get_Height_2(x - 1, y);
			float h = Field->Get_Height_2(x, y);
			float h21 = Field->Get_Height_2(x + 1, y);
			float h02 = Field->Get_Height_2(x - 1, y + 1);
			float h12 = Field->Get_Height_2(x, y + 1);
			float h22 = Field->Get_Height_2(x + 1, y + 1);
			Vector3 v00(neg, neg, h00 - h);
			Vector3 v10(0.0f, neg, h10 - h);
			Vector3 v20(spacing, neg, h20 - h);
			Vector3 v01(neg, 0.0f, h01 - h);
			Vector3 v21(spacing, 0.0f, h21 - h);
			Vector3 v02(neg, spacing, h02 - h);
			Vector3 v12(0.0f, spacing, h12 - h);
			Vector3 v22(spacing, spacing, h22 - h);
			n = Vector3::Cross_Product(v01, v00) + Vector3::Cross_Product(v00, v10);
			n.Normalize();
			m_Unknown1E4[index] = Land_Nearest_Normal(Normals, n);
			n = Vector3::Cross_Product(v20, v21) + Vector3::Cross_Product(v10, v20);
			n.Normalize();
			m_Unknown1E4[index + 1] = Land_Nearest_Normal(Normals, n);
			n = Vector3::Cross_Product(v02, v01) + Vector3::Cross_Product(v12, v02);
			n.Normalize();
			m_Unknown1E4[index + size] = Land_Nearest_Normal(Normals, n);
			n = Vector3::Cross_Product(v22, v12) + Vector3::Cross_Product(v21, v22);
			n.Normalize();
			m_Unknown1E4[index + size + 1] = Land_Nearest_Normal(Normals, n);
			index += 2;
		}
		index += size + 1;
	}
}

// NOT EXACT: functional, from the disassembly and pseudocode of 0x470F80
void LandClass::Unknown_470F80(unsigned char *a, unsigned char *b, unsigned char *c)
{
	// the client: three copies of 65535 / (pi / 2) = 41720.875 (float 0xAE9AD0 in the four corners'
	// blocks, float 0xAE9ACC in the edges', double 0xAE9AC0 inside): likely one inline helper or macro per kind
	static const float HORIZON_SCALE_4 = 41720.875f;
	static const float HORIZON_SCALE_2 = 41720.875f;
	static const double HORIZON_SCALE_1 = 41720.875;
	unsigned int size = Field->Get_Size();
	unsigned int mask = Field->Get_Mask();
	unsigned int last = size - 1;
	unsigned int last_row = mask & last;			// the last row (column), wrapped
	unsigned int wrap = mask & size;				// the row (column) after the last, wrapped: 0
	unsigned short *hz = (unsigned short *)c;	// per cell: the horizons towards -x and +x (log-coded tangents)
	unsigned short avg[2];
	unsigned char h[8];
	unsigned int bits;
	unsigned char *rgb;
	unsigned int n = 0;
	unsigned int x;
	unsigned int y;
	unsigned int k0, k1, k2, k3;

	// the first row: its vertices average with the last row's (the field wraps); the corners with all four corners
	k0 = (0 << Field->Get_Log2());
	k1 = k0 + mask;
	k2 = (mask << Field->Get_Log2());
	k3 = k2 + mask;
	avg[0] = (unsigned short)((hz[k0 * 2 + 0] + hz[k1 * 2 + 0] + hz[k2 * 2 + 0] + hz[k3 * 2 + 0]) >> 2);
	avg[1] = (unsigned short)((hz[k0 * 2 + 1] + hz[k1 * 2 + 1] + hz[k2 * 2 + 1] + hz[k3 * 2 + 1]) >> 2);
	bits = (avg[0] + 0x77000) << 11 | 0x7FF;
	m_Unknown1E8[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_4);
	bits = (avg[1] + 0x77000) << 11 | 0x7FF;
	m_Unknown1EC[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_4);
	h[0] = (unsigned char)(avg[0] >> 8);
	h[1] = (unsigned char)(avg[1] >> 8);
	h[2] = (unsigned char)((a[k0 * 3 + 1] + a[k1 * 3 + 1] + a[k2 * 3 + 1] + a[k3 * 3 + 1]) >> 2);
	h[3] = (unsigned char)((b[k0 * 3 + 1] + b[k1 * 3 + 1] + b[k2 * 3 + 1] + b[k3 * 3 + 1]) >> 2);
	h[4] = (unsigned char)((a[k0 * 3] + a[k1 * 3] + a[k2 * 3] + a[k3 * 3]) >> 2);
	h[5] = (unsigned char)((b[k0 * 3 + 2] + b[k1 * 3 + 2] + b[k2 * 3 + 2] + b[k3 * 3 + 2]) >> 2);
	h[6] = (unsigned char)((b[k0 * 3] + b[k1 * 3] + b[k2 * 3] + b[k3 * 3]) >> 2);
	h[7] = (unsigned char)((a[k0 * 3 + 2] + a[k1 * 3 + 2] + a[k2 * 3 + 2] + a[k3 * 3 + 2]) >> 2);
	rgb = &Colors[(0 << Field->Get_Log2()) * 3];
	{
		Vector3 color((float)rgb[2] * (1.0f / 255.0f), (float)rgb[1] * (1.0f / 255.0f), (float)rgb[0] * (1.0f / 255.0f));
		Unknown_472F80(&m_Unknown1F0[n], &m_Unknown1F4[n], &m_Unknown1F8[n], h, m_Unknown1E4[n], color);
	}
	n++;
	for (x = 1; x < last; x++) {
		k0 = (0 << Field->Get_Log2()) + x;
		k1 = (mask << Field->Get_Log2()) + x;
		avg[0] = (unsigned short)((hz[k0 * 2 + 0] + hz[k1 * 2 + 0]) >> 1);
		avg[1] = (unsigned short)((hz[k0 * 2 + 1] + hz[k1 * 2 + 1]) >> 1);
		bits = (avg[0] + 0x77000) << 11 | 0x7FF;
		m_Unknown1E8[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_2);
		bits = (avg[1] + 0x77000) << 11 | 0x7FF;
		m_Unknown1EC[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_2);
		h[0] = (unsigned char)(avg[0] >> 8);
		h[1] = (unsigned char)(avg[1] >> 8);
		h[2] = (unsigned char)((a[k0 * 3 + 1] + a[k1 * 3 + 1]) >> 1);
		h[3] = (unsigned char)((b[k0 * 3 + 1] + b[k1 * 3 + 1]) >> 1);
		h[4] = (unsigned char)((a[k0 * 3] + a[k1 * 3]) >> 1);
		h[5] = (unsigned char)((b[k0 * 3 + 2] + b[k1 * 3 + 2]) >> 1);
		h[6] = (unsigned char)((b[k0 * 3] + b[k1 * 3]) >> 1);
		h[7] = (unsigned char)((a[k0 * 3 + 2] + a[k1 * 3 + 2]) >> 1);
		rgb = &Colors[((0 << Field->Get_Log2()) + x) * 3];
		{
			Vector3 color((float)rgb[2] * (1.0f / 255.0f), (float)rgb[1] * (1.0f / 255.0f), (float)rgb[0] * (1.0f / 255.0f));
			Unknown_472F80(&m_Unknown1F0[n], &m_Unknown1F4[n], &m_Unknown1F8[n], h, m_Unknown1E4[n], color);
		}
		n++;
	}
	k0 = (0 << Field->Get_Log2()) + wrap;
	k1 = (mask << Field->Get_Log2()) + wrap;
	k2 = (0 << Field->Get_Log2()) + last;
	k3 = (mask << Field->Get_Log2()) + last;
	avg[0] = (unsigned short)((hz[k0 * 2 + 0] + hz[k1 * 2 + 0] + hz[k2 * 2 + 0] + hz[k3 * 2 + 0]) >> 2);
	avg[1] = (unsigned short)((hz[k0 * 2 + 1] + hz[k1 * 2 + 1] + hz[k2 * 2 + 1] + hz[k3 * 2 + 1]) >> 2);
	bits = (avg[0] + 0x77000) << 11 | 0x7FF;
	m_Unknown1E8[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_4);
	bits = (avg[1] + 0x77000) << 11 | 0x7FF;
	m_Unknown1EC[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_4);
	h[0] = (unsigned char)(avg[0] >> 8);
	h[1] = (unsigned char)(avg[1] >> 8);
	h[2] = (unsigned char)((a[k0 * 3 + 1] + a[k1 * 3 + 1] + a[k2 * 3 + 1] + a[k3 * 3 + 1]) >> 2);
	h[3] = (unsigned char)((b[k0 * 3 + 1] + b[k1 * 3 + 1] + b[k2 * 3 + 1] + b[k3 * 3 + 1]) >> 2);
	h[4] = (unsigned char)((a[k0 * 3] + a[k1 * 3] + a[k2 * 3] + a[k3 * 3]) >> 2);
	h[5] = (unsigned char)((b[k0 * 3 + 2] + b[k1 * 3 + 2] + b[k2 * 3 + 2] + b[k3 * 3 + 2]) >> 2);
	h[6] = (unsigned char)((b[k0 * 3] + b[k1 * 3] + b[k2 * 3] + b[k3 * 3]) >> 2);
	h[7] = (unsigned char)((a[k0 * 3 + 2] + a[k1 * 3 + 2] + a[k2 * 3 + 2] + a[k3 * 3 + 2]) >> 2);
	rgb = &Colors[((0 << Field->Get_Log2()) + size) * 3 - 3];
	{
		Vector3 color((float)rgb[2] * (1.0f / 255.0f), (float)rgb[1] * (1.0f / 255.0f), (float)rgb[0] * (1.0f / 255.0f));
		Unknown_472F80(&m_Unknown1F0[n], &m_Unknown1F4[n], &m_Unknown1F8[n], h, m_Unknown1E4[n], color);
	}
	n++;
	// the inner rows: the first and last columns average with each other, the rest is the cell's own
	for (y = 1; y < last; y++) {
		k0 = y << Field->Get_Log2();
		k1 = k0 + mask;
		avg[0] = (unsigned short)((hz[k0 * 2 + 0] + hz[k1 * 2 + 0]) >> 1);
		avg[1] = (unsigned short)((hz[k0 * 2 + 1] + hz[k1 * 2 + 1]) >> 1);
		bits = (avg[0] + 0x77000) << 11 | 0x7FF;
		m_Unknown1E8[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_2);
		bits = (avg[1] + 0x77000) << 11 | 0x7FF;
		m_Unknown1EC[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_2);
		h[0] = (unsigned char)(avg[0] >> 8);
		h[1] = (unsigned char)(avg[1] >> 8);
		h[2] = (unsigned char)((a[k0 * 3 + 1] + a[k1 * 3 + 1]) >> 1);
		h[3] = (unsigned char)((b[k0 * 3 + 1] + b[k1 * 3 + 1]) >> 1);
		h[4] = (unsigned char)((a[k0 * 3] + a[k1 * 3]) >> 1);
		h[5] = (unsigned char)((b[k0 * 3 + 2] + b[k1 * 3 + 2]) >> 1);
		h[6] = (unsigned char)((b[k0 * 3] + b[k1 * 3]) >> 1);
		h[7] = (unsigned char)((a[k0 * 3 + 2] + a[k1 * 3 + 2]) >> 1);
		rgb = &Colors[(y << Field->Get_Log2()) * 3];
		{
			Vector3 color((float)rgb[2] * (1.0f / 255.0f), (float)rgb[1] * (1.0f / 255.0f), (float)rgb[0] * (1.0f / 255.0f));
			Unknown_472F80(&m_Unknown1F0[n], &m_Unknown1F4[n], &m_Unknown1F8[n], h, m_Unknown1E4[n], color);
		}
		n++;
		for (x = 1; x < last; x++) {
			k0 = (y << Field->Get_Log2()) + x;
			avg[0] = hz[k0 * 2 + 0];
			avg[1] = hz[k0 * 2 + 1];
			bits = (avg[0] + 0x77000) << 11 | 0x7FF;
			m_Unknown1E8[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_1);
			bits = (avg[1] + 0x77000) << 11 | 0x7FF;
			m_Unknown1EC[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_1);
			h[0] = (unsigned char)(avg[0] >> 8);
			h[1] = (unsigned char)(avg[1] >> 8);
			h[2] = a[k0 * 3 + 1];
			h[3] = b[k0 * 3 + 1];
			h[4] = a[k0 * 3];
			h[5] = b[k0 * 3 + 2];
			h[6] = b[k0 * 3];
			h[7] = a[k0 * 3 + 2];
			rgb = &Colors[((y << Field->Get_Log2()) + x) * 3];
			{
				Vector3 color((float)rgb[2] * (1.0f / 255.0f), (float)rgb[1] * (1.0f / 255.0f), (float)rgb[0] * (1.0f / 255.0f));
				Unknown_472F80(&m_Unknown1F0[n], &m_Unknown1F4[n], &m_Unknown1F8[n], h, m_Unknown1E4[n], color);
			}
			n++;
		}
		k0 = (y << Field->Get_Log2()) + wrap;
		k1 = (y << Field->Get_Log2()) + last;
		avg[0] = (unsigned short)((hz[k0 * 2 + 0] + hz[k1 * 2 + 0]) >> 1);
		avg[1] = (unsigned short)((hz[k0 * 2 + 1] + hz[k1 * 2 + 1]) >> 1);
		bits = (avg[0] + 0x77000) << 11 | 0x7FF;
		m_Unknown1E8[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_2);
		bits = (avg[1] + 0x77000) << 11 | 0x7FF;
		m_Unknown1EC[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_2);
		h[0] = (unsigned char)(avg[0] >> 8);
		h[1] = (unsigned char)(avg[1] >> 8);
		h[2] = (unsigned char)((a[k0 * 3 + 1] + a[k1 * 3 + 1]) >> 1);
		h[3] = (unsigned char)((b[k0 * 3 + 1] + b[k1 * 3 + 1]) >> 1);
		h[4] = (unsigned char)((a[k0 * 3] + a[k1 * 3]) >> 1);
		h[5] = (unsigned char)((b[k0 * 3 + 2] + b[k1 * 3 + 2]) >> 1);
		h[6] = (unsigned char)((b[k0 * 3] + b[k1 * 3]) >> 1);
		h[7] = (unsigned char)((a[k0 * 3 + 2] + a[k1 * 3 + 2]) >> 1);
		rgb = &Colors[((y << Field->Get_Log2()) + size) * 3 - 3];
		{
			Vector3 color((float)rgb[2] * (1.0f / 255.0f), (float)rgb[1] * (1.0f / 255.0f), (float)rgb[0] * (1.0f / 255.0f));
			Unknown_472F80(&m_Unknown1F0[n], &m_Unknown1F4[n], &m_Unknown1F8[n], h, m_Unknown1E4[n], color);
		}
		n++;
	}
	// the last row: averages with the first
	k0 = last_row << Field->Get_Log2();
	k1 = wrap << Field->Get_Log2();
	k2 = k0 + mask;
	k3 = k1 + mask;
	avg[0] = (unsigned short)((hz[k0 * 2 + 0] + hz[k1 * 2 + 0] + hz[k2 * 2 + 0] + hz[k3 * 2 + 0]) >> 2);
	avg[1] = (unsigned short)((hz[k0 * 2 + 1] + hz[k1 * 2 + 1] + hz[k2 * 2 + 1] + hz[k3 * 2 + 1]) >> 2);
	bits = (avg[0] + 0x77000) << 11 | 0x7FF;
	m_Unknown1E8[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_4);
	bits = (avg[1] + 0x77000) << 11 | 0x7FF;
	m_Unknown1EC[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_4);
	h[0] = (unsigned char)(avg[0] >> 8);
	h[1] = (unsigned char)(avg[1] >> 8);
	h[2] = (unsigned char)((a[k0 * 3 + 1] + a[k1 * 3 + 1] + a[k2 * 3 + 1] + a[k3 * 3 + 1]) >> 2);
	h[3] = (unsigned char)((b[k0 * 3 + 1] + b[k1 * 3 + 1] + b[k2 * 3 + 1] + b[k3 * 3 + 1]) >> 2);
	h[4] = (unsigned char)((a[k0 * 3] + a[k1 * 3] + a[k2 * 3] + a[k3 * 3]) >> 2);
	h[5] = (unsigned char)((b[k0 * 3 + 2] + b[k1 * 3 + 2] + b[k2 * 3 + 2] + b[k3 * 3 + 2]) >> 2);
	h[6] = (unsigned char)((b[k0 * 3] + b[k1 * 3] + b[k2 * 3] + b[k3 * 3]) >> 2);
	h[7] = (unsigned char)((a[k0 * 3 + 2] + a[k1 * 3 + 2] + a[k2 * 3 + 2] + a[k3 * 3 + 2]) >> 2);
	rgb = &Colors[(last << Field->Get_Log2()) * 3];
	{
		Vector3 color((float)rgb[2] * (1.0f / 255.0f), (float)rgb[1] * (1.0f / 255.0f), (float)rgb[0] * (1.0f / 255.0f));
		Unknown_472F80(&m_Unknown1F0[n], &m_Unknown1F4[n], &m_Unknown1F8[n], h, m_Unknown1E4[n], color);
	}
	n++;
	for (x = 1; x < last; x++) {
		k0 = (last_row << Field->Get_Log2()) + x;
		k1 = (wrap << Field->Get_Log2()) + x;
		avg[0] = (unsigned short)((hz[k0 * 2 + 0] + hz[k1 * 2 + 0]) >> 1);
		avg[1] = (unsigned short)((hz[k0 * 2 + 1] + hz[k1 * 2 + 1]) >> 1);
		bits = (avg[0] + 0x77000) << 11 | 0x7FF;
		m_Unknown1E8[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_2);
		bits = (avg[1] + 0x77000) << 11 | 0x7FF;
		m_Unknown1EC[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_2);
		h[0] = (unsigned char)(avg[0] >> 8);
		h[1] = (unsigned char)(avg[1] >> 8);
		h[2] = (unsigned char)((a[k0 * 3 + 1] + a[k1 * 3 + 1]) >> 1);
		h[3] = (unsigned char)((b[k0 * 3 + 1] + b[k1 * 3 + 1]) >> 1);
		h[4] = (unsigned char)((a[k0 * 3] + a[k1 * 3]) >> 1);
		h[5] = (unsigned char)((b[k0 * 3 + 2] + b[k1 * 3 + 2]) >> 1);
		h[6] = (unsigned char)((b[k0 * 3] + b[k1 * 3]) >> 1);
		h[7] = (unsigned char)((a[k0 * 3 + 2] + a[k1 * 3 + 2]) >> 1);
		rgb = &Colors[((last << Field->Get_Log2()) + x) * 3];
		{
			Vector3 color((float)rgb[2] * (1.0f / 255.0f), (float)rgb[1] * (1.0f / 255.0f), (float)rgb[0] * (1.0f / 255.0f));
			Unknown_472F80(&m_Unknown1F0[n], &m_Unknown1F4[n], &m_Unknown1F8[n], h, m_Unknown1E4[n], color);
		}
		n++;
	}
	k0 = (last_row << Field->Get_Log2()) + wrap;
	k1 = (wrap << Field->Get_Log2()) + wrap;
	k2 = (wrap << Field->Get_Log2()) + last;
	k3 = (last_row << Field->Get_Log2()) + last;
	avg[0] = (unsigned short)((hz[k0 * 2 + 0] + hz[k1 * 2 + 0] + hz[k2 * 2 + 0] + hz[k3 * 2 + 0]) >> 2);
	avg[1] = (unsigned short)((hz[k0 * 2 + 1] + hz[k1 * 2 + 1] + hz[k2 * 2 + 1] + hz[k3 * 2 + 1]) >> 2);
	bits = (avg[0] + 0x77000) << 11 | 0x7FF;
	m_Unknown1E8[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_4);
	bits = (avg[1] + 0x77000) << 11 | 0x7FF;
	m_Unknown1EC[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_4);
	h[0] = (unsigned char)(avg[0] >> 8);
	h[1] = (unsigned char)(avg[1] >> 8);
	h[2] = (unsigned char)((a[k0 * 3 + 1] + a[k1 * 3 + 1] + a[k2 * 3 + 1] + a[k3 * 3 + 1]) >> 2);
	h[3] = (unsigned char)((b[k0 * 3 + 1] + b[k1 * 3 + 1] + b[k2 * 3 + 1] + b[k3 * 3 + 1]) >> 2);
	h[4] = (unsigned char)((a[k0 * 3] + a[k1 * 3] + a[k2 * 3] + a[k3 * 3]) >> 2);
	h[5] = (unsigned char)((b[k0 * 3 + 2] + b[k1 * 3 + 2] + b[k2 * 3 + 2] + b[k3 * 3 + 2]) >> 2);
	h[6] = (unsigned char)((b[k0 * 3] + b[k1 * 3] + b[k2 * 3] + b[k3 * 3]) >> 2);
	h[7] = (unsigned char)((a[k0 * 3 + 2] + a[k1 * 3 + 2] + a[k2 * 3 + 2] + a[k3 * 3 + 2]) >> 2);
	rgb = &Colors[((last << Field->Get_Log2()) + size) * 3 - 3];
	{
		Vector3 color((float)rgb[2] * (1.0f / 255.0f), (float)rgb[1] * (1.0f / 255.0f), (float)rgb[0] * (1.0f / 255.0f));
		Unknown_472F80(&m_Unknown1F0[n], &m_Unknown1F4[n], &m_Unknown1F8[n], h, m_Unknown1E4[n], color);
	}
	n++;
}

// NOT EXACT: functional, from the disassembly and pseudocode of 0x472F80
void LandClass::Unknown_472F80(unsigned char *red, unsigned char *green, unsigned char *blue,
	unsigned char *horizons, unsigned char normal, const Vector3 &color)
{
	// 1 / sin(67.5), 1 / sin(22.5) degrees: per direction 2..5, the step's length over the cell
	float scales[4] = { 1.0823922f, 2.613126f, 2.613126f, 1.0823922f };
	float nx = Normals[normal].X;
	float ny = Normals[normal].Y;
	float nz = Normals[normal].Z;
	// the caller's codes into LandDirection order: 0 is +x (the horizon at +0x1EC), 4 is -x (+0x1E8)
	unsigned char dirs[8];
	dirs[0] = horizons[1];
	dirs[1] = horizons[5];
	dirs[2] = horizons[3];
	dirs[3] = horizons[6];
	dirs[4] = horizons[0];
	dirs[5] = horizons[4];
	dirs[6] = horizons[2];
	dirs[7] = horizons[7];
	// the vertex's own slope raises the horizon on the side it faces away from
	int i;
	for (i = 0; i < 4; i++) {
		float z = nz * LandDirectionY[i + 2];
		float d = LandDirectionX[i + 2] * ny - LandDirectionY[i + 2] * nx;
		int dir = i;
		if (d <= 0.0f) {
			dir = i + 4;
		}
		float t = (float)fabs(d) / ((float)fabs(z) * scales[i]);
		unsigned char code;
		if (t < 248.0f) {
			if (t > 0.00390625f) {
				code = (unsigned char)((*(unsigned int *)&t >> 19) - 0x770);	// the client: sub al, 0x70
			} else {
				code = 0;
			}
		} else {
			code = 0x7F;
		}
		if (dirs[dir] > code) {
			code = dirs[dir];
		}
		dirs[dir] = code;
	}
	// the sky seen over the eight horizons, cosine-weighted by the normal
	float sx = 0.0f;
	float sy = 0.0f;
	float sz = 0.0f;
	for (i = 0; i < 8; i++) {
		sx += LandHorizonTable[i][dirs[i]].X;
		sy += LandHorizonTable[i][dirs[i]].Y;
		sz += LandHorizonTable[i][dirs[i]].Z;
	}
	float sky = sx * nx + (sy * ny + sz * nz);	// the client adds y and z first
	// bounced light: the share grows with the ground's colour where the sky is hidden
	float r = sky / (1.0f - (1.0f - sky) * color.X);
	float g = sky / (1.0f - (1.0f - sky) * color.Y);
	float b = sky / (1.0f - (1.0f - sky) * color.Z);
	// at least 1 / 512 (a double), then 0..1 to a byte as 256 x - 1: the low byte of x + 32768 as a float
	float fr = (float)((r > 0.00195314 ? r : 0.00195314) + 32768.0f);
	float fg = (float)((g > 0.00195314 ? g : 0.00195314) + 32768.0f);
	float fb = (float)((b > 0.00195314 ? b : 0.00195314) + 32768.0f);
	*red = (unsigned char)(*(unsigned char *)&fr - 1);
	*green = (unsigned char)(*(unsigned char *)&fg - 1);
	*blue = (unsigned char)(*(unsigned char *)&fb - 1);
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
		Unknown_4733C0(row, ratio, (unsigned short *)c_row);
		c_row += size * 4;
		Unknown_473780(row, ratio, b_row);
		b_row += size * 3;
	}
	for (row = 0; row < size; row++) {
		Unknown_473BC0(row, ratio, a_row);
		a_row += size * 3;
	}
}

// NAME 2: a horizon slope as the high bits of its float, saturating (0x4733C0: 16 bits; 0x473780 / 0x473BC0: 8 bits)
static inline unsigned short Pack_Slope16(float slope)
{
	if (slope >= 255.96875f) {
		return 0x7FFF;
	}
	if (slope <= 0.00390625f) {
		return 0;
	}
	return (unsigned short)((*(unsigned int *)&slope >> 11) - 0x7000);
}

static inline unsigned char Pack_Slope8(float slope)
{
	if (slope >= 248.0f) {
		return 0x7F;
	}
	if (slope <= 0.00390625f) {
		return 0;
	}
	return (unsigned char)((*(unsigned int *)&slope >> 19) - 0x70);
}

void LandClass::Unknown_4733C0(int row, float ratio, unsigned short *out)
{
	unsigned int row0 = (Field->Get_Mask() & row) << Field->Get_Log2();
	unsigned short *heights = Field->Get_Heights();
	unsigned int row1 = ((row + 1) & Field->Get_Mask()) << Field->Get_Log2();
	unsigned int steps = ((unsigned int)Field->Get_Size() > 128 ? 128 : Field->Get_Size()) * 2;
	float scale = ratio + ratio;
	for (unsigned int i = 0; i < Field->Get_Size(); i++) {
		unsigned int i1 = (i + 1) & Field->Get_Mask();
		float center = ((float)heights[row1 + i1] + heights[row0 + i1] + heights[row1 + i] + heights[row0 + i]) * 0.25f;
		float rise = (Field->Get_Max_Value() - center) * scale;
		float best = 0.05f;
		float dist = 1.0f;
		unsigned int x = i;
		unsigned int k;
		for (k = 1; k < steps && dist * best < rise; k += 2) {
			float slope = (((float)heights[x + row1] + heights[x + row0]) * 0.5f - center) * Reciprocals[k - 1] * scale;
			if (slope > best) {
				best = slope;
			}
			dist += 2.0f;
			x = (x - 1) & Field->Get_Mask();
		}
		*out++ = Pack_Slope16(best);
		best = 0.05f;
		dist = 1.0f;
		x = i + 1;
		for (k = 1; k < steps && dist * best < rise; k += 2) {
			float slope = (((float)heights[x + row1] + heights[x + row0]) * 0.5f - center) * Reciprocals[k - 1] * scale;
			if (slope > best) {
				best = slope;
			}
			x = (x + 1) & Field->Get_Mask();
			dist += 2.0f;
		}
		*out++ = Pack_Slope16(best);
	}
}

// NOT EXACT: frame (the client's frame 0x14 bigger, 147 masked rows; 2 forms, attempts.csv 0x473780)
void LandClass::Unknown_473780(int row, float ratio, unsigned char *out)
{
	unsigned int row0 = (Field->Get_Mask() & row) << Field->Get_Log2();
	float scale = ratio + ratio;
	unsigned short *heights = Field->Get_Heights();
	unsigned int next = row + 1;
	float diagonal = scale * 0.70710677f;
	unsigned int row1 = (next & Field->Get_Mask()) << Field->Get_Log2();
	for (unsigned int i = 0; i < Field->Get_Size(); i++) {
		unsigned int i1 = (i + 1) & Field->Get_Mask();
		float center = ((float)heights[row1 + i1] + heights[row0 + i1] + heights[row1 + i] + heights[row0 + i]) * 0.25f;
		float best_left = 0.05f;
		float best_mid = 0.05f;
		float best_right = 0.05f;
		const float *recip = Reciprocals;
		float dist = 1.0f;
		unsigned int y = next;
		unsigned int offset = row1;
		unsigned int left = i;
		unsigned int right = i1;
		for (int k = 0; k < 12; k++) {
			float h_left = heights[(left & Field->Get_Mask()) + offset];
			float mid = ((float)heights[offset + i1] + heights[i + offset]) * 0.5f;
			float h_right = heights[(right & Field->Get_Mask()) + offset];
			float s_left = (h_left - center) * diagonal * *recip;
			float s_mid = (mid - center) * scale * *recip;
			float s_right = (h_right - center) * diagonal * *recip;
			if (s_left > best_left) {
				best_left = s_left;
			}
			if (s_mid > best_mid) {
				best_mid = s_mid;
			}
			if (s_right > best_right) {
				best_right = s_right;
			}
			y = (y + 1) & Field->Get_Mask();
			recip += 2;
			left--;
			right++;
			offset = (offset + Field->Get_Size()) & Field->Get_Count_Mask();
			dist += 2.0f;
		}
		out[0] = Pack_Slope8(best_left);
		out[1] = Pack_Slope8(best_mid);
		out[2] = Pack_Slope8(best_right);
		out += 3;
	}
}

// NOT EXACT: frame (the client's frame 0x14 bigger; 2 forms, attempts.csv 0x473BC0)
void LandClass::Unknown_473BC0(int row, float ratio, unsigned char *out)
{
	unsigned int row0 = (Field->Get_Mask() & row) << Field->Get_Log2();
	float scale = ratio + ratio;
	unsigned short *heights = Field->Get_Heights();
	float diagonal = scale * 0.70710677f;
	unsigned int row1 = ((row + 1) & Field->Get_Mask()) << Field->Get_Log2();
	for (unsigned int i = 0; i < Field->Get_Size(); i++) {
		unsigned int i1 = (i + 1) & Field->Get_Mask();
		float center = ((float)heights[row1 + i1] + heights[i1 + row0] + heights[row1 + i] + heights[row0 + i]) * 0.25f;
		float best_left = 0.05f;
		float best_mid = 0.05f;
		float best_right = 0.05f;
		const float *recip = Reciprocals;
		float dist = 1.0f;
		unsigned int y = row;
		unsigned int offset = row0;
		unsigned int left = i;
		unsigned int right = i1;
		for (int k = 0; k < 12; k++) {
			float h_left = heights[(left & Field->Get_Mask()) + offset];
			float mid = ((float)heights[offset + i1] + heights[i + offset]) * 0.5f;
			float h_right = heights[(right & Field->Get_Mask()) + offset];
			float s_left = (h_left - center) * diagonal * *recip;
			float s_mid = (mid - center) * scale * *recip;
			float s_right = (h_right - center) * diagonal * *recip;
			if (s_left > best_left) {
				best_left = s_left;
			}
			if (s_mid > best_mid) {
				best_mid = s_mid;
			}
			if (s_right > best_right) {
				best_right = s_right;
			}
			y = (y - 1) & Field->Get_Mask();
			recip += 2;
			left--;
			right++;
			offset = (offset - Field->Get_Size()) & Field->Get_Count_Mask();
			dist += 2.0f;
		}
		out[0] = Pack_Slope8(best_left);
		out[1] = Pack_Slope8(best_mid);
		out[2] = Pack_Slope8(best_right);
		out += 3;
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
