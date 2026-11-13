// Earth & Beyond client (reconstruction): app\client\ui_starbase.cpp (the client's __FILE__ 'e:\build\beta\G\app\Client\ui_starbase.cpp', 0x632E50-0x648340 by its asserts, its string order and ui_slot_scut.cpp's end).
#define __INTRFACE_MEMBERS	// intrface.h's members (the constructor 0x540D30 with the client: the symbol chat menu's)
// The starbase interface (constructor 0x6333E0, SB1), its helpers 0x638FF0-0x6437A0 as far as rebuilt (SB1, run 2026-11-09b), its lights helpers 0x640CC0-0x640F30 and the lights panel (0x646170-0x648238, ST1, at the end); the set-up 0x634140 (SU1) and the functions with asserts sit on the client's lines (862, 2004-4180).
#include "eff_o.h"	// first: client.h then skips its partial EffectObjectClass
#include "pdaint.h"	// first, before sclient.h: the PDA's character window (0x638390)
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
#include "..\packets\pkaction.h"	// ActionPacketClass (0x63DFE0)
#include "env_map_manager.h"	// EnvironmentMapManagerClass (0x6366A0)
#include "ui_talkt.h"	// UITalkTreeClass, the view's +0x190 (0x6390A0)
#include "..\..\ww3d2\ww3d.h"
#include "..\shared\message.h"
#include "local_interface_sort.h"
#include "unknown_675120.h"	// ButtonGadgetClass
#include "hanim.h"
#include "ui_cinfo.h"
#include "ui_attrb_skills.h"
#include "ui_attrb.h"

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
RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj);	// 0x497300 (cdecl, ro_funcs.cpp's neighbourhood)  // NAME 1 [robj]: as ui_avatr.cpp's

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
	void Unknown_78A200();								// 0x78A200 (0x638DA0: the ambient avatars restarted with 0x78B570(true))
	Unknown78A910AvatarClass(int a, AvatarDescClass *desc, EnvironmentMapManagerClass *env_maps);	// 0x78A910 (0x642740: the NPC's +0x1C and description)  // NAME 1 [a]: neutral
	void Unknown_78AD20(int anim, int min, int max, const char *model, const char *motion);	// 0x78AD20 (0x642740: an Action%02d, its roll range, ExtraAnim%02d and ExtraName%02d)  // NAME 1 [anim, min, max, model, motion]: as unknown_78a910.cpp's
	void Unknown_78B010(const Matrix3D &tm); void Unknown_78B290(const Matrix3D &tm);	// 0x78B010 / 0x78B290 (0x642740: a Location%02d's OffsetBoneName / DestBoneName bone)  // NAME 1 [tm]: neutral
	void Unknown_78B530(int a, int b, int c, int d);	// 0x78B530 (0x642740: MinWait, MaxWait, RelocateChance, ActionChance)  // NAME 1 [a, b, c, d]: neutral
	char Unknown384[0x3B9 - 0x384];					// +0x384
	bool Unknown3B9;									// +0x3B9 (0x642740: the INI's 'Sequential')
	char Unknown3BA[0x3E4 - 0x3BA];					// +0x3BA (0x3E4 bytes: 0x642740's new)
};
#include "sbase_info.h"
// The four rooms (the constructor's index): the key 0x698730 looks up (0xB8A870), the room's model part (0xB8A880),
// and the race letters by the starbase's column +0x18 (0xB8A950)
static int Unknown_B8A870[4] = { 0, 1, 3, 2 };	// NAME 1: neutral (D10)
static const char *Unknown_B8A880[4] = { "hangar", "main", "bazaar", "lounge" };	// NAME 1: neutral (D10)
static const char *Unknown_B8A950[10] = { "t", "j", "p", "n", "n", "n", "n", "n", "n", "n" };	// NAME 1: neutral (D10)





