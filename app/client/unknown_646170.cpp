// Earth & Beyond client (reconstruction): app\client\unknown_646170.cpp, PROVISIONAL file name (D137). Notes at the end of the file.
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
#include "camera.h"	// CameraClass, the view's +0x6C (0x63F490)
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

// The starbase view's +0x70 scene: unknown_761c80.h's Unknown761C80Class. The starbase interface gives its avatars
// and ships a light environment of their own (Add_Light_Environment 0x7621A0) and sets the environments' fill
// intensity and ambient (Set_Fill_Intensity 0x762920, Set_Environment_Ambient 0x762940) from the room's lights.
// (These lines keep the file's line count: the asserts below sit on the client's __LINE__.)

#include "unknown_761c80.h"



// The starbase avatar (unknown_78a910.cpp's Unknown78A910AvatarClass)
class Unknown78A910AvatarClass : public CharacterViewAvatarClass	// NAME 1: neutral (D10)
{
public:
	void Unknown_78B570(bool b);						// 0x78B570  // NAME 1 [b]: neutral

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

#include "co.h"

#include "postate.h"
#include "coltest.h"
#include "random.h"
#include "..\shared\auxdata\auxdata.h"
extern RandomClass Unknown_BDBB84; extern Random4Class Unknown_BDB1C0;	// NAME 1 [Unknown_BDB1C0]: 0xBDB1C0, as camctrl_dock.cpp's
// The docking sounds by the starbase's column +0x18 (0xB8A8B0: 0x6415B0)
// What 0x634140 news for the starbase interface's +0xB8: UIStarbaseGadget6B6050Class (0x44 bytes, constructor 0x6B6050,
// table 0xAFE174), in its own file unknown_6b6050.cpp (the client's object sits between viewbttn.cpp and wnd_file.cpp)
#include "unknown_6b6050.h"
//
//
//
//
//
#include "dynamic_window_class.h"
#include "gd_dynamicwindow.h"
#include "unknown_51c800.h"
// What 0x634140 news for the starbase interface's +0x1F8 (0xA4 bytes, constructor 0x644F80 with the space client, table
// 0xAF84CC, its other sides 0xAF84C0 / 0xAF84B4): the symbol chat menu ('fc_symbolchat.ini'; SYMBOL_CHAT_WINDOW with one
// SYMBOL_CHAT_SECTION, an option a row: Cancel, Chat, Trade, Group Invite); interim placement in this file, D137
struct UIStarbaseSymbolChatOptionStruct	// NAME 1: neutral (D10): a row of the menu (0x0C bytes: 0x645510 news it)
{
	UIStarbaseSymbolChatOptionStruct();								// 0x644E50 (uncalled out of line: 0x645510 inlines it)
	~UIStarbaseSymbolChatOptionStruct();				// 0x644E70
	RefCountPtr<DynamicWindowClass> Window;				// +0x00 (SYMBOL_CHAT_OPTION)
	RefCountPtr<Unknown51C800Class> Text;				// +0x04 (the option's name)
	GadgetClass *Gadget;								// +0x08 (a DynamicWindowGadgetClass over Window; its id the option)
};
// (UIStarbaseUnknown1F8Class: sbaseview.h, for the starbase interface's RefCountPtr at +0x1F8; these lines kept blank)




















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
		Starbase = (UIStarbaseClass *)((SBaseViewClass *)((LineDisplayClientClass *)Client)->As_Space_Client()->Unknown135C)->Get_Starbase().Peek();
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
		if (Slider[i]->Is_Enabled()) {
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
			if (Slider[i]->Is_Enabled()) {
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
		text->Change_Text(" ", 0);
		text = Unknown_BE67C4->Unknown_6D2700("far_light02a", InterfaceMesh);
		text->Change_Text(" ", 0);
		text = Unknown_BE67C4->Unknown_6D2700("Int_light02a", InterfaceMesh);
		text->Change_Text(" ", 0);
		text = Unknown_BE67C4->Unknown_6D2700("Fill_light02a", InterfaceMesh);
		text->Change_Text(" ", 0);
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


/*
**	Notes
**
**	RECONSTRUCTED (S2, run 2026-11-18): client 0x646170-0x6483C0, an object of its own by the CRT initialiser table
**	(SG): the starbase lights panel (UIStarbaseLightsClass 0x646170-0x648270, its vector<LightClass *> instances
**	0x648310 / 0x648340), then <string>'s guarded initialiser 0x648380 (entry 419, the object's only one) and the
**	class's vector deleting destructors 0x6483B0 / 0x6483C0; ui_string_list.cpp follows. The preamble is
**	ui_starbase.cpp's without aux_group.h. No assert or string names the file: PROVISIONAL (D137).
*/
