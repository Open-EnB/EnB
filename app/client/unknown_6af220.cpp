// Earth & Beyond client (reconstruction): app\client\unknown_6af220.cpp, the GM tools' player inventory window
// ('tsi27a'). No string names the file (no asserts): the name is provisional (NAME 1, D137). Notes at the end.
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>
#include "unknown_6af220.h"
#include "client.h"
#include "gadget.h"
#include "gd_robj.h"
#include "gd_slider.h"
#include "fontmanager.h"
#include "local_interface_sort.h"
#include "..\input\input.h"
#include "..\packets\pkgmdestroyitem.h"
#include "..\audio\soundlib.h"

extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)

// The buttons Unknown_54 makes: their models' names (0xB91B80)
static const char *Unknown_B91B80[4] = { "exit", "button01", "button02", "button03" };	// NAME 1: neutral (D10)

// 0x6AF220
Unknown6AF220Class::Unknown6AF220Class(SClient *client) :
	ChatPanelUIBase(client),
	Slider(NULL),
	Top(0),
	Selected(-1),
	Unknown104(0)
{
	memset(Buttons, 0, sizeof(Buttons));
	memset(Rows, 0, sizeof(Rows));
	for (int i = 0; i < 7; i++) {
		RowTexts[i] = NULL;
	}
	Names["InvShipEquipped"] = "Equipped:";
	Names["InvShipCargo"] = "Cargo:";
	Names["InvPlayerSecure"] = "Vault:";
}

// NOT EXACT: inline budget (explain.py: shape, 7 rows once registers are masked): the client folds the expanded erase's copy (begin == end) and calls the vector's _Destroy 0x432C30 out of line; ours either keeps the copy's end in a stack slot (this form) or, with slot +0x5C's body written out here, folds it and expands _Destroy
// 0x6AF810
Unknown6AF220Class::~Unknown6AF220Class()
{
	Unknown_5C();
	if (Slider) {
		Slider->Unknown_66E460(RefCountPtr<RenderObjClass>(), 0, 0);
	}
}

