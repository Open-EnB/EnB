// Earth & Beyond client (reconstruction): decal.cpp (the client's 'e:\build\beta\G\app\Client\decal.cpp', its FAIL_IF /
// assert path), the decal library: the ship's insignias (decal_textures.ini), the secret ones
// (secret_decal_textures.ini) and the swatches (decal_swatches.ini, each a mesh whose passes are copied), and the
// material of one decal index for shiputils.cpp's 0x80EAF0. The character creator's ship screen (ui_shipcust.cpp),
// the game (sclient.cpp) and the scripted movies load it with the same three INI names.
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <vector>
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
// NOT EXACT: functional (D64), from the pseudocode: the two base materials, then each INI's sections in order
// @0x75BE00
void SClientUnknown1334Class::Load(const char *textures, const char *swatches, const char *secret_textures)
{
	Unknown30.Unknown00 = 1;
	Unknown30.Unknown04[0][0] = NULL;
	Unknown30.Unknown04[0][1] = NULL;
	Unknown30.Unknown24[0] = ShaderClass::_PresetAlphaShader;
	Unknown30.Unknown24[0].Set_Fog_Func(ShaderClass::FOG_ENABLE);
	Unknown30.Unknown34[0] = Create_Get(new VertexMaterialClass);
	Unknown30.Unknown34[0]->Set_Lighting(true);
	Unknown74.Unknown00 = 1;
	Unknown74.Unknown04[0][0] = NULL;
	Unknown74.Unknown04[0][1] = NULL;
	Unknown74.Unknown24[0] = ShaderClass::_PresetOpaqueShader;
	Unknown74.Unknown24[0].Set_Color_Mask(ShaderClass::COLOR_WRITE_DISABLE);
	Unknown74.Unknown24[0].Set_Depth_Mask(ShaderClass::DEPTH_WRITE_DISABLE);
	Unknown74.Unknown24[0].Set_Texturing(ShaderClass::TEXTURING_DISABLE);
	Unknown74.Unknown34[0] = Create_Get(new VertexMaterialClass);
	FAIL_IF(!textures) return;	// NAME 3 [decal_texture_ini_filename]: the FAIL_IF's
	FAIL_IF(!swatches) return;
	FAIL_IF(!secret_textures) return;
	INIClass texture_ini(textures);
	INIClass swatch_ini(swatches);
	INIClass secret_ini(secret_textures);
	char section[252];
	int i;
	TextureArray.erase(TextureArray.begin(), TextureArray.end());
	TextureArray.reserve(texture_ini.Section_Count());
	for (i = 0; ; i++) {
		sprintf(section, "Decal%04d", i);
		if (!texture_ini.Find_Section(section)) {
			break;
		}
		RefCountPtr<TextureClass> texture = Create_Get(Unknown_82EF70(texture_ini, section));
		texture->Set_U_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);
		texture->Set_V_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);
		TextureArray.insert(TextureArray.end(), 1, texture);
	}
	SecretTextureArray.erase(SecretTextureArray.begin(), SecretTextureArray.end());
	SecretTextureArray.reserve(secret_ini.Section_Count());
	for (i = 0; ; i++) {
		sprintf(section, "Decal%04d", i);
		if (!secret_ini.Find_Section(section)) {
			break;
		}
		RefCountPtr<TextureClass> texture = Create_Get(Unknown_82EF70(secret_ini, section));
		texture->Set_U_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);
		texture->Set_V_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);
		SecretTextureArray.insert(SecretTextureArray.end(), 1, texture);
	}
	SwatchArray.erase(SwatchArray.begin(), SwatchArray.end());
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
		if (name[0] == '\0') {
			G_ASSERT(0);
			Unknown_4BC260("Decals", "", __FILE__, __LINE__, "No Swatch specified in decal_swatches.ini for swatch decal %d", i);
		} else {
			sprintf(filename, "%s.w3d", name);
			RenderObjClass *robj = AssetManager->Create_Render_Obj(name);
			if (robj == NULL) {
				AssetManager->Load_3D_Assets(filename);
				robj = AssetManager->Create_Render_Obj(name);
			}
			if (robj == NULL) {
				G_ASSERT(0);
				Unknown_4BC260("Decals", "", __FILE__, __LINE__, "Could not create swatch %s from file %s for swatch decal %d", name, filename, i);
			} else {
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
			}
		}
		SwatchArray.insert(SwatchArray.end(), 1, swatch);
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

/*
**	Reconstruction notes (decal.cpp). They sit here to keep 0x75CB10's FAIL_IFs on the client's lines 157 / 159 / 161
**	(Load's own asserts, client lines 36-38 and 132-145, are not on theirs yet: Load is functional).
**
**	Placement: the class's destructor 0x618A50, its 0x61DF40 and the material's constructor / destructor 0x619280 /
**	0x618C10 sit in the 0x61xxxx range (ui_shipcust.cpp's neighbours), 0x82EF70 far after; all are here for now
**	(interim placement, D137) until their objects are known.
**	0x619280: 0x8441B in each shader is ShaderClass's default. 0x618C10: the members' destructors. 0x618A50: the
**	vectors' elements released. 0x82EF70: 'File' (default 'default.tga') through the asset manager, clamped unless
**	'TextureWrap'. Load: an insignia's base is one pass, alpha blended and fogged, lit; an empty slot's draws nothing.
**
**	0x75CB10: the client's FAIL_IF names (i, TextureArray, SwatchArray, SecretTextureArray, mat_desc); the base
**	material first (the swatch, a negative index, after: 'jl'); the secret texture before the plain one; the tint
**	test on a Vector4 difference (evaluated W, Z, Y, X) against a float variable (fld / fcompp; a literal gives
**	fcomp qword); the tint's Set_* calls through the RefCountPtr (mov ecx, [edi]), not the new material's pointer.
*/
