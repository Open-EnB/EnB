// Earth & Beyond client (reconstruction): VIDTEST's stand-in for the land (vidland.h). Notes at the end.
// NOT EXACT, the whole file: functional, after VIDTEST's 2002 copy (land.h).
#include "vidland.h"
#include "heightfield.h"
#include "ini.h"
#include "TARGA.H"
#include "texture.h"
#include "surfaceclass.h"
#include "assetmgr.h"
#include "vertmaterial.h"
#include "dx8wrapper.h"
#include "dx8vertexbuffer.h"
#include "dx8indexbuffer.h"
#include "dx8fvf.h"
#include "camera.h"
#include "rinfo.h"
#include "random.h"
#include "matrix3d.h"
#include "ffactory.h"
#include "wwfile.h"
#include "mesh.h"
#include "meshmdl.h"
#include "wwmath.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

/*
** One vertex of the land: position, the lightmap's uv, the detail map's uv (DX8_FVF_XYZUV2)
*/
struct LandVertexStruct	// NAME 2: ours
{
	float	X, Y, Z;
	float	U1, V1;
	float	U2, V2;
};

static const float SKIRT_DEPTH = 400.0f;

static inline int Int_Min(int a, int b) { return a < b ? a : b; }	// NAME 2: ours
static inline int Int_Max(int a, int b) { return a > b ? a : b; }

VidtestLandClass::VidtestLandClass(void) :
	Field(NULL),
	HasSea(false),
	Flag(false),
	Dirty(true),
	PatchGridWidth(16),
	LightmapChunk(8),
	PatchCount(0),
	DetailScaleX(1.0f),
	DetailScaleY(1.0f),
	Colors(NULL),
	Lightmap(NULL),
	HorizonEast(NULL),
	HorizonWest(NULL),
	Normals(NULL),
	SkyWeights(NULL),
	LightmapRow(0),
	LightmapTexture(NULL),
	DetailTexture(NULL),
	Material(NULL),
	IndexBuffer(NULL),
	PatchLevel(NULL),
	CurrentPolys(0)
{
	for (int i = 0; i < MAX_LEVELS; i++) {
		LevelStart[i] = 0;
		LevelPolys[i] = 0;
	}
	Set_Force_Visible(true);
}

VidtestLandClass::~VidtestLandClass(void)
{
	for (int i = 0; i < DecoBatches.Count(); i++) {
		DecoBatchStruct *batch = DecoBatches[i];
		REF_PTR_RELEASE(batch->Texture);
		REF_PTR_RELEASE(batch->Material);
		REF_PTR_RELEASE(batch->VertexBuffer);
		REF_PTR_RELEASE(batch->IndexBuffer);
		delete batch;
	}
	DecoBatches.Delete_All();
	for (i = 0; i < RowBuffers.Count(); i++) {
		REF_PTR_RELEASE(RowBuffers[i]);
	}
	RowBuffers.Delete_All();
	REF_PTR_RELEASE(IndexBuffer);
	REF_PTR_RELEASE(LightmapTexture);
	REF_PTR_RELEASE(DetailTexture);
	REF_PTR_RELEASE(Material);
	delete [] PatchLevel;
	delete [] Colors;
	delete [] Lightmap;
	delete [] HorizonEast;
	delete [] HorizonWest;
	delete [] Normals;
	delete [] SkyWeights;
	delete Field;
}

/*
** VIDTEST 0x4068F0 (the client inlines it in mainview.cpp, from land%04d.ini)
*/
VidtestLandClass *VidtestLandClass::Create(const char *ini_name, bool flag)
{
	VidtestLandClass *land = new VidtestLandClass;
	if (!land->Load_Assets(ini_name)) {
		land->Release_Ref();
		return NULL;
	}
	land->Flag = flag;
	return land;
}

