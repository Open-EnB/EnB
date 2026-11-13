// Earth & Beyond client (reconstruction): app\Client\ui_class.cpp. Notes at the end of the file.
#include <stdio.h>
#include <string.h>
#include <vector>
#include <string>
#include "sclient.h"
#include "tasks\tsk_login.h"
#include "rendobj.h"
#include "assetmgr.h"
#include "camera.h"
#include "layer.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\shared\g_assert.h"
#include "..\audio\soundlib.h"
#include "dx8renderer.h"
#include "vector4.h"
#include "tutorial_manager.h"
#include "ui_class.h"
#include "charview.h"
#include "gadget.h"
#include "unknown_675120.h"
#include "local_interface_sort.h"
#include "tooltip.h"
#include "unknown_signalbase.h"

#include "fontmanager.h"

extern FontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4

class EnvironmentMapManagerClass { public: void Unknown_760770(int base_asset, bool a); };

// The control keys event the constructor registers (0x10 bytes) and what InterfaceClass +0x4C is (as ui_avsel.cpp's)
#include "control_key_list.h"	// ControlKeyListClass: InterfaceClass +0x4C (Add 0x721E60, Remove 0x721EF0), keyed by kernel.h's KernelEventClass










#include "gd_slider.h"
#include "ww3d.h"
//
//
//
//
//
//

typedef SignalBaseClass::MemberCallback<GadgetClass, ClassSelectInterface, void (ClassSelectInterface::*)(FlashingGadgetEvent *, GadgetClass *), FlashingGadgetEvent> UIClassCallbackClass;

RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj);	// 0x497300
void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp)
void Unknown_496DB0(RenderObjClass *robj);	// 0x496DB0 (ro_funcs; 0x577B80 calls it on each shown sub-object)
void Unknown_42ECF0(float *x, float *y);	// 0x42ECF0 (the pointer's position, 0..1)
void Unknown_6CDBC0(const char *text, int a);	// 0x6CDBC0
void Unknown_65F2D0(const char *text);	// 0x65F2D0
float Unknown_77B7D0(float t);	// 0x77B7D0 (cdecl: 0..1 eased with sin / cos; 0x576AC0 passes the turn time left)  // NAME 1 [t]: neutral
const char *Unknown_82F3C0(int index);	// 0x82F3C0

struct ClassSelectInfoStruct {
	int Race;
	int Profession;
	int Angle;
};

static ClassSelectInfoStruct Unknown_B7DB60[9] = {
	{ 0, 0, 20 },
	{ 0, 2, 0 },
	{ 0, 1, 340 },
	{ 2, 2, 260 },
	{ 2, 1, 240 },
	{ 2, 0, 220 },
	{ 1, 0, 140 },
	{ 1, 1, 120 },
	{ 1, 2, 100 }
};

struct ClassSelectButtonDefStruct {
	const char *Name;
	const char *SubObject;
};

static ClassSelectButtonDefStruct Unknown_B7DBCC[14] = {
	{ "confirm", "confirm_fe_class00" },
	{ "exit", "back_fe_class00" },
	{ "shirt_style_lef", NULL },
	{ "shirt_style_rig", NULL },
	{ "detail", "skill_fe_class00" },
	{ "enforcer", "enforcer_fe_class00" },
	{ NULL, "scout_fe_class00" },
	{ "tradesman", "tradesman_fe_class00" },
	{ "sentinel", "sentinel_fe_class00" },
	{ NULL, "privateer_fe_class00" },
	{ "warrior", "warrior_fe_class00" },
	{ "defender", "defender_fe_class00" },
	{ NULL, "seeker_fe_class00" },
	{ "explorer", "explorer_fe_class00" }
};

