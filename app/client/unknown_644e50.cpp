// Earth & Beyond client (reconstruction): app\client\unknown_644e50.cpp, PROVISIONAL file name (D137). Notes at the end of the file.
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
#include "..\shared\auth_product.h"
// The client at InterfaceClass +0x40, as ui_chat_panel.cpp declares it
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SpaceClientClass *As_Space_Client();					// +0x18 (SpaceClientClass's returns itself)  // NAME 3: ui_manu.cpp's FAIL_IF's
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
class Unknown78A910AvatarClass : public AvatarClass	// NAME 1: neutral (D10)
{
public:
	void Unknown_78B570(bool b);						// 0x78B570  // NAME 1 [b]: neutral

	Unknown78A910AvatarClass(int a, AvatarDescriptionClass *desc, EnvironmentMapManagerClass *env_maps);	// 0x78A910 (0x642740: the NPC's +0x1C and description)  // NAME 1 [a]: neutral
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
// (ui_starbase.cpp's auxdata.h left out: the client's object opens with no PermStringInit)
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




















// The symbol chat menu (0x644E50-0x645E80, the starbase interface's +0x1F8)
// @0x00644E50
UIStarbaseSymbolChatOptionStruct::UIStarbaseSymbolChatOptionStruct() :
	Window(NULL),
	Text(NULL),
	Gadget(NULL)
{
}

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

// The gadget made over the window's raw pointer (Window.Peek()), as the chat reply list 0x56E2A0
// 0x644F80
UIStarbaseUnknown1F8Class::UIStarbaseUnknown1F8Class(SpaceClientClass *sclient) :
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
	Gadget = new DynamicWindowGadgetClass(Window.Peek(), 0x55, 3, false);
	Unknown64.Unknown_6870E0(Gadget, 0, View, true, true);
	Gadget->Unknown_65FFA0(-1);
}

// The options removed through Unknown_645A60, as the chat reply list's destructor 0x56E5A0
// 0x6452C0
UIStarbaseUnknown1F8Class::~UIStarbaseUnknown1F8Class()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
		Window->Remove_From_Scene();
	}
	Unknown_645A60();








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

// 0x645510 (the four options made, those the player cannot use greyed; the loop's bound is i < 5: cmp esi, 5 / jl)
// 0x645510
void UIStarbaseUnknown1F8Class::Unknown_645510()
{
	InterfaceSortScopeClass scope(this);
	SpaceClientClass *client = ((LineDisplayClientClass *)Client)->As_Space_Client();
	for (int i = 1; i < 5; i++) {
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
		option->Gadget = new DynamicWindowGadgetClass(option->Window.Peek(), 0x55, 3, false);
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



/*
**	Notes
**
**	RECONSTRUCTED (S2, run 2026-11-18): client 0x644E50-0x646160, an object of its own by the CRT initialiser table
**	(SG): the starbase interface's symbol chat menu (its +0x1F8: UIStarbaseSymbolChatOptionStruct,
**	UIStarbaseUnknown1F8Class 0x644F80-0x645DC0, their vector instances 0x645DF0-0x645E80), then <string>'s guarded
**	initialiser 0x646120 (entry 418, the object's only one) and the class's vector deleting destructors 0x646150 /
**	0x646160. ui_starbase.cpp's object ends with its own guard 0x644DE0 (entry 417); the lights panel
**	(unknown_646170.cpp) follows with 0x648380 (entry 419). The preamble is ui_starbase.cpp's (its includes and
**	declarations, no definitions) without aux_group.h, which would define and number the Delta globals the client's
**	object does not. No assert or string names the file: PROVISIONAL (D137); the client links objects in name order,
**	so its name sorts after ui_starbase.cpp.
*/
