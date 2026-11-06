// Earth & Beyond client (reconstruction): app\client\ui_playeropt.cpp (the client's __FILE__
// 'e:\build\beta\G\app\Client\ui_playeropt.cpp', 0x5D8C40-0x5DAE8F by its assert, its tables and its neighbours).
#include <string.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <list>
#include "ui_playeropt.h"
#include "sclient.h"
#include "gadget.h"
#include "gd_slider.h"
#include "fontmanager.h"
#include "client.h"
#include "..\input\input.h"
#include "..\packets\pkclientchat.h"
#include "co.h"
#include "gd_robj.h"
#include "local_interface_sort.h"
#include "..\audio\soundlib.h"
#include "..\shared\unknown_debug.h"

// The client at InterfaceClass +0x40, as ui_chat_panel.cpp declares it
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (SClient's returns itself)  // NAME 3: ui_manu.cpp's FAIL_IF's
};

extern FontManagerClass *Unknown_BE67C4;				// NAME 1 [Unknown_BE67C4]: 0xBE67C4

// The buttons' models and tooltips (0xB84594)
static const char *Unknown_B84594[] = { "exit", "button01", "button02", "button03" };	// NAME 1: neutral (D10)

// 0x5D8C40
UIPlayerOptClass::UIPlayerOptClass(SClient *client) :
	UIChatPanelBaseClass(client),
	Slider(NULL),
	Top(0),
	Selection(-1),
	Unknown0B0(0),
	Unknown0C0(0)
{
	memset(Unknown0CC, 0, sizeof(Unknown0CC));
	memset(Unknown0DC, 0, sizeof(Unknown0DC));
	for (int i = 0; i < 7; i++) {
		Unknown0F8[i] = NULL;
	}
}

// 0x5D8E90
UIPlayerOptClass::~UIPlayerOptClass()
{
	Unknown0AC->Unknown_5C();
	Unknown0AC = NULL;
	Unknown_5C();
	if (Slider) {
		Slider->Unknown_66E460(RefCountPtr<RenderObjClass>(), 0, 0);
	}
}

// 0x5D9100
void UIPlayerOptClass::Unknown_54()
{
	if (Unknown_98("tsi27a", "fa_tsi27a.ini")) {
		int i;
		for (i = 0; i < 4; i++) {
			Unknown0CC[i] = Unknown_56D510(i + 1, Unknown_B84594[i], NULL);
			GadgetClass::Set_Tooltip(Unknown0CC[i], "UI_PLAYEROPT", Unknown_B84594[i]);
		}
		char name[0x20];
		for (i = 0; i < 7; i++) {
			_snprintf(name, sizeof(name), "name%02d", i);
			Unknown0DC[i] = Unknown_56D510(i + 6, name, NULL);
			Unknown0DC[i]->Unknown2C_0A = true;
			_snprintf(name, sizeof(name), "name%02d_tsi27a", i);
			Unknown0F8[i] = Create_Peek(Unknown_BE67C4->Get_Style(name, Peek_Unknown84()));
		}
		Slider = Unknown_56D8D0(0xE, 0);
		Slider->Unknown_66E460(*(const RefCountPtr<RenderObjClass> *)&Unknown84, 0, 0);
		Unknown0AC = Create_Get(new UIPTradeCreditsClass(Client->As_Space_Client(), 1, 0x14));
		Unknown0AC->Unknown_54();
		Unknown0AC->Unknown0AC[0xCD - 0xAC] = true;
		Unknown0AC->Unknown_64A270("Ignore Player");
		Unknown_56D970(1, Vector3(0.0f, 0.0f, 0.0f));
	}
}

// 0x5D9410
void UIPlayerOptClass::Unknown_58()
{
	if (!Is_Active()) {
		InterfaceSortScopeClass sort(this);
		UIChatPanelBaseClass::Unknown_58();
		Unknown_5D9BB0();
		Unknown_5D9C70();
		Top = -1;
		Unknown_5D9A40(0);
		Unknown_5DA570();
		Unknown_5DA2B0(true);
		if (Unknown0CC[0]) {
			((RObjGadgetClass *)Unknown0CC[0])->Unknown_66C360();
		}
	}
}

// 0x5D9610
void UIPlayerOptClass::Unknown_5C()
{
	if (Active) {
		Unknown_5DA2B0(false);
		Unknown_5D9BB0();
		Unknown_5D9C70();
		InterfaceClass::Unknown_5C();
	}
}

// 0x5D9730
void UIPlayerOptClass::Unknown_60()
{
	Unknown_5D9AC0();
	Unknown_5D9FD0();
	Unknown_5DAAD0();
	Unknown_5DA0D0();
}

