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
#include <queue>
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

bool LandClass::Save_Sea_Mask(const char *filename)
{
	bool has_sea = HasSea;
	if (!has_sea) {
		return has_sea;
	}
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

// NOT EXACT: register-only by eye, explain.py says field-offset (58 masked rows, 22 with the registers masked: in the second
// neighbour ecx / edx are swapped, in the third the client keeps cell.I in ebp across the push). A std::queue and the
// patches indexed at each use give the client's frame 0x7C and inlining: the seed push_back 0x46BF70 out of line, the
// queue's first iterator constructor called and the second inlined, _Freefront inlined (attempts.csv 0x46AA40)
void LandClass::Limit_Levels(int x, int y)
{
	std::queue<Vector2i> queue;
	queue.push(Vector2i(x, y));
	Vector2i next;
	while (!queue.empty()) {
		Vector2i &cell = queue.front();
		if (Patches[cell.J * GridWidth + cell.I]->Visited) {
			queue.pop();
		} else {
			Patches[cell.J * GridWidth + cell.I]->Visited = true;
			unsigned int level = Patches[cell.J * GridWidth + cell.I]->Level + 1;
			if (level > 4) {
				level = 4;
			}
			next.J = cell.J;
			next.I = cell.I + 1;
			if (next.I < GridWidth && !Patches[next.J * GridWidth + next.I]->Visited) {
				queue.push(next);
				if ((unsigned int)Patches[next.J * GridWidth + next.I]->Level > level) {
					Patches[next.J * GridWidth + next.I]->Level = level;
				}
			}
			next.I = cell.I - 1;
			if (next.I >= 0 && !Patches[next.J * GridWidth + next.I]->Visited) {
				queue.push(next);
				if ((unsigned int)Patches[next.J * GridWidth + next.I]->Level > level) {
					Patches[next.J * GridWidth + next.I]->Level = level;
				}
			}
			next.I = cell.I;
			next.J = cell.J + 1;
			if (next.J < GridWidth && !Patches[next.J * GridWidth + next.I]->Visited) {
				queue.push(next);
				if ((unsigned int)Patches[next.J * GridWidth + next.I]->Level > level) {
					Patches[next.J * GridWidth + next.I]->Level = level;
				}
			}
			next.J = cell.J - 1;
			if (next.J >= 0 && !Patches[next.J * GridWidth + next.I]->Visited) {
				queue.push(next);
				if ((unsigned int)Patches[next.J * GridWidth + next.I]->Level > level) {
					Patches[next.J * GridWidth + next.I]->Level = level;
				}
			}
			queue.pop();
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

// NOT EXACT: shape (199 masked rows: the client's frame is 0xC larger, it copies the camera position and keeps the spacing on the x87 stack past its use (fild qword PatchSize, fmul st(1), fstp st(0)); attempts.csv 0x46B3C0)
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
	if ((Dirty == true) | (CameraCellX ^ cell_x) | (cell_z ^ CameraCellZ) | (cell_y ^ CameraCellY)) {
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
**	it and Load_Prototype_Names 0x478970 reads an ini section's names (nothing calls either; the reader inlines the second). The same
**	as an ini ([Prototypes], [DECO%d] DecID, M, Scale): the reader 0x478B50 and the writer 0x4791D0
**	(NOT EXACT, marked; nothing calls either).
**
**	The client builds this code as three objects (their COMDATs close each: LandClass's inline virtuals and the
**	deque's at 0x46BEC0-0x46C6E0 after Save_Sea_Mask, the RefCountPtr<RenderObjClass> vector's at 0x474000-0x4744C0
**	before LandPatchClass 0x474530, the decorations' from 0x47CCE0; Load_Assets calls the patch's constructor and
**	Init out of line): this file is the first (0x468E90-0x46C730), unknown_46c730.cpp the lighting and horizons
**	(0x46C730-0x4744C0), unknown_474530.cpp the patches, Render and the decorations (0x474530-0x47F520). The two
**	file names are interim (D137).
*/
