// Earth & Beyond client (reconstruction): app\client\ui_option_slider.cpp, the options' slider pages ('opt04a': the
// graphics, the sound or the interface). No string names the file (no asserts): the name is provisional (NAME 1, D137).
// Notes at the end.
#include <stdio.h>
#include <string.h>
#include "ui_option_slider.h"
#include "ui_chat_panel.h"
#include "client.h"
#include "sclient.h"
#include "gadget.h"
#include "gd_robj.h"
#include "gd_slider.h"
#include "option.h"
#include "fontmanager.h"
#include "local_interface_sort.h"
#include "..\audio\soundlib.h"

// The check box (unknown_7e7e00.h: 0x68 bytes, table 0xB075E4)
#include "unknown_7e7e00.h"

extern FontManagerClass *Unknown_BE67C4;	// NAME 1 [Unknown_BE67C4]: 0xBE67C4, as ui_login.cpp's

// The buttons Unknown_54 makes (0xB8369C), each page's ini (0xB836B0) and texts (0xB836BC)
static const char *Unknown_B8369C[4] = { "exit", "ok", "cancel", "resolution" };	// NAME 1: neutral (D10)
static const char *Unknown_B836B0[3] = { "fa_opt04a-graphics.ini", "fa_opt04a-sound.ini", "fa_opt04a-interface.ini" };	// NAME 1: neutral (D10)
static const char *Unknown_B836BC[3][4] = {	// NAME 1: neutral (D10)
	{ NULL, "ok_opt04a", "cancel_opt04a", "resolution_opt04a" },
	{ NULL, "ok_opt04a", "cancel_opt04a", NULL },
	{ NULL, "ok_opt04a", "cancel_opt04a", NULL },
};

// The option each page's check shows (0xB836EC; option.cpp's bools), NULL for none
static const char *Unknown_B836EC[3] = { "Graphics_Default", "Doppler_Enabled", NULL };	// NAME 1: neutral (D10)

// The option each page's sliders show (0xB836F8; option.cpp's floats), NULL for an unused slider
static const char *Unknown_B836F8[3][5] = {	// NAME 1: neutral (D10)
	{ "Graphics_Gamma", "Graphics_Land_Poly", "Graphics_Land_Detail", "Graphics_Space_Poly", "Graphics_Texture_Size" },
	{ "Sound_Music", "Sound_SFX", "Sound_Dialog" },
	{ "Interface_Tooltip_Delay" },
};

// 0x5CEDD0
UIOptionSliderClass::UIOptionSliderClass(SClient *client, int page, bool mode) :
	ChatPanelUIBase(client),
	Page(page),
	Mode(mode),
	Check(NULL)
{
	memset(Buttons, 0, sizeof(Buttons));
	memset(Sliders, 0, sizeof(Sliders));
	memset(Values, 0, sizeof(Values));
}

// 0x5CEEC0
UIOptionSliderClass::~UIOptionSliderClass()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
	}
	for (int i = 0; i < 5; i++) {
		if (Sliders[i]) {
			Sliders[i]->Unknown_66E460(RefCountPtr<RenderObjClass>(), 0, 0);
		}
	}
}

// 0x5CEFB0
void UIOptionSliderClass::Unknown_54()
{
	if (Unknown_98("opt04a", Unknown_B836B0[Page])) {
		int i;
		for (i = 0; i < 4; i++) {
			Buttons[i] = Add_Button(i + 1, Unknown_B8369C[i], Unknown_B836BC[Page][i]);
			GadgetClass::Set_Tooltip(Buttons[i], "UI_OPTION_SLIDER", Unknown_B8369C[i]);
		}
		for (i = 0; i < 5; i++) {
			Sliders[i] = Add_Slider(i + 6, 0);
			Sliders[i]->Unknown_66E460(InterfaceMesh, 1, i);
			if (Unknown_B836F8[Page][i]) {
				Sliders[i]->Unknown_40(true);
				Sliders[i]->Unknown_8C(false);
			} else {
				char name[32];
				_snprintf(name, sizeof(name), "track%02d", i);
				RefCountPtr<RenderObjClass> track = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name(name));	// NAME 1: neutral (D10)
				track->Set_Hidden(true);
				Sliders[i]->Unknown_40(false);
				Sliders[i]->Unknown_8C(true);
			}
		}
		Check = Add_Check_Button(12, "check00", NULL);
		if (Page) {
			Buttons[3]->Unknown_40(false);
			((RObjGadgetClass *)Buttons[3])->Unknown_8C(true);
		}
		Add_Back_Gadget(0, Vector3(0.0f, 0.0f, 0.0f));
	}
}

// 0x5CF230
void UIOptionSliderClass::Unknown_58()
{
	if (!Active) {
		InterfaceSortScopeClass sort(this);	// NAME 1: neutral (D10)
		ChatPanelUIBase::Unknown_58();
		Unknown_5CF6C0();
		Unknown_5CF580();
	}
}

// 0x5CF2C0
void UIOptionSliderClass::Unknown_5C()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
	}
}

// 0x5CF2E0
void UIOptionSliderClass::Unknown_60()
{
	if (!Closed) {
		Unknown_5CF880(false);
	}
}

