// Earth & Beyond client (reconstruction): app\client\ui_starbase.cpp (the client's __FILE__
// 'e:\build\beta\G\app\Client\ui_starbase.cpp', 0x632E50-0x648340 by its asserts, its string order and ui_slot_scut.cpp's end).
// The starbase interface (constructor 0x6333E0, SB1), its helpers 0x638FF0-0x6437A0 as far as rebuilt (SB1, run 2026-11-09b), its lights helpers
// 0x640CC0-0x640F30 and the lights panel (0x646170-0x648238, ST1); the functions with asserts sit on the client's lines (2004-4180) in the padding.
#include <stdio.h>
#include <string.h>
#include <vector>
#include "ui_chat_panel.h"
#include "sbaseview.h"
#include "gd_slider.h"
#include "fontmanager.h"
#include "..\..\ww3d2\light.h"
#include "wwmath.h"
#include "aabox.h"
#include "matrix3d.h"
#include "rendobj.h"
#include "scene.h"
#include "assetmgr.h"
#include "starbase_lights.h"
#include "..\audio\soundlib.h"
#include "control_keys_manager.h"
#include "avatar.h"
#include "tasks\tsk_login.h"
#include "..\shared\g_assert.h"
#include "..\packets\pkstarbase_avatar_change.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\shared\message.h"

// The client at InterfaceClass +0x40, as ui_chat_panel.cpp declares it
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (SClient's returns itself)  // NAME 3: ui_manu.cpp's FAIL_IF's
	virtual void Unknown_1C(); virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C(); virtual void Unknown_40();
	virtual void Unknown_44(int arg);					// +0x44 (the starbase interface's destructor passes its +0x190's +0x90)  // NAME 1 [arg]: neutral
	char Unknown04[0x112C - 0x04];
	unsigned int AvatarID;								// +0x112C (client.h's ClientClass::AvatarID)  // NAME 3: client.h's
	unsigned int Get_Avatar_ID() { return(AvatarID); }	// client.h's
};
extern FontManagerClass *Unknown_BE67C4;	// NAME 1 [Unknown_BE67C4]: 0xBE67C4, as ui_login.cpp's

void Unknown_496C00(RenderObjClass *robj, float u, float v, Vector3 *color);	// 0x496C00 (ro_funcs: the texture's colour at u, v)  // NAME 1 [robj, u, v, color]: as ui_avatar_cust.cpp's
void Unknown_42ECF0(float *x, float *y);			// 0x42ECF0 (the pointer, 0..1)  // NAME 1 [x, y]: as ui_avatar_cust.cpp's
void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl)  // NAME 1 [robj, make_geometry_unique]: as ui_avatar_cust.cpp's
bool Unknown_42EC60(int button, int arg);		// 0x42EC60 (input.cpp: a button or key down)  // NAME 1 [button, arg]: neutral

// The starbase view's +0x70 scene (sbaseview.cpp's SBaseViewUnknown70Class), as far as the starbase interface calls it
class UIStarbaseSceneClass : public SceneClass	// NAME 1: neutral (D10)
{
public:
	void Unknown_7621A0(RenderObjClass *obj);			// 0x7621A0  // NAME 1 [obj]: neutral
	void Unknown_762920(float fill);					// 0x762920 (its +0xAC)  // NAME 1 [fill]: what 0x640EC0 passes
	void Unknown_762940(const Vector3 &color);			// 0x762940 (its +0xA0: the ambient colour)  // NAME 1 [color]: what 0x640E90 passes
};

// The starbase avatar (unknown_78a910.cpp's Unknown78A910AvatarClass)
class Unknown78A910AvatarClass : public CharacterViewAvatarClass	// NAME 1: neutral (D10)
{
public:
	void Unknown_78B570(bool b);						// 0x78B570  // NAME 1 [b]: neutral
};

// The four rooms (the constructor's index): the key 0x698730 looks up (0xB8A870), the room's model part (0xB8A880),
// and the race letters by the starbase's column +0x18 (0xB8A950)
static int Unknown_B8A870[4] = { 0, 1, 3, 2 };	// NAME 1: neutral (D10)
static const char *Unknown_B8A880[4] = { "hangar", "main", "bazaar", "lounge" };	// NAME 1: neutral (D10)
static const char *Unknown_B8A950[10] = { "t", "j", "p", "n", "n", "n", "n", "n", "n", "n" };	// NAME 1: neutral (D10)

// kernel.h's input event, as UIStarbaseLightsKeyEventStruct below, with its second short
struct UIStarbaseKeyEventStruct	// NAME 1: neutral (D10)
{
	UIStarbaseKeyEventStruct(short key, short modifiers) : Key(key), Unknown02(modifiers), Unknown04(0), Unknown08(0), Unknown0C(0) {}	// NAME 1 [key, modifiers]: neutral
	short Key;											// +0x00  // NAME 1: neutral (D10)
	short Unknown02;									// +0x02
	float Unknown04;									// +0x04
	float Unknown08;									// +0x08
	int Unknown0C;										// +0x0C
};

// @0x00632E50
UIStarbaseUnknownCCClass::~UIStarbaseUnknownCCClass()
{
	if (Avatar) {
		Avatar->Unknown_788EF0();
		Avatar->Remove();
		Avatar = NULL;
	}
	if (Unknown0C) {
		Unknown0C->Remove();
		Unknown0C = NULL;
	}
	if (Unknown10) {
		Unknown10->Remove();
		Unknown10 = NULL;
	}
	if (Unknown14) {
		GadgetClass::Destroy(Unknown14);
		Unknown14 = NULL;
	}
}

inline SBaseViewUnknown1B4Class *UIStarbaseClass::Unknown_Info() { return(UnknownB0->Unknown1B4); }
// 0x6333E0
UIStarbaseClass::UIStarbaseClass(UIViewClass *view, LayerClass *layer, int index, const char *name) :
	Unknown5BB320Class(new UIMeshClass(name, 1), view, *layer),
	Room(index),
	UnknownB0((SBaseViewClass *)view),
	UnknownB4(NULL),
	UnknownB8(0),
	UnknownBC(NULL),
	UnknownD4(100),
	Unknown180(0),
	Unknown184(NULL),
	Unknown188(NULL),
	Unknown18C(false),
	Unknown190(0),
	Unknown194(NULL),
	Unknown1A0(NULL),
	Unknown1AC(0),
	Unknown1B0(false),
	Unknown1B1(false),
	Unknown1B2(false),
	Unknown1B3(false),
	Unknown1B4(-1.0f),
	Unknown1B8(0.5f),
	Unknown1EC(0.0f, 0.0f, 0.0f),
	Unknown1F8(NULL),
	Unknown1FC(false),
	Unknown1FD(false),
	Unknown1FE(false),
	Unknown1FF(false),
	Unknown200(false),
	Unknown201(false),
	Unknown214(false),
	Unknown215(false),
	Unknown216(false),
	Unknown217(false),
	Unknown218(false),
	Unknown219(false),
	Unknown21A(false),
	Unknown21B(false),
	Unknown21C(false),
	Unknown220(0)
{
	int i;
	for (i = 0; i < 4; i++) {
		UnknownF8[i] = 0;
		Unknown108[i] = NULL;
	}
	for (i = 0; i < 4; i++) {
		Unknown118[i] = 0;
		Unknown128[i] = NULL;
		Unknown138[i] = false;
	}
	for (i = 0; i < 14; i++) {
		Unknown13C[i] = 0;
	}
	UnknownB0 = (SBaseViewClass *)View->Unknown_30();
	((ControlKeyListClass *)Unknown4C)->Add(*(const ControlKeyComboStruct *)&UIStarbaseKeyEventStruct(2, 0), (void *)0x2B);
	((ControlKeyListClass *)Unknown4C)->Add(*(const ControlKeyComboStruct *)&UIStarbaseKeyEventStruct(2, 0x1000), (void *)0x2C);
	UnknownB4 = (UIStarbaseUnknownB4Struct *)Unknown_Info()->Unknown_698730(Unknown_B8A870[Room]);
	int column = Unknown_Info()->Unknown18;
	char model[32];
	if (UnknownB4->Unknown0B) {
		_snprintf(model, sizeof(model), "%sextra_%s_3", Unknown_B8A950[column], Unknown_B8A880[Room]);
	} else if (UnknownB4->Unknown0A) {
		_snprintf(model, sizeof(model), "%sextra_%s_2", Unknown_B8A950[column], Unknown_B8A880[Room]);
	} else {
		_snprintf(model, sizeof(model), "%sextra_%s", Unknown_B8A950[column], Unknown_B8A880[Room]);
	}
	UnknownBC = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj(model));
	if (UnknownBC) {
		Unknown_Model()->Add_Sub_Object(UnknownBC.Peek());
	}
}