static ClassSelectButtonDefStruct Unknown_B7DC44[8] = {
	{ "skill00", "skill00_fe_class00" },
	{ "skill01", "skill01_fe_class00" },
	{ "skill02", "skill02_fe_class00" },
	{ "skill03", "skill03_fe_class00" },
	{ "skill04", "skill04_fe_class00" },
	{ "skill05", "skill05_fe_class00" },
	{ "skill06", "skill06_fe_class00" },
	{ "skill07", "skill07_fe_class00" }
};

struct ClassSelectTitleStruct {	// NAME 1: neutral (D10): the class select title and subtitle (0x5779D0)
	const char *Title;		// +0x00 (0x82F3C0's name for the class)  // NAME 1: what 0x5779D0 shows it in
	const char *Subtitle;	// +0x04 ('Terran Warrior' ...)  // NAME 1: what 0x5779D0 shows it in
};

// 0xBE5B6C (the dynamic initialiser 0x5745B0 fills it: 0x82F3C0(0, 2, 1, 8, 7, 6, 3, 4, 5) in Unknown_B7DB60's order)
static ClassSelectTitleStruct Unknown_BE5B6C[9] = {
	{ Unknown_82F3C0(0), "Terran Warrior" },
	{ Unknown_82F3C0(2), "Terran Explorer" },
	{ Unknown_82F3C0(1), "Terran Tradesman" },
	{ Unknown_82F3C0(8), "Progen Explorer" },
	{ Unknown_82F3C0(7), "Progen Tradesman" },
	{ Unknown_82F3C0(6), "Progen Warrior" },
	{ Unknown_82F3C0(3), "Jenquai Warrior" },
	{ Unknown_82F3C0(4), "Jenquai Tradesman" },
	{ Unknown_82F3C0(5), "Jenquai Explorer" }
};

static unsigned char Unknown_B7DC8C[9] = { 0, 0, 1, 0, 1, 0, 0, 1, 0 };	// the client's bytes: 0x577B80 hides Scout, Seeker, Privateer (0x82F3C0's order)
static unsigned char Unknown_B7DC98[9] = { 0, 1, 0, 0, 1, 0, 0, 1, 0 };	// the client's bytes: 0x575660's arrows skip the same three (Unknown_B7DB60's order)

#pragma auto_inline(off)
// the backward step through a local (prev): `index--` reuses dec's flags (jns), the client tests (test / jge)
// @0x5746C0
static int Unknown_5746C0(int index, bool forward)
{
	while (true) {
		if (forward) {
			index = (index + 1) % 9;
			if (Unknown_B7DC98[index] == 0) return(index);
		} else {
			int prev = index - 1;
			index = (prev < 0) ? 8 : prev;
			if (Unknown_B7DC98[index] == 0) return(index);
		}
	}
}
#pragma auto_inline(on)