/*
** VIDTEST 0x405750, client 0x4691D0: the height field, [GENERAL] HASSEA, [LAND] COLOR_MAP, DETAIL_MAP,
** XDETAIL_SCALE, YDETAIL_SCALE, FLIP_TGA, Patch_Grid_Width (case-sensitive: land.ini's
** PATCH_GRID_WIDTH is not read), LIGHTMAP_CHUNK; [SEA] (not rebuilt: river land.ini has no sea).
*/
bool VidtestLandClass::Load_Assets(const char *ini_name)
{
	INIClass ini;
	ini.Load(ini_name);
	Field = HeightFieldClass::Load(ini_name, 0xFFFFFFFF);	// ours: no maximum size (the client passes its graphics option)
	if (Field == NULL) {
		return false;
	}
	HasSea = ini.Get_Bool("GENERAL", "HASSEA", false);
	char color_map[100];
	char detail_map[100];
	ini.Get_String("LAND", "COLOR_MAP", "", color_map, sizeof(color_map));
	ini.Get_String("LAND", "DETAIL_MAP", "", detail_map, sizeof(detail_map));
	DetailScaleX = ini.Get_Float("LAND", "XDETAIL_SCALE", 1.0f);
	DetailScaleY = ini.Get_Float("LAND", "YDETAIL_SCALE", 1.0f);
	bool flip_tga = ini.Get_Bool("LAND", "FLIP_TGA", false);
	PatchGridWidth = ini.Get_Int("LAND", "Patch_Grid_Width", 16);
	LightmapChunk = ini.Get_Int("LAND", "LIGHTMAP_CHUNK", 8);
	if (PatchGridWidth < 1 || PatchGridWidth > 16) {
		PatchGridWidth = 16;
	}

	int size = Field->Get_Size();
	PatchCount = (size + PatchGridWidth - 1) / PatchGridWidth;

	// the colour map, one texel per vertex (VIDTEST averages a larger map down to the field's size)
	Colors = new unsigned char[size * size * 3];
	memset(Colors, 0x80, size * size * 3);
	Targa targa;
	if (color_map[0] && targa.Open(color_map, TGA_READMODE) == 0) {
		if (!flip_tga) {
			targa.Header.ImageDescriptor ^= 0x20;
		}
		if (targa.Load(color_map, TGAF_IMAGE, false) == 0 && targa.GetImage() != NULL) {
			int w = targa.Header.Width;
			int h = targa.Header.Height;
			int bpp = targa.Header.PixelDepth / 8;
			const unsigned char *image = (const unsigned char *)targa.GetImage();
			if (bpp >= 3 && w > 0 && h > 0) {
				for (int j = 0; j < size; j++) {
					for (int i = 0; i < size; i++) {
						// average the source texels that fall on this vertex
						int x0 = i * w / size, x1 = Int_Max((i + 1) * w / size, x0 + 1);
						int y0 = j * h / size, y1 = Int_Max((j + 1) * h / size, y0 + 1);
						unsigned int r = 0, g = 0, b = 0, n = 0;
						for (int y = y0; y < y1; y++) {
							for (int x = x0; x < x1; x++) {
								const unsigned char *p = image + (y * w + x) * bpp;
								b += p[0];
								g += p[1];
								r += p[2];
								n++;
							}
						}
						// with FLIP_TGA the TGA's rows in file order (bottom-up) are the field's rows: the
						// original's lightmap texel (i, j) is the colour of the file's row j (d3d8shot dump of
						// its lightmap, 0.994 correlation; flipped, as before, 0.01)
						unsigned char *c = Colors + (j * size + i) * 3;
						c[0] = (unsigned char)(r / n);
						c[1] = (unsigned char)(g / n);
						c[2] = (unsigned char)(b / n);
					}
				}
			}
		}
		targa.Close();
	}

	// the detail map
	if (detail_map[0]) {
		DetailTexture = WW3DAssetManager::Get_Instance()->Get_Texture(detail_map);
	}

	// the lightmap (VIDTEST: TextureClass(size, size, 32 or 16 bit, MIP_LEVELS_1))
	Lightmap = new unsigned int[size * size];
	for (int k = 0; k < size * size; k++) {
		const unsigned char *c = Colors + k * 3;
		Lightmap[k] = 0xFF000000 | (c[0] << 16) | (c[1] << 8) | c[2];
	}
	LightmapTexture = new TextureClass(size, size, WW3D_FORMAT_X8R8G8B8, MIP_LEVELS_1);
	LightmapTexture->Set_U_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);
	LightmapTexture->Set_V_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);
	Build_Horizons();
	Load_PLD();
	Upload_Lightmap();

	// the material and shader: lighting off (the lightmap is the colour), the detail map scaled 2x;
	// culled and fogged as VIDTEST's land (its draws: D3DRS_CULLMODE CW, FOGENABLE with the scene's
	// fog colour; d3d8shot draw dump of frames 500/900, M2.4)
	Material = new VertexMaterialClass;
	Material->Set_Lighting(false);
	Shader = ShaderClass::_PresetOpaqueShader;
	Shader.Set_Cull_Mode(ShaderClass::CULL_MODE_ENABLE);
	Shader.Set_Fog_Func(ShaderClass::FOG_ENABLE);
	Shader.Set_Texturing(ShaderClass::TEXTURING_ENABLE);
	Shader.Set_Primary_Gradient(ShaderClass::GRADIENT_DISABLE);
	if (DetailTexture != NULL) {
		Shader.Set_Post_Detail_Color_Func(ShaderClass::DETAILCOLOR_SCALE2X);
	}

	Build_Buffers();

	// the bounds
	float half = Field->Get_Extent() * 0.5f;
	float zmin = 1e30f, zmax = -1e30f;
	for (int j = 0; j < size; j++) {
		for (int i = 0; i < size; i++) {
			float z = Field->Get_Vertex_Height(i, j);
			if (z < zmin) zmin = z;
			if (z > zmax) zmax = z;
		}
	}
	BoundBox.Center.Set(0.0f, 0.0f, (zmin + zmax) * 0.5f);
	BoundBox.Extent.Set(half, half, (zmax - zmin) * 0.5f + SKIRT_DEPTH);
	BoundSphere.Center = BoundBox.Center;
	BoundSphere.Radius = BoundBox.Extent.Length();
	return true;
}