#pragma auto_inline(off)

// @0x00633B50
UIStarbaseClass::~UIStarbaseClass()
{
	int i;
	if (Unknown190) {
		Client->Unknown_44(Unknown190->Get_Unknown90());
		delete Unknown190;
		Unknown190 = NULL;
	}
	UIStarbaseClass::Unknown_5C();
	Unknown1F8 = NULL;
	UnknownB4 = NULL;
	InterfaceMesh->Unknown_5B9930(-1);
	if (UnknownBC) {
		UnknownBC->Remove();
		UnknownBC = NULL;
	}
	delete UnknownC0.D;
	if (Unknown180) {
		GadgetClass::Destroy(Unknown180);
		Unknown180 = NULL;
	}
	if (Unknown184) {
		Unknown184->Remove();
		Unknown184 = NULL;
	}
	for (i = 0; i < 14; i++) {
		delete Unknown13C[i];
	}
	for (i = 0; i < 4; i++) {
		if (Unknown128[i]) {
			Unknown128[i]->Remove();
			Unknown128[i] = NULL;
		}
	}
	for (i = 0; i < 4; i++) {
		if (Unknown108[i]) {
			Unknown108[i]->Remove();
			Unknown108[i] = NULL;
		}
	}
	std::list<RefCountPtr<RenderObjClass> >::iterator it;
	for (it = Unknown194.begin(); it != Unknown194.end(); it++) {
		RefCountPtr<RenderObjClass> robj = *it;
		InterfaceMesh->Peek_Object()->Remove_Sub_Object(robj.Peek());
		robj->Remove();
	}
	Unknown194.clear();
	for (it = Unknown1A0.begin(); it != Unknown1A0.end(); it++) {
		RefCountPtr<RenderObjClass> robj = *it;
		InterfaceMesh->Peek_Object()->Remove_Sub_Object(robj.Peek());
		robj->Set_Animation();
		robj->Remove();
	}
	Unknown1A0.clear();
}

// 0x63BDB0
// @0x0063BDB0
void UIStarbaseClass::Unknown_63BDB0(const Matrix3D &transform)
{
	Matrix3D tm;
	if (!UnknownC0.D->Avatar) {
		return;
	}
	UnknownC0.D->Avatar->Remove();
	((UIStarbaseSceneClass *)UnknownB0->Unknown70.Peek())->Add_Render_Object(UnknownC0.D->Avatar.Peek());
	((UIStarbaseSceneClass *)UnknownB0->Unknown70.Peek())->Unknown_7621A0(UnknownC0.D->Avatar.Peek());
	UnknownB0->Unknown_69DFC0(UnknownC0.D->Avatar, transform, false);
	UnknownB0->Unknown_69EB20(UnknownC0.D->Avatar->Unknown_787A90(), 1, 1);
	UnknownC0.D->Avatar->Unknown_77E3D0(1);
	UnknownC0.D->Avatar->Unknown230 = false;
	UnknownC0.D->Avatar->Unknown36C = true;
	UnknownC0.D->Avatar->Unknown_785F20(true);
	UnknownC0.D->Avatar->BodyObj->Set_LOD_Bias(100.0f);
	UnknownC0.D->Avatar->Unknown_786E60();
	Unknown_640F50(UnknownC0.D->Avatar.Peek());
	if (UnknownC0.D->Avatar != NULL) {
		UnknownC0.D->Avatar->Unknown_786E60();
	}
	Unknown214 = false;
	Unknown215 = false;
	Unknown216 = false;
	Unknown217 = false;
	Unknown218 = false;
	Unknown219 = false;
	Unknown21A = false;
	Unknown1B3 = false;
	if (Unknown_42EC60(2, 0)) {
		Unknown21A = true;
	}
	UnknownB0->Unknown_6A2430(&Unknown214, &Unknown215, &Unknown216, &Unknown217, &Unknown1B3);
	if (Unknown1B3 && UnknownC0.D != NULL) {
		UnknownC0.D->Avatar->Unknown_786B60(false);
	}
	tm = UnknownC0.D->Avatar->Unknown330;
	Unknown1EC = tm.Get_Translation();
	UnknownB0->Unknown_69CBE0();
	UnknownC0.E = 0x40;
	Unknown_63E2B0();
	Unknown1B4 = 0.0f;
}

// 0x63C080
// @0x0063C080
void UIStarbaseClass::Unknown_63C080(const char *name, bool a, bool b)
{
	Matrix3D tm;
	if (!UnknownC0.D->Avatar) {
		return;
	}
	UnknownC0.D->Avatar->Remove();
	UnknownC0.D->Avatar = NULL;
	unsigned int id = ((LineDisplayClientClass *)Client)->AvatarID;
	UnknownC0.D->Avatar = Create_NEW((CharacterViewAvatarClass *)((LineDisplayClientClass *)Client)->As_Space_Client()->Unknown_74C7B0(id));
	((UIStarbaseSceneClass *)UnknownB0->Unknown70.Peek())->Add_Render_Object(UnknownC0.D->Avatar.Peek());
	((UIStarbaseSceneClass *)UnknownB0->Unknown70.Peek())->Unknown_7621A0(UnknownC0.D->Avatar.Peek());
	UnknownB0->Unknown_69DC80(UnknownC0.D->Avatar, name, b);
	if (a) {
		UnknownB0->Unknown_69E510(UnknownC0.D->Avatar->Unknown_787A90());
	} else {
		UnknownB0->Unknown_69EB20(UnknownC0.D->Avatar->Unknown_787A90(), 1, 1);
	}
	UnknownC0.D->Avatar->Unknown_77E3D0(1);
	UnknownC0.D->Avatar->Unknown230 = false;
	UnknownC0.D->Avatar->Unknown36C = true;
	UnknownC0.D->Avatar->Unknown_785F20(true);
	UnknownC0.D->Avatar->BodyObj->Set_LOD_Bias(100.0f);
	UnknownC0.D->Avatar->Unknown_786E60();
	Unknown_640F50(UnknownC0.D->Avatar.Peek());
	if (UnknownC0.D->Avatar != NULL) {
		UnknownC0.D->Avatar->Unknown_786E60();
	}
	Unknown214 = false;
	Unknown215 = false;
	Unknown216 = false;
	Unknown217 = false;
	Unknown218 = false;
	Unknown219 = false;
	Unknown21A = false;
	Unknown1B3 = false;
	if (Unknown_42EC60(2, 0)) {
		Unknown21A = true;
	}
	UnknownB0->Unknown_6A2430(&Unknown214, &Unknown215, &Unknown216, &Unknown217, &Unknown1B3);
	if (Unknown1B3 && UnknownC0.D != NULL) {
		UnknownC0.D->Avatar->Unknown_786B60(false);
	}
	tm = UnknownC0.D->Avatar->Unknown330;
	Unknown1EC = tm.Get_Translation();
	if (b) {
		if (!UnknownB0->Unknown_69CBE0()) {
			UnknownC0.E = 0;
			Unknown_63E2B0();
		}
	} else {
		UnknownB0->Unknown_69CBE0();
		UnknownC0.E = 0x40;
		Unknown_63E2B0();
	}
	Unknown1B4 = 0.0f;
}

// 0x63D2F0
// @0x0063D2F0
void UIStarbaseClass::Unknown_63D2F0(int index)
{
	if (Unknown13C[index]->Avatar != NULL) {
		Matrix3D tm1 = Unknown13C[index]->Avatar->Get_Transform();
		Matrix3D tm2 = UnknownC0.D->Avatar->Get_Transform();
		Vector3 diff = tm2.Get_Translation() - tm1.Get_Translation();
		diff.Normalize();
		Vector3 z = tm2.Get_Z_Vector();
		Vector3 cross = Vector3::Cross_Product(z, diff);
		tm1.Set(diff, cross, z, tm1.Get_Translation());
		Unknown13C[index]->Avatar->Unknown_788700(tm1);
		tm2.Set(-diff, -cross, z, tm2.Get_Translation());
		UnknownC0.D->Avatar->Unknown_788700(tm2);
		UnknownB0->Unknown_69EB20(UnknownC0.D->Avatar->Unknown_787450(Unknown13C[index]->Avatar.Peek()), 1, 1);
		Unknown1FC = true;
	}
	if (Unknown13C[index]->Unknown08 != NULL) {
		Unknown13C[index]->Unknown08->Unknown_78A080();
		Unknown13C[index]->Unknown08->Unknown_78B570(false);
		Matrix3D tm1 = Unknown13C[index]->Unknown08->Get_Transform();
		Matrix3D tm2 = UnknownC0.D->Avatar->Get_Transform();
		Vector3 diff = tm2.Get_Translation() - tm1.Get_Translation();
		diff.Normalize();
		Vector3 z = tm2.Get_Z_Vector();
		Vector3 cross = Vector3::Cross_Product(z, diff);
		tm1.Set(diff, cross, z, tm1.Get_Translation());
		Unknown13C[index]->Unknown08->Unknown_788700(tm1);
		tm2.Set(-diff, -cross, z, tm2.Get_Translation());
		UnknownC0.D->Avatar->Unknown_788700(tm2);
		UnknownB0->Unknown_69EB20(UnknownC0.D->Avatar->Unknown_787450(Unknown13C[index]->Unknown08.Peek()), 1, 1);
		Unknown1FC = true;
	}
}

