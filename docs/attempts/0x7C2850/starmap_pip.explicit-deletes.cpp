// Earth & Beyond client (reconstruction): the PDA's star map picture-in-picture (notes at the end)
#include <float.h>
#include <string.h>
#include <string>
#include "starmap_pip.h"
#include "radar_system.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "co_data.h"
#include "starmap_data.h"
#include "fontmanager.h"
#include "fontdatabase.h"
#include "option.h"
#include "local_interface_sort.h"
#include "intrface.h"
#include "starmap_pip_support.h"
#include "targeting.h"
#include "..\audio\soundlib.h"
#include "..\common\unknown_navlist.h"
#include "..\shared\g_assert.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\scene.h"
#include "wwmath.h"
#include "..\..\ww3d2\camera.h"
#include "mouse_targeting.h"
#include "nav_list_builder.h"
#include "radar_system_data_cache.h"
#include "..\..\ww3d2\layer.h"
#include "..\shared\auxdata\permstr.h"

static Vector3 CoordsColor(0.0f, 0.5f, 1.0f);		// 0xBE87A0 (0x7BF870): the avatar's or the map's position  // NAME 1: what 0x7C1C50 shows in it
static Vector3 PointerCoordsColor(0.1f, 1.0f, 0.1f);	// 0xBE87B0 (0x7BF8A0): the pointer's  // NAME 1: what 0x7C1C50 shows in it
static Vector3 TargetCoordsColor(1.0f, 0.1f, 0.1f);	// 0xBE8790 (0x7BF8D0): the target's  // NAME 1: what 0x7C1C50 shows in it
static Vector3 HoverCoordsColor(1.0f, 1.0f, 0.0f);	// 0xBE87C0 (0x7BF900): the hovered object's  // NAME 1: what 0x7C1C50 shows in it

// @0x7BF930
StarmapPipClass::StarmapPipClass(SClient *client) :
	MapPipClass(client, true),
	Unknown50(false),
	Unknown51(false),
	Unknown54(1.0f),
	ScaleMult(1.0f),
	CurMult(1.0f),
	Unknown60(0),
	Unknown68(0.0f, 0.0f, 0.0f),
	// (+0x70 is Unknown68.Z)
	Unknown74(0.0f, 0.0f),
	Unknown7C(0.0f, 0.0f),
	MapRange_ws(0.0f, 0.0f),
	TargetPath(NULL),
	WarpPath(NULL),
	Unknown94(-1),
	Unknown98(-1),
	MouseTarget(NULL),
	UnknownA0(NULL),
	UnknownA4(NULL),
	ShowCoords(0),
	UnknownB8(1.0f),
	UnknownBC(1.0f),
	Label(0)
{
	Unknown_BE67BC->Load("fc_radar.ini");
	TargetPath = new NavListLineDrawClass(Client, 0.75f, 1, Vector3(0.0f, 0.8f, 0.0f), 0.5f);
	WarpPath = new NavListLineDrawClass(Client, 0.75f, 2, Vector3(0.8f, 0.0f, 0.0f), 0.5f);
	UnknownA0 = new Unknown_7C3220Class(Scene, false, 4);
	UnknownA4 = new Unknown_7C3220Class(Scene, true, 4);
	XPos = Create_NEW(new FontSys("STARMAP_X_POS", 0, 0));
	YPos = Create_NEW(new FontSys("STARMAP_Y_POS", 0, 0));
	ZPos = Create_NEW(new FontSys("STARMAP_Z_POS", 0, 0));
	ShowCoords = OptionClass::Instance->Get_Int("Starmap_ShowCoords");
	Label = OptionClass::Instance->Get_Int("Starmap_Label");
}


// @0x7BFF20
StarmapPipClass::~StarmapPipClass()
{
	IntOptionStruct *option = OptionClass::Instance->Find_Int("Starmap_ShowCoords");
	if (option) {
		option->Set_Value(ShowCoords);
	}
	option = OptionClass::Instance->Find_Int("Starmap_Label");
	if (option) {
		option->Set_Value(Label);
	}
	OptionClass::Instance->Save();
	Unknown_7C2280();
	delete UnknownA0;
	UnknownA0 = NULL;
	delete UnknownA4;
	UnknownA4 = NULL;
	delete TargetPath;
	TargetPath = NULL;
	delete WarpPath;
	WarpPath = NULL;
	Unknown_BE67BC->Unload("fc_radar.ini");
}

