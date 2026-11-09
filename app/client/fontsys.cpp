// Earth & Beyond client (reconstruction): app\client\fontsys.cpp. Notes at the end of the file.
#include "fontmanager.h"
#include "fontdatabase.h"
#include "font32bit.h"
#include "..\shared\g_assert.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\..\ww3d2\dx8wrapper.h"
#include "..\..\ww3d2\texture.h"
#include "..\..\ww3d2\rinfo.h"
#include "..\..\wwmath\aabox.h"
#include "..\..\wwmath\sphere.h"
#include <math.h>
#include <string.h>
#include "unknown_fontsound.h"
#include "..\..\ww3d2\shader.h"
#include "wwprofile.h"

// NOT EXACT: written from the client's code (0x6D47C0) for the POC, not yet matched
FontSys::FontSys(const char *name, RenderObjClass *object, const char *text) :
	DynamicMeshClass(2, 4),
	Font(NULL)
{
	FontStringStruct *def = Unknown_BE67B8->Find_String(name);
	if (def != NULL) {
		Info = *def;
	}
	if (text != NULL) {
		Info.Text = text;
	}
	if (Info.Text.Is_Empty()) {
		Info.Text = " ";
	}
	Unknown_6D5170(object);
	Unknown_6D6390();
	Unknown_6D5D40();
	Unknown26C = (int)(Info.Wait * 1000.0f);
}

// NOT EXACT: written from the client's code (0x6D4C70) for the POC, not yet matched
FontSys::FontSys(FontStringStruct def, RenderObjClass *object, const char *text) :
	DynamicMeshClass(2, 4),
	Font(NULL)
{
	Info = def;
	if (text != NULL) {
		Info.Text = text;
	}
	if (Info.Text.Is_Empty()) {
		Info.Text = " ";
	}
	Unknown_6D5170(object);
	Unknown_6D6390();
	Unknown_6D5D40();
	Unknown26C = (int)(Info.Wait * 1000.0f);
}

// 0x6D5140: out of its container, then the reference dropped (~ManagedFontClass, 0x6D0A70)
void FontSys::Unknown_6D5140()
{
	Remove();
	Release_Ref();
}