#pragma auto_inline(on)

// 0x640810
void UIStarbaseClass::Unknown_640810()
{
	int i = 0;
	int a = Unknown_Info()->Unknown18;
	int room = Room;
	int c = Unknown_Kind();
	Unknown_640CC0();
	while (true) {
		char name[64];  // NAME 1: neutral
		_snprintf(name, sizeof(name), "D_LIGHT%02d", i);
		if (!InterfaceMesh->Peek_Object()->Get_Bone_Index(name)) {
			break;
		}
		Matrix3D tm = InterfaceMesh->Peek_Object()->Get_Bone_Transform(name);
		LightClass *light = Unknown_BE82D0->Unknown_764570(a, room, c, i);
		light->Set_Transform(tm);
		((SceneClass *)UnknownB0->Unknown70.Peek())->Add_Render_Object(light);
		Lights.push_back(light);
		i++;
	}
	if (i) {
		Vector3 color = Unknown_BE82D0->Unknown_764CF0(a, room, c);
		((UIStarbaseSceneClass *)UnknownB0->Unknown70.Peek())->Unknown_762940(color);
		float fill = Unknown_BE82D0->Unknown_764EA0(a, room, c);
		((UIStarbaseSceneClass *)UnknownB0->Unknown70.Peek())->Unknown_762920(fill);
	} else {
		Matrix3D tm(true);
		tm.Set_Translation(Vector3(100.0f, 50.0f, 10.0f));
		LightClass *light = new LightClass;
		light->Set_Intensity(1.0f);
		light->Set_Force_Visible(true);
		light->Set_Far_Attenuation_Range(0.0, 500.0);
		light->Set_Ambient(Vector3(0.0f, 0.0f, 0.0f));
		light->Set_Specular(Vector3(0.0f, 0.0f, 0.0f));
		light->Set_Diffuse(Vector3(0.8f, 0.6f, 0.4f));
		light->Set_Transform(tm);
		((SceneClass *)UnknownB0->Unknown70.Peek())->Add_Render_Object(light);
		light->Add_Ref();
		Lights.push_back(light);
		((UIStarbaseSceneClass *)UnknownB0->Unknown70.Peek())->Unknown_762920(0.7f);
	}
}

// Inlined into 0x640810 above, so before the pragma
// 0x640CC0
void UIStarbaseClass::Unknown_640CC0()
{
	for (std::vector<LightClass *>::iterator it = Lights.begin(); it != Lights.end(); it++) {
		LightClass *light = *it;
		light->Remove();
		light->Release_Ref();
	}
	Lights.clear();
}

// Out of line, as the client: its lights panel below calls 0x640D80-0x640F30 (auto_inline off, as co.cpp's 0x6F3ED0)
#pragma auto_inline(off)

// 0x640D80
void UIStarbaseClass::Unknown_640D80(int *a, int *b, int *c)
{
	*a = UnknownB0->Unknown1B4->Unknown18;
	*b = Room;
	if (UnknownB4->Unknown0B) {
		*c = 3;
	} else {
		*c = UnknownB4->Unknown0A ? 2 : 1;
	}
}

// 0x640DF0
std::vector<LightClass *> UIStarbaseClass::Unknown_640DF0()
{
	return(Lights);
}

// 0x640E90
void UIStarbaseClass::Unknown_640E90(const Vector3 &color)
{
	((UIStarbaseSceneClass *)UnknownB0->Unknown70.Peek())->Unknown_762940(color);
}

// 0x640EC0
void UIStarbaseClass::Unknown_640EC0(float fill)
{
	((UIStarbaseSceneClass *)UnknownB0->Unknown70.Peek())->Unknown_762920(fill);
}

// 0x640EF0
void UIStarbaseClass::Unknown_640EF0(int index)
{
}

// 0x640F10
void UIStarbaseClass::Unknown_640F10()
{
}

// 0x640F30
void UIStarbaseClass::Unknown_640F30(int index, float range)
{
}

// 0x638FF0
void UIStarbaseClass::Unknown_7C(UIViewClass *view, bool arg)
{
	InterfaceClass::Unknown_7C(view, arg);
	Unknown1F8->Unknown_7C(view, false);
}

// 0x639030
void UIStarbaseClass::Unknown_639030(int arg)
{
	if (UnknownC0.B >= 0) {
		UIStarbaseUnknownCCClass *entry = Unknown13C[UnknownC0.B];
		if (entry->Avatar != NULL) {
			entry->Avatar->Unknown_784A50((const char *)arg, true, false);
			return;
		}
		if (entry->Unknown08 != NULL) {
			entry->Unknown08->Unknown_784A50((const char *)arg, true, false);
		}
	}
}

// 0x63B5A0
void UIStarbaseClass::Unknown_63B5A0()
{
	SClient *client = ((LineDisplayClientClass *)Client)->As_Space_Client();
	bool busy = false;
	ChatPanelInterface *panel = client->ChatPanel.Peek();
	if (panel) {
		if (Unknown_7CA2D0()->Unknown34 || panel->UnknownEC) {
			busy = true;
		}
	}
	if (client->Mouse->Unknown_515320() != 2 && !busy) {
		client->Mouse->Unknown_515120(2);
	}
	UnknownB0->Unknown_6A0E50();
}

// 0x63E8F0
bool UIStarbaseClass::Unknown_63E8F0(int id)
{
	for (std::list<UIStarbaseUnknownECStruct *>::iterator it = UnknownEC.begin(); it != UnknownEC.end(); ++it) {
		if (id == (*it)->ID) {
			return(true);
		}
	}
	return(false);
}

// 0x640110
void UIStarbaseClass::Unknown_640110(int state)
{
	if (!UnknownB0->Unknown_69CBE0() || state != 0) {
		UnknownC0.E = state;
		Unknown_63E2B0();
	}
}

// 0x641AB0
void UIStarbaseClass::Unknown_641AB0()
{
	if (Unknown190) {
		Client->Unknown_44(Unknown190->Get_Unknown90());
		delete Unknown190;
		Unknown190 = NULL;
	}
}

// 0x641B00
void UIStarbaseClass::Unknown_641B00()
{
	if (UnknownC0.D->Avatar != NULL) {
		UnknownC0.D->Avatar->Unknown_786E60();
	}
	Unknown214 = false;
	Unknown215 = false;
	Unknown216 = false;
	Unknown217 = false;
	Unknown218 = false;
	Unknown219 = false;
	Unknown21A = false;
	Unknown1B3 = false;
}

// 0x641B70
void UIStarbaseClass::Unknown_641B70()
{
	if (Unknown_42EC60(2, 0)) {
		Unknown21A = true;
	}
	UnknownB0->Unknown_6A2430(&Unknown214, &Unknown215, &Unknown216, &Unknown217, &Unknown1B3);
	if (Unknown1B3 && UnknownC0.D != NULL) {
		UnknownC0.D->Avatar->Unknown_786B60(false);
	}
}

#pragma auto_inline(on)

// The colour pad's gadget (0x80 bytes, constructor 0x671300), as ui_avatar_cust.cpp's UIAvatarCustGadget671300Class
class Unknown_671300Class : public GadgetClass	// NAME 1: neutral (D10)
{
public:
	Unknown_671300Class(RenderObjClass *area, int a, int b, int c, int d);	// 0x671410 (0x646360: the palette, 0, 0, 0, 0)  // NAME 1 [area, a, b, c, d]: neutral
	virtual bool Unknown_30(float x, float y);			// +0x30 (GadgetClass's pure slot: the pad's own)  // NAME 1 [x, y]: GadgetClass's
	void Unknown_671630();								// 0x671630 (the buttons' flags off)
	char Unknown30[0x64 - 0x30];						// +0x30
	bool Unknown64;										// +0x64 (the left button held)
	bool Unknown65;										// +0x65 (the right button held)
	char Unknown66[0x80 - 0x66];						// +0x66
};

