// Earth & Beyond client (reconstruction): app\Client\ui_class.cpp. Notes at the end of the file.
#include <stdio.h>
#include <string.h>
#include <vector>
#include <string>
#include "sclient.h"
#include "tasks\tsk_login.h"
#include "rendobj.h"
#include "camera.h"
#include "layer.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\shared\unknown_debug.h"
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

#define Unknown_BE67B8 Unknown_BE67B8_FontDatabase
#include "fontmanager.h"
#undef Unknown_BE67B8
extern FontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4

class SClientUnknown1338Class { public: void Unknown_760770(int base_asset, bool a); };

// The control keys event the constructor registers (0x10 bytes) and what InterfaceClass +0x4C is (as ui_avsel.cpp's)
struct UIClassKeyEventStruct	// NAME 1: neutral (D10): kernel.h's input event
{
	UIClassKeyEventStruct(short key) : Key(key), Unknown02(0), Unknown04(0), Unknown08(0), Unknown0C(0) {}	// NAME 1 [key]: neutral
	short Key;											// +0x00 (0x1B, 0x0D)  // NAME 1: neutral (D10)
	short Unknown02;									// +0x02
	float Unknown04;									// +0x04
	float Unknown08;									// +0x08
	int Unknown0C;										// +0x0C
};
class UIClassKeysClass { public: void Unknown_721E60(const UIClassKeyEventStruct *event, int id); void Unknown_721EF0(const UIClassKeyEventStruct *event); };	// NAME 1: neutral (D10): InterfaceClass +0x4C's 0x721E60  // NAME 1 [event, id]: what the constructor passes

// gd_slider.cpp's slider (0x128 bytes)
class GadgetSlider : public RObjGadgetClass	// NAME 5: the client's RTTI .?AVGadgetSlider@@
{
public:
	GadgetSlider(int arg, LayerClass *layer);		// 0x66DFC0
	void Unknown_66E460(const RefCountPtr<RenderObjClass> &object, int a, int b);	// 0x66E460
	float Value;										// +0xD4
};

typedef SignalBaseClass::MemberCallback<GadgetClass, ClassSelectInterface, void (ClassSelectInterface::*)(FlashingGadgetEvent *, GadgetClass *), FlashingGadgetEvent> UIClassCallbackClass;
typedef SignalBaseClass::CallbackClass UIClassCallbackBaseClass;

class UIClassNotifierClass
{
public:
	void Add(int id, UIClassCallbackBaseClass *callback);
	void Remove(int id, UIClassCallbackBaseClass *callback);
};

RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj);	// 0x497300
void Unknown_496AE0(RenderObjClass *robj, int a);	// 0x496AE0
void Unknown_42ECF0(int *x, int *y);	// 0x42ECF0
void Unknown_6CDBC0(const char *text, int a);	// 0x6CDBC0
void Unknown_65F2D0(const char *text);	// 0x65F2D0
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

static unsigned char Unknown_B7DC8C[9] = { 0, 0, 1, 1, 0, 0, 1, 0, 0 };
static unsigned char Unknown_B7DC98[9] = { 0, 0, 1, 1, 0, 0, 1, 0, 0 };

// 0x5746C0
static int Unknown_5746C0(int index, bool forward)
{
	while (true) {
		if (forward) {
			index = (index + 1) % 9;
			if (Unknown_B7DC98[index] == 0) return(index);
		} else {
			index--;
			if (index < 0) index = 8;
			if (Unknown_B7DC98[index] == 0) return(index);
		}
	}
}


