// gutter_data.cpp: a client object's gutter icon and its two labels (0x7B4F30-0x7B6AB0); notes at the end
#include <string>
#include <string.h>
#include "gutter_data.h"
#include "co.h"
#include "co_data.h"
#include "radar_data.h"
#include "client.h"
#include "sclient.h"
#include "assetmgr.h"
#include "texture.h"
#include "camera.h"
#include "interface_scene.h"
#include "local_interface_sort.h"
#include "fontmanager.h"
#include "fontdatabase.h"
#include "user_click_indicator.h"
#include "tasks\tsk_login.h"
#include "matrix3d.h"
#include "wwmath.h"
#include "..\shared\g_assert.h"

bool Unknown_495FD0(RenderObjClass *robj, TextureClass *tex, int index);	// 0x495FD0 (ro_funcs.cpp): the texture onto the mesh's stage index
void Unknown_42ECF0(float *x, float *y);							// 0x42ECF0 (cdecl): the pointer's position, 0..1  // NAME 1 [x, y]: as gadget.cpp's

const char *Get_Icon_Name(ClientObjectClass *co);		// 0x7B6A00

inline void GutterDataClass::Remove_Labels_From_Scene()
{
	if (NameLabel && NameLabel->Peek_Scene()) {
		NameLabel->Remove();
	}
	if (GuildLabel && GuildLabel->Peek_Scene()) {
		GuildLabel->Remove();
	}
}

// @0x7B4F30
GutterDataClass::GutterDataClass(ClientObjectClass *co) :
	RadarIconDataClass(co, "gutter_icon"),
	NameLabel(NULL),
	GuildLabel(NULL),
	OnScreen(false),
	Indicator(NULL)
{
	RefCountPtr<TextureClass> icon = Create_NEW(WW3DAssetManager::Get_Instance()->Get_Texture(Get_Icon_Name(co), MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true));





























	FAIL_IF(!icon) return;
	Unknown_495FD0(Get_Icon_RObj().Peek(), icon.Peek(), 0);
}

// NOT EXACT: register (this in esi and spilled, the client ebx, frame 4 bytes less; the shape is the client's)
// @0x7B50D0
void GutterDataClass::Update_Name_Label(ClientObjectClass *co)
{
	FAIL_IF(!co) return;


	FAIL_IF(!co->AuxData) return;

	std::string text("");
	std::string level("");
	if (co->Unknown_6F5A10() && !co->Unknown_6F5A40()) {
		co->Unknown_6FF330(level);
	}
	if (!NameLabel) {
		co->Unknown_6FF7A0(text);
		text += level;
		NameLabel = Create_NEW(new FontSys("RADAR_NAME", 0, text.c_str()));
	} else {
		if (co->Unknown174 > 0.0f) {
			NameLabel->Change_Text("\"....................\"", 0);
			NameLabel->Unknown_6D8A80(true);
		} else {
			if (co->Unknown_6FFC80(text)) {
				text += level;
			} else if (strcmp(NameLabel->Unknown_6D87D0(), "\"....................\"") == 0) {
				co->Unknown_6FF7A0(text);
				text += level;
			} else {
				goto done;
			}
			NameLabel->Change_Text(text.c_str(), 0);
			NameLabel->Unknown_6D8A80(false);
		}
	}
done:
	;
}

// NOT EXACT: tail merge and register (the client keeps the first Set_Color's first stores and jumps to the third; this in esi, ours edi)
// @0x7B54E0
void GutterDataClass::Update_Guild_Label(ClientObjectClass *co)
{

	FAIL_IF(!co) return;

	std::string text;
	Vector3 color;
	if (!GuildLabel) {
		co->Unknown_6FFFF0(text);
		GuildLabel = Create_NEW(new FontSys("RADAR_NAME", 0, text.c_str()));
		if (co->Unknown_700670(color)) {
			GuildLabel->Set_Color(color);
		}
	} else {
		if (!co->Unknown_700400() && !Get_Client()->Unknown_7283F0()) {
			goto done;
		}
		co->Unknown_6FFFF0(text);
		GuildLabel->Change_Text(text.c_str(), 0);
		if (co->Unknown_700670(color)) {
			GuildLabel->Set_Color(color);
		}
	}
	GuildLabel->Set_Hidden(text.length() == 0);
done:
	;
}

// @0x7B57D0
GutterDataClass::~GutterDataClass()
{
	Set_Click_Indicator(false, NULL);
	Remove_From_Scene();
}

// @0x7B58F0
bool GutterDataClass::Add_To_Scene(const RefCountPtr<SimpleSceneClass> &scene)
{
	bool added = RadarIconDataClass::Add_To_Scene(scene);
	if (added) {
		if (Indicator) {
			InterfaceSortClass sort((InterfaceSceneClass *)scene.Peek(), false);
			Indicator->Unknown_6B3ED0(scene);
		}
		return(true);
	}
	return(false);
}