// @0x00632E50
UIStarbaseUnknownCCClass::~UIStarbaseUnknownCCClass()
{
	if (Avatar) {
		Avatar->Unknown_788EF0();
		Avatar->Remove();
		Avatar = NULL;
	}
	if (NameTag) {
		NameTag->Remove();
		NameTag = NULL;
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
inline int UIStarbaseClass::Unknown_Kind() { int c = 1; if (RoomInfo->Unknown0B) { c = 3; } else if (RoomInfo->Unknown0A) { c = 2; } return(c); }
inline StarbaseInfoClass *UIStarbaseClass::Get_Info() { return(UnknownB0->Info); }
// 0x6333E0
UIStarbaseClass::UIStarbaseClass(UIViewClass *view, LayerClass *layer, int index, const char *name) :
	Unknown5BB320Class(new UIMeshClass(name, 1), view, *layer),
	Room(index),
	UnknownB0((SBaseViewClass *)view),
	RoomInfo(NULL),
	UnknownB8(0),
	ExtraBones(NULL),
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
	Unknown4C.Add(KernelEventClass(2, 0), (void *)0x2B);
	Unknown4C.Add(KernelEventClass(2, 0x1000), (void *)0x2C);
	RoomInfo = Get_Info()->Find_Room(Unknown_B8A870[Room]);
	int column = Get_Info()->Style;
	char model[32];
	if (RoomInfo->Unknown0B) {
		_snprintf(model, sizeof(model), "%sextra_%s_3", Unknown_B8A950[column], Unknown_B8A880[Room]);
	} else if (RoomInfo->Unknown0A) {
		_snprintf(model, sizeof(model), "%sextra_%s_2", Unknown_B8A950[column], Unknown_B8A880[Room]);
	} else {
		_snprintf(model, sizeof(model), "%sextra_%s", Unknown_B8A950[column], Unknown_B8A880[Room]);
	}
	ExtraBones = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj(model));
	if (ExtraBones) {
		Unknown_Model()->Add_Sub_Object(ExtraBones.Peek());
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
	RoomInfo = NULL;
	InterfaceMesh->Unknown_5B9930(-1);
	if (ExtraBones) {
		ExtraBones->Remove();
		ExtraBones = NULL;
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
#pragma auto_inline(on)
// 0x633010
bool UIStarbaseUnknownCCClass::Unknown_633010()
{
	bool r = false;
	if ((Unknown18 & 0x3E) || Avatar->Unknown318.size()) r = true;
	return(r);
}

// 0x633040
void UIStarbaseUnknownCCClass::Unknown_633040()
{
	if (Unknown20 > 0.0f) {
		Unknown20 -= WW3D::Get_Frame_Time() * 0.001f;
		if (Unknown20 < 0.0f) {
			Unknown20 = 0;
			NameTag->Change_Text(Avatar->Desc.First_Name, 0);
			NameTag->Unknown_6D8A80(false);
		}
	}
}
#pragma auto_inline(off)
// 0x6330E0
void UIStarbaseUnknownCCClass::Unknown_6330E0(const Vector3 &color)
{
	Unknown20 = 2.0f;
	((FontSys *)NameTag.Peek())->Change_Text("\".....\"", 0);
	FontSys *tag = (FontSys *)NameTag.Peek();
	tag->Unknown494.Color = color;
	tag->Unknown4A4 = tag->Unknown494;
	((FontSys *)NameTag.Peek())->Unknown_6D8A80(true);
}



















































































// 0x633180
void UIStarbaseUnknownCCClass::Unknown_633180()
{
	FAIL_IF(!Avatar && !AmbientAvatar) return;
	FAIL_IF(!NameTag) return;
	std::string name = "";
	if (Avatar) {
		name = Avatar->Desc.Get_Full_Name();
	} else if (AmbientAvatar) {
		name = AmbientAvatar->Desc.Get_Full_Name();
	}
	NameTag->Change_Text(name.c_str(), 0);
	NameTag->Set_Hidden(true);
	NameTag->Unknown_6CE7D0(2);
	if (Unknown10) {
		Unknown10->Change_Text(Unknown24.c_str(), 0);
		Unknown10->Set_Hidden(true);
		Unknown10->Unknown_6CE7D0(2);
	}
}






#pragma auto_inline(on)

// 0x640810
void UIStarbaseClass::Unknown_640810()
{
	int i = 0;
	int a = Get_Info()->Style;
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
	*a = UnknownB0->Info->Style;
	*b = Room;
	if (RoomInfo->Unknown0B) {
		*c = 3;
	} else {
		*c = RoomInfo->Unknown0A ? 2 : 1;
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
		if (entry->AmbientAvatar != NULL) {
			entry->AmbientAvatar->Unknown_784A50((const char *)arg, true, false);
		}
	}
}
#pragma auto_inline(on)
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
#pragma auto_inline(off)
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
#pragma auto_inline(on)
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



#include "co.h"

#include "postate.h"
#include "coltest.h"
#include "random.h"
#include "aux_roots.h"
#include "..\shared\auxdata\auxdata.h"
extern RandomClass Unknown_BDBB84; extern Random4Class Unknown_BDB1C0;	// NAME 1 [Unknown_BDB1C0]: 0xBDB1C0, as camctrl_dock.cpp's
// The docking sounds by the starbase's column +0x18 (0xB8A8B0: 0x6415B0)
static const char *Unknown_B8A8B0[10] = { "terren_space_ship_dock_1.wav", "jenquai_space_ship_dock_1.wav", "progen_space_ship_dock_1.wav", "terren_space_ship_dock_1.wav", "terren_space_ship_dock_1.wav", "terren_space_ship_dock_1.wav", "terren_space_ship_dock_1.wav", "terren_space_ship_dock_1.wav", "terren_space_ship_dock_1.wav", "terren_space_ship_dock_1.wav" };	// NAME 1: neutral (D10)
static const char *Unknown_B8A8D8[10] = { "terren_space_ship_undock_1.wav", "jenquai_space_ship_undock_1.wav", "progen_space_ship_undock_1.wav", "terren_space_ship_undock_1.wav", "terren_space_ship_undock_1.wav", "terren_space_ship_undock_1.wav", "terren_space_ship_undock_1.wav", "terren_space_ship_undock_1.wav", "terren_space_ship_undock_1.wav", "terren_space_ship_undock_1.wav" };	// NAME 1: neutral (D10): the undocking sounds (0x638500)
// What 0x634140 news for the starbase interface's +0xB8 (0x44 bytes, constructor 0x6B6050, table 0xAFE174): a full-screen
// RectangleGadgetClass that keeps the view (not rebuilt; interim placement in this file, D137)
class UIStarbaseGadget6B6050Class : public RectangleGadgetClass	// NAME 1: neutral (D10)
{
public:
	UIStarbaseGadget6B6050Class(int x, int y, int width, int height, SBaseViewClass *view);	// 0x6B6050 (0, 0, 640, 480, the view)  // NAME 1 [x, y, width, height, view]: what 0x634140 passes
	SBaseViewClass *Unknown40;							// +0x40
};
#include "dynamic_window_class.h"
#include "gd_dynamicwindow.h"
#include "unknown_51c800.h"
// What 0x634140 news for the starbase interface's +0x1F8 (0xA4 bytes, constructor 0x644F80 with the space client, table
// 0xAF84CC, its other sides 0xAF84C0 / 0xAF84B4): the symbol chat menu ('fc_symbolchat.ini'; SYMBOL_CHAT_WINDOW with one
// SYMBOL_CHAT_SECTION, an option a row: Cancel, Chat, Trade, Group Invite); interim placement in this file, D137
struct UIStarbaseSymbolChatOptionStruct	// NAME 1: neutral (D10): a row of the menu (0x0C bytes: 0x645510 news it)
{
	UIStarbaseSymbolChatOptionStruct() : Window(NULL), Text(NULL), Gadget(NULL) {}
	~UIStarbaseSymbolChatOptionStruct();				// 0x644E70
	RefCountPtr<DynamicWindowClass> Window;				// +0x00 (SYMBOL_CHAT_OPTION)
	RefCountPtr<Unknown51C800Class> Text;				// +0x04 (the option's name)
	GadgetClass *Gadget;								// +0x08 (a DynamicWindowGadgetClass over Window; its id the option)
};
class UIStarbaseUnknown1F8Class : public InterfaceClass	// NAME 1: neutral (D10)
{
public:
	UIStarbaseUnknown1F8Class(SClient *sclient);		// 0x644F80  // NAME 1 [sclient]: what 0x634140 passes
	virtual ~UIStarbaseUnknown1F8Class();				// +0x00 0x6452C0 (deleting 0x645290)
	virtual void Unknown_58();							// +0x58 0x645B20 (opened at the pointer, 'fontclick.wav')
	virtual void Unknown_5C();							// +0x5C 0x645C50
	virtual bool Unknown_68(int key);					// +0x68 0x645C80 (an option chosen: kept in +0x84)  // NAME 1 [key]: InterfaceClass's
	void Unknown_645510();								// 0x645510 (the four options made, those the player cannot use greyed)
	void Unknown_645A60();								// 0x645A60 (the options removed)
	void Unknown_645CC0();								// 0x645CC0 (empty; no caller)
	void Unknown_645CE0();								// 0x645CE0 (the options rebuilt unless open; the starbase interface's 0x63B630)
	int Unknown_645DC0();								// 0x645DC0 (the chosen option, cleared; the starbase interface's 0x63DFE0)
	int Unknown84;										// +0x84 (the chosen option: 0x645C80)
	RefCountPtr<DynamicWindowClass> Window;				// +0x88 (SYMBOL_CHAT_WINDOW)
	RefCountPtr<DynamicWindowClass> Section;			// +0x8C (SYMBOL_CHAT_SECTION)
	GadgetClass *Gadget;								// +0x90 (a DynamicWindowGadgetClass over Window)
	std::vector<UIStarbaseSymbolChatOptionStruct *> Options;	// +0x94
};

// The VB_ models of 0x634140's NPC slots (0xB8AA18, by the NPC's 0x6976C0), the ico_ models of its terminals (0xB90C2C),
// the door bones by room (0xB8AA3C, four a room) and their 0x698730 keys (0xB8AABC), the starbase models of the door
// signs (0xB8A9A0 plain, 0xB8A9B8 / 0xB8A9D0 / 0xB8A9E8 / 0xB8AA00 by the column when the key's +0x0B is set), the
// monitor models by column (0xB8A978), the bar parts by room (0xB8A8A0) and the '%s_%dmd' names by room and column (0xB8ABBC)
static const char *Unknown_B8AA18[9] = { "VB_WEAPON", "VB_SYSTEM", "VB_CORE", "VB_CONSUMABLE", "VB_JUNK", "VB_COMPONENT", "VB_RESOURCE", "VB_BLACKMKT", NULL };	// NAME 1: neutral (D10)
static const char *Unknown_B90C2C[12] = { "ico_refinery", "ico_analyze", "ico_manufacture", "ico_jobs", "ico_browser", "ico_ship_con", "ico_ava_con", "ico_encyclo", "ico_encyclo", "ico_encyclo", "ico_encyclo", "ico_encyclo" };	// NAME 1: neutral (D10)
static int Unknown_B90C60[12] = { 5, 6, 7, 8, 9, 13, 14, -1, -1, -1, -1, -1 };	// NAME 1: neutral (D10): the view's interface type by facility type (0x6390A0)
static const char *Unknown_B8AA3C[16] = { "door_forward", NULL, NULL, NULL, "door_forward", "door_left", "door_back", "door_right", "door_forward", "door_left", "door_back", "door_right", "door_forward", "door_left", "door_back", "door_right" };	// NAME 1: neutral (D10)
static const char *Unknown_B8AA7C[16] = { "cam_door_fd_out", "", "", "", "cam_door_fd_out", "cam_door_lf_out", "cam_door_bk_out", "cam_door_rt_out", "", "cam_door_lf_out", "", "", "", "", "", "cam_door_rt_out" };	// NAME 1: neutral (D10): the doors' camera bones by room and column (0x63FCB0)
static int Unknown_B8AABC[16] = { 1, -1, -1, -1, -1, 2, 0, 3, -1, 1, -1, -1, -1, -1, -1, 1 };	// NAME 1: neutral (D10)
static int Unknown_B8AB3C[16] = { 1, -1, -1, -1, -1, 3, 0, 2, -1, 1, -1, -1, -1, -1, -1, 1 };	// NAME 1: neutral (D10): the room each door leads to (0x63FCB0)
static const char *Unknown_B8AB7C[16] = { "cam_door_fd_in", "", "", "", "cam_door_fd_in", "cam_door_lf_in", "cam_door_bk_in", "cam_door_rt_in", "", "cam_door_lf_in", "", "", "", "", "", "cam_door_rt_in" };	// NAME 1: neutral (D10): the doors' arrival camera bones by room and column (0x6366A0)
static const char *Unknown_B8A9A0[6] = { "sn_hangar", "sn_maindeck", "sn_npclounge", "sn_merchants", "sn_lounge", NULL };	// NAME 1: neutral (D10)
static const char *Unknown_B8A9B8[6] = { "sn_hangar", "sn_maindeck", "sn_npclounge", "sn_merchants", "sn_lounge", NULL };	// NAME 1: neutral (D10)
static const char *Unknown_B8A9D0[6] = { "sn_hangar", "sn_maindeck", "sn_npclounge", "sn_const", "sn_lounge", NULL };	// NAME 1: neutral (D10)
static const char *Unknown_B8A9E8[6] = { "sn_hangar", "sn_maindeck", "sn_npclounge", "sn_merchants", "sn_lounge", NULL };	// NAME 1: neutral (D10)
static const char *Unknown_B8AA00[6] = { "sn_hangar", "sn_maindeck", "sn_casino", "sn_merchants", "sn_lounge", NULL };	// NAME 1: neutral (D10)
static const char *Unknown_B8A978[10] = { "tsd01", "jsd01", "psd01", "tsd07", "tsd07", "tsd07", "tsd07", "tsd07", "tsd07", "tsd07" };	// NAME 1: neutral (D10)
static const char *Unknown_B8A8A0[4] = { "B", "08_H", "08_V", "08" };	// NAME 1: neutral (D10)
static const char *Unknown_B8ABBC[4][10] = {	// NAME 1: neutral (D10)
	{ "tsd01a", "jsd01a", "psd01a", "tsd07a", "tsd07a", "tsd07a", "tsd07a", "tsd07a", "tsd07a", "tsd07a" },
	{ "tbar01_h", "jbar01_h", "pbar01_h", "nbar01_h", "nbar01_h", "nbar01_h", "nbar01_h", "nbar01_h", "nbar01_h", "nbar01_h" },
	{ "tbar01_v", "jbar01_v", "pbar01_v", "nbar01_v", "nbar01_v", "nbar01_v", "nbar01_v", "nbar01_v", "nbar01_v", "nbar01_v" },
	{ "tbar01", "jbar01", "pbar01", "nbar01", "nbar01", "nbar01", "nbar01", "nbar01", "nbar01", "nbar01" },
};
void Unknown_496DB0(RenderObjClass *robj);	// 0x496DB0 (ro_funcs.cpp, cdecl), as avatar.cpp's  // NAME 1 [robj]: avatar.cpp's
inline UIStarbaseUnknownCCClass::UIStarbaseUnknownCCClass() : Unknown00(0), Avatar(NULL), AmbientAvatar(NULL), NameTag(NULL), Unknown10(NULL), Unknown14(0), Unknown18(0), Unknown1C(0), Unknown20(0), Unknown24(""), Unknown34(-1) {}
// NOT EXACT: 0x634140, functional from the pseudocode (D64; SU1): the window's set-up (slot +0x54): the full-screen
// gadget, the +0x1F8 interface, the player's avatar entry with its name and guild texts, the 14 NPC slots, the four
// terminals, the four doors with their signs, the ship on the hangar's extents, the bar and table models and the
// '%s_%dmd' animated models
// 0x634140
void UIStarbaseClass::Unknown_54()
{
	RefCountPtr<RenderObjClass> robj;
	int column = Get_Info()->Style;
	{
		InterfaceSortScopeClass scope(this);
		UnknownB8 = new UIStarbaseGadget6B6050Class(0, 0, 640, 480, UnknownB0);
		UnknownB8->Set_Last_Sort_Group(true);
		UnknownB8->Unknown_65FFA0(1000);
		Unknown64.Unknown_6870E0(UnknownB8, 0x2D, View, true, false);
		Unknown_5BB4F0(UnknownB8);
		Unknown_Model()->Set_Ignore_LOD_Cost(true);
		Unknown1F8 = Create_NEW(new UIStarbaseUnknown1F8Class(((LineDisplayClientClass *)Client)->As_Space_Client()));
		Unknown1F8->Unknown_54();
		WW3D::Enable_Overbright_Modify_On_Load(true);
		UnknownC0.D = new UIStarbaseUnknownCCClass;
		UnknownB0->Unknown_6A1880(UnknownC0.D->NameTag, 0);
		unsigned int id = ((LineDisplayClientClass *)Client)->AvatarID;
		UnknownC0.D->Avatar = Create_NEW((CharacterViewAvatarClass *)((LineDisplayClientClass *)Client)->As_Space_Client()->Unknown_74C7B0(id));
		UnknownC0.D->NameTag->Change_Text(UnknownC0.D->Avatar->Desc.First_Name, 0);
		UnknownC0.D->NameTag->Unknown494.Color.Set(0.33f, 0.33f, 0.99f);
		UnknownC0.D->NameTag->Unknown4A4 = UnknownC0.D->NameTag->Unknown494;
		UnknownB0->Unknown_6A1880(UnknownC0.D->Unknown10, 3);
		if (((ClientClass *)Client)->Unknown_728330()) {
			((ClientClass *)Client)->Get_Avatar()->Unknown_6FFFF0(UnknownC0.D->Unknown24);
			Vector3 color;
			((ClientClass *)Client)->Get_Avatar()->Unknown_700670(color);
			UnknownC0.D->Unknown10->Unknown494.Color = color;
			UnknownC0.D->Unknown10->Unknown4A4 = UnknownC0.D->Unknown10->Unknown494;
			UnknownC0.D->Unknown34 = ((ClientClass *)Client)->Get_Avatar()->Unknown_6FFF50();
		} else {
			UnknownC0.D->Unknown24 = "";
		}
		UnknownC0.D->Unknown10->Change_Text(UnknownC0.D->Unknown24.c_str(), 0);
		UnknownC0.D->Unknown10->Set_Hidden(true);
		UnknownC0.D->Unknown10->Unknown_6CE7D0(2);
		char name[32];
		char model[32];
		int i;
		for (i = 0; i < 14; i++) {
			if (RoomInfo->Find_NPC(i) == NULL) {
				continue;
			}
			StarbaseNPCClass *npc = RoomInfo->Find_NPC(i);
			if (npc->Unknown1C) {
				Unknown13C[i] = new UIStarbaseUnknownCCClass;
				Unknown13C[i]->AmbientAvatar = Create_NEW(Unknown_642740(npc));
				Unknown13C[i]->Unknown14 = new ButtonGadgetClass(Unknown13C[i]->AmbientAvatar->BodyObj.Peek(), *UnknownB0->Unknown80, 5, 0);
				Unknown13C[i]->Unknown14->Unknown2C_0A = true;
				Unknown13C[i]->Unknown14->Set_Last_Sort_Group(true);
				Unknown64.Unknown_6870E0(Unknown13C[i]->Unknown14, i + 2, View, true, true);
				continue;
			}
			_snprintf(name, sizeof(name), "avatar%02d", i);
			if (!InterfaceMesh->Peek_Object()->Get_Bone_Index(name)) {
				continue;
			}
			Unknown13C[i] = new UIStarbaseUnknownCCClass;
			if (npc->Booth == 9) {
				robj = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj(Unknown_B8AA18[npc->Get_Booth_Index()]));
				Unknown_496AE0(robj.Peek(), false);
				InterfaceMesh->Peek_Object()->Add_Sub_Object_To_Bone(robj.Peek(), name);
				Unknown13C[i]->Unknown14 = new ButtonGadgetClass(robj.Peek(), *UnknownB0->Unknown80, 5, 0);
				Unknown13C[i]->Unknown14->Unknown2C_0A = true;
				Unknown64.Unknown_6870E0(Unknown13C[i]->Unknown14, i + 2, View, true, false);
			} else {
				AvatarDescClass desc(npc->Desc);
				Unknown13C[i]->Avatar = Create_NEW(new CharacterViewAvatarClass(&desc, true, false, ((LineDisplayClientClass *)Client)->As_Space_Client()->EnvironmentMapManager));
				if (Unknown13C[i]->Avatar) {
					Unknown13C[i]->Avatar->Unknown330 = InterfaceMesh->Peek_Object()->Get_Bone_Transform(name);
					Unknown13C[i]->Avatar->Unknown_77E3D0(1);
					Unknown13C[i]->Avatar->Unknown_785F20(true);
					Unknown13C[i]->Unknown14 = new ButtonGadgetClass(Unknown13C[i]->Avatar->BodyObj.Peek(), *UnknownB0->Unknown80, 5, 0);
					Unknown13C[i]->Unknown14->Unknown2C_0A = true;
					Unknown13C[i]->Unknown14->Set_Last_Sort_Group(true);
					Unknown64.Unknown_6870E0(Unknown13C[i]->Unknown14, i + 2, View, true, true);
					Unknown13C[i]->Unknown24 = npc->Get_Name();
					if (npc->Booth > 1 && npc->Booth < 10) {
						WW3D::Enable_Overbright_Modify_On_Load(false);
						RefCountPtr<RenderObjClass> weapon = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(Unknown_B8AA18[npc->Get_Booth_Index()]));
						if (weapon) {
							InterfaceMesh->Peek_Object()->Add_Sub_Object_To_Bone(weapon.Peek(), name);
							Unknown194.push_back(weapon);
						}
						WW3D::Enable_Overbright_Modify_On_Load(true);
					}
				}
			}
		}
		WW3D::Enable_Overbright_Modify_On_Load(false);
		bool wall = (Room == 0 && ExtraBones);
		for (i = 0; i < 4; i++) {
			int found;
			if (!wall) {
				_snprintf(name, sizeof(name), "terminal%02d", i);
				found = InterfaceMesh->Peek_Object()->Get_Bone_Index(name);
			} else {
				_snprintf(name, sizeof(name), "wallterm%02d", i);
				found = ExtraBones->Get_Bone_Index(name);
			}
			if (!found) {
				continue;
			}
			StarbaseFacilityClass *terminal = RoomInfo->Find_Facility(i);
			if (terminal) {
				_snprintf(model, sizeof(model), wall ? "%s_wall_term" : "%s_monitor", Unknown_B8A950[column]);
				Unknown108[i] = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(model));
			}
			if (Unknown108[i]) {
				Unknown_496AE0(Unknown108[i].Peek(), false);
				(wall ? ExtraBones.Peek() : InterfaceMesh->Peek_Object())->Add_Sub_Object_To_Bone(Unknown108[i].Peek(), name);
				if (terminal->Type != -1) {
					RefCountPtr<RenderObjClass> icon = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(Unknown_B90C2C[terminal->Type]));
					(wall ? ExtraBones.Peek() : InterfaceMesh->Peek_Object())->Add_Sub_Object_To_Bone(icon.Peek(), name);
					Unknown194.push_back(icon);
				}
				UnknownF8[i] = new ButtonGadgetClass(Unknown108[i].Peek(), Layer, 5, 3);
				UnknownF8[i]->Unknown2C_0A = true;
				UnknownF8[i]->Set_Last_Sort_Group(true);
				Unknown64.Unknown_6870E0(UnknownF8[i], i + 0x11, View, true, false);
			}
		}

		for (i = 0; i < 4; i++) {
			const char *bone = Unknown_B8AA3C[Room * 4 + i];
			if (bone == NULL) {
				continue;
			}
			int key = Unknown_B8AABC[Room * 4 + i];
			if (key < 0 || Get_Info()->Find_Room(key) == NULL) {
				if (Room == 0) {
					_snprintf(model, sizeof(model), "%s%s", Unknown_B8A978[column], Unknown_B8A8A0[0]);
				} else {
					_snprintf(model, sizeof(model), "%sbar%s", Unknown_B8A950[column], Unknown_B8A8A0[Room]);
				}
				Unknown128[i] = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(model));
				if (Unknown128[i]) {
					Unknown_496AE0(Unknown128[i].Peek(), false);
					InterfaceMesh->Peek_Object()->Add_Sub_Object_To_Bone(Unknown128[i].Peek(), bone);
				}
			} else {
				_snprintf(model, sizeof(model), "%sbar06", Unknown_B8A950[column]);
				Unknown128[i] = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(model));
				if (Unknown128[i]) {
					Unknown_496AE0(Unknown128[i].Peek(), false);
					InterfaceMesh->Peek_Object()->Add_Sub_Object_To_Bone(Unknown128[i].Peek(), bone);
					Unknown118[i] = new ButtonGadgetClass(Unknown128[i].Peek(), Layer, 5, 3);
					Unknown118[i]->Unknown2C_0A = true;
					Unknown118[i]->Unknown_40(false);
					Unknown118[i]->Set_Last_Sort_Group(true);
					Unknown64.Unknown_6870E0(Unknown118[i], i + 0x16, View, true, false);
					std::string sign("");
					if (!Get_Info()->Find_Room(key)->Unknown0B) {
						sign = Unknown_B8A9A0[key];
					} else if (column == 0) {
						sign = Unknown_B8A9B8[key];
					} else if (column == 1) {
						sign = Unknown_B8A9D0[key];
					} else if (column == 2) {
						sign = Unknown_B8A9E8[key];
					} else {
						sign = Unknown_B8AA00[key];
					}
					robj = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(sign.c_str()));
					if (robj) {
						InterfaceMesh->Peek_Object()->Add_Sub_Object_To_Bone(robj.Peek(), bone);
						Unknown194.push_back(robj);
					}
				}
			}
		}

		if (Room == 0 && ((ClientClass *)Client)->Get_Ship()) {
			Unknown184 = Create_Get(((ClientClass *)Client)->Get_Ship()->Unknown_6F3BC0());
			Unknown_496AE0(Unknown184.Peek(), false);
			Unknown_496DB0(Unknown184.Peek());
			Unknown184->Set_LOD_Bias(100.0f);
			Unknown184->Set_Hidden(false);
			Play_State_Animation(Unknown184.Peek(), 1, 1.0f);
			RefCountPtr<RenderObjClass> extents;
			if (!ExtraBones) {
				G_ASSERT(ExtraBones);
				extents = Unknown184;
			} else {
				extents = Create_Get(ExtraBones->Get_Sub_Object_By_Name("ship_extents00"));
			}
			extents->Scale(0.6f, 0.6f, 0.6f);
			Unknown180 = new ButtonGadgetClass(extents.Peek(), Layer, 5, 3);
			Unknown180->Unknown2C_0A = true;
			Unknown180->Set_Last_Sort_Group(true);
			Unknown64.Unknown_6870E0(Unknown180, 1, View, true, true);
		}
		if (RoomInfo->HasBar) {
			_snprintf(model, sizeof(model), "%sbar05", Unknown_B8A950[column]);
			robj = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj(model));
			if (robj) {
				InterfaceMesh->Peek_Object()->Add_Sub_Object_To_Bone(robj.Peek(), "bar");
				Unknown194.push_back(robj);
			}
		}
		if (RoomInfo->HasTables) {
			for (int table = 0; ; table++) {
				_snprintf(name, sizeof(name), "table%02d", table);
				if (!InterfaceMesh->Peek_Object()->Get_Bone_Index(name)) {
					break;
				}
				_snprintf(model, sizeof(model), "%sbar07", Unknown_B8A950[column]);
				robj = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj(model));
				if (robj) {
					InterfaceMesh->Peek_Object()->Add_Sub_Object_To_Bone(robj.Peek(), name);
					Unknown194.push_back(robj);
				}
			}
		}

		const char *mesh_name = NULL;
		RenderObjClass *mesh = Unknown_Model();
		if (mesh) {
			mesh_name = mesh->Get_Name();
		}
		UnknownB0->Unknown_6A24C0(Room, mesh, mesh_name);
		for (int n = 1; ; n++) {
			const char *prefix = Unknown_B8ABBC[Room][column];
			if (RoomInfo->Unknown0B) {
				_snprintf(model, sizeof(model), "%s3_%dmd", prefix, n);
			} else if (RoomInfo->Unknown0A) {
				_snprintf(model, sizeof(model), "%s2_%dmd", prefix, n);
			} else {
				_snprintf(model, sizeof(model), "%s_%dmd", prefix, n);
			}
			robj = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj(model));
			UnknownB0->Unknown_6A2500(Room, robj.Peek(), mesh_name);
			if (!robj) {
				break;
			}
			Unknown1A0.push_back(robj);
			if (RoomInfo->Unknown0B) {
				_snprintf(name, sizeof(name), "%s3_%dmd.%s3_%dan", prefix, n, prefix, n);
			} else if (RoomInfo->Unknown0A) {
				_snprintf(name, sizeof(name), "%s2_%dmd.%s2_%dan", prefix, n, prefix, n);
			} else {
				_snprintf(name, sizeof(name), "%s_%dmd.%s_%dan", prefix, n, prefix, n);
			}
			RefCountPtr<HAnimClass> anim = Create_Get(WW3DAssetManager::Get_Instance()->Get_HAnim(name));
			robj->Set_Animation(anim.Peek(), 0.0f, 1);
		}
		Unknown_10();
	}
}

