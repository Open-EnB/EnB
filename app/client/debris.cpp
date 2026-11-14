// Earth & Beyond client (reconstruction): app\client\debris.cpp. Notes at the end of the file.
#include "debris.h"
#include "pointgr.h"
#include "linegrp.h"
#include "shader.h"
#include "camera.h"
#include "rinfo.h"
#include "ww3d.h"
#include "wwprofile.h"
#include <math.h>
#include "..\shared\g_assert.h"

// The hash bytes of Render's lattice (0xB92080, the object's first data)
static unsigned char Unknown_B92080[256] = {  // NAME 1: neutral (D10)
	0x18, 0xBB, 0x5D, 0xD6, 0x1D, 0x9C, 0xD0, 0xAF, 0xDD, 0x76, 0x0C, 0x17, 0xC7, 0x9F, 0x0E, 0x7C,
	0xEE, 0x66, 0x08, 0x54, 0xBC, 0xA3, 0x9A, 0xEF, 0x84, 0x06, 0xC1, 0x95, 0x39, 0x4C, 0xB9, 0xB4,
	0x99, 0xB5, 0xC8, 0xC9, 0x62, 0x93, 0x97, 0x81, 0x25, 0xBD, 0x40, 0xF2, 0x6B, 0xF0, 0x1A, 0x1C,
	0x15, 0xE8, 0xF5, 0x4E, 0x80, 0x7A, 0x9D, 0xDE, 0xD2, 0x0D, 0x4A, 0xBE, 0x85, 0x09, 0xFE, 0xEC,
	0xB3, 0x37, 0x07, 0xAB, 0x27, 0x12, 0x04, 0xD7, 0x65, 0x3D, 0x55, 0x64, 0x88, 0x56, 0x00, 0xF7,
	0x8F, 0x2B, 0x44, 0xE4, 0x77, 0xEB, 0xED, 0x78, 0x6A, 0xF1, 0x3B, 0xCC, 0x79, 0x87, 0x8D, 0x74,
	0x47, 0x8E, 0x53, 0xD1, 0xBA, 0xE5, 0x19, 0x57, 0xB7, 0xC0, 0xB6, 0x14, 0x46, 0x63, 0xFA, 0x6E,
	0x49, 0x0F, 0xE1, 0x24, 0xB8, 0x73, 0x83, 0xC4, 0x5E, 0x94, 0x31, 0x50, 0x1E, 0x71, 0x3E, 0x1F,
	0xCD, 0xA1, 0x4F, 0x82, 0x61, 0x70, 0x20, 0xCA, 0x6C, 0x48, 0xFC, 0x68, 0xDC, 0x60, 0xA8, 0x72,
	0x7F, 0x9B, 0x3F, 0x2E, 0x45, 0xA4, 0xD5, 0xD8, 0x41, 0xFF, 0x6D, 0xDF, 0x36, 0x75, 0xD9, 0xAC,
	0x3A, 0x8A, 0x67, 0x91, 0x5C, 0x59, 0xFB, 0x10, 0xA7, 0x4D, 0xA0, 0xE3, 0x2F, 0x2A, 0x2D, 0xC6,
	0xF4, 0xE6, 0x51, 0x01, 0xE2, 0xD4, 0x0A, 0xE0, 0x96, 0x30, 0x89, 0x5A, 0x21, 0x2C, 0xE9, 0x5B,
	0x11, 0xA5, 0x98, 0x38, 0x03, 0x35, 0xCB, 0xAD, 0x23, 0x7E, 0xDB, 0x52, 0x6F, 0x4B, 0x42, 0x92,
	0xDA, 0x0B, 0x58, 0xF8, 0xB1, 0xB0, 0xAE, 0xB2, 0x32, 0x90, 0xA6, 0xC2, 0x69, 0xF6, 0x29, 0x1B,
	0xAA, 0xC5, 0x02, 0x7D, 0x33, 0xBF, 0xA2, 0x26, 0x28, 0x05, 0x5F, 0x34, 0xF9, 0x86, 0x43, 0xE7,
	0x3C, 0xCF, 0xFD, 0x7B, 0xD3, 0xEA, 0x9E, 0x16, 0xA9, 0x22, 0xCE, 0x13, 0xC3, 0xF3, 0x8C, 0x8B,
};