// NOT EXACT: frame (0.45: the client's frame is 0x34, ours 0x38: its per-iteration RefCountPtr sits in the dead view parameter's slot)
// @0x574710
ClassSelectInterface::ClassSelectInterface(UIViewClass *view, LayerClass *layer) :
	InterfaceClass(view, (int)layer->Peek_Scene()),
	CharacterView(NULL),
	LoginTask(NULL),
	Unknown8C(0),
	UnknownE4((float)Unknown_B7DB60[0].Angle),
	UnknownE8((float)Unknown_B7DB60[0].Angle),
	UnknownEC(0),
	UnknownF0(0),
	Slider(NULL),
	SliderValue(0),
	Unknown13C(false),
	Unknown13D(false),
	Unknown13E(false),
	Unknown13F(false),
	Unknown140(false),
	Unknown141(false)
{
	InterfaceSortScopeClass sort(this);
	int i;
	for (i = 0; i < 14; i++) {
		Buttons[i] = NULL;
	}
	for (i = 0; i < 8; i++) {
		SkillGadgets[i] = NULL;
	}
	for (i = 0; i < 7; i++) {
		Models[i] = NULL;
	}
	CharacterView = (CharacterViewClass *)View->Unknown_28();
	LoginTask = (LoginTaskClass *)CharacterView->Unknown14->Unknown_08();
	CharacterView->Unknown_67A250();
	CharacterView->Unknown_67ACE0(0);
	CharacterView->Unknown_67ACE0(1);
	CharacterView->Unknown_67AB20(0, "fe_class00");
	RenderObjClass *model = CharacterView->ViewRObjs[0];
	CharacterView->Unknown068->Set_Clip_Planes(10.0f, 10000.0f);
	if (CharacterView->ViewRObjs[1] == NULL) {
		CharacterView->Unknown_67AB20(1, "fe_class01");
		CharacterView->Unknown_67A290(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, -90.0f, -90.0f), 90.0f);
		CharacterView->Unknown_67B7A0(0);
		Unknown_577B80();
	}
	if (CharacterView->ViewRObjs[3] == NULL) {
		CharacterView->Unknown_67AB20(3, "comp00a");
		CharacterView->Unknown_67AB20(4, "FE_NAR_AGG");
	}
	CharacterView->Unknown_67B4F0(1);
	INI.Load("fa_fe_class00.ini");
	Unknown_BE67BC->Load("fa_fe_class00.ini");
	Unknown_BE67C4->Add(model);

	for (i = 0; i < 14; i++) {
		if (Unknown_B7DBCC[i].Name != NULL) {
			RefCountPtr<RenderObjClass> sub = Create_Get(model->Get_Sub_Object_By_Name(Unknown_B7DBCC[i].Name));
			Unknown_496AE0(sub.Peek(), 0);
			Buttons[i] = new ButtonGadgetClass(sub.Peek(), *layer, 0x55, 3);
			Unknown64.Unknown_6870E0(Buttons[i], i + 1, View, 1, 0);
		}
	}

	for (i = 0; i < 8; i++) {
		if (Unknown_B7DC44[i].Name != NULL) {
			RefCountPtr<RenderObjClass> sub = Create_Get(model->Get_Sub_Object_By_Name(Unknown_B7DC44[i].Name));
			Unknown_496AE0(sub.Peek(), 0);
			SkillGadgets[i] = new ButtonGadgetClass(sub.Peek(), *layer, 0x55, 3);
			Unknown64.Unknown_6870E0(SkillGadgets[i], i + 0x11, View, 1, 0);
			if (Unknown_B7DC44[i].SubObject != NULL) {
				((RObjGadgetClass *)SkillGadgets[i])->Unknown_98(Unknown_BE67C4->Get_Style(Unknown_B7DC44[i].SubObject, model));
			}
		}
	}

	Slider = new GadgetSlider(false, layer);
	((GadgetSlider *)Slider)->Unknown_66E460(Unknown_497300(model), 0, 0);
	Unknown64.Unknown_6870E0(Slider, 0x1A, View, 1, 0);

	((ControlKeyListClass *)Unknown4C)->Add(KernelEventClass(0x1B), (void *)0x10);
	((ControlKeyListClass *)Unknown4C)->Add(KernelEventClass(0x0D), (void *)1);

	Unknown_5760C0(true);

	for (i = 0; i < 9; i++) {
		if (CharacterView->Unknown088.Race == Unknown_B7DB60[i].Race &&
		    CharacterView->Unknown088.Profession == Unknown_B7DB60[i].Profession) {
			Unknown8C = i;
			break;
		}
	}

	UnknownE8 = UnknownE4 = (float)Unknown_B7DB60[Unknown8C].Angle;
	CharacterView->Unknown_67C440(Unknown_B7DB60[Unknown8C].Race, 1);
	Unknown_5779D0();
	Unknown_576A80();
	CharacterView->Unknown3A4->Unknown_760770(100000, 1);
	CharacterView->Unknown_67C7B0();

	if (!TutorialManagerClass::Instance->Is_Completed("ClassSelectionIntro")) {
		CharacterView->Unknown_67C920("1492_00_328se.mp3");
		TutorialManagerClass::Instance->Set_Completed("ClassSelectionIntro");
	}

	if (!(LoginTask->Previous_Step == 4) && Unknown8C == 0) {
		CharacterView->Unknown_67C920("1492_00_344se.mp3");
		CharacterView->Unknown_67C920("1492_00_345se.mp3");
		CharacterView->Unknown_67C920("1492_00_346se.mp3");
	}

	void (ClassSelectInterface::*function)(FlashingGadgetEvent *, GadgetClass *) = &ClassSelectInterface::Unknown_577DA0;
	SignalMap *notifier = &GadgetClass::Get_Signal_Map();
	notifier->connect(NULL, new UIClassCallbackClass(this, function));

	CharacterView->Unknown30->Unknown0F = true;
}