// The pad and its pull (the panel's +0xD8), as ui_avatar_cust.cpp's UIAvatarCustGadgetHolderStruct
struct UIAvatrPullClass	// NAME 1: neutral (D10)
{
	UIAvatrPullClass();				// 0x585920 (0x10 bytes)
	void Unknown_5859A0();								// 0x5859A0 (once the gadget is in)
	GadgetClass *Unknown00;								// +0x00 (the pad: slots +0x08 / +0x64)
	RenderObjClass *Unknown04;							// +0x04 (the pull, a bone of the panel's model)
	bool Unknown08;										// +0x08 (dragged: 0x647B30)
	char Unknown09[0x10 - 0x09];						// +0x09
};

// The starbase lights panel (0x444 bytes; constructor 0x646170 over ui_chat_panel_base.cpp's 0x56D280, table
// 0xAF8610; no caller of the constructor in the client: a debug panel). Its texts are 'near_light02a',
// 'far_light02a', 'Int_light02a', 'Fill_light02a' and 'GO_light02a' in the 'fa_light02a.ini' model (0x646360).
// Declared as far as this file's functions use it (interim placement in this file, D137).
class UIStarbaseLightsClass : public ChatPanelUIBase	// NAME 1: neutral, after its model's 'light02a' names
{
public:
	UIStarbaseLightsClass(SClient *client);				// 0x646170  // NAME 1 [client]: the base's
	virtual ~UIStarbaseLightsClass();					// 0x646290 (deleting 0x646260)
	virtual void Unknown_54();							// +0x54 0x646360
	virtual void Unknown_58();							// +0x58 0x6466F0 (activated: the starbase, its lights, light 0)
	virtual void Unknown_5C();							// +0x5C 0x6467B0
	virtual void Unknown_60();							// +0x60 0x6467E0 (every frame: the sliders, the pad)
	virtual bool Unknown_68(int key);					// +0x68 0x646810 (the panel's keys 1-6)  // NAME 1 [key]: InterfaceClass's
	void Unknown_646940();								// 0x646940
	void Unknown_646A00();								// 0x646A00 (the lights read)
	void Unknown_646E00();								// 0x646E00 (key 2)
	void Unknown_648190();								// 0x648190 (no caller: the pad points from the lights manager)
	void Unknown_646F70(bool force);					// 0x646F70 (the sliders into the current light; its texts)  // NAME 1 [force]: neutral
	void Unknown_6474C0(int index);						// 0x6474C0 (a light chosen; mode 1: the ambient light)  // NAME 1 [index]: neutral
	void Unknown_647B30();								// 0x647B30 (the colour pad dragged)
	void Unknown_647F30(Vector3 point);					// 0x647F30 (the pull placed at a pad point, the colour read)  // NAME 1 [point]: neutral

	// The base's +0x84 model as the font manager takes it (ChatPanelUIBase declares it a plain pointer)
	const RefCountPtr<RenderObjClass> &Model() { return(*(const RefCountPtr<RenderObjClass> *)&Unknown84); }	// NAME 1 [Model]: neutral

	GadgetClass *UnknownA0[6];							// +0xA0 (the buttons exit, save, light00-03: 0x646360; the constructor clears them with one memset)
	GadgetSlider *Slider[4];							// +0xB8 (near, far, intensity, fill: 0x646F70's texts)  // NAME 1: neutral
	float SliderValue[4];								// +0xC8 (the values last seen: 0x646F70 compares them)  // NAME 1: neutral
	UIAvatrPullClass *UnknownD8;		// +0xD8
	UIStarbaseClass *Starbase;							// +0xDC (0x640E90 / 0x640EC0 / 0x640EF0 / 0x640F30, ui_starbase.cpp's)  // NAME 1: neutral
	int UnknownE0;										// +0xE0 (0: a light; else the ambient light)
	int UnknownE4;										// +0xE4
	int UnknownE8;										// +0xE8
	int UnknownEC;										// +0xEC (1 from the constructor)
	int Current;										// +0xF0 (the light chosen; -1 from the constructor: 'Light %02d')  // NAME 1: neutral
	int UnknownF4;										// +0xF4
	float Intensity[20];								// +0xF8 ('Int %0.2f'; LightClass::Set_Intensity)  // NAME 1: the LightClass setter's
	float Fill;											// +0x148 (0.5 from the constructor; 'Fill %0.2f')  // NAME 1: its text's
	Vector3 Color[20];									// +0x14C (from the pad: Set_Diffuse)  // NAME 1: neutral
	Vector3 Position[20];								// +0x23C (the pad points)  // NAME 1: neutral
	Vector3 Atten[20];									// +0x32C (X near, Y far: Set_Far_Attenuation_Range)  // NAME 1: neutral
	Vector3 AmbientColor;								// +0x41C (to 0x640E90)  // NAME 1: neutral
	Vector3 AmbientPosition;							// +0x428  // NAME 1: neutral
	std::vector<LightClass *> Lights;					// +0x434 (the allocator byte from the constructor's argument slot)  // NAME 1: neutral
};

// 0x646170
UIStarbaseLightsClass::UIStarbaseLightsClass(SClient *client) :
	ChatPanelUIBase(client),
	UnknownD8(NULL),
	Starbase(NULL),
	UnknownE0(0),
	UnknownE4(0),
	UnknownE8(0),
	UnknownEC(1),
	Current(-1),
	UnknownF4(0),
	Fill(0.5f)
{
	memset(UnknownA0, 0, sizeof(UnknownA0));
	memset(Slider, 0, sizeof(Slider));
	memset(SliderValue, 0, sizeof(SliderValue));
}

// 0x646290
UIStarbaseLightsClass::~UIStarbaseLightsClass()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
		Starbase->Unknown_640F10();
	}
}

// The buttons 0x646360 makes: their models' names and their texts (0xB8B864 / 0xB8B880)
static const char *Unknown_B8B864[6] = { "exit", "save", "light00", "light01", "light02", "light03" };	// NAME 1: neutral (D10)
static const char *Unknown_B8B880[6] = { NULL, "save_light02a", "previous_light02a", "next_light02a", "diffuse_light02a", "ambient_light02a" };	// NAME 1: neutral (D10)

// kernel.h's input event, as ui_class.cpp's UIClassKeyEventStruct (0x646360's Escape)
struct UIStarbaseLightsKeyEventStruct	// NAME 1: neutral (D10)
{
	UIStarbaseLightsKeyEventStruct(short key) : Key(key), Unknown02(0), Unknown04(0), Unknown08(0), Unknown0C(0) {}	// NAME 1 [key]: neutral
	short Key;											// +0x00 (0x1B)  // NAME 1: neutral (D10)
	short Unknown02;									// +0x02
	float Unknown04;									// +0x04
	float Unknown08;									// +0x08
	int Unknown0C;										// +0x0C
};

// 0x646360
void UIStarbaseLightsClass::Unknown_54()
{
	if (Unknown_98("light02a", "fa_light02a.ini")) {
		Unknown_646940();
		int i;
		for (i = 0; i < 6; i++) {
			UnknownA0[i] = Unknown_56D510(i + 1, Unknown_B8B864[i], Unknown_B8B880[i]);
		}
		for (i = 0; i < 4; i++) {
			Slider[i] = Unknown_56D8D0(i + 8, 0);
			Slider[i]->Unknown_66E460(Model(), true, i);
			if (i == 2) {
				Slider[2]->Unknown_66FA90(3.0f, 0.01f, true);
				Slider[2]->Unknown_A4(0.5f, false);
			} else if (i == 3) {
				Slider[3]->Unknown_66FA90(3.0f, 0.01f, true);
				Slider[3]->Unknown_A4(0.75f, false);
			} else {
				Slider[i]->Unknown_66FA90(1000.0f, 0.1f, true);
				Slider[i]->Unknown_A4(500.0f, false);
			}
		}
		UnknownD8 = new UIAvatrPullClass;
		RefCountPtr<RenderObjClass> palette = Create_Get(Unknown84->Get_Sub_Object_By_Name("COLOR_PALETTE"));
		Unknown_496AE0(palette.Peek(), false);
		UnknownD8->Unknown00 = new Unknown_671300Class(palette.Peek(), 0, 0, 0, 0);
		UnknownD8->Unknown00->Unknown2C_0A = true;
		UnknownD8->Unknown04 = Unknown84->Get_Sub_Object_By_Name("PULL_COLOR");
		Unknown_496AE0(UnknownD8->Unknown04, true);
		UnknownD8->Unknown_5859A0();
		UnknownD8->Unknown00->Unknown_65FFA0(-1);
		Unknown64.Unknown_6870E0(UnknownD8->Unknown00, 0xD, View, true, false);
		Unknown_56D970(1, Vector3(0.0f, 0.0f, 0.0f));
		((ControlKeyListClass *)Unknown4C)->Add(*(const ControlKeyComboStruct *)&UIStarbaseLightsKeyEventStruct(0x1B), (void *)1);
	}
}