// @0x7C01C0
GadgetClass *StarmapPipClass::Unknown_18()
{
	return(new GadgetStarmapClass(this));
}

// @0x7C0240
void StarmapPipClass::Unknown_1C(int arg)
{
	Unknown_7C2060(ShowCoords + 1);
}

// @0x7C0260
void StarmapPipClass::Unknown_20(int arg)
{
	Unknown60 = Client->AvatarID;
	if ((char)arg && Unknown50) {
		Unknown_Set_ScaleMult(0.5f);
	}
}



// @0x7C0330
void StarmapPipClass::Unknown_24(int arg)
{
	if (!Unknown50) {
		return;
	}
	switch (Label) {
		case 0:
			Label = 1;
			((SClient *)Client->Unknown_18())->Unknown_74D990(0, "Starmap PIP will now display labels of your own ship and other group member ships only.", 0);
			break;
		case 1:
			Label = 2;
			((SClient *)Client->Unknown_18())->Unknown_74D990(0, "Starmap PIP will now display labels of all visible ships.", 0);
			break;
		case 2:
			Label = 3;
			((SClient *)Client->Unknown_18())->Unknown_74D990(0, "Starmap PIP will now display all visible labels in the sector.", 0);
			break;
		case 3:
			Label = 0;
			((SClient *)Client->Unknown_18())->Unknown_74D990(0, "Starmap PIP will now not display any labels.", 0);
			break;
	}
	IntOptionStruct *option = OptionClass::Instance->Find_Int("Starmap_Label");
	if (option) {
		option->Set_Value(Label);
	}
	Unknown_7C2850(true);
}




// @0x7C0AF0
bool StarmapPipClass::Unknown_0C(bool on)
{
	if (on) {
		Unknown60 = Client->AvatarID;
	} else {
		Unknown_7C2280();
	}
	return(MapPipClass::Unknown_0C(on));
}


// @0x7C0BC0
bool StarmapPipClass::Unknown_04(const RefCountPtr<InterfaceClass> &iface_ptr, const RefCountPtr<RenderObjClass> &vp, int arg)
{
	if (!MapPipClass::Unknown_04(iface_ptr, vp, arg)) {
		return(false);
	}
	XPos->Set_Sort_Level(Unknown48->Get_Sort_Level());
	YPos->Set_Sort_Level(Unknown48->Get_Sort_Level());
	ZPos->Set_Sort_Level(Unknown48->Get_Sort_Level());
	Unknown_7C2060(ShowCoords);
	return(true);
}

// NOT EXACT: written from the pseudocode (D64), functional
// @0x7C0C80
bool StarmapPipClass::Unknown_10(const RefCountPtr<RenderObjClass> &vp)
{
	if (!MapPipClass::Unknown_10(vp)) {
		return(false);
	}
	Unknown68.Z = (ViewportSize.Y < ViewportSize.X) ? ViewportSize.X : ViewportSize.Y;
	Unknown_7C1510(false);
	Vector3 position(Rect.Left, Rect.Bottom, 0.0f);
	XPos->Set_Position(position);
	YPos->Set_Position(position);
	ZPos->Set_Position(position);
	return(true);
}


// @0x7C0D40
bool StarmapPipClass::Unknown_08()
{
	if (!MapPipClass::Unknown_08()) {
		return(false);
	}
	Unknown_7C2060(ShowCoords);
	return(true);
}


// @0x7C0D70
void StarmapPipClass::Unknown_14()
{
	MapPipClass::Unknown_14();
	Unknown50 = false;
	Unknown51 = false;
	if (ViewportGadget) {
		ViewportGadget->Unknown_40(false);
	}
	Unknown48->Set_Hidden(true);
	Unknown4C->Set_Hidden(true);
	Unknown74.Set(FLT_MAX, FLT_MAX);
	Unknown7C.Set(-FLT_MAX, -FLT_MAX);
	UnknownA4->Unknown_7C3830(NULL);
	UnknownA0->Unknown_7C3830(NULL);
	for (std::vector<Unknown_7C3220Class *>::iterator it = Labels.begin(); it != Labels.end(); it++) {
		delete *it;
	}
	Labels.clear();
}