// 0x5D9870
bool UIPlayerOptClass::Unknown_68(int key)
{
	switch (key) {
		case 1:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_5C();
			Unknown98 = 1;
			return(true);
		case 2:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown0AC->Unknown_58();
			Unknown0AC->Unknown_64A2B0("");
			return(true);
		case 3:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_5DA340();
			return(true);
		case 4:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_5DAB50();
			return(true);
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			Unknown_5DA060(key - 6);
			return(true);
	}
	return(false);
}

// 0x5D99F0
void UIPlayerOptClass::Unknown_7C(UIViewClass *view, bool arg)
{
	if (View != view) {
		if (Unknown0AC) {
			Unknown0AC->Unknown_7C(view, false);
		}
		InterfaceClass::Unknown_7C(view, arg);
	}
}

// 0x5D9A40
void UIPlayerOptClass::Unknown_5D9A40(int count)
{
	Slider->Unknown_66FA90((float)count, 7.0f, true);
	Unknown_5D9AC0();
}

// 0x5D9AC0
void UIPlayerOptClass::Unknown_5D9AC0()
{
	int top = (int)Slider->Value;
	if (Top != top) {
		Top = top;
		Unknown_5D9B10();
	}
	Slider->Unknown_66FD80(false);
}

// 0x5D9B10
void UIPlayerOptClass::Unknown_5D9B10()
{
	for (int i = 0; i < 7; i++) {
		unsigned int index = Top + i;
		if (index < Unknown0B0.size()) {
			Unknown0F8[i]->Change_Text(Unknown0B0[index].c_str(), 0);
			Unknown0DC[i]->Unknown_40(true);
		} else {
			Unknown0F8[i]->Change_Text("", 0);
			Unknown0DC[i]->Unknown_40(false);
		}
	}
}

// 0x5D9BB0
void UIPlayerOptClass::Unknown_5D9BB0()
{
	Unknown0B0.clear();
}

// 0x5D9C70
void UIPlayerOptClass::Unknown_5D9C70()
{
	Unknown0C0.clear();
}

// NOT EXACT: inline budget (the client calls the vector's erase out of line, keeps the list iterator in a stack slot and rotates the loop)
// 0x5D9D10
void UIPlayerOptClass::Unknown_5D9D10()
{
	Unknown_5D9BB0();
	for (std::list<std::string>::iterator node = Unknown0C0.begin(); node != Unknown0C0.end(); node++) {
		std::string name = *node;
		std::vector<std::string>::iterator pos;
		for (pos = Unknown0B0.begin(); pos != Unknown0B0.end(); pos++) {
			std::string other = *pos;
			if (wcscoll((const wchar_t *)name.c_str(), (const wchar_t *)other.c_str()) < 0) {
				break;
			}
		}
		Unknown0B0.insert(pos, name);
	}
	Top = -1;
	Unknown_5D9A40(Unknown0B0.size());
	Selection = -1;
	Unknown0CC[2]->Unknown_40(false);
}

// 0x5D9FD0
void UIPlayerOptClass::Unknown_5D9FD0()
{
	float x;
	float y;
	Unknown_42ECF0(&x, &y);
	for (int i = 0; i < 7; i++) {
		if (Unknown0DC[i]->Unknown_30(x, y) || Top + i == Selection) {
			Unknown0F8[i]->Unknown_6CE910(true);
		} else {
			Unknown0F8[i]->Unknown_6CE910(false);
		}
	}
}

// 0x5DA060
void UIPlayerOptClass::Unknown_5DA060(int index)
{
	int size = Unknown0B0.size();
	int selection = Top + index;
	if (selection < size) {
		Selection = selection;
		Unknown0CC[2]->Unknown_40(true);
	}
}

// 0x5DA0D0
void UIPlayerOptClass::Unknown_5DA0D0()
{
	if (Unknown0AC.Peek() != NULL && Unknown0AC->Is_Active()) {
		int state = Unknown0AC->Unknown_64A220();
		if (state) {
			int length;
			if (state == 2 && (length = strlen(Unknown0AC->Unknown_64A240())) != 0) {
				int id = ((ClientClass *)Client)->Unknown1138;
				ClientChatRequestPacketClass packet(10, id);
				packet.Unknown14 = std::string(Unknown0AC->Unknown_64A240());
				((ClientClass *)Client)->Send_Packet(&packet);
			}
			Unknown0AC->Unknown_5C();
		}
	}
}

