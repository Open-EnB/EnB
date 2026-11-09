// Earth & Beyond client (reconstruction): app\client\ui_radio_admin.cpp, the private channel's admin window ('cht05').
// No string names the file (no asserts): the name is provisional (NAME 1, D137). Notes at the end.
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <string>
#include <vector>
#include <list>
#include "ui_radio_admin.h"
#include "ui_chat_panel.h"
#include "client.h"
#include "gadget.h"
#include "gd_robj.h"
#include "gd_slider.h"
#include "local_interface_sort.h"
#include "..\packets\pkclientchat.h"
#include "..\audio\soundlib.h"

extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)
void Unknown_42ECF0(float *x, float *y);	// 0x42ECF0 (the pointer's position)  // NAME 1 [x, y]: neutral

// The buttons Unknown_54 makes: their models' names, also their tooltips (0xB85344)
static const char *Unknown_B85344[7] = { "EXIT", "BUTTON01", "BUTTON02", "BUTTON03", "BUTTON04", "BUTTON05", "BUTTON06" };	// NAME 1: neutral (D10)

// 0x5E4160
UIRadioAdminClass::UIRadioAdminClass(SClient *client) :
	ChatPanelUIBase(client),
	Slider(NULL),
	Top(0),
	Selection(-1)
{
	memset(Buttons, 0, sizeof(Buttons));
	memset(Rows, 0, sizeof(Rows));
	for (int i = 0; i < 7; i++) {
		Texts[i] = NULL;
	}
}

// 0x5E4300
// NOT EXACT: inline budget (the client calls the vector's erase 0x4FC7C0 and the list's erase(first, last) out of line; ours expands both)
UIRadioAdminClass::~UIRadioAdminClass()
{
	if (Active) {
		Unknown_5E5A40(false);
		Names.erase(Names.begin(), Names.end());
		Unknown0BC.erase(Unknown0BC.begin(), Unknown0BC.end());
		InterfaceClass::Unknown_5C();
	}
	if (Slider) {
		Slider->Unknown_66E460(RefCountPtr<RenderObjClass>(), 0, 0);
	}
}

// 0x5E4500
void UIRadioAdminClass::Unknown_54()
{
	if (Unknown_98("cht05", "cht05.ini")) {
		int i;
		for (i = 0; i < 7; i++) {
			Buttons[i] = Unknown_56D510(i + 1, Unknown_B85344[i], NULL);
			GadgetClass::Set_Tooltip(Buttons[i], "UI_RADIO_ADMIN", Unknown_B85344[i]);
		}
		for (i = 0; i < 7; i++) {
			char name[0x20];	// NAME 1: neutral (D10)
			_snprintf(name, sizeof(name), "NAME%02d", i);
			Rows[i] = Unknown_56D510(i + 9, name, NULL);
			Rows[i]->Unknown2C_0A = true;
			_snprintf(name, sizeof(name), "name%02d_Cht05", i);
			Texts[i] = Create_Peek(Unknown_BE67C4->Get_Style(name, Interface_Mesh()));
		}
		Slider = Unknown_56D8D0(0x11, 0);
		Slider->Unknown_66E460(Unknown84, 0, 0);
		Unknown_56D970(0, Vector3(0.0f, 0.0f, 0.0f));
	}
}

// 0x5E4720
void UIRadioAdminClass::Unknown_58()
{
	if (!Active) {
		InterfaceSortScopeClass sort(this);	// NAME 1: neutral (D10)
		ChatPanelUIBase::Unknown_58();
		Unknown_5E4DB0();
		Unknown_5E4E70();
		Top = -1;
		Unknown_5E4C60(0);
		Unknown_5E5B10();
		Unknown_5E5A40(true);
		if (Buttons[0]) {
			((RObjGadgetClass *)Buttons[0])->Unknown_66C360();
		}
	}
}

// 0x5E49A0
void UIRadioAdminClass::Unknown_5C()
{
	if (Active) {
		Unknown_5E5A40(false);
		Unknown_5E4DB0();
		Unknown_5E4E70();
		InterfaceClass::Unknown_5C();
	}
}

// 0x5E4AC0
void UIRadioAdminClass::Unknown_60()
{
	Unknown_5E4CD0();
	Unknown_5E5200();
}

// 0x5E4B80
bool UIRadioAdminClass::Unknown_68(int key)
{
	switch (key) {
		case 1:
		case 7:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_5C();
			Unknown98 = 1;
			return(true);
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_5E5360(key);
			return(true);
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			Unknown_5E5290(key - 9);
			return(true);
	}
	return(false);
}