// @0x7C0EA0
void StarmapPipClass::Unknown_7C0EA0(bool on)
{
	if (on) {
		Unknown60 = Client->AvatarID;
		return;
	}
	Unknown60 = 0;
}



































float Unknown_7BF630();								// 0x7BF630 (starmap_data.cpp: the PC icon size)
// NOT EXACT: shape (0.80): the client loads the gadget's Position.X through its address (add eax, 0x38) and puts the
// Unknown60 = 0 block after the return; forms tried in attempts.csv
// @0x7C0440
void StarmapPipClass::Unknown_7C0440(ClientObjectClass *hover, MouseTargetClass *mouse_target)
{
	FAIL_IF(MouseTarget) {
		MouseTarget = NULL;
	}
	if (!Unknown50 && !Unknown_7C1350()) {
		return;
	}
	if (Unknown51) {
		Unknown_7C1510(true);
	}
	if (!ViewportGadget->WrapPointer) {
		MouseTarget = mouse_target;
	}
	if (MouseTarget) {
		float size = Unknown_7BF630();
		MouseTarget->Set(ViewportGadget->Position.X, ViewportGadget->Position.Y, size / ViewportSize.X, size / ViewportSize.Y,
			LayerClass(Scene.Peek(), Camera.Peek()));
	}
	if (Unknown60) {
		ClientObjectClass *co = Client->Find_Object(Unknown60);
		if (co) {
			Vector3 pos = co->Unknown_24();
			Unknown_7C1770(pos.X, pos.Y);
		}
	} else {
		Unknown60 = 0;
	}
}












































void Unknown_491550(float *x, float *y);			// 0x491550 (scales by the device resolution; its file is not known)  // NAME 1: neutral (D10)
// @0x7C0620
void StarmapPipClass::Unknown_7C0620(RadarSystemDataCacheClass *entry)
{
	StarmapDataClass *sdata = entry->Get_Starmap_Data(true);


	FAIL_IF(!sdata) return;

	ClientObjectClass *co = sdata->Peek_Client_Object();
	FAIL_IF(!co) return;

	Vector3 pos = co->Unknown_24();
	float radius = co->Unknown_30();
	if (sdata->Update(entry, pos, Scene)) {
		Unknown_7C1280(pos, radius);
	}
	Vector3 map_pos(pos.X * CurMult, pos.Y * CurMult, 0.0f);
	Unknown_491550(&map_pos.X, &map_pos.Y);
	Vector2 position(map_pos.X - Unknown68.X, map_pos.Y - Unknown68.Y);
	Unknown_491550(&position.X, &position.Y);
	position.X = position.X / ViewportSize.X + 0.5f;
	position.Y = 0.5f - position.Y / ViewportSize.Y;
	float angle = co->Unknown_28(true).Get_Z_Rotation() - WWMATH_PI / 2.0f;
	sdata->Update_Transform(map_pos, position, angle, CurMult, co->Unknown_6F5A10());
	if (MouseTarget && position.X >= 0.0f && position.X <= 1.0f && position.Y >= 0.0f && position.Y <= 1.0f) {
		MouseTarget->Test(sdata, Vector3(position.X, position.Y, -1.0f), radius * CurMult, MouseTarget->Get_Half_Size().X);
	}
}


// @0x7C0910
void StarmapPipClass::Unknown_7C0910(ClientObjectClass *target, NavListBuilderClass *warp_path, NavListBuilderClass *target_path)
{
	if (Unknown94 != warp_path->Unknown1C) {
		Unknown94 = warp_path->Unknown1C;
		TargetPath->Unknown_7B99C0(Client->Get_Avatar(), warp_path, Scene, false);
	}
	int index = warp_path->Index;
	TargetPath->Unknown_7B9DC0(index);
	if (Unknown98 != target_path->Unknown1C) {
		Unknown98 = target_path->Unknown1C;
		WarpPath->Unknown_7B99C0(Client->Get_Avatar(), target_path, Scene, true);
	}
	WarpPath->Unknown_7B9DC0(target_path->Index);
	ClientObjectClass *hover = MouseTarget ? MouseTarget->Get_Target() : NULL;
	UnknownA4->Unknown_7C3830(hover);
	UnknownA0->Unknown_7C3830(target);
	Unknown_7C2850(false);
	Unknown_7C1C50(target, hover);
	MouseTarget = NULL;
}










































































































































































































