/*
** One vertex buffer per patch row (each patch: (G+1)^2 grid vertices, then 4 x (G+1) skirt vertices),
** one index buffer with every level's triangles in patch-local indices
*/
void VidtestLandClass::Build_Buffers(void)
{
	int g = PatchGridWidth;
	int side = g + 1;
	int grid = side * side;
	int per_patch = grid + 4 * side;
	PatchLevel = new unsigned char[PatchCount * PatchCount];
	memset(PatchLevel, 0, PatchCount * PatchCount);

	int size = Field->Get_Size();
	float inv = 1.0f / (float)size;
	for (int py = 0; py < PatchCount; py++) {
		DX8VertexBufferClass *vb = new DX8VertexBufferClass(DX8_FVF_XYZUV2,
			(unsigned short)(PatchCount * per_patch));
		{
			VertexBufferClass::WriteLockClass lock(vb);
			LandVertexStruct *v = (LandVertexStruct *)lock.Get_Vertex_Array();
			for (int px = 0; px < PatchCount; px++) {
				LandVertexStruct *base = v + px * per_patch;
				for (int y = 0; y < side; y++) {
					for (int x = 0; x < side; x++) {
						// as the original's land (d3d8shot dump): size + 1 vertices a side, from -extent/2 to
						// +extent/2, the last row and column at the clamped field's heights, uv = index / size
						int i = px * g + x;
						int j = py * g + y;
						Vector3 p = Field->Get_Vertex(i, j);
						LandVertexStruct &o = base[y * side + x];
						o.X = p.X;
						o.Y = p.Y;
						o.Z = p.Z;
						o.U1 = i * inv;
						o.V1 = j * inv;
						o.U2 = o.U1 * DetailScaleX;
						o.V2 = o.V1 * DetailScaleY;
					}
				}
				// skirts: bottom (y = 0), top (y = g), left (x = 0), right (x = g)
				for (int k = 0; k < side; k++) {
					int src[4] = { k, g * side + k, k * side, k * side + g };
					for (int e = 0; e < 4; e++) {
						LandVertexStruct &o = base[grid + e * side + k];
						o = base[src[e]];
						o.Z -= SKIRT_DEPTH;
					}
				}
			}
		}
		RowBuffers.Add(vb);
	}

	// the index lists, level s = stride 1 << s
	int total = 0;
	for (int s = 0; s < MAX_LEVELS; s++) {
		int step = 1 << s;
		if (step > g) break;
		int n = g / step;
		total += n * n * 6 + 4 * n * 6;
	}
	IndexBuffer = new DX8IndexBufferClass((unsigned short)total);
	{
		IndexBufferClass::WriteLockClass lock(IndexBuffer);
		unsigned short *idx = lock.Get_Index_Array();
		int at = 0;
		for (int s = 0; s < MAX_LEVELS; s++) {
			int step = 1 << s;
			LevelStart[s] = at;
			LevelPolys[s] = 0;
			if (step > g) {
				LevelStart[s] = LevelStart[s - 1];
				LevelPolys[s] = LevelPolys[s - 1];
				continue;
			}
			for (int y = 0; y < g; y += step) {
				for (int x = 0; x < g; x += step) {
					unsigned short v00 = (unsigned short)(y * side + x);
					unsigned short v10 = (unsigned short)(y * side + x + step);
					unsigned short v01 = (unsigned short)((y + step) * side + x);
					unsigned short v11 = (unsigned short)((y + step) * side + x + step);
					idx[at++] = v00; idx[at++] = v10; idx[at++] = v11;
					idx[at++] = v00; idx[at++] = v11; idx[at++] = v01;
				}
			}
			for (int e = 0; e < 4; e++) {
				for (int k = 0; k < g; k += step) {
					unsigned short a, b;
					if (e == 0) { a = (unsigned short)k; b = (unsigned short)(k + step); }
					else if (e == 1) { a = (unsigned short)(g * side + k); b = (unsigned short)(g * side + k + step); }
					else if (e == 2) { a = (unsigned short)(k * side); b = (unsigned short)((k + step) * side); }
					else { a = (unsigned short)(k * side + g); b = (unsigned short)((k + step) * side + g); }
					unsigned short a2 = (unsigned short)(grid + e * side + k);
					unsigned short b2 = (unsigned short)(grid + e * side + k + step);
					idx[at++] = a; idx[at++] = b; idx[at++] = b2;
					idx[at++] = a; idx[at++] = b2; idx[at++] = a2;
				}
			}
			LevelPolys[s] = (at - LevelStart[s]) / 3;
		}
	}
}

/*
** The horizon along x in both directions, for the shadow test (VIDTEST keeps two 16-bit arrays,
** 0x1E8 / 0x1EC, from the .pld cache; 0..65535 = 0..pi/2)
*/
/*
** The .pld cache (VIDTEST 0x40611E / 0x40BA00): the heightmap's name with "pgm" changed to "pld";
** a count (the field's vertices), then per vertex, array after array: the normal's index (1 byte),
** two 16-bit horizons (0x1E8, 0x1EC), the sky's share in R, G and B (1 byte each); then the 256
** normals (Vector3). VIDTEST uses it when it is newer than the PGM and the ini, else rebuilds and
** writes it (0x40B510, 0x40BB90). Here: the normals and the sky shares, when the file is there
** and its count fits (the horizons stay ours: VIDTEST compares them with the sun's zenith angle).
*/
bool VidtestLandClass::Load_PLD(void)
{
	char name[100];
	strncpy(name, Field->Get_Name(), sizeof(name) - 1);
	name[sizeof(name) - 1] = 0;
	char *dot = strrchr(name, '.');
	if (dot == NULL || stricmp(dot, ".pgm") != 0) {
		return false;
	}
	dot[2] = 'l';
	dot[3] = 'd';
	FileClass *file = _TheFileFactory->Get_File(name);
	if (file == NULL) {
		return false;
	}
	bool ok = false;
	if (file->Open(FileClass::READ)) {
		int size = Field->Get_Size();
		int count = 0;
		if (file->Read(&count, 4) == 4 && count == size * size) {
			unsigned char *index = new unsigned char[count];
			unsigned short *horizon = new unsigned short[count];
			unsigned char *weights[3];
			for (int c = 0; c < 3; c++) weights[c] = new unsigned char[count];
			Vector3 table[256];
			ok = file->Read(index, count) == count
				&& file->Read(horizon, count * 2) == count * 2
				&& file->Read(horizon, count * 2) == count * 2
				&& file->Read(weights[0], count) == count
				&& file->Read(weights[1], count) == count
				&& file->Read(weights[2], count) == count
				&& file->Read(table, sizeof(table)) == sizeof(table);
			if (ok) {
				SkyWeights = new unsigned char[count * 3];
				for (int k = 0; k < count; k++) {
					Normals[k] = table[index[k]];
					SkyWeights[k * 3] = weights[0][k];
					SkyWeights[k * 3 + 1] = weights[1][k];
					SkyWeights[k * 3 + 2] = weights[2][k];
				}
			}
			delete [] index;
			delete [] horizon;
			for (c = 0; c < 3; c++) delete [] weights[c];
		}
		file->Close();
	}
	_TheFileFactory->Return_File(file);
	return ok;
}