// @0x7B5A60
bool GutterDataClass::Remove_From_Scene()
{
	Remove_Labels_From_Scene();
	if (Indicator) {
		Indicator->Unknown_6B4400();
	}
	return(RadarIconDataClass::Remove_From_Scene());
}

// @0x7B5AD0
RefCountPtr<RenderObjClass> GutterDataClass::Get_Render_Obj()
{
	ClientObjectClass *co = Get_Client_Object();














































	FAIL_IF(!co) return(NULL);
	return(Create_Peek(co->RenderObject));
}

// @0x7B5B50
void GutterDataClass::Set_Sort_Group(int unused, int group)
{















	FAIL_IF(!Get_Icon_RObj()) return;

	int level = SortLevel;
	switch (group) {
		case 1:
			level -= 4;
			break;
		case 2:
			level -= 2;
			break;
	}
	Get_Icon_RObj()->Set_Sort_Level(level);
	if (NameLabel) {
		NameLabel->Set_Sort_Level(level - 1);
	}
	if (GuildLabel) {
		GuildLabel->Set_Sort_Level(level - 1);
	}
}

// @0x7B5BF0
float GutterDataClass::Update(const Vector3 &pos, float size, bool on_screen, const RefCountPtr<SimpleSceneClass> &scene, float scale)
{
	Add_To_Scene(scene);
	Vector3 pt = pos;
	OnScreen = on_screen;
	if (on_screen) {
		size = ((size - 0.017677668f) > 0.017677668f) ? (size - 0.017677668f) : 0.017677668f;
		pt.X -= 0.5f;
		pt.Z = 0.0f;
		pt.Y -= 0.5f;
		pt.Normalize();
		if (size > 0.15f) {
			size = 0.15f;
		}
		if (size + pos.X > 0.9875f) {
			size = 0.9875f - pos.X;
		} else if (pos.X - size < 0.0125f) {
			size = pos.X - 0.0125f;
		}
		if (size + pos.Y > 0.9875f) {
			size = 0.9875f - pos.Y;
		} else if (pos.Y - size < 0.0125f) {
			size = pos.Y - 0.0125f;
		}
		pt.X *= size;
		pt.Y *= size;
		pt.X += pos.X;
		pt.Y += pos.Y;
	} else {
		size = 0.017677668f;
	}
	Position.Set(pt.X, pt.Y);
	pt.Z = 0.0f;
	pt.X = pt.X * 640.0f - 320.0f;
	pt.Y = (float)(pt.Y * -480.0f) + 240.0f;
	Get_Icon_RObj()->Set_Position(pt);
	Set_Scale(scale);
	Update_Labels();
	return(size);
}

// @0x7B5FF0
void GutterDataClass::Hide()
{
	Remove_Labels_From_Scene();
	Remove_From_Scene();
}

// @0x7B6040
bool GutterDataClass::Set_Click_Indicator(bool turn_on, UIViewClass *view)
{
	if (Indicator) {
		delete Indicator;
		Indicator = NULL;
	}
	if (turn_on) {


































		FAIL_IF(!view) return(false);
		Indicator = new UserClickIndicatorClass;
		if (!Indicator->Unknown_6B3D00(view)) {
			Set_Click_Indicator(false, NULL);
			return(false);
		}
	}
	return(true);
}

// @0x7B6170
void GutterDataClass::Update_Indicator(const Vector3 &pos, float size)
{
	if (Indicator) {
		SphereClass sphere;
		sphere.Center.Z = 0.0f;
		sphere.Center.X = pos.X * 640.0f - 320.0f;
		sphere.Center.Y = (float)(pos.Y * -480.0f) + 240.0f;
		sphere.Radius = size * 640.0f;
		Indicator->Unknown_6B3F50(sphere);
	}
}

// @0x7B61F0
void GutterDataClass::Set_Scale(float scale)
{
	if ((scale >= 0.0f) && (scale <= 1.0f)) {
		Matrix3D m(true);
		RenderObjClass *icon = Get_Icon_RObj().Peek();
		m.Set_Translation(icon->Get_Transform().Get_Translation());
		if (0.1f > scale) {
			scale = 0.1f;
		}
		m.Scale(scale);
		Get_Icon_RObj()->Set_Transform(m);
	}
}