// 0x6B8F20
DebrisFieldClass::DebrisFieldClass(float spacing, float size, float min_dist, float max_dist, float r, float g, float b, float a, bool additive, TextureClass *texture, float fade_in, float fade_out, int primitive, unsigned rows_columns_log2, float blur_time) :
	Spacing(spacing),
	Size(size),
	MinDist(min_dist),
	MaxDist(max_dist),
	Color(r, g, b),
	Alpha(a),
	FadeIn(fade_in),
	FadeOut(fade_out),
	UnknownB4(10000),
	Additive(additive),
	UnknownBC(2),
	PointGroup(NULL),
	LineGroup(NULL),
	UnknownC8(1.0f),
	UnknownCC(NULL),
	UnknownD0(NULL),
	UnknownD4(0),
	UnknownD8(NULL),
	UnknownDC(NULL),
	UnknownE0(NULL),
	UnknownE4(NULL),
	UnknownE8(0, 0, 0),
	BlurTime(blur_time),
	UnknownF8(0, 0, 0),
	Primitive(primitive)
{
	switch (primitive) {
		case 0:
			PointGroup = new PointGroupClass;
			if (texture) {
				PointGroup->Set_Texture(texture);
			}
			PointGroup->Set_Frame_Row_Column_Count_Log2(rows_columns_log2);
			PointGroup->Set_Point_Mode(PointGroupClass::TRIS);
			UnknownD8 = new ShareBufferClass<unsigned char>(UnknownB4);
			break;
		case 1:
			PointGroup = new PointGroupClass;
			if (texture) {
				PointGroup->Set_Texture(texture);
			}
			PointGroup->Set_Frame_Row_Column_Count_Log2(rows_columns_log2);
			PointGroup->Set_Point_Mode(PointGroupClass::QUADS);
			UnknownD8 = new ShareBufferClass<unsigned char>(UnknownB4);
			break;
		case 2:
			LineGroup = new LineGroupClass;
			if (texture) {
				LineGroup->Set_Texture(texture);
			}
			LineGroup->Set_Line_Mode(LineGroupClass::TETRAHEDRON);
			UnknownDC = new ShareBufferClass<Vector3>(UnknownB4);
			UnknownE4 = new ShareBufferClass<float>(UnknownB4);
			break;
		case 3:
			LineGroup = new LineGroupClass;
			if (texture) {
				LineGroup->Set_Texture(texture);
			}
			LineGroup->Set_Line_Mode(LineGroupClass::PRISM);
			UnknownDC = new ShareBufferClass<Vector3>(UnknownB4);
			UnknownE4 = new ShareBufferClass<float>(UnknownB4);
			break;
		default:
















			G_ASSERT(0);
			break;
	}
	Unknown_6BB470();
	UnknownCC = new ShareBufferClass<Vector3>(UnknownB4);
	UnknownD0 = new ShareBufferClass<Vector4>(UnknownB4);
	Bits |= IS_FORCE_VISIBLE;
}

// 0x6B96A0
DebrisFieldClass::DebrisFieldClass(const DebrisFieldClass &src) :
	Spacing(src.Spacing),
	Size(src.Size),
	MinDist(src.MinDist),
	MaxDist(src.MaxDist),
	Color(src.Color),
	Alpha(src.Alpha),
	FadeIn(src.FadeIn),
	FadeOut(src.FadeOut),
	UnknownB4(src.UnknownB4),
	Additive(src.Additive),
	UnknownBC(src.UnknownBC),
	UnknownC8(src.UnknownC8),
	UnknownF8(src.UnknownF8),
	Primitive(src.Primitive)
{




















	G_ASSERT(0);
}

// 0x6B9850
DebrisFieldClass &DebrisFieldClass::operator=(const DebrisFieldClass &that)
{
	RenderObjClass::operator=(that);
	if (this != &that) {
		Spacing = that.Spacing;
		Size = that.Size;
		MinDist = that.MinDist;
		MaxDist = that.MaxDist;
		Color = that.Color;
		Alpha = that.Alpha;
		Additive = that.Additive;
		FadeIn = that.FadeIn;
		FadeOut = that.FadeOut;
		UnknownC8 = that.UnknownC8;
		UnknownF8 = that.UnknownF8;
		Primitive = that.Primitive;


















		G_ASSERT(0);
	}
	return *this;
}

// 0x6B9990
DebrisFieldClass::~DebrisFieldClass()
{
	if (PointGroup) {
		delete PointGroup;
		PointGroup = NULL;
	}
	if (LineGroup) {
		delete LineGroup;
		LineGroup = NULL;
	}
	REF_PTR_RELEASE(UnknownCC);
	REF_PTR_RELEASE(UnknownD0);
	REF_PTR_RELEASE(UnknownD8);
	REF_PTR_RELEASE(UnknownDC);
	REF_PTR_RELEASE(UnknownE0);
	REF_PTR_RELEASE(UnknownE4);
}