// 0x5CF310
bool UIOptionSliderClass::Unknown_68(int key)
{
	SClient *sclient = (SClient *)((ClientClass *)Client)->Unknown_18();	// NAME 1: neutral (D10)
	ChatPanelInterface *chat = sclient->ChatPanel.Peek();	// NAME 1: neutral (D10)
	switch (key) {
		case 1:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_5CF880(true);
			((SClient *)((ClientClass *)Client)->Unknown_18())->MainView->Unknown_514480();
			((SClient *)((ClientClass *)Client)->Unknown_18())->MainView->Unknown_5146D0();
			Unknown_5C();
			Closed = 1;
			return(true);
		case 2:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			if (!Mode) {
				chat->Unknown_565F30(1);
			}
			Unknown_5CF7E0();
			((SClient *)((ClientClass *)Client)->Unknown_18())->MainView->Unknown_514480();
			((SClient *)((ClientClass *)Client)->Unknown_18())->MainView->Unknown_5146D0();
			Unknown_5C();
			Closed = 1;
			return(true);
		case 3:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			if (!Mode) {
				chat->Unknown_565F30(1);
			}
			Unknown_5CF880(true);
			((SClient *)((ClientClass *)Client)->Unknown_18())->MainView->Unknown_514480();
			((SClient *)((ClientClass *)Client)->Unknown_18())->MainView->Unknown_5146D0();
			Unknown_5C();
			Closed = 1;
			return(true);
		case 4:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			chat->Unknown_565F30(10);
			Unknown_5C();
			Closed = 1;
			return(true);
		case 12:
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			Unknown_5CF580();
			((SClient *)((ClientClass *)Client)->Unknown_18())->MainView->Unknown_514480();
			((SClient *)((ClientClass *)Client)->Unknown_18())->MainView->Unknown_5146D0();
			return(true);
	}
	return(false);
}

#pragma auto_inline(off)
// 0x5CF580
void UIOptionSliderClass::Unknown_5CF580()
{
	if (Page == 0) {
		for (int i = 0; i < 5; i++) {
			char name[32];
			_snprintf(name, sizeof(name), "text%02d_opt04a", i + 1);
			RefCountPtr<FontSys> text = Unknown_BE67C4->Unknown_6D2700(name, InterfaceMesh);	// NAME 1: neutral (D10)
			if (Check->Checked) {
				text->Unknown_Set_2FC(0.33f);
				Sliders[i]->Unknown_40(false);
			} else {
				text->Unknown_Set_2FC(1.0f);
				Sliders[i]->Unknown_40(true);
			}
		}
	}
}

// 0x5CF6C0
void UIOptionSliderClass::Unknown_5CF6C0()
{
	OptionClass *options = OptionClass::Instance;	// NAME 1: neutral (D10)
	if (Unknown_B836EC[Page]) {
		BoolOptionStruct *option = options->Find_Bool(Unknown_B836EC[Page]);	// NAME 1: neutral (D10)
		if (option) {
			Check->Unknown_7E8030(option->Get_Value());
		}
	} else {
		((RObjGadgetClass *)Check)->Unknown_8C(true);
		Check->Unknown_40(false);
	}
	for (int i = 0; i < 5; i++) {
		if (Unknown_B836F8[Page][i]) {
			Sliders[i]->Unknown_66FA90(100.0f, 1.0f, true);
			FloatOptionStruct *option = options->Find_Float(Unknown_B836F8[Page][i]);	// NAME 1: neutral (D10)
			if (option) {
				Sliders[i]->Unknown_A4(option->Get_Value(), false);
			}
			Values[i] = option->Get_Value();
		}
	}
}

// 0x5CF7E0
void UIOptionSliderClass::Unknown_5CF7E0()
{
	OptionClass *options = OptionClass::Instance;	// NAME 1: neutral (D10)
	if (Unknown_B836EC[Page]) {
		BoolOptionStruct *option = options->Find_Bool(Unknown_B836EC[Page]);	// NAME 1: neutral (D10)
		if (option) {
			option->Set_Value(Check->Checked);
		}
	}
	for (int i = 0; i < 5; i++) {
		if (Unknown_B836F8[Page][i]) {
			FloatOptionStruct *option = options->Find_Float(Unknown_B836F8[Page][i]);	// NAME 1: neutral (D10)
			if (option) {
				option->Set_Value(Sliders[i]->Get_Value());
			}
		}
	}
}

// 0x5CF880
void UIOptionSliderClass::Unknown_5CF880(bool restore)
{
	if (Page == 1) {
		OptionClass *options = OptionClass::Instance;	// NAME 1: neutral (D10)
		for (int i = 0; i < 5; i++) {
			if (Unknown_B836F8[Page][i]) {
				FloatOptionStruct *option = options->Find_Float(Unknown_B836F8[Page][i]);	// NAME 1: neutral (D10)
				if (option) {
					option->Value = restore ? Values[i] : Sliders[i]->Value;
				}
			}
		}
	}
}

#pragma auto_inline(on)

/*
**	Notes.
**	Object 0x5CEDD0-0x5CF95F: the adjustor thunks 0x5CF940 / 0x5CF950 of the second and third tables close it. The
**	ui_option_check.cpp pattern (ChatPanelUIBase, the option tables per page); the four helpers sit under
**	auto_inline(off): the client calls each out of line. Unknown_5CF6C0 reads the float option's
**	value into Values even when Find_Float found none (as the client does). Page 1 (sound) previews the sliders every frame
**	(Unknown_60) and puts the kept values back on exit or cancel; MainView's 0x514480 / 0x5146D0 apply the options.
*/