// NOT EXACT: shape (attempts.csv): the client reaches Width..Scale (+0x2B4..+0x2CC) through one pointer, as a sub-object's
// inline would, and stores +0x430 / +0x4C5 after the resolution call's arguments; the colours from TXT1
void FontSys::Unknown_6D5170(RenderObjClass *object)
{
	int i;
	Unknown4DC = 1.0f;
	Unknown4E0 = 1.0f;
	Object = object;
	Unknown300 = true;
	Unknown301 = false;
	Unknown402 = false;
	Unknown302 = false;
	Unknown404 = false;
	Unknown405 = false;
	Unknown406 = false;
	Unknown408 = false;
	Unknown4E5 = true;
	Unknown440 = 0;
	FontIndex = 0;
	Unknown450 = 0;
	Unknown250 = 2;
	Unknown410 = 0;
	Unknown444 = 0;
	Unknown2F4[0] = 1.0f;
	Unknown2F4[1] = -1.0f;
	Unknown2F4[2] = 1.0f;
	Unknown448 = 0;
	Unknown454 = 0.0f;
	Unknown458 = 0;
	LineCount = 0;
	LineNumber = 0;
	Unknown418 = 0;
	Unknown41C = 0;
	Unknown420[0] = 0;
	Unknown420[2] = 0;
	Unknown420[1] = 1000;
	Unknown420[3] = 1000;
	Unknown488[2] = 0;
	Unknown488[1] = 0;
	Unknown488[0] = 0;
	Unknown4C5 = false;
	Unknown4C8[0] = -1;
	Unknown4C8[1] = -1;
	Unknown4D0.X = 0.0f;
	Unknown4D0.Y = 0.0f;
	Unknown4D0.Z = 0.0f;
	Unknown47C.Z = 0.0f;
	Unknown47C.X = Info.Offset[0] * 640.0f;
	Unknown47C.Y = Info.Offset[1] * 480.0f;
	Unknown470 = Unknown47C;
	Unknown430[0] = -320;
	Unknown430[2] = 240;
	Unknown430[1] = 320;
	Unknown430[3] = -240;
	Unknown4C5 = false;
	WW3D::Get_Device_Resolution(Width, Height, Bits, Windowed);
	Unknown2C4 = 0;
	Unknown2C8 = 0;
	Scale = 1.0f;
	Unknown26C = 0;
	Unknown270 = 0;
	Unknown274[0] = 0; Unknown274[1] = 0; Unknown274[2] = 0; Unknown274[3] = 0; Unknown274[4] = 0; Unknown274[5] = 0; Unknown274[6] = 0; Unknown274[7] = 0; Unknown274[8] = 0; Unknown274[9] = 0; Unknown274[10] = 0; Unknown274[11] = 0;
	for (i = 0; i < 4; i++) Unknown274[12 + i] = 0; Unknown254[0] = 0; Unknown254[1] = 0; Unknown254[2] = 0; Unknown254[3] = 0; Unknown254[4] = 0; Unknown254[5] = 0;
	for (i = 0; i < 4; i++) {
		Effects[0][i] = Unknown_BE67B8->Find_Effect(Info.EffectOn[i].Get_Buffer(Info.EffectOn[i].Get_Length()));
		Effects[1][i] = Unknown_BE67B8->Find_Effect(Info.EffectConst[i].Get_Buffer(Info.EffectConst[i].Get_Length()));
		Effects[2][i] = Unknown_BE67B8->Find_Effect(Info.EffectOff[i].Get_Buffer(Info.EffectOff[i].Get_Length()));
		Effects[3][i] = Unknown_BE67B8->Find_Effect(Info.MouseOver[i].Get_Buffer(Info.MouseOver[i].Get_Length()));
		Effects[4][i] = Unknown_BE67B8->Find_Effect(Info.MouseClick[i].Get_Buffer(Info.MouseClick[i].Get_Length()));
		Effects[5][i] = Unknown_BE67B8->Find_Effect(Info.EffectTriggered[i].Get_Buffer(Info.EffectTriggered[i].Get_Length()));
	}
	if (Effects[0][0] == NULL && Effects[0][1] == NULL && Effects[0][2] == NULL) {
		Unknown440 = 1;
	}
	Unknown44C = 0;
	Colors[0] = Unknown_BE67B8->Find_Color(Info.Color[0]);
	Colors[1] = Unknown_BE67B8->Find_Color(Info.Color[1]);
	if (Colors[1] == NULL) {
		Colors[1] = Colors[0];
	}
	float *color = (float *)&Unknown494;	// floats r, g, b, a (TXT1: Update_Text reads them so)
	float *second = (float *)&Unknown4B4;
	if (Colors[0] != NULL) {
		const int *rgb = (const int *)Colors[0];	// FontColorStruct (fontdatabase.cpp): +4 / +8 / +0xC r, g, b
		color[0] = (float)rgb[1] * 0.00392156f;
		color[1] = (float)rgb[2] * 0.00392156f;
		color[2] = (float)rgb[3] * 0.00392156f;
		color[3] = 1.0f;
		Unknown4A4 = Unknown494;
		rgb = (const int *)Colors[1];
		second[0] = (float)rgb[1] * 0.00392156f;
		second[1] = (float)rgb[2] * 0.00392156f;
		second[2] = (float)rgb[3] * 0.00392156f;
		second[3] = 1.0f;
	} else {
		color[0] = 10.0f;
		color[1] = 10.0f;
		color[2] = 10.0f;
		color[3] = 1.0f;
		Unknown4A4 = Unknown494;
		second[0] = 0.0f;
		second[1] = 0.0f;
		second[2] = 0.0f;
		second[3] = 1.0f;
	}
	memset(Unknown303, 0, sizeof(Unknown303));
	Unknown414 = 0;
	FontStruct *font = Unknown_BE67B8->Find_Font(Info.FontType[FontIndex], Scale);
	Font = font->font32Ptr;
	Underline = Info.Underline;
	if (Info.Underline) {
		Unknown2E0.Unknown_6D8DC0(&Font, "_", 0);
	}
	Unknown4C5 = false;
	Unknown4E4 = true;
	Unknown4E6 = false;
}