// 0x6466F0
void UIStarbaseLightsClass::Unknown_58()
{
	if (!Active) {
		ChatPanelUIBase::Unknown_58();
		UnknownE0 = 0;
		Starbase = ((SBaseViewClass *)((LineDisplayClientClass *)Client)->As_Space_Client()->Unknown135C)->Get_Starbase().Peek();
		Starbase->Unknown_640D80(&UnknownE4, &UnknownE8, &UnknownEC);
		Unknown_646A00();
		Unknown_6474C0(0);
	}
}

// 0x6467B0
void UIStarbaseLightsClass::Unknown_5C()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
		Starbase->Unknown_640F10();
	}
}

// 0x6467E0
void UIStarbaseLightsClass::Unknown_60()
{
	Unknown_646F70(false);
	Unknown_647B30();
}

// 0x646810
bool UIStarbaseLightsClass::Unknown_68(int key)
{
	((LineDisplayClientClass *)Client)->As_Space_Client();
	SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
	switch (key) {
		case 1:
			Unknown_5C();
			Unknown98 = 1;
			return(true);
		case 2:
			Unknown_646E00();
			return(true);
		case 4:
			Unknown_6474C0((Current + 1) % UnknownF4);
			return(true);
		case 3:
			{
				int index = Current - 1;
				if (index < 0) {
					index = UnknownF4 - 1;
				}
				Unknown_6474C0(index);
			}
			return(true);
		case 5:
			UnknownE0 = 0;
			Unknown_6474C0(Current);
			return(true);
		case 6:
			UnknownE0 = 1;
			Unknown_6474C0(Current);
			return(true);
	}
	return(false);
}

// 0x646940
void UIStarbaseLightsClass::Unknown_646940()
{
	for (int i = 0; i < 20; i++) {
		Intensity[i] = 1.0f;
		Color[i].X = 0.5f;
		Color[i].Y = 0.5f;
		Color[i].Z = 0.5f;
		Position[i].X = 0.5f;
		Position[i].Y = 0.5f;
		Position[i].Z = 0.5f;
		Atten[i].X = 0.0f;
		Atten[i].Y = 50.0f;
		Atten[i].Z = 0.0f;
	}
	AmbientColor.Set(0.0f, 0.0f, 0.0f);
	AmbientPosition.Set(0.0f, 0.0f, 0.0f);
	Fill = 0.75f;
}

// 0x646A00
void UIStarbaseLightsClass::Unknown_646A00()
{
	Unknown_BE82D0->Unknown_764530();
	Lights.size();		// dead read: spends the inline budget so the temporary's _Destroy is called out of line (0x644410) as the client's
	Lights = Starbase->Unknown_640DF0();
	UnknownF4 = Lights.size();
	int i;
	for (i = 0; i < UnknownF4; i++) {
		LightClass *light = Lights[i];
		light->Get_Ambient(&AmbientColor);
		light->Get_Diffuse(&Color[i]);
		Intensity[i] = light->Get_Intensity();
		light->Get_Far_Attenuation_Range(Atten[i].X, Atten[i].Y);
	}
	Fill = Unknown_BE82D0->Unknown_764EA0(UnknownE4, UnknownE8, UnknownEC);
	AmbientColor = Unknown_BE82D0->Unknown_764CF0(UnknownE4, UnknownE8, UnknownEC);
	for (i = 0; i < UnknownF4; i++) {
		Position[i] = Unknown_BE82D0->Unknown_764AB0(UnknownE4, UnknownE8, UnknownEC, i);
	}
	AmbientPosition = Unknown_BE82D0->Unknown_764AB0(UnknownE4, UnknownE8, UnknownEC, 1000);
}

// 0x646E00
void UIStarbaseLightsClass::Unknown_646E00()
{
	int i = 0;
	for (std::vector<LightClass *>::iterator it = Lights.begin(); it != Lights.end(); it++, i++) {
		Unknown_BE82D0->Unknown_764880(UnknownE4, UnknownE8, UnknownEC, i, *it);
	}
	Unknown_BE82D0->Unknown_764F60(UnknownE4, UnknownE8, UnknownEC, Fill);
	Unknown_BE82D0->Unknown_764E20(UnknownE4, UnknownE8, UnknownEC, AmbientColor);
	for (i = 0; i < UnknownF4; i++) {
		Unknown_BE82D0->Unknown_764C30(UnknownE4, UnknownE8, UnknownEC, i, Position[i]);
	}
	Unknown_BE82D0->Unknown_764C30(UnknownE4, UnknownE8, UnknownEC, 1000, AmbientPosition);
	Unknown_BE82D0->Unknown_764550();
}

// 0x646F70
void UIStarbaseLightsClass::Unknown_646F70(bool force)
{
	bool changed = false;
	int i;
	for (i = 0; i < 4; i++) {
		if (Slider[i]->Unknown_60()) {
			Slider[i]->Unknown_66FD80();
			if (Slider[i]->Value != SliderValue[i]) {
				changed = true;
			}
		}
	}
	if (changed || force) {
		Atten[Current].X = Slider[0]->Get_Value();
		Atten[Current].Y = Slider[1]->Get_Value();
		Intensity[Current] = Slider[2]->Get_Value();
		Fill = Slider[3]->Get_Value();
		if (Atten[Current].X > Atten[Current].Y) {
			Atten[Current].X = Atten[Current].Y;
			Slider[0]->Unknown_A4(Atten[Current].X, false);
		}
		LightClass *light = Lights[Current];
		light->Set_Far_Attenuation_Range(Atten[Current].X, Atten[Current].Y);
		light->Set_Intensity(WWMath::Max(0.01f, Intensity[Current]));
		Starbase->Unknown_640EC0(Fill);
		Starbase->Unknown_640F30(Current, Atten[Current].Y);
		for (i = 0; i < 4; i++) {
			if (Slider[i]->Unknown_60()) {
				SliderValue[i] = Slider[i]->Get_Value();
			}
		}
		char buffer[64];
		RefCountPtr<FontSys> text = Unknown_BE67C4->Unknown_6D2700("near_light02a", Model());
		_snprintf(buffer, sizeof(buffer), "Near %d", (int)Slider[0]->Get_Value());
		text->Change_Text(buffer, 0);
		text = Unknown_BE67C4->Unknown_6D2700("far_light02a", Model());
		_snprintf(buffer, sizeof(buffer), "Far %d", (int)Slider[1]->Get_Value());
		text->Change_Text(buffer, 0);
		text = Unknown_BE67C4->Unknown_6D2700("Int_light02a", Model());
		_snprintf(buffer, sizeof(buffer), "Int %0.2f", Slider[2]->Get_Value());
		text->Change_Text(buffer, 0);
		text = Unknown_BE67C4->Unknown_6D2700("Fill_light02a", Model());
		_snprintf(buffer, sizeof(buffer), "Fill %0.2f", Slider[3]->Get_Value());
		text->Change_Text(buffer, 0);
	}
}

// 0x6474C0
void UIStarbaseLightsClass::Unknown_6474C0(int index)
{
	Current = index;
	Starbase->Unknown_640EF0(index);
	if (UnknownE0 == 0) {
		Slider[0]->Unknown_8C(false);
		Slider[1]->Unknown_8C(false);
		Slider[2]->Unknown_8C(false);
		Slider[0]->Unknown_40(true);
		Slider[1]->Unknown_40(true);
		Slider[2]->Unknown_40(true);
		Slider[0]->Unknown_A4(Atten[Current].X, false);
		Slider[1]->Unknown_A4(Atten[Current].Y, false);
		Starbase->Unknown_640F30(Current, Atten[Current].Y);
		Slider[2]->Unknown_A4(Intensity[Current], false);
		Slider[3]->Unknown_A4(Fill, false);
		for (int i = 0; i < 4; i++) {
			SliderValue[i] = Slider[i]->Get_Value();
		}
		Unknown_647F30(Position[Current]);
		RefCountPtr<FontSys> text = Unknown_BE67C4->Unknown_6D2700("GO_light02a", Model());
		char buffer[64];
		_snprintf(buffer, sizeof(buffer), "Light %02d", Current);
		text->Change_Text(buffer, 0);
		Unknown_646F70(true);
	} else {
		Slider[0]->Unknown_8C(true);
		Slider[1]->Unknown_8C(true);
		Slider[2]->Unknown_8C(true);
		Slider[0]->Unknown_40(false);
		Slider[1]->Unknown_40(false);
		Slider[2]->Unknown_40(false);
		Unknown_647F30(AmbientPosition);
		RefCountPtr<FontSys> text = Unknown_BE67C4->Unknown_6D2700("GO_light02a", Model());
		char buffer[64];
		_snprintf(buffer, sizeof(buffer), "Ambient Light", Current);
		text->Change_Text(buffer, 0);
		text = Unknown_BE67C4->Unknown_6D2700("near_light02a", Model());
		text->Change_Text("", 0);
		text = Unknown_BE67C4->Unknown_6D2700("far_light02a", Model());
		text->Change_Text("", 0);
		text = Unknown_BE67C4->Unknown_6D2700("Int_light02a", Model());
		text->Change_Text("", 0);
		text = Unknown_BE67C4->Unknown_6D2700("Fill_light02a", Model());
		text->Change_Text("", 0);
	}
}

