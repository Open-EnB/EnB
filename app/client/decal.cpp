// Earth & Beyond client (reconstruction): decal.cpp, the decal library (notes at the end)
#include <stdio.h>
#include <math.h>
#include "decal.h"
#include "..\shared\unknown_debug.h"
#include "..\shared\unknown_message.h"
#include "..\shared\enbassetmgr.h"
#include "..\..\wwlib\ini.h"
#include "..\..\wwmath\vector4.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\mesh.h"
#include "..\..\ww3d2\meshmdl.h"
#include "..\..\ww3d2\texture.h"
#include "..\..\ww3d2\vertmaterial.h"
#define SECRET_DECALS 1000	// NAME 3: the FAIL_IF's 'i - SECRET_DECALS'
// NOT EXACT: functional (D64), from the pseudocode
// @0x75BE00
void SClientUnknown1334Class::Load(const char *decal_texture_ini_filename, const char *decal_swatch_ini_filename, const char *secret_texture_ini_filename)
{
	Unknown30.Unknown00 = 1;
	Unknown30.Unknown04[0][0] = NULL;
	Unknown30.Unknown04[0][1] = NULL;
	Unknown30.Unknown24[0] = ShaderClass::_PresetAlphaShader;
	Unknown30.Unknown24[0].Set_Fog_Func(ShaderClass::FOG_ENABLE);
	Unknown30.Unknown34[0] = Create_NEW(new VertexMaterialClass);
	Unknown30.Unknown34[0]->Set_Lighting(true);
	Unknown74.Unknown00 = 1;
	Unknown74.Unknown04[0][0] = NULL;
	Unknown74.Unknown04[0][1] = NULL;
	Unknown74.Unknown24[0] = ShaderClass::_PresetOpaqueShader;
	Unknown74.Unknown24[0].Set_Color_Mask(ShaderClass::COLOR_WRITE_DISABLE);
	Unknown74.Unknown24[0].Set_Depth_Mask(ShaderClass::DEPTH_WRITE_DISABLE);
	Unknown74.Unknown24[0].Set_Texturing(ShaderClass::TEXTURING_DISABLE);
	Unknown74.Unknown34[0] = Create_NEW(new VertexMaterialClass);

	FAIL_IF(!decal_texture_ini_filename) return;
	FAIL_IF(!decal_swatch_ini_filename) return;
	FAIL_IF(!secret_texture_ini_filename) return;
	INIClass texture_ini(decal_texture_ini_filename);
	INIClass swatch_ini(decal_swatch_ini_filename);
	INIClass secret_ini(secret_texture_ini_filename);
	char section[252];
	int i;

	TextureArray.clear();
	TextureArray.reserve(texture_ini.Section_Count());
	for (i = 0; ; i++) {
		sprintf(section, "Decal%04d", i);
		if (!texture_ini.Find_Section(section)) {
			break;
		}
		RefCountPtr<TextureClass> texture = Create_Get(Unknown_82EF70(texture_ini, section));
		texture->Set_U_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);
		texture->Set_V_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);
		TextureArray.push_back(texture);
	}

	SecretTextureArray.clear();
	SecretTextureArray.reserve(secret_ini.Section_Count());
	for (i = 0; ; i++) {
		sprintf(section, "Decal%04d", i);
		if (!secret_ini.Find_Section(section)) {
			break;
		}
		RefCountPtr<TextureClass> texture = Create_Get(Unknown_82EF70(secret_ini, section));
		texture->Set_U_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);
		texture->Set_V_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);
		SecretTextureArray.push_back(texture);
	}

	SwatchArray.clear();
	SwatchArray.reserve(swatch_ini.Section_Count());
	for (i = 0; ; i++) {
		sprintf(section, "Decal%02d", i);
		if (!swatch_ini.Find_Section(section)) {
			break;
		}
		Unknown619280Class swatch = Unknown30;
		char name[252];
		char filename[256];
		swatch_ini.Get_String(section, "Swatch", "", name, 0xFB);
		if (name[0] != '\0') {
			sprintf(filename, "%s.w3d", name);
			RenderObjClass *robj = AssetManager->Create_Render_Obj(name);
			if (robj == NULL) {
				AssetManager->Load_3D_Assets(filename);
				robj = AssetManager->Create_Render_Obj(name);
			}
			if (robj != NULL) {
				if (robj->Class_ID() == RenderObjClass::CLASSID_MESH) {
					MeshModelClass *model = ((MeshClass *)robj)->Peek_Model();
					unsigned int passes = model->Get_Pass_Count();
					swatch.Unknown00 = passes;
					for (unsigned int pass = 0; pass < passes; pass++) {
						for (int stage = 0; stage < 2; stage++) {
							swatch.Unknown04[pass][stage] = Create_Peek(model->Peek_Single_Texture(pass, stage));
						}
						swatch.Unknown24[pass] = model->Get_Single_Shader(pass);
						swatch.Unknown34[pass] = Create_Peek(model->Peek_Single_Material(pass));
					}






























				} else {
					G_ASSERT(0);
					Unknown_4BC260("Decals", "", __FILE__, __LINE__, "Swatch %s for swatch decal %d is not a mesh!", name, i);
				}
				robj->Release_Ref();


			} else {
				G_ASSERT(0);
				Unknown_4BC260("Decals", "", __FILE__, __LINE__, "Could not create swatch %s from file %s for swatch decal %d", name, filename, i);
			}

		} else {
			G_ASSERT(0);
			Unknown_4BC260("Decals", "", __FILE__, __LINE__, "No Swatch specified in decal_swatches.ini for swatch decal %d", i);
		}
		SwatchArray.push_back(swatch);
	}
}