// @0x7C0EE0
NavigationListClass *StarmapPipClass::Unknown_7C0EE0()
{
	NavigationListClass *list;
	FAIL_IF(!WarpPath) return(NULL);
	FAIL_IF(!TargetPath) return(NULL);
	list = WarpPath->List;
	if (list && list->Get(-1)) {
		return(list);
	}
	list = TargetPath->List;
	if (list && list->Get(-1)) {
		return(list);
	}
	return(NULL);
}

















// @0x7C0FA0
bool StarmapPipClass::Unknown_7C0FA0()
{
	if (Unknown50) {
		RadarSystemClass *radar = ((SClient *)Client)->MainView->Radar;
		FAIL_IF(!radar) return(false);
		ClientObjectClass *hover = radar->Hover;
		if (hover && hover->Get_Game_ID() != Client->AvatarID) {
			TargetingClass *targeting = ((SClient *)Client)->Targeting;





			FAIL_IF(!targeting) return(false);
			targeting->Select_And_Set_Filter(hover);
			SoundLibraryClass::Play_2D_Sound("TextClick01.wav", 2);
			return(true);
		}
	}
	return(false);
}












// @0x7C1060
bool StarmapPipClass::Unknown_7C1060()
{
	if (!Unknown50) {
		return(false);
	}
	RadarSystemClass *radar = ((SClient *)Client)->MainView->Radar;
	FAIL_IF(!radar) return(false);
	return((ClientObjectClass *)radar->Hover != NULL);
}














// @0x7C10D0
bool StarmapPipClass::Unknown_7C10D0()
{
	RadarSystemClass *radar = ((SClient *)Client)->MainView->Radar;
	FAIL_IF(!radar) return(false);
	ClientObjectClass *hover = radar->Hover;
	if (hover) {
		Unknown60 = hover->Get_Game_ID();
		return(true);
	}
	return(false);
}


// @0x7C1120
void StarmapPipClass::Unknown_7C1120(float arg)
{
	if (Unknown50) {
		float mult = (ScaleMult + ScaleMult) * arg + ScaleMult;
		if (mult < 0.5f) {
			mult = 0.5f;
		}
		Unknown_Set_ScaleMult(mult);
	}
}

// @0x7C1200
Vector2 StarmapPipClass::Unknown_7C1200(const Vector2 &mouse)
{
	Vector2 pos((mouse.X - 0.5f) * ViewportSize.X, -((mouse.Y - 0.5f) * ViewportSize.Y));
	pos.X += Unknown68.X;
	pos.Y += Unknown68.Y;
	pos /= CurMult;
	return(pos);
}


// @0x7C1280
void StarmapPipClass::Unknown_7C1280(const Vector3 &position, float radius)
{
	bool changed = false;
	if (position.X - radius < Unknown74.X) {
		Unknown74.X = position.X - radius;
		changed = true;
	}
	if (Unknown7C.X < position.X + radius) {
		Unknown7C.X = position.X + radius;
		changed = true;
	}
	if (position.Y - radius < Unknown74.Y) {
		Unknown74.Y = position.Y - radius;
		changed = true;
	}
	if (Unknown7C.Y < position.Y + radius) {
		Unknown7C.Y = position.Y + radius;
		changed = true;
	}
	if (changed) {
		MapRange_ws.Set(Unknown7C.X - Unknown74.X, Unknown7C.Y - Unknown74.Y);
		Unknown51 = true;
	}
}

// For the inline below:
// 0x7C0260, 0x7C1120, 0x7C1510 and 0x7C1A90 expand it (FAIL_IF on 1039).





























