// NOT EXACT: register
// @0x574710
ClassSelectInterface::ClassSelectInterface(UIViewClass *view, LayerClass *layer) :
	InterfaceClass(view, (int)layer->Peek_Scene()),
	CharacterView(NULL),
	LoginTask(NULL),
	Unknown8C(0),
	UnknownE4(20.0f),
	UnknownE8(20.0f),
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
	((FontStringFilesClass *)Unknown_BE67BC)->Load("fa_fe_class00.ini");
	Unknown_BE67C4->Add(model);

	for (i = 0; i < 14; i++) {
		if (Unknown_B7DBCC[i].Name != NULL) {
			RenderObjClass *sub = model->Get_Sub_Object_By_Name(Unknown_B7DBCC[i].Name);
			if (sub) {
				Unknown_496AE0(sub, 0);
				ButtonGadgetClass *btn = new ButtonGadgetClass(sub, *layer, 0x55, 3);
				Buttons[i] = btn;
				Unknown64.Unknown_6870E0(btn, i + 1, View, 1, 0);
				sub->Release_Ref();
			}
		}
	}

	for (i = 0; i < 8; i++) {
		if (Unknown_B7DC44[i].Name != NULL) {
			RenderObjClass *sub = model->Get_Sub_Object_By_Name(Unknown_B7DC44[i].Name);
			if (sub) {
				Unknown_496AE0(sub, 0);
				ButtonGadgetClass *btn = new ButtonGadgetClass(sub, *layer, 0x55, 3);
				SkillGadgets[i] = btn;
				Unknown64.Unknown_6870E0(btn, i + 0x11, View, 1, 0);
				if (Unknown_B7DC44[i].SubObject != NULL) {
					FontStyleClass *text_obj = Unknown_BE67C4->Get_Style(Unknown_B7DC44[i].SubObject, (unsigned int)model);
					btn->Unknown_98(text_obj);
				}
				sub->Release_Ref();
			}
		}
	}

	GadgetSlider *slider = new GadgetSlider(0, layer);
	Slider = slider;
	slider->Unknown_66E460(Unknown_497300(model), 0, 0);
	Unknown64.Unknown_6870E0(Slider, 0x1A, View, 1, 0);

	((UIClassKeysClass *)Unknown4C)->Unknown_721E60(&UIClassKeyEventStruct(0x1B), 0x10);
	((UIClassKeysClass *)Unknown4C)->Unknown_721E60(&UIClassKeyEventStruct(0x0D), 1);

	Unknown_5760C0(true);

	for (i = 0; i < 9; i++) {
		if (CharacterView->Unknown088.Race == Unknown_B7DB60[i].Race &&
		    CharacterView->Unknown088.Profession == Unknown_B7DB60[i].Profession) {
			Unknown8C = i;
			break;
		}
	}

	UnknownE4 = (float)Unknown_B7DB60[Unknown8C].Angle;
	UnknownE8 = UnknownE4;
	CharacterView->Unknown_67C440(Unknown_B7DB60[Unknown8C].Race, 1);
	Unknown_5779D0();
	CharacterView->Unknown_67B390(Unknown_B7DB60[Unknown8C].Race, Unknown_B7DB60[Unknown8C].Profession);
	CharacterView->Unknown3A4->Unknown_760770(100000, 1);
	CharacterView->Unknown_67C7B0();

	if (!TutorialManagerClass::Instance->Is_Completed("ClassSelectionIntro")) {
		CharacterView->Unknown_67C920("1492_00_328se.mp3");
		TutorialManagerClass::Instance->Set_Completed("ClassSelectionIntro");
	}

	if (LoginTask->Previous_Step != 4 && Unknown8C == 0) {
		CharacterView->Unknown_67C920("1492_00_344se.mp3");
		CharacterView->Unknown_67C920("1492_00_345se.mp3");
		CharacterView->Unknown_67C920("1492_00_346se.mp3");
	}

	void (ClassSelectInterface::*function)(FlashingGadgetEvent *, GadgetClass *) = &ClassSelectInterface::Unknown_577DA0;
	UIClassNotifierClass *notifier = (UIClassNotifierClass *)&GadgetClass::Get_Signal_Map();
	notifier->Add(0, new UIClassCallbackClass(this, function));

	CharacterView->Unknown30->Unknown0F = true;
}