// 0x5E4C60
void UIRadioAdminClass::Unknown_5E4C60(int count)
{
	Slider->Unknown_66FA90((float)count, 7.0f, true);
	Unknown_5E4CD0();
}

// 0x5E4CD0
void UIRadioAdminClass::Unknown_5E4CD0()
{
	int top = (int)Slider->Value;	// NAME 1: neutral (D10)
	if (Top != top) {
		Top = top;
		Unknown_5E4D10();
	}
}

// 0x5E4D10
void UIRadioAdminClass::Unknown_5E4D10()
{
	for (int i = 0; i < 7; i++) {
		unsigned int index = Top + i;	// NAME 1: neutral (D10)
		if (index < Names.size()) {
			Texts[i]->Change_Text(Names[index].c_str(), 0);
			Rows[i]->Unknown_40(true);
		} else {
			Texts[i]->Change_Text(" ", 0);
			Rows[i]->Unknown_40(false);
		}
	}
}

// 0x5E4DB0
void UIRadioAdminClass::Unknown_5E4DB0()
{
	Names.clear();
}

// 0x5E4E70
void UIRadioAdminClass::Unknown_5E4E70()
{
	Unknown0BC.clear();
}

// 0x5E4F10
// NOT EXACT: inline budget (the client calls the vector's erase 0x4FC7C0 out of line; ours expands it)
void UIRadioAdminClass::Unknown_5E4F10()
{
	Names.erase(Names.begin(), Names.end());
	for (std::list<std::string>::iterator it = Unknown0BC.begin(); it != Unknown0BC.end(); it++) {
		std::string name = *it;	// NAME 1: neutral (D10)
		std::vector<std::string>::iterator pos;
		for (pos = Names.begin(); pos != Names.end(); pos++) {
			std::string other = *pos;	// NAME 1: neutral (D10)
			if (wcscoll((const wchar_t *)name.c_str(), (const wchar_t *)other.c_str()) < 0) {
				break;
			}
		}
		Names.insert(pos, name);
	}
	Top = -1;
	Unknown_5E4C60(Names.size());
	Selection = -1;
	Buttons[1]->Unknown_40(false);
	Buttons[2]->Unknown_40(false);
	Buttons[3]->Unknown_40(false);
	Buttons[4]->Unknown_40(false);
	Buttons[5]->Unknown_40(false);
}

// 0x5E5200
void UIRadioAdminClass::Unknown_5E5200()
{
	float x;
	float y;
	Unknown_42ECF0(&x, &y);
	for (int i = 0; i < 7; i++) {
		if (Rows[i]->Unknown_30(x, y) || Top + i == Selection) {
			Texts[i]->Unknown_6CE910(true);
		} else {
			Texts[i]->Unknown_6CE910(false);
		}
	}
}

// 0x5E5290
void UIRadioAdminClass::Unknown_5E5290(int index)
{
	Selection = index + Top;
	if (Selection >= 0 && Selection < (int)Names.size()) {
		SClient *sclient = (SClient *)((ClientClass *)Client)->Unknown_18();	// NAME 1: neutral (D10)
		ChatPanelInterface *chat = sclient->ChatPanel.Peek();	// NAME 1: neutral (D10)
		if (chat && chat->Unknown224) {
			Buttons[1]->Unknown_40(true);
			Buttons[2]->Unknown_40(true);
			Buttons[3]->Unknown_40(true);
			Buttons[4]->Unknown_40(true);
			Buttons[5]->Unknown_40(true);
		}
	}
}