// @0x575220
ClassSelectInterface::~ClassSelectInterface()
{
	Unknown_576A60();
	Unknown_5C();
	Descriptions.clear();
	SkillDescriptions.clear();
	Unknown_BE67C4->Remove(CharacterView->Peek_Model());
	Unknown_BE67BC->Unload("fa_fe_class00.ini");
	CharacterView->Unknown_67A9B0(0);

	void (ClassSelectInterface::*function)(FlashingGadgetEvent *, GadgetClass *) = &ClassSelectInterface::Unknown_577DA0;
	SignalMap *notifier = &GadgetClass::Get_Signal_Map();
	UIClassCallbackClass callback(this, function);
	notifier->disconnect(0, callback);
}


// @0x5754E0
void ClassSelectInterface::Unknown_60()
{
	Unknown_576AC0();
	Unknown_577D30();
	Unknown_577EE0();
	Unknown_577F20();
	Unknown_575CD0();
}


// key 7 alone sets the class under `!= 1` and returns in place (the others break to the end): the client's per-case ebx = 1
// @0x575660
bool ClassSelectInterface::Unknown_68(int key)
{
	if (Unknown13C || Unknown13D || Unknown13E || Unknown13F || Unknown140 || Unknown141) {
		return(false);
	}
	GadgetClass *skill = NULL;
	switch (key) {
		case 1:
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			CharacterView->Unknown1CC[1] = true;
			Unknown_575D50();
			LoginTask->Go_Next_Step();
			CharacterView->Unknown_67C7B0();
			return(true);
		case 2:
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			if (LoginTask->Previous_Step == 1) {
				LoginTask->Set_Step(1, false);
			} else {
				LoginTask->Go_Back_Step();
			}
			CharacterView->Unknown_67ACE0(1);
			CharacterView->Unknown1CC[1] = true;
			CharacterView->Unknown1CC[3] = true;
			CharacterView->Unknown1CC[4] = true;
			CharacterView->Unknown_67C7B0();
			return(true);
		case 3: {
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			int next = Unknown8C - 1;
			if (next < 0) next = 8;
			if (Unknown_B7DC98[next]) next = Unknown_5746C0(next, false);
			Unknown8C = next;
			Unknown_5779D0();
			return(true);
		}
		case 4: {
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			int next = (Unknown8C + 1) % 9;
			Unknown8C = Unknown_B7DC98[next] ? Unknown_5746C0(next, true) : next;
			Unknown_5779D0();
			return(true);
		}
		case 5:
			break;
		case 16:
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			CharacterView->Unknown_67B270();
			Unknown13D = true;
			CharacterView->Unknown_67C7B0();
			return(true);
		case 6:
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			if (Unknown8C == 0) {
				break;
			}
			Unknown8C = 0;
			Unknown_5779D0();
			return(true);
		case 7:
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			if (Unknown8C != 1) {
				Unknown8C = 1;
				Unknown_5779D0();
			}
			return(true);

		case 8:
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			if (Unknown8C == 2) {
				break;
			}
			Unknown8C = 2;
			Unknown_5779D0();
			return(true);
		case 9:
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			if (Unknown8C == 3) {
				break;
			}
			Unknown8C = 3;
			Unknown_5779D0();
			return(true);
		case 10:
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			if (Unknown8C == 4) {
				break;
			}
			Unknown8C = 4;
			Unknown_5779D0();
			return(true);
		case 11:
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			if (Unknown8C == 5) {
				break;
			}
			Unknown8C = 5;
			Unknown_5779D0();
			return(true);
		case 12:
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			if (Unknown8C == 6) {
				break;
			}
			Unknown8C = 6;
			Unknown_5779D0();
			return(true);
		case 13:
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			if (Unknown8C == 7) {
				break;
			}
			Unknown8C = 7;
			Unknown_5779D0();
			return(true);
		case 14:
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			if (Unknown8C == 8) {
				break;
			}
			Unknown8C = 8;
			Unknown_5779D0();
			return(true);
		case 17:
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			skill = SkillGadgets[0];
			break;
		case 18:
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			skill = SkillGadgets[1];
			break;
		case 19:
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			skill = SkillGadgets[2];
			break;
		case 20:
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			skill = SkillGadgets[3];
			break;
		case 21:
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			skill = SkillGadgets[4];
			break;
		case 22:
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			skill = SkillGadgets[5];
			break;
		case 23:
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			skill = SkillGadgets[6];
			break;
		case 24:
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			skill = SkillGadgets[7];
			break;
		default:
			return(false);
	}
	if (skill) {
		skill->Unknown2C_06 = 1;
	}
	return(true);
}