// NOT EXACT: register (attempts.csv): the client keeps i in ebp and spills this across the loop (14 rows)
void FontSys::Set_Container(RenderObjClass *con)
{
	if (con != NULL) {
		int level = con->Get_Sort_Level();
		int count = con->Get_Num_Sub_Objects();
		for (int i = 0; i < count; i++) {
			RenderObjClass *sub = con->Get_Sub_Object(i);
			if (sub != NULL) {
				if (sub->Class_ID() != 0x1000D) {
					int sub_level = sub->Get_Sort_Level();
					if (sub_level != SORT_LEVEL_NONE && (sub_level < level || level == SORT_LEVEL_NONE)) {
						level = sub_level;
					}
				}
				sub->Release_Ref();
			}
		}
		switch (level) {
			case SORT_LEVEL_NONE:
				break;
			case 1:
				Set_Sort_Level(1);
				Disable_Sort();
				RenderObjClass::Set_Container(con);
				return;
			default:
				Set_Sort_Level(level - 1);
				Disable_Sort();
				RenderObjClass::Set_Container(con);
				return;
		}
	} else {
		Set_Sort_Level(SORT_LEVEL_NONE);
	}
	Enable_Sort();
	RenderObjClass::Set_Container(con);
}

// 0x6D5A80: a character cell's size: the text rectangle in 640 x 480 units, times Scale
void FontSys::Unknown_6D5A80(float &width, float &height)
{
	width = (640.0f / (float)Width) * (float)(Unknown420[1] - Unknown420[0]);
	height = (480.0f / (float)Height) * (float)(Unknown420[3] - Unknown420[2]);
	width *= Scale;
	height *= Scale;
}

// 0x6D5B90
void FontSys::Get_Obj_Space_Bounding_Sphere(SphereClass &sphere) const
{
	AABoxClass box;
	Get_Obj_Space_Bounding_Box(box);
	sphere.Center = box.Center;
	sphere.Radius = (box.Extent.X > box.Extent.Y) ? box.Extent.X : box.Extent.Y;
}

// NOT EXACT: written from the client's code (0x6D5C10) for the POC, not yet matched
void FontSys::Get_Obj_Space_Bounding_Box(AABoxClass &box) const
{
	float x = (float)(Unknown420[1] - Unknown420[0]) * (640.0f / (float)Width) * 0.5f;
	float y = (float)(Unknown420[3] - Unknown420[2]) * (480.0f / (float)Height) * 0.5f;
	if (Info.Justify[1] == 3) {
		box.Center.X = x;
	} else if (Info.Justify[1] == 4) {
		box.Center.X = -x;
	} else {
		box.Center.X = 0.0f;
	}
	if (Info.Justify[0] == 1) {
		box.Center.Y = -y;
	} else if (Info.Justify[0] == 2) {
		box.Center.Y = y;
	} else {
		box.Center.Y = 0.0f;
	}
	box.Center.Z = 0.0f;
	box.Extent.X = x;
	box.Extent.Y = y;
	box.Extent.Z = 0.0f;
}

// 0x6D5D40: the screen rectangle +0x430 from the attach bone's box (clamped to 640 x 480 about the centre)
void FontSys::Unknown_6D5D40()
{
	if (Object != NULL) {
		RenderObjClass *sub = Object->Get_Sub_Object_By_Name(Info.Attach);
		if (sub != NULL) {
			sub->Get_Position();
			AABoxClass box = sub->Get_Bounding_Box();
			int value = (int)(box.Center.X - box.Extent.X + Info.BoxOffset[0]);
			Unknown430[0] = value < -320 ? -320 : (value > 320 ? 320 : value);
			value = (int)(box.Center.X + box.Extent.X - Info.BoxOffset[0]);
			Unknown430[1] = value < -320 ? -320 : (value > 320 ? 320 : value);
			value = (int)(box.Center.Y + box.Extent.Y - Info.BoxOffset[1]);
			Unknown430[2] = value < -240 ? -240 : (value > 240 ? 240 : value);
			value = (int)(box.Center.Y - box.Extent.Y + Info.BoxOffset[1]);
			Unknown430[3] = value < -240 ? -240 : (value > 240 ? 240 : value);
			sub->Release_Ref();
			Unknown250 = 1;
			return;
		}
		Unknown250 = 2;
	}
}