#pragma auto_inline(off)
// @0x7C1350
bool StarmapPipClass::Unknown_7C1350()
{
	bool found = false;
	Unknown_14();
	ClientObjectClass *avatar = Client->Get_Avatar();
	HashNodeClass<ClientObjectClass *> *node = Client->ObjectList.First_Valid();
	while (node) {
		ClientObjectClass *co = node->Get_Record();
		FAIL_IF(!co) continue;
		if (co == avatar || Get_Navigation_Data(co, false)) {
			Unknown_7C1280(co->Unknown_24(), co->Unknown_30());
			found = true;
		}
		node = node->Next_Valid();
	}
	if (!found) {
		return(false);
	}
	Unknown50 = true;
	if (ViewportGadget) {
		ViewportGadget->Unknown_40(true);
	}
	Unknown48->Set_Hidden(false);
	Unknown4C->Set_Hidden(false);
	Unknown_7C1510(false);
	if (!Unknown60) {
		if (ScaleMult == 1.0f) {
			Unknown_7C1770(MapRange_ws.X * 0.5f + Unknown74.X, MapRange_ws.Y * 0.5f + Unknown74.Y);
		} else if (avatar) {
			Vector3 pos = avatar->Unknown_24();
			Unknown_7C1770(pos.X, pos.Y);
		}
	}
	return(true);
}
#pragma auto_inline(on)






















// @0x7C1510
void StarmapPipClass::Unknown_7C1510(bool arg)
{
	if (Unknown50) {
		FAIL_IF(MapRange_ws.X < WWMATH_EPSILON) return;
		FAIL_IF(MapRange_ws.Y < WWMATH_EPSILON) return;
		float old = Unknown54;
		Vector2 scale(ViewportSize.X / MapRange_ws.X, ViewportSize.Y / MapRange_ws.Y);
		Unknown_Set_54(min(scale.X, scale.Y));
		if (arg) {
			Unknown_7C1A90(old * ScaleMult / Unknown54);
		}




		Unknown51 = false;
	}
}
































// NOT EXACT: stack (the client keeps diff at +0x24 and t at +0x1C, the other way round: 0.88)

#pragma auto_inline(off)
// @0x7C1770
void StarmapPipClass::Unknown_7C1770(float x, float y)
{
	if (Unknown50) {
		FAIL_IF(!Camera) return;
		x = WWMath::Clamp(x, Unknown74.X, Unknown7C.X);
		y = WWMath::Clamp(y, Unknown74.Y, Unknown7C.Y);
		Unknown68.X = x * CurMult;
		Unknown68.Y = y * CurMult;
		Vector3 pos = Unknown68;
		Unknown_491550(&pos.X, &pos.Y);
		Camera->Set_Position(pos);
		int left = (int)(pos.X - ViewportSize.X * 0.5f);
		int top = (int)(ViewportSize.Y * 0.5f + pos.Y);
		int right = (int)(ViewportSize.X * 0.5f + pos.X);
		int bottom = (int)(pos.Y - ViewportSize.Y * 0.5f);
		UnknownA0->Unknown_7C3A90(left, top, right, bottom);
		UnknownA4->Unknown_7C3A90(left, top, right, bottom);
		for (std::vector<Unknown_7C3220Class *>::iterator it = Labels.begin(); it != Labels.end(); it++) {
			(*it)->Unknown_7C3A90(left, top, right, bottom);
		}














		FAIL_IF(ScaleMult < WWMATH_EPSILON) return;
		Vector2 range = MapRange_ws * 3.0f;
		Vector2 min = MapRange_ws / ScaleMult;
		Vector2 diff = range - min;
		Vector2 t((x - Unknown74.X) / MapRange_ws.X, (Unknown7C.Y - y) / MapRange_ws.Y);
		Unknown48->Unknown_7C3CA0(range.X, min.X, diff.X * t.X, true);
		Unknown4C->Unknown_7C3CA0(range.Y, min.Y, t.Y * diff.Y, false);
	}
}
#pragma auto_inline(on)




















inline void StarmapPipClass::Unknown_Set_ScaleMult(float mult)
{
	if (ScaleMult == mult) return;
	FAIL_IF(CurMult < WWMATH_EPSILON) {
		CurMult = 1.0f;
	}
	Vector2 scale(Unknown68.X / CurMult, Unknown68.Y / CurMult);
	ScaleMult = mult;
	CurMult = Unknown54 * mult;
	Unknown_7C1770(scale.X, scale.Y);
}