#pragma auto_inline(off)

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
	if (Unknown13C[index]->AmbientAvatar != NULL) {
		Unknown13C[index]->AmbientAvatar->Unknown_78A080();
		Unknown13C[index]->AmbientAvatar->Unknown_78B570(false);
		Matrix3D tm1 = Unknown13C[index]->AmbientAvatar->Get_Transform();
		Matrix3D tm2 = UnknownC0.D->Avatar->Get_Transform();
		Vector3 diff = tm2.Get_Translation() - tm1.Get_Translation();
		diff.Normalize();
		Vector3 z = tm2.Get_Z_Vector();
		Vector3 cross = Vector3::Cross_Product(z, diff);
		tm1.Set(diff, cross, z, tm1.Get_Translation());
		Unknown13C[index]->AmbientAvatar->Unknown_788700(tm1);
		tm2.Set(-diff, -cross, z, tm2.Get_Translation());
		UnknownC0.D->Avatar->Unknown_788700(tm2);
		UnknownB0->Unknown_69EB20(UnknownC0.D->Avatar->Unknown_787450(Unknown13C[index]->AmbientAvatar.Peek()), 1, 1);
		Unknown1FC = true;
	}
}

#pragma auto_inline(on)

#pragma auto_inline(off)
float Unknown_831E90(const Vector3 &a, float a_radius, const Vector3 &b, float b_radius);	// 0x831E90 (cdecl): the distance between two spheres, as gutter_radar.cpp's  // NAME 1: neutral (D10)

// 0x63B310
void UIStarbaseClass::Unknown_63B310()
{
	if (!Unknown21B && UnknownC0.D->Avatar != NULL && !Unknown200 && !Unknown1B2 && !Unknown1FD) {
		Matrix3D tm = UnknownC0.D->Avatar->Unknown_787A90();
		if (UnknownC0.D->Avatar->Unknown2F5 && !Unknown1FC) {
			tm = Unknown_63E930(tm);
		}
		if (Unknown1B1) {
			if (UnknownB0->UnknownE4 == 0.0f) {
				Unknown1B1 = false;
			}
			UnknownB0->Unknown_69EB20(tm, 1, false);
		} else if (UnknownC0.B < 0 && UnknownC0.A < 0) {
			UnknownB0->Unknown_69E510(tm);
		}
	}
}

// NOT EXACT: 0x63BC30, functional (loop shape: the client keeps the test of the count at the loop's top before the first sweep, ours rotates it; while, do-while, for(;;) and goto forms tried; C3)
// 0x63BC30
void UIStarbaseClass::Unknown_63BC30(RefCountPtr<CharacterViewAvatarClass> &avatar)
{
	int tries = 0;
	Matrix3D tm = avatar->Unknown_787DE0();
retry:
	if (tries < 10) {
		tries++;
		if (avatar->Unknown_787F40(Unknown_Model())) {
			goto retry;
		}
	}
	if (tries < 10) {
		Vector3 pos = tm.Get_Translation();
		if ((pos - Unknown1EC).Length() > 1.0f) {
			Unknown1EC = pos;
		}
	} else {
		tm.Set_Translation(Unknown1EC);
		avatar->Unknown330 = tm;
	}
}

// 0x63C4B0
void UIStarbaseClass::Unknown_63C4B0(RenderObjClass *obj, const char *bone)
{
	Matrix3D tm = InterfaceMesh->Peek_Object()->Get_Bone_Transform(bone);
	((SceneClass *)UnknownB0->Unknown70.Peek())->Add_Render_Object(obj);
	((UIStarbaseSceneClass *)UnknownB0->Unknown70.Peek())->Unknown_7621A0(obj);
	obj->Set_Transform(tm);
}

// NOT EXACT: 0x63D280, functional (inline budget: the client inlines SGI's find with the vector's size 0x644470 out of line; ours calls the out-of-line find 0x643E60; C3)
// 0x63D280
UIStarbaseUnknownCCClass *UIStarbaseClass::Unknown_63D280(unsigned int id)
{
	UIStarbaseUnknownD4TableClass::iterator found = UnknownD4.find(id);
	return(found.Cur ? found.Cur->Value : NULL);
}

// NOT EXACT: 0x63AAF0, functional (inline budget: the client inlines SGI's begin with the vector's size 0x644470 out of line; ours calls begin 0x643DF0; C3)
// 0x63AAF0
void UIStarbaseClass::Unknown_63AAF0()
{
	if (UnknownC0.D->Avatar) {
		UnknownC0.D->Avatar->Unknown_1C4();
		if (UnknownC0.D->Avatar->Unknown318.size() == 0) {
			Unknown_63BC30(UnknownC0.D->Avatar);
		}
		Unknown_640F50(UnknownC0.D->Avatar.Peek());
	}
	for (int i = 0; i < 14; i++) {
		if (Unknown13C[i]) {
			if (Unknown13C[i]->Avatar) {
				Unknown13C[i]->Avatar->Unknown_1C4();
			}
			if (Unknown13C[i]->AmbientAvatar) {
				Unknown13C[i]->AmbientAvatar->Unknown_1C4();
			}
		}
	}
	UIStarbaseUnknownD4TableClass::iterator it;
	for (it = UnknownD4.begin(); it != UnknownD4.end(); ++it) {
		UIStarbaseUnknownCCClass *savatar = it.Cur->Value;
		savatar->Avatar->Unknown_1C4();
		Unknown_640F50(savatar->Avatar.Peek());
	}
}

#pragma auto_inline(on)

// 0x63BA70
bool UIStarbaseClass::Unknown_63BA70(RenderObjClass *a, RenderObjClass *b, float radius)
{
	Vector3 pa = a->Get_Transform().Get_Translation();
	Vector3 pb = b->Get_Transform().Get_Translation();
	if (Unknown_831E90(pa, 3.0f, pb, radius) < 3.0f) {
		return(true);
	}
	return(false);
}