// 0x6D6390: the text printed, sized and shown; +0x408 says it is the blank " " (the constructors' default)
void FontSys::Unknown_6D6390()
{
	const char *text = Info.Text;
	if (strcmp(text, " ") == 0) Unknown408 = true; else Unknown408 = false;
	Unknown4C4 = Unknown2D0.Unknown_6D8DC0(&Font, text, 0);
	Unknown420[1] = Unknown2C4 = Font->String_Pixel_Width(text);
	Unknown420[3] = Unknown2C8 = Font->Get_Height();
	Update_Text(false);
	if (Object != NULL && Info.Attach.Get_Length() > 0) {
		Object->Add_Sub_Object_To_Bone(this, Info.Attach);
	}
}

// 0xBFAE3C / 0xBFAE40 are WW3D::SyncTime / PreviousSyncTime (WW3D::Sync 0x918200 writes them): the frame time is
// WW3D::Get_Frame_Time() (the client reads the two statics inline).
extern Unknown_6BCFB0Class *Unknown_BE67B4;	// NAME 1 [Unknown_BE67B4]: neutral (D10): FontSys's sounds (main_client.cpp declares it so)

// 0x6D5F40: the shader for Info.BlendType[FontIndex]; every case ends with no depth writes and an always-pass compare
void FontSys::Unknown_6D5F40(ShaderClass &shader)
{
	switch (Info.BlendType[FontIndex]) {
		case 0:
			shader = ShaderClass::_PresetAlphaShader;
			break;
		case 1:
			shader = ShaderClass::_PresetAlphaShader;
			shader.Set_Dst_Blend_Func(ShaderClass::DSTBLEND_ONE_MINUS_SRC_COLOR);
			break;
		case 2:
			shader = ShaderClass::_PresetAlphaShader;
			shader.Set_Dst_Blend_Func(ShaderClass::DSTBLEND_ONE);
			break;
		case 3:
			shader = ShaderClass::_PresetAlphaShader;
			shader.Set_Dst_Blend_Func(ShaderClass::DSTBLEND_SRC_ALPHA);
			break;
		case 4:
			shader = ShaderClass::_PresetMultiplicativeShader;
			shader.Set_Dst_Blend_Func(ShaderClass::DSTBLEND_SRC_COLOR);
			break;
		case 5:
			shader = ShaderClass::_PresetOpaqueShader;
			break;
		case 6:
			shader = ShaderClass::_PresetOpaqueShader;
			shader.Set_Alpha_Test(ShaderClass::ALPHATEST_ENABLE);
			break;
		default:
			shader = ShaderClass::_PresetAlphaShader;
			break;
	}
	shader.Set_Depth_Mask(ShaderClass::DEPTH_WRITE_DISABLE);
	shader.Set_Depth_Compare(ShaderClass::PASS_ALWAYS);
}

// 0x6D60C0: the delay before the text shows (+0x26C ms, counted in +0x270); true while it lasts. Plays Info.SoundOn
// when it ends (or at once without one) unless +0x440 says otherwise.
// NOT EXACT: written from the client's code (0x6D60C0) for the POC, not yet matched
bool FontSys::Unknown_6D60C0()
{
	if (Unknown274[6] == 0) {
		Unknown274[6] = 1;
		return false;
	}
	int frame = WW3D::Get_Frame_Time();
	if (Unknown270 < Unknown26C) {
		Unknown274[0] = Unknown270;
		Bits &= ~IS_NOT_HIDDEN;
		Unknown270 += frame;
		if (Unknown270 < Unknown26C) {
			return true;
		}
	}
	if (Unknown26C > 0) {
		if (Unknown300) {
			Bits |= IS_NOT_HIDDEN;
		}
		Unknown270 -= Unknown26C;
		Unknown274[0] -= Unknown26C;
		Unknown26C = 0;
		Unknown274[1] = 0;
		if (Unknown440 == 0 && Unknown300) {
			Unknown_BE67B4->Play(Info.SoundOn);
		}
		return false;
	}
	if (Unknown440 == 0 && Unknown270 == 0 && Unknown300) {
		Unknown_BE67B4->Play(Info.SoundOn);
	}
	if (Unknown274[6] != 0) {
		Unknown274[0] = Unknown270;
		Unknown270 += frame;
		Unknown274[1]++;
		Unknown274[3] += frame;
	}
	return false;
}