void VidtestLandClass::Build_Horizons(void)
{
	int size = Field->Get_Size();
	float spacing = Field->Get_Spacing();
	HorizonEast = new unsigned short[size * size];
	HorizonWest = new unsigned short[size * size];
	Normals = new Vector3[size * size];
	for (int n = 0; n < size * size; n++) {
		Normals[n] = Field->Get_Normal(n % size, n / size);
	}
	const float scale = 65535.0f / (WWMATH_PI * 0.5f);
	for (int j = 0; j < size; j++) {
		for (int i = 0; i < size; i++) {
			float h = Field->Get_Vertex_Height(i, j);
			float east = 0.0f, west = 0.0f;
			for (int k = 1; k < size; k++) {
				float d = k * spacing;
				if (i + k < size) {
					float t = (Field->Get_Vertex_Height(i + k, j) - h) / d;
					if (t > east) east = t;
				}
				if (i - k >= 0) {
					float t = (Field->Get_Vertex_Height(i - k, j) - h) / d;
					if (t > west) west = t;
				}
			}
			HorizonEast[j * size + i] = (unsigned short)(atan(east) * scale);
			HorizonWest[j * size + i] = (unsigned short)(atan(west) * scale);
		}
	}
}

/*
** VIDTEST 0x409820 / 0x409BD0 / 0x40A500: a texel is colour x (light x max(0, n.sun) + sky share x sky)
** where the sun clears the horizon (with a penumbra either side), colour x sky share x sky in shadow
*/
void VidtestLandClass::Compute_Lightmap(const LandLightingStruct &lighting, int first_row, int rows)
{
	int size = Field->Get_Size();
	Vector3 sun = lighting.SunDirection;
	float elevation = (float)asin(WWMath::Clamp(sun.Z, -1.0f, 1.0f));
	float bias = WWMath::Max(lighting.ShadowBias, 0.001f) * 10.0f;
	const float scale = 65535.0f / (WWMATH_PI * 0.5f);
	float lo = WWMath::Clamp(elevation - bias, 0.0f, WWMATH_PI * 0.5f) * scale;
	float hi = WWMath::Clamp(elevation + bias, 0.0f, WWMATH_PI * 0.5f) * scale;
	const unsigned short *horizon = sun.X >= 0.0f ? HorizonEast : HorizonWest;
	int last = WWMath::Min(first_row + rows, size);
	for (int j = first_row; j < last; j++) {
		for (int i = 0; i < size; i++) {
			int k = j * size + i;
			const unsigned char *c = Colors + k * 3;
			// the sky's share: VIDTEST's per-vertex sky arrays (0x1F0 / 0x1F4 / 0x1F8, from the .pld);
			// here the sky not hidden by the two horizons
			float r, g, b;
			if (SkyWeights != NULL) {
				const unsigned char *w = SkyWeights + k * 3;
				r = w[0] * (1.0f / 256.0f) * lighting.SkyColor.X;
				g = w[1] * (1.0f / 256.0f) * lighting.SkyColor.Y;
				b = w[2] * (1.0f / 256.0f) * lighting.SkyColor.Z;
			} else {
				float sky = 1.0f - (HorizonEast[k] + HorizonWest[k]) * (1.0f / (2.0f * 65535.0f));
				r = sky * lighting.SkyColor.X;
				g = sky * lighting.SkyColor.Y;
				b = sky * lighting.SkyColor.Z;
			}
			float lit = 1.0f;
			if (horizon[k] >= hi) {
				lit = 0.0f;
			} else if (horizon[k] > lo) {
				lit = (hi - horizon[k]) / (hi - lo);
			}
			if (lit > 0.0f) {
				float d = Vector3::Dot_Product(Normals[k], sun);
				if (d > 0.0f) {
					d *= lit;
					r += lighting.LightColor.X * d;
					g += lighting.LightColor.Y * d;
					b += lighting.LightColor.Z * d;
				}
			}
			// times the colour map (VIDTEST 0x40A500)
			unsigned int ir = (unsigned int)WWMath::Min(WWMath::Min(r, 1.0f) * c[0], 255.0f);
			unsigned int ig = (unsigned int)WWMath::Min(WWMath::Min(g, 1.0f) * c[1], 255.0f);
			unsigned int ib = (unsigned int)WWMath::Min(WWMath::Min(b, 1.0f) * c[2], 255.0f);
			Lightmap[k] = 0xFF000000 | (ir << 16) | (ig << 8) | ib;
		}
	}
}

void VidtestLandClass::Upload_Lightmap(void)
{
	SurfaceClass *surface = LightmapTexture->Get_Surface_Level(0);
	if (surface == NULL) {
		return;
	}
	int size = Field->Get_Size();
	int pitch = 0;
	unsigned char *bits = (unsigned char *)surface->Lock(&pitch);
	if (bits != NULL) {
		for (int j = 0; j < size; j++) {
			memcpy(bits + j * pitch, Lightmap + j * size, size * 4);
		}
		surface->Unlock();
	}
	REF_PTR_RELEASE(surface);
}

/*
** VIDTEST 0x40A9F0 (the full rebuild) and 0x40AB10 (time-sliced: LIGHTMAP_CHUNK rows a call, into a
** second texture swapped in when complete; here into the CPU copy, uploaded when complete)
*/
void VidtestLandClass::Update_Lightmap_Full(const LandLightingStruct &lighting)
{
	Compute_Lightmap(lighting, 0, Field->Get_Size());
	Upload_Lightmap();
	LightmapRow = 0;
}