// NOT EXACT: register
// @0x575220
ClassSelectInterface::~ClassSelectInterface()
{
	CharacterView->Unknown_67B320();
	Unknown_5C();
	Descriptions.clear();
	SkillDescriptions.clear();
	Unknown_BE67C4->Remove((unsigned int)CharacterView->ViewRObjs[0]);
	((FontStringFilesClass *)Unknown_BE67BC)->Unload("fa_fe_class00.ini");
	CharacterView->Unknown_67A9B0(0);

	void (ClassSelectInterface::*function)(FlashingGadgetEvent *, GadgetClass *) = &ClassSelectInterface::Unknown_577DA0;
	UIClassNotifierClass *notifier = (UIClassNotifierClass *)&GadgetClass::Get_Signal_Map();
	UIClassCallbackClass callback(this, function);
	notifier->Remove(0, &callback);
}


// NOT EXACT: register
// @0x5754E0
void ClassSelectInterface::Unknown_60()
{
	Unknown_576AC0();
	for (int i = 0; i < 9; i++) {
		GadgetClass *g = Buttons[i + 5];
		if (g) {
			if (Unknown8C == i) {
				g->Unknown2C_0B = 1;
				g->Unknown_6603A0(0, 0);
			} else {
				g->Unknown2C_0B = 0;
				g->Unknown_6607D0();
			}
		}
	}
	int slider_val = (int)((GadgetSlider *)Slider)->Value;
	if (SliderValue != slider_val) {
		SliderValue = slider_val;
		Unknown_576270();
	}
	int mouse_x, mouse_y;
	Unknown_42ECF0(&mouse_x, &mouse_y);
	for (int j = 0; j < 8; j++) {
		GadgetClass *g = SkillGadgets[j];
		if (g && !g->Unknown_30((float)mouse_x, (float)mouse_y)) {
			g->Unknown2C_06 = 0;
		}
	}
	if (CharacterView->Unknown_67B2B0()) {
		Unknown13D = false;
	}
	bool flag = CharacterView->Unknown30->Unknown0E;
	if (flag) {
		if (!Unknown13D) {
			Unknown13D = true;
			Unknown_5760C0(false);
			return;
		}
	} else {
		if (Unknown13D) {
			Unknown13D = false;
			Unknown_5760C0(true);
		}
	}
}


// NOT EXACT: register
// @0x575660
bool ClassSelectInterface::Unknown_68(int key)
{
	if (Unknown13C || Unknown13D || Unknown13E || Unknown13F || Unknown140 || Unknown141) {
		return(false);
	}
	switch (key - 1) {
		case 0:
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			CharacterView->Unknown1CC[1] = true;
			Unknown_575D50();
			LoginTask->Go_Next_Step();
			CharacterView->Unknown_67C7B0();
			return(true);
		case 1:
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
		case 2: {
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			int next = Unknown8C - 1;
			if (next < 0) next = 8;
			if (Unknown_B7DC98[next]) next = Unknown_5746C0(next, false);
			Unknown8C = next;
			Unknown_5779D0();
			return(true);
		}
		case 3: {
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			int next = (Unknown8C + 1) % 9;
			if (Unknown_B7DC98[next]) next = Unknown_5746C0(next, true);
			Unknown8C = next;
			Unknown_5779D0();
			return(true);
		}
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13: {
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			int class_idx = key - 6;
			if (Unknown8C != class_idx) {
				Unknown8C = class_idx;
				Unknown_5779D0();
			}
			return(true);
		}
		case 15:
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			CharacterView->Unknown_67B270();
			Unknown13D = true;
			CharacterView->Unknown_67C7B0();
			return(true);
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23: {
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Click", 2);
			int skill_idx = key - 17;
			if (skill_idx >= 0 && skill_idx < 8 && SkillGadgets[skill_idx]) {
				SkillGadgets[skill_idx]->Unknown2C_06 = 1;
			}
			return(true);
		}
	}
	return(true);
}


// NOT EXACT: register
// @0x575D50
void ClassSelectInterface::Unknown_575D50()
{
	AvatarDescClass desc(Unknown_B7DB60[Unknown8C].Race, Unknown_B7DB60[Unknown8C].Profession, CharacterView->Unknown088.Gender);
	CharacterView->Unknown088 = desc;
}