// Unknown_60 inlines it
// @0x575CD0
void ClassSelectInterface::Unknown_575CD0()
{
	if (CharacterView->Unknown_67B2B0()) {
		Unknown13D = false;
	}
	bool active = CharacterView->Unknown34->Active;
	if (CharacterView->Unknown34->Active && !Unknown13D) {
		Unknown13D = true;
		Unknown_5760C0(false);
		return;
	}
	if (!CharacterView->Unknown34->Active && Unknown13D) {
		Unknown13D = false;
		Unknown_5760C0(true);
	}
}


// @0x575D50
void ClassSelectInterface::Unknown_575D50()
{
	AvatarDescClass *desc = &CharacterView->Unknown088;
	CharacterView->Unknown088 = AvatarDescClass(Unknown_B7DB60[Unknown8C].Race, Unknown_B7DB60[Unknown8C].Profession, desc->Gender);
}


// @0x5760C0
void ClassSelectInterface::Unknown_5760C0(bool arg)
{
	for (int i = 0; i < 14; i++) {
		if (Buttons[i]) {
			Buttons[i]->Unknown_40(arg);
			if (Unknown_B7DBCC[i].SubObject) {
				FontSys *font = Unknown_BE67C4->Get_Style(Unknown_B7DBCC[i].SubObject, CharacterView->Peek_Model());
				if (font) {
					if (arg) {
						font->Unknown_Set_2FC(1.0f);
					} else {
						font->Unknown_Set_2FC(0.33f);
					}
				}
			}
		}
	}
	Buttons[4]->Unknown_40(false);
	((RObjGadgetClass *)Buttons[4])->Unknown_8C(true);
	Unknown_BE67C4->Get_Style("skill_fe_class00", CharacterView->Peek_Model())->Set_Hidden(1);
}


// Unknown_5779D0 inlines it
// @0x5761D0
void ClassSelectInterface::Unknown_5761D0()
{
	FontSys *title = Unknown_BE67C4->Get_Style("title_fe_class00", CharacterView->Peek_Model());
	if (title) {
		title->Change_Text(Unknown_BE5B6C[Unknown8C].Title, 0);
	}
	FontSys *subtitle = Unknown_BE67C4->Get_Style("subtitle_fe_class00", CharacterView->Peek_Model());
	if (subtitle) {
		subtitle->Change_Text(Unknown_BE5B6C[Unknown8C].Subtitle, 0);
	}
}


