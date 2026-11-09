// Earth & Beyond client (reconstruction): app\client\ui_radio_list.cpp, the chat channel list window ('cht02').
// No string names the file (no asserts): the name is provisional (NAME 1, D137). Notes at the end.
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <string>
#include <vector>
#include <list>
#include "ui_radio_list.h"
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

// The buttons Unknown_54 makes: their models' names (0xB856CC)
static const char *Unknown_B856CC[3] = { "EXIT", "BUTTON01", "BUTTON02" };	// NAME 1: neutral (D10)

// 0x5E7500
UIRadioListClass::UIRadioListClass(SClient *client, int state) :
	ChatPanelUIBase(client),
	State(state),
	Slider(NULL),
	Top(0),
	Selection(-1)
{
	int i;
	memset(Buttons, 0, sizeof(Buttons));
	memset(Rows, 0, sizeof(Rows));
	for (i = 0; i < 7; i++) {
		Texts[i] = NULL;
	}
}

// 0x5E76B0
// NOT EXACT: inline budget (the client calls the vector's erase 0x4FC7C0 and the list's erase(first, last) out of line; ours expands both)
UIRadioListClass::~UIRadioListClass()
{
	if (Active) {
		Unknown_5E8860(false);
		Names.erase(Names.begin(), Names.end());
		Unknown0C0.erase(Unknown0C0.begin(), Unknown0C0.end());
		InterfaceClass::Unknown_5C();
	}
	if (Slider) {
		Slider->Unknown_66E460(RefCountPtr<RenderObjClass>(), 0, 0);
	}
}

// 0x5E78B0
void UIRadioListClass::Unknown_54()
{
	if (Unknown_98("cht02", "cht02.ini")) {
		int i;
		for (i = 0; i < 3; i++) {
			Buttons[i] = Unknown_56D510(i + 1, Unknown_B856CC[i], NULL);
		}
		for (i = 0; i < 7; i++) {
			char name[0x20];	// NAME 1: neutral (D10)
			_snprintf(name, sizeof(name), "NAME%02d", i);
			Rows[i] = Unknown_56D510(i + 5, name, NULL);
			Rows[i]->Unknown2C_0A = true;
			_snprintf(name, sizeof(name), "name%02d_Cht02", i);
			Texts[i] = Create_Peek(Unknown_BE67C4->Get_Style(name, Interface_Mesh()));
		}
		Unknown_5E8C20();
		Slider = Unknown_56D8D0(0x0D, 0);
		Slider->Unknown_66E460(Unknown84, 0, 0);
		Unknown_56D970(0, Vector3(0.0f, 0.0f, 0.0f));
	}
}

// 0x5E7AD0
void UIRadioListClass::Unknown_58()
{
	if (!Active) {
		InterfaceSortScopeClass sort(this);	// NAME 1: neutral (D10)
		ChatPanelUIBase::Unknown_58();
		Unknown_5E80E0();
		Unknown_5E81A0();
		Top = -1;
		Unknown_5E7F90(0);
		Unknown_5E8930();
		Unknown_5E8860(true);
		Unknown_5E8040();
		if (Buttons[0]) {
			((RObjGadgetClass *)Buttons[0])->Unknown_66C360();
		}
	}
}

// 0x5E7C80
void UIRadioListClass::Unknown_5C()
{
	if (Active) {
		Unknown_5E8860(false);
		Unknown_5E80E0();
		Unknown_5E81A0();
		InterfaceClass::Unknown_5C();
	}
}

// 0x5E7DA0
void UIRadioListClass::Unknown_60()
{
	Unknown_5E8000();
	Unknown_5E84F0();
}

// 0x5E7E60
bool UIRadioListClass::Unknown_68(int key)
{
	switch (key) {
		case 1:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_5C();
			Unknown98 = 1;
			return(true);
		case 2:
		case 3:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_5E85F0(key);
			return(true);
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			Unknown_5E8580(key - 5);
			return(true);
	}
	return(false);
}

