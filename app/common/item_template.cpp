// Earth & Beyond client (reconstruction): app\common\item_template.cpp, PROVISIONAL file name (D137; notes at the end)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "..\client\client.h"
#include "ini.h"
#include "strtok_r.h"
#include "rendobj.h"
#include "bmp2d.h"
#include "matinfo.h"
#include "assetmgr.h"
#include "texture.h"
#include "matrix3d.h"
#include "vector4.h"
#include "..\packets\pkitemba.h"

// 0x831340
Unknown831340Class::Unknown831340Class(int a, int b, int c, const RefCountPtr<ItemBaseInfoClass> &d, int e, int f, int g, int h, int i, int j, int k, int l,
	const std::string &name, const std::string &m, const std::string &n) :
	ItemTemplateID(a),
	Category(b),
	Subcategory(c),
	ItemType(g),
	Icon(0),
	Name(name),
	Unknown24(m),
	Unknown34(n),
	GameBasset(e),
	TechLevel(h),
	Cost(i),
	MaxStack(j),
	EffectUsage(k),
	Flags(l),
	Info(d)
{
	if (!Info) {
		Info = Create_NEW(new ItemBaseInfoClass);
	}
	if (f != -1) {
		static INIClass ini("basset.ini");
		char section[256];
		sprintf(section, "BASE-%d", f);
		char *file = ini.Get_Alloc_String(section, "Icon", 0);
		if (file) {
			int width = ini.Get_Int(section, "Width", -1);	// NAME 2: the INI key
			int height = ini.Get_Int(section, "Height", -1);	// NAME 2: the INI key
			Icon = new Bitmap2DObjClass(file, 0, 0, false, false, false, width, height, false);
			free(file);
		}
	}
}

// 0x8316D0
Unknown831340Class::~Unknown831340Class()
{
	if (Icon) {
		Icon->Remove();
		Icon->Release_Ref();
		Icon = 0;
	}
	Info = NULL;
}

// 0x831840
TextureClass *Unknown831340Class::Unknown_831840()
{
	if (!Icon) {
		return(0);
	}
	MaterialInfoClass *material = Icon->Get_Material_Info();	// NAME 2: what it holds
	TextureClass *texture = material->Get_Texture(0);	// NAME 2: what it holds
	if (!texture) {
		texture = WW3DAssetManager::Get_Instance()->Get_Texture("BLANK.TGA", MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true);
	}
	material->Release_Ref();
	return(texture);
}

// 0x8318B0: ClientClass's item template list's Find, inlined
Unknown831340Class *Unknown_8318B0(int item_template_id, ClientClass *client)	// NAME 1: neutral (D10)  // NAME 3 [item_template_id]: ui_manu.cpp's FAIL_IF
{
	HashNodeClass<Unknown831340Class *> *node = client->Unknown831340List.Find(item_template_id);
	if (node) {
		return(node->Get_Record());
	}
	return(0);
}

// 0x831950: three rows of four comma-separated floats
void Unknown_831950(Matrix3D &transform, const char *text)	// NAME 1: neutral (D10)  // NAME 1 [transform, text]: what 0x7251C0 passes
{
	char *lasts = 0;
	char buffer[128];
	strcpy(buffer, text);
	char *next = buffer;
	for (int row = 0; row < 3; row++) {
		Vector4 v;
		char *token = strtok_r(next, ",", &lasts);
		next = 0;
		v.X = (float)atof(token);
		v.Y = (float)atof(strtok_r(0, ",", &lasts));
		v.Z = (float)atof(strtok_r(0, ",", &lasts));
		v.W = (float)atof(strtok_r(0, ",", &lasts));
		transform[row] = v;
	}
}

/*
** Notes (reconstruction)
**
** The object after circbuff.cpp (0x830B60-0x830FA0 area), client 0x831340-0x831A6F: ClientClass's item templates
** (client.h's Unknown831340Class, the entries ITEM_BASE 37 adds to ClientClass +0xC94 and the 'Null Item' client.cpp
** adds), their lookup 0x8318B0 and 0x831950 (cbasset.ini's Mount transform). No __FILE__ or RTTI in the window: the
** file name is PROVISIONAL (D137). 0x8316B0 is the compiler's atexit destructor of the constructor's static
** INIClass (0xBF3278, guard 0xBF3270).
*/