/*
**	0x6D64C0 FontSys::Update_Text (the client's profile label): with unknown set (Render) it runs the delay (0x6D60C0),
**	places the text (the transform's translation), refreshes a clipped last line with "...", steps the effects
**	(0x6CA0C0 per list) and the timers; then, unless nothing changed (+0x4C5 with the same polygon count +0x4C8),
**	rebuilds the mesh: per pass (+0x444: 1 adds the second colour +0x4B4, offset by +0x418 / +0x41C) one quad per
**	strip of the text texture +0x2D0 (strips of Size pixels, stacked Height rows apart: 0x6D8DC0), clipped to the
**	text rectangle +0x420 and the screen rectangle +0x430, and the underline quad from +0x2E0's "_" (Underline).
**	Colours +0x494 / +0x4A4 / +0x4B4 are floats r, g, b, a (0x6D5170); fontmanager.h's FontStyleColorStruct says int.
*/
// NOT EXACT: written from the client's code (0x6D64C0) for the POC, not yet matched
void FontSys::Update_Text(bool unknown)
{
	WWPROFILE("FontSys::Update_Text");
	float *color = (float *)&Unknown494;		// r, g, b, a now
	float *target = (float *)&Unknown4A4;		// r, g, b, a it moves to
	float *second = (float *)&Unknown4B4;		// the second pass's r, g, b
	float *offset = (float *)&Unknown488;		// +0x488: an offset added to the position (floats)
	float pos_x = 0.0f;
	float pos_y = 0.0f;
	int i;

	if (unknown) {
		Unknown444 = 0;
		if (Unknown_6D60C0()) {
			return;
		}
		if (Unknown4E5) {
			Unknown4E5 = false;
			Unknown_6D5D40();
		}
		if (LineNumber > 0) {
			Unknown4C5 = false;
		}
		Vector3 at;
		if (Unknown302) {
			at.X = Unknown464[0] + Unknown47C.X + offset[0];
			at.Y = Unknown464[1] + Unknown47C.Y + offset[1];
			at.Z = Unknown464.Z + Unknown47C.Z + offset[2];
		} else {
			Vector3 p = Get_Position();
			at.X = p.X + Unknown47C.X + offset[0];
			at.Y = p.Y + Unknown47C.Y + offset[1];
			at.Z = p.Z + Unknown47C.Z + offset[2];
		}
		pos_x = at.X;
		pos_y = at.Y;
		Transform[0][3] = at.X;
		Transform[1][3] = at.Y;
		Transform[2][3] = at.Z;
		int line_height = (int)(Scale * 480.0f * Unknown2C8 / Height);
		if (LineNumber > 0 && (float)Unknown430[3] < at.Y) {
			float limit = (float)(Unknown430[3] + line_height);
			bool refresh = false;
			if (at.Y < limit && LineNumber != LineCount && !Unknown406) {
				Unknown406 = true;
				refresh = true;
			} else if (at.Y > limit && LineNumber != LineCount && Unknown406) {
				Unknown406 = false;
				refresh = true;
			}
			if (refresh) {
				StringClass text = Info.Text;
				if (Unknown406) {
					text += "...";
				}
				*(int *)((char *)Font.Peek() + 0xD28) = *(int *)((char *)Font.Peek() + 0xD2C);	// Font->XSpacing = Font->MinSpacing (protected in font32bit.h)
				Unknown4C4 = Unknown2D0.Unknown_6D8DC0(&Font, text, 0);
				Unknown2C4 = Font->String_Pixel_Width(text);
				Unknown2C8 = Font->Get_Height();
				Unknown4C5 = false;
				Unknown420[1] = Unknown2C4;
				Unknown420[3] = Unknown2C8;
			}
		}
		for (i = 0; i < 4; i++) {
			if (Unknown_6CD180() && Unknown2F4[2] >= 0.5f) {
				Unknown_6CA0C0(Effects[3][i]);
			}
			if (Unknown_6CD6F0()) {
				Unknown_6CA0C0(Effects[4][i]);
			}
			if (Unknown274[12 + i] > 0) {
				Unknown_6CA0C0(Effects[5][i]);
			}
			if (Unknown440 == 0) {
				Unknown_6CA0C0(Effects[0][i]);
			} else if (Unknown440 == 1) {
				Unknown_6CA0C0(Effects[1][i]);
			} else if (Unknown440 == 2) {
				Unknown_6CA0C0(Effects[2][i]);
			}
		}
		if (Unknown4E6) {
			float a = (float)WW3D::Get_Frame_Time() * 0.001f;
			a = a + a + color[3];
			color[3] = a;
			if (a > 1.0f) {
				color[3] = a - 1.0f;
			}
		}
		int elapsed = Unknown270 - Unknown274[0];
		if (Unknown274[8] > 0) {
			int left = Unknown274[8] - elapsed;
			if (Unknown404) {
				Unknown274[8] = left < 0 ? 0 : left;
			} else {
				Unknown274[8] = left < 1 ? 1 : left;
			}
		}
		for (i = 0; i < 4; i++) {
			int left = Unknown274[12 + i] - elapsed;
			Unknown274[12 + i] = left < 0 ? 0 : left;
		}
		Unknown_BE67B4->Update();
		Unknown_6CCE60();
	}

	if (color[0] != target[0] || color[1] != target[1] || color[2] != target[2] || color[3] != target[3]) {
		Unknown4C5 = false;
	}
	int size = Unknown2D0.Size;
	int width = Unknown2C4;
	int height = Unknown2C8;
	TextureClass *texture = Unknown2D0.Texture;
	float step = 1.0f / (float)size;
	int strips = (int)((float)width * step + ((width & (size - 1)) != 0 ? 1.0f : 0.0f));
	float origin_x;
	if (Info.Justify[1] == 3) {
		origin_x = 0.0f;
	} else if (Info.Justify[1] == 4) {
		origin_x = (float)width;
	} else {
		origin_x = (float)(width / 2);
	}
	float origin_y;
	if (Info.Justify[0] == 1) {
		origin_y = 0.0f;
	} else if (Info.Justify[0] == 2) {
		origin_y = (float)height;
	} else {
		origin_y = (float)(height / 2);
	}
	int quads = (Unknown444 != 0) ? 2 : 1;
	if (Underline && !Unknown408) {
		quads *= 2;
	}
	int polys = quads * strips * 2;
	if (Unknown4C5 && Unknown4C8[0] == polys) {
		return;
	}
	if (Unknown4C8[1] < polys) {
		Resize(polys, polys * 2);
		Unknown4C8[1] = polys;
	}
	Unknown4C5 = true;
	Unknown4C8[0] = polys;
	Reset_Mesh_Counters();
	ShaderClass shader;
	Unknown_6D5F40(shader);
	Set_Shader(shader);
	for (i = 0; i < 3; i++) {				// the colour steps to the target, 1 a frame
		if (color[i] > target[i]) {
			color[i] -= 1.0f;
			if (color[i] < target[i]) color[i] = target[i];
		} else if (color[i] < target[i]) {
			color[i] += 1.0f;
			if (color[i] > target[i]) color[i] = target[i];
		}
	}

	for (int pass = Unknown444; pass >= 0; pass--) {
		float alpha = (color[3] < Unknown2F4[2]) ? color[3] : Unknown2F4[2];
		if (pass == 0) {
			Set_Vertex_Color(Vector4(color[0], color[1], color[2], alpha));
		} else {
			Set_Vertex_Color(Vector4(second[0], second[1], second[2], alpha));
		}
		int done = 0;
		float strip_v = (float)height * step;
		for (int k = 0; k < strips; k++) {
			int strip_x = k * size;
			int strip_w = width - strip_x;
			if (strip_w >= size) {
				strip_w = size;
			}
			float u_size = (float)strip_w * step;
			float v0 = (float)(k * height) * step;
			float px;
			float top;
			if (pass != 0) {
				px = (float)strip_x - (origin_x - (float)Unknown418);
				top = origin_y - (float)Unknown41C;
			} else {
				px = (float)strip_x - origin_x;
				top = origin_y;
			}
			float sx = (float)Width * (1.0f / 640.0f);
			float sy = (float)Height * (1.0f / 480.0f);
			int screen_x = (int)(pos_x * sx + px * Scale);
			int screen_y = (int)(sy * pos_y + top * Scale);
			int clip_l = (int)(Unknown430[0] * sx);
			int clip_r = (int)(Unknown430[1] * sx);
			int clip_t = (int)(Unknown430[2] * sy);
			int clip_b = (int)(Unknown430[3] * sy);
			int r0 = (int)(Unknown420[0] * Scale);
			int r1 = (int)(Unknown420[1] * Scale);
			int r2 = (int)(Unknown420[2] * Scale);
			int r3 = (int)(Unknown420[3] * Scale);
			int c = (int)(done * Scale);
			int v = c - screen_x + clip_l;
			int left = (int)((float)(r0 > v ? r0 : v) / Scale);
			v = c - screen_x + clip_r;
			int right = (int)((float)(r1 < v ? r1 : v) / Scale);
			v = screen_y - clip_t;
			int top_limit = (int)((float)(r2 > v ? r2 : v) / Scale);
			v = screen_y - clip_b;
			int bottom_limit = (int)((float)(r3 < v ? r3 : v) / Scale);
			if (right <= left + 1) {
				right = left + 1;
			}
			if (done <= right && done + strip_w >= left) {
				int qx = (int)px;
				int qw = strip_w;
				float u_start = 0.0f;
				float u_end = u_size;
				if (left > done) {
					int cut = left - done;
					qx += cut;
					qw -= cut;
					u_start = ((float)cut / (float)strip_w) * u_size;
				}
				if (qw + done > right) {
					qw = right - done;
					u_end = (float)qw * step;
				}
				int qy = (int)(top - (float)top_limit);
				float vt = (float)top_limit / (float)height;
				int rows = bottom_limit - top_limit;
				if (rows < 0) {
					rows = 0;
				}
				if (vt > 1.0f) {
					vt = 1.0f;
				}
				float va = vt * strip_v + v0;
				float v_size = ((float)rows / (float)height) * strip_v;
				float xs = 640.0f / (float)Width * Scale * Unknown2F4[0];
				float ys = 480.0f / (float)Height * Scale * Unknown2F4[0];
				if (Unknown4DC > 1.0f) xs /= Unknown4DC;
				if (Unknown4E0 > 1.0f) ys /= Unknown4E0;
				float x = (float)qx * xs;
				float y = (float)qy * ys;
				float w = xs * (float)qw;
				if (0.1f / Unknown4DC > w) w = 0.1f / Unknown4DC;
				float h = (float)rows * ys;
				if (0.1f / Unknown4E0 > h) h = 0.1f / Unknown4E0;
				float v_low = v_size + va;
				float v_high = va;
				float lift = (Unknown2F4[1] + 1.0f) * h * 0.5f;
				float y_high = y - lift;
				float y_low = (y - h) + lift;
				if (y_low > y_high) {
					float t = y_low; y_low = y_high; y_high = t;
					t = v_low; v_low = v_high; v_high = t;
				}
				Set_Texture(texture);
				Begin_Tri_Strip();
				Location(x, y_low, 0.0f);
				UV(u_start, v_low);
				End_Vertex();
				Location(x + w, y_low, 0.0f);
				UV(u_end, v_low);
				End_Vertex();
				Location(x, y_high, 0.0f);
				UV(u_start, v_high);
				End_Vertex();
				Location(x + w, y_high, 0.0f);
				UV(u_end, v_high);
				End_Vertex();
				End_Tri_Strip();
				if (Underline && !Unknown408 && (float)Unknown2E0.Size > 0.0f) {	// the underline: +0x2E0's "_" cell, stretched
					float size2 = (float)Unknown2E0.Size;
					float u2 = (float)Font->String_Pixel_Width("_") / size2;
					float v2 = (float)Font->Get_Height() / size2;
					float u_a = u2 * 0.2f;
					float u_b = u2 * 0.8f;
					Set_Texture(Unknown2E0.Texture);
					float drop = (float)height * 0.083333336f;
					Begin_Tri_Strip();
					Location(x, y_low - drop, 0.0f);
					UV(u_a, v2);
					End_Vertex();
					Location(x + w, y_low - drop, 0.0f);
					UV(u_b, v2);
					End_Vertex();
					Location(x, y_high - drop, 0.0f);
					UV(u_a, 0.0f);
					End_Vertex();
					Location(x + w, y_high - drop, 0.0f);
					UV(u_b, 0.0f);
					End_Vertex();
					End_Tri_Strip();
				}
			}
			done += strip_w;
		}
	}
}













































































































































































































