// @0x7C1A90
void StarmapPipClass::Unknown_7C1A90(float mult)
{
	if (Unknown50) {
		if (mult < 0.5f) {
			mult = 0.5f;
		}
		Unknown_Set_ScaleMult(mult);
	}
}







inline void StarmapPipClass::Unknown_Set_54(float value)
{
	if (Unknown54 == value) return;
	FAIL_IF(CurMult < WWMATH_EPSILON) {
		CurMult = 1.0f;
	}
	Vector2 scale(Unknown68.X / CurMult, Unknown68.Y / CurMult);
	Unknown54 = value;
	CurMult = ScaleMult * value;
	Unknown_7C1770(scale.X, scale.Y);
}


// @0x7C1B70
void StarmapPipClass::Unknown_7C1B70(float value)
{
	if (Unknown50) {
		Unknown_Set_54(value);
	}
}

// @0x7C1C30
void StarmapPipClass::Unknown_7C1C30()
{
	CurMult = ScaleMult * Unknown54;
}



































// @0x7C1C50
void StarmapPipClass::Unknown_7C1C50(ClientObjectClass *target, ClientObjectClass *hover)
{
	if (!Unknown50) {
		return;
	}
	if (!ShowCoords) {
		return;
	}
	Vector3 pos;
	const Vector3 *color;
	bool inside = Unknown_693020();
	bool show = true;
	if (inside && hover) {
		pos = hover->Unknown_24();
		color = &HoverCoordsColor;
	} else if (inside && target && MouseTarget && MouseTarget->Get_Self_Priority()) {
		pos = target->Unknown_24();
		color = &TargetCoordsColor;
	} else if (inside && ShowCoords == 2) {
		Vector2 p = Unknown_7C1200(ViewportGadget->Position);
		pos.X = p.X;
		pos.Y = p.Y;
		pos.Z = 0.0f;
		color = &PointerCoordsColor;
	} else {
		ClientObjectClass *co;
		if (Unknown60 && (co = Client->Find_Object(Unknown60)) != NULL) {
			pos = co->Unknown_24();
		} else if (ShowCoords == 2) {
			FAIL_IF(CurMult < WWMATH_EPSILON) {
				CurMult = 1.0f;
			}
			pos.Set(Unknown68.X / CurMult, Unknown68.Y / CurMult, 0.0f);
		} else {
			show = false;
		}
		color = &CoordsColor;
	}
	if (show) {
		XPos->Unknown_6CE420(0, "X: %5.2f", pos.X * 0.001f);
		YPos->Unknown_6CE420(0, "Y: %5.2f", pos.Y * 0.001f);
		ZPos->Unknown_6CE420(0, "Z: %5.2f", pos.Z * 0.001f);
		XPos->Set_Color(*color);
		YPos->Set_Color(*color);
		ZPos->Set_Color(*color);
	} else {
		XPos->Change_Text("", 0);
		YPos->Change_Text("", 0);
		ZPos->Change_Text("", 0);
	}
}


// NOT EXACT: written from the pseudocode (D64), functional
// @0x7C2060
void StarmapPipClass::Unknown_7C2060(int mode)
{
	ShowCoords = mode;
	if (!Interface) {
		if (XPos->Peek_Scene()) {
			XPos->Remove();
		}
		if (YPos->Peek_Scene()) {
			YPos->Remove();
		}
		if (ZPos->Peek_Scene()) {
			ZPos->Remove();
		}
	} else if (mode > 0 && mode <= 2) {
		InterfaceSortClass sort(Interface.Peek());
		if (!XPos->Peek_Scene()) {
			XPos->Add((SceneClass *)Interface->Unknown44);
		}
		if (!YPos->Peek_Scene()) {
			YPos->Add((SceneClass *)Interface->Unknown44);
		}
		if (!ZPos->Peek_Scene()) {
			ZPos->Add((SceneClass *)Interface->Unknown44);
		}
	} else {
		if (XPos->Peek_Scene()) {
			XPos->Remove();
		}
		if (YPos->Peek_Scene()) {
			YPos->Remove();
		}
		if (ZPos->Peek_Scene()) {
			ZPos->Remove();
		}
		ShowCoords = 0;
	}
}

