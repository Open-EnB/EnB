// Earth & Beyond client (reconstruction): app\client\debris.cpp. Notes at the end of the file.
#include "debris.h"
#include "pointgr.h"
#include "linegrp.h"
#include "shader.h"
#include "..\shared\g_assert.h"

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
**	  filling the share buffers, then the point or line group's Set_Arrays / Render) is not rebuilt yet.
*/
