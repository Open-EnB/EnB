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
#include "texproject.h"
#include "quat.h"
#include "random.h"
#include "matinfo.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern LandUnknown466BE0Class *Unknown_BDC0D8;	// NAME 1: unknown
// NAME 2: the distance (in field cells) of level 1 per lod setting, doubled per level (0xB7188C: eight entries, VIDTEST's
// Land LOD 0..7 indexes it directly)
int LandLodDistances[8] = { 8, 16, 24, 32, 48, 64, 96, 128 };
	// NAME 1: unknown; deletes a global object (0xBDC0D8) and clears it; called by the land's dtor only

LandClass::LandClass(void) :
	LightmapSerial(0),
	Reciprocals(NULL),
	HasSea(false),
	CameraCellX(-999999),
	CameraCellY(-999999),
	CameraCellZ(0),
	Unknown120(NULL),
	DetailTexture(NULL),
	Material(NULL),
	Patches(NULL),
	Is16Bit(false),
	Flag(false),
	Dirty(true),
	FrontLightmap(0),
	BackLightmap(1),
	SeaCount(0),
	Unknown17C(ShaderClass::_PresetOpaqueShader),
	Unknown184(ShaderClass::_PresetOpaqueShader),
	PatchRecords(NULL),
	Unknown190(false),
	Unknown191(false),
	Unknown192(true),
	Thread(NULL),
	UpdateMin(0, 0),
	UpdateMax(0, 0),
	CursorX(0),
	CursorY(0),
	ThreadDone(false),
	Unknown1E4(NULL),
	Unknown1E8(NULL),
	Unknown1EC(NULL),
	Unknown1F0(NULL),
	Unknown1F4(NULL),
	Unknown1F8(NULL),
	HorizonLow(0),
	HorizonHigh(0),
	SkyColorDX8(0),
	SunFromWest(true),
	ShadowStep(0),
	Unknown120C(NULL),
	Colors(NULL),
	Unknown1214(NULL)
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
	Unknown1E4 = new unsigned char[Field->Get_Count()];
	Unknown1E8 = new unsigned short[Field->Get_Count()];
	Unknown1EC = new unsigned short[Field->Get_Count()];
	Unknown1F0 = new unsigned char[Field->Get_Count()];
	Unknown1F4 = new unsigned char[Field->Get_Count()];
	Unknown1F8 = new unsigned char[Field->Get_Count()];
	Unknown120C = new unsigned int[Field->Get_Count()];
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
	Unknown120 = new TextureClass(Field->Get_Size(), Field->Get_Size(), format, MIP_LEVELS_1, TextureClass::POOL_SYSTEMMEM);
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
			Unknown1214 = new unsigned char[pixels];
			for (int y = 0; y < targa.Header.Height; y++) {
				for (int x = 0; x < targa.Header.Width; x++) {
					int n = y * targa.Header.Width + x;
					Colors[n * 3] = image[n * 4];
					Colors[n * 3 + 1] = image[n * 4 + 1];
					Colors[n * 3 + 2] = image[n * 4 + 2];
					Unknown1214[n] = image[n * 4 + 3];
				}
			}
			break;
	}

	// a colour map larger than a texture may be: average it down to max_size a side
	if (max_size < (unsigned int)targa.Header.Width) {
		unsigned char *colors = new unsigned char[max_size * max_size * 3];
		unsigned char *extra = NULL;
		if (Unknown1214 != NULL) {
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
						if (Unknown1214 != NULL) {
							a += Unknown1214[n];
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
				if (Unknown1214 != NULL) {
					extra[k] = (unsigned char)a;
				}
			}
		}
		delete [] Colors;
		Colors = colors;
		if (Unknown1214 != NULL) {
			delete [] Unknown1214;
			Unknown1214 = extra;
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
	REF_PTR_RELEASE(Unknown120);
	REF_PTR_RELEASE(DetailTexture);
	REF_PTR_RELEASE(Material);
	Unknown178 = 0;
	Unknown180 = 0;
	Unknown188 = 0;
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
	if (Unknown1E4 != NULL) delete [] Unknown1E4;
	if (Unknown1E8 != NULL) delete [] Unknown1E8;
	if (Unknown1EC != NULL) delete [] Unknown1EC;
	if (Unknown1F0 != NULL) delete [] Unknown1F0;
	if (Unknown1F4 != NULL) delete [] Unknown1F4;
	if (Unknown1F8 != NULL) delete [] Unknown1F8;
	if (Unknown120C != NULL) delete [] Unknown120C;
	if (Colors != NULL) delete [] Colors;
	if (Unknown1214 != NULL) delete [] Unknown1214;
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
		return;
	}
	if (CursorY == size) {
		Swap_Lightmaps();
		CursorY = 0;
		CursorX = 0;
		Set_Lighting(lighting);
		Prepare_Lighting();
	} else {
		Vector2i &min = UpdateMin;
		Vector2i &max = UpdateMax;
		min.Set(CursorX, CursorY);
		max.Set(LightmapChunk + CursorX, CursorY + LightmapChunk);
		Compute_Lighting();
		Apply_Colors();
		Upload_Lightmap(min, max);
		CursorX += LightmapChunk;
		if (CursorX == size) {
			CursorX = 0;
			CursorY += LightmapChunk;
		}
	}
}