void VidtestLandClass::Update_Lightmap_ManualThreaded(const LandLightingStruct &lighting)
{
	int size = Field->Get_Size();
	int rows = LightmapChunk > 0 ? LightmapChunk : size;
	Compute_Lightmap(lighting, LightmapRow, rows);
	LightmapRow += rows;
	if (LightmapRow >= size) {
		Upload_Lightmap();				// a whole new map: VIDTEST swaps its two lightmap textures here
		LightmapRow = 0;
	}
}

/*
** VIDTEST 0x4078F0: the level of each patch from its distance to the camera; lod 0..7 (the Land LOD
** test's level), higher is finer
*/
void VidtestLandClass::Update_Patches(CameraClass &camera, int lod)
{
	Vector3 eye = camera.Get_Position();
	float patch = PatchGridWidth * Field->Get_Spacing();
	float half = Field->Get_Extent() * 0.5f;
	float near_dist = patch * 0.75f * (float)(1 << (lod / 2)) * ((lod & 1) ? 1.41421356f : 1.0f);
	CurrentPolys = 0;
	for (int py = 0; py < PatchCount; py++) {
		for (int px = 0; px < PatchCount; px++) {
			Vector3 center(-half + (px + 0.5f) * patch, -half + (py + 0.5f) * patch, 0.0f);
			int ci = Int_Min(px * PatchGridWidth + PatchGridWidth / 2, Field->Get_Size() - 1);
			int cj = Int_Min(py * PatchGridWidth + PatchGridWidth / 2, Field->Get_Size() - 1);
			center.Z = Field->Get_Vertex_Height(ci, cj);
			SphereClass sphere(center, patch * 0.75f + 2000.0f);
			unsigned char &level = PatchLevel[py * PatchCount + px];
			if (camera.Cull_Sphere(sphere)) {
				level = 0xFF;
				continue;
			}
			float d = (center - eye).Length();
			int s = 0;
			while (s < MAX_LEVELS - 1 && d > near_dist * (float)(1 << s)) {
				s++;
			}
			level = (unsigned char)s;
			CurrentPolys += LevelPolys[s];
		}
	}
	Dirty = false;
}

void VidtestLandClass::Get_Obj_Space_Bounding_Sphere(SphereClass &sphere) const
{
	sphere = BoundSphere;
}

void VidtestLandClass::Get_Obj_Space_Bounding_Box(AABoxClass &box) const
{
	box = BoundBox;
}

void VidtestLandClass::Render(RenderInfoClass &rinfo)
{
	if (Field == NULL || IndexBuffer == NULL) {
		return;
	}
	int g = PatchGridWidth;
	int side = g + 1;
	int per_patch = side * side + 4 * side;

	Matrix3D identity(true);
	DX8Wrapper::Set_Transform(D3DTS_WORLD, identity);
	DX8Wrapper::Set_Material(Material);
	DX8Wrapper::Set_Shader(Shader);
	DX8Wrapper::Set_Texture(0, LightmapTexture);
	DX8Wrapper::Set_Texture(1, DetailTexture);
	for (int py = 0; py < PatchCount; py++) {
		DX8Wrapper::Set_Vertex_Buffer(RowBuffers[py]);
		for (int px = 0; px < PatchCount; px++) {
			int level = PatchLevel[py * PatchCount + px];
			if (level == 0xFF) continue;
			DX8Wrapper::Set_Index_Buffer(IndexBuffer, (unsigned short)(px * per_patch));
			DX8Wrapper::Draw_Triangles((unsigned short)LevelStart[level], (unsigned short)LevelPolys[level],
				0, (unsigned short)per_patch);
		}
	}
	DX8Wrapper::Set_Texture(1, NULL);

	Render_Decorations(rinfo);
}

/*
** Decorations (VIDTEST 0x415C40, client 0x479D00): decoration.ini [set]: NUM_DECOS, MIN_SCALE,
** MAX_SCALE, NUM_PROTOS, PROTO%d (default TDTREE01A); Random4Class(0x1105) picks a prototype, a scale
** and a place (u, v) on the field, and a rotation about z. As VIDTEST (0x416170 = client 0x47A2F0),
** the prototypes' meshes are copied, transformed, into shared buffers: one batch per texture /
** shader / material, split where a buffer would pass 65535 vertices or indices.
*/
struct DecoPartStruct	// NAME 2: ours (VIDTEST 0x416170 batches the prototypes)
{
	MeshModelClass *			Model;
	Matrix3D						Transform;		// in the prototype
	TextureClass *				Texture;
	ShaderClass					Shader;
	VertexMaterialClass *	Material;
	int							Batch;
	bool							Shadow;
	bool operator == (const DecoPartStruct &that) const	{ return Model == that.Model && Batch == that.Batch; }
	bool operator != (const DecoPartStruct &that) const	{ return !(*this == that); }
};

struct DecoVertexStruct	// NAME 2: ours
{
	float	X, Y, Z;
	float	NX, NY, NZ;
	float	U, V;
};

struct DecoInstanceStruct	// NAME 2: ours
{
	int		Proto;
	Matrix3D	Transform;
	Matrix3D	Shadow;			// the shadow mesh's: on the ground (VIDTEST 0x416170)
};