// 0x5E5360
// NOT EXACT: inline budget and stack slots (cases 5 and 6: the client destroys the channel's temporary inline and expands the selected name's assignment in case 6, its temporaries at other frame offsets)
void UIRadioAdminClass::Unknown_5E5360(int key)
{
	if (Selection < 0 || Selection >= (int)Names.size()) {
		return;
	}
	SClient *sclient = (SClient *)((ClientClass *)Client)->Unknown_18();	// NAME 1: neutral (D10)
	ChatPanelInterface *chat = sclient->ChatPanel.Peek();	// NAME 1: neutral (D10)
	switch (key) {
		case 2:
		{
			int id = ((ClientClass *)Client)->Unknown1138;	// NAME 1: neutral (D10)
			ClientChatRequestPacketClass packet(0x10, id);
			packet.Unknown24 = chat->Unknown_5682C0();
			packet.Unknown14 = Names[Selection];
			((ClientClass *)Client)->Send_Packet(&packet);
			break;
		}
		case 3:
		{
			int id = ((ClientClass *)Client)->Unknown1138;	// NAME 1: neutral (D10)
			ClientChatRequestPacketClass packet(0x11, id);
			packet.Unknown24 = chat->Unknown_5682C0();
			packet.Unknown14 = Names[Selection];
			((ClientClass *)Client)->Send_Packet(&packet);
			break;
		}
		case 4:
		{
			int id = ((ClientClass *)Client)->Unknown1138;	// NAME 1: neutral (D10)
			ClientChatRequestPacketClass packet(0x0E, id);
			packet.Unknown24 = chat->Unknown_5682C0();
			packet.Unknown14 = Names[Selection];
			((ClientClass *)Client)->Send_Packet(&packet);
			break;
		}
		case 5:
		{
			int id = ((ClientClass *)Client)->Unknown1138;	// NAME 1: neutral (D10)
			ClientChatRequestPacketClass packet(0x14, id);
			packet.Unknown24 = chat->Unknown_5682C0();
			packet.Unknown14 = Names[Selection];
			((ClientClass *)Client)->Send_Packet(&packet);
			break;
		}
		case 6:
		{
			int id = ((ClientClass *)Client)->Unknown1138;	// NAME 1: neutral (D10)
			ClientChatRequestPacketClass packet(0x12, id);
			packet.Unknown24 = chat->Unknown_5682C0();
			packet.Unknown14 = Names[Selection];
			((ClientClass *)Client)->Send_Packet(&packet);
			break;
		}
	}
}

// 0x5E5A40
void UIRadioAdminClass::Unknown_5E5A40(bool on)
{
	int i;
	for (i = 0; i < 7; i++) {
		if (Buttons[i]) {
			Buttons[i]->Unknown_40(on);
		}
	}
	for (i = 0; i < 7; i++) {
		if (Rows[i]) {
			Rows[i]->Unknown_40(on);
		}
	}
	if (Selection < 0) {
		Buttons[1]->Unknown_40(false);
		Buttons[2]->Unknown_40(false);
		Buttons[3]->Unknown_40(false);
		Buttons[4]->Unknown_40(false);
		Buttons[5]->Unknown_40(false);
	}
}

// 0x5E5B10
void UIRadioAdminClass::Unknown_5E5B10()
{
	SClient *sclient = (SClient *)((ClientClass *)Client)->Unknown_18();	// NAME 1: neutral (D10)
	ChatPanelInterface *chat = sclient->ChatPanel.Peek();	// NAME 1: neutral (D10)
	int id = ((ClientClass *)Client)->Unknown1138;	// NAME 1: neutral (D10)
	ClientChatRequestPacketClass packet(0x1B, id);
	packet.Unknown24 = chat->Unknown_5682C0();
	((ClientClass *)Client)->Send_Packet(&packet);
}

// 0x5E5D70
void UIRadioAdminClass::Unknown_5E5D70(std::vector<std::string> names)
{
	Unknown_5E4E70();
	for (std::vector<std::string>::iterator it = names.begin(); it != names.end(); it++) {
		Unknown0BC.push_back(*it);
	}
	Unknown_5E4F10();
	Unknown_5E5A40(true);
	Unknown_5E4D10();
}

// 0x5E5EF0
void UIRadioAdminClass::Unknown_5E5EF0(std::string name)
{
	Unknown0BC.push_back(name);
	Unknown_5E4F10();
	Unknown_5E4D10();
}

// 0x5E6030
void UIRadioAdminClass::Unknown_5E6030(std::string name)
{
	for (std::list<std::string>::iterator it = Unknown0BC.begin(); it != Unknown0BC.end(); it++) {
		std::string other = *it;	// NAME 1: neutral (D10)
		if (strcmp(other.c_str(), name.c_str()) == 0) {
			Unknown0BC.erase(it);
			break;
		}
	}
	Unknown_5E4F10();
	Unknown_5E4D10();
}

/*
**	Notes.
**	Object 0x5E4160-0x5E628F: the constructor 0x5E4160 opens it, after ui_ptrade.cpp's adjustor thunks 0x5E4140 /
**	0x5E4150; this window's adjustor thunks 0x5E6270 / 0x5E6280 close it and ui_radio_entry.cpp's constructor
**	0x5E6290 follows. ui_chat_panel.cpp makes it as its stand-in Unknown5E4160Class (request 10, also kept at +0xE8);
**	cpacket.cpp calls 0x5E5D70, 0x5E5EF0 and 0x5E6030 through its stand-in. Requests: 27 (0x1B) asks for the
**	channel's members; buttons 2-6 send 16, 17, 14, 20 and 18 with the channel (Unknown24) and the selected member
**	(Unknown14).
*/
