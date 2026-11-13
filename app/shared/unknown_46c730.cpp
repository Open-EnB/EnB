// Earth & Beyond client (reconstruction): app\shared\ terrain, the lighting, the lightmap upload and the horizon tables (file name unknown, interim: D137). Notes at the end.
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
	// NAME 1: unknown; deletes a global object (0xBDC0D8) and clears it; called by the land's dtor only


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

// NOT EXACT: shape (0x46D6B0: 64 masked rows, 29 with registers masked; the frame is the client's 0x20 once the four
// channels are separate pointers, the client's pointer-difference induction variables; the setup's load order and
// which difference stays in a register are left; attempts.csv)
void LandClass::Upload_Lightmap(Vector2i &min, Vector2i &max)
{
	SurfaceClass *surface = Unknown120->Get_Surface_Level(0);
	if (Is16Bit) {
		int j = min.J;
		int size = Field->Get_Size();
		unsigned short *words = (unsigned short *)Unknown120C;
		unsigned char *p0 = (unsigned char *)Unknown120C;
		unsigned char *p1 = p0 + 1;
		unsigned char *p2 = p0 + 2;
		unsigned char *p3 = p0 + 3;
		for (; j < max.J; j++) {
			for (int i = min.I; i < max.I; i++) {
				int n = j * size + i;
				words[n] = (unsigned short)((((unsigned short)(p3[n * 4] & 0xF0) << 4) | (unsigned short)(p2[n * 4] & 0xF0)) << 4)
					| (unsigned short)(unsigned char)((p1[n * 4] & 0xF0) | (p0[n * 4] >> 4));
			}
		}
	}
	surface->Copy(min, max, (unsigned char *)Unknown120C);
	DX8Wrapper::_Update_Texture(Unknown120, LightmapTextures[BackLightmap]);
	REF_PTR_RELEASE(surface);
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

void LandClass::Apply_Colors(void)
{
	unsigned char *lightmap = (unsigned char *)Unknown120C;
	unsigned char *colors = Colors;
	int size = Field->Get_Size();
	unsigned int i, j, value;
	if (Unknown1214 == NULL) {
		for (j = UpdateMin.J; j < (unsigned int)UpdateMax.J; j++) {
			for (i = UpdateMin.I; i < (unsigned int)UpdateMax.I; i++) {
				unsigned int n = j * size + i;
				unsigned char *color = &colors[n * 3];
				unsigned char *light = &lightmap[n * 4];
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
		unsigned char *extra = Unknown1214;
		for (j = UpdateMin.J; j < (unsigned int)UpdateMax.J; j++) {
			for (i = UpdateMin.I; i < (unsigned int)UpdateMax.I; i++) {
				unsigned int n = j * size + i;
				unsigned char *color = &colors[n * 3];
				unsigned char *light = &lightmap[n * 4];
				unsigned char *add = &extra[n];
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

// NOT EXACT: shape (688 masked rows, frame 0x1D8 vs the client's 0x19C: the client recomputes y + 1 per x step and
// fetches all eight heights before the differences; LD7's form, attempts.csv 0x46F120)
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
	int index = 0;
	int x;
	int y;

	// the four corners (x, y in { 0, Size }): written at (0, 0), (Size - 1, 0), (0, Size - 1), (Size - 1, Size - 1)
	for (y = 0; y < size + 1; y += size) {
		for (x = 0; x < size + 1; x += size) {
			int xp = x + 1;
			int yp = y + 1;
			float h00 = Field->Get_Height_2(xp - 2, y - 1);
			float h10 = Field->Get_Height_2(x, y - 1);
			float h20 = Field->Get_Height_2(xp, y - 1);
			float h01 = Field->Get_Height_2(xp - 2, y);
			float h = Field->Get_Height_2(x, y);
			float h21 = Field->Get_Height_2(xp, y);
			float h02 = Field->Get_Height_2(xp - 2, yp);
			float h12 = Field->Get_Height_2(x, yp);
			Vector3 v00(-spacing, -spacing, h00 - h);
			Vector3 v10(0.0f, -spacing, h10 - h);
			Vector3 v20(spacing, -spacing, h20 - h);
			Vector3 v01(-spacing, 0.0f, h01 - h);
			Vector3 v21(spacing, 0.0f, h21 - h);
			Vector3 v02(-spacing, spacing, h02 - h);
			Vector3 v12(0.0f, spacing, h12 - h);
			float h22 = Field->Get_Height_2(xp, yp);
			Vector3 v22(spacing, spacing, h22 - h);
			Vector3 c1;
			Vector3::Cross_Product(v00, v10, &c1);
			Vector3 c0;
			Vector3::Cross_Product(v10, v20, &c0);
			Vector3 c2;
			Vector3::Cross_Product(v20, v21, &c2);
			Vector3 c3;
			Vector3::Cross_Product(v21, v22, &c3);
			Vector3 c4;
			Vector3::Cross_Product(v22, v12, &c4);
			Vector3 c5;
			Vector3::Cross_Product(v12, v02, &c5);
			Vector3 c6;
			Vector3::Cross_Product(v02, v01, &c6);
			Vector3 c7;
			Vector3::Cross_Product(v01, v00, &c7);
			Vector3 n = c0 + c1 + c2 + c3 + c4 + c5 + c6 + c7;
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
			int xp = x + 1;
			int yp = y + 1;
			float h00 = Field->Get_Height_2(xp - 2, y - 1);
			float h20 = Field->Get_Height_2(xp, y - 1);
			float h01 = Field->Get_Height_2(xp - 2, y);
			float h = Field->Get_Height_2(x, y);
			float h21 = Field->Get_Height_2(xp, y);
			float h02 = Field->Get_Height_2(xp - 2, yp);
			Vector3 v00(-spacing, -spacing, h00 - h);
			Vector3 v20(spacing, -spacing, h20 - h);
			Vector3 v01(-spacing, 0.0f, h01 - h);
			Vector3 v21(spacing, 0.0f, h21 - h);
			Vector3 v02(-spacing, spacing, h02 - h);
			float h22 = Field->Get_Height_2(xp, yp);
			Vector3 v22(spacing, spacing, h22 - h);
			Vector3 c9;
			Vector3::Cross_Product(v02, v01, &c9);
			Vector3 c8;
			Vector3::Cross_Product(v01, v00, &c8);
			Vector3 n = c8 + c9;
			n.Normalize();
			Unknown1E4[index] = Nearest_Normal(n);
			Vector3 c11;
			Vector3::Cross_Product(v21, v22, &c11);
			Vector3 c10;
			Vector3::Cross_Product(v20, v21, &c10);
			n = c10 + c11;
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
			int xp = x + 1;
			int yp = y + 1;
			float h00 = Field->Get_Height_2(xp - 2, y - 1);
			float h10 = Field->Get_Height_2(x, y - 1);
			float h20 = Field->Get_Height_2(xp, y - 1);
			float h = Field->Get_Height_2(x, y);
			float h02 = Field->Get_Height_2(xp - 2, yp);
			float h12 = Field->Get_Height_2(x, yp);
			Vector3 v00(-spacing, -spacing, h00 - h);
			Vector3 v10(0.0f, -spacing, h10 - h);
			Vector3 v20(spacing, -spacing, h20 - h);
			Vector3 v02(-spacing, spacing, h02 - h);
			Vector3 v12(0.0f, spacing, h12 - h);
			float h22 = Field->Get_Height_2(xp, yp);
			Vector3 v22(spacing, spacing, h22 - h);
			Vector3 c13;
			Vector3::Cross_Product(v00, v10, &c13);
			Vector3 c12;
			Vector3::Cross_Product(v10, v20, &c12);
			Vector3 n = c12 + c13;
			n.Normalize();
			Unknown1E4[index] = Nearest_Normal(n);
			Vector3 c15;
			Vector3::Cross_Product(v22, v12, &c15);
			Vector3 c14;
			Vector3::Cross_Product(v12, v02, &c14);
			n = c14 + c15;
			n.Normalize();
			Unknown1E4[index + size] = Nearest_Normal(n);
			index += size - 1;
		}
		index += 3 - size * 2;
		// the interior even x in 2 .. Size - 2: one quadrant each at (x - 1, y - 1), (x, y - 1), (x - 1, y), (x, y)
		for (x = 2; x <= size - 2; x += 2) {
			int xp = x + 1;
			int yp = y + 1;
			float h00 = Field->Get_Height_2(xp - 2, y - 1);
			float h10 = Field->Get_Height_2(x, y - 1);
			float h20 = Field->Get_Height_2(xp, y - 1);
			float h01 = Field->Get_Height_2(xp - 2, y);
			float h = Field->Get_Height_2(x, y);
			float h21 = Field->Get_Height_2(xp, y);
			float h02 = Field->Get_Height_2(xp - 2, yp);
			float h12 = Field->Get_Height_2(x, yp);
			Vector3 v00(-spacing, -spacing, h00 - h);
			Vector3 v10(0.0f, -spacing, h10 - h);
			Vector3 v20(spacing, -spacing, h20 - h);
			Vector3 v01(-spacing, 0.0f, h01 - h);
			Vector3 v21(spacing, 0.0f, h21 - h);
			Vector3 v02(-spacing, spacing, h02 - h);
			Vector3 v12(0.0f, spacing, h12 - h);
			float h22 = Field->Get_Height_2(xp, yp);
			Vector3 v22(spacing, spacing, h22 - h);
			Vector3 c17;
			Vector3::Cross_Product(v00, v10, &c17);
			Vector3 c16;
			Vector3::Cross_Product(v01, v00, &c16);
			Vector3 n = c16 + c17;
			n.Normalize();
			Unknown1E4[index] = Nearest_Normal(n);
			Vector3 c19;
			Vector3::Cross_Product(v10, v20, &c19);
			Vector3 c18;
			Vector3::Cross_Product(v20, v21, &c18);
			n = c18 + c19;
			n.Normalize();
			Unknown1E4[index + 1] = Nearest_Normal(n);
			Vector3 c21;
			Vector3::Cross_Product(v12, v02, &c21);
			Vector3 c20;
			Vector3::Cross_Product(v02, v01, &c20);
			n = c20 + c21;
			n.Normalize();
			Unknown1E4[index + size] = Nearest_Normal(n);
			Vector3 c23;
			Vector3::Cross_Product(v21, v22, &c23);
			Vector3 c22;
			Vector3::Cross_Product(v22, v12, &c22);
			n = c22 + c23;
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

// NOT EXACT: shape (2042 masked rows, frame 0x7C vs the client's 0x6C: the prologue's set-up order and the corner
// blocks' index forms; LD7's carry form, attempts.csv 0x470F80)
void LandClass::Unknown_470F80(unsigned char *a, unsigned char *b, unsigned char *c)
{
	// the client: three copies of 65535 / (pi / 2) = 41720.875 (float 0xAE9AD0 in the four corners'
	// blocks, float 0xAE9ACC in the edges', double 0xAE9AC0 inside): likely one inline helper or macro per kind
	static const float HORIZON_SCALE_4 = 41720.875f;
	static const float HORIZON_SCALE_2 = 41720.875f;
	static const double HORIZON_SCALE_1 = 41720.875;
	unsigned int size = Field->Get_Size();
	unsigned int mask = Field->Get_Mask();
	unsigned int log2 = Field->Get_Log2();
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
	k0 = (0 << log2);
	k1 = k0 + mask;
	k2 = (mask << log2);
	k3 = k2 + mask;
	avg[0] = (unsigned short)(((unsigned int)hz[k0 * 2 + 0] + (unsigned int)hz[k1 * 2 + 0] + (unsigned int)hz[k2 * 2 + 0] + (unsigned int)hz[k3 * 2 + 0]) >> 2);
	avg[1] = (unsigned short)(((unsigned int)hz[k0 * 2 + 1] + (unsigned int)hz[k1 * 2 + 1] + (unsigned int)hz[k2 * 2 + 1] + (unsigned int)hz[k3 * 2 + 1]) >> 2);
	bits = (avg[0] + 0x77000) << 11 | 0x7FF;
	Unknown1E8[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_4);
	bits = (avg[1] + 0x77000) << 11 | 0x7FF;
	Unknown1EC[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_4);
	h[0] = (unsigned char)(avg[0] >> 8);
	h[1] = (unsigned char)(avg[1] >> 8);
	h[2] = (unsigned char)(((unsigned int)a[k0 * 3 + 1] + (unsigned int)a[k1 * 3 + 1] + (unsigned int)a[k2 * 3 + 1] + (unsigned int)a[k3 * 3 + 1]) >> 2);
	h[3] = (unsigned char)(((unsigned int)b[k0 * 3 + 1] + (unsigned int)b[k1 * 3 + 1] + (unsigned int)b[k2 * 3 + 1] + (unsigned int)b[k3 * 3 + 1]) >> 2);
	h[4] = (unsigned char)(((unsigned int)a[k0 * 3] + (unsigned int)a[k1 * 3] + (unsigned int)a[k2 * 3] + (unsigned int)a[k3 * 3]) >> 2);
	h[5] = (unsigned char)(((unsigned int)b[k0 * 3 + 2] + (unsigned int)b[k1 * 3 + 2] + (unsigned int)b[k2 * 3 + 2] + (unsigned int)b[k3 * 3 + 2]) >> 2);
	h[6] = (unsigned char)(((unsigned int)b[k0 * 3] + (unsigned int)b[k1 * 3] + (unsigned int)b[k2 * 3] + (unsigned int)b[k3 * 3]) >> 2);
	h[7] = (unsigned char)(((unsigned int)a[k0 * 3 + 2] + (unsigned int)a[k1 * 3 + 2] + (unsigned int)a[k2 * 3 + 2] + (unsigned int)a[k3 * 3 + 2]) >> 2);
	rgb = &Colors[(0 << log2) * 3];
	{
		Vector3 color((float)rgb[2] * (1.0f / 255.0f), (float)rgb[1] * (1.0f / 255.0f), (float)rgb[0] * (1.0f / 255.0f));
		Unknown_472F80(&Unknown1F0[n], &Unknown1F4[n], &Unknown1F8[n], h, Unknown1E4[n], color);
	}
	n++;
	for (x = 1; x < last; x++) {
		avg[0] = (unsigned short)(((unsigned int)hz[(k0 * 2 + x * 2) + 0] + (unsigned int)hz[(k2 * 2 + x * 2) + 0]) >> 1);
		avg[1] = (unsigned short)(((unsigned int)hz[(k0 * 2 + x * 2) + 1] + (unsigned int)hz[(k2 * 2 + x * 2) + 1]) >> 1);
		bits = (avg[0] + 0x77000) << 11 | 0x7FF;
		Unknown1E8[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_2);
		bits = (avg[1] + 0x77000) << 11 | 0x7FF;
		Unknown1EC[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_2);
		h[0] = (unsigned char)(avg[0] >> 8);
		h[1] = (unsigned char)(avg[1] >> 8);
		h[2] = (unsigned char)(((unsigned int)a[(k0 * 3 + x * 3) + 1] + (unsigned int)a[(k2 * 3 + x * 3) + 1]) >> 1);
		h[3] = (unsigned char)(((unsigned int)b[(k0 * 3 + x * 3) + 1] + (unsigned int)b[(k2 * 3 + x * 3) + 1]) >> 1);
		h[4] = (unsigned char)(((unsigned int)a[(k0 * 3 + x * 3)] + (unsigned int)a[(k2 * 3 + x * 3)]) >> 1);
		h[5] = (unsigned char)(((unsigned int)b[(k0 * 3 + x * 3) + 2] + (unsigned int)b[(k2 * 3 + x * 3) + 2]) >> 1);
		h[6] = (unsigned char)(((unsigned int)b[(k0 * 3 + x * 3)] + (unsigned int)b[(k2 * 3 + x * 3)]) >> 1);
		h[7] = (unsigned char)(((unsigned int)a[(k0 * 3 + x * 3) + 2] + (unsigned int)a[(k2 * 3 + x * 3) + 2]) >> 1);
		rgb = &Colors[((0 << log2) + x) * 3];
		{
			Vector3 color((float)rgb[2] * (1.0f / 255.0f), (float)rgb[1] * (1.0f / 255.0f), (float)rgb[0] * (1.0f / 255.0f));
			Unknown_472F80(&Unknown1F0[n], &Unknown1F4[n], &Unknown1F8[n], h, Unknown1E4[n], color);
		}
		n++;
	}
	k0 = (0 << log2) + wrap;
	k1 = (mask << log2) + wrap;
	k2 = (0 << log2) + last;
	k3 = (mask << log2) + last;
	avg[0] = (unsigned short)(((unsigned int)hz[k0 * 2 + 0] + (unsigned int)hz[k1 * 2 + 0] + (unsigned int)hz[k2 * 2 + 0] + (unsigned int)hz[k3 * 2 + 0]) >> 2);
	avg[1] = (unsigned short)(((unsigned int)hz[k0 * 2 + 1] + (unsigned int)hz[k1 * 2 + 1] + (unsigned int)hz[k2 * 2 + 1] + (unsigned int)hz[k3 * 2 + 1]) >> 2);
	bits = (avg[0] + 0x77000) << 11 | 0x7FF;
	Unknown1E8[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_4);
	bits = (avg[1] + 0x77000) << 11 | 0x7FF;
	Unknown1EC[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_4);
	h[0] = (unsigned char)(avg[0] >> 8);
	h[1] = (unsigned char)(avg[1] >> 8);
	h[2] = (unsigned char)(((unsigned int)a[k0 * 3 + 1] + (unsigned int)a[k1 * 3 + 1] + (unsigned int)a[k2 * 3 + 1] + (unsigned int)a[k3 * 3 + 1]) >> 2);
	h[3] = (unsigned char)(((unsigned int)b[k0 * 3 + 1] + (unsigned int)b[k1 * 3 + 1] + (unsigned int)b[k2 * 3 + 1] + (unsigned int)b[k3 * 3 + 1]) >> 2);
	h[4] = (unsigned char)(((unsigned int)a[k0 * 3] + (unsigned int)a[k1 * 3] + (unsigned int)a[k2 * 3] + (unsigned int)a[k3 * 3]) >> 2);
	h[5] = (unsigned char)(((unsigned int)b[k0 * 3 + 2] + (unsigned int)b[k1 * 3 + 2] + (unsigned int)b[k2 * 3 + 2] + (unsigned int)b[k3 * 3 + 2]) >> 2);
	h[6] = (unsigned char)(((unsigned int)b[k0 * 3] + (unsigned int)b[k1 * 3] + (unsigned int)b[k2 * 3] + (unsigned int)b[k3 * 3]) >> 2);
	h[7] = (unsigned char)(((unsigned int)a[k0 * 3 + 2] + (unsigned int)a[k1 * 3 + 2] + (unsigned int)a[k2 * 3 + 2] + (unsigned int)a[k3 * 3 + 2]) >> 2);
	rgb = &Colors[((0 << log2) + size) * 3 - 3];
	{
		Vector3 color((float)rgb[2] * (1.0f / 255.0f), (float)rgb[1] * (1.0f / 255.0f), (float)rgb[0] * (1.0f / 255.0f));
		Unknown_472F80(&Unknown1F0[n], &Unknown1F4[n], &Unknown1F8[n], h, Unknown1E4[n], color);
	}
	n++;
	// the inner rows: the first and last columns average with each other, the rest is the cell's own
	for (y = 1; y < last; y++) {
		k0 = y << log2;
		k1 = k0 + mask;
		avg[0] = (unsigned short)(((unsigned int)hz[k0 * 2 + 0] + (unsigned int)hz[k1 * 2 + 0]) >> 1);
		avg[1] = (unsigned short)(((unsigned int)hz[k0 * 2 + 1] + (unsigned int)hz[k1 * 2 + 1]) >> 1);
		bits = (avg[0] + 0x77000) << 11 | 0x7FF;
		Unknown1E8[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_2);
		bits = (avg[1] + 0x77000) << 11 | 0x7FF;
		Unknown1EC[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_2);
		h[0] = (unsigned char)(avg[0] >> 8);
		h[1] = (unsigned char)(avg[1] >> 8);
		h[2] = (unsigned char)(((unsigned int)a[k0 * 3 + 1] + (unsigned int)a[k1 * 3 + 1]) >> 1);
		h[3] = (unsigned char)(((unsigned int)b[k0 * 3 + 1] + (unsigned int)b[k1 * 3 + 1]) >> 1);
		h[4] = (unsigned char)(((unsigned int)a[k0 * 3] + (unsigned int)a[k1 * 3]) >> 1);
		h[5] = (unsigned char)(((unsigned int)b[k0 * 3 + 2] + (unsigned int)b[k1 * 3 + 2]) >> 1);
		h[6] = (unsigned char)(((unsigned int)b[k0 * 3] + (unsigned int)b[k1 * 3]) >> 1);
		h[7] = (unsigned char)(((unsigned int)a[k0 * 3 + 2] + (unsigned int)a[k1 * 3 + 2]) >> 1);
		rgb = &Colors[(y << log2) * 3];
		{
			Vector3 color((float)rgb[2] * (1.0f / 255.0f), (float)rgb[1] * (1.0f / 255.0f), (float)rgb[0] * (1.0f / 255.0f));
			Unknown_472F80(&Unknown1F0[n], &Unknown1F4[n], &Unknown1F8[n], h, Unknown1E4[n], color);
		}
		n++;
		for (x = 1; x < last; x++) {
			k0 = (y << log2) + x;
			avg[0] = (unsigned int)hz[k0 * 2 + 0];
			avg[1] = (unsigned int)hz[k0 * 2 + 1];
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
			rgb = &Colors[((y << log2) + x) * 3];
			{
				Vector3 color((float)rgb[2] * (1.0f / 255.0f), (float)rgb[1] * (1.0f / 255.0f), (float)rgb[0] * (1.0f / 255.0f));
				Unknown_472F80(&Unknown1F0[n], &Unknown1F4[n], &Unknown1F8[n], h, Unknown1E4[n], color);
			}
			n++;
		}
		k0 = (y << log2) + wrap;
		k1 = (y << log2) + last;
		avg[0] = (unsigned short)(((unsigned int)hz[k0 * 2 + 0] + (unsigned int)hz[k1 * 2 + 0]) >> 1);
		avg[1] = (unsigned short)(((unsigned int)hz[k0 * 2 + 1] + (unsigned int)hz[k1 * 2 + 1]) >> 1);
		bits = (avg[0] + 0x77000) << 11 | 0x7FF;
		Unknown1E8[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_2);
		bits = (avg[1] + 0x77000) << 11 | 0x7FF;
		Unknown1EC[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_2);
		h[0] = (unsigned char)(avg[0] >> 8);
		h[1] = (unsigned char)(avg[1] >> 8);
		h[2] = (unsigned char)(((unsigned int)a[k0 * 3 + 1] + (unsigned int)a[k1 * 3 + 1]) >> 1);
		h[3] = (unsigned char)(((unsigned int)b[k0 * 3 + 1] + (unsigned int)b[k1 * 3 + 1]) >> 1);
		h[4] = (unsigned char)(((unsigned int)a[k0 * 3] + (unsigned int)a[k1 * 3]) >> 1);
		h[5] = (unsigned char)(((unsigned int)b[k0 * 3 + 2] + (unsigned int)b[k1 * 3 + 2]) >> 1);
		h[6] = (unsigned char)(((unsigned int)b[k0 * 3] + (unsigned int)b[k1 * 3]) >> 1);
		h[7] = (unsigned char)(((unsigned int)a[k0 * 3 + 2] + (unsigned int)a[k1 * 3 + 2]) >> 1);
		rgb = &Colors[((y << log2) + size) * 3 - 3];
		{
			Vector3 color((float)rgb[2] * (1.0f / 255.0f), (float)rgb[1] * (1.0f / 255.0f), (float)rgb[0] * (1.0f / 255.0f));
			Unknown_472F80(&Unknown1F0[n], &Unknown1F4[n], &Unknown1F8[n], h, Unknown1E4[n], color);
		}
		n++;
	}
	// the last row: averages with the first
	k0 = last_row << log2;
	k1 = wrap << log2;
	k2 = k0 + mask;
	k3 = k1 + mask;
	avg[0] = (unsigned short)(((unsigned int)hz[k0 * 2 + 0] + (unsigned int)hz[k1 * 2 + 0] + (unsigned int)hz[k2 * 2 + 0] + (unsigned int)hz[k3 * 2 + 0]) >> 2);
	avg[1] = (unsigned short)(((unsigned int)hz[k0 * 2 + 1] + (unsigned int)hz[k1 * 2 + 1] + (unsigned int)hz[k2 * 2 + 1] + (unsigned int)hz[k3 * 2 + 1]) >> 2);
	bits = (avg[0] + 0x77000) << 11 | 0x7FF;
	Unknown1E8[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_4);
	bits = (avg[1] + 0x77000) << 11 | 0x7FF;
	Unknown1EC[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_4);
	h[0] = (unsigned char)(avg[0] >> 8);
	h[1] = (unsigned char)(avg[1] >> 8);
	h[2] = (unsigned char)(((unsigned int)a[k0 * 3 + 1] + (unsigned int)a[k1 * 3 + 1] + (unsigned int)a[k2 * 3 + 1] + (unsigned int)a[k3 * 3 + 1]) >> 2);
	h[3] = (unsigned char)(((unsigned int)b[k0 * 3 + 1] + (unsigned int)b[k1 * 3 + 1] + (unsigned int)b[k2 * 3 + 1] + (unsigned int)b[k3 * 3 + 1]) >> 2);
	h[4] = (unsigned char)(((unsigned int)a[k0 * 3] + (unsigned int)a[k1 * 3] + (unsigned int)a[k2 * 3] + (unsigned int)a[k3 * 3]) >> 2);
	h[5] = (unsigned char)(((unsigned int)b[k0 * 3 + 2] + (unsigned int)b[k1 * 3 + 2] + (unsigned int)b[k2 * 3 + 2] + (unsigned int)b[k3 * 3 + 2]) >> 2);
	h[6] = (unsigned char)(((unsigned int)b[k0 * 3] + (unsigned int)b[k1 * 3] + (unsigned int)b[k2 * 3] + (unsigned int)b[k3 * 3]) >> 2);
	h[7] = (unsigned char)(((unsigned int)a[k0 * 3 + 2] + (unsigned int)a[k1 * 3 + 2] + (unsigned int)a[k2 * 3 + 2] + (unsigned int)a[k3 * 3 + 2]) >> 2);
	rgb = &Colors[(last << log2) * 3];
	{
		Vector3 color((float)rgb[2] * (1.0f / 255.0f), (float)rgb[1] * (1.0f / 255.0f), (float)rgb[0] * (1.0f / 255.0f));
		Unknown_472F80(&Unknown1F0[n], &Unknown1F4[n], &Unknown1F8[n], h, Unknown1E4[n], color);
	}
	n++;
	for (x = 1; x < last; x++) {
		k0 = (last_row << log2) + x;
		k1 = (wrap << log2) + x;
		avg[0] = (unsigned short)(((unsigned int)hz[k0 * 2 + 0] + (unsigned int)hz[k1 * 2 + 0]) >> 1);
		avg[1] = (unsigned short)(((unsigned int)hz[k0 * 2 + 1] + (unsigned int)hz[k1 * 2 + 1]) >> 1);
		bits = (avg[0] + 0x77000) << 11 | 0x7FF;
		Unknown1E8[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_2);
		bits = (avg[1] + 0x77000) << 11 | 0x7FF;
		Unknown1EC[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_2);
		h[0] = (unsigned char)(avg[0] >> 8);
		h[1] = (unsigned char)(avg[1] >> 8);
		h[2] = (unsigned char)(((unsigned int)a[k0 * 3 + 1] + (unsigned int)a[k1 * 3 + 1]) >> 1);
		h[3] = (unsigned char)(((unsigned int)b[k0 * 3 + 1] + (unsigned int)b[k1 * 3 + 1]) >> 1);
		h[4] = (unsigned char)(((unsigned int)a[k0 * 3] + (unsigned int)a[k1 * 3]) >> 1);
		h[5] = (unsigned char)(((unsigned int)b[k0 * 3 + 2] + (unsigned int)b[k1 * 3 + 2]) >> 1);
		h[6] = (unsigned char)(((unsigned int)b[k0 * 3] + (unsigned int)b[k1 * 3]) >> 1);
		h[7] = (unsigned char)(((unsigned int)a[k0 * 3 + 2] + (unsigned int)a[k1 * 3 + 2]) >> 1);
		rgb = &Colors[((last << log2) + x) * 3];
		{
			Vector3 color((float)rgb[2] * (1.0f / 255.0f), (float)rgb[1] * (1.0f / 255.0f), (float)rgb[0] * (1.0f / 255.0f));
			Unknown_472F80(&Unknown1F0[n], &Unknown1F4[n], &Unknown1F8[n], h, Unknown1E4[n], color);
		}
		n++;
	}
	k0 = (last_row << log2) + wrap;
	k1 = (wrap << log2) + wrap;
	k2 = (wrap << log2) + last;
	k3 = (last_row << log2) + last;
	avg[0] = (unsigned short)(((unsigned int)hz[k0 * 2 + 0] + (unsigned int)hz[k1 * 2 + 0] + (unsigned int)hz[k2 * 2 + 0] + (unsigned int)hz[k3 * 2 + 0]) >> 2);
	avg[1] = (unsigned short)(((unsigned int)hz[k0 * 2 + 1] + (unsigned int)hz[k1 * 2 + 1] + (unsigned int)hz[k2 * 2 + 1] + (unsigned int)hz[k3 * 2 + 1]) >> 2);
	bits = (avg[0] + 0x77000) << 11 | 0x7FF;
	Unknown1E8[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_4);
	bits = (avg[1] + 0x77000) << 11 | 0x7FF;
	Unknown1EC[n] = (unsigned short)(atan(*(float *)&bits) * HORIZON_SCALE_4);
	h[0] = (unsigned char)(avg[0] >> 8);
	h[1] = (unsigned char)(avg[1] >> 8);
	h[2] = (unsigned char)(((unsigned int)a[k0 * 3 + 1] + (unsigned int)a[k1 * 3 + 1] + (unsigned int)a[k2 * 3 + 1] + (unsigned int)a[k3 * 3 + 1]) >> 2);
	h[3] = (unsigned char)(((unsigned int)b[k0 * 3 + 1] + (unsigned int)b[k1 * 3 + 1] + (unsigned int)b[k2 * 3 + 1] + (unsigned int)b[k3 * 3 + 1]) >> 2);
	h[4] = (unsigned char)(((unsigned int)a[k0 * 3] + (unsigned int)a[k1 * 3] + (unsigned int)a[k2 * 3] + (unsigned int)a[k3 * 3]) >> 2);
	h[5] = (unsigned char)(((unsigned int)b[k0 * 3 + 2] + (unsigned int)b[k1 * 3 + 2] + (unsigned int)b[k2 * 3 + 2] + (unsigned int)b[k3 * 3 + 2]) >> 2);
	h[6] = (unsigned char)(((unsigned int)b[k0 * 3] + (unsigned int)b[k1 * 3] + (unsigned int)b[k2 * 3] + (unsigned int)b[k3 * 3]) >> 2);
	h[7] = (unsigned char)(((unsigned int)a[k0 * 3 + 2] + (unsigned int)a[k1 * 3 + 2] + (unsigned int)a[k2 * 3 + 2] + (unsigned int)a[k3 * 3 + 2]) >> 2);
	rgb = &Colors[((last << log2) + size) * 3 - 3];
	{
		Vector3 color((float)rgb[2] * (1.0f / 255.0f), (float)rgb[1] * (1.0f / 255.0f), (float)rgb[0] * (1.0f / 255.0f));
		Unknown_472F80(&Unknown1F0[n], &Unknown1F4[n], &Unknown1F8[n], h, Unknown1E4[n], color);
	}
	n++;
}

// NOT EXACT: literal (13 masked rows: the direction loop's x87 stack, the client copies nz and ny where ours copies w.Y and exchanges w.X; attempts.csv 0x472F80)
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
		Vector3 c(nz * w.Y, 0.0f, w.X * ny - w.Y * nx);
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

// NOT EXACT: register-only (1 row: the mask test's cmp byte [ebx + eax], ours [eax + ebx]; attempts.csv 0x46E670)
void LandClass::Compute_Horizons(INIClass &ini)
{
	int index = 0;
	std::vector< RefCountPtr<RenderObjClass> > casters;
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
	memcpy(field_heights, heights, Field->Get_Count() * 2);
	delete [] a;
	delete [] b;
	delete [] c;
	delete [] heights;
	delete [] mask;
}

void LandClass::Unknown_473290(unsigned char **a, unsigned char **b, unsigned char **c)
{
	int size = Field->Get_Size();
	*a = new unsigned char[Field->Get_Count() * 3];
	*b = new unsigned char[Field->Get_Count() * 3];
	*c = new unsigned char[Field->Get_Count() * 4];
	const float &spacing = Field->Get_Spacing();
	float ratio = Field->Get_VScale() / spacing;
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

// NOT EXACT: shape (26 masked rows: the left / right updates' registers and scheduling (the client keeps left in the fild temporary's slot), as 0x473BC0; attempts.csv 0x473780)
void LandClass::Unknown_473780(int row, float ratio, unsigned char *out)
{
	unsigned int row0 = (Field->Get_Mask() & row) << Field->Get_Log2();
	float scale = ratio + ratio;
	float diagonal = scale * 0.70710677f;
	unsigned short *heights = Field->Get_Heights();
	unsigned int row1 = ((row + 1) & Field->Get_Mask()) << Field->Get_Log2();
	for (unsigned int i = 0; i < Field->Get_Size(); i++) {
		unsigned int i1 = (i + 1) & Field->Get_Mask();
		float center = ((float)heights[row1 + i1] + heights[row0 + i1] + heights[row1 + i] + heights[row0 + i]) * 0.25f;
		unsigned int left = i;
		unsigned int right = i1;
		Vector3 best(0.05f, 0.05f, 0.05f);
		const float *recip = Reciprocals;
		float dist = 1.0f;
		unsigned int y = row + 1;
		unsigned int offset = row1;
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

// NOT EXACT: shape (24 masked rows: the left index's register and the loop end's scheduling; the client's frame since the Vector3 best / h / s; attempts.csv 0x473BC0)
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
		unsigned int right = i1;
		unsigned int y = row;
		unsigned int left = i;
		const float *recip = Reciprocals;
		float dist = 1.0f;
		unsigned int offset = row0;
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
**	RECONSTRUCTED: the lighting, the lightmap upload and the horizon tables (client 0x46C730-0x4744C0), the second object of the terrain's code:
**	land.cpp's end-of-file notes describe the three and the evidence for the split.
*/