// NOT EXACT: register
// @0x5760C0
void ClassSelectInterface::Unknown_5760C0(bool arg)
{
	for (int i = 0; i < 14; i++) {
		if (Buttons[i]) {
			Buttons[i]->Unknown_40(arg);
			if (Unknown_B7DBCC[i].SubObject) {
				FontStyleClass *font = Unknown_BE67C4->Get_Style(Unknown_B7DBCC[i].SubObject, (unsigned int)CharacterView->ViewRObjs[0]);
				if (font) {
					font->Unknown_Set_2FC(arg ? 1.0f : 0.33f);
				}
			}
		}
	}
	if (Buttons[4]) {
		Buttons[4]->Unknown_40(false);
		((RObjGadgetClass *)Buttons[4])->Unknown_8C(true);
	}
	FontStyleClass *skill_obj = Unknown_BE67C4->Get_Style("skill_fe_class00", (unsigned int)CharacterView->ViewRObjs[0]);
	if (skill_obj) {
		skill_obj->Set_Hidden(1);
	}
}


// NOT EXACT: register
// @0x576270
void ClassSelectInterface::Unknown_576270()
{
	int count = (int)Descriptions.size();
	for (int i = 0; i < 8; i++) {
		int idx = SliderValue + i;
		if (idx < count) {
			Unknown_6CDBC0(Descriptions[idx].c_str(), 0);
			Unknown_65F2D0(SkillDescriptions[idx].c_str());
		} else {
			Unknown_6CDBC0("", 0);
			Unknown_65F2D0("");
		}
		if (SkillGadgets[i]) {
			SkillGadgets[i]->Unknown2C_06 = 0;
		}
	}
}


// NOT EXACT: register
// @0x576AC0
void ClassSelectInterface::Unknown_576AC0()
{
}


// NOT EXACT: register
// @0x5779D0
void ClassSelectInterface::Unknown_5779D0()
{
	CharacterView->Unknown_67B320();
	RenderObjClass *model = CharacterView->ViewRObjs[0];
	FontStyleClass *title = Unknown_BE67C4->Get_Style("title_fe_class00", (unsigned int)model);
	if (title) {
		Unknown_6CDBC0(Unknown_B7DBCC[Unknown8C + 5].SubObject, 0);
	}
	FontStyleClass *sub = Unknown_BE67C4->Get_Style("subtitle_fe_class00", (unsigned int)model);
	if (sub) {
		Unknown_6CDBC0(Unknown_B7DBCC[Unknown8C + 5].SubObject, 0);
	}
	CharacterView->Unknown_67C440(Unknown_B7DB60[Unknown8C].Race, false);
	if ((float)Unknown_B7DB60[Unknown8C].Angle != UnknownE4) {
		UnknownE4 = (float)Unknown_B7DB60[Unknown8C].Angle;
		UnknownEC = 0x3F800000;
		SoundLibraryClass::Play_2D_Sound("planet_spin.wav", 2);
	}
	CharacterView->Unknown_67C7B0();
}


// NOT EXACT: register
// @0x577B80
void ClassSelectInterface::Unknown_577B80()
{
	RenderObjClass *model = CharacterView->ViewRObjs[1];
	if (!model) return;
	for (int i = 0; i < 9; i++) {
		const char *name = Unknown_82F3C0(i);
		char buf[64];
		sprintf(buf, "%s03", name);
		RenderObjClass *sub = model->Get_Sub_Object_By_Name(buf);
		if (sub) {
			sub->Set_Hidden(Unknown_B7DC8C[i] ? 1 : 0);
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

	if (gadget->Flash != NULL && gadget->Unknown2C_02) {
		if (gadget == Buttons[0] || gadget == Buttons[1] || gadget == Buttons[2] ||
		    gadget == Buttons[3] || gadget == Buttons[4] || gadget == Buttons[Unknown8C + 5]) {
			SoundLibraryClass::Play_2D_Sound("Login_Mouse_Over_Buttons", 2);
		}
	}
}