// 0x647B30
void UIStarbaseLightsClass::Unknown_647B30()
{
	if (((Unknown_671300Class *)UnknownD8->Unknown00)->Unknown64) {
		UnknownD8->Unknown08 = true;
	}
	if ((((Unknown_671300Class *)UnknownD8->Unknown00)->Unknown64 || ((Unknown_671300Class *)UnknownD8->Unknown00)->Unknown65) && UnknownD8->Unknown08) {
		AABoxClass box;
		UnknownD8->Unknown00->Unknown_64(box);
		float x;
		float y;
		Unknown_42ECF0(&x, &y);
		x = x * 640.0f - 320.0f;
		y = 240.0f - y * 480.0f;
		x = WWMath::Min(x, box.Center.X + box.Extent.X);
		x = WWMath::Max(x, box.Center.X - box.Extent.X);
		y = WWMath::Min(y, box.Center.Y + box.Extent.Y);
		y = WWMath::Max(y, box.Center.Y - box.Extent.Y);
		int bone = Unknown84->Get_Sub_Object_Bone_Index(UnknownD8->Unknown04);
		Unknown84->Capture_Bone(bone);
		Matrix3D tm = UnknownD8->Unknown04->Get_Transform();
		tm.Set_X_Translation(x - box.Center.X);
		tm.Set_Y_Translation(y - box.Center.Y);
		tm[2][3] = 0.0f;
		Unknown84->Control_Bone(bone, tm, false);
		Vector2 point;
		point.X = (x - (box.Center.X - box.Extent.X)) / (box.Extent.X + box.Extent.X);
		point.Y = (y - (box.Center.Y - box.Extent.Y)) / (box.Extent.Y + box.Extent.Y);
		if (UnknownE0 == 0) {
			GadgetUnknown08Struct *area = UnknownD8->Unknown00->Unknown_08();
			Unknown_496C00(area->Unknown30, point.X, 1.0f - point.Y, &Color[Current]);
			Position[Current].Set(point.X, point.Y, 0.0f);
			Lights[Current]->Set_Diffuse(Color[Current]);
		} else {
			GadgetUnknown08Struct *area = UnknownD8->Unknown00->Unknown_08();
			Unknown_496C00(area->Unknown30, point.X, 1.0f - point.Y, &AmbientColor);
			AmbientPosition.Set(point.X, point.Y, 0.0f);
			Starbase->Unknown_640E90(AmbientColor);
		}
	} else {
		UnknownD8->Unknown08 = false;
	}
	((Unknown_671300Class *)UnknownD8->Unknown00)->Unknown_671630();
}

// 0x647F30
void UIStarbaseLightsClass::Unknown_647F30(Vector3 point)
{
	AABoxClass box;
	UnknownD8->Unknown00->Unknown_64(box);
	float top = box.Center.Y - box.Extent.Y;
	float left = box.Center.X - box.Extent.X;
	float width = box.Extent.X * 2.0f;
	float height = box.Extent.Y * 2.0f;
	int bone = Unknown84->Get_Sub_Object_Bone_Index(UnknownD8->Unknown04);
	Unknown84->Capture_Bone(bone);
	Matrix3D tm = UnknownD8->Unknown04->Get_Transform();
	tm.Set_X_Translation((point.X * width) + left - box.Center.X);
	tm.Set_Y_Translation((point.Y * height) + top - box.Center.Y);
	tm[2][3] = 0.0f;
	Unknown84->Control_Bone(bone, tm, false);
	if (UnknownE0 == 0) {
		GadgetUnknown08Struct *area = UnknownD8->Unknown00->Unknown_08();
		Unknown_496C00(area->Unknown30, point.X, 1.0f - point.Y, &Color[Current]);
		Position[Current].Set(point.X, point.Y, 0.0f);
	} else {
		GadgetUnknown08Struct *area = UnknownD8->Unknown00->Unknown_08();
		Unknown_496C00(area->Unknown30, point.X, 1.0f - point.Y, &AmbientColor);
		AmbientPosition.Set(point.X, point.Y, 0.0f);
	}
}

// 0x648190
void UIStarbaseLightsClass::Unknown_648190()
{
	for (int i = 0; i < UnknownF4; i++) {
		Position[i] = Unknown_BE82D0->Unknown_764AB0(UnknownE4, UnknownE8, UnknownEC, i);
	}
	AmbientPosition = Unknown_BE82D0->Unknown_764AB0(UnknownE4, UnknownE8, UnknownEC, 1000);
}
































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































// NOT EXACT: 0x63A100, functional (the busy flag in memory and the Q key in bl in the client, swapped here; branch order otherwise as the client's; SB1)
// 0x63A100
void UIStarbaseClass::Unknown_63A100()
{
	SClient *sclient = ((LineDisplayClientClass *)Client)->As_Space_Client();
	FAIL_IF(!sclient) return;
	ChatPanelInterface *panel = sclient->ChatPanel.Peek();
	bool busy = false;
	if (panel) {
		if (Unknown_7CA2D0()->Unknown34 || panel->UnknownEC) {
			busy = true;
		}
	}
	if (!Unknown_42EC60(2, 0)) {
		Unknown21A = false;
	}
	if (!UnknownC0.D->Avatar || UnknownC0.D->Avatar->Unknown380 || Unknown1FC || Unknown1FD || Unknown1FE || Unknown1FF || Unknown200 || Unknown201
		|| UnknownC0.D->Avatar->Unknown318.size() || UnknownB0->Unknown_69CCD0() || Unknown1F8->Active || !sclient->Unknown_74B5E0()) {
		if (sclient->Mouse->Unknown_515320() != 2 && sclient->Mouse->Unknown_515320() < 0x16 && !busy) {
			sclient->Mouse->Unknown_515120(2);
		}
		if (!UnknownC0.D->Avatar->Unknown318.size()) {
			if (UnknownC0.D->Avatar != NULL) {
				UnknownC0.D->Avatar->Unknown_786E60();
			}
			Unknown214 = false;
			Unknown215 = false;
			Unknown216 = false;
			Unknown217 = false;
			Unknown218 = false;
			Unknown219 = false;
			Unknown21A = false;
			Unknown1B3 = false;
		}
		return;
	}
	if (sclient->ChatPanel->ChatInt->Unknown_570F20()) {
		Unknown214 = false;
		Unknown215 = false;
		Unknown216 = false;
		Unknown217 = false;
		Unknown218 = false;
		Unknown219 = false;
	}
	float x;
	float y;
	Unknown_42ECF0(&x, &y);
	bool key = Unknown_42EC60(0x11, 0);
	if (OptionClass::Instance->Get_Bool("Starbase_Default_Run")) {
		UnknownC0.D->Avatar->Unknown_784F60(true);
	} else {
		UnknownC0.D->Avatar->Unknown_784F60(false);
	}
	if (Unknown21A) {
		bool top = y < 0.5f;
		bool middle;
		if (x <= 0.4f || x >= 0.6f) {
			middle = false;
		} else {
			middle = true;
		}
		if (x <= 0.2f || x >= 0.8f || key) {
			if (!Unknown1B3) {
				UnknownC0.D->Avatar->Unknown_786C70();
			}
		} else {
			if (middle && sclient->Mouse->Unknown_515320() != 0x12 && !busy) {
				sclient->Mouse->Unknown_515120(0x12);
			}
			if (!(UnknownC0.D->Avatar->Unknown2F8 & 2)) {
				UnknownC0.D->Avatar->Unknown_786B60(true);
			} else {
				UnknownC0.D->Avatar->Unknown_786B60(false);
			}
		}
		if (y < 0.5f) {
			UnknownC0.D->Avatar->Unknown_786D40(0.5f - y);
		} else if (y > 0.5f) {
			UnknownC0.D->Avatar->Unknown_786D70(y - 0.5f);
		}
		if (x < 0.5f) {
			if (!key) {
				UnknownC0.D->Avatar->Unknown_786E40();
				UnknownC0.D->Avatar->Unknown_786CF0(0.5f - x);
			} else {
				UnknownC0.D->Avatar->Unknown_786D20();
				UnknownC0.D->Avatar->Unknown_786DC0(0.5f - x);
			}
			if (middle) {
				return;
			}
			if (top) {
				if (sclient->Mouse->Unknown_515320() != 0x11 && !busy) {
					sclient->Mouse->Unknown_515120(0x11);
				}
				return;
			}
			if (sclient->Mouse->Unknown_515320() != 0x0F && !busy) {
				sclient->Mouse->Unknown_515120(0x0F);
			}
			return;
		}
		if (x > 0.5f) {
			if (!key) {
				UnknownC0.D->Avatar->Unknown_786E40();
				UnknownC0.D->Avatar->Unknown_786CC0(x - 0.5f);
			} else {
				UnknownC0.D->Avatar->Unknown_786D20();
				UnknownC0.D->Avatar->Unknown_786E00(x - 0.5f);
			}
			if (middle) {
				return;
			}
			if (top) {
				if (sclient->Mouse->Unknown_515320() != 0x13 && !busy) {
					sclient->Mouse->Unknown_515120(0x13);
				}
				return;
			}
			if (sclient->Mouse->Unknown_515320() != 0x15 && !busy) {
				sclient->Mouse->Unknown_515120(0x15);
			}
		}
		return;
	}
	if (sclient->Mouse->Unknown_515320() != 2 && ((UnknownB8->Unknown2C_02 && UnknownB8->Unknown2C_04) || sclient->Mouse->Unknown_515320() < 0x16) && !busy) {
		sclient->Mouse->Unknown_515120(2);
	}
	if (Unknown214) {
		if (!(UnknownC0.D->Avatar->Unknown2F8 & 2)) {
			UnknownC0.D->Avatar->Unknown_786B60(true);
		} else {
			UnknownC0.D->Avatar->Unknown_786B60(false);
		}
	} else if (Unknown215) {
		if (!(UnknownC0.D->Avatar->Unknown2F8 & 1)) {
			UnknownC0.D->Avatar->Unknown_786BF0(true);
		} else {
			UnknownC0.D->Avatar->Unknown_786BF0(false);
		}
	} else if (!Unknown1B3) {
		UnknownC0.D->Avatar->Unknown_786C70();
	}
	if (Unknown217) {
		if (!key) {
			UnknownC0.D->Avatar->Unknown_786E40();
			UnknownC0.D->Avatar->Unknown_786CC0(1.0f);
		} else {
			UnknownC0.D->Avatar->Unknown_786D20();
			UnknownC0.D->Avatar->Unknown_786E00(1.0f);
		}
	} else if (Unknown216) {
		if (!key) {
			UnknownC0.D->Avatar->Unknown_786E40();
			UnknownC0.D->Avatar->Unknown_786CF0(1.0f);
		} else {
			UnknownC0.D->Avatar->Unknown_786D20();
			UnknownC0.D->Avatar->Unknown_786DC0(1.0f);
		}
	} else if (x > 0.99f && key) {
		UnknownC0.D->Avatar->Unknown_786D20();
		UnknownC0.D->Avatar->Unknown_786E00(1.0f);
	} else if (x < 0.01f && key) {
		UnknownC0.D->Avatar->Unknown_786D20();
		UnknownC0.D->Avatar->Unknown_786DC0(1.0f);
	} else {
		UnknownC0.D->Avatar->Unknown_786E40();
		UnknownC0.D->Avatar->Unknown_786D20();
	}
	if (Unknown218) {
		UnknownC0.D->Avatar->Unknown_786D40(1.0f);
	} else if (Unknown219) {
		UnknownC0.D->Avatar->Unknown_786D70(1.0f);
	} else if (y > 0.99f && key) {
		UnknownC0.D->Avatar->Unknown_786D70(1.0f);
	} else if (y < 0.01f && key) {
		UnknownC0.D->Avatar->Unknown_786D40(1.0f);
	} else {
		UnknownC0.D->Avatar->Unknown_786DA0();
	}
}









































































































































































































































































































































































































































































































































































































































