// the bit clear written in both branches (tail-merged): VC6 then keeps its mask in ebp, as the client
// @0x576270
void ClassSelectInterface::Unknown_576270()
{
	for (int i = 0; i < 8; i++) {
		if (SliderValue + i < (int)Descriptions.size()) {
			((RObjGadgetClass *)SkillGadgets[i])->Unknown34->Change_Text(Descriptions[SliderValue + i].c_str(), 0);
			SkillGadgets[i]->Set_Unknown14(SkillDescriptions[SliderValue + i].c_str());
			SkillGadgets[i]->Unknown2C_06 = 0;
		} else {
			((RObjGadgetClass *)SkillGadgets[i])->Unknown34->Change_Text("", 0);
			SkillGadgets[i]->Set_Unknown14("");
			SkillGadgets[i]->Unknown2C_06 = 0;
		}
	}
}







// @0x5779D0
void ClassSelectInterface::Unknown_5779D0()
{
	Unknown_576A60();
	Unknown_5761D0();
	Unknown_576360();
	Unknown_5767F0();
	CharacterView->Unknown_67C440(Unknown_B7DB60[Unknown8C].Race, false);
	int angle = Unknown_B7DB60[Unknown8C].Angle;
	if ((float)angle != UnknownE4) {
		int delta = (int)((float)angle - UnknownE4);
		if (delta > 180) {
			delta -= 360;
		} else if (delta < -180) {
			delta += 360;
		}
		UnknownEC = 1.0f;
		UnknownE8 = (float)delta + UnknownE4;
		SoundLibraryClass::Play_2D_Sound("planet_spin.wav", 2);
	}
	CharacterView->Unknown_67C7B0();
}


// @0x577B80
void ClassSelectInterface::Unknown_577B80()
{
	RenderObjClass *robj = CharacterView->ViewRObjs[1];
	RefCountPtr<RenderObjClass> model = Unknown_497300(robj);
	for (int i = 0; i < 9; i++) {
		char buf[64];
		_snprintf(buf, sizeof(buf), "%s03", Unknown_82F3C0(i));
		int bone = model->Get_Bone_Index(buf);
		for (int j = 0; j < model->Get_Num_Sub_Objects_On_Bone(bone); j++) {
			RefCountPtr<RenderObjClass> sub = Create_Get(model->Get_Sub_Object_On_Bone(j, bone));
			if (Unknown_B7DC8C[i]) {
				sub->Set_Hidden(1);
			} else {
				sub->Set_Hidden(0);
				Unknown_496DB0(sub.Peek());
			}
		}
	}
}

























































































































































































































































































































































// Unknown_60 inlines it
// @0x577D30
void ClassSelectInterface::Unknown_577D30()
{
	for (int i = 0; i < 9; i++) {
		if (Buttons[i + 5]) {
			if (Unknown8C == i) {
				Buttons[i + 5]->Unknown2C_0B = 1;
				Buttons[i + 5]->Unknown_6603A0(0, 0);
			} else {
				Buttons[i + 5]->Unknown2C_0B = 0;
				Buttons[i + 5]->Unknown_6607D0();
			}
		}
	}
}


// @0x577DA0
void ClassSelectInterface::Unknown_577DA0(FlashingGadgetEvent *event, GadgetClass *sender)
{
	FAIL_IF(!event)
		return;

	GadgetClass *gadget = event->Gadget;
	FAIL_IF(!gadget)
		return;

	if (gadget->Flash != NULL && gadget->Is_Unknown2C_02()) {
		// the client's last test compares the gadget with +0x8C itself (cmp eax, [ecx+0x8C]), not a button
		if (gadget == Buttons[0] || gadget == Buttons[1] || gadget == Buttons[2] ||
		    gadget == Buttons[3] || gadget == Buttons[4] || gadget == (GadgetClass *)Unknown8C) {
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Over_Buttons", 2);
		}
	}
}