// NOT EXACT: written from the client's code (0x6D8270) for the POC, not yet matched
void FontSys::Render(RenderInfoClass &rinfo)
{
	Unknown4C4 = false;
	if (Unknown440 < 3) {
		G_ASSERT(SortLevel == SORT_LEVEL_NONE || !Model->Get_Flag(MeshGeometryClass::SORT));
		if (WW3D::Are_Static_Sort_Lists_Enabled() && SortLevel != SORT_LEVEL_NONE) {
			WW3D::Add_To_Static_Sort_List(this, SortLevel);
			return;
		}
		Vector3 pos = Get_Position();
		Update_Text(true);
		if (PolyCount != 0 && Is_Not_Hidden_At_All() && Unknown26C <= Unknown270) {
			Matrix3D tm = Transform;
			float x = tm[0][3];
			float y = tm[1][3];
			if (Unknown4DC > 1.0f) x *= Unknown4DC;
			if (Unknown4E0 > 1.0f) y *= Unknown4E0;
			float px = (float)floor(x / (640.0f / (float)Width));
			float py = (float)floor(y / (480.0f / (float)Height));
			x = (px + 0.5f) * (640.0f / (float)Width);
			y = (py + 0.5f) * (480.0f / (float)Height);
			if (Unknown4DC > 1.0f) x /= Unknown4DC;
			if (Unknown4E0 > 1.0f) y /= Unknown4E0;
			tm.Set_Translation(Vector3(x, y, 0.001f));
			DX8Wrapper::Set_Transform(D3DTS_WORLD, tm);
			Model->Render(rinfo);
		}
		Transform[0][3] = pos.X;
		Transform[1][3] = pos.Y;
		Transform[2][3] = pos.Z;
	}
}