/*
** VIDTEST 0x418400: the first mesh under robj whose name holds ".D" or ".d" (tree) or ".S" or ".s"
** (its shadow), depth first; add-reffed
*/
static RenderObjClass *Find_Deco_Mesh(RenderObjClass *robj, bool tree)	// NAME 2: ours
{
	if (robj == NULL) return NULL;
	if (robj->Class_ID() == RenderObjClass::CLASSID_MESH) {
		const char *name = robj->Get_Name();
		if (name != NULL && (strstr(name, tree ? ".d" : ".s") != NULL || strstr(name, tree ? ".D" : ".S") != NULL)) {
			robj->Add_Ref();
			return robj;
		}
		return NULL;
	}
	for (int i = 0; i < robj->Get_Num_Sub_Objects(); i++) {
		RenderObjClass *sub = robj->Get_Sub_Object(i);
		RenderObjClass *found = Find_Deco_Mesh(sub, tree);
		REF_PTR_RELEASE(sub);
		if (found != NULL) return found;
	}
	return NULL;
}

bool VidtestLandClass::Create_Decorations(int set)
{
	INIClass ini;
	if (!ini.Load("decoration.ini")) {
		return false;
	}
	char section[16];
	sprintf(section, "%d", set);
	int count = ini.Get_Int(section, "NUM_DECOS", 5000);
	float min_scale = ini.Get_Float(section, "MIN_SCALE", 1.0f);
	float max_scale = ini.Get_Float(section, "MAX_SCALE", 1.0f);
	int protos = ini.Get_Int(section, "NUM_PROTOS", 1);
	if (protos < 1) protos = 1;
	WW3DAssetManager *assets = WW3DAssetManager::Get_Instance();

	// the prototypes' meshes
	DynamicVectorClass<DecoPartStruct> parts;
	DynamicVectorClass<int> first_part;
	DynamicVectorClass<RenderObjClass *> keep;
	for (int p = 0; p < protos; p++) {
		char key[16];
		char name[64];
		sprintf(key, "PROTO%d", p);
		ini.Get_String(section, key, "TDTREE01A", name, sizeof(name));
		first_part.Add(parts.Count());
		RenderObjClass *robj = assets->Create_Render_Obj(name);
		if (robj == NULL) continue;
		robj->Set_Transform(Matrix3D(true));
		keep.Add(robj);
		// the tree is the first mesh named ".D"/".d", its shadow the first ".S"/".s" (VIDTEST 0x418400:
		// TDTREE01A.DTREE, 6 polygons, and TDTREE01A.STREE, one triangle)
		RenderObjClass *meshes[2];
		meshes[0] = Find_Deco_Mesh(robj, true);
		meshes[1] = Find_Deco_Mesh(robj, false);
		for (int m = 0; m < 2; m++) {
			if (meshes[m] == NULL) continue;
			MeshModelClass *model = ((MeshClass *)meshes[m])->Peek_Model();
			if (model != NULL && model->Get_Vertex_Count() > 0 && model->Get_Polygon_Count() > 0) {
				DecoPartStruct part;
				part.Model = model;
				part.Model->Add_Ref();
				part.Transform = meshes[m]->Get_Transform();
				part.Texture = model->Peek_Single_Texture(0, 0);
				if (part.Texture == NULL) part.Texture = model->Peek_Texture(0, 0, 0);
				part.Shader = model->Has_Shader_Array(0) ? model->Get_Shader(0, 0) : model->Get_Single_Shader(0);
				part.Material = model->Peek_Single_Material(0);
				part.Batch = -1;
				part.Shadow = (m == 1);
				parts.Add(part);
			}
			meshes[m]->Release_Ref();
		}
	}
	first_part.Add(parts.Count());

	// the batches: one per texture / shader / material
	DynamicVectorClass<int> batch_of_kind;
	DynamicVectorClass<DecoPartStruct *> kinds;
	for (int i = 0; i < parts.Count(); i++) {
		for (int k = 0; k < kinds.Count(); k++) {
			if (kinds[k]->Texture == parts[i].Texture && kinds[k]->Material == parts[i].Material
				&& kinds[k]->Shader == parts[i].Shader && kinds[k]->Shadow == parts[i].Shadow) {
				parts[i].Batch = k;
				break;
			}
		}
		if (parts[i].Batch < 0) {
			parts[i].Batch = kinds.Count();
			kinds.Add(&parts[i]);
		}
	}

	// the instances
	Random4Class random(0x1105);
	DecoInstanceStruct *instances = new DecoInstanceStruct[count];
	for (int d = 0; d < count; d++) {
		// VIDTEST 0x415C40's order: the prototype (Pick_Random_Number, no draw for one prototype), the
		// scale, then v before u (the second draw is the sample's first coordinate), the angle
		int p = random(0, protos - 1);
		float scale = min_scale + (max_scale - min_scale) * random.Get_Float();
		float v = random.Get_Float();
		float u = random.Get_Float();
		float angle = random.Get_Float() * 2.0f * WWMATH_PI;
		Vector3 pos;
		Field->Sample(&pos, u, v);
		Matrix3D tm(true);
		tm.Set_Translation(pos);
		tm.Rotate_Z(angle);

		// the shadow (VIDTEST 0x416170): the matrix turned so that its z axis follows the ground's
		// normal n, by the quaternion (z x n) sin(a/2), cos(a/2) with a = acos(z.n), not normalised
		// (as VIDTEST's), each column v going to q v q*; lifted 3 units along n
		Matrix3D shadow = tm;
		{
			float spacing = Field->Get_Spacing();
			float half_extent = Field->Get_Extent() * 0.5f;
			int ni = Int_Max(0, Int_Min(Field->Get_Size() - 1, (int)((pos.X + half_extent) / spacing + 0.5f)));
			int nj = Int_Max(0, Int_Min(Field->Get_Size() - 1, (int)((pos.Y + half_extent) / spacing + 0.5f)));
			Vector3 n = Field->Get_Normal(ni, nj);
			Vector3 z(tm[0][2], tm[1][2], tm[2][2]);
			Vector3 axis;
			Vector3::Cross_Product(z, n, &axis);
			if (axis.Length2() > 0.0001f) {
				float a = (float)acos(WWMath::Clamp(Vector3::Dot_Product(z, n), -1.0f, 1.0f)) * 0.5f;
				float w = (float)cos(a);
				Vector3 q = axis * (float)sin(a);
				for (int c = 0; c < 3; c++) {
					Vector3 col(tm[0][c], tm[1][c], tm[2][c]);
					Vector3 t;
					Vector3::Cross_Product(q, col, &t);
					t += col * w;									// the vector part of q v
					float tw = -Vector3::Dot_Product(q, col);		// its scalar part
					Vector3 x;
					Vector3::Cross_Product(t, -q, &x);
					Vector3 r = (-q) * tw + t * w + x;				// (q v) q*
					shadow[0][c] = r.X;
					shadow[1][c] = r.Y;
					shadow[2][c] = r.Z;
				}
			}
			shadow.Set_Translation(pos + n * 3.0f);
		}
		for (int r = 0; r < 3; r++) {
			tm[r][0] *= scale;
			tm[r][1] *= scale;
			tm[r][2] *= scale;
			shadow[r][0] *= scale;
			shadow[r][1] *= scale;
			shadow[r][2] *= scale;
		}
		instances[d].Proto = p;
		instances[d].Transform = tm;
		instances[d].Shadow = shadow;
	}

	// the patch each decoration stands on: VIDTEST keeps them per patch and draws a patch's only while
	// its level is below 3 (0x410B90; the original draws no trees at Land LOD 1, one draw per visible
	// patch at LOD 7: d3d8shot draw dump, M2.4)
	float patch_size = PatchGridWidth * Field->Get_Spacing();
	float half = Field->Get_Extent() * 0.5f;
	int *patch_of = new int[count];
	for (d = 0; d < count; d++) {
		Vector3 t = instances[d].Transform.Get_Translation();
		int px = Int_Max(0, Int_Min(PatchCount - 1, (int)((t.X + half) / patch_size)));
		int py = Int_Max(0, Int_Min(PatchCount - 1, (int)((t.Y + half) / patch_size)));
		patch_of[d] = py * PatchCount + px;
	}

	// fill the buffers, patch by patch, kind by kind
	for (int patch = 0; patch < PatchCount * PatchCount; patch++) {
		for (int k = 0; k < kinds.Count(); k++) {
			int verts = 0, polys = 0;
			for (int d = 0; d < count; d++) {
				if (patch_of[d] != patch) continue;
				for (int i = first_part[instances[d].Proto]; i < first_part[instances[d].Proto + 1]; i++) {
					if (parts[i].Batch != k) continue;
					verts += parts[i].Model->Get_Vertex_Count();
					polys += parts[i].Model->Get_Polygon_Count();
				}
			}
			if (polys > 0 && verts <= 65000 && polys * 3 <= 65000) {
				DecoBatchStruct *batch = new DecoBatchStruct;
				batch->Texture = kinds[k]->Texture;
				if (batch->Texture) batch->Texture->Add_Ref();
				batch->Material = kinds[k]->Material;
				if (batch->Material) batch->Material->Add_Ref();
				// VIDTEST's trees are alpha tested and write depth, without blending (its draws:
				// ALPHATESTENABLE, ALPHAREF 0x60, ZWRITEENABLE, ALPHABLENDENABLE off)
				// (the shadows keep their mesh's shader: blended, no depth writes)
				batch->Shader = kinds[k]->Shader;
				batch->Shadow = kinds[k]->Shadow;
				if (!batch->Shadow) {
					batch->Shader.Set_Alpha_Test(ShaderClass::ALPHATEST_ENABLE);
					batch->Shader.Set_Src_Blend_Func(ShaderClass::SRCBLEND_ONE);
					batch->Shader.Set_Dst_Blend_Func(ShaderClass::DSTBLEND_ZERO);
					batch->Shader.Set_Depth_Mask(ShaderClass::DEPTH_WRITE_ENABLE);
				}
				batch->Vertices = verts;
				batch->Polys = polys;
				batch->Patch = patch;
				batch->VertexBuffer = new DX8VertexBufferClass(DX8_FVF_XYZNUV1, (unsigned short)verts);
				batch->IndexBuffer = new DX8IndexBufferClass((unsigned short)(polys * 3));
				{
					VertexBufferClass::WriteLockClass vlock(batch->VertexBuffer);
					IndexBufferClass::WriteLockClass ilock(batch->IndexBuffer);
					DecoVertexStruct *out = (DecoVertexStruct *)vlock.Get_Vertex_Array();
					unsigned short *idx = ilock.Get_Index_Array();
					int base = 0;
					for (int n = 0; n < count; n++) {
						if (patch_of[n] != patch) continue;
						for (int i = first_part[instances[n].Proto]; i < first_part[instances[n].Proto + 1]; i++) {
							if (parts[i].Batch != k) continue;
							MeshModelClass *model = parts[i].Model;
							Matrix3D tm;
							Matrix3D::Multiply(parts[i].Shadow ? instances[n].Shadow : instances[n].Transform,
								parts[i].Transform, &tm);
							const Vector3 *pos = model->Get_Vertex_Array();
							const Vector3 *nrm = model->Get_Vertex_Normal_Array();
							const Vector2 *uv = model->Get_UV_Array(0, 0);
							int vc = model->Get_Vertex_Count();
							for (int v = 0; v < vc; v++) {
								Vector3 p = tm * pos[v];
								Vector3 q = nrm ? tm.Rotate_Vector(nrm[v]) : Vector3(0.0f, 0.0f, 1.0f);
								q.Normalize();
								out->X = p.X; out->Y = p.Y; out->Z = p.Z;
								out->NX = q.X; out->NY = q.Y; out->NZ = q.Z;
								out->U = uv ? uv[v].U : 0.0f;
								out->V = uv ? uv[v].V : 0.0f;
								out++;
							}
							const TriIndex *tri = model->Get_Polygon_Array();
							int tc = model->Get_Polygon_Count();
							for (int t = 0; t < tc; t++) {
								*idx++ = (unsigned short)(base + tri[t].I);
								*idx++ = (unsigned short)(base + tri[t].J);
								*idx++ = (unsigned short)(base + tri[t].K);
							}
							base += vc;
						}
					}
				}
				DecoBatches.Add(batch);
			}
		}
	}
	delete [] patch_of;
	delete [] instances;
	for (i = 0; i < parts.Count(); i++) {
		parts[i].Model->Release_Ref();
	}
	for (i = 0; i < keep.Count(); i++) {
		keep[i]->Release_Ref();
	}
	return true;
}