// 0x63D060
void UIStarbaseClass::Unknown_63D060(unsigned int id)
{
	Unknown5BB320Class::Unknown_5C();
	UIStarbaseUnknownD4TableClass::iterator it;
	it = UnknownD4.find(id);
	if (it.Cur) {
		UIStarbaseUnknownCCClass *savatar = it.Cur->Value;
		if (savatar) {
			Message_Info("Sbase", "", __FILE__, __LINE__, "(%s) Removing avatar %s from room %d", UnknownC0.D->Avatar->Desc.First_Name, savatar->Avatar->Desc.First_Name, Room);
			savatar->Avatar->Unknown_788EF0();
			if (savatar->Unknown14) {
				Unknown64.Unknown_687150(savatar->Unknown14);
			}
			delete savatar;
		}
		UnknownD4.erase(id);
	}
	Unknown5BB320Class::Unknown_58();
}






















































































































































































































// 0x63E4B0
void UIStarbaseClass::Unknown_63E4B0(int new_room, int old_room)
{
	int room = -1;
	int old = -1;
	if (new_room >= 0) {
		room = new_room;
	}
	if (old_room >= 0) {
		old = old_room;
	}
	SClient *client = ((LineDisplayClientClass *)Client)->As_Space_Client();
	client->Send_Packet(&Starbase_Room_ChangePacketClass(((LineDisplayClientClass *)Client)->Get_Avatar_ID(), room, old));
	Message_Info("Sbase", "", __FILE__, __LINE__, "(%s) Changing room from %d to %d", UnknownC0.D->Avatar->Desc.First_Name, old, room);
}





























































































































































































































































































































































































































































































































































































































































































































// The file's later functions, at their asserts' lines (FAIL_IF '!savatar' l.3995)

// 0x63CF30
void UIStarbaseClass::Unknown_63CF30(int avatar_id, int new_room)
{
	for (std::list<UIStarbaseUnknownECStruct *>::iterator it = UnknownEC.begin(); it != UnknownEC.end(); ++it) {
		if (avatar_id == (*it)->ID) {
			return;
		}
	}
	UIStarbaseUnknownECStruct *entry = new UIStarbaseUnknownECStruct(avatar_id);
	UnknownEC.push_back(entry);
	if (Room == 0 && new_room == -1) {
		UIStarbaseUnknownD4TableClass::iterator found = UnknownD4.find(avatar_id);
		if (found.Cur) {
			UIStarbaseUnknownCCClass *savatar = found.Cur->Value;
			if (savatar) {
				savatar->Avatar->Unknown_788EF0();
				savatar->Avatar->Unknown_7896B0(3);
			}
		}
	}
}

// 0x63D1F0
void UIStarbaseClass::Unknown_63D1F0(int id, Matrix3D transform, int flags)
{
	UIStarbaseUnknownD4TableClass::iterator it = UnknownD4.find(id);
	if (it.Cur) {
		UIStarbaseUnknownCCClass *savatar = it.Cur->Value;
		if (savatar) {
			savatar->Avatar->Unknown_788730(transform, true);
			savatar->Unknown18 = flags;
			if (flags & 1) {
				savatar->Avatar->Unknown_784F60(true);
			} else {
				savatar->Avatar->Unknown_784F60(false);
			}
		}
	}
}

// 0x63E5C0
void UIStarbaseClass::Unknown_63E5C0(int avatar_id, float orient, Vector3 *position, int action)
{
	for (std::list<UIStarbaseUnknownECStruct *>::iterator it = UnknownEC.begin(); it != UnknownEC.end(); ++it) {
		if (avatar_id == (*it)->ID) {
			return;
		}
	}
	SClient *client = ((LineDisplayClientClass *)Client)->As_Space_Client();
	if (client && client->AvatarID != avatar_id) {
		Matrix3D tm(true);
		tm.Rotate_Z(orient);
		tm.Set_Translation(*position);
		UIStarbaseUnknownD4TableClass::iterator found = UnknownD4.find(avatar_id);
		UIStarbaseUnknownCCClass *savatar = found.Cur ? found.Cur->Value : NULL;
		if (!savatar) {
			if (client->Unknown_74C960(avatar_id)) {
				Unknown_63CA50(avatar_id, tm, action);
			}
		} else {
			Unknown_63D1F0(avatar_id, tm, action);
		}
	}
}

// 0x63BB10
void UIStarbaseClass::Unknown_63BB10(GadgetClass *gadget)
{
	if (gadget && ((RObjGadgetClass *)gadget)->RObj && UnknownC0.D->Avatar) {
		Vector3 pos = ((RObjGadgetClass *)gadget)->RObj->Get_Transform().Get_Translation();
		Vector3 diff = UnknownC0.D->Avatar->Get_Transform().Get_Translation() - pos;
		int distance = (int)diff.Length();
		if (distance > 999) {
			distance = 999;
		} else if (distance < 1) {
			distance = 1;
		}
		gadget->Unknown_65FFA0(distance);
	}
}