// Unknown_576360 inlines it
// @0x577E70
void ClassSelectInterface::Unknown_577E70(int count)
{
	((GadgetSlider *)Slider)->Unknown_66FA90((float)count, 8.0f, true);
	int value = (int)((GadgetSlider *)Slider)->Value;
	if (SliderValue != value) {
		SliderValue = value;
		Unknown_576270();
	}
}


// Unknown_60 inlines it; Unknown_577E70 has the same code
// @0x577EE0
void ClassSelectInterface::Unknown_577EE0()
{
	int value = (int)((GadgetSlider *)Slider)->Value;
	if (SliderValue != value) {
		SliderValue = value;
		Unknown_576270();
	}
}


// Unknown_60 inlines it
// @0x577F20
void ClassSelectInterface::Unknown_577F20()
{
	float x, y;
	Unknown_42ECF0(&x, &y);
	for (int i = 0; i < 8; i++) {
		if (SkillGadgets[i] && !SkillGadgets[i]->Unknown_30(x, y)) {
			SkillGadgets[i]->Unknown2C_06 = 0;
		}
	}
}


// 0xBDA908 (the description's "") is the pooled empty string literal
// @0x576360
void ClassSelectInterface::Unknown_576360()
{
	char section[64];
	char entry[64];
	_snprintf(section, sizeof(section), "CLASS-%d%d", Unknown_B7DB60[Unknown8C].Race, Unknown_B7DB60[Unknown8C].Profession);
	Descriptions.clear();
	SkillDescriptions.clear();
	int i = 0;
	while (true) {
		_snprintf(entry, sizeof(entry), "Info%d", i);
		if (INI.Find_Entry(section, entry) == NULL) {
			Unknown_577E70(Descriptions.size());
			Unknown_576270();
			return;
		} else {
			StringClass text;
			StringClass tip;
			INI.Get_String(text, section, entry, " ");
			if (!text.Is_Empty()) {
				std::string name = (const char *)text;
				Descriptions.push_back(name);
				std::string key = name + " " + Unknown_BE5B6C[Unknown8C].Title;
				if (INI.Find_Entry("SKILL_TOOL_TIPS", key.c_str()) != NULL) {
					INI.Get_String(tip, "SKILL_TOOL_TIPS", key.c_str(), " ");
				} else {
					INI.Get_String(tip, "SKILL_TOOL_TIPS", name.c_str(), " ");
				}
				std::string description = "";
				if (!tip.Is_Empty()) {
					description = (const char *)tip;
				}
				SkillDescriptions.push_back(description);
			}
		}
		i++;
	}
}


// @0x5767F0
void ClassSelectInterface::Unknown_5767F0()
{
	char section[64];
	char entry[64];
	char name[64];
	char bone[64];
	RenderObjClass *model = CharacterView->ViewRObjs[0];
	_snprintf(section, sizeof(section), "CLASS-%d%d", Unknown_B7DB60[Unknown8C].Race, Unknown_B7DB60[Unknown8C].Profession);
	for (int i = 0; i < 7; i++) {
		if (Models[i]) {
			model->Remove_Sub_Object(Models[i].Peek());
			Models[i] = NULL;
		}
		_snprintf(entry, sizeof(entry), "Stat%d", i);
		if (INI.Find_Entry(section, entry) != NULL) {
			_snprintf(name, sizeof(name), "fe_stats%02d", INI.Get_Int(section, entry, 0));
			Models[i] = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(name));
			if (Models[i]) {
				_snprintf(bone, sizeof(bone), "stats%02d", i + 1);
				model->Add_Sub_Object_To_Bone(Models[i].Peek(), bone);
			}
		}
	}
}


// no code callers
// @0x576A40
void ClassSelectInterface::Unknown_576A40()
{
}


// Unknown_5779D0 and the destructor inline it
// @0x576A60
void ClassSelectInterface::Unknown_576A60()
{
	CharacterView->Unknown_67B320();
}