// NOT EXACT: register and slots (ours spills rdata and holds old_horizontal in ebp; the client the reverse)
// @0x7B63C0
void GutterDataClass::Update_Labels()
{
	RadarDataClass *rdata = Get_Radar_Data(Get_Client_Object(), false);
	if (!rdata) {
		return;
	}
	ClientObjectClass *rdata_co = rdata->Get_Client_Object();







	FAIL_IF(!rdata_co) return;
	const Vector3 &color = Get_Color();
	Update_Name_Label(rdata_co);
	Update_Guild_Label(rdata_co);
	if ((rdata->Unknown08 != 1) && (rdata->Unknown08 != 2) && ((rdata_co->Type != 1) || !OnScreen)) {
		Remove_Labels_From_Scene();
		return;
	}
	if (rdata_co->Unknown174 > 0.0f) {
		Vector3 masked_color = rdata_co->Unknown178;
		NameLabel->Set_Color(masked_color);
	} else {
		NameLabel->Set_Color(color);
	}
	if (rdata_co->Unknown_700860()) {
		NameLabel->Unknown_6D8A80(true);
	} else {
		NameLabel->Unknown_6D8A80(false);
	}
	Vector3 guild_color;
	if (!rdata_co->Unknown_700670(guild_color)) {
		GuildLabel->Set_Color(color);
	}
	if (!NameLabel->Peek_Scene()) {
		NameLabel->Add(Get_Icon_RObj()->Peek_Scene());
	}
	if (!GuildLabel->Peek_Scene()) {
		GuildLabel->Add(Get_Icon_RObj()->Peek_Scene());
	}
	RenderObjClass *cursor = NULL;
	LoginUnknown18Class *mouse = NULL;
	int old_vertical = NameLabel->Info.Justify[0];
	int old_horizontal = NameLabel->Info.Justify[1];
	if (rdata->Unknown08 == 1) {
		SClient *client = (SClient *)Unknown_18();
		if (client) {
			mouse = client->Mouse;
			if (mouse) {
				cursor = mouse->Unknown04;
			}
		}
	}
	Vector3 label_pos = Get_Icon_RObj()->Get_Position();
	int horizontal;
	int vertical;
	float offset_x;
	float offset_y;
	if (cursor) {
		Unknown_42ECF0(&label_pos.X, &label_pos.Y);
		label_pos.X -= mouse->Unknown1C * 0.5f;
		label_pos.Y += mouse->Unknown20 * 0.5f;
		horizontal = 3;
		if (old_horizontal == horizontal) {
			if (label_pos.X > 0.6) {
				horizontal = 4;
			}
		} else if (label_pos.X > 0.4) {
			horizontal = 4;
		}
		vertical = 1;
		const Vector3 &extent = cursor->Get_Bounding_Box().Extent;
		offset_x = (horizontal == 4) ? -0.001f : extent.X;
		label_pos.X = label_pos.X * 640.0f - 320.0f;
		offset_y = 0.0f;
		float width = 0.0f;
		float height = 0.0f;
		label_pos.Y = (float)(label_pos.Y * -480.0f) + 240.0f;
		NameLabel->Unknown_6D5B10(width, height);
		float lines = 2.0f;
		if (GuildLabel->Unknown300) {
			lines = 3.0f;
		}
		float needed = lines * height;
		if (label_pos.Y - needed < -240.0f) {
			label_pos.Y = needed - 240.0f;
		}
	} else {
		horizontal = 3;
		if (old_horizontal == horizontal) {
			if (Position.X > 0.6) {
				horizontal = 4;
			}
		} else if (Position.X > 0.4) {
			horizontal = 4;
		}
		vertical = 1;
		if (old_vertical == vertical) {
			if (Position.Y > 0.6) {
				vertical = 2;
			}
		} else if (Position.Y > 0.4) {
			vertical = 2;
		}
		offset_x = -0.011f;
		if (horizontal != 4) {
			offset_x = 0.011f;
		}
		offset_y = (vertical == 1) ? -0.011f : 0.011f;
	}
	float screen_x = offset_x * 640.0f;
	float screen_y = offset_y * 480.0f;
	NameLabel->Set_Offset(screen_x, screen_y);
	NameLabel->Set_Justify(vertical, horizontal);
	NameLabel->Set_Position(label_pos);
	float name_width, name_height;
	GuildLabel->Unknown_6D5B10(name_width, name_height);
	label_pos.Y -= name_height;
	GuildLabel->Set_Offset(screen_x, screen_y);
	GuildLabel->Set_Justify(vertical, horizontal);
	GuildLabel->Set_Position(label_pos);
}

// @0x7B6A00
const char *Get_Icon_Name(ClientObjectClass *co)
{

























































	FAIL_IF(!co) return("g_dead.tga");
	switch (co->Type) {
		case 0: return("g_npc.tga");
		case 1: return("g_pc.tga");
		case 2: return("g_cap.tga");
		case 3: return("g_planet.tga");
		case 4: return("g_item.tga");
		case 9: return("g_misile.tga");
		case 10: return("g_nebula.tga");
		case 11: return("g_gate.tga");
		case 12: return("g_sbase.tga");
		case 24: return("g_build.tga");
		case 25: return("g_dead.tga");
		case 27: return("g_mine.tga");
		case 34:
		case 38: return("g_resrce.tga");
		case 35: return("g_drone.tga");
		case 37: return("g_deco.tga");
		default:







			G_ASSERT(false);
			return("g_dead.tga");
	}
}

/*
**	Notes
**
**	The object: 0x7B4F30 (the constructor; __FILE__ 0xB9EC38, FAIL_IFs from line 76) to the <string> initialiser after
**	0x7B6A00. The hint's 0x7B5BF0 / 0x7B6170 are this class's Update and Update_Indicator (gutter_radar.cpp's 0x7B7040).
*/