// 0x638CB0
void UIStarbaseClass::Unknown_88()
{
	Unknown1B3 = false;
	Unknown1FD = true;
	int i;
	for (i = 0; i < 4; i++) {
		if (UnknownF8[i]) {
			UnknownF8[i]->Unknown_40(false);
		}
	}
	for (i = 0; i < 14; i++) {
		if (Unknown13C[i] && Unknown13C[i]->Unknown14) {
			Unknown13C[i]->Unknown14->Unknown_40(false);
		}
	}
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

// NOT EXACT: 0x638DA0, functional (register: the client takes the RefCountPtr temporary's address in eax after pushing the model, ours in ecx before; a named temporary is worse; C3)
// 0x638DA0
void UIStarbaseClass::Unknown_8C()
{
	if (Unknown1FD) {
		Unknown1FD = false;
		Unknown1FC = true;
		Unknown1B1 = true;
		UnknownB0->Unknown_69EB20(UnknownC0.D->Avatar->Unknown_787A90(), 1, true);
		if (Unknown1B0 && UnknownC0.B >= 0) {
			UnknownB0->Unknown_69F540(Unknown_497300(((RObjGadgetClass *)Unknown13C[UnknownC0.B]->Unknown14)->RObj), "vb_blackmkt.vb_blackmkt-2", 0, false, false);
			Unknown1B0 = false;
		}
		UnknownC0.A = -1;
		UnknownC0.B = -1;
		UnknownC0.C = -1;
		Unknown1B3 = false;
		int i;
		for (i = 0; i < 4; i++) {
			if (UnknownF8[i]) {
				UnknownF8[i]->Unknown_40(true);
			}
		}
		for (i = 0; i < 14; i++) {
			if (Unknown13C[i]) {
				if (Unknown13C[i]->Unknown14) {
					Unknown13C[i]->Unknown14->Unknown_40(true);
				}
				if (Unknown13C[i]->AmbientAvatar) {
					Unknown13C[i]->AmbientAvatar->Unknown_78A200();
					Unknown13C[i]->AmbientAvatar->Unknown_78B570(true);
				}
			}
		}
		if (!UnknownB0->Unknown_69CBE0()) {
			UnknownC0.E = 0;
			Unknown_63E2B0();
		}
		if (UnknownC0.D->Avatar) {
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
}

// The door sounds by the starbase's column (0xB8A900 opening, 0xB8A928 closing; 0x63C590)
static const char *Unknown_B8A900[10] = { "terren_station_door_1.wav", "jenquai_station_door_1.wav", "progen_station_door_1.wav", "terren_station_door_1.wav", "terren_station_door_1.wav", "terren_station_door_1.wav", "terren_station_door_1.wav", "terren_station_door_1.wav", "terren_station_door_1.wav", "terren_station_door_1.wav" };	// NAME 1: neutral (D10)
static const char *Unknown_B8A928[10] = { "terran_station_door_close", "jenquai_station_door_close", "progen_station_door_close", "terran_station_door_close", "terran_station_door_close", "terran_station_door_close", "terran_station_door_close", "terran_station_door_close", "terran_station_door_close", "terran_station_door_close" };	// NAME 1: neutral (D10)

// NOT EXACT: 0x63C590, functional (register: with 0x63BA70 inline, as the client has it, the shape is the client's; esi / edi swapped for the door and the avatar, the two flags' stores; C3)
// 0x63C590
void UIStarbaseClass::Unknown_63C590()
{
	for (int i = 0; i < 4; i++) {
		bool open = false;
		if (Unknown118[i]) {
			if (Unknown_63BA70(UnknownC0.D->Avatar.Peek(), ((RObjGadgetClass *)Unknown118[i])->RObj, 10.0f)) {


				open = true;
			} else {
				UIStarbaseUnknownD4TableClass::iterator it;
				for (it = UnknownD4.begin(); it != UnknownD4.end(); ++it) {
					if (Unknown_63BA70(it.Cur->Value->Avatar.Peek(), ((RObjGadgetClass *)Unknown118[i])->RObj, 10.0f)) {


						open = true;
						break;
					}
				}
			}
		}
		if (Unknown138[i] != open) {
			if (UnknownB0->Unknown_69F850(i + 2)) {
				int column = UnknownB0->Info->Style;
				char anim[64];
				_snprintf(anim, sizeof(anim), "%sbar06.%sbar06-1", Unknown_B8A950[column], Unknown_B8A950[column]);
				UnknownB0->Unknown_69F540(Unknown128[i], anim, i + 2, false, !open);
				SoundLibraryClass::Play_2D_Sound(open ? Unknown_B8A900[column] : Unknown_B8A928[column], 2);
				Unknown138[i] = open;
			}
		}
	}
}
// NOT EXACT: 0x637130, functional (inline budget: the client inlines the string's assign (_Grow, the copy) at the last eight keys, ours calls it at all sixteen; O4)
void UIStarbaseClass::Unknown_70(ControlKeysManager *manager)
{
	FAIL_IF(!manager) return;
	manager->Add_Keys(&Unknown4C, std::string("Move Forward Toggle"), (void *)0x1D, 0);
	manager->Add_Keys(&Unknown4C, std::string("First Person"), (void *)0x1B, 0);
	manager->Add_Keys(&Unknown4C, std::string("Avatar Run"), (void *)0x1C, 0);
	manager->Add_Keys(&Unknown4C, std::string("Move Backward"), (void *)0x1F, 0);
	manager->Add_Keys(&Unknown4C, std::string("Move Backward"), (void *)0x20, 0x1000);
	manager->Add_Keys(&Unknown4C, std::string("Move Forward"), (void *)0x21, 0);
	manager->Add_Keys(&Unknown4C, std::string("Move Forward"), (void *)0x22, 0x1000);
	manager->Add_Keys(&Unknown4C, std::string("Tilt Down"), (void *)0x27, 0);
	manager->Add_Keys(&Unknown4C, std::string("Tilt Down"), (void *)0x28, 0x1000);
	manager->Add_Keys(&Unknown4C, std::string("Tilt Up"), (void *)0x29, 0);
	manager->Add_Keys(&Unknown4C, std::string("Tilt Up"), (void *)0x2A, 0x1000);
	manager->Add_Keys(&Unknown4C, std::string("Turn Left"), (void *)0x23, 0);
	manager->Add_Keys(&Unknown4C, std::string("Turn Left"), (void *)0x24, 0x1000);
	manager->Add_Keys(&Unknown4C, std::string("Turn Right"), (void *)0x25, 0);
	manager->Add_Keys(&Unknown4C, std::string("Turn Right"), (void *)0x26, 0x1000);
	manager->Add_Keys(&Unknown4C, std::string("Restore Camera"), (void *)0x1E, 0);
}



// NOT EXACT: 0x637A90, functional (inline budget: the client inlines the string's assign (_Grow, the copy) at the last eight keys, ours calls it at all sixteen; O4)
void UIStarbaseClass::Unknown_74(ControlKeysManager *manager)
{
	FAIL_IF(!manager) return;
	manager->Remove_Keys(&Unknown4C, std::string("Move Forward Toggle"), 0);
	manager->Remove_Keys(&Unknown4C, std::string("First Person"), 0);
	manager->Remove_Keys(&Unknown4C, std::string("Avatar Run"), 0);
	manager->Remove_Keys(&Unknown4C, std::string("Move Backward"), 0);
	manager->Remove_Keys(&Unknown4C, std::string("Move Backward"), 0x1000);
	manager->Remove_Keys(&Unknown4C, std::string("Move Forward"), 0);
	manager->Remove_Keys(&Unknown4C, std::string("Move Forward"), 0x1000);
	manager->Remove_Keys(&Unknown4C, std::string("Tilt Down"), 0);
	manager->Remove_Keys(&Unknown4C, std::string("Tilt Down"), 0x1000);
	manager->Remove_Keys(&Unknown4C, std::string("Tilt Up"), 0);
	manager->Remove_Keys(&Unknown4C, std::string("Tilt Up"), 0x1000);
	manager->Remove_Keys(&Unknown4C, std::string("Turn Left"), 0);
	manager->Remove_Keys(&Unknown4C, std::string("Turn Left"), 0x1000);
	manager->Remove_Keys(&Unknown4C, std::string("Turn Right"), 0);
	manager->Remove_Keys(&Unknown4C, std::string("Turn Right"), 0x1000);
	manager->Remove_Keys(&Unknown4C, std::string("Restore Camera"), 0);
}


// @0x00636D50
// 0x636D50 (slot +0x5C, the interface closed: the teleport effect, the ship, the animation channels, the lights, the
// models' parts, the +0xEC entries, the NPC slots and the own avatar taken out; the client's view by room)
void UIStarbaseClass::Unknown_5C()
{
	int i;
	if (Unknown220) {
		EffectObjectClass *effect = ((ClientClass *)Client)->Get_Effect(Unknown220);
		if (effect) {
			effect->Unknown_6E6E30();
		}
		Unknown220 = 0;
	}
	if (Unknown184) {
		Unknown184->Remove();
	}
	UnknownB0->Unknown_6A2100();
	for (i = 0; i < 6; i++) {
		UnknownB0->Unknown_69F480(i);
	}
	UnknownB0->Unknown_69DB50(1);
	Unknown_640CC0();
	std::list<RefCountPtr<RenderObjClass> >::iterator it;
	for (it = Unknown1A0.begin(); it != Unknown1A0.end(); it++) {
		RefCountPtr<RenderObjClass> robj = *it;
		InterfaceMesh->Peek_Object()->Remove_Sub_Object(robj.Peek());
	}
	for (it = Unknown194.begin(); it != Unknown194.end(); it++) {
		RefCountPtr<RenderObjClass> robj = *it;
		robj->Set_Hidden(true);
	}
	std::list<UIStarbaseUnknownECStruct *>::iterator entry;
	for (entry = UnknownEC.begin(); entry != UnknownEC.end(); entry++) {
		delete *entry;
	}
	UnknownEC.clear();
	for (i = 0; i < 14; i++) {
		if (Unknown13C[i]) {
			if (Unknown13C[i]->Avatar) {
				Unknown13C[i]->Avatar->Remove();
			}
			if (Unknown13C[i]->AmbientAvatar) {
				Unknown13C[i]->AmbientAvatar->Remove();
			}
			if (Unknown13C[i]->NameTag) {
				Unknown13C[i]->NameTag->Remove();
				Unknown13C[i]->NameTag = NULL;
			}
		}
	}
	UnknownC0.D->Avatar->Unknown230 = true;
	UnknownC0.D->Avatar->Unknown36C = false;
	UnknownC0.D->Avatar->Unknown_785F20(false);
	UnknownC0.D->Avatar->Unknown_788EF0();
	UnknownC0.D->Avatar->Remove();
	UnknownC0.D->NameTag->Set_Hidden(true);
	if (UnknownC0.D->Unknown10) {
		UnknownC0.D->Unknown10->Set_Hidden(true);
	}
	UnknownB0->Unknown_6A1130();
	int view;
	switch (Room) {
		case 0:
			view = 0x20;
			break;
		case 2:
			view = 0x24;
			break;
		case 1:
			view = 0x26;
			break;
		case 3:
			view = 0x22;
			break;
		default:
			view = 0x27;
			break;
	}
	((LineDisplayClientClass *)Client)->As_Space_Client()->Unknown_744560(view);
	Unknown5BB320Class::Unknown_5C();
	Unknown_63C890();
}



inline RefCountPtr<UICinfoClass> PDAInterfaceClass::Get_Character() { return(Character); }	// 0x6384D0 (out of line after 0x638390)




// @0x00638390
// 0x638390 (slot +0x60, every frame: the arrival countdown, the room's updates, the base's frame, then the PDA's
// attributes window when it is shown)
void UIStarbaseClass::Unknown_60()
{
	if (!Room && Unknown1AC > 0 && --Unknown1AC == 0) {
		Unknown_6415B0();
	}
	Unknown_640360();
	Unknown_63FF80();
	Unknown_63DFE0();
	Unknown_63A100();
	Unknown_639630();
	Unknown_63E770();
	Unknown_63AAF0();
	Unknown_641C00();
	Unknown_63B310();
	Unknown_63AC60();
	Unknown_63C590();
	Unknown5BB320Class::Unknown_60();
	Unknown_6390A0();
	Unknown_63FCB0();
	SClient *sclient = ((LineDisplayClientClass *)Client)->As_Space_Client();
	if (sclient && sclient->PDAInterface) {
		UICinfoClass *cinfo = sclient->PDAInterface->Get_Character().Peek();
		if (cinfo && cinfo->UnknownB4 && cinfo->UnknownB4->UnknownC0 && cinfo->UnknownB4->UnknownC0->Active) {
			cinfo->UnknownB4->UnknownC0->Unknown_5473B0();
		}
	}
}


// @0x00638500
// 0x638500 (slot +0x68, a key or button: the facilities and NPCs (2..0xF, 0x11..0x14, 100..), 1 leaving the starbase,
// the movement and camera keys of 0x637130)
bool UIStarbaseClass::Unknown_68(int key)
{
	if (Unknown1FC || Unknown1FD || Unknown1FE || Unknown1FF || Unknown200 || Unknown201 || UnknownB0->Unknown_69CCD0() || Unknown1F8->Active) {
		return(false);
	}
	if (key >= 100) {
		Unknown_63B630(100, key - 100);
		return(true);
	}
	switch (key) {
		case 0x11:
		case 0x12:
		case 0x13:
		case 0x14:
			SoundLibraryClass::Play_2D_Sound("ButtonClick02.wav", 2);
			Unknown_63B630(0x11, key - 0x11);
			return(true);
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			SoundLibraryClass::Play_2D_Sound("ButtonClick02.wav", 2);
			Unknown_63B630(2, key - 2);
			return(true);
		case 1:
			if (UnknownC0.D->Avatar && UnknownC0.D->Avatar->Unknown318.empty()) {
				int column = UnknownB0->Info->Style;
				Unknown_63B5A0();
				UnknownB0->Unknown_69E6E0("camera_leave", 6, false, 0);
				InterfaceMesh->Unknown_5B9830();
				char anim[64];
				if (RoomInfo->Unknown0A) {
					_snprintf(anim, sizeof(anim), "%sa2.%sa2-out", Unknown_B8A978[column], Unknown_B8A978[column]);
				} else {
					_snprintf(anim, sizeof(anim), "%sa.%sa-out", Unknown_B8A978[column], Unknown_B8A978[column]);
				}
				InterfaceMesh->Unknown_5B8EE0(0, 0, anim, false, 0, 0, 0, -1);
				SoundLibraryClass::Play_2D_Sound(Unknown_B8A8D8[column], 2);
				UnknownC0.D->Avatar->Unknown_7896B0(3);
				UnknownB0->Unknown_69CBE0();
				UnknownC0.E = 4;
				Unknown_63E2B0();
				Unknown_63E4B0(-1, Room);
				Unknown21C = true;
				Unknown1FC = true;
				Unknown201 = true;
				Unknown1B2 = true;
				Unknown_641B00();
				UnknownB0->Unknown_6A23E0(Unknown214, Unknown215, Unknown216, Unknown217, Unknown1B3);
			}
			return(true);
		case 0x1D:
			if (UnknownC0.D->Avatar && UnknownC0.D->Avatar->Unknown318.empty()) {
				if (Unknown1B3) {
					UnknownC0.D->Avatar->Unknown_786C70();
				} else if (!(UnknownC0.D->Avatar->Unknown2F8 & 1)) {
					UnknownC0.D->Avatar->Unknown_786B60(false);
				}
				Unknown1B3 = !Unknown1B3;
			}
			return(true);
		case 0x1B:
			if (UnknownC0.D->Avatar && !Unknown1B1) {
				UnknownC0.D->Avatar->Unknown2F5 = !UnknownC0.D->Avatar->Unknown2F5;
				UnknownB0->Unknown_69E510(UnknownC0.D->Avatar->Unknown_787A90());
				*(Matrix3D *)Unknown1BC = UnknownC0.D->Avatar->Unknown_787A90();
				BoolOptionStruct *option = OptionClass::Instance->Find_Bool("Starbase_Third_Person_Mode");
				if (option) {
					option->Value = UnknownC0.D->Avatar->Unknown2F5;
				}
			}
			return(true);
		case 0x1C:
			{
				BoolOptionStruct *option = OptionClass::Instance->Find_Bool("Starbase_Default_Run");
				if (option) {
					option->Value = !option->Value;
				}
			}
			return(true);
		case 0x1E:
			if (CharacterViewAvatarClass *avatar = UnknownC0.D->Avatar.Peek()) {
				avatar->Unknown310 = 0;
				avatar->Unknown30C = 0;
			}
			return(true);
		case 0x21:
			Unknown214 = true;
			return(true);
		case 0x22:
			Unknown214 = false;
			return(true);
		case 0x1F:
			Unknown215 = true;
			return(true);
		case 0x20:
			Unknown215 = false;
			return(true);
		case 0x25:
			Unknown217 = true;
			return(true);
		case 0x26:
			Unknown217 = false;
			return(true);
		case 0x23:
			Unknown216 = true;
			return(true);
		case 0x24:
			Unknown216 = false;
			return(true);
		case 0x29:
			Unknown218 = true;
			return(true);
		case 0x2A:
			Unknown218 = false;
			return(true);
		case 0x27:
			Unknown219 = true;
			return(true);
		case 0x28:
			Unknown219 = false;
			return(true);
		case 0x2B:
			Unknown21A = true;
			return(true);
		case 0x2C:
			Unknown21A = false;
			return(true);
	}
	return(false);
}



// NOT EXACT: 0x6366A0, functional (inline budget: the client builds the ship transform through ??_H (the Vector4 constructor 0x468E70 out of line, as 0x640360) and calls the avatar RefCountPtr's operator-> out of line; the column in ebx, ours in ebp; O4)
// 0x6366A0 (slot +0x58, the interface opened: the room's model in the view, the camera at the door the avatar came
// through (or 'camera_3rd' on arrival), the ship and its animation in the hangar, the NPC slots, the parts, the fog)
void UIStarbaseClass::Unknown_58()
{
	InterfaceSortScopeClass scope(this);
	Unknown5BB320Class::Unknown_58();
	Unknown1F8->Unknown_7C(View, false);
	int column = UnknownB0->Info->Style;
	((LineDisplayClientClass *)Client)->As_Space_Client()->EnvironmentMapManager->Unknown_760770(100000, true);
	UnknownB0->Unknown_69DBA0(1, Create_Get(InterfaceMesh->Unknown_5B8B20()));
	Unknown21C = false;
	Unknown1B3 = false;
	Unknown1B2 = false;
	UnknownC0.A = -1;
	UnknownC0.B = -1;
	UnknownC0.C = -1;
	memset(Unknown138, 0, sizeof(Unknown138));
	int i;
	if (UnknownB0->Unknown58 == -1) {
		UnknownB0->Unknown_69E260("camera_3rd");
		Unknown1FD = true;
		Unknown1AC = 4;
	} else {
		for (i = 0; i < 4; i++) {
			if (Unknown_B8AB3C[Room * 4 + i] == UnknownB0->Unknown58) {
				Unknown_63C080(Unknown_B8AB7C[Room * 4 + i], true, true);
				UnknownB0->Unknown_69E510(UnknownC0.D->Avatar->Unknown_787A90());
				break;
			}
		}
		Unknown200 = false;
		Unknown1FC = false;
		Unknown1B1 = false;
	}
	if (!Room) {
		RenderObjClass *ship = Unknown184.Peek();
		if (ship && !Unknown1AC) {
			Matrix3D tm;
			tm = InterfaceMesh->Peek_Object()->Get_Bone_Transform("ship");
			((SceneClass *)UnknownB0->Unknown70.Peek())->Add_Render_Object(ship);
			((UIStarbaseSceneClass *)UnknownB0->Unknown70.Peek())->Unknown_7621A0(ship);
			ship->Set_Transform(tm);
		}
		char anim[32];
		if (RoomInfo->Unknown0A) {
			_snprintf(anim, sizeof(anim), "%sa2.%sa2-1", Unknown_B8A978[column], Unknown_B8A978[column]);
		} else {
			_snprintf(anim, sizeof(anim), "%sa.%sa-1", Unknown_B8A978[column], Unknown_B8A978[column]);
		}
		UnknownB0->Unknown_69F540(Unknown_497300(InterfaceMesh->Peek_Object()), anim, 1, true, false);
	}
	int view;
	switch (Room) {
		case 0:
			view = 0x1F;
			break;
		case 2:
			view = 0x23;
			break;
		case 1:
			view = 0x25;
			break;
		case 3:
			view = 0x21;
			break;
		default:
			view = 0x27;
			break;
	}
	((LineDisplayClientClass *)Client)->As_Space_Client()->Unknown_744560(view);
	for (i = 0; i < 14; i++) {
		if (Unknown13C[i]) {
			if (Unknown13C[i]->Avatar) {
				((SceneClass *)UnknownB0->Unknown70.Peek())->Add_Render_Object(Unknown13C[i]->Avatar.Peek());
				((UIStarbaseSceneClass *)UnknownB0->Unknown70.Peek())->Unknown_7621A0(Unknown13C[i]->Avatar.Peek());
			} else if (Unknown13C[i]->AmbientAvatar) {
				((SceneClass *)UnknownB0->Unknown70.Peek())->Add_Render_Object(Unknown13C[i]->AmbientAvatar.Peek());
				((UIStarbaseSceneClass *)UnknownB0->Unknown70.Peek())->Unknown_7621A0(Unknown13C[i]->AmbientAvatar.Peek());
			}
			UnknownB0->Unknown_6A1880(Unknown13C[i]->NameTag, 1);
			if (Unknown13C[i]->Unknown24.length()) {
				UnknownB0->Unknown_6A1880(Unknown13C[i]->Unknown10, 1);
			}
			Unknown13C[i]->Unknown_633180();
		}
	}
	Unknown_640810();
	std::list<RefCountPtr<RenderObjClass> >::iterator it;
	for (it = Unknown194.begin(); it != Unknown194.end(); it++) {
		RefCountPtr<RenderObjClass> robj = *it;
		robj->Set_Hidden(false);
	}
	for (it = Unknown1A0.begin(); it != Unknown1A0.end(); it++) {
		RefCountPtr<RenderObjClass> robj = *it;
		InterfaceMesh->Peek_Object()->Add_Sub_Object_To_Bone(robj.Peek(), "root");
	}
	if (RoomInfo->Unknown0C) {
		UnknownB0->Unknown_6A20C0(RoomInfo->Unknown18, *(float *)&RoomInfo->Unknown10, *(float *)&RoomInfo->Unknown14);
	}
	if (OptionClass::Instance->Get_Bool("Starbase_Third_Person_Mode")) {
		if (!UnknownC0.D->Avatar->Unknown2F5) {
			UnknownC0.D->Avatar->Unknown2F5 = !UnknownC0.D->Avatar->Unknown2F5;
		}
	} else if (UnknownC0.D->Avatar->Unknown2F5) {
		UnknownC0.D->Avatar->Unknown2F5 = !UnknownC0.D->Avatar->Unknown2F5;
	}
	((LineDisplayClientClass *)Client)->As_Space_Client()->Mouse->Unknown_515120(2);
	((LineDisplayClientClass *)Client)->As_Space_Client()->Mouse->Unknown_515400();
}











// NOT EXACT: 0x6390A0, functional (inline budget: the client destroys the two name strings through _Tidy(true) out of line, ours inlines it; O4)
// 0x6390A0 (0x638390's frame: the arrival walk to a 'Spawn%02d' bone, then the facility's or NPC's interface once the
// camera has arrived, the departure once the leaving animation ends)
void UIStarbaseClass::Unknown_6390A0()
{
	if (Unknown1FD) {
		return;
	}
	if (!Room && Unknown200) {
		if (InterfaceMesh->Unknown_5BA320(0, false)) {
			Unknown_63E4B0(Room, -1);
			char name[64];
			if (ExtraBones) {
				_snprintf(name, sizeof(name), "Spawn%02d", Unknown_BDBB84(0, 0x11));
				Matrix3D tm = ExtraBones->Get_Bone_Transform(name);
				Unknown_63BDB0(tm);
			} else {
				_snprintf(name, sizeof(name), "Spawn%02d", Unknown_BDBB84(0, 4));
				Unknown_63C080(name, false, false);
			}
			UnknownC0.D->Avatar->Unknown_7896B0(2);
			Unknown200 = false;
			Unknown1FC = true;
			Unknown1B1 = true;
		} else {
			Matrix3D tm = InterfaceMesh->Peek_Object()->Get_Bone_Transform("camera_3rd");
			UnknownB0->Unknown_69E510(tm);
		}
	}
	if (Unknown1FC && UnknownB0->UnknownE4 == 0.0f) {
		Unknown1FC = false;
		if (UnknownC0.B >= 0) {
			StarbaseNPCClass *npc = RoomInfo->Find_NPC(UnknownC0.B);
			if (npc->Booth >= StarbaseNPCClass::BOOTH_NONE && npc->Booth <= StarbaseNPCClass::BOOTH_BLACKMKT) {
				UnknownB0->Unknown_69F8F0(11, npc->Unknown04);
				if (UnknownB0->Unknown190) {
					UnknownB0->Unknown190->Unknown_64C7B0(npc->Desc.Get_Full_Name().c_str());
				}
			}
		} else if (UnknownC0.A >= 0) {
			UnknownB0->Unknown_69F8F0(Unknown_B90C60[RoomInfo->Find_Facility(UnknownC0.A)->Type], 0);
		}
	}
	if (Unknown201 && UnknownC0.D->Avatar->Unknown318.empty()) {
		if (InterfaceMesh->Unknown_5BA320(0, false)) {
			UnknownB0->Unknown170 = true;
			Unknown201 = false;
		}
		if (!UnknownC0.D->Avatar->Unknown380 && UnknownC0.D->Avatar->Unknown_7863E0()) {
			UnknownC0.D->Avatar->Remove();
			UnknownC0.D->NameTag->Set_Hidden(true);
			UnknownC0.D->Avatar->Unknown_786370(false);
		}
	}
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








































































































// NOT EXACT: 0x63AC60, functional (frame 0x28 / 0x2C; the client calls the own guild string's assign out of line and inlines the others'; O4)
// 0x63AC60 (0x638390's frame: the name tags shown or hidden, their guild texts and colours kept up to date)
void UIStarbaseClass::Unknown_63AC60()
{
	if (OptionClass::Instance->Get_Bool("Show_Self_Name") && UnknownC0.D->Avatar && !UnknownC0.D->Avatar->Unknown_7863E0()) {
		UnknownC0.D->NameTag->Set_Hidden(false);
		if (UnknownC0.D->Unknown10) {
			UnknownC0.D->Unknown10->Set_Hidden(false);
		}
		Unknown_63F490(UnknownC0.D);
		if (((ClientClass *)Client)->Get_Avatar()->Unknown_6FFF50() != UnknownC0.D->Unknown34) {
			if (((ClientClass *)Client)->Unknown_728300()) {
				((ClientClass *)Client)->Get_Avatar()->Unknown_6FFFF0(UnknownC0.D->Unknown24);
				Vector3 color;
				((ClientClass *)Client)->Get_Avatar()->Unknown_700670(color);
				UnknownC0.D->Unknown10->Unknown494.Color = color;
				UnknownC0.D->Unknown10->Unknown4A4 = UnknownC0.D->Unknown10->Unknown494;
				UnknownC0.D->Unknown34 = ((ClientClass *)Client)->Get_Avatar()->Unknown_6FFF50();
			} else {
				UnknownC0.D->Unknown24 = "";
				UnknownC0.D->Unknown34 = -1;
			}
			UnknownC0.D->Unknown10->Change_Text(UnknownC0.D->Unknown24.c_str(), 0);
		}
	} else {
		UnknownC0.D->NameTag->Set_Hidden(true);
		if (UnknownC0.D->Unknown10) {
			UnknownC0.D->Unknown10->Set_Hidden(true);
		}
	}
	for (int i = 0; i < 14; i++) {
		if (Unknown13C[i]) {
			Unknown_63F490(Unknown13C[i]);
		}
	}
	UIStarbaseUnknownD4TableClass::iterator it;
	for (it = UnknownD4.begin(); it != UnknownD4.end(); it++) {
		UIStarbaseUnknownCCClass *savatar = it.Cur->Value;
		FAIL_IF(!savatar) continue;
		if (UnknownB0->Unknown_69CCD0()) {
			if (savatar->NameTag->Unknown440 == 1) {
				savatar->NameTag->Unknown_6CE7D0(2);
			}
			continue;
		}
		Unknown_63F490(savatar);
		savatar->Unknown_633040();
		if (savatar->Unknown20 <= 0.0f) {
			if (Unknown_641EB0(savatar)) {
				savatar->NameTag->Unknown494.Color = Vector3(0.66f, 0.0f, 0.66f);
			} else {
				savatar->NameTag->Unknown494.Color = Vector3(0.33f, 0.33f, 0.99f);
			}
			savatar->NameTag->Unknown4A4 = savatar->NameTag->Unknown494;
		}
		ClientObjectClass *object = ((LineDisplayClientClass *)Client)->As_Space_Client()->Find_Object(savatar->Unknown00);
		if (object && object->Unknown_6FFF50() != savatar->Unknown34) {
			if (object->Unknown_6FFE90()) {
				object->Unknown_6FFFF0(savatar->Unknown24);
				Vector3 color;
				if (Unknown_641EB0(savatar)) {
					color.Set(0.66f, 0.0f, 0.66f);
				} else {
					color.Set(0.33f, 0.33f, 0.99f);
				}
				object->Unknown_700670(color);
				savatar->Unknown10->Unknown494.Color = color;
				savatar->Unknown10->Unknown4A4 = savatar->Unknown10->Unknown494;
				savatar->Unknown34 = object->Unknown_6FFF50();
			} else {
				savatar->Unknown24 = "";
				savatar->Unknown34 = -1;
			}
			savatar->Unknown10->Change_Text(savatar->Unknown24.c_str(), 0);
		}
	}
}











































































































































// NOT EXACT: 0x63B630, functional (register: the black market booth's RefCountPtr temporary's address taken in ecx after the model's load in the client, in edx before it here, as 0x638DA0; O4)
// 0x63B630 (0x638500's keys: 2 an NPC slot, 0x11 a terminal, 100 a symbol chat option; the camera turned to it)
void UIStarbaseClass::Unknown_63B630(int type, int index)
{
	UnknownC0.A = -1;
	UnknownC0.B = -1;
	UnknownC0.C = -1;
	switch (type) {
		case 2:
			UnknownC0.B = index;
			UnknownC0.D->Avatar->Unknown_786E60();
			Unknown_63D2F0(UnknownC0.B);
			if (RoomInfo->Find_NPC(UnknownC0.B)->Booth == StarbaseNPCClass::BOOTH_BLACKMKT) {
				UnknownB0->Unknown_69F540(Unknown_497300(((RObjGadgetClass *)Unknown13C[UnknownC0.B]->Unknown14)->RObj), "vb_blackmkt.vb_blackmkt-1", 0, false, false);
				Unknown1B0 = true;
			}
			UnknownB0->Unknown_69CBE0();
			UnknownC0.E = 0x20;
			Unknown_63E2B0();
			break;
		case 0x11:
			if (RoomInfo->Find_Facility(index)->Type == StarbaseFacilityClass::BROWSER && Unknown_7CA2D0()->Unknown34) {
				((LineDisplayClientClass *)Client)->As_Space_Client()->Unknown_74D990(0x13, "Cannot activate web terminal while using the embedded browser feature", 0);
				Unknown_63B5A0();
				return;
			}
			UnknownC0.A = index;
			UnknownC0.D->Avatar->Unknown_786E60();
			Unknown_63DCD0(UnknownC0.A);
			char name[32];
			if (!Room && ExtraBones) {
				_snprintf(name, sizeof(name), "cam_wterm%02d", index);
				Matrix3D tm = ExtraBones->Get_Bone_Transform(name);
				UnknownB0->Unknown_69EB20(tm, 1, true);
			} else {
				_snprintf(name, sizeof(name), "cam_terminal%02d", index);
				UnknownB0->Unknown_69E6E0(name, 1, false, 0);
			}
			Unknown1FC = true;
			UnknownB0->Unknown_69CBE0();
			UnknownC0.E = 0x10;
			Unknown_63E2B0();
			break;
		case 100:
			UnknownC0.C = index;
			((UIStarbaseUnknown1F8Class *)Unknown1F8.Peek())->Unknown_645CE0();
			Unknown1F8->Unknown_58();
			break;
	}
	Unknown_63B5A0();
}




























































































































































// 0x63C890
void UIStarbaseClass::Unknown_63C890()
{
	UIStarbaseUnknownD4TableClass::iterator it;
	for (it = UnknownD4.begin(); it != UnknownD4.end(); ++it) {
		UIStarbaseUnknownCCClass *savatar = it.Cur->Value;
		FAIL_IF(!savatar) continue;

		Message_Info("Sbase", "", __FILE__, __LINE__, "(%s) Clearing avatar %s from room %d", UnknownC0.D->Avatar->Desc.First_Name, savatar->Avatar->Desc.First_Name, Room);
		savatar->Avatar->Unknown_788EF0();
		savatar->Avatar->Unknown_7885D0(false);
		savatar->Avatar->Unknown_786E60();
		if (savatar->Unknown14) {
			Unknown64.Unknown_687150(savatar->Unknown14);
		}
		delete savatar;
	}
	UnknownD4.Table.clear();
}


































// NOT EXACT: 0x63CA50, functional (inline budget: the client calls the name string's assign out of line, ours inlines it; its client pointer in edi, the bool by a branch; O4)
// 0x63CA50 ('(%s) Adding avatar %s in room %d': a known avatar's entry made, its name tags and gadget, into +0xD4)
void UIStarbaseClass::Unknown_63CA50(int id, const Matrix3D &transform, int flags)
{
	InterfaceSortScopeClass scope(this);
	Unknown5BB320Class::Unknown_5C();
	SClient *sclient = ((LineDisplayClientClass *)Client)->As_Space_Client();
	UIStarbaseUnknownCCClass *savatar = new UIStarbaseUnknownCCClass;
	savatar->Unknown00 = id;
	savatar->Avatar = Create_NEW(sclient->Unknown_74C7B0(id));
	savatar->Avatar->Set_Transform(transform);
	savatar->Avatar->Unknown330 = transform;
	savatar->Avatar->Unknown_785F20(true);
	savatar->Avatar->Unknown_77E3D0(1);
	savatar->Avatar->BodyObj->Set_LOD_Bias(1.0f);
	UnknownB0->Unknown_6A1880(savatar->Unknown10, 3);
	ClientObjectClass *object = sclient->Find_Object(savatar->Unknown00);
	if (object && object->Unknown_6FFF00()) {
		object->Unknown_6FFFF0(savatar->Unknown24);
		Vector3 color(0.33f, 0.33f, 0.99f);
		object->Unknown_700670(color);
		savatar->Unknown10->Unknown494.Color = color;
		savatar->Unknown10->Unknown4A4 = savatar->Unknown10->Unknown494;
		savatar->Unknown34 = object->Unknown_6FFF50();
	} else {
		savatar->Unknown24 = "";
	}
	savatar->Unknown18 = flags;
	savatar->Avatar->Unknown_784F60((flags & 1) != 0);
	UnknownB0->Unknown_6A1880(savatar->NameTag, 0);
	savatar->Unknown_633180();
	UnknownD4[id] = savatar;
	savatar->Unknown14 = new ButtonGadgetClass(savatar->Avatar->BodyObj.Peek(), *UnknownB0->Unknown80, 5, 0);
	savatar->Unknown14->Unknown2C_0A = true;
	savatar->Unknown14->Set_Last_Sort_Group(true);
	Unknown64.Unknown_6870E0(savatar->Unknown14, id + 100, View, true, true);
	((SceneClass *)UnknownB0->Unknown70.Peek())->Add_Render_Object(savatar->Avatar.Peek());
	((UIStarbaseSceneClass *)UnknownB0->Unknown70.Peek())->Unknown_7621A0(savatar->Avatar.Peek());
	Unknown5BB320Class::Unknown_58();
	if (flags & 0x40) {
		savatar->Avatar->Unknown_7896B0(2);
	}
	Message_Info("Sbase", "", __FILE__, __LINE__, "(%s) Adding avatar %s in room %d", UnknownC0.D->Avatar->Desc.First_Name, savatar->Avatar->Desc.First_Name, Room);
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



















































enum { MAX_FACILITY = 4 };	// the facility gadgets +0xF8 / +0x108 (0x63DCD0's FAIL_IF 'facility < 0 || facility >= MAX_FACILITY')  // NAME 3: the FAIL_IF's












































// 0x63DCD0
void UIStarbaseClass::Unknown_63DCD0(int facility)
{
	FAIL_IF(facility < 0 || facility >= MAX_FACILITY) return;
	if (UnknownF8[facility] && Unknown108[facility]) {
		Matrix3D tm1 = Unknown108[facility]->Get_Transform();
		Matrix3D tm = UnknownC0.D->Avatar->Get_Transform();
		Vector3 diff = tm1.Get_Translation() - tm.Get_Translation();
		diff.Normalize();
		Vector3 z = tm.Get_Z_Vector();
		Vector3 cross = Vector3::Cross_Product(z, diff);
		tm.Set(diff, cross, z, tm.Get_Translation());
		UnknownC0.D->Avatar->Unknown_788700(tm);
	}
}


// NOT EXACT: 0x63DFE0, functional (frame: 0x50 against the client's 0x54, its three early-out blocks kept in place with their own epilogues where ours merges two at the end; O4)
// 0x63DFE0 (0x638390's frame: the symbol chat menu's chosen option for the avatar UnknownC0.C: 2 a chat to it, 3 a trade
// (ACTION 20), 4 ACTION 10)
void UIStarbaseClass::Unknown_63DFE0()
{
	if (Unknown1F8->Active) {
		if (UnknownB0->Unknown_69CCD0()) {
			Unknown1F8->Unknown_5C();
		}
		SClient *sclient = ((LineDisplayClientClass *)Client)->As_Space_Client();
		UIStarbaseUnknownD4TableClass::const_iterator it = UnknownD4.find(UnknownC0.C);
		UIStarbaseUnknownCCClass *savatar;
		if (it == UnknownD4.end() || (savatar = it.Cur->Value) == NULL) {
			sclient->Unknown_74D990(0x13, "The player you are interacting with has left the area.", 0);
			Unknown1F8->Unknown_5C();
			return;
		}
		if (savatar->Unknown_633010()) {
			Unknown1F8->Unknown_5C();
			return;
		}
		int option = ((UIStarbaseUnknown1F8Class *)Unknown1F8.Peek())->Unknown_645DC0();
		if (option) {
			switch (option) {
				case 4:
					sclient->Send_Packet(&ActionPacketClass(sclient->Get_Avatar_ID(), 10, savatar->Unknown00, 0));
					break;
				case 3:
					{
						SBaseViewClass *view = (SBaseViewClass *)sclient->Unknown135C;
						if (view && view->PTrade && !view->PTrade->Active) {
							sclient->Send_Packet(&ActionPacketClass(sclient->Get_Avatar_ID(), 0x14, savatar->Unknown00, 0));
						}
					}
					break;
				case 2:
					if (savatar->Avatar) {
						sclient->ChatPanel->Unknown_5658B0(savatar->Avatar->Desc.First_Name);
						sclient->Unknown_744E70(false);
					}
					break;
			}
			Unknown1F8->Unknown_5C();
			Unknown_641B00();
		}
	}
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


// 0x640150
void UIStarbaseClass::Unknown_640150()
{
	Matrix3D tm = InterfaceMesh->Peek_Object()->Get_Bone_Transform("safe_spot");
	tm.In_Place_Pre_Rotate_Z(Unknown_BDB1C0.Get_Float() * (WWMATH_PI * 2.0f));
	UnknownC0.D->Avatar->Set_Transform(tm);
	UnknownC0.D->Avatar->Unknown330 = tm;
	Unknown_63E2B0();
}
// NOT EXACT: 0x641C00, functional (frame: the client keeps the NPC position's y / z in two more stack copies, sub esp 0x98 against 0x8C; B1)
// 0x641C00
void UIStarbaseClass::Unknown_641C00()
{
	if (UnknownC0.D->Avatar) {
		for (int i = 0; i < 14; i++) {
			if (Unknown13C[i] && Unknown13C[i]->Avatar) {
				CharacterViewAvatarClass *npc = Unknown13C[i]->Avatar.Peek();
				Matrix3D tm = UnknownC0.D->Avatar->Unknown330;
				Vector3 pos = npc->Get_Transform().Get_Translation();
				Vector3 diff = tm.Get_Translation() - pos;
				float distance = diff.Length();
				if (distance < 5.0f) {
					float push = 5.0f - distance;
					diff.Normalize();
					tm.Set_Translation(tm.Get_Translation() + diff * push);
					UnknownC0.D->Avatar->Unknown330 = tm;
				}
			}
		}
	}
}
// 0x641EB0
bool UIStarbaseClass::Unknown_641EB0(UIStarbaseUnknownCCClass *savatar)
{
	if (savatar && ((LineDisplayClientClass *)Client)->As_Space_Client()->Unknown_74D570()) {
		DeltaCompoundClass *aux_data = ((LineDisplayClientClass *)Client)->As_Space_Client()->AuxData;
		if (aux_data) {
			NameClass name("GroupInfo");
			DeltaGroupInfoClass *group = (DeltaGroupInfoClass *)Unknown_Find_Field<DeltaCompoundClass>(aux_data, name);
			if (group && group->Unknown70) {
				for (int i = 0; i < 5; i++) {
					if (savatar->Unknown00 == ((DeltaGroupMemberClass *)group->Members.Children[i])->GameID.Value) {
						return(true);
					}
				}
			}
		}
	}
	return(false);
}
// 0x6435E0
void UIStarbaseClass::Unknown_6435E0()
{
	Matrix3D tm = UnknownC0.D->Avatar->Get_Transform();
	UnknownC0.D->Avatar->Remove();
	UnknownC0.D->Avatar = NULL;
	unsigned int id = ((LineDisplayClientClass *)Client)->AvatarID;
	UnknownC0.D->Avatar = Create_NEW((CharacterViewAvatarClass *)((LineDisplayClientClass *)Client)->As_Space_Client()->Unknown_74C7B0(id));
	Unknown_63BDB0(tm);
}

// 0x6418F0
void UIStarbaseClass::Unknown_6418F0()
{
	if (Unknown190) {
		Client->Unknown_44(Unknown190->Get_Unknown90());
		delete Unknown190;
		Unknown190 = NULL;
	}
	Unknown190 = (UIStarbaseUnknown190Class *)new ClientObjectClass((ClientClass *)Client, 0x29, ((ClientClass *)Client)->Next_Local_ID());
	((ClientObjectClass *)Unknown190)->Unknown98 = new Unknown7192A0Class(InterfaceMesh->Peek_Object(), ((ClientObjectClass *)Unknown190)->Unknown98, "ship");
	((ClientObjectClass *)Unknown190)->Unknown_6F33F0(Unknown184.Peek(), -1, 1.0f);
	SphereClass sphere = Unknown184->Get_Bounding_Sphere();
	((ClientObjectClass *)Unknown190)->Unknown_34(sphere.Radius);
	((ClientClass *)Client)->ClientClass::Add_Object((ClientObjectClass *)Unknown190);
}
// 0x6415B0
void UIStarbaseClass::Unknown_6415B0()
{
	SClient *client = ((LineDisplayClientClass *)Client)->As_Space_Client();
	RenderObjClass *ship = Unknown184.Peek();
	int column = UnknownB0->Info->Style;
	if (ship) {
		Matrix3D tm = InterfaceMesh->Peek_Object()->Get_Bone_Transform("ship");
		((SceneClass *)UnknownB0->Unknown70.Peek())->Add_Render_Object(ship);
		((UIStarbaseSceneClass *)UnknownB0->Unknown70.Peek())->Unknown_7621A0(ship);
		ship->Set_Transform(tm);
	}
	char anim[64];
	if (client->Unknown13C1) {
		if (RoomInfo->Unknown0A) {
			_snprintf(anim, sizeof(anim), "%sa2.%sa2-tele", Unknown_B8A978[column], Unknown_B8A978[column]);
		} else {
			_snprintf(anim, sizeof(anim), "%sa.%sa-tele", Unknown_B8A978[column], Unknown_B8A978[column]);
		}
		Unknown_6417D0();
	} else {
		if (RoomInfo->Unknown0A) {
			_snprintf(anim, sizeof(anim), "%sa2.%sa2-in", Unknown_B8A978[column], Unknown_B8A978[column]);
		} else {
			_snprintf(anim, sizeof(anim), "%sa.%sa-in", Unknown_B8A978[column], Unknown_B8A978[column]);
		}
		SoundLibraryClass::Play_2D_Sound(Unknown_B8A8B0[column], 2);
	}
	InterfaceMesh->Unknown_5B8EE0(0, 0, anim, false, 0, 0, 0, -1);
	InterfaceMesh->Unknown_5B9410();
	Unknown1FD = false;
	Unknown200 = true;
	client->Unknown13C1 = false;
}



















// NOT EXACT: 0x640F50, functional (the avatar dropped onto the room's model: a 0.5 box swept 50 down from its position at
// its height; the client copies the position through a Matrix3D temporary and keeps a smaller frame; B1)
// 0x640F50
void UIStarbaseClass::Unknown_640F50(CharacterViewAvatarClass *avatar)
{
	CastResultStruct result;
	Matrix3D tm(true);
	tm.Set_Translation(avatar->Unknown330.Get_Translation());


	tm[2][3] += avatar->Unknown_77D9F0(true);
	Vector3 move(0.0f, 0.0f, -50.0f);
	OBBoxClass box(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.5f, 0.5f, 0.5f));
	OBBoxClass world;
	OBBoxClass::Transform(tm, box, &world);
	OBBoxCollisionTestClass test(world, move, &result, COLLISION_TYPE_1);
	Matrix3D model_tm = Unknown_Model()->Get_Transform();
	Matrix3D inv;
	model_tm.Get_Orthogonal_Inverse(inv);
	OBBoxCollisionTestClass local_test(test, inv);
	if (Unknown_Model()->Cast_OBBox(local_test)) {
		Matrix3D m = avatar->Unknown330;
		m.Set_Translation(tm.Get_Translation() + move * result.Fraction);
		avatar->Unknown330 = m;
	}
	avatar->Unknown23C = result.SurfaceType;
}
































































// NOT EXACT: 0x63FCB0, functional (frame: sub esp 0x68 against the client's 0xBC, the door and avatar positions in other stack copies; O4)
// 0x63FCB0 (0x638390's frame: the own avatar within 5 of a door's 'cam_door_*_out' bone leaves the room through it)
void UIStarbaseClass::Unknown_63FCB0()
{
	for (int i = 0; i < 4; i++) {
		int key = Unknown_B8AABC[Room * 4 + i];
		if (key >= 0 && UnknownB0->Info->Find_Room(key)) {
			char name[32];
			_snprintf(name, sizeof(name), "%s", Unknown_B8AA7C[Room * 4 + i]);
			if (strcmp(name, "") != 0) {
				Vector3 door;
				Unknown_Model()->Get_Bone_Transform(name).Get_Translation(&door);
				Vector3 pos;
				UnknownC0.D->Avatar->Get_Transform().Get_Translation(&pos);
				Vector2 diff = Vector2(pos.X, pos.Y) - Vector2(door.X, door.Y);
				if (diff.Length() < 5.0f) {
					int room = Unknown_B8AB3C[Room * 4 + i];
					UnknownB0->Unknown_69CBE0();
					UnknownC0.E = 4;
					Unknown_63E2B0();
					Unknown_63E4B0(room, Room);
					UnknownB0->Unknown_6A23E0(Unknown214, Unknown215, Unknown216, Unknown217, Unknown1B3);
					UnknownB0->Unknown_69F460(room);
					Unknown_641B00();
					Unknown21C = true;
					((SBaseViewClass *)View->Unknown_30())->Unknown1E0 = Vector3(diff.X * 0.9f, diff.Y * 0.9f, 0);
				}
			}
		}
	}
}










































































































































// NOT EXACT: 0x640360, functional (inline budget: the client copies the 'ship' bone transform through ??_H, the out-of-line Vector4 constructor 0x468E70; ours inlines it; B1)
// 0x640360
void UIStarbaseClass::Unknown_640360()
{
	if (Unknown18C) {
		Unknown5BB320Class::Unknown_5C();
		if (Unknown180) {
			Unknown64.Unknown_687150(Unknown180);
			GadgetClass::Destroy(Unknown180);
			Unknown180 = NULL;
		}
		if (Unknown184) {
			Unknown184->Remove();
			Unknown184 = NULL;
		}
		ClientObjectClass *ship_object = ((ClientClass *)Client)->Get_Ship();
		if (ship_object) {
			Unknown184 = Create_Get(ship_object->Unknown_6F3BC0());
			Unknown_496DB0(Unknown184.Peek());
			Unknown184->Set_LOD_Bias(100.0f);
			RenderObjClass *ship = Unknown184.Peek();
			Matrix3D tm = InterfaceMesh->Peek_Object()->Get_Bone_Transform("ship");
			((SceneClass *)UnknownB0->Unknown70.Peek())->Add_Render_Object(ship);
			((UIStarbaseSceneClass *)UnknownB0->Unknown70.Peek())->Unknown_7621A0(ship);
			ship->Set_Transform(tm);
			Play_State_Animation(Unknown184.Peek(), 1, 1.0f);
			RefCountPtr<RenderObjClass> extents;
			if (ExtraBones) {
				extents = Create_Get(ExtraBones->Get_Sub_Object_By_Name("ship_extents00")); } else {
				G_ASSERT(ExtraBones);
				extents = Unknown184;
			}
			Unknown180 = new ButtonGadgetClass(extents.Peek(), Layer, 5, 3);
			Unknown180->Set_Last_Sort_Group(true);
			Unknown180->Unknown2C_0A = true;
			Unknown64.Unknown_6870E0(Unknown180, 1, View, true, true);
		}
		Unknown5BB320Class::Unknown_58();
		Unknown18C = false;
	} else if (Unknown184) {
		Matrix3D tm = InterfaceMesh->Peek_Object()->Get_Bone_Transform("ship");
		Unknown184->Set_Transform(tm);
	}
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








// NOT EXACT: 0x642740, functional (B1): a lounge NPC's wandering avatar from '<race>starbase_ambient.ini', its section the
// NPC's first name: the walk points Location%02d (OffsetBoneName / DestBoneName bones of ExtraBones), the idle actions
// Action%02d with their Chance%02d, ExtraName%02d and ExtraAnim%02d
// 0x642740
Unknown78A910AvatarClass *UIStarbaseClass::Unknown_642740(StarbaseNPCClass *npc)
{
	if (!ExtraBones) {
		return(NULL);
	}
	char file_name[64];
	_snprintf(file_name, sizeof(file_name), "%sstarbase_ambient.ini", Unknown_B8A950[UnknownB0->Info->Style]);
	INIClass ini(file_name);
	if (!ini.Is_Loaded()) {
		return(NULL);
	}
	char section[32];
	_snprintf(section, sizeof(section), "%s", npc->Desc.First_Name);
	std::string offset_bone("");
	std::string dest_bone;
	dest_bone = "";
	StringClass str;
	ini.Get_String(str, section, "OffsetBoneName", "");
	if (!str.Is_Empty()) {
		offset_bone = str;
	}
	ini.Get_String(str, section, "DestBoneName", "");
	if (!str.Is_Empty()) {
		dest_bone = str;
	}
	Unknown78A910AvatarClass *avatar = new Unknown78A910AvatarClass(npc->Unknown1C, &npc->Desc, ((LineDisplayClientClass *)Client)->As_Space_Client()->EnvironmentMapManager);
	avatar->Unknown3B9 = ini.Find_Entry(section, "Sequential") ? ini.Get_Bool(section, "Sequential", false) : false;
	avatar->Unknown_78B530(ini.Get_Int(section, "MinWait", 5), ini.Get_Int(section, "MaxWait", 15), ini.Get_Int(section, "RelocateChance", 33), ini.Get_Int(section, "ActionChance", 67));
	Matrix3D tm(true);
	Matrix3D start(true);
	char entry[32];
	char bone[32];
	int i;
	for (i = 0; ; i++) {
		_snprintf(entry, sizeof(entry), "Location%02d", i);
		if (!ini.Find_Entry(section, entry)) {
			break;
		}
		int location = ini.Get_Int(section, entry, 0);
		if (offset_bone.length()) {
			_snprintf(bone, sizeof(bone), "%s%02d", offset_bone.c_str(), location);
			tm = ExtraBones->Get_Bone_Transform(bone);
			avatar->Unknown_78B010(tm);
		}
		_snprintf(bone, sizeof(bone), "%s%02d", dest_bone.c_str(), location);
		tm = ExtraBones->Get_Bone_Transform(bone);
		avatar->Unknown_78B290(tm);
		if (i == 0) {
			start = tm;
		}
	}
	int top = -1;
	int bottom = 0;
	for (i = 0; ; i++) {
		_snprintf(bone, sizeof(bone), "Action%02d", i);
		if (!ini.Find_Entry(section, bone)) {
			break;
		}
		int anim = ini.Get_Int(section, bone, 200);
		_snprintf(bone, sizeof(bone), "Chance%02d", i);
		top += ini.Get_Int(section, bone, 1);
		std::string extra_name("");
		std::string extra_anim("");
		_snprintf(bone, sizeof(bone), "ExtraName%02d", i);
		ini.Get_String(str, section, bone, "");
		if (!str.Is_Empty()) {
			extra_name = str;
		}
		_snprintf(bone, sizeof(bone), "ExtraAnim%02d", i);
		ini.Get_String(str, section, bone, "");
		if (!str.Is_Empty()) {
			extra_anim = str;
		}
		avatar->Unknown_78AD20(anim, bottom, top, extra_anim.c_str(), extra_name.c_str());
		bottom = top + 1;
	}
	avatar->Set_Transform(start);
	avatar->Unknown330 = start;
	avatar->Unknown_785F20(true);
	avatar->Unknown_77E3D0(1);
	avatar->BodyObj->Set_LOD_Bias(1.0f);
	avatar->Unknown_784F60(false);
	return(avatar);
}












// NOT EXACT: 0x63E930, functional (shape: the client builds the box and matrices through ??_H temporaries and keeps other x87 copies; as 0x640F50; O4)
// 0x63E930 (0x63B310: the camera transform pulled in against the room's model: a 0.5 box swept from above the avatar's
// head to the camera; on a hit the camera halfway between the hit and the last camera position +0x1BC, kept there)
Matrix3D UIStarbaseClass::Unknown_63E930(const Matrix3D &tm)
{
	CastResultStruct result;
	Matrix3D start(true);
	CharacterViewAvatarClass *avatar = UnknownC0.D->Avatar.Peek();
	start.Set_Translation(avatar->Get_Transform().Get_Translation());
	start[2][3] += avatar->Unknown_77D9F0(true) + 1.0f;
	Vector3 move = tm.Get_Translation() - start.Get_Translation();
	OBBoxClass box(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.5f, 0.5f, 0.5f));
	OBBoxClass world;
	OBBoxClass::Transform(start, box, &world);
	OBBoxCollisionTestClass test(world, move, &result, COLLISION_TYPE_1);
	Matrix3D model_tm = Unknown_Model()->Get_Transform();
	Matrix3D inv;
	model_tm.Get_Orthogonal_Inverse(inv);
	OBBoxCollisionTestClass local_test(test, inv);
	Matrix3D out = tm;
	if (Unknown_Model()->Cast_OBBox(local_test)) {
		Vector3 hit = start.Get_Translation() + move * result.Fraction;
		out.Set_Translation((hit + ((Matrix3D *)Unknown1BC)->Get_Translation()) * 0.5f);
	}
	*(Matrix3D *)Unknown1BC = out;
	return(out);
}




































































// 0x6417D0
void UIStarbaseClass::Unknown_6417D0()
{
	Unknown_6418F0();
	Vector3 hsv_shift(0.0f, 0.0f, 0.0f);
	EffectObjectClass *effect = Unknown_6E9490(0x30E, ((ClientClass *)Client)->Next_Local_ID(), 0, ((ClientClass *)Client)->Unknown10BC, (ClientClass *)Client, 1.0f, hsv_shift, 1.0f, -1, 0);
	FAIL_IF(!effect) return;
	UnknownB00060EffectClass *source = effect->Unknown_10();
	if (source) {
		source->Set_Source_Object_ID(Unknown190->Get_Unknown90());
	}
	SBaseViewClass *view = (SBaseViewClass *)((LineDisplayClientClass *)Client)->As_Space_Client()->Unknown135C;
	((ClientClass *)Client)->Add_Effect(effect, (int)view->Unknown_5C(), (int)view->Unknown6C.Peek());
	Unknown220 = effect->ID;
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
			((FontSys *)savatar->NameTag.Peek())->Change_Text("\".....\"", 0);
			FontSys *tag = (FontSys *)savatar->NameTag.Peek();
			tag->Unknown494.Color = color;
			tag->Unknown4A4 = tag->Unknown494;
			((FontSys *)savatar->NameTag.Peek())->Unknown_6D8A80(true);
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
// 0x6439B0 (SGI's _Hashtable_const_iterator(const iterator &), out of line)
UIStarbaseUnknownD4TableClass::const_iterator::const_iterator(const iterator &it) :
	Cur(it.Cur),
	Table(it.Table)
{
}

// @0x00643C20
// 0x643C20 (SGI's hashtable(n, hf, eql) with _M_initialize_buckets inline: the next prime of the table, the vector's
// reserve and insert(end(), n, 0) 0x644740)
UIStarbaseUnknownD4TableClass::UIStarbaseUnknownD4TableClass(unsigned int n, const UIStarbaseUnknownD4Hash &hf, const UIStarbaseUnknownD4Equal &eql) :
	Hash(hf),
	Equal(eql),
	GetKey(UIStarbaseUnknownD4GetKey()),
	Buckets(),
	NumElements(0)
{
	Initialize_Buckets(n);
}

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

// 0x644A20 (SGI's hashtable::find_or_insert, out of line)
UIStarbaseUnknownD4TableClass::ValueType &UIStarbaseUnknownD4TableClass::find_or_insert(const ValueType &obj)
{
	resize(NumElements + 1);
	unsigned int n = Bucket_Of_Key(obj.first);
	NodeStruct *first = Buckets[n];
	for (NodeStruct *cur = first; cur; cur = cur->Next) {
		if (cur->Key == obj.first) {
			return(cur->Get_Value());
		}
	}
	NodeStruct *tmp = (NodeStruct *)operator new(sizeof(NodeStruct));
	tmp->Next = 0;
	new ((void *)&tmp->Get_Value()) ValueType(obj);
	tmp->Next = first;
	Buckets[n] = tmp;
	++NumElements;
	return(tmp->Get_Value());
}

// 0x644C10 (SGI's hashtable::resize, out of line)
void UIStarbaseUnknownD4TableClass::resize(unsigned int hint)
{
	const unsigned int old_n = Buckets.size();
	if (hint > old_n) {
		const unsigned int n = Next_Size(hint);
		if (n > old_n) {
			std::vector<NodeStruct *, std::_Bucket_allocator<NodeStruct *> > tmp(n, (NodeStruct *)0);
			for (unsigned int bucket = 0; bucket < old_n; ++bucket) {
				NodeStruct *first = Buckets[bucket];
				while (first) {
					unsigned int new_bucket = Bucket_Of_Key(first->Key, n);
					Buckets[bucket] = first->Next;
					first->Next = tmp[new_bucket];
					tmp[new_bucket] = first;
					first = Buckets[bucket];
				}
			}
			Buckets.swap(tmp);
		}
	}
}

#pragma auto_inline(on)

// The symbol chat menu (0x644E70-0x645E80, the starbase interface's +0x1F8)
// 0x644E70
UIStarbaseSymbolChatOptionStruct::~UIStarbaseSymbolChatOptionStruct()
{
	if (Window) {
		Window->Remove_All(false);
		Window = NULL;
	}
	if (Text) {
		Text = NULL;
	}
	if (Gadget) {
		GadgetClass::Destroy(Gadget);
		Gadget = NULL;
	}
}

// NOT EXACT: 0x644F80, functional (register / frame: the client's EH state in bl and its RefCountPtr temporary in edi, one stack slot less; B1)
// 0x644F80
UIStarbaseUnknown1F8Class::UIStarbaseUnknown1F8Class(SClient *sclient) :
	InterfaceClass((LineDisplayClientClass *)sclient),
	Unknown84(0),
	Window(NULL),
	Section(NULL),
	Gadget(NULL),
	Options(0)
{
	InterfaceSortScopeClass scope(this);
	Unknown_BE67BC->Load("fc_symbolchat.ini");
	Window = Create_NEW(new DynamicWindowClass("SYMBOL_CHAT_WINDOW", 3));
	Window->Set_Level(8);
	Section = Create_NEW(new DynamicWindowClass("SYMBOL_CHAT_SECTION", 1));
	Window->Add_Object(Section.Peek(), 0);
	Gadget = new DynamicWindowGadgetClass(Window, 0x55, 3, false);
	Unknown64.Unknown_6870E0(Gadget, 0, View, true, true);
	Gadget->Unknown_65FFA0(-1);
}

// NOT EXACT: 0x6452C0, functional (inline budget: the client calls the options' vector clear 0x645E30 out of line, ours inlines it; B1)
// 0x6452C0
UIStarbaseUnknown1F8Class::~UIStarbaseUnknown1F8Class()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
		Window->Remove_From_Scene();
	}
	for (std::vector<UIStarbaseSymbolChatOptionStruct *>::iterator it = Options.begin(); it != Options.end(); it++) {
		UIStarbaseSymbolChatOptionStruct *option = *it;
		Unknown64.Unknown_687150(option->Gadget);
		delete option;
	}
	Options.clear();
	if (Section) {
		Section->Remove_All(false);
	}
	if (Gadget) {
		Unknown64.Unknown_687150(Gadget);
		GadgetClass::Destroy(Gadget);
		Gadget = NULL;
	}
	if (Section) {
		Section->Remove_All(false);
		Section = NULL;
	}
	if (Window) {
		Window->Remove_All(false);
		Window = NULL;
	}
	Unknown_BE67BC->Unload("fc_symbolchat.ini");
}

// NOT EXACT: 0x645510, functional (frame: two more stack slots in ours, every local 8 bytes up; the client's order otherwise; B1)
// 0x645510
void UIStarbaseUnknown1F8Class::Unknown_645510()
{
	InterfaceSortScopeClass scope(this);
	SClient *client = ((LineDisplayClientClass *)Client)->As_Space_Client();
	for (int i = 1; i <= 4; i++) {
		char name[32];
		bool usable = false;
		switch (i) {
			case 1:
				_snprintf(name, sizeof(name), "Cancel");
				usable = true;
				break;
			case 2:
				_snprintf(name, sizeof(name), "Chat");
				usable = true;
				break;
			case 3:
				_snprintf(name, sizeof(name), "Trade");
				usable = true;
				break;
			case 4:
				_snprintf(name, sizeof(name), "Group Invite");
				if (client->Unknown_74D690() || !client->Unknown_74D570()) {
					usable = true;
				}
				break;
		}
		RefCountPtr<FontSys> text;
		if (usable) {
			text = Create_NEW(new FontSys("Symbol_Chat_Default", NULL, NULL));
		} else {
			text = Create_NEW(new FontSys("Symbol_Chat_Unusable", NULL, NULL));
		}
		text->Change_Text(name, 0);
		UIStarbaseSymbolChatOptionStruct *option = new UIStarbaseSymbolChatOptionStruct;
		option->Window = Create_NEW(new DynamicWindowClass("SYMBOL_CHAT_OPTION", 1));
		Section->Add_Object(option->Window.Peek(), i);
		option->Text = Create_NEW(new Unknown51C800Class(text.Peek(), 6));
		option->Window->Add_Object(option->Text.Peek(), 0);
		option->Gadget = new DynamicWindowGadgetClass(option->Window, 0x55, 3, false);
		Unknown64.Unknown_6870E0(option->Gadget, i, View, true, true);
		option->Gadget->Unknown_65FFA0(-1);
		option->Gadget->Unknown_40(usable);
		Options.push_back(option);
	}
}

// 0x645A60
void UIStarbaseUnknown1F8Class::Unknown_645A60()
{
	for (std::vector<UIStarbaseSymbolChatOptionStruct *>::iterator it = Options.begin(); it != Options.end(); it++) {
		UIStarbaseSymbolChatOptionStruct *option = *it;
		Unknown64.Unknown_687150(option->Gadget);
		delete option;
	}
	Options.clear();
	if (Section) {
		Section->Remove_All(false);
	}
}

// 0x645B20
void UIStarbaseUnknown1F8Class::Unknown_58()
{
	if (!Active) {
		InterfaceSortScopeClass scope(this);
		float x;
		float y;
		Unknown_42ECF0(&x, &y);
		x = x * 640.0f - 320.0f;
		y = 240.0f - y * 480.0f;
		Window->Set_Center(Vector2(x, y));
		Window->Add_To_Scene((SceneClass *)View->Unknown_50());
		InterfaceClass::Unknown_58();
		SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
	}
}

// 0x645C50
void UIStarbaseUnknown1F8Class::Unknown_5C()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
		Window->Remove_From_Scene();
	}
}

// 0x645C80
bool UIStarbaseUnknown1F8Class::Unknown_68(int key)
{
	if (key) {
		Unknown84 = key;
		SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
		return(true);
	}
	return(false);
}

// 0x645CC0
void UIStarbaseUnknown1F8Class::Unknown_645CC0()
{
}

#pragma auto_inline(off)
// 0x645CE0
void UIStarbaseUnknown1F8Class::Unknown_645CE0()
{
	if (!Active) {
		Unknown_645A60();
		Unknown_645510();
	}
}

// 0x645DC0
int UIStarbaseUnknown1F8Class::Unknown_645DC0()
{
	int option = Unknown84;
	Unknown84 = 0;
	return(option);
}
#pragma auto_inline(on)

// The starbase lights panel (0x646170-0x648238): moved after the file's other functions, as the client's order (SU1)
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
	void Unknown_648270();								// 0x648270 (no caller: the pad points into the lights manager)
	void Unknown_646F70(bool force);					// 0x646F70 (the sliders into the current light; its texts)  // NAME 1 [force]: neutral
	void Unknown_6474C0(int index);						// 0x6474C0 (a light chosen; mode 1: the ambient light)  // NAME 1 [index]: neutral
	void Unknown_647B30();								// 0x647B30 (the colour pad dragged)
	void Unknown_647F30(Vector3 point);					// 0x647F30 (the pull placed at a pad point, the colour read)  // NAME 1 [point]: neutral




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












// 0x646360
void UIStarbaseLightsClass::Unknown_54()
{
	if (Unknown_98("light02a", "fa_light02a.ini")) {
		Unknown_646940();
		int i;
		for (i = 0; i < 6; i++) {
			UnknownA0[i] = Add_Button(i + 1, Unknown_B8B864[i], Unknown_B8B880[i]);
		}
		for (i = 0; i < 4; i++) {
			Slider[i] = Add_Slider(i + 8, 0);
			Slider[i]->Unknown_66E460(InterfaceMesh, true, i);
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
		RefCountPtr<RenderObjClass> palette = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name("COLOR_PALETTE"));
		Unknown_496AE0(palette.Peek(), false);
		UnknownD8->Unknown00 = new Unknown_671300Class(palette.Peek(), 0, 0, 0, 0);
		UnknownD8->Unknown00->Unknown2C_0A = true;
		UnknownD8->Unknown04 = InterfaceMesh->Get_Sub_Object_By_Name("PULL_COLOR");
		Unknown_496AE0(UnknownD8->Unknown04, true);
		UnknownD8->Unknown_5859A0();
		UnknownD8->Unknown00->Unknown_65FFA0(-1);
		Unknown64.Unknown_6870E0(UnknownD8->Unknown00, 0xD, View, true, false);
		Add_Back_Gadget(1, Vector3(0.0f, 0.0f, 0.0f));
		Unknown4C.Add(KernelEventClass(0x1B), (void *)1);
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
			Closed = 1;
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
		RefCountPtr<FontSys> text = Unknown_BE67C4->Unknown_6D2700("near_light02a", InterfaceMesh);
		_snprintf(buffer, sizeof(buffer), "Near %d", (int)Slider[0]->Get_Value());
		text->Change_Text(buffer, 0);
		text = Unknown_BE67C4->Unknown_6D2700("far_light02a", InterfaceMesh);
		_snprintf(buffer, sizeof(buffer), "Far %d", (int)Slider[1]->Get_Value());
		text->Change_Text(buffer, 0);
		text = Unknown_BE67C4->Unknown_6D2700("Int_light02a", InterfaceMesh);
		_snprintf(buffer, sizeof(buffer), "Int %0.2f", Slider[2]->Get_Value());
		text->Change_Text(buffer, 0);
		text = Unknown_BE67C4->Unknown_6D2700("Fill_light02a", InterfaceMesh);
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
		RefCountPtr<FontSys> text = Unknown_BE67C4->Unknown_6D2700("GO_light02a", InterfaceMesh);
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
		RefCountPtr<FontSys> text = Unknown_BE67C4->Unknown_6D2700("GO_light02a", InterfaceMesh);
		char buffer[64];
		_snprintf(buffer, sizeof(buffer), "Ambient Light", Current);
		text->Change_Text(buffer, 0);
		text = Unknown_BE67C4->Unknown_6D2700("near_light02a", InterfaceMesh);
		text->Change_Text("", 0);
		text = Unknown_BE67C4->Unknown_6D2700("far_light02a", InterfaceMesh);
		text->Change_Text("", 0);
		text = Unknown_BE67C4->Unknown_6D2700("Int_light02a", InterfaceMesh);
		text->Change_Text("", 0);
		text = Unknown_BE67C4->Unknown_6D2700("Fill_light02a", InterfaceMesh);
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
		int bone = InterfaceMesh->Get_Sub_Object_Bone_Index(UnknownD8->Unknown04);
		InterfaceMesh->Capture_Bone(bone);
		Matrix3D tm = UnknownD8->Unknown04->Get_Transform();
		tm.Set_X_Translation(x - box.Center.X);
		tm.Set_Y_Translation(y - box.Center.Y);
		tm[2][3] = 0.0f;
		InterfaceMesh->Control_Bone(bone, tm, false);
		Vector2 point;
		point.X = (x - (box.Center.X - box.Extent.X)) / (box.Extent.X + box.Extent.X);
		point.Y = (y - (box.Center.Y - box.Extent.Y)) / (box.Extent.Y + box.Extent.Y);
		if (UnknownE0 == 0) {
			RObjGadgetClass *area = UnknownD8->Unknown00->As_RObjGadget();
			Unknown_496C00(area->RObj, point.X, 1.0f - point.Y, &Color[Current]);
			Position[Current].Set(point.X, point.Y, 0.0f);
			Lights[Current]->Set_Diffuse(Color[Current]);
		} else {
			RObjGadgetClass *area = UnknownD8->Unknown00->As_RObjGadget();
			Unknown_496C00(area->RObj, point.X, 1.0f - point.Y, &AmbientColor);
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
	int bone = InterfaceMesh->Get_Sub_Object_Bone_Index(UnknownD8->Unknown04);
	InterfaceMesh->Capture_Bone(bone);
	Matrix3D tm = UnknownD8->Unknown04->Get_Transform();
	tm.Set_X_Translation((point.X * width) + left - box.Center.X);
	tm.Set_Y_Translation((point.Y * height) + top - box.Center.Y);
	tm[2][3] = 0.0f;
	InterfaceMesh->Control_Bone(bone, tm, false);
	if (UnknownE0 == 0) {
		RObjGadgetClass *area = UnknownD8->Unknown00->As_RObjGadget();
		Unknown_496C00(area->RObj, point.X, 1.0f - point.Y, &Color[Current]);
		Position[Current].Set(point.X, point.Y, 0.0f);
	} else {
		RObjGadgetClass *area = UnknownD8->Unknown00->As_RObjGadget();
		Unknown_496C00(area->RObj, point.X, 1.0f - point.Y, &AmbientColor);
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

// 0x648270
void UIStarbaseLightsClass::Unknown_648270()
{
	for (int i = 0; i < UnknownF4; i++) {
		Unknown_BE82D0->Unknown_764C30(UnknownE4, UnknownE8, UnknownEC, i, Position[i]);
	}
	Unknown_BE82D0->Unknown_764C30(UnknownE4, UnknownE8, UnknownEC, 1000, AmbientPosition);
}