void LandClass::Update_Lightmap_Threaded(const LandLightingStruct &lighting)
{
	if (LightmapTextures[FrontLightmap]->Is_Dirty()) {
		Update_Lightmap_Full(lighting);
		return;
	}
	if (ThreadDone) {
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
		min.Set(CursorX, y);
		max.Set(CursorX + LightmapChunk, LightmapChunk + y);
		Upload_Lightmap(min, max);
		CursorX += LightmapChunk;
		if (CursorX == size) {
			CursorX = 0;
			CursorY += LightmapChunk;
		}
	} else {
		if (Thread == NULL) {
			Set_Lighting(lighting);
			Prepare_Lighting();
			Vector2i &min = UpdateMin;
			Vector2i &max = UpdateMax;
			min.I = 0;
			min.J = 0;
			int size = Field->Get_Size();
			max.I = size;
			max.J = size;
			Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Lightmap_Thread, this, 0, NULL);
		}
		if (WaitForSingleObject(Thread, 0) == WAIT_OBJECT_0) {
			Thread = NULL;
			ThreadDone = true;
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
	if (Unknown_BDC0D8->Unknown11) {
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

// NOT EXACT: written from the pseudocode (0x474B30); VIDTEST's land scene runs it (D101)
void LandClass::Render(RenderInfoClass &rinfo)
{
	DX8Wrapper::Set_Texture(0, LightmapTextures[FrontLightmap]);
	DX8Wrapper::Set_Texture(1, DetailTexture);
	DX8Wrapper::Set_Shader(Shader);
	DX8Wrapper::Set_Material(Material);
	static DynamicVectorClass<Vector3> sea_quads(10);
	sea_quads.Delete_All();
	bool companion = Unknown_BDC0D8->Unknown11;
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
				(*patch)->Draw_Strip();
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
	for (std::vector< RefCountPtr<TexProjectClass> >::iterator it = Unknown1218.begin(); it != Unknown1218.end(); it++) {
		TexProjectClass *projector = (*it).Peek();
		projected.erase(projected.begin(), projected.end());
		Field->Get_Triangles(projector->Get_Bounding_Volume(), projected);
		projector->Peek_Material_Pass()->Install_Materials();
		Unknown_477E20(projected);
	}

	// the patch records' extra buffers (+0x00 / +0x04 and +0x08 / +0x0C), up to level 2
	if (Unknown192) {
		if (Unknown190) {
			DX8Wrapper::Set_Texture(0, (TextureClass *)Unknown178.Peek());
			DX8Wrapper::Set_Texture(1, NULL);
			DX8Wrapper::Set_Shader(Unknown17C);
			DX8Wrapper::Set_Material((VertexMaterialClass *)Unknown180.Peek());
			for (i = 0; i < PatchCount; i++) {
				LandPatchRecordClass &record = PatchRecords[i];
				if (record.Visible && record.Unknown10 > 0 && (unsigned int)record.Level < 3) {
					DX8Wrapper::Set_Vertex_Buffer((VertexBufferClass *)record.Unknown00.Peek());
					DX8Wrapper::Set_Index_Buffer((IndexBufferClass *)record.Unknown04.Peek(), 0);
					DX8Wrapper::Draw_Triangles(0, (unsigned short)(record.Unknown14 / 3), 0, (unsigned short)record.Unknown10);
				}
			}
		}
		if (Unknown191) {
			DX8Wrapper::Set_Texture(0, (TextureClass *)Unknown178.Peek());
			DX8Wrapper::Set_Texture(1, NULL);
			DX8Wrapper::Set_Shader(Unknown184);
			DX8Wrapper::Set_Material((VertexMaterialClass *)Unknown188.Peek());
			DX8Wrapper::Set_DX8_Render_State(D3DRS_ZBIAS, 1);
			for (i = 0; i < PatchCount; i++) {
				LandPatchRecordClass &record = PatchRecords[i];
				if (record.Visible && record.Unknown18 > 0 && (unsigned int)record.Level < 3) {
					DX8Wrapper::Set_Vertex_Buffer((VertexBufferClass *)record.Unknown08.Peek());
					DX8Wrapper::Set_Index_Buffer((IndexBufferClass *)record.Unknown0C.Peek(), 0);
					DX8Wrapper::Draw_Triangles(0, (unsigned short)(record.Unknown1C / 3), 0, (unsigned short)record.Unknown18);
				}
			}
			DX8Wrapper::Set_DX8_Render_State(D3DRS_ZBIAS, 0);
		}
	}
}

// 0x477E20: the projected triangles as a dynamic FVF 0x252 list (at most 30000 vertices), raised 3 units where the device
// has no z bias, drawn with z bias 1
void Unknown_477E20(std::vector<Vector3> verts)
{
	int vertex_count = (int)WWMath::Min((float)verts.size(), 30000.0f);
	int polygon_count = vertex_count / 3;
	float z_offset = 0.0f;
	if (!DX8Wrapper::Get_Current_Caps()->Support_ZBias()) {
		z_offset = 3.0f;
	}
	DynamicIBAccessClass ib(BUFFER_TYPE_DYNAMIC_DX8, vertex_count);
	{
		DynamicIBAccessClass::WriteLockClass lock(&ib);
		unsigned short *indices = lock.Get_Index_Array();
		for (int i = 0; i < vertex_count; i++) {
			*indices++ = (unsigned short)i;
		}
	}
	DynamicVBAccessClass vb(BUFFER_TYPE_DYNAMIC_DX8, 0x252, vertex_count);
	{
		DynamicVBAccessClass::WriteLockClass lock(&vb);
		unsigned char *vertices = (unsigned char *)lock.Get_Formatted_Vertex_Array();
		const FVFInfoClass &fi = vb.FVF_Info();
		int n = 0;
		for (std::vector<Vector3>::iterator it = verts.begin(); it != verts.end(); it++) {
			*(Vector3 *)(vertices + fi.Get_Location_Offset()) = Vector3((*it).X, (*it).Y, (*it).Z + z_offset);
			vertices += fi.Get_FVF_Size();
			if (++n >= vertex_count) break;
		}
	}
	DX8Wrapper::Set_Index_Buffer(ib, 0);
	DX8Wrapper::Set_Vertex_Buffer(vb);
	DX8Wrapper::Set_DX8_Render_State(D3DRS_ZBIAS, 1);
	DX8Wrapper::Draw_Triangles(0, (unsigned short)polygon_count, 0, (unsigned short)vertex_count);
	DX8Wrapper::Set_DX8_Render_State(D3DRS_ZBIAS, 0);
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

// The client calls it out of line from the land's destructor (0x46A656); defined here it inlines there (the empty-loop
// lever doesn't stop it): auto_inline(off) keeps it out of line
#pragma auto_inline(off)
LandPatchClass::~LandPatchClass(void)
{
	REF_PTR_RELEASE(VertexBuffer);
}
#pragma auto_inline(on)

LandPatchRecordClass::LandPatchRecordClass(void) :
	Unknown10(0),
	Unknown14(0),
	Unknown18(0),
	Unknown1C(0),
	Visible(false),
	Level(0)
{
}

// The client releases the four pointers in the body, then again as members (0x4749D0); with an inline candidate here the
// land's destructor gains three EH states (its delete [] through __ehvec_dtor): auto_inline(off) as the patch's
#pragma auto_inline(off)
LandPatchRecordClass::~LandPatchRecordClass(void)
{
	Unknown00 = NULL;
	Unknown04 = NULL;
	Unknown08 = NULL;
	Unknown0C = NULL;
}
#pragma auto_inline(on)

void LandClass::Set_Strip_Buffer(int level)
{
	DX8Wrapper::Set_Index_Buffer(StripBuffers[level], 0);
}

void LandClass::Set_Edge_Buffer(int level)
{
	DX8Wrapper::Set_Index_Buffer(EdgeBuffers[level], 0);
}

void LandClass::Add_Unknown1218(const RefCountPtr<TexProjectClass> &object)
{
	Unknown1218.push_back(object);
}

void LandClass::Clear_Unknown1218(void)
{
	Unknown1218.clear();
}

char *Unknown_4A1590(char *buffer, int size, const char *string);	// 0x4A1590: a bounded strcpy  // NAME 1: neutral placeholder (D10)

// NAME 2: the .dcb's first word (0x4795D0, 0x479A50)
#define LAND_DECORATION_ID			0xDEC0DEC0

// NAME 2: a decoration as the .dcb keeps it, 0x20 bytes (0x479A50): no height, the reader puts it on the ground
struct LandDecorationRecordStruct
{
	unsigned char				Proto;
	Quaternion					Rotation;
	float							X;
	float							Y;
	float							Scale;
};

void LandClass::Load_Prototype_Names(std::vector<std::string> &names, INIClass &ini, const char *section)
{
	int count = ini.Get_Int(section, "NUM_PROTOS", 0);
	for (int i = 0; i < count; i++) {
		char entry[256];
		char name[1024];
		sprintf(entry, "PROTO%d", i);
		ini.Get_String(section, entry, "TDTREE01A", name, sizeof(name));
		names.push_back(std::string(name));
	}
}

// NAME 2: one inline level around the name's string and push_back: the client calls the string's assign and the names'
// insert out of line, as this depth gives (Load_Decorations_Ini 0x478B50)
static inline void Land_Add_Name(std::vector<std::string> &names, const char *name)
{
	std::string string(name);
	names.push_back(string);
}

// NOT EXACT: inline-budget (32 masked rows: inside the inlined decorations' insert the client calls size() 0x47D170 and
// _Ufill 0x47E560 out of line, ours inlines them; the ini sits 4 bytes lower; attempts.csv 0x478B50)
void LandClass::Load_Decorations_Ini(std::vector<std::string> &names, std::vector<LandDecorationStruct> &decorations, const char *filename)
{
	INIClass ini;
	char section[256];
	ini.Load(filename);
	if (ini.Is_Loaded()) {
		int count = ini.Get_Int("Prototypes", "NUM_PROTOS", 0);
		for (int i = 0; i < count; i++) {
			char entry[256];
			char name[1024];
			sprintf(entry, "PROTO%d", i);
			ini.Get_String("Prototypes", entry, "TDTREE01A", name, sizeof(name));
			Land_Add_Name(names, name);
		}
		unsigned int proto_count;
		if (names.begin() != NULL && (proto_count = names.size()) != 0) {
			int n = 0;
			sprintf(section, "DECO%d", n);
			while (ini.Find_Section(section) != NULL) {
				LandDecorationStruct decoration;
				decoration.Proto = ini.Get_Int(section, "DecID", 0);
				if ((unsigned int)decoration.Proto >= proto_count) {
					decoration.Proto = 0;
				}
				Matrix3D tm(true);
				char buffer[256];
				ini.Get_String(section, "M", "", buffer, sizeof(buffer));
				char *token = strtok(buffer, ",");
				for (int column = 0; column < 4; column++) {
					for (int row = 0; row < 3; row++) {
						if (token != NULL) {
							tm[row][column] = (float)atof(token);
							token = strtok(NULL, ",");
						}
					}
				}
				decoration.Transform = tm;
				decoration.Scale = ini.Get_Float(section, "Scale", 1.0f);
				decorations.push_back(decoration);
				n++;
				sprintf(section, "DECO%d", n);
			}
		}
	}
}

// 0x4791D0: the prototypes and decorations as an ini (Load_Decorations_Ini's layout; M is the matrix column by column,
// "%f" then ",%f"); nothing when either list is empty
// NOT EXACT: frame (stack slots only: the client keeps the decorations' begin above the ini and the row counter below it;
// 25 forms, declaration order changes nothing; attempts.csv 0x4791D0)
void LandClass::Save_Decorations_Ini(std::vector<std::string> &names, std::vector<LandDecorationStruct> &decorations, const char *filename)
{
	INIClass ini;
	unsigned int proto_count = names.size();
	unsigned int count = decorations.size();
	if (proto_count != 0 && count != 0) {
		ini.Put_Int("Prototypes", "NUM_PROTOS", proto_count);
		char section[256];
		unsigned int i;
		for (i = 0; i < proto_count; i++) {
			sprintf(section, "PROTO%d", i);
			ini.Put_String("Prototypes", section, names[i].c_str());
		}
		for (i = 0; i < count; i++) {
			sprintf(section, "DECO%d", i);
			LandDecorationStruct &decoration = decorations[i];
			ini.Put_Int(section, "DecID", decoration.Proto);
			const Matrix3D &tm = decoration.Transform;
			std::string m;
			bool first = true;
			for (int column = 0; column < 4; column++) {
				for (int row = 0; row < 3; row++) {
					char buffer[512];
					if (first) {
						first = false;
						sprintf(buffer, "%f", tm[row][column]);
					} else {
						sprintf(buffer, ",%f", tm[row][column]);
					}
					m += buffer;
				}
			}
			ini.Put_String(section, "M", m.c_str());
			ini.Put_Float(section, "Scale", decoration.Scale);
		}
		ini.Save(filename);
	}
}

void LandClass::Load_Decorations(std::vector<std::string> &names, std::vector<LandDecorationStruct> &decorations, const char *filename)
{
	if (Field == NULL) {
		return;
	}
	file_auto_ptr file(_TheFileFactory, filename);
	if (!file->Is_Available()) {
		return;
	}
	file->Open(FileClass::READ);
	unsigned int id;
	if (file->Read(&id, 4) != 4 || id != LAND_DECORATION_ID) {
		return;
	}
	unsigned int count;
	if (file->Read(&count, 4) != 4) {
		return;
	}
	for (id = 0; id < count; id++) {
		char name[32];
		memset(name, 0, sizeof(name));
		std::string string;
		if (file->Read(name, sizeof(name)) != sizeof(name)) {
			return;
		}
		string = name;
		names.push_back(string);
	}
	unsigned int decoration_count;
	if (file->Read(&decoration_count, 4) != 4) {
		return;
	}
	for (id = 0; id < decoration_count; id++) {
		LandDecorationRecordStruct record;
		if (file->Read(&record, sizeof(record)) != sizeof(record)) {
			return;
		}
		Quaternion rotation(record.Rotation.X, record.Rotation.Y, record.Rotation.Z, record.Rotation.W);
		Matrix3D tm = Build_Matrix3D(rotation);
		Vector3 pos;
		pos.X = record.X;
		pos.Y = record.Y;
		pos.Z = Field->Get_Height(pos);
		tm.Set_Translation(pos);
		LandDecorationStruct decoration;
		decoration.Proto = record.Proto;
		if ((unsigned int)decoration.Proto > count) {
			decoration.Proto = 0;
		}
		decoration.Transform = tm;
		decoration.Scale = record.Scale;
		decorations.push_back(decoration);
	}
	file->Close();
}

// NAME 2: a decoration vertex, FVF 0x112 (0x47C6A0; the patch records' buffers, 0x47A2F0); plain floats: 0x47DE10 copies it as one block
struct LandDecorationVertexStruct
{
	float						Position[3];
	float						Normal[3];
	float						UV[2];
};

// NOT EXACT: shape (written from the disassembly: the client keeps this in a frame slot, calls the string constructor and the
// vector insert out of line, builds the rotation from a zero local; 1 form, attempts.csv 0x479D00)
void LandClass::Create_Decorations(int set)
{
	INIClass ini;
	ini.Load("decoration.ini");
	char section[256];
	sprintf(section, "%d", set);
	int count = ini.Get_Int(section, "NUM_DECOS", 0);
	if (count <= 0) {
		return;
	}
	float min_scale = ini.Get_Float(section, "MIN_SCALE", 1.0f);
	float max_scale = ini.Get_Float(section, "MAX_SCALE", 2.0f);
	std::vector<std::string> names;
	int proto_count = ini.Get_Int(section, "NUM_PROTOS", 0);
	for (int i = 0; i < proto_count; i++) {
		char entry[256];
		char name[1024];
		sprintf(entry, "PROTO%d", i);
		ini.Get_String(section, entry, "TDTREE01A", name, sizeof(name));
		names.push_back(std::string(name));
	}
	if (names.size() <= 0) {
		return;
	}
	std::vector<LandDecorationStruct> decorations;
	Random4Class random(0x1105);
	while (count--) {
		LandDecorationStruct decoration;
		decoration.Proto = Pick_Random_Number(random, 0, names.size() - 1);
		decoration.Scale = random.Get_Float() * (max_scale - min_scale) + min_scale;
		Vector3 pos;
		do {
			Vector2 uv(random.Get_Float(), random.Get_Float());
			pos = Field->Sample(uv);
		} while (HasSea && pos.Z <= SeaLevel);
		float angle = random.Get_Float() * (2.0f * WWMATH_PI);
		float c = (float)cos(angle);
		float s = (float)sin(angle);
		decoration.Transform.Set(c, -s, 0.0f, pos.X, s, c, 0.0f, pos.Y, 0.0f, 0.0f, 1.0f, pos.Z);
		decorations.push_back(decoration);
	}
	Build_Decorations(names, decorations);
}

// 0x47C580 (VIDTEST 0x418400): the first mesh under robj, depth first, whose name holds ".d" or ".D" (a tree) or ".s" or
// ".S" (its shadow); add-reffed
// NOT EXACT: functional, from the disassembly
static RenderObjClass *Land_Find_Decoration_Mesh(RenderObjClass *robj, bool tree)	// NAME 2: after what it does
{
	if (robj->Class_ID() == RenderObjClass::CLASSID_MESH) {
		const char *name = robj->Get_Name();
		if (name == NULL) {
			return NULL;
		}
		if (tree) {
			if (strstr(name, ".d") == NULL && strstr(name, ".D") == NULL) {
				return NULL;
			}
		} else {
			if (strstr(name, ".s") == NULL && strstr(name, ".S") == NULL) {
				return NULL;
			}
		}
		robj->Add_Ref();
		return robj;
	}
	int count = robj->Get_Num_Sub_Objects();
	for (int i = 0; i < count; i++) {
		RenderObjClass *sub = robj->Get_Sub_Object(i);
		if (sub != NULL) {
			RenderObjClass *found = Land_Find_Decoration_Mesh(sub, tree);
			sub->Release_Ref();
			if (found != NULL) {
				return found;
			}
		}
	}
	return NULL;
}

// 0x47C6A0: a mesh's vertices (scaled, then through tm; normals rotated, not normalised; no normals or uvs give zeros) and its
// triangles' indices plus base, appended; returns the largest index the triangles use
// NOT EXACT: inline budget (the third index push_back's reallocation inlines the first _Ucopy, the client calls it; index-loop registers; attempts.csv 0x47C6A0)
static unsigned short Land_Append_Decoration_Mesh(MeshClass *mesh, std::vector<LandDecorationVertexStruct> &vertices,
	std::vector<unsigned short> &indices, unsigned short base, const Matrix3D &tm, float scale)	// NAME 2: after what it does
{
	unsigned short largest = 0;
	MeshModelClass *model = mesh->Peek_Model();
	const Vector2 *uv = model->Get_UV_Array(0, 0);
	const TriIndex *polys = model->Get_Polygon_Array();
	const Vector3 *points = model->Get_Vertex_Array();
	const Vector3 *normals = model->Get_Vertex_Normal_Array();
	if (polys != NULL && points != NULL) {
		int i;
		int vertex_count = model->Get_Vertex_Count();
		int poly_count = model->Get_Polygon_Count();
		for (i = 0; i < vertex_count; i++) {
			Vector3 pos = tm * (scale * points[i]);
			LandDecorationVertexStruct vertex;
			vertex.Position[0] = pos.X;
			vertex.Position[1] = pos.Y;
			vertex.Position[2] = pos.Z;
			Vector3 normal(0.0f, 0.0f, 0.0f);
			if (normals != NULL) {
				normal = tm.Rotate_Vector(normals[i]);
			}
			vertex.Normal[0] = normal.X;
			vertex.Normal[1] = normal.Y;
			vertex.Normal[2] = normal.Z;
			Vector2 tex(0.0f, 0.0f);
			if (uv != NULL) {
				tex = uv[i];
			}
			vertex.UV[0] = tex.X;
			vertex.UV[1] = tex.Y;
			vertices.push_back(vertex);
		}
		for (i = 0; i < poly_count; i++) {
			largest = (unsigned short)WWMath::Max((float)polys[i].I, (float)largest);
			indices.push_back((unsigned short)(base + polys[i].I));
			largest = (unsigned short)WWMath::Max((float)polys[i].J, (float)largest);
			indices.push_back((unsigned short)(base + polys[i].J));
			largest = (unsigned short)WWMath::Max((float)polys[i].K, (float)largest);
			indices.push_back((unsigned short)(base + polys[i].K));
		}
	}
	return largest;
}

// NOT EXACT: functional, from the disassembly and the pseudocode (0x47A2F0 = VIDTEST 0x416170, 7 KB). The prototypes' tree
// and shadow meshes; the first tree's texture, shader and vertex material (+0x178 / +0x17C / +0x180) and the first shadow's
// shader and material (+0x184 / +0x188), every other prototype's must be the same (else no decorations); each decoration
// appended to its patch's lists (the patch from World_To_Grid over the grid width), the shadow turned onto the ground's
// normal (the quaternion from z to the normal, its half angle not normalised) and lifted 3 units along it; then the patch
// records' vertex and index buffers. Nothing when the height field wraps. The client sets a global byte 0xBFAE52 while it
// loads the prototypes when the land's +0x135 is set (not rebuilt).
void LandClass::Build_Decorations(std::vector<std::string> &names, std::vector<LandDecorationStruct> &decorations)
{
	int count = decorations.size();
	if (count < 1) {
		return;
	}
	int proto_count = names.size();
	Unknown190 = false;
	Unknown191 = false;
	if (Field->Is_Wrapping()) {
		return;
	}
	std::vector<MeshClass *> trees;
	std::vector<MeshClass *> shadows;
	int i;
	for (i = 0; i < proto_count; i++) {
		RenderObjClass *robj = WW3DAssetManager::Get_Instance()->Create_Render_Obj(names[i].c_str());
		if (robj == NULL) {
			for (int k = 0; k < (int)trees.size(); k++) {
				if (trees[k] != NULL) trees[k]->Release_Ref();
				if (shadows[k] != NULL) shadows[k]->Release_Ref();
			}
			return;
		}
		trees.push_back((MeshClass *)Land_Find_Decoration_Mesh(robj, true));
		shadows.push_back((MeshClass *)Land_Find_Decoration_Mesh(robj, false));
		robj->Release_Ref();
	}
	bool has_trees = false;
	bool has_shadows = false;
	for (i = 0; i < proto_count; i++) {
		if (trees[i] != NULL) {
			MaterialInfoClass *info = trees[i]->Get_Material_Info();
			TextureClass *texture = info->Get_Texture(0);
			Unknown178 = Create_Get(texture);
			Unknown17C = trees[i]->Peek_Model()->Get_Shader(0, 0);
			Unknown180 = Create_Get(info->Get_Vertex_Material(0));
			has_trees = true;
			info->Release_Ref();
			break;
		}
	}
	for (i = 0; i < proto_count; i++) {
		if (shadows[i] != NULL) {
			MaterialInfoClass *info = shadows[i]->Get_Material_Info();
			Unknown184 = shadows[i]->Peek_Model()->Get_Shader(0, 0);
			Unknown188 = Create_Get(info->Get_Vertex_Material(0));
			has_shadows = true;
			info->Release_Ref();
			break;
		}
	}
	bool same = true;
	for (i = 0; i < proto_count && same; i++) {
		if (has_trees && trees[i] != NULL) {
			MaterialInfoClass *info = trees[i]->Get_Material_Info();
			TextureClass *texture = info->Get_Texture(0);
			ShaderClass shader = trees[i]->Peek_Model()->Get_Shader(0, 0);
			VertexMaterialClass *material = info->Peek_Vertex_Material(0);
			if ((RefCountClass *)texture != Unknown178.Peek() || !(shader == Unknown17C)
				|| material->Get_CRC() != ((VertexMaterialClass *)Unknown180.Peek())->Get_CRC()) {
				same = false;
			}
			REF_PTR_RELEASE(texture);
			info->Release_Ref();
		}
		if (same && has_shadows && shadows[i] != NULL) {
			MaterialInfoClass *info = shadows[i]->Get_Material_Info();
			TextureClass *texture = info->Get_Texture(0);
			ShaderClass shader = shadows[i]->Peek_Model()->Get_Shader(0, 0);
			VertexMaterialClass *material = info->Peek_Vertex_Material(0);
			if ((RefCountClass *)texture != Unknown178.Peek() || !(shader == Unknown184)
				|| material->Get_CRC() != ((VertexMaterialClass *)Unknown188.Peek())->Get_CRC()) {
				same = false;
			}
			REF_PTR_RELEASE(texture);
			info->Release_Ref();
		}
	}
	if (same && (has_trees || has_shadows)) {
		Unknown190 = has_trees;
		Unknown191 = has_shadows;
		std::vector<LandDecorationVertexStruct> *tree_vertices = new std::vector<LandDecorationVertexStruct>[PatchCount];
		std::vector<unsigned short> *tree_indices = new std::vector<unsigned short>[PatchCount];
		std::vector<LandDecorationVertexStruct> *shadow_vertices = new std::vector<LandDecorationVertexStruct>[PatchCount];
		std::vector<unsigned short> *shadow_indices = new std::vector<unsigned short>[PatchCount];
		unsigned short *tree_counts = new unsigned short[PatchCount];
		unsigned short *shadow_counts = new unsigned short[PatchCount];
		memset(tree_counts, 0, PatchCount * sizeof(unsigned short));
		memset(shadow_counts, 0, PatchCount * sizeof(unsigned short));
		for (int d = 0; d < count; d++) {
			const LandDecorationStruct &decoration = decorations[d];
			Matrix3D tm = decoration.Transform;
			Vector3 pos = tm.Get_Translation();
			float gi, gj;
			Field->World_To_Grid(Vector2(pos.X, pos.Y), &gi, &gj);
			int patch = GridWidth * ((int)floor(gj / (float)(unsigned int)GridWidth) & GridMask)
				+ ((int)floor(gi / (float)(unsigned int)GridWidth) & GridMask);
			if (Unknown190 && trees[decoration.Proto] != NULL) {
				unsigned short largest = Land_Append_Decoration_Mesh(trees[decoration.Proto], tree_vertices[patch], tree_indices[patch],
					tree_counts[patch], tm, decoration.Scale);
				tree_counts[patch] += largest + 1;
			}
			if (Unknown191 && shadows[decoration.Proto] != NULL) {
				Matrix3D shadow = tm;
				Vector3 normal;
				Field->Get_Normal(&normal, Vector2(pos.X, pos.Y));
				Vector3 translation = pos + normal * 3.0f;
				Vector3 z(tm[0][2], tm[1][2], tm[2][2]);
				Vector3 axis;
				Vector3::Cross_Product(z, normal, &axis);
				if (axis.Length2() > 0.0001f) {
					float half = (float)acos(Vector3::Dot_Product(z, normal)) * 0.5f;
					float w = (float)cos(half);
					Vector3 q = axis * (float)sin(half);
					for (int c = 0; c < 3; c++) {
						Vector3 col(tm[0][c], tm[1][c], tm[2][c]);
						Vector3 t;
						Vector3::Cross_Product(q, col, &t);
						t += col * w;
						float tw = -Vector3::Dot_Product(q, col);
						Vector3 x;
						Vector3::Cross_Product(t, -q, &x);
						Vector3 r = (-q) * tw + t * w + x;
						shadow[0][c] = r.X;
						shadow[1][c] = r.Y;
						shadow[2][c] = r.Z;
					}
				}
				shadow.Set_Translation(translation);
				unsigned short largest = Land_Append_Decoration_Mesh(shadows[decoration.Proto], shadow_vertices[patch], shadow_indices[patch],
					shadow_counts[patch], shadow, decoration.Scale);
				shadow_counts[patch] += largest + 1;
			}
		}
		for (int p = 0; p < PatchCount; p++) {
			LandPatchRecordClass &record = PatchRecords[p];
			for (int pass = 0; pass < 2; pass++) {
				std::vector<LandDecorationVertexStruct> &verts = pass ? shadow_vertices[p] : tree_vertices[p];
				std::vector<unsigned short> &idx = pass ? shadow_indices[p] : tree_indices[p];
				RefCountPtr<RefCountClass> &vb = pass ? record.Unknown08 : record.Unknown00;
				RefCountPtr<RefCountClass> &ib = pass ? record.Unknown0C : record.Unknown04;
				int &vertex_count = pass ? record.Unknown18 : record.Unknown10;
				int &index_count = pass ? record.Unknown1C : record.Unknown14;
				int nv = verts.size();
				int ni = idx.size();
				if (nv < 1) {
					vertex_count = 0;
					index_count = 0;
					vb = NULL;
					ib = NULL;
					continue;
				}
				vertex_count = nv;
				index_count = ni;
				DX8VertexBufferClass *vertex_buffer = new DX8VertexBufferClass(0x112, (unsigned short)nv);
				vb = Create_NEW(vertex_buffer);
				DX8IndexBufferClass *index_buffer = new DX8IndexBufferClass((unsigned short)ni);
				ib = Create_NEW(index_buffer);
				{
					VertexBufferClass::WriteLockClass lock(vertex_buffer);
					LandDecorationVertexStruct *out = (LandDecorationVertexStruct *)lock.Get_Vertex_Array();
					for (int v = 0; v < nv; v++) {
						out[v] = verts[v];
					}
				}
				{
					IndexBufferClass::WriteLockClass lock(index_buffer);
					unsigned short *out = lock.Get_Index_Array();
					for (int n = 0; n < ni; n++) {
						out[n] = idx[n];
					}
				}
			}
		}
		delete [] tree_vertices;
		delete [] tree_indices;
		delete [] shadow_vertices;
		delete [] shadow_indices;
		delete [] tree_counts;
		delete [] shadow_counts;
	}
	for (i = 0; i < (int)trees.size(); i++) {
		if (trees[i] != NULL) trees[i]->Release_Ref();
		if (shadows[i] != NULL) shadows[i]->Release_Ref();
	}
}

// The decorations as the .dec file: the prototype names (32 bytes each), then per decoration its record (the translation through
// Get_Translation's out-parameter form, as the client's frame shows)
void LandClass::Save_Decorations(std::vector<std::string> &names, std::vector<LandDecorationStruct> &decorations, const char *filename)
{
	unsigned int count = names.size();
	unsigned int decoration_count = decorations.size();
	if (count == 0 || decoration_count == 0 || count > 255) {
		return;
	}
	file_auto_ptr file(_TheWritingFileFactory, filename);
	file->Open(FileClass::WRITE);
	unsigned int id = LAND_DECORATION_ID;
	file->Write(&id, 4);
	file->Write(&count, 4);
	for (id = 0; id < count; id++) {
		char name[32];
		memset(name, 0, sizeof(name));
		Unknown_4A1590(name, sizeof(name), names[id].c_str());
		file->Write(name, sizeof(name));
	}
	file->Write(&decoration_count, 4);
	for (id = 0; id < decoration_count; id++) {
		LandDecorationRecordStruct record;
		record.Proto = (unsigned char)decorations[id].Proto;
		Quaternion rotation = Build_Quaternion(decorations[id].Transform);
		Vector3 pos;
		decorations[id].Transform.Get_Translation(&pos);
		record.Rotation = rotation;
		record.X = pos.X;
		record.Y = pos.Y;
		record.Scale = decorations[id].Scale;
		file->Write(&record, sizeof(record));
	}
	file->Close();
}

// NOT EXACT: frame (0x14, the client 0x20: it keeps this, the size and the buffer in frame slots); 2 forms tried
void LandClass::Upload_Lightmap(Vector2i &min, Vector2i &max)
{
	SurfaceClass *surface = Unknown120->Get_Surface_Level(0);
	if (Is16Bit) {
		unsigned char *bytes = (unsigned char *)Unknown120C;
		unsigned short *words = (unsigned short *)Unknown120C;
		int size = Field->Get_Size();
		for (int j = min.J; j < max.J; j++) {
			for (int i = min.I; i < max.I; i++) {
				int n = j * size + i;
				words[n] = (unsigned short)((((unsigned short)(bytes[n * 4 + 3] & 0xF0) << 4) | (unsigned short)(bytes[n * 4 + 2] & 0xF0)) << 4)
					| (unsigned short)(unsigned char)((bytes[n * 4 + 1] & 0xF0) | (bytes[n * 4] >> 4));
			}
		}
	}
	surface->Copy(min, max, (unsigned char *)Unknown120C);
	DX8Wrapper::_Update_Texture(Unknown120, LightmapTextures[BackLightmap]);
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

// NOT EXACT: order-only (7 masked rows: the client sets xi = x after the inner loop's Size test, ours before; the slots of inv_size / v and i / xi swapped; attempts.csv 0x474790)
void LandPatchClass::Build_Vertices(int x, int y, float detail_u, float detail_v)
{
	float lowest = 2.0f;
	float highest = -1.0f;
	float inv_size = 1.0f / (float)(unsigned int)Land->Field->Get_Size();
	float vscale = Land->Field->Get_VScale();
	float height_scale = 1.0f / vscale;
	{
		VertexBufferClass::WriteLockClass lock(VertexBuffer);
		LandVertexStruct *vertex = (LandVertexStruct *)lock.Get_Vertex_Array();
		for (int j = 0; j < Size; j++) {
			int xi = x;
			for (int i = 0; i < Size; i++) {
				vertex->X = (float)i;
				vertex->Y = (float)j;
				float height = Land->Field->Get_Height_2(xi, j + y) * height_scale;
				if (lowest >= height) lowest = height;
				if (highest <= height) highest = height;
				vertex->Z = height;
				float v = (float)(j + y) * inv_size;
				vertex->V1 = v;
				vertex->V2 = v * detail_v;
				float u = (float)xi * inv_size;
				vertex->U1 = u;
				vertex->U2 = u * detail_u;
				vertex++;
				xi++;
			}
		}
	}
	MidHeight = (highest + lowest) * 0.5f;
}

void LandPatchClass::Draw_Strip(void)
{
	DX8Wrapper::Set_Vertex_Buffer(VertexBuffer);
	DX8Wrapper::Draw_Strip(0, (unsigned short)Land->StripCounts[Level], 0, (unsigned short)VertexCount);
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
	SkyColorDX8 = DX8Wrapper::Convert_Color(Lighting.SkyColor, 0.0f);
	if (Lighting.Valid) {
		static const float max_angle = WWMATH_PI / 2.0f;
		float scale = 65535.0f / max_angle;
		float angle = Lighting.SunAngle;
		SunFromWest = Lighting.SunDirection.X < 0.0f;
		float low = WWMath::Clamp(angle - Lighting.ShadowBias, 0.0f, max_angle);
		float high = WWMath::Clamp(angle + Lighting.ShadowBias, 0.0f, max_angle);
		HorizonLow = (unsigned short)(low * scale);
		HorizonHigh = (unsigned short)(high * scale);
		ShadowStep = (float)(1.0 / (ceil(scale * Lighting.ShadowBias) * 2.0));
		for (int i = 0; i < 256; i++) {
			const Vector3 &n = Normals[i];
			float d = n.X * Lighting.SunDirection.X + n.Z * Lighting.SunDirection.Z + Lighting.SunDirection.Y * n.Y;
			Vector3 color;
			if (d > 0.0f) {
				color = Lighting.LightColor * d;
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
	if (Lighting.Valid) {
		float step = ShadowStep;
		unsigned short low = HorizonLow;
		unsigned short high = HorizonHigh;
		unsigned short horizon;
		if (SunFromWest) {
		for (j = UpdateMin.J; j < (unsigned int)UpdateMax.J; j++) {
			for (i = UpdateMin.I; i < (unsigned int)UpdateMax.I; i++) {
				n = Field->Get_Size() * j + i;
				horizon = Unknown1E8[n];
				if (low > horizon) {
					Unknown120C[n] = NormalColors[Unknown1E4[n]];
					color = (unsigned char *)&Unknown120C[n];
					r = (unsigned char)((Unknown1F0[n] * sky[2]) >> 8) + color[2];
					g = (unsigned char)((Unknown1F4[n] * sky[1]) >> 8) + color[1];
					b = (unsigned char)((Unknown1F8[n] * sky[0]) >> 8) + color[0];
					if (r > 255) r = 255;
					if (g > 255) g = 255;
					if (b > 255) b = 255;
					color[0] = (unsigned char)b;
					color[1] = (unsigned char)g;
					color[2] = (unsigned char)r;
					color[3] = 0xFF;
				} else if (high > horizon) {
					float t = (float)(int)(high - horizon) * step;
					Unknown120C[n] = NormalColors[Unknown1E4[n]];
					color = (unsigned char *)&Unknown120C[n];
					r = (unsigned int)((float)(unsigned char)((Unknown1F0[n] * sky[2]) >> 8) + (float)color[2] * t);
					g = (unsigned int)((float)(unsigned char)((Unknown1F4[n] * sky[1]) >> 8) + (float)color[1] * t);
					b = (unsigned int)((float)(unsigned char)((Unknown1F8[n] * sky[0]) >> 8) + (float)color[0] * t);
					if (r > 255) r = 255;
					if (g > 255) g = 255;
					if (b > 255) b = 255;
					color[0] = (unsigned char)b;
					color[1] = (unsigned char)g;
					color[2] = (unsigned char)r;
					color[3] = 0xFF;
				} else {
					Unknown120C[n] = SkyColorDX8;
					color = (unsigned char *)&Unknown120C[n];
					unsigned int f1 = Unknown1F4[n];
					unsigned int f2 = Unknown1F8[n];
					color[2] = (unsigned char)((color[2] * Unknown1F0[n]) >> 8);
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
				horizon = Unknown1EC[n];
				if (low > horizon) {
					Unknown120C[n] = NormalColors[Unknown1E4[n]];
					color = (unsigned char *)&Unknown120C[n];
					r = (unsigned char)((Unknown1F0[n] * sky[2]) >> 8) + color[2];
					g = (unsigned char)((Unknown1F4[n] * sky[1]) >> 8) + color[1];
					b = (unsigned char)((Unknown1F8[n] * sky[0]) >> 8) + color[0];
					if (r > 255) r = 255;
					if (g > 255) g = 255;
					if (b > 255) b = 255;
					color[0] = (unsigned char)b;
					color[1] = (unsigned char)g;
					color[2] = (unsigned char)r;
					color[3] = 0xFF;
				} else if (high > horizon) {
					float t = (float)(int)(high - horizon) * step;
					Unknown120C[n] = NormalColors[Unknown1E4[n]];
					color = (unsigned char *)&Unknown120C[n];
					r = (unsigned int)((float)(unsigned char)((Unknown1F0[n] * sky[2]) >> 8) + (float)color[2] * t);
					g = (unsigned int)((float)(unsigned char)((Unknown1F4[n] * sky[1]) >> 8) + (float)color[1] * t);
					b = (unsigned int)((float)(unsigned char)((Unknown1F8[n] * sky[0]) >> 8) + (float)color[0] * t);
					if (r > 255) r = 255;
					if (g > 255) g = 255;
					if (b > 255) b = 255;
					color[0] = (unsigned char)b;
					color[1] = (unsigned char)g;
					color[2] = (unsigned char)r;
					color[3] = 0xFF;
				} else {
					Unknown120C[n] = SkyColorDX8;
					color = (unsigned char *)&Unknown120C[n];
					unsigned int f1 = Unknown1F4[n];
					unsigned int f2 = Unknown1F8[n];
					color[2] = (unsigned char)((color[2] * Unknown1F0[n]) >> 8);
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
				Unknown120C[n] = SkyColorDX8;
				color = (unsigned char *)&Unknown120C[n];
				unsigned int f1 = Unknown1F4[n];
				unsigned int f2 = Unknown1F8[n];
				unsigned int f0 = Unknown1F0[n];
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
	unsigned char *lightmap = (unsigned char *)Unknown120C;
	unsigned char *colors = Colors;
	unsigned char *extra = Unknown1214;
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

// The strips run forward and back on alternate rows (an int flag); the edge buffers' fans per side
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
			int forward = 1;
			for (int row = step; row < size; row += step) {
				if (forward) {
					for (int col = 0; col < size - step; col += step) {
						*index++ = row * size + col;
						*index++ = col + (row - step) * size;
					}
					*index++ = (row + 1) * size - 1;
					*index++ = (row - step + 1) * size - 1;
					*index++ = (row + 1) * size - 1;
					*index++ = (row + 1) * size - 1;
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
			int i;
			for (i = step; i <= size; i += step) {
				*index++ = (i - step) * size;
				*index++ = i * size;
				*index++ = (i - (step >> 1)) * size;
			}
			for (i = step; i < size; i += step) {
				*index++ = (i - step + 1) * size - 1;
				*index++ = (i - (step >> 1) + 1) * size - 1;
				*index++ = (i + 1) * size - 1;
			}
			for (i = step; i < size; i += step) {
				*index++ = i - step;
				*index++ = i - (step >> 1);
				*index++ = i;
			}
			for (i = step; i < size; i += step) {
				*index++ = (size - 1) * size - step + i;
				*index++ = i + (size - 1) * size;
				*index++ = i + ((size - 1) * size - (step >> 1));
			}
		}
	} while (level-- != 0);
}

// NAME 2: the eight compass directions' unit vectors x (1 - 1/sqrt 2) mixes, per direction (0xAE97BC, 0xAE97DC)
static const float LandDirectionX[8] = { 0.70710677f, 0.29289323f, -0.29289323f, -0.70710677f, -0.70710677f, -0.29289323f, 0.29289323f, 0.70710677f };
static const float LandDirectionY[8] = { 0.29289323f, 0.70710677f, 0.70710677f, 0.29289323f, -0.29289323f, -0.70710677f, -0.70710677f, -0.29289323f };

// NAME 2: a horizon slope as the high bits of its float, saturating, and back (0x46EE60 / 0x46EEB0: 8 bits; 0x46EEE0 / 0x46EF30:
// 16 bits). Inlined everywhere; the client keeps their unreferenced out-of-line copies
unsigned char Pack_Slope8(float slope)
{
	unsigned char code;
	if (slope >= 248.0f) {
		code = 0x7F;
	} else if (slope <= 0.00390625f) {
		code = 0;
	} else {
		code = (unsigned char)((*(unsigned int *)&slope >> 19) - 0x770);
	}
	return code;
}

float Unpack_Slope8(unsigned char code)
{
	unsigned int bits = (code + 0x770) << 19 | 0x7FFFF;
	return *(float *)&bits;
}

unsigned short Pack_Slope16(float slope)
{
	if (slope >= 255.96875f) {
		return 0x7FFF;
	}
	if (slope <= 0.00390625f) {
		return 0;
	}
	return (unsigned short)((*(unsigned int *)&slope >> 11) - 0x7000);
}

float Unpack_Slope16(unsigned short code)
{
	unsigned int bits = (code + 0x77000) << 11 | 0x7FF;
	return *(float *)&bits;
}

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

// The casters into the height field: each vertex under a caster's box is raised to where a ray straight down from the box's top hits it.
// The client passes the corner Vector3 itself as World_To_Grid's Vector2 (its X, Y)
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
		if (top <= corner.Z) {
			top = corner.Z;
		}
		float x0, y0;
		Field->World_To_Grid((const Vector2 &)corner, &x0, &y0);
		corner = box.Center + box.Extent;
		if (top <= corner.Z) {
			top = corner.Z;
		}
		float x1, y1;
		Field->World_To_Grid((const Vector2 &)corner, &x1, &y1);
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
					CastResultStruct result;
					result.ComputeContactPoint = true;
					Vector3 p0 = vertex;
					p0.Z = top;
					LineSegClass ray(p0, vertex);
					RayCollisionTestClass raytest(ray, &result, COLLISION_TYPE_ALL);
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

// The index of the table normal nearest a vertex normal (largest dot product, first wins ties); 0x46F120 inlines it eight times
unsigned char LandClass::Nearest_Normal(const Vector3 &n)
{
	unsigned char best_index = 0;
	float best = -1.0f;
	for (int k = 0; k < 256; k++) {
		const Vector3 &N = Normals[k];
		float dot = N.X * n.X + N.Z * n.Z + N.Y * n.Y;
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
	// around it, the client's -1 / +1 neighbours wrap through Get_Height_2's mask), matched against Normals into Unknown1E4.
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
			Unknown1E4[index] = Nearest_Normal(n);
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
			Unknown1E4[index] = Nearest_Normal(n);
			n = Vector3::Cross_Product(v20, v21) + Vector3::Cross_Product(v21, v22);
			n.Normalize();
			Unknown1E4[index + 1] = Nearest_Normal(n);
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
			Unknown1E4[index] = Nearest_Normal(n);
			n = Vector3::Cross_Product(v12, v02) + Vector3::Cross_Product(v22, v12);
			n.Normalize();
			Unknown1E4[index + size] = Nearest_Normal(n);
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
			Unknown1E4[index] = Nearest_Normal(n);
			n = Vector3::Cross_Product(v20, v21) + Vector3::Cross_Product(v10, v20);
			n.Normalize();
			Unknown1E4[index + 1] = Nearest_Normal(n);
			n = Vector3::Cross_Product(v02, v01) + Vector3::Cross_Product(v12, v02);
			n.Normalize();
			Unknown1E4[index + size] = Nearest_Normal(n);
			n = Vector3::Cross_Product(v22, v12) + Vector3::Cross_Product(v21, v22);
			n.Normalize();
			Unknown1E4[index + size + 1] = Nearest_Normal(n);
			index += 2;
		}
		index += size + 1;
	}
}

void LandClass::Get_Vertex_Color(unsigned int x, unsigned int y, Vector3 &color)
{
	unsigned char *rgb = &Colors[((y << Field->Get_Log2()) + x) * 3];
	color.Set((float)rgb[2] * (1.0f / 255.0f), (float)rgb[1] * (1.0f / 255.0f), (float)rgb[0] * (1.0f / 255.0f));
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
	Unknown1E8[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_4);
	bits = (avg[1] + 0x77000) << 11 | 0x7FF;
	Unknown1EC[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_4);
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
		Unknown_472F80(&Unknown1F0[n], &Unknown1F4[n], &Unknown1F8[n], h, Unknown1E4[n], color);
	}
	n++;
	for (x = 1; x < last; x++) {
		k0 = (0 << Field->Get_Log2()) + x;
		k1 = (mask << Field->Get_Log2()) + x;
		avg[0] = (unsigned short)((hz[k0 * 2 + 0] + hz[k1 * 2 + 0]) >> 1);
		avg[1] = (unsigned short)((hz[k0 * 2 + 1] + hz[k1 * 2 + 1]) >> 1);
		bits = (avg[0] + 0x77000) << 11 | 0x7FF;
		Unknown1E8[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_2);
		bits = (avg[1] + 0x77000) << 11 | 0x7FF;
		Unknown1EC[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_2);
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
			Unknown_472F80(&Unknown1F0[n], &Unknown1F4[n], &Unknown1F8[n], h, Unknown1E4[n], color);
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
	Unknown1E8[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_4);
	bits = (avg[1] + 0x77000) << 11 | 0x7FF;
	Unknown1EC[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_4);
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
		Unknown_472F80(&Unknown1F0[n], &Unknown1F4[n], &Unknown1F8[n], h, Unknown1E4[n], color);
	}
	n++;
	// the inner rows: the first and last columns average with each other, the rest is the cell's own
	for (y = 1; y < last; y++) {
		k0 = y << Field->Get_Log2();
		k1 = k0 + mask;
		avg[0] = (unsigned short)((hz[k0 * 2 + 0] + hz[k1 * 2 + 0]) >> 1);
		avg[1] = (unsigned short)((hz[k0 * 2 + 1] + hz[k1 * 2 + 1]) >> 1);
		bits = (avg[0] + 0x77000) << 11 | 0x7FF;
		Unknown1E8[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_2);
		bits = (avg[1] + 0x77000) << 11 | 0x7FF;
		Unknown1EC[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_2);
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
			Unknown_472F80(&Unknown1F0[n], &Unknown1F4[n], &Unknown1F8[n], h, Unknown1E4[n], color);
		}
		n++;
		for (x = 1; x < last; x++) {
			k0 = (y << Field->Get_Log2()) + x;
			avg[0] = hz[k0 * 2 + 0];
			avg[1] = hz[k0 * 2 + 1];
			bits = (avg[0] + 0x77000) << 11 | 0x7FF;
			Unknown1E8[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_1);
			bits = (avg[1] + 0x77000) << 11 | 0x7FF;
			Unknown1EC[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_1);
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
				Unknown_472F80(&Unknown1F0[n], &Unknown1F4[n], &Unknown1F8[n], h, Unknown1E4[n], color);
			}
			n++;
		}
		k0 = (y << Field->Get_Log2()) + wrap;
		k1 = (y << Field->Get_Log2()) + last;
		avg[0] = (unsigned short)((hz[k0 * 2 + 0] + hz[k1 * 2 + 0]) >> 1);
		avg[1] = (unsigned short)((hz[k0 * 2 + 1] + hz[k1 * 2 + 1]) >> 1);
		bits = (avg[0] + 0x77000) << 11 | 0x7FF;
		Unknown1E8[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_2);
		bits = (avg[1] + 0x77000) << 11 | 0x7FF;
		Unknown1EC[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_2);
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
			Unknown_472F80(&Unknown1F0[n], &Unknown1F4[n], &Unknown1F8[n], h, Unknown1E4[n], color);
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
	Unknown1E8[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_4);
	bits = (avg[1] + 0x77000) << 11 | 0x7FF;
	Unknown1EC[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_4);
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
		Unknown_472F80(&Unknown1F0[n], &Unknown1F4[n], &Unknown1F8[n], h, Unknown1E4[n], color);
	}
	n++;
	for (x = 1; x < last; x++) {
		k0 = (last_row << Field->Get_Log2()) + x;
		k1 = (wrap << Field->Get_Log2()) + x;
		avg[0] = (unsigned short)((hz[k0 * 2 + 0] + hz[k1 * 2 + 0]) >> 1);
		avg[1] = (unsigned short)((hz[k0 * 2 + 1] + hz[k1 * 2 + 1]) >> 1);
		bits = (avg[0] + 0x77000) << 11 | 0x7FF;
		Unknown1E8[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_2);
		bits = (avg[1] + 0x77000) << 11 | 0x7FF;
		Unknown1EC[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_2);
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
			Unknown_472F80(&Unknown1F0[n], &Unknown1F4[n], &Unknown1F8[n], h, Unknown1E4[n], color);
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
	Unknown1E8[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_4);
	bits = (avg[1] + 0x77000) << 11 | 0x7FF;
	Unknown1EC[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_4);
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
		Unknown_472F80(&Unknown1F0[n], &Unknown1F4[n], &Unknown1F8[n], h, Unknown1E4[n], color);
	}
	n++;
}

// NOT EXACT: literal (14 masked rows: the direction loop's x87 stack, the client copies nz where ours copies w.Y and keeps w.Y one step longer; attempts.csv 0x472F80)
void LandClass::Unknown_472F80(unsigned char *red, unsigned char *green, unsigned char *blue,
	unsigned char *horizons, unsigned char normal, const Vector3 &color)
{
	// 1 / sin(67.5), 1 / sin(22.5) degrees: per direction 2..5, the step's length over the cell
	float scales[4] = { 1.0823922f, 2.613126f, 2.613126f, 1.0823922f };
	const Vector3 &n = Normals[normal];
	float nx = n.X;
	float ny = n.Y;
	float nz = n.Z;
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
		Vector3 w(LandDirectionX[i + 2], LandDirectionY[i + 2], 0.0f);
		Vector3 c;
		c.X = nz * w.Y;
		c.Z = w.X * ny - w.Y * nx;
		float d = c.Z;
		int dir = i;
		if (d <= 0.0f) {
			dir = i + 4;
		}
		float t = (float)fabs(d) / ((float)fabs(c.X) * scales[i]);
		unsigned char code = Pack_Slope8(t);
		dirs[dir] = (dirs[dir] > code) ? dirs[dir] : code;
	}
	// the sky seen over the eight horizons, cosine-weighted by the normal
	float sx = 0.0f;
	float sy = 0.0f;
	float sz = 0.0f;
	for (i = 0; i < 8; i++) {
		const Vector3 &h = LandHorizonTable[i][dirs[i]];
		sx += h.X;
		sy += h.Y;
		sz += h.Z;
	}
	float sky = sx * nx + (sy * ny + sz * nz);	// the client adds y and z first
	// bounced light: the share grows with the ground's colour where the sky is hidden
	float r = sky / (1.0f - (1.0f - sky) * color.X);
	float g = sky / (1.0f - (1.0f - sky) * color.Y);
	float b = sky / (1.0f - (1.0f - sky) * color.Z);
	// at least 1 / 512 (a double), then 0..1 to a byte as 256 x - 1: the low byte of x + 32768 as a float
	float mr = r > 0.00195314 ? r : 0.00195314;
	float mg = g > 0.00195314 ? g : 0.00195314;
	float mb = b > 0.00195314 ? b : 0.00195314;
	float fr = mr + 32768.0f;
	*red = (unsigned char)(*(unsigned char *)&fr - 1);
	float fg = mg + 32768.0f;
	*green = (unsigned char)(*(unsigned char *)&fg - 1);
	float fb = mb + 32768.0f;
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
			Unknown1E8[i] = 0xFFFF;
			Unknown1EC[i] = 0xFFFF;
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

// NOT EXACT: shape (119 masked rows, 54 with registers masked; the client's frame since the Vector3 best / h / s; attempts.csv 0x473780)
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
		Vector3 best(0.05f, 0.05f, 0.05f);
		const float *recip = Reciprocals;
		float dist = 1.0f;
		unsigned int y = next;
		unsigned int offset = row1;
		unsigned int right = i1;
		unsigned int left = i;
		for (int k = 0; k < 12; k++) {
			Vector3 h;
			h.X = heights[(left & Field->Get_Mask()) + offset];
			h.Y = ((float)heights[offset + i1] + heights[i + offset]) * 0.5f;
			h.Z = heights[(right & Field->Get_Mask()) + offset];
			Vector3 s;
			s.X = (h.X - center) * diagonal;
			s.X *= *recip;
			s.Y = (h.Y - center) * scale;
			s.Y *= *recip;
			s.Z = (h.Z - center) * diagonal;
			s.Z *= *recip;
			if (s.X > best.X) {
				best.X = s.X;
			}
			if (s.Y > best.Y) {
				best.Y = s.Y;
			}
			if (s.Z > best.Z) {
				best.Z = s.Z;
			}
			y = (y + 1) & Field->Get_Mask();
			recip += 2;
			left--;
			right++;
			offset = (offset + Field->Get_Size()) & Field->Get_Count_Mask();
			dist += 2.0f;
		}
		out[0] = Pack_Slope8(best.X);
		out[1] = Pack_Slope8(best.Y);
		out[2] = Pack_Slope8(best.Z);
		out += 3;
	}
}

// NOT EXACT: shape (26 masked rows: the outer loop's set-up order and the left index's register; the client's frame since the Vector3 best / h / s; attempts.csv 0x473BC0)
void LandClass::Unknown_473BC0(int row, float ratio, unsigned char *out)
{
	unsigned int row0 = (Field->Get_Mask() & row) << Field->Get_Log2();
	float scale = ratio + ratio;
	float diagonal = scale * 0.70710677f;
	unsigned short *heights = Field->Get_Heights();
	unsigned int row1 = ((row + 1) & Field->Get_Mask()) << Field->Get_Log2();
	for (unsigned int i = 0; i < Field->Get_Size(); i++) {
		unsigned int i1 = (i + 1) & Field->Get_Mask();
		float center = ((float)heights[row1 + i1] + heights[i1 + row0] + heights[row1 + i] + heights[row0 + i]) * 0.25f;
		Vector3 best(0.05f, 0.05f, 0.05f);
		const float *recip = Reciprocals;
		float dist = 1.0f;
		unsigned int y = row;
		unsigned int offset = row0;
		unsigned int right = i1;
		unsigned int left = i;
		for (int k = 0; k < 12; k++) {
			Vector3 h;
			h.X = heights[(left & Field->Get_Mask()) + offset];
			h.Y = ((float)heights[offset + i1] + heights[i + offset]) * 0.5f;
			h.Z = heights[(right & Field->Get_Mask()) + offset];
			Vector3 s;
			s.X = (h.X - center) * diagonal;
			s.X *= *recip;
			s.Y = (h.Y - center) * scale;
			s.Y *= *recip;
			s.Z = (h.Z - center) * diagonal;
			s.Z *= *recip;
			if (s.X > best.X) {
				best.X = s.X;
			}
			if (s.Y > best.Y) {
				best.Y = s.Y;
			}
			if (s.Z > best.Z) {
				best.Z = s.Z;
			}
			y = (y - 1) & Field->Get_Mask();
			recip += 2;
			left--;
			right++;
			offset = (offset - Field->Get_Size()) & Field->Get_Count_Mask();
			dist += 2.0f;
		}
		out[0] = Pack_Slope8(best.X);
		out[1] = Pack_Slope8(best.Y);
		out[2] = Pack_Slope8(best.Z);
		out += 3;
	}
}

bool LandClass::Load_PLD(const char *filename)
{
	FILE *file = fopen(filename, "rb");
	if (file != NULL) {
		unsigned int count = 0;
		if (fread(&count, 4, 1, file) == 1 && count == Field->Get_Count()
			&& fread(Unknown1E4, 1, count, file) == count
			&& fread(Unknown1E8, 2, count, file) == count
			&& fread(Unknown1EC, 2, count, file) == count
			&& fread(Unknown1F0, 1, count, file) == count
			&& fread(Unknown1F4, 1, count, file) == count
			&& fread(Unknown1F8, 1, count, file) == count
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
			&& fwrite(Unknown1E4, 1, count, file) == count
			&& fwrite(Unknown1E8, 2, count, file) == count
			&& fwrite(Unknown1EC, 2, count, file) == count
			&& fwrite(Unknown1F0, 1, count, file) == count
			&& fwrite(Unknown1F4, 1, count, file) == count
			&& fwrite(Unknown1F8, 1, count, file) == count) {
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
**	Compute_Horizons 0x46E670, Update_Patches, the tessellation and Render 0x474B30 are written too
**	(some NOT EXACT, marked); VIDTEST draws its land with this class since 2026-10-28 (D101), and its
**	trees: Create_Decorations 0x479D00 (VIDTEST 0x415C40) and Build_Decorations 0x47A2F0 (= VIDTEST 0x416170)
**	fill the patch records that Render draws (both functional; drawcheck frame 900 the original's draws).
**	The zone loader (mainview 0x5137F0) reads land_%04d.dcb with Load_Decorations 0x4795D0 and builds
**	from it, or makes the decorations from decoration.ini without one; Save_Decorations 0x479A50 writes
**	it and Load_Prototype_Names 0x478970 reads an ini section's names (nothing calls either). The same
**	as an ini ([Prototypes], [DECO%d] DecID, M, Scale): the reader 0x478B50 and the writer 0x4791D0
**	(NOT EXACT, marked; nothing calls either).
*/