// the constructor inlines it
// @0x576A80
void ClassSelectInterface::Unknown_576A80()
{
	CharacterView->Unknown_67B390(Unknown_B7DB60[Unknown8C].Race, Unknown_B7DB60[Unknown8C].Profession);
}


// Unknown_60, every frame: the planet turned toward UnknownE8, the class's dialog when it arrives, the moons
// NOT EXACT: inline-budget (0.80: ours inlines all 18 dialog names' string assigns, the client calls the last one out of line; two dead size() reads at the top gave 0.84 and the client's 17, then the register order of the 0x67B390 call and the identity matrix stores were left)
// @0x576AC0
void ClassSelectInterface::Unknown_576AC0()
{
	float frame = (float)WW3D::Get_Frame_Time() * 0.001f;
	Matrix3D tm(true);
	RefCountPtr<RenderObjClass> model = Unknown_497300(CharacterView->ViewRObjs[1]);
	int bone = model->Get_Bone_Index("planet");
	Unknown_5760C0(true);
	if (UnknownEC > 0.0f) {
		Unknown_5760C0(false);
		float angle;
		UnknownEC -= frame;
		if (UnknownEC < 0.0f) {
			UnknownE4 = UnknownE8;
			UnknownEC = 0.0f;
			if (UnknownE4 > 360.0f) {
				UnknownE4 -= 360.0f;
			} else if (UnknownE4 < 0.0f) {
				UnknownE4 += 360.0f;
			}
			angle = UnknownE4;
			CharacterView->Unknown_67B390(Unknown_B7DB60[Unknown8C].Race, Unknown_B7DB60[Unknown8C].Profession);
			CharacterView->Unknown_67C7B0();
			switch (Unknown8C) {
				case 0:
					CharacterView->Unknown_67C920("1492_00_344se.mp3");
					CharacterView->Unknown_67C920("1492_00_345se.mp3");
					CharacterView->Unknown_67C920("1492_00_346se.mp3");
					break;
				case 2:
					CharacterView->Unknown_67C920("1492_00_335se.mp3");
					CharacterView->Unknown_67C920("1492_00_336se.mp3");
					CharacterView->Unknown_67C920("1492_00_337se.mp3");
					break;
				case 3:
					CharacterView->Unknown_67C920("1492_00_338se.mp3");
					CharacterView->Unknown_67C920("1492_00_339se.mp3");
					CharacterView->Unknown_67C920("1492_00_340se.mp3");
					break;
				case 5:
					CharacterView->Unknown_67C920("1492_00_329se.mp3");
					CharacterView->Unknown_67C920("1492_00_330se.mp3");
					CharacterView->Unknown_67C920("1492_00_331se.mp3");
					break;
				case 6:
					CharacterView->Unknown_67C920("1492_00_341se.mp3");
					CharacterView->Unknown_67C920("1492_00_342se.mp3");
					CharacterView->Unknown_67C920("1492_00_343se.mp3");
					break;
				case 8:
					CharacterView->Unknown_67C920("1492_00_332se.mp3");
					CharacterView->Unknown_67C920("1492_00_333se.mp3");
					CharacterView->Unknown_67C920("1492_00_334se.mp3");
					break;
			}
		} else {
			float t = Unknown_77B7D0(UnknownEC);
			angle = t * UnknownE4 + (1.0f - t) * UnknownE8;
		}
		angle /= 180.0f;
		tm.Rotate_Z(angle * WWMATH_PI);
	} else {
		float angle = UnknownE4 / 180.0f;
		tm.Rotate_Z(angle * WWMATH_PI);
	}
	model->Capture_Bone(bone);
	model->Control_Bone(bone, tm, false);
	UnknownF0 += frame * 0.33f;
	for (int i = 0; i < 9; i++) {
		Matrix3D moon(true);
		int moon_bone = model->Get_Bone_Index(Unknown_82F3C0(i));
		moon.Rotate_Z(UnknownF0);
		model->Capture_Bone(moon_bone);
		model->Control_Bone(moon_bone, moon, false);
	}
}
