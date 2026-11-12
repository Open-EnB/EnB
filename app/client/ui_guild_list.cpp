// Earth & Beyond client (reconstruction): app\client\ui_guild_list.cpp, the guild list window (no __FILE__; client
// 0x81E030-0x81F0FF after ui_guild.cpp's end 0x81E02F, its data 0xBA8204-0xBA8267 before verbmanager.cpp's __FILE__).
// PLACEMENT: provisional (D137): the file name from the tooltip section 'UI_GUILD_LIST' (the windows' UI_<file> pattern) and the model 'tsi36a'; a __FILE__ or an assert would place it
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include "ui_guild_list.h"
#include "gadget.h"
#include "gd_slider.h"
#include "fontmanager.h"
#include "gd_robj.h"
#include "rendobj.h"
#include "..\audio\soundlib.h"

extern FontManagerClass *Unknown_BE67C4;				// NAME 1 [Unknown_BE67C4]: 0xBE67C4

// The buttons' models (0xBA8204)
static const char *Unknown_BA8204[2] = { "exit", "button01" };	// NAME 1: neutral (D10)

// The buttons' tooltips (0xBA8210)
static const char *Unknown_BA8210[2] = { "EXIT", "CLOSE" };	// NAME 1: neutral (D10)

// 0x81E030
UIGuildListClass::UIGuildListClass(SClient *client) :
	ChatPanelUIBase(client),
	Slider(NULL),
	Top(0),
	Unknown0A8(0),
	Unknown0AC(0)
{
	memset(Unknown0BC, 0, sizeof(Unknown0BC));
	for (int i = 0; i < 9; i++) {
		Unknown0C4[i] = NULL;
	}
}

// 0x81E1E0
UIGuildListClass::~UIGuildListClass()
{
	Unknown_5C();
	if (Slider) {
		Slider->Unknown_66E460(RefCountPtr<RenderObjClass>(), 0, 0);
	}
}

// 0x81E3E0
void UIGuildListClass::Unknown_54()
{
	if (Unknown_98("tsi36a", "fa_tsi36a.ini")) {
		int i;
		for (i = 0; i < 2; i++) {
			Unknown0BC[i] = Add_Button(i + 1, Unknown_BA8204[i], NULL);
			GadgetClass::Set_Tooltip(Unknown0BC[i], "UI_GUILD_LIST", Unknown_BA8210[i]);
		}
		for (i = 0; i < 9; i++) {
			char name[0x20];
			_snprintf(name, sizeof(name), "name%02d_TSI36a", i);
			Unknown0C4[i] = Create_Peek(Unknown_BE67C4->Get_Style(name, Interface_Mesh()));
		}
		Slider = Add_Slider(0x0E, 0);
		Slider->Unknown_66E460(InterfaceMesh, 0, 0);
		Add_Back_Gadget(1, Vector3(0.0f, 0.0f, 0.0f));
	}
}

// 0x81E5C0
void UIGuildListClass::Unknown_58()
{
	if (!Is_Active()) {
		ChatPanelUIBase::Unknown_58();
		Unknown0AC.clear();
		Top = -1;
		Unknown_81E9A0(0);
		Slider->Unknown_A4(0.0f, false);
		Unknown_81EC50(true);
		if (Unknown0BC[0]) {
			((RObjGadgetClass *)Unknown0BC[0])->Unknown_66C360();
		}
		Unknown0A8 = 0;
	}
}

// 0x81E770
void UIGuildListClass::Unknown_5C()
{
	if (Is_Active()) {
		Unknown0A8 = 0;
		Unknown_81EC50(false);
		Unknown_81EB90();
		InterfaceClass::Unknown_5C();
	}
}

// 0x81E860
void UIGuildListClass::Unknown_60()
{
	Unknown_81EA70();
	Slider->Unknown_66FD80(false);
}

// 0x81E920
bool UIGuildListClass::Unknown_68(int key)
{
	if (key > 0 && key <= 2) {
		SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
		Unknown0A8 = 1;
		return(true);
	}
	return(false);
}

// 0x81E970
void UIGuildListClass::Unknown_7C(UIViewClass *view, bool arg)
{
	if (View != view) {
		InterfaceClass::Unknown_7C(view, arg);
	}
}

// 0x81E9A0
void UIGuildListClass::Unknown_81E9A0(int count)
{
	Slider->Unknown_66FA90((float)count, 9.0f, true);
	Unknown_81EA70();
}

// 0x81EA70
void UIGuildListClass::Unknown_81EA70()
{
	int top = (int)Slider->Value;
	if (Top != top) {
		Top = top;
		Unknown_81EB10();
	}
}

// 0x81EB10
void UIGuildListClass::Unknown_81EB10()
{
	for (int i = 0; i < 9; i++) {
		unsigned int index = Top + i;
		if (index < Unknown0AC.size()) {
			Unknown0C4[i]->Change_Text(Unknown0AC[index].c_str(), 0);
		} else {
			Unknown0C4[i]->Change_Text("", 0);
		}
	}
}

// 0x81EB90
void UIGuildListClass::Unknown_81EB90()
{
	Unknown0AC.clear();
}

// 0x81EC50
void UIGuildListClass::Unknown_81EC50(bool on)
{
	for (int i = 0; i < 2; i++) {
		if (Unknown0BC[i]) {
			Unknown0BC[i]->Unknown_40(on);
		}
	}
}

// 0x81EC90
void UIGuildListClass::Unknown_81EC90(std::vector<std::string> lines)
{
	std::vector<std::string>::iterator it;
	for (it = lines.begin(); it != lines.end(); it++) {
		Unknown0AC.push_back(*it);
	}
	Unknown_81EB10();
	Unknown_81E9A0(Unknown0AC.size());
}

// 0x81EEA0
void UIGuildListClass::Unknown_81EEA0(std::vector<std::string> lines)
{
	Unknown0AC.clear();
	std::vector<std::string>::iterator it;
	for (it = lines.begin(); it != lines.end(); it++) {
		Unknown0AC.push_back(*it);
	}
	Top = -1;
	Unknown_81E9A0(Unknown0AC.size());
	Unknown_81EB10();
	Unknown_81EC50(true);
}

/*
**	Reconstruction notes (GL1, 2026-11-09)
**
**	The object runs 0x81E030-0x81F0FF: the constructor 0x81E030, the deleting destructor 0x81E1B0, the destructor
**	0x81E1E0, the slots +0x54 0x81E3E0, +0x58 0x81E5C0, +0x5C 0x81E770, +0x60 0x81E860, +0x68 0x81E920, +0x7C 0x81E970,
**	the helpers 0x81E9A0-0x81EEA0, then the deleting destructor's adjustor entries 0x81F0E0 / 0x81F0F0 (tables 0xB082F8,
**	0xB082EC). The next object (0x81F100, std::map templates) ends before verbmanager.cpp's __FILE__ 0xBA8268. Its data
**	0xBA8204-0xBA8267 (the models, the tooltips, 'name%02d_TSI36a', 'UI_GUILD_LIST', 'tsi36a', 'fa_tsi36a.ini').
*/