// 0x6D8650: the text's position: +0x464 (with +0x302 set) or the transform's, plus +0x47C and the effects' offset +0x488
Vector3 FontSys::Unknown_6D8650()
{
	Vector3 pos;
	if (Unknown302) {
		pos = Unknown464 + Unknown47C + Unknown488;
	} else {
		pos = Get_Position() + Unknown47C + Unknown488;
	}
	return pos;
}

// 0x6D87D0: the text (Info.Text)
const char *FontSys::Unknown_6D87D0()
{
	return Info.Text;
}

// 0x6D87F0 (D185's variant)
void FontSys::Set_Transform(const Matrix3D &m)
{
	RenderObjClass::Set_Transform(m);
	Unknown4E5 = true;
	if (Unknown4D0 != m.Get_Translation()) {
		Unknown4D0 = m.Get_Translation();
		Unknown4C5 = false;
	}
}

// 0x6D88C0
void FontSys::Set_Position(const Vector3 &v)
{
	RenderObjClass::Set_Position(v);
	Unknown4E5 = true;
	Unknown4C5 = false;
}

// 0x6D88F0: the screen rectangle the text is clipped to (+0x430: left, right, top, bottom)
void FontSys::Unknown_6D88F0(int left, int top, int right, int bottom)
{
	Unknown430[0] = left;
	Unknown430[2] = top;
	Unknown430[1] = right;
	Unknown430[3] = bottom;
	Unknown4C5 = false;
}

