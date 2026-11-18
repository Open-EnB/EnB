// Earth & Beyond client (reconstruction): a line drawn on the star map PIP (notes at the end)
#include "starmap_pip_support.h"
#include "..\shared\g_assert.h"
#include "vector3.h"
#include "aux_group.h"	// the group constructors (0x7C3E20 / 0x7C3E50 number them)
#include <string.h>
#include "starmap_pip.h"
#include "sclient.h"
#include "co.h"
#include "co_data.h"
#include "starmap_data.h"
#include "fontmanager.h"
#include "fontdatabase.h"
#include "radar_system_data_cache.h"
#include "..\..\ww3d2\scene.h"

// @0x7C3220
Unknown_7C3220Class::Unknown_7C3220Class(const RefCountPtr<SceneClass> &scene, bool arg, int type) :
	ObjectID(0),
	Scene(scene),
	Name("")
{
	Unknown_BE67BC->Load("fc_radar.ini");
	switch (type) {
		case 4:
			Text = Create_NEW(new FontSys("RADAR_NAME", 0, 0));
			Text->Set_Sort_Level(arg ? 1 : 2);
			Text->Unknown_6CE910(!arg);
			break;
		case 3:
			Text = Create_NEW(new FontSys("RADAR_JUMPSTART", 0, 0));
			Text->Set_Sort_Level(3);
			Text->Unknown_6D8A80(true);
			break;
		case 0:
			Text = Create_NEW(new FontSys("RADAR_GROUP", 0, 0));
			Text->Set_Sort_Level(4);
			break;
		case 1:
			Text = Create_NEW(new FontSys("RADAR_PLAYER", 0, 0));
			Text->Set_Sort_Level(5);
			break;
		case 2:
			Text = Create_NEW(new FontSys("RADAR_OBJECT", 0, 0));
			Text->Set_Sort_Level(6);
			break;
	}
}


// @0x7C3720
Unknown_7C3220Class::~Unknown_7C3220Class()
{
	Text->Remove();
	Text = NULL;
	Unknown_BE67BC->Unload("fc_radar.ini");
}

// @0x7C3830
void Unknown_7C3220Class::Unknown_7C3830(ClientObjectClass *co)
{
	if (co) {
		unsigned id = co->Get_Game_ID();
		std::string name("");
		co->Unknown_6FF7A0(name);
		if (ObjectID != id || strcmp(Text->Unknown_6D87D0(), name.c_str()) != 0) {
			ObjectID = id;
			if (name.compare("Unknown") == 0 && Name.length()) {
				name = Name;
			}
			Text->Change_Text(name.c_str(), 0);
			if (!Text->Peek_Scene()) {
				Text->Add(Scene.Peek());
			}
		}
		if (ObjectID) {
			StarmapDataClass *sdata = Get_Starmap_Data(co, false);
			if (sdata) {
				Text->Set_Position(sdata->Get_Position());
			} else {
				ObjectID = 0;
				Text->Remove();
			}
		}
	} else if (ObjectID) {
		ObjectID = 0;
		Text->Remove();
	}
}


// @0x7C3A90
void Unknown_7C3220Class::Unknown_7C3A90(int left, int top, int right, int bottom)
{
	Text->Unknown_6D88F0(left, top, right, bottom);
}


StarmapPipSupportClass::StarmapPipSupportClass() :
	Unknown_466920Class(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 2.0f, 0.8f, 0.8f, 0.8f, 0.8f),
	Unknown10C(0.0f, 0.0f),
	Unknown114(1.0f)
{
}


StarmapPipSupportClass::~StarmapPipSupportClass()
{










































	G_ASSERT(!Peek_Scene());
}

void StarmapPipSupportClass::Unknown_7C3C60(const Vector2 &arg1, float arg2)
{
	Unknown10C = arg1;
	Unknown114 = arg2;
}

void StarmapPipSupportClass::Unknown_7C3CA0(float total_range, float arg1, float arg2, bool arg3)
{
	FAIL_IF(!total_range) return;
	float a = arg1 / total_range;
	if (1.0f < a) {
		a = 1.0f;
	}
	a = a * Unknown114;
	float b = arg2 / total_range;
	if (1.0f < b) {
		b = 1.0f;
	}
	b = b * Unknown114;
	if (arg3) {
		float x = b + Unknown10C.X;
		float y = Unknown10C.Y - 1.0f;
		Reset(Vector3(x, y, 0.0f), Vector3(x + a, y, 0.0f));
	} else {
		float y = Unknown10C.Y - b;
		float x = Unknown10C.X + 1.0f;
		Reset(Vector3(x, y, 0.0f), Vector3(x, y - a, 0.0f));
	}
}

/*
**	Reconstruction notes
**
**	The object's window is 0x7C3AC0 to 0x7C3EB3: __FILE__ 0xB9F758 on the asserts at lines 151 and 162, the literals
**	0xB9F748 ('!Peek_Scene()') and 0xB9F798 ('!total_range') beside it. The object itself starts at 0x7C31D0 (PS, run
**	2026-11-18): the CRT table's entry 667 is starmap_pip.cpp's <string> initialiser 0x7C3180, entry 668 this object's
**	permstr.h static 0x7C31D0 (atexit 0x7C3200), so the star map labels (Unknown_7C3220Class: 0x7C3220, 0x7C3720, 0x7C3830,
**	0x7C3A90, which starmap_pip.cpp calls) are this file's, defined first; the blank lines above the asserts absorb them. 0x7C3F10 up (table 0xB07008) reaches target_find.cpp's
**	literals: 'idx >= MAX_VISIBLE_GROUP_MEMBERS' at 0xB9F7AC is its FAIL_IF (0x7C43A0, __FILE__ 0xB9F7D4), not this file's.
*/
