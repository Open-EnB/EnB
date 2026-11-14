// Earth & Beyond client (reconstruction): app\Shared\segline_create.cpp (notes at the end)
#include <stdio.h>
#include "segline_create.h"
#include "enbassetmgr.h"
#include "ini.h"
#include "point.h"
#include "texture.h"
#include "shader.h"
#include "colorspace.h"
#include "vector2.h"
#include "vector3.h"
#include "g_assert.h"

// @0x4981E0
Unknown4981E0Class::Unknown4981E0Class(bool create)
	: Line(NULL)
{
	if (create) {
		Line = Create_NEW(new SegmentedLineClass);
	}
}

// @0x4982F0
Unknown4981E0Class::Unknown4981E0Class(const RefCountPtr<SegmentedLineClass> &line)
	: Line(line)
{
}

// @0x498320
Unknown4981E0Class::~Unknown4981E0Class()
{
}

// @0x498350
void Unknown4981E0Class::Unknown_498350(const RefCountPtr<SegmentedLineClass> &line)
{
	Line = line;
}






























// @0x4983A0
void Unknown4981E0Class::Unknown_4983A0(INIClass *ini, const char *section, float scale, const Vector3 &hsv_shift)
{
	FAIL_IF(!Line) return;

	TPoint3D<float> origin(0.0f, 0.0f, 0.0f);
	TPoint3D<float> white(1.0f, 1.0f, 1.0f);
	TPoint3D<float> point;

	float width = ini->Get_Float(section, "Width", 0.0f) * scale;

	RefCountPtr<TextureClass> texture;
	char name[256];
	if (ini->Get_String(section, "Texture", "", name, 255)) {
		texture = Create_NEW(AssetManager->Recolor_Texture(name, (float *)&hsv_shift, MIP_LEVELS_ALL));
	}

	int type = ini->Get_Int(section, "Shader", 0);
	ShaderClass shader;
	switch (type) {
		case 0:
			shader = ShaderClass::_PresetOpaqueSolidShader;
			break;
		case 1:
			shader = ShaderClass::_PresetAdditiveSpriteShader;
			break;
		case 2:
			shader = ShaderClass::_PresetAlphaSpriteShader;
			break;
		case 3:
			shader = ShaderClass::_PresetATestSpriteShader;
			break;
		case 4:
			shader = ShaderClass::_PresetATestBlendSpriteShader;
			break;
		case 5:
			shader = ShaderClass::_PresetScreenSpriteShader;
			break;
		case 6:
			shader = ShaderClass::_PresetMultiplicativeSpriteShader;
			break;
		default:
			shader = ShaderClass::_PresetOpaqueSolidShader;
			break;
	}

















	point = ini->Get_Point(section, "Color", white);
	Vector3 rgb(point.X, point.Y, point.Z);
	Recolor(rgb, hsv_shift);
	float opacity = ini->Get_Float(section, "Opacity", 1.0f);
	if (rgb.X > 0.9961f && rgb.Y > 0.9961f && rgb.Z > 0.9961f && opacity > 0.9961f) {
		shader.Set_Primary_Gradient(ShaderClass::GRADIENT_DISABLE);
	} else {
		shader.Set_Primary_Gradient(ShaderClass::GRADIENT_MODULATE);
	}

	SegLineRendererClass::TextureMapMode mode;
	switch (ini->Get_Int(section, "UVMode", 0)) {
		case 0:
			mode = SegLineRendererClass::UNIFORM_WIDTH_TEXTURE_MAP;
			break;
		case 1:
			mode = SegLineRendererClass::TILED_TEXTURE_MAP;
			break;
		case 2:
			mode = SegLineRendererClass::UNIFORM_LENGTH_TEXTURE_MAP;
			break;
	}
	float tile = ini->Get_Float(section, "UVTile", 1.0f);
	TPoint2D<float> zero_rate(0.0f, 0.0f);
	TPoint2D<float> uv = ini->Get_Point(section, "UVRate", zero_rate);
	Vector2 rate(uv.X, uv.Y);
	int subdivs = ini->Get_Int(section, "SubDivs", 0);
	float noise = ini->Get_Float(section, "Noise", 0.0f) * scale;
	bool freeze = ini->Get_Bool(section, "Freeze", false);
















	Vector3 points[32];
	char entry[256];
	unsigned int pcount;
	for (pcount = 0; pcount != 32; pcount++) {
		sprintf(entry, "Point%d", pcount);
		if (!ini->Is_Present(section, entry)) {
			break;
		}
		point = ini->Get_Point(section, entry, origin);
		points[pcount].Set(point.X * scale, point.Y * scale, point.Z * scale);
	}

	if (pcount < 2) {
		G_ASSERT(pcount == 0);
		points[0].Set(0.0f, 0.0f, 0.0f);
		points[1].Set(0.0f, 0.0f, 0.0f);
		pcount = 2;
	}

	Line->Set_Points(pcount, points);
	Line->Set_Texture(texture.Peek());
	Line->Set_Shader(shader);
	Line->Set_Width(width);
	Line->Set_Color(rgb);
	Line->Set_Opacity(opacity);
	Line->Set_Noise_Amplitude(noise);
	Line->Set_Subdivision_Levels(subdivs);
	Line->Set_Texture_Mapping_Mode(mode);
	Line->Set_Texture_Tile_Factor(tile);
	Line->Set_UV_Offset_Rate(rate);
	Line->Set_Merge_Intersections(true);
	Line->Set_Freeze_Random(freeze);
}

/*
** Reconstruction notes (E&B's own file; __FILE__ e:\build\beta\G\app\shared\segline_create.cpp, asserts on lines
** 72 '!Line' and 189 'pcount == 0').
** - The object: 0x4981E0-0x4989CB plus the switch table 0x4989D0, then its out-of-line templates Create_NEW
**   <SegmentedLineClass> 0x498B90, ~RefCountPtr<SegmentedLineClass> 0x498BC0 (the constructor's unwind) and
**   operator-> 0x498BF0 (0x4983A0's Set_Shader call); accel.cpp's object follows at 0x498C10. Before it, 0x498120
**   (the render target maker proj_man.h names) belongs to another file.
** - The class (Unknown4981E0Class, a neutral name) holds one RefCountPtr<SegmentedLineClass>, Line by the FAIL_IF.
**   0x4981E0 makes a new line when asked; 0x4982F0 wraps one (galaxy_map_robj.cpp's own line); 0x498350, with no
**   caller, sets it.
** - 0x4983A0 reads the line's settings from an INI section: Width and Noise scaled, the texture through
**   ENBAssetManager::Recolor_Texture (name, hsv shift), the shader preset by number (ww3d2's presets by their
**   static initialisers 0x93B240-0x93B390 in shader.cpp's definition order: 0 and default OpaqueSolid 0xC0113C,
**   1 AdditiveSprite, 2 AlphaSprite, 3 ATestSprite, 4 ATestBlendSprite, 5 ScreenSprite, 6 MultiplicativeSprite),
**   the colour through colorspace.h's Recolor, the primary gradient off when colour and opacity are all but 1,
**   UVMode 1 / 2 swapped onto TILED / UNIFORM_LENGTH, and up to 32 points Point0..Point31 (fewer than two: two
**   at the origin).
*/