/*
**	Reconstruction notes (fontsys.cpp)
**
**	The client's FontSys (fontmanager.h; was FontSys until 2026-11-03): a DynamicMeshClass(2, 4) that
**	draws one string template. __FILE__ 'e:\build\beta\G\app\Client\fontsys.cpp' (0xB92828) is used once, by
**	Render 0x6D8270's G_ASSERT on line 914 (DynamicMeshClass::Render's check, copied). The file's start (the
**	fontmanager.cpp | fontsys.cpp boundary between 0x6D2A70 and 0x6D47C0) is not settled: the constructors
**	0x6D47C0 / 0x6D4C70 are here as the class's own code, interim (D137).
**	Vtable 0xAFECB4 (122 slots, as DynamicMeshClass's): destructor 0x6D4C40 (the deleting form of the inline
**	~FontSys 0x67F230), Class_ID 0x67EF00, Get_Name 0x67EF20, Render 0x6D8270, Set_Container 0x6D5920,
**	Set_Transform 0x6D87F0, Set_Position 0x6D88C0, the bounding volumes 0x6D5B90 / 0x6D5C10, Set_Hidden 0x67EF40,
**	Is_Complete 0x67EEE0.
**	Functional (D64) for the POC client: TextGadgetClass's constructor needs a real object with a table. Not yet
**	rebuilt: Update_Text 0x6D64C0, 0x6D8C50, 0x6D8DC0 (fonttexture.cpp).
*/