void VidtestLandClass::Render_Decorations(RenderInfoClass &rinfo)
{
	Matrix3D identity(true);
	DX8Wrapper::Set_Transform(D3DTS_WORLD, identity);
	// the trees of every patch, then their shadows with a depth bias (VIDTEST 0x410B90's two loops;
	// the original's shadow draws: D3DRS_ZBIAS 1)
	for (int pass = 0; pass < 2; pass++) {
		if (pass == 1) DX8Wrapper::Set_DX8_Render_State(D3DRS_ZBIAS, 1);
		for (int i = 0; i < DecoBatches.Count(); i++) {
			DecoBatchStruct *batch = DecoBatches[i];
			if (batch->Shadow != (pass == 1)) continue;
			if (PatchLevel == NULL || PatchLevel[batch->Patch] >= 3) continue;		// also 0xFF: culled
			DX8Wrapper::Set_Material(batch->Material);
			DX8Wrapper::Set_Shader(batch->Shader);
			DX8Wrapper::Set_Texture(0, batch->Texture);
			DX8Wrapper::Set_Vertex_Buffer(batch->VertexBuffer);
			DX8Wrapper::Set_Index_Buffer(batch->IndexBuffer, 0);
			DX8Wrapper::Draw_Triangles(0, (unsigned short)batch->Polys, 0, (unsigned short)batch->Vertices);
			CurrentPolys += batch->Polys;
		}
	}
	DX8Wrapper::Set_DX8_Render_State(D3DRS_ZBIAS, 0);
}