// 0x5E7F90
void UIRadioListClass::Unknown_5E7F90(int count)
{
	Slider->Unknown_66FA90((float)count, 7.0f, true);
	Unknown_5E8000();
}

// 0x5E8000
void UIRadioListClass::Unknown_5E8000()
{
	int top = (int)Slider->Value;	// NAME 1: neutral (D10)
	if (Top != top) {
		Top = top;
		Unknown_5E8040();
	}
}

// 0x5E8040
void UIRadioListClass::Unknown_5E8040()
{
	for (int i = 0; i < 7; i++) {
		unsigned int index = Top + i;	// NAME 1: neutral (D10)
		if (index < Names.size()) {
			Texts[i]->Change_Text(Names[index].c_str(), 0);
			Rows[i]->Unknown_40(true);
		} else {
			Texts[i]->Change_Text("", 0);
			Rows[i]->Unknown_40(false);
		}
	}
}

// 0x5E80E0
void UIRadioListClass::Unknown_5E80E0()
{
	Names.clear();
}

// 0x5E81A0
void UIRadioListClass::Unknown_5E81A0()
{
	Unknown0C0.clear();
}

// 0x5E8240
// NOT EXACT: inline budget (the client calls the vector's erase 0x4FC7C0 out of line; ours expands it)
void UIRadioListClass::Unknown_5E8240()
{
	Names.erase(Names.begin(), Names.end());
	for (std::list<std::string>::iterator it = Unknown0C0.begin(); it != Unknown0C0.end(); it++) {
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
	Unknown_5E7F90(Names.size());
	Selection = -1;
	Buttons[1]->Unknown_40(false);
}

// 0x5E84F0
void UIRadioListClass::Unknown_5E84F0()
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

// 0x5E8580
void UIRadioListClass::Unknown_5E8580(int index)
{
	if (State != 1) {
		Selection = index + Top;
		if (Selection >= 0 && Selection < (int)Names.size()) {
			Buttons[1]->Unknown_40(true);
		}
	}
}

// 0x5E85F0
// NOT EXACT: inline budget (the client calls basic_string::erase out of line in the assignment's self-assign branch; ours expands it)
void UIRadioListClass::Unknown_5E85F0(int key)
{
	SClient *sclient = (SClient *)((ClientClass *)Client)->Unknown_18();	// NAME 1: neutral (D10)
	ChatPanelInterface *chat = sclient->ChatPanel.Peek();	// NAME 1: neutral (D10)
	switch (State) {
		case 0:
			if (key == 2) {
				Unknown_5C();
				Unknown98 = 1;
				if (Selection >= 0) {
					int id = ((ClientClass *)Client)->Unknown1138;	// NAME 1: neutral (D10)
					ClientChatRequestPacketClass packet(6, id);
					packet.Unknown24 = Names[Selection];
					((ClientClass *)Client)->Send_Packet(&packet);
				}
			} else if (key == 3) {
				Unknown_5C();
				Unknown98 = 1;
				if (chat) {
					chat->Unknown_565F60(2);
				}
			}
			break;
		case 1:
			if (key == 2) {
				Unknown_5C();
				Unknown98 = 1;
				if (chat) {
					chat->Unknown_565F60(1);
				}
			}
			break;
	}
}

// 0x5E8860
void UIRadioListClass::Unknown_5E8860(bool on)
{
	int i;
	for (i = 0; i < 3; i++) {
		if (Buttons[i]) {
			Buttons[i]->Unknown_40(on);
		}
	}
	for (i = 0; i < 7; i++) {
		if (Rows[i]) {
			Rows[i]->Unknown_40(on);
		}
	}
	if (on && Selection < 0) {
		if (State == 0) {
			Buttons[1]->Unknown_40(false);
		} else if (State == 1) {
			((RObjGadgetClass *)Buttons[2])->Unknown_8C(true);
			Buttons[2]->Unknown_40(false);
		}
	}
}

// 0x5E8930
void UIRadioListClass::Unknown_5E8930()
{
	SClient *sclient = (SClient *)((ClientClass *)Client)->Unknown_18();	// NAME 1: neutral (D10)
	ChatPanelInterface *chat = sclient->ChatPanel.Peek();	// NAME 1: neutral (D10)
	if (State == 0) {
		int id = ((ClientClass *)Client)->Unknown1138;	// NAME 1: neutral (D10)
		ClientChatRequestPacketClass packet(0x19, id);
		packet.Unknown24 = chat->Unknown_5682C0();
		((ClientClass *)Client)->Send_Packet(&packet);
	} else if (State == 1) {
		int id = ((ClientClass *)Client)->Unknown1138;	// NAME 1: neutral (D10)
		ClientChatRequestPacketClass packet(0x1B, id);
		packet.Unknown24 = chat->Unknown_5682C0();
		((ClientClass *)Client)->Send_Packet(&packet);
	}
}

// 0x5E8C20
void UIRadioListClass::Unknown_5E8C20()
{
	RefCountPtr<FontSys> title = Create_Peek(Unknown_BE67C4->Get_Style("TITLE_Cht02", Interface_Mesh()));	// NAME 1: neutral (D10)
	RefCountPtr<FontSys> button1 = Create_Peek(Unknown_BE67C4->Get_Style("BUTTON01_Cht02", Interface_Mesh()));	// NAME 1: neutral (D10)
	RefCountPtr<FontSys> button2 = Create_Peek(Unknown_BE67C4->Get_Style("BUTTON02_Cht02", Interface_Mesh()));	// NAME 1: neutral (D10)
	switch (State) {
		case 0:
			if (title) {
				title->Change_Text("Public Channels", 0);
			}
			if (button1) {
				button1->Change_Text("Join", 0);
			}
			if (button2) {
				button2->Change_Text("Private", 0);
			}
			break;
		case 1:
			if (title) {
				title->Change_Text("Channel Members", 0);
			}
			if (button1) {
				button1->Change_Text("OK", 0);
			}
			if (button2) {
				button2->Change_Text(" ", 0);
			}
			break;
	}
}

// 0x5E8E30
void UIRadioListClass::Unknown_5E8E30(int state, std::vector<std::string> names)
{
	if (state != State) {
		return;
	}
	Unknown_5E81A0();
	for (std::vector<std::string>::iterator it = names.begin(); it != names.end(); it++) {
		Unknown0C0.push_back(*it);
	}
	Unknown_5E8240();
	Unknown_5E8860(true);
	Unknown_5E8040();
}

// 0x5E8FF0
void UIRadioListClass::Unknown_5E8FF0(int state, std::string name)
{
	if (state != State) {
		return;
	}
	Unknown0C0.push_back(name);
	Unknown_5E8240();
	Unknown_5E8040();
}

// 0x5E9160
void UIRadioListClass::Unknown_5E9160(int state, std::string name)
{
	if (state != State) {
		return;
	}
	for (std::list<std::string>::iterator it = Unknown0C0.begin(); it != Unknown0C0.end(); it++) {
		std::string other = *it;	// NAME 1: neutral (D10)
		if (strcmp(other.c_str(), name.c_str()) == 0) {
			Unknown0C0.erase(it);
			break;
		}
	}
	Unknown_5E8240();
	Unknown_5E8040();
}

/*
**	Notes.
**	Object 0x5E7500-0x5E940F: the constructor 0x5E7500 opens it, after ui_radio_entry.cpp's adjustor thunks
**	0x5E74E0 / 0x5E74F0; this window's adjustor thunks 0x5E93F0 / 0x5E9400 close it and ui_radio_main.cpp's
**	constructor 0x5E9410 follows. ui_chat_panel.cpp makes it (+0xE4) as its stand-in; cpacket.cpp calls 0x5E8E30,
**	0x5E8FF0 and 0x5E9160 through its stand-in UIChatPanelUnknownE4Class (the state is the first argument: the
**	client pushes it last). Requests: 6 joins the selected channel, 25 (0x19) asks for the public channels, 27 (0x1B)
**	for the channel's members.
*/