// 0x6B9B70
RenderObjClass *DebrisFieldClass::Clone(void) const
{
	return new DebrisFieldClass(*this);
}

// 0x6B9BF0
// NOT EXACT: 0.53, frame (sub esp 0xA5C against 0xA54); written from the client's code, the loops and the stack slots differ
void DebrisFieldClass::Render(RenderInfoClass &rinfo)
{
	WWPROFILE("DebrisFieldClass::Render");
	unsigned int sort_level = 0;
	if (!WW3D::Is_Sorting_Enabled()) {
		if (PointGroup) {
			sort_level = PointGroup->Get_Shader().Guess_Sort_Level();
		} else if (LineGroup) {
			sort_level = LineGroup->Get_Shader().Guess_Sort_Level();
		}
	}
	if (WW3D::Are_Static_Sort_Lists_Enabled() && sort_level != 0) {
		WW3D::Add_To_Static_Sort_List(this, sort_level);
	} else {

	CameraClass *camera = new CameraClass(rinfo.Camera);
	camera->Set_Position(Get_Position() + UnknownF8);
	float znear;
	float zfar;
	camera->Get_Clip_Planes(znear, zfar);
	float size = Size;
	float min_dist = znear / cos(camera->Get_Vertical_FOV() * 0.5f) + size;
	if (MinDist < min_dist) {
		MinDist = min_dist;
		if (MaxDist <= min_dist) {
			MaxDist = min_dist * 1.1f;
		}
	}
	UnknownD4 = 0;
	if (PointGroup) {
		PointGroup->Set_Point_Size(size);
	} else if (LineGroup) {
		LineGroup->Set_Line_Size(size);
	}

	FrustumClass frustum = camera->Get_Frustum();
	Vector2 vpmin;
	Vector2 vpmax;
	camera->Get_View_Plane(vpmin, vpmax);
	frustum.Init(frustum.CameraTransform, vpmin, vpmax, MinDist, MaxDist);

	Vector3 bmax(frustum.BoundMax.X + Size, frustum.BoundMax.Y + Size, frustum.BoundMax.Z + Size);
	Vector3 bmin(frustum.BoundMin.X - Size, frustum.BoundMin.Y - Size, frustum.BoundMin.Z - Size);
	float inv_spacing = 1.0 / Spacing;
	float f = floor(inv_spacing * bmin.X);
	int x0 = f;
	bmin.X = f * Spacing;
	f = floor(inv_spacing * bmin.Y);
	int y0 = f;
	bmin.Y = f * Spacing;
	f = floor(inv_spacing * bmin.Z);
	int z0 = f;
	bmin.Z = f * Spacing;
	int x1 = ceil(inv_spacing * bmax.X);
	int y1 = ceil(inv_spacing * bmax.Y);
	int z1 = ceil(inv_spacing * bmax.Z);

	Matrix3D view;
	camera->Get_Transform().Get_Inverse(view);
	Vector3 cur = view * bmin;
	Vector3 step_x = view.Rotate_Vector(Vector3(Spacing, 0.0f, 0.0f));
	Vector3 step_y = view.Rotate_Vector(Vector3(0.0f, Spacing, 0.0f));
	Vector3 step_z = view.Rotate_Vector(Vector3(0.0f, 0.0f, Spacing));

	static Vector3 offsets[8] = {
		Vector3(0.4f, 0.57f, 0.56f),
		Vector3(0.58f, 0.57f, 0.41f),
		Vector3(0.28f, 0.25f, 0.27f),
		Vector3(0.26f, 0.25f, 0.73f),
		Vector3(0.28f, 0.7f, 0.26f),
		Vector3(0.71f, 0.72f, 0.75f),
		Vector3(0.75f, 0.3f, 0.26f),
		Vector3(0.7f, 0.25f, 0.73f)
	};
	Vector3 jitter[8];
	for (int i = 0; i < 8; i++) {
		jitter[i] = view.Rotate_Vector(Spacing * offsets[i]);
	}

	float alpha_ramp[512];
	float inv_max = 1.0f / MaxDist;
	float inv_max2 = inv_max * inv_max;
	float near_r = inv_max * MinDist;
	float fade_in = (1.0f - near_r) * FadeOut;
	float fade_out = (1.0f - FadeIn) * (1.0f - near_r);
	float full_r = fade_in + near_r;
	float far_r = 1.0f - fade_out;
	float inv_fade_in = 1.0f / fade_in;
	float inv_fade_out = 1.0f / fade_out;
	float r2 = 0.0f;
	for (i = 0; i < 512; i++) {
		float r = sqrt(r2);
		if (r < near_r) {
			alpha_ramp[i] = 0.0f;
		} else if (r < full_r) {
			alpha_ramp[i] = (r - near_r) * UnknownC8 * inv_fade_in;
		} else if (r < far_r) {
			alpha_ramp[i] = UnknownC8;
		} else {
			alpha_ramp[i] = (1.0f - (r - far_r) * inv_fade_out) * UnknownC8;
		}
		r2 += 1.0f / 512.0f;
	}

	Vector3 back_y = step_y * (float)(y1 - y0);
	Vector3 back_z = step_z * (float)(z1 - z0);
	Vector3 *locs = UnknownCC->Get_Array();
	Vector4 *diffuse = UnknownD0->Get_Array();
	unsigned char *frames = NULL;
	if (UnknownD8) {
		frames = UnknownD8->Get_Array();
	}
	float *ucoords = NULL;
	if (UnknownE4) {
		ucoords = UnknownE4->Get_Array();
	}
	int x;
	int y;
	int z;
	switch (Unknown_6BB440()) {
		case 0:
			for (x = x0; x < x1; x++) {
				for (y = y0; y < y1; y++) {
					int xy = y ^ x;
					for (z = z0; z < z1; z++) {
						unsigned int hash = z ^ xy;
						int corner = (((hash >> 3) & 0x38) ^ (hash & 0x38)) >> 3 ^ (hash & 7);
						locs[UnknownD4] = cur + jitter[corner];
						float d2 = locs[UnknownD4].Length2() * inv_max2;
						if (d2 < 1.0f) {
							float alpha = alpha_ramp[(int)floor(d2 * 512.0f)];
							diffuse[UnknownD4].Set(Color.X, Color.Y, Color.Z, alpha * Alpha);
							if (frames) {
								frames[UnknownD4] = Unknown_B92080[(((hash >> 16) & 0xFF) << 8 ^ (hash & 0xFF00)) >> 8 ^ (hash & 0xFF)];
							}
							if (ucoords) {
								ucoords[UnknownD4] = Unknown_B92080[(((hash >> 16) & 0xFF) << 8 ^ (hash & 0xFF00)) >> 8 ^ (hash & 0xFF)] * (1.0f / 255.0f);
							}
							if (++UnknownD4 >= (unsigned)UnknownB4) {
								x = x1;
								y = y1;
								z = z1;
							}
						}
						cur += step_z;
					}
					cur += step_y - back_z;
				}
				cur += step_x - back_y;
			}
			break;
		case 1:
			for (x = x0; x < x1; x++) {
				for (y = y0; y < y1; y++) {
					int xy = y ^ x;
					for (z = z0; z < z1; z++) {
						unsigned int hash = z ^ xy;
						int corner = (((hash >> 3) & 0x38) ^ (hash & 0x38)) >> 3 ^ (hash & 7);
						locs[UnknownD4] = cur + jitter[corner];
						float d2 = locs[UnknownD4].Length2() * inv_max2;
						if (d2 < 1.0f) {
							float alpha = alpha_ramp[(int)floor(d2 * 512.0f)];
							diffuse[UnknownD4].Set(alpha * Color.X, alpha * Color.Y, alpha * Color.Z, Alpha);
							if (frames) {
								frames[UnknownD4] = Unknown_B92080[(((hash >> 16) & 0xFF) << 8 ^ (hash & 0xFF00)) >> 8 ^ (hash & 0xFF)];
							}
							if (ucoords) {
								ucoords[UnknownD4] = Unknown_B92080[(((hash >> 16) & 0xFF) << 8 ^ (hash & 0xFF00)) >> 8 ^ (hash & 0xFF)] * (1.0f / 255.0f);
							}
							if (++UnknownD4 >= (unsigned)UnknownB4) {
								x = x1;
								y = y1;
								z = z1;
							}
						}
						cur += step_z;
					}
					cur += step_y - back_z;
				}
				cur += step_x - back_y;
			}
			break;
		default:



































































































































































			G_ASSERT(0);
			break;
	}

	if (LineGroup) {
		Vector3 *ends = UnknownDC->Get_Array();
		Vector4 *tails = NULL;
		if (UnknownE0 == NULL) {
			LineGroup->Set_Tail_Diffuse(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
		} else {
			tails = UnknownE0->Get_Array();
		}
		Vector3 blur;
		Matrix3D::Rotate_Vector(view, UnknownE8, &blur);
		blur.Normalize();
		blur *= BlurTime;
		for (unsigned int n = 0; n < (unsigned)UnknownD4; n++) {
			ends[n] = locs[n] + blur;
			if (tails) {
				tails[n].Set(diffuse[n].X, diffuse[n].Y, diffuse[n].Z, 0.0f);
			}
		}
	}

	static Matrix3D identity(true);
	if (PointGroup) {
		PointGroup->Set_Arrays(UnknownCC, UnknownD0, NULL, NULL, UnknownD8, UnknownD8, UnknownD4);
		PointGroup->Render(rinfo, identity);
	} else if (LineGroup) {
		LineGroup->Set_Arrays(UnknownCC, UnknownDC, UnknownD0, UnknownE0, NULL, NULL, UnknownE4, UnknownD4);
		LineGroup->Render(rinfo, identity);
	}
	if (camera) {
		camera->Release_Ref();
	}
	}
}

// 0x6BB440
int DebrisFieldClass::Unknown_6BB440()
{
	return(Additive || UnknownBC != 0);
}

// 0x6BB470
void DebrisFieldClass::Unknown_6BB470()
{
	ShaderClass shader;
	if (Additive) {
		shader = ShaderClass::_PresetAdditiveSpriteShader;
		if (UnknownE0) {
			UnknownE0->Release_Ref();
			UnknownE0 = NULL;
		}
	} else {
		if (LineGroup) {
			UnknownE0 = new ShareBufferClass<Vector4>(UnknownB4);
		}
		switch (UnknownBC) {
			case 0:
				shader = ShaderClass::_PresetAlphaSpriteShader;
				break;
			case 1:
				shader = ShaderClass::_PresetATestSpriteShader;
				break;
			case 2:
				shader = ShaderClass::_PresetATestBlendSpriteShader;
				break;
		}
	}
	if (PointGroup) {
		PointGroup->Set_Shader(shader);
	} else if (LineGroup) {
		LineGroup->Set_Shader(shader);
	}
}

// 0x6BB640
void DebrisFieldClass::Unknown_6BB640(const Vector3 &offset)
{
	if (UnknownF8.Length2() > 0.0f) {
		UnknownE8 = UnknownF8 - offset;
	}
	UnknownF8 = offset;
}


/*
**	Reconstruction notes (debris.cpp, client 0x6B8F20-0x6BB740; __FILE__ 0xB921B4, G_ASSERT(0) on the client's lines
**	115 (the constructor's unknown primitive), 161 (the copy constructor) and 199 (operator=), 577 (Render); D25:
**	blank lines keep them there).
**
**	- The object opens at 0x6B8F20 (0x6B8F00 / 0x6B8F10 before it are the previous object's adjustor thunks to 0x6B8540)
**	  and its own code ends at 0x6BB6D8; ShareBufferClass<Vector4>'s deleting destructor 0x6BB710 follows the int3 run
**	  and its destructor 0x6BB740 (shared by the identical <unsigned char>, <Vector3>, <float> instances) after it.
**	- Class_ID (0x10006, 0x6B9620) and Get_Num_Polys (0x6B9640) are inline: emitted with the table after the
**	  constructor, before the deleting destructor 0x6B9670.
**	- The copy constructor builds the base with RenderObjClass's default constructor (no argument pushed) and copies
**	  only the settings; operator= calls RenderObjClass::operator= before its self test.
**	- 0x6BB470's shaders are ww3d2's presets by their static initialisers (0x93B2D0-0x93B350 in definition order):
**	  0xC010F8 AdditiveSprite, 0xC01174 AlphaSprite, 0xC0117C ATestSprite, 0xC01184 ATestBlendSprite.
**	- Render 0x6B9BF0 (6 KB: a CameraClass copy of the frame's camera, its frustum, a function-static table of eight
**	  Vector3 offsets at 0xBE66F8 (guard 0xBE66F0, atexit 0x6BB420), a 512-entry alpha ramp, a hashed lattice walk
**	  filling the share buffers, then the point or line group's Set_Arrays / Render with a static identity 0xBE6758)
**	  is written from the client's code, NOT EXACT. Unknown_B92080 is the object's first data (after bar3d.cpp's
**	  __FILE__ string), 256 hash bytes.
*/