/*
**	Reconstruction notes
**
**	The client's terrain, 0x4668F0-0x47A2F0 (no asserts, no __FILE__; the class name VidtestLandClass
**	from "Land.Peek() == (LandClass*)obj", the method name from the profile label
**	"Land::Update_Lightmap_ManualThreaded" 0x46DAA2), and VIDTEST's 2002 copy 0x405410-0x416170.
**	The file name is unknown: land.cpp is a proposal.
**
**	NOT EXACT: written functionally (D42) for VIDTEST's land test (milestones.md M2.4). Kept from
**	VIDTEST: the ini keys and defaults (and the case-sensitive Patch_Grid_Width), the lightmap
**	(0x409820: a 256-entry table of light x max(0, n.sun) over the quantised normals; 0x409BD0: that
**	plus the sky x the vertex's sky share where the sun clears the horizon, a penumbra, the sky term
**	alone in shadow; 0x40A500: times the colour map), its time slicing by LIGHTMAP_CHUNK rows, the
**	.pld cache's normals and sky shares, the decorations' ini, Random4Class(0x1105) and their
**	batching into per-patch buffers. Not rebuilt: the client's tessellation and render (0x474B30,
**	VB/IB per frame, triangle strips; here fixed per-patch strides with skirts), the sea, the
**	lightmap thread, the shadow test's own horizons (ours are computed, compared with the sun's
**	elevation), the decoration cache (land%04d.dcb) and the .pld rebuild. The trees' shadows follow
**	0x416170/0x410B90 functionally (ours take the nearest grid normal: within about 5-20 units of
**	the original's triangles).
**
**	Checked against the original's draw calls (tools/harness/run_reference.py --dump, drawdump.py,
**	frames 500 and 900, M2.4): the same camera, the same vertex grid (257 a side from -25000 to
**	+25000, uv = index / 256) and the same heights at every common vertex, so the land's orientation
**	was right; the colour map's rows were not (the original's lightmap is the TGA in file row order:
**	0.994 correlation), its land is fogged and culled (CW), its trees are drawn per patch only while
**	the patch's level is below 3 (none at Land LOD 1), alpha tested with depth writes, and its
**	random numbers come in the order of 0x415C40 (prototype by Pick_Random_Number, scale, v, u,
**	angle). With the original's sun angle, frames 500-900 differ in 2-4% of the pixels.
**
**	The .pld file (VIDTEST 0x40611E-0x406243, read by 0x40BA00 with fopen "rb", written by
**	0x40BB90): precomputed lighting data, used when newer than both the PGM and the ini, else
**	rebuilt (0x40B510) and written. river4_256.pld (527,364 bytes): a count (65,536 = 256 x 256),
**	then per vertex, array by array: the normal's index (1 byte, +0x1E4), two 16-bit horizons
**	(+0x1E8 for a sun at -x, +0x1EC at +x), the sky's share in R, G and B (1 byte each, +0x1F0 /
**	+0x1F4 / +0x1F8); then the 256 normals (Vector3, +0x1FC).
*/
