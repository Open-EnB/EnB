// Earth & Beyond client (reconstruction): app\client\ui_starbase.cpp (the client's __FILE__
// 'e:\build\beta\G\app\Client\ui_starbase.cpp', 0x632E50-0x648340 by its asserts, its string order and ui_slot_scut.cpp's end).
// So far the starbase interface's constructor 0x6333E0 (SB1), its lights helpers 0x640CC0-0x640F30 and the starbase lights panel (the class below,
// 0x646170-0x648238), ST1; the file's own functions with their asserts (lines 368-4180) lie around them in the client.
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
struct UIStarbaseLightsGadgetHolderStruct	// NAME 1: neutral (D10)
{
	UIStarbaseLightsGadgetHolderStruct();				// 0x585920 (0x10 bytes)
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
	UIStarbaseLightsGadgetHolderStruct *UnknownD8;		// +0xD8
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
		UnknownD8 = new UIStarbaseLightsGadgetHolderStruct;
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
