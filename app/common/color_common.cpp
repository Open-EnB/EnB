// Earth & Beyond client (reconstruction): app\common\color_common.cpp (__FILE__ 0xBA93DC). Notes at the end of the file.
#include <string.h>
#include "color_common.h"
#include "vector3.h"
#include "ini.h"
#include "..\shared\g_assert.h"







void Unknown_831A70(int type, Vector3 &color)
{
	switch (type) {
		case 0:
			color.Set(0.0f, 0.0f, 0.0f); return;
		case 1:
			color.Set(0.66f, 0.0f, 0.66f); return;
		case 2:
			color.Set(0.0f, 0.66f, 0.66f); return;
		case 3:
			color.Set(0.0f, 1.0f, 0.0f); return;
		case 4:
			color.Set(0.31f, 1.0f, 0.0f); return;
		case 5:
			color.Set(0.99f, 0.99f, 0.33f); return;
		case 6:
			color.Set(0.99f, 0.33f, 0.33f); return;
		case 7:
			color.Set(0.66f, 0.33f, 0.0f); return;
		case 8:
			color.Set(0.66f, 0.0f, 0.0f); return;
		case 9:
			color.Set(0.33f, 0.99f, 0.99f); return;
		case 10:
			color.Set(0.33f, 0.33f, 0.99f); return;
		case 11:
			color.Set(0.0f, 0.0f, 0.66f); return;
		case 13:
			color.Set(0.33f, 0.33f, 0.33f); return;
		case 14:
			color.Set(0.66f, 0.66f, 0.66f); return;
		case 15: break;
		default:
			G_ASSERT(false);
			break;
	}
	color.Set(1.0f, 1.0f, 1.0f);
}

// The chat types' names (0xBA928C), in type order
static const char *Unknown_BA928C[] = {	// NAME 1: neutral (D10)
	"Context",
	"OOC",
	"Broadcast",
	"Tell",
	"Group",
	"NPC",
	"Tip",
	"Local",
	"Guild",
	"GeneralChannel",
	"OOCChannel",
	"NewbieChannel",
	"MarketChannel",
	"PrivateChannel",
	"RaceChannel",
	"ClassChannel",
	"Echo",
	"ErrorContext",
	"ErrorOOC",
	"ErrorSystem",
	"Combat",
	"Warning",
	"Attention",
	"Debug",
};

bool Unknown_831C80(const char *name, int &type)
{
	for (int i = 0; i < (int)(sizeof(Unknown_BA928C) / sizeof(Unknown_BA928C[0])); i++) {
		if (stricmp(name, Unknown_BA928C[i]) == 0) {
			type = i;
			return(true);
		}
	}



	G_ASSERT(0);
	return(false);
}

int Unknown_831CF0(int index, bool special)
{
	static bool first = true;		// 0xBA9410  // NAME 1: neutral (D10)
	static int textures;			// 0xBF328C: decal_textures.ini's sections  // NAME 1: neutral (D10)
	static int swatches;			// 0xBF3294: decal_swatches.ini's sections  // NAME 1: neutral (D10)
	static int secret;				// 0xBF3290: secret_decal_textures.ini's sections  // NAME 1: neutral (D10)
	if (first) {
		INIClass textures_ini("decal_textures.ini");		// NAME 1: neutral (D10)
		INIClass swatches_ini("decal_swatches.ini");		// NAME 1: neutral (D10)
		INIClass secret_ini("secret_decal_textures.ini");	// NAME 1: neutral (D10)
		textures = textures_ini.Section_Count();
		swatches = swatches_ini.Section_Count();
		secret = secret_ini.Section_Count();
		first = false;
	}
	if (special) {
		if (index < 0 && index >= -swatches) {
			return(index);
		}
		if (index >= 1000 && index < secret + 1000) {
			return(index);
		}
	}
	if (index >= 0 && index < textures) {
		return(index);
	}
	return(0x7FFFFFF);
}

/*
**	Notes.
**	Object 0x831950-0x831E7F: it opens after the object whose guarded std::ctype<unsigned short>::id initialiser is
**	0x831920 (item_template.cpp's provisional window, 0x831340-0x83193F) and closes with its own at 0x831E60 (guard
**	0xC416FC). 0x831950 (the Mount transform parser item_template.cpp defines) therefore sits in this object, not in
**	item_template.cpp's; it is left there for now (a placement proposal). 0x831E90-0x83209F (the sphere distances)
**	follow the guard, so they are the next object's: distance_calc.cpp. The asserts sit on the client's lines 47
**	('false') and 92 ('0'). No function here has a caller in the retail client.
*/