// NOT EXACT: 0x63E770, functional (the client compares the found iterator with end() through an out-of-line 8-byte copy 0x6439B0, a const_iterator conversion the stand-in table lacks; SB1)
// 0x63E770
void UIStarbaseClass::Unknown_63E770()
{
	float time = WW3D::Get_Frame_Time() * 0.001f;
	std::list<UIStarbaseUnknownECStruct *>::iterator it = UnknownEC.begin();
	while (it != UnknownEC.end()) {
		UIStarbaseUnknownECStruct *entry = *it;
		entry->Time += time;
		UIStarbaseUnknownD4TableClass::iterator found = UnknownD4.find(entry->ID);
		if (found != UnknownD4.end() && found.Cur->Value) {
			CharacterViewAvatarClass *avatar = found.Cur->Value->Avatar.Peek();
			if (avatar->Unknown318.size() && entry->Time <= 10.0f) {
				++it;
				continue;
			}
			if (avatar->Unknown380 && entry->Time <= 10.0f) {
				++it;
				continue;
			}
		}
		Unknown_63D060(entry->ID);
		delete entry;
		it = UnknownEC.erase(it);
	}
}

// 0x63FF80
void UIStarbaseClass::Unknown_63FF80()
{
	float time = WW3D::Get_Frame_Time() * 0.001f;
	if (Unknown1B8 > 0.0f && (Unknown1B8 -= time) < 0.0f) {
		Unknown1B8 = 0.5f;
		Unknown_641FA0();
	}
	if ((UnknownC0.E & 0x40) && UnknownC0.D->Avatar->Unknown380 == 0) {
		if (!UnknownB0->Unknown_69CBE0()) {
			UnknownC0.E = 0;
			Unknown_63E2B0();
		}
	}
	if (!Unknown1FD) {
		if (!UnknownB0->Unknown_69CCD0() && !Unknown1F8->Active) {
			if (UnknownC0.D->Avatar->Unknown2F8 == 0) {
				Unknown1B4 = 0.0f;
				return;
			}
			if (Unknown1B4 >= 0.0f && (Unknown1B4 -= time) < 0.0f) {
				Unknown1B4 = 0.5f;
				Unknown_63E2B0();
			}
		}
	}
}

// NOT EXACT: 0x63E2B0, functional (the flag test's own load of the avatar, the position's y / z through the matrix copy, the id's load order; SB1)
// 0x63E2B0
void UIStarbaseClass::Unknown_63E2B0()
{
	if (!Unknown21C) {
		int flags = UnknownC0.E;
		if (UnknownC0.D->Avatar->Unknown2F4) {
			flags |= 1;
		}
		Matrix3D tm = UnknownC0.D->Avatar->Unknown330;
		float orient = tm.Get_Z_Rotation();
		Vector3 position = UnknownC0.D->Avatar->Unknown330.Get_Translation();
		SClient *client = ((LineDisplayClientClass *)Client)->As_Space_Client();
		int room = -1;
		if (Room >= 0) {
			room = Room;
		}
		client->Send_Packet(&Starbase_Avatar_ChangePacketClass(((LineDisplayClientClass *)Client)->Get_Avatar_ID(), room, orient, &position, flags));
		Unknown_641FA0();
	}
}

// 0x641FA0's distance from the avatar to a gadget's model, 1..999 (five copies: a macro, as no inline budget is spent on it)
#define UNKNOWN_DISTANCE(gadget) \
	if ((gadget) && ((RObjGadgetClass *)(gadget))->RObj && UnknownC0.D->Avatar) { \
		Vector3 pos = ((RObjGadgetClass *)(gadget))->RObj->Get_Transform().Get_Translation(); \
		Vector3 apos = UnknownC0.D->Avatar->Get_Transform().Get_Translation(); \
		int distance = (int)(apos - pos).Length(); \
		if (distance > 999) { \
			distance = 999; \
		} else if (distance < 1) { \
			distance = 1; \
		} \
		(gadget)->Unknown_65FFA0(distance); \
	}

// The same through a difference vector (the hash table's copy: its own x87 order)
#define UNKNOWN_DISTANCE_2(gadget) \
	if ((gadget) && ((RObjGadgetClass *)(gadget))->RObj && UnknownC0.D->Avatar) { \
		Vector3 pos = ((RObjGadgetClass *)(gadget))->RObj->Get_Transform().Get_Translation(); \
		Vector3 diff = UnknownC0.D->Avatar->Get_Transform().Get_Translation() - pos; \
		int distance = (int)WWMath::Sqrt(diff.X * diff.X + diff.Y * diff.Y + diff.Z * diff.Z); \
		if (distance > 999) { \
			distance = 999; \
		} else if (distance < 1) { \
			distance = 1; \
		} \
		(gadget)->Unknown_65FFA0(distance); \
	}

// NOT EXACT: 0x641FA0, register only (this in edi, the gadget in ebx: the client's are swapped; SB1)
// 0x641FA0
void UIStarbaseClass::Unknown_641FA0()
{
	int i;
	if (UnknownC0.D->Avatar) {
		Unknown5BB320Class::Unknown_5C();
		for (i = 0; i < 4; i++) {
			GadgetClass *gadget = Unknown118[i];
			UNKNOWN_DISTANCE(gadget)
		}
		if (Room == 0) {
			GadgetClass *gadget = Unknown180;
			UNKNOWN_DISTANCE(gadget)
		}
		for (i = 0; i < 4; i++) {
			GadgetClass *gadget = UnknownF8[i];
			UNKNOWN_DISTANCE(gadget)
		}
		for (i = 0; i < 14; i++) {
			if (Unknown13C[i]) {
				GadgetClass *gadget = Unknown13C[i]->Unknown14;
				UNKNOWN_DISTANCE(gadget)
			}
		}
		UIStarbaseUnknownD4TableClass::iterator it;
		for (it = UnknownD4.begin(); it != UnknownD4.end(); it++) {
			UIStarbaseUnknownCCClass *savatar = it.Cur->Value;
			FAIL_IF(!savatar) continue;
			GadgetClass *gadget = savatar->Unknown14;
			UNKNOWN_DISTANCE_2(gadget)
		}
		Unknown5BB320Class::Unknown_58();
	}
}






// 0x6425D0
void UIStarbaseClass::Unknown_6425D0(const char *name, const Vector3 &color)
{
	UIStarbaseUnknownD4TableClass::iterator it;
	for (it = UnknownD4.begin(); it.Cur; ++it) {
		UIStarbaseUnknownCCClass *savatar = it.Cur->Value;
		FAIL_IF(!savatar) continue;
		if (stricmp(savatar->Avatar->Desc.First_Name, name) == 0) {
			savatar->Unknown20 = 2.0f;
			((FontSys *)savatar->Unknown0C.Peek())->Change_Text("\".....\"", 0);
			FontSys *tag = (FontSys *)savatar->Unknown0C.Peek();
			tag->Unknown494.Color = color;
			tag->Unknown4A4 = tag->Unknown494;
			((FontSys *)savatar->Unknown0C.Peek())->Unknown_6D8A80(true);
			return;
		}
	}
}






















































































































































// 0x6437A0
void UIStarbaseClass::Unknown_6437A0(int unknown, AvatarDescClass *desc)
{
	SClient *sclient = ((LineDisplayClientClass *)Client)->As_Space_Client();
	FAIL_IF(!sclient) return;
	sclient->Unknown_74C530(unknown);
	sclient->Unknown_74C3B0(unknown, *desc);
	UIStarbaseUnknownD4TableClass::iterator it = UnknownD4.find(unknown);
	if (it != UnknownD4.end()) {
		CharacterViewAvatarClass *avatar = it.Cur->Value->Avatar.Peek();
		int flags = it.Cur->Value->Unknown18;
		Matrix3D tm = avatar->Get_Transform();
		Unknown_63D060(unknown);
		Unknown_63CA50(unknown, tm, flags);
	}
}

// Out of line as the client's (auto-inlined into the callers above otherwise, though defined after them)
#pragma auto_inline(off)
// NOT EXACT: 0x643DF0, functional (the client's loop re-tests the vector's start each pass; SB1)
// 0x643DF0 (SGI's hashtable::begin, out of line)
UIStarbaseUnknownD4TableClass::iterator UIStarbaseUnknownD4TableClass::begin()
{
	for (unsigned int n = 0; n < Buckets.size(); ++n) {
		if (Buckets[n]) {
			iterator it;
			it.Cur = Buckets[n];
			it.Table = this;
			return(it);
		}
	}
	return(end());
}

// 0x643E60 (SGI's hashtable::find, out of line)
UIStarbaseUnknownD4TableClass::iterator UIStarbaseUnknownD4TableClass::find(const unsigned int &key)
{
	unsigned int n = Bucket_Of_Key(key);
	NodeStruct *first;
	for (first = Buckets[n]; first && !(first->Key == key); first = first->Next) {
	}
	iterator it;
	it.Cur = first;
	it.Table = this;
	return(it);
}

#pragma auto_inline(on)
