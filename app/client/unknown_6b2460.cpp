// Earth & Beyond client (reconstruction): app\client\unknown_6b2460.cpp, the GM tools' mission log window
// ('tsi27a'). No string names the file (no asserts): the name is provisional (NAME 1, D137). Notes at the end.
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include "unknown_6b2460.h"
#include "client.h"
#include "gadget.h"
#include "gd_robj.h"
#include "gd_slider.h"
#include "fontmanager.h"
#include "local_interface_sort.h"
#include "..\input\input.h"
#include "..\packets\pkgmresetmission.h"
#include "..\audio\soundlib.h"

extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)

// The buttons Unknown_54 makes: their models' names (0xB91C40)
static const char *Unknown_B91C40[4] = { "exit", "button01", "button02", "button03" };	// NAME 1: neutral (D10)

// 0x6B2460
Unknown6B2460Class::Unknown6B2460Class(SClient *client) :
	ChatPanelUIBase(client),
	Slider(NULL),
	Top(0),
	Selected(-1),
	UnknownF4(0)
{
	memset(Buttons, 0, sizeof(Buttons));
	memset(Rows, 0, sizeof(Rows));
	for (int i = 0; i < 7; i++) {
		RowTexts[i] = NULL;
	}
}

// 0x6B25D0
Unknown6B2460Class::~Unknown6B2460Class()
{
	Unknown_5C();
	if (Slider) {
		Slider->Unknown_66E460(RefCountPtr<RenderObjClass>(), 0, 0);
	}
}

// 0x6B27E0
void Unknown6B2460Class::Unknown_54()
{
	if (Unknown_98("tsi27a", "fa_tsi27a.ini")) {
		char name[0x40];
		int i;
		for (i = 0; i < 4; i++) {
			Buttons[i] = Unknown_56D510(i + 1, Unknown_B91C40[i], NULL);
			if (i > 0) {
				_snprintf(name, sizeof(name), "button%02d_tsi27a", i);
				FontSys *style = Unknown_BE67C4->Get_Style(name, Peek_Unknown84());
				if (style) {
					if (i == 1) {
						style->Change_Text("Reset", 0);
					} else if (i == 2) {
						style->Change_Text("Reset All", 0);
					} else if (i == 3) {
						style->Change_Text("", 0);
					}
				}
			}
		}
		((RObjGadgetClass *)Buttons[3])->Unknown_8C(true);
		for (i = 0; i < 7; i++) {
			_snprintf(name, 0x20, "name%02d", i);
			Rows[i] = Unknown_56D510(i + 6, name, NULL);
			Rows[i]->Unknown2C_0A = true;
			_snprintf(name, 0x20, "name%02d_tsi27a", i);
			RowTexts[i] = Create_Peek(Unknown_BE67C4->Get_Style(name, Peek_Unknown84()));
		}
		FontSys *title = Unknown_BE67C4->Get_Style("ignore00_tsi27a", Peek_Unknown84());
		if (title) {
			title->Change_Text("Mission Log", 0);
		}
		Slider = Unknown_56D8D0(0xE, 0);
		Slider->Unknown_66E460(*(const RefCountPtr<RenderObjClass> *)&Unknown84, 0, 0);
		Unknown_56D970(1, Vector3(0.0f, 0.0f, 0.0f));
	}
}

// 0x6B2AB0
void Unknown6B2460Class::Unknown_58()
{
	if (!Is_Active()) {
		InterfaceSortScopeClass sort(this);
		ChatPanelUIBase::Unknown_58();
		Unknown_6B3980();
		Top = -1;
		Unknown_6B2F60(0);
		Unknown_6B32B0(true);
		if (Buttons[0]) {
			((RObjGadgetClass *)Buttons[0])->Unknown_66C360();
		}
		ChatPanelUIBase::Unknown_58();
	}
}

// 0x6B2C30
void Unknown6B2460Class::Unknown_5C()
{
	if (Active) {
		Unknown_6B32B0(false);
		Unknown_6B3980();
		InterfaceClass::Unknown_5C();
	}
}

// 0x6B2D10
void Unknown6B2460Class::Unknown_60()
{
	Unknown_6B2FE0();
	Unknown_6B31A0();
}

// 0x6B2DF0
bool Unknown6B2460Class::Unknown_68(int key)
{
	switch (key) {
		case 1:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_5C();
			Unknown98 = 1;
			return(true);
		case 2:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_6B35F0();
			return(true);
		case 3:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_6B3810();
			return(true);
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			Unknown_6B3240(key - 6);
			return(true);
	}
	return(false);
}

