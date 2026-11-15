// Earth & Beyond client (reconstruction): app\client\unknown_82ecb0.cpp (PROVISIONAL name, D137; 0x82ECB0-0x82F1F3). Notes at the end.
#include <string.h>
#include "unknown_82ecb0.h"
#include "..\..\wwmath\matrix3d.h"
#include "..\..\wwmath\vector3.h"
#include "..\..\wwlib\ini.h"
#include "..\..\ww3d2\texture.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\intersec.h"
#include "..\shared\enbassetmgr.h"

// NAME 5: the client's Get_Type_Name string 'WW3DLightAttenuationType' (aux_roots.h, 0x708520)
enum WW3DLightAttenuationType { UNKNOWN_B0100C_0 };	// NAME 1 [UNKNOWN_B0100C_0]: neutral (D10)
bool String_To_Value(const char *text, WW3DLightAttenuationType *value);	// 0x82F180
char *Value_To_String(char *buf, int size, WW3DLightAttenuationType value);	// 0x82F1D0

// 0xBA89C8: attenuation type names for String_To_Value / Value_To_String
static const char *Unknown_BA89C8[] = {	// NAME 1: neutral (D10)
	"None", "3DSMAX", "OpenGLSimple", "OpenGLFull"
};

// 0x82ECB0
Matrix3D Unknown_82ECB0(const Vector3 &direction, const Vector3 &position)
{
	const Vector3 up(0.0f, 0.0f, 1.0f);
	Vector3 x = direction;
	x.Normalize();
	Vector3 y = Vector3::Cross_Product(up, x);
	y.Normalize();
	Vector3 z = Vector3::Cross_Product(x, y);
	Matrix3D tm(x, y, z, position);
	return tm;
}

// 0x82EF50
void Unknown_82EF50()
{
}

// 0x82EF70
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

// 0x82F000
TextureClass *Unknown_82F000(INIClass &ini, const char *section, float *hsv)
{
	char name[50];
	ini.Get_String(section, "File", "default.tga", name, sizeof(name));
	bool wrap = ini.Get_Bool(section, "TextureWrap", true);
	TextureClass *texture = AssetManager->Recolor_Texture(name, hsv, MIP_LEVELS_ALL);
	if (!wrap) {
		texture->Set_U_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);
		texture->Set_V_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);
	}
	return texture;
}

// 0x82F090
RenderObjClass *Unknown_82F090(IntersectionClass *inter, float screen_x, float screen_y, const LayerClass &layer, RenderObjClass *robj, IntersectionResultClass *final_result)
{
	if (robj->Get_Num_Sub_Objects() != 0) {
		for (int i = 0; i < robj->Get_Num_Sub_Objects(); i++) {
			RenderObjClass *sub = robj->Get_Sub_Object(i);
			RenderObjClass *hit = Unknown_82F090(inter, screen_x, screen_y, layer, sub, final_result);
			sub->Release_Ref();
			if (hit != NULL) {
				return hit;
			}
		}
	}
	if (inter->Intersect_Screen_Point_RenderObject(screen_x, screen_y, layer, robj, final_result)) {
		if (robj->Get_User_Data() != NULL) {
			return robj;
		}
	}
	return NULL;
}

// 0x82F180
bool String_To_Value(const char *text, WW3DLightAttenuationType *value)
{
	for (int i = 0; i < 4; i++) {
		if (stricmp(text, Unknown_BA89C8[i]) == 0) {
			*value = (WW3DLightAttenuationType)i;
			return true;
		}
	}
	return false;
}

// 0x82F1D0
char *Value_To_String(char *buf, int size, WW3DLightAttenuationType value)
{
	strncpy(buf, Unknown_BA89C8[value], size);
	return buf;
}

/*
**	Reconstruction notes (app\client\unknown_82ecb0.cpp, PROVISIONAL name, D137; 0x82ECB0-0x82F1F3):
**
**	Object boundaries: follows app\common\udpconn.cpp (which ends with its guarded <string> initialiser at
**	0x82EC80) and precedes app\common\chartemplate.cpp (which begins with its .CRT$XCU vector initialiser at
**	0x82F210). No __FILE__, assert or RTTI names this object. 0x82EF70 (Unknown_82EF70, moved from app\client\decal.cpp
**	under D66) sits inside this range and shares the strings "File", "default.tga" and "TextureWrap"
**	(0xBA89A0..0xBA89B8) with 0x82F000.
**
**	All seven functions in the range are byte-exact:
**	- 0x82ECB0 (514 bytes): builds an orthonormal right-handed Matrix3D at position with X along direction
**	  and Y horizontal (Cross(up, x)); a local const Vector3 up(0,0,1) gives the client's Normalize() order.
**	- 0x82EF50 (1 byte): empty ret stub (D99).
**	- 0x82EF70 (103 bytes): loads an INI texture ("File", default "default.tga"; clamped unless "TextureWrap")
**	  via ENBAssetManager.
**	- 0x82F000 (104 bytes): loads and recolours an INI texture ("File", "TextureWrap") via ENBAssetManager.
**	- 0x82F090 (178 bytes): recursively picks the first sub-object / object with non-null User_Data under
**	  IntersectionClass::Intersect_Screen_Point_RenderObject.
**	- 0x82F180 / 0x82F1D0 (60 / 35 bytes): String_To_Value / Value_To_String for WW3DLightAttenuationType
**	  ("None", "3DSMAX", "OpenGLSimple", "OpenGLFull", 0xBA89C8), already declared in aux_roots.h.
*/