// 0x6AFB10
void Unknown6AF220Class::Unknown_54()
{
	if (Unknown_98("tsi27a", "fa_tsi27a.ini")) {
		char name[0x40];
		int i;
		for (i = 0; i < 4; i++) {
			Buttons[i] = Unknown_56D510(i + 1, Unknown_B91B80[i], NULL);
			if (i > 0) {
				_snprintf(name, sizeof(name), "button%02d_tsi27a", i);
				FontSys *style = Unknown_BE67C4->Get_Style(name, Peek_Unknown84());
				if (style) {
					if (i == 1) {
						style->Change_Text("Remove", 0);
					} else if (i == 2) {
						style->Change_Text("Remove All", 0);
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
			title->Change_Text("Inventory", 0);
		}
		Slider = Unknown_56D8D0(0xE, 0);
		Slider->Unknown_66E460(*(const RefCountPtr<RenderObjClass> *)&Unknown84, 0, 0);
		Unknown_56D970(1, Vector3(0.0f, 0.0f, 0.0f));
	}
}

// NOT EXACT: register-only (explain.py): the expanded erase's copy loop walks ebp where ours walks ebx, and the -1 for +0x104 / Top is in edi where ours has eax
// 0x6AFDE0
void Unknown6AF220Class::Unknown_58()
{
	if (!Is_Active()) {
		InterfaceSortScopeClass sort(this);
		ChatPanelUIBase::Unknown_58();
		Unknown_6B1000();
		Top = -1;
		Unknown_6B02C0(0);
		Unknown_6B0940(true);
		if (Buttons[0]) {
			((RObjGadgetClass *)Buttons[0])->Unknown_66C360();
		}
		ChatPanelUIBase::Unknown_58();
	}
}

// 0x6AFF60
void Unknown6AF220Class::Unknown_5C()
{
	if (Active) {
		Unknown_6B0940(false);
		Unknown_6B1000();
		InterfaceClass::Unknown_5C();
	}
}

// 0x6B0040
void Unknown6AF220Class::Unknown_60()
{
	Unknown_6B0340();
	Unknown_6B06C0();
}

// 0x6B0090
bool Unknown6AF220Class::Unknown_68(int key)
{
	switch (key) {
		case 1:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_5C();
			Unknown98 = 1;
			return(true);
		case 2:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_6B0C80();
			return(true);
		case 3:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_6B0E90();
			return(true);
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			Unknown_6B0820(key - 6);
			Items.size();	// dead read (D130, not the client's code): spends the inline budget so the expanded 0x6B0820 calls the map's lower_bound 0x6B18E0 out of line as the client does
			return(true);
	}
	return(false);
}

// 0x6B0290
void Unknown6AF220Class::Unknown_7C(UIViewClass *view, bool arg)
{
	if (View != view) {
		InterfaceClass::Unknown_7C(view, arg);
	}
}

// 0x6B02C0
void Unknown6AF220Class::Unknown_6B02C0(int count)
{
	Slider->Unknown_66FA90((float)count, 7.0f, true);
	Unknown_6B0340();
}

// 0x6B0340
void Unknown6AF220Class::Unknown_6B0340()
{
	int top = (int)Slider->Value;
	if (Top != top) {
		Top = top;
		Unknown_6B0460();
	}
	Slider->Unknown_66FD80(false);
}

// 0x6B0390
bool Unknown6AF220Class::Unknown_6B0390(unsigned int index)
{
	if (index < Items.size()) {
		if (Names.find(Items[index]) != Names.end()) {
			return(true);
		}
	}
	return(false);
}

// 0x6B0460
void Unknown6AF220Class::Unknown_6B0460()
{
	for (int i = 0; i < 7; i++) {
		unsigned int index = Top + i;
		if (index < Items.size()) {
			if (Unknown_6B0390(index)) {
				RowTexts[i]->Change_Text(Names[Items[index]].c_str(), 0);
				Rows[i]->Unknown_40(false);
			} else {
				RowTexts[i]->Change_Text(Items[index].c_str(), 0);
				Rows[i]->Unknown_40(true);
			}
		} else {
			RowTexts[i]->Change_Text("", 0);
			Rows[i]->Unknown_40(false);
		}
	}
}

// 0x6B05F0
void Unknown6AF220Class::Unknown_6B05F0()
{
	Top = -1;
	Unknown_6B02C0(Items.size());
	Selected = -1;
	Buttons[1]->Unknown_40(false);
}

// 0x6B06C0
void Unknown6AF220Class::Unknown_6B06C0()
{
	float x = 0.0f;
	float y = 0.0f;
	Names.size();	// dead read (D130, not the client's code): spends the inline budget so slot +0x60 calls this function out of line as the client does
	Unknown_42ECF0(&x, &y);
	for (int i = 0; i < 7; i++) {
		if (Rows[i]->Unknown_30(x, y) || Top + i == Selected || Unknown_6B0390(Top + i)) {
			RowTexts[i]->Unknown_6CE910(true);
		} else {
			RowTexts[i]->Unknown_6CE910(false);
		}
	}
}

// 0x6B0820
void Unknown6AF220Class::Unknown_6B0820(int row)
{
	int size = Items.size();
	if (Top + row < size) {
		int index = Top + row;
		Selected = index;
		if (Unknown_6B0390(index)) {
			Buttons[1]->Unknown_40(false);
		} else {
			Buttons[1]->Unknown_40(true);
		}
	}
}

// 0x6B0940
void Unknown6AF220Class::Unknown_6B0940(bool on)
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

// 0x6B09D0
void Unknown6AF220Class::Unknown_6B09D0(int unknown, std::vector<std::string> *strings)
{
	Unknown104 = unknown;
	Items = *strings;
	Unknown_6B0F30(0);
	Unknown_6B0940(true);
}

// 0x6B0C80
void Unknown6AF220Class::Unknown_6B0C80()
{
	int index = Selected;
	if (index != -1) {
		GMDestroyItemPacketClass packet(Unknown104, Items[index].c_str(), false);
		((ClientClass *)Client)->Send_Packet(&packet);
		Items.erase(Items.begin() + index);
		Unknown_6B0F30(0);
	}
}

// 0x6B0E90
void Unknown6AF220Class::Unknown_6B0E90()
{
	GMDestroyItemPacketClass packet(Unknown104, NULL, true);
	((ClientClass *)Client)->Send_Packet(&packet);
	Unknown_5C();
	Unknown98 = 1;
}

// 0x6B0F30
void Unknown6AF220Class::Unknown_6B0F30(int unknown)
{
	Unknown_6B05F0();
	Unknown_6B0460();
}

// 0x6B1000
void Unknown6AF220Class::Unknown_6B1000()
{
	Items.erase(Items.begin(), Items.end());
	Unknown104 = -1;
}

/*
**	Notes.
**	Object 0x6AF220-0x6B245F: the constructor 0x6AF220 opens it; its map<string, string> and vector<string>
**	instances 0x6B1250-0x6B23E0 follow its last function 0x6B1000, and this window's adjustor thunks 0x6B2440 /
**	0x6B2450 close it (the sibling window 0x6B2460, GM_VIEW_MISSION_LOG's, follows). cpacket.cpp's 0x73F2E0 makes
**	it for GM_EDIT_PLAYER_INVENTORY 234 through its stand-in of the same name. The base is ChatPanelUIBase
**	(0x56D280, 40 slots), which the table 0xAFDC6C shares slots 1-20, 25, 27-30 and 32-39 with.
**	Every function is written; exact but the destructor 0x6AF810 (inline budget) and slot +0x58 0x6AFDE0
**	(register-only). Two dead reads (D130, marked, not the client's code): Names.size() in 0x6B06C0 and
**	Items.size() in slot +0x68. 0x6B0940's last test is two nested ifs: with 'on && Selected < 0' its four callers
**	expand it, the client calls it. 0x6B0F30 takes one argument it never reads (the client's 'ret 4').
*/