// @0x7C2280
void StarmapPipClass::Unknown_7C2280()
{
	for (std::vector<Unknown_7C3220Class *>::iterator it = Labels.begin(); it != Labels.end(); it++) {
		delete *it;
	}
	Labels.clear();
}

// @0x7C2340
void StarmapPipClass::Unknown_7C2340(unsigned id, std::string name, int type)
{
	ClientObjectClass *co = ((SClient *)Client->Unknown_18())->Find_Object(id);
	if (!co) {
		return;
	}
	Unknown_7C3220Class *label = new Unknown_7C3220Class(Scene, false, type);
	Vector3 pos = Unknown68;
	Unknown_491550(&pos.X, &pos.Y);
	label->Unknown_7C3A90((int)(pos.X - ViewportSize.X * 0.5f), (int)(ViewportSize.Y * 0.5f + pos.Y),
		(int)(ViewportSize.X * 0.5f + pos.X), (int)(pos.Y - ViewportSize.Y * 0.5f));
	label->Set_Name(name);
	label->Unknown_7C3830(co);
	Labels.push_back(label);
}


// @0x7C25B0
void Unknown_7C3220Class::Set_Name(std::string name)
{
	Name = name;
}

#include "..\shared\auxdata\auxdata.h"
#include "aux_roots.h"					// DeltaGroupInfoClass (0x7C2850)

// @0x7C2850
void StarmapPipClass::Unknown_7C2850(bool force)
{
	if (!Label) {
		for (std::vector<Unknown_7C3220Class *>::iterator dit = Labels.begin(); dit != Labels.end(); dit++) {
			delete *dit;
		}
		Labels.erase(Labels.begin(), Labels.end());
		return;
	}
	if (!Client->Get_Objects_Changed() && Labels.size() && !force) {
		for (std::vector<Unknown_7C3220Class *>::iterator it = Labels.begin(); it != Labels.end(); it++) {
			ClientObjectClass *co = ((SClient *)Client->Unknown_18())->Find_Object((*it)->ObjectID);
			if (co) {
				(*it)->Unknown_7C3830(co);
			}
		}
		return;
	}
	for (std::vector<Unknown_7C3220Class *>::iterator lit = Labels.begin(); lit != Labels.end(); lit++) {
		delete *lit;
	}
	Labels.erase(Labels.begin(), Labels.end());
	HashNodeClass<ClientObjectClass *> *node = Client->ObjectList.First_Valid();
	while (node) {
		ClientObjectClass *co = node->Get_Record();
		node = node->Next_Valid();
		StarmapDataClass *sdata = Get_Starmap_Data(co, false);
		if (!sdata || sdata->Unknown38) {
			continue;
		}
		bool grouped;
		if (co->Unknown_6F5A40() && ((SClient *)Client->Unknown_18())->AuxData) {
			NameClass name("GroupInfo");
			DeltaGroupInfoClass *group = (DeltaGroupInfoClass *)Unknown_Find_Field<DeltaCompoundClass>(((SClient *)Client->Unknown_18())->AuxData, name);
			if (group) {
				for (int i = 0; i < 5; i++) {
					if (co->Get_Game_ID() == ((DeltaGroupMemberClass *)group->Members.Children[i])->GameID.Value) {
						grouped = true;
						goto found;
					}
				}
			}
		}
		grouped = false;
found:
		bool player = co->Unknown_6F5A40();
		bool avatar = Client->AvatarID == co->Get_Game_ID();
		std::string name("");
		co->Unknown_6FF7A0(name);
		bool flag = player && co->Unknown_700860();
		if (!name.length() || (Label == 1 && !grouped && !avatar) || (Label == 2 && !player)) {
			continue;
		}
		if (flag) {
			Unknown_7C2340(co->Get_Game_ID(), name, 3);
		} else if (grouped) {
			Unknown_7C2340(co->Get_Game_ID(), name, 0);
		} else if (player) {
			Unknown_7C2340(co->Get_Game_ID(), name, 1);
		} else {
			Unknown_7C2340(co->Get_Game_ID(), name, 2);
		}
	}
}

// @0x7C2D30
void StarmapPipClass::Unknown_7C2D30()
{
	TargetPath->Unknown_7B9CD0();
	WarpPath->Unknown_7B9CD0();
}