// 0x6B2F30
void Unknown6B2460Class::Unknown_7C(UIViewClass *view, bool arg)
{
	if (View != view) {
		InterfaceClass::Unknown_7C(view, arg);
	}
}

// 0x6B2F60
void Unknown6B2460Class::Unknown_6B2F60(int count)
{
	Slider->Unknown_66FA90((float)count, 7.0f, true);
	Unknown_6B2FE0();
}

// 0x6B2FE0
void Unknown6B2460Class::Unknown_6B2FE0()
{
	int top = (int)Slider->Value;
	if (Top != top) {
		Top = top;
		Unknown_6B3030();
	}
	Slider->Unknown_66FD80(false);
}

// 0x6B3030
void Unknown6B2460Class::Unknown_6B3030()
{
	for (int i = 0; i < 7; i++) {
		unsigned int index = Top + i;
		if (index < Items.size()) {
			RowTexts[i]->Change_Text(Items[index].c_str(), 0);
			Rows[i]->Unknown_40(true);
		} else {
			RowTexts[i]->Change_Text("", 0);
			Rows[i]->Unknown_40(false);
		}
	}
}

// 0x6B30D0
void Unknown6B2460Class::Unknown_6B30D0()
{
	Top = -1;
	Unknown_6B2F60(Items.size());
	Selected = -1;
	Buttons[1]->Unknown_40(false);
}

// 0x6B31A0
void Unknown6B2460Class::Unknown_6B31A0()
{
	float x = 0.0f;
	float y = 0.0f;
	Unknown_42ECF0(&x, &y);
	for (int i = 0; i < 7; i++) {
		if (Rows[i]->Unknown_30(x, y) || Top + i == Selected) {
			RowTexts[i]->Unknown_6CE910(true);
		} else {
			RowTexts[i]->Unknown_6CE910(false);
		}
	}
}

// 0x6B3240
void Unknown6B2460Class::Unknown_6B3240(int row)
{
	int size = Items.size();
	if (Top + row < size) {
		Selected = Top + row;
		Buttons[1]->Unknown_40(true);
	}
}

// 0x6B32B0
void Unknown6B2460Class::Unknown_6B32B0(bool on)
{
	int i;
	for (i = 0; i < 4; i++) {
		if (Buttons[i]) {
			Buttons[i]->Unknown_40(on);
		}
	}
	for (i = 0; i < 7; i++) {
		if (Rows[i]) {
			Rows[i]->Unknown_40(on);
		}
	}
	if (on) {
		if (Selected < 0) {
			Buttons[1]->Unknown_40(false);
		}
	}
}

// 0x6B3340
void Unknown6B2460Class::Unknown_6B3340(int unknown, std::vector<std::string> *strings)
{
	UnknownF4 = unknown;
	Items = *strings;
	Unknown_6B38B0(0);
	Unknown_6B32B0(true);
}

// 0x6B35F0
void Unknown6B2460Class::Unknown_6B35F0()
{
	int index = Selected;
	if (index != -1) {
		index += Top;
		GMResetMissionPacketClass packet(UnknownF4, Items[index].c_str(), false);
		((ClientClass *)Client)->Send_Packet(&packet);
		Items.erase(Items.begin() + index);
		Unknown_6B38B0(0);
	}
}

// 0x6B3810
void Unknown6B2460Class::Unknown_6B3810()
{
	GMResetMissionPacketClass packet(UnknownF4, NULL, true);
	((ClientClass *)Client)->Send_Packet(&packet);
	Unknown_5C();
	Unknown98 = 1;
}

// 0x6B38B0
void Unknown6B2460Class::Unknown_6B38B0(int unknown)
{
	Unknown_6B30D0();
	Unknown_6B3030();
}

// 0x6B3980
void Unknown6B2460Class::Unknown_6B3980()
{
	Items.clear();
	UnknownF4 = -1;
}

/*
**	Notes.
**	Object 0x6B2460-0x6B3A8F: the constructor 0x6B2460 opens it, this window's adjustor thunks 0x6B3A70 / 0x6B3A80
**	close it (user_click_indicator.cpp's 0x6B3A90 follows; the sibling window unknown_6af220.cpp, GM_EDIT_PLAYER_
**	INVENTORY's, precedes it). cpacket.cpp's 0x73F1F0 makes it for GM_VIEW_MISSION_LOG 235 (it includes this
**	window's header). The base is ChatPanelUIBase (0x56D280, 40 slots); the layout is unknown_6af220.cpp's without
**	its map of headings. 0x6B35F0 reads Items[Selected + Top] although 0x6B3240 already stores Top plus the row in
**	Selected: the client's own code, kept.
*/