// NOT EXACT: stack slot (operator new's temporary in [esp + 0x28] against the client's 0x30; nothing else differs)
// @0x75CB10
void SClientUnknown1334Class::Unknown_75CB10(int i, const Vector4 &color, Unknown619280Class *mat_desc)
{
	if (i < SECRET_DECALS) {
		FAIL_IF(i >= (int)TextureArray.size() || i < -(int)SwatchArray.size()) return;
	} else {
		FAIL_IF(i - SECRET_DECALS > (int) SecretTextureArray.size()) return;
	}
	FAIL_IF(!mat_desc) return;
	if (i >= 0) {
		*mat_desc = Unknown30;
		if (i >= SECRET_DECALS) {
			mat_desc->Unknown04[0][0] = SecretTextureArray[i - SECRET_DECALS];
		} else {
			mat_desc->Unknown04[0][0] = TextureArray[i];
		}
		static const float epsilon = 0.00390625f;	// NAME 1 [epsilon]: neutral; a float variable, not a literal: the client compares with fld / fcompp
		Vector4 diff = Vector4(1.0f, 1.0f, 1.0f, 1.0f) - color;
		if (fabs(diff.X) > epsilon || fabs(diff.Y) > epsilon || fabs(diff.Z) > epsilon || fabs(diff.W) > epsilon) {
			VertexMaterialClass *source = mat_desc->Unknown34[0].Peek();
			VertexMaterialClass *material = new VertexMaterialClass;
			*material = *source;
			mat_desc->Unknown34[0] = Create_Get(material);
			mat_desc->Unknown34[0]->Set_Ambient(color.X, color.Y, color.Z);
			mat_desc->Unknown34[0]->Set_Diffuse(color.X, color.Y, color.Z);
			mat_desc->Unknown34[0]->Set_Opacity(color.W);
		}
	} else {
		*mat_desc = SwatchArray[-(i + 1)];
		TextureArray.size();	// a dead read: it spends the inline budget so the swatch's 4 material copies call 0x75D000 as in the client
	}
}

// @0x619280
Unknown619280Class::Unknown619280Class()
	: Unknown00(0)
{
}
// @0x618C10
Unknown619280Class::~Unknown619280Class()
{
}
// @0x618A50
SClientUnknown1334Class::~SClientUnknown1334Class()
{
}
// @0x61DF40
RefCountPtr<TextureClass> SClientUnknown1334Class::Unknown_61DF40(int index)
{
	return TextureArray[index];
}
#pragma auto_inline(off)
// @0x82EF70
TextureClass *Unknown_82EF70(INIClass &ini, const char *section)
{
	char name[50];
	ini.Get_String(section, "File", "default.tga", name, sizeof(name));
	bool wrap = ini.Get_Bool(section, "TextureWrap", true);
	TextureClass *texture = AssetManager->Get_Texture(name);
	if (!wrap) {
		texture->Set_U_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);
		texture->Set_V_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);
	}
	return texture;
}
#pragma auto_inline(on)

/*
**	Reconstruction notes (decal.cpp, 'e:\build\beta\G\app\Client\decal.cpp'): the decal library, the ship's insignias
**	(decal_textures.ini), the secret ones (secret_decal_textures.ini, indices from 1000) and the swatches
**	(decal_swatches.ini, each a mesh whose passes are copied), and the material of one decal index for shiputils.cpp's
**	0x80EAF0. The character creator's ship screen (ui_shipcust.cpp), the game (sclient.cpp) and the scripted movies
**	load it with the same three INI names. The asserts sit on the client's lines (Load: FAIL_IFs 36-38, G_ASSERTs
**	132 / 139 / 144 with __LINE__ on the next line; 0x75CB10: 157 / 159 / 161): blank lines pad Load to them.
**
**	Placement: the class's destructor 0x618A50, its 0x61DF40 and the material's constructor / destructor 0x619280 /
**	0x618C10 sit in the 0x61xxxx range (ui_shipcust.cpp's neighbours), 0x82EF70 far after; all are here for now
**	(interim placement, D137) until their objects are known, after Load and 0x75CB10 so the asserts fit. Load calls
**	0x82EF70 out of line (another file's function in the client): auto_inline(off) keeps VC6 from inlining it here.
**
**	Load: the parameters are the FAIL_IFs' names; clear() (the first one's erase out of line, 0x75D0B0, the other two's
**	copy, 0x75DAE0 / 0x75DB50, as in the client) and push_back (its insert(end(), 1, x) out of line, 0x75D280 /
**	0x75D5F0, as in the client, where insert(end(), 1, x) written out inlines the texture ones); the swatch's
**	not-a-mesh / not-created / no-name branches nest in the asserts' line order. The two base materials take
**	Create_NEW: the client calls 0x75CFA0 there, a GET-form wrapper other than 0x75CB10's Create_Get 0x47D040 (same
**	body; the name is a guess among ref_ptr.h's two GET forms). Left (0.46): the client keeps &Unknown30 in a slot
**	and has 8 more bytes of frame.
**	0x619280: 0x8441B in each shader is ShaderClass's default. 0x618C10: the members' destructors. 0x618A50: the
**	vectors' elements released. 0x82EF70: 'File' (default 'default.tga') through the asset manager, clamped unless
**	'TextureWrap'. Load: an insignia's base is one pass, alpha blended and fogged, lit; an empty slot's draws nothing.
**
**	0x75CB10: the client's FAIL_IF names (i, TextureArray, SwatchArray, SecretTextureArray, mat_desc); the base
**	material first (the swatch, a negative index, after: 'jl'); the secret texture before the plain one; the tint
**	test on a Vector4 difference (evaluated W, Z, Y, X) against a float variable (fld / fcompp; a literal gives
**	fcomp qword); the tint's Set_* calls through the RefCountPtr (mov ecx, [edi]), not the new material's pointer.
*/