// 0x5DA2B0
void UIPlayerOptClass::Unknown_5DA2B0(bool on)
{
	int i;
	for (i = 0; i < 4; i++) {
		if (Unknown0CC[i]) {
			Unknown0CC[i]->Unknown_40(on);
		}
	}
	for (i = 0; i < 7; i++) {
		if (Unknown0DC[i]) {
			Unknown0DC[i]->Unknown_40(on);
		}
	}
	if (!on) {
		return;
	}
	if (Selection < 0) {
		Unknown0CC[2]->Unknown_40(false);
	}
}




















































































// 0x5DA340
void UIPlayerOptClass::Unknown_5DA340()
{
	int list_size = Unknown0B0.size();
	FAIL_IF(Selection < 0 || Selection >= list_size) return;
	std::string name = Unknown0B0[Selection];
	int id = ((ClientClass *)Client)->Unknown1138;
	ClientChatRequestPacketClass packet(0xB, id);
	packet.Unknown14 = std::string(name.c_str());
	((ClientClass *)Client)->Send_Packet(&packet);
}

// 0x5DA570
void UIPlayerOptClass::Unknown_5DA570()
{
	int id = ((ClientClass *)Client)->Unknown1138;
	ClientChatRequestPacketClass packet(0x17, id);
	((ClientClass *)Client)->Send_Packet(&packet);
}

// 0x5DA600
void UIPlayerOptClass::Unknown_5DA600(std::vector<std::string> names)
{
	Unknown_5D9C70();
	for (std::vector<std::string>::iterator it = names.begin(); it != names.end(); it++) {
		Unknown0C0.push_back(*it);
	}
	Unknown_5D9D10();
	Unknown_5D9B10();
	Unknown_5DA2B0(true);
}

// 0x5DA780
void UIPlayerOptClass::Unknown_5DA780(std::string name)
{
	Unknown0C0.push_back(name);
	Unknown_5D9D10();
	Unknown_5D9B10();
}

// 0x5DA8C0
void UIPlayerOptClass::Unknown_5DA8C0(std::string name)
{
	for (std::list<std::string>::iterator it = Unknown0C0.begin(); it != Unknown0C0.end(); it++) {
		std::string other = *it;
		if (strcmp(other.c_str(), name.c_str()) == 0) {
			Unknown0C0.erase(it);
			break;
		}
	}
	Unknown_5D9D10();
	Unknown_5D9B10();
}

// 0x5DAAD0
void UIPlayerOptClass::Unknown_5DAAD0()
{
	SClient *sclient = Client->As_Space_Client();
	ClientObjectClass *target;
	if (!sclient->Unknown_744B60() && sclient && sclient->MainView && (target = sclient->MainView->Unknown_5C()) != NULL && target->Unknown_6F5A40()) {
		Unknown0CC[3]->Unknown_40(true);
	} else {
		Unknown0CC[3]->Unknown_40(false);
	}
}

// 0x5DAB50
void UIPlayerOptClass::Unknown_5DAB50()
{
	ClientObjectClass *target = Unknown_5DAD50();
	if (target) {
		std::string name;
		target->Get_Name(name);
		int id = ((ClientClass *)Client)->Unknown1138;
		ClientChatRequestPacketClass packet(10, id);
		packet.Unknown14 = std::string(name.c_str());
		((ClientClass *)Client)->Send_Packet(&packet);
	}
}

// 0x5DAD50
ClientObjectClass *UIPlayerOptClass::Unknown_5DAD50()
{
	SClient *sclient = Client->As_Space_Client();
	if (sclient->Unknown_744B60()) {
		return(NULL);
	}
	ClientObjectClass *target = NULL;
	if (sclient && sclient->MainView) {
		target = sclient->MainView->Unknown_5C();
		if (target == NULL || !target->Unknown_6F5A40()) {
			return(NULL);
		}
	}
	return(target);
}

/*
**	Reconstruction notes (CP1, 2026-11-06)
**
**	The object runs 0x5D8C40-0x5DAE8F: the constructor, the destructor, the slots +0x54-+0x7C and the list helpers
**	in address order, then the list's insert(it, n, x) 0x5DADC0, a static initialiser 0x5DAE40 and the deleting
**	destructor's two this-adjusting entries 0x5DAE70 / 0x5DAE80 (tables 0xAF58A4, 0xAF5898). ui_popup_browser.cpp's
**	window follows at 0x5DAE90. The window is ui_friends.cpp's twin: the same slider, rows and request packets over
**	a list of names (requests 0x17, 10, 11); its text entry is ui_ptrade.cpp's credits entry (0x649890) asking
**	'Ignore Player'. Compiler-generated: 0x5D8E60 (deleting destructor), 0x5DADC0, 0x5DAE40, 0x5DAE70, 0x5DAE80.
*/