// 0x7C2DA0 (the labels' vector clear) .. 0x7C30E0 are not rebuilt yet.


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


#pragma auto_inline(off)
// @0x7C3A90
void Unknown_7C3220Class::Unknown_7C3A90(int left, int top, int right, int bottom)
{
	Text->Unknown_6D88F0(left, top, right, bottom);
}
#pragma auto_inline(on)

/*
**	Reconstruction notes
**
**	The object's boundaries in the client: __FILE__ 0xB9F4D8 ('e:\build\beta\G\app\client\starmap_pip.cpp'),
**	asserts on lines 283-1157 from 0x7C0260 to 0x7C1C50. The constructor 0x7BF930 sets the table 0xB06C8C
**	(slots 0x7BFEF0, 0x7C0BC0, 0x7C0D40, 0x7C0AF0, 0x7C0C80, 0x7C0D70, 0x7C01C0, 0x7C0240, 0x7C0260, 0x7C0330)
**	and its literals ('STARMAP_X_POS' 0xB9F4A8 .. 'Starmap_ShowCoords' 0xB9F470) sit just before the file's
**	first FAIL_IF strings (0xB9F4B8, 0xB9F4D8); the label class's 'RADAR_*' literals (0xB9F704-0xB9F734) follow
**	the slots' messages (0xB9F50C-0xB9F5D8). 'Starmap_Label' 0xB9F31C is pooled with an earlier object's.
**	0x7C3A90 ends the object: a forward to FontSys called only from this file (0x7C1770, 0x7C2340), defined after
**	0x7C3830 where the order puts it; starmap_pip_support.cpp starts at 0x7C3AC0.
**
**	The FAIL_IFs on 1039 and 1069 sit in inlines the client expands (1039: 0x7C0260, 0x7C1120, 0x7C1510, 0x7C1A90;
**	1069: 0x7C1510, 0x7C1B70): Unknown_Set_ScaleMult and Unknown_Set_54 here, defined where their lines put them, after
**	their first callers (VC6 inlines them from the later definition). 0x7C2280 (no caller) is the labels' delete
**	that 0x7C0AF0 and the destructor expand; 0x7C0D70 has the loop written out (that form is the exact one there).
**	+0x84 is a Vector2: as two ints the constructor's 'or ecx,-1' moved one store earlier.
**
**	The object starts at 0x7BF820: permstr.h's PermStringInitClass static (0xBE87BC), then the four colours of
**	0x7C1C50's coordinates (0x7BF870-0x7BF900, in their definition order). 0x7C1C50 calls 0x7C1200 inline and keeps
**	the tail after the fallback only in the if (show) {} else {} form. 0x7C3A90 sits under auto_inline(off): the
**	client calls it from 0x7C1770 and 0x7C2340 (VC6 expands a later definition otherwise).
**
**	Exact: 0x7BF870, 0x7BF8A0, 0x7BF8D0, 0x7BF900, 0x7BF930, 0x7BFF20, 0x7C01C0, 0x7C0240, 0x7C0330, 0x7C0620, 0x7C0910,
**	0x7C0AF0, 0x7C0BC0, 0x7C0D40, 0x7C0D70, 0x7C0EA0, 0x7C0EE0, 0x7C0FA0, 0x7C1060, 0x7C10D0, 0x7C1120, 0x7C1200, 0x7C1280,
**	0x7C1350, 0x7C1510, 0x7C1A90, 0x7C1B70, 0x7C1C30, 0x7C1C50, 0x7C2280, 0x7C2340, 0x7C25B0, 0x7C2D30, 0x7C3220, 0x7C3720,
**	0x7C3830, 0x7C3A90. NOT EXACT (D64, functional; attempts.csv): 0x7C0260 (MapPipClass's slot +0x20 takes an
**	int, the client tests a byte), 0x7C0440 (the gadget's Position.X load, the else block's place), 0x7C0C80 (the max's
**	fcompp form), 0x7C2060 (registers, order), 0x7C1770 (two stack slots swapped; +0x68..+0x70 is one Vector3, Unknown68,
**	which it copies whole). Not rebuilt: 0x7C2850 (the labels), 0x7C2DA0-0x7C30E0 (the POC and clienttest get them
**	from generated stubs).
*/
