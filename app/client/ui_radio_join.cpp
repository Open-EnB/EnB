// Earth & Beyond client (reconstruction): app\client\ui_radio_join.cpp, the private channel's join window ('cht03').
// No string names the file (no asserts): the name is provisional (NAME 1, D137). Notes at the end.
#include <stdio.h>
#include <string.h>
#include <string>
#include "ui_radio_join.h"
#include "ui_chat_panel.h"
#include "client.h"
#include "gadget.h"
#include "gd_robj.h"
#include "textgadg.h"
#include "local_interface_sort.h"
#include "..\packets\pkclientchat.h"
#include "..\audio\soundlib.h"

extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)
extern const int Unknown_B08C98;			// 0xB08C98 (4)  // NAME 1: neutral (D10)
extern const int Unknown_B08CA8;			// 0xB08CA8 (0x40)  // NAME 1: neutral (D10)

// The chat panel's request flag (+0x225), read through an inline as the client does (al loaded, then tested)
static inline bool Is_Request_Pending(ChatPanelInterface *chat) { return(chat->Unknown225); }	// NAME 1: neutral (D10)

// The buttons Unknown_54 makes: their models' names (0xB85AB0)
static const char *Unknown_B85AB0[4] = { "EXIT", "TEXT_FIELD", "BUTTON01", "BUTTON03" };	// NAME 1: neutral (D10)

// 0x5EAB40
UIRadioJoinClass::UIRadioJoinClass(SClient *client, int state) :
	ChatPanelUIBase(client),
	State(state),
	Entry(NULL),
	UnknownA8(false)
{
}

// 0x5EABC0
UIRadioJoinClass::~UIRadioJoinClass()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
	}
}

// 0x5EAC50
void UIRadioJoinClass::Unknown_54()
{
	if (Unknown_98("cht03", "cht03.ini")) {
		for (int i = 0; i < 4; i++) {
			Buttons[i] = Unknown_56D510(i + 1, Unknown_B85AB0[i], NULL);
		}
		Entry = new TextGadgetClass(this, 4, "TEXTFIELD_Cht03", Interface_Mesh(), Unknown_B08CAC | Unknown_B08CA8 | Unknown_B08C98, 0);
		Entry->Set_Text("", 0);
		Unknown64.Unknown_6870E0(Entry, 6, View, 1, 0);
		Unknown_56D970(0, Vector3(0.0f, 0.0f, 0.0f));
		Unknown_5EB140();
	}
}

// 0x5EADD0
void UIRadioJoinClass::Unknown_58()
{
	if (!Active) {
		InterfaceSortScopeClass sort(this);	// NAME 1: neutral (D10)
		ChatPanelUIBase::Unknown_58();
		if (Buttons[0]) {
			((RObjGadgetClass *)Buttons[0])->Unknown_66C360();
		}
		Entry->Unknown_74(true);
		Entry->Set_Text("", 0);
		UnknownA8 = true;
	}
}

// 0x5EAEA0
void UIRadioJoinClass::Unknown_5C()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
	}
}

// 0x5EAEC0
void UIRadioJoinClass::Unknown_60()
{
	if (Entry) {
		Entry->Unknown_6AAD10();
		if (UnknownA8 && !Entry->Active) {
			Unknown_5C();
			Unknown98 = 1;
			Unknown_5EB2F0();
		}
	}
}

// 0x5EAF30
bool UIRadioJoinClass::Unknown_68(int key)
{
	SClient *sclient = (SClient *)((ClientClass *)Client)->Unknown_18();	// NAME 1: neutral (D10)
	ChatPanelInterface *chat = sclient->ChatPanel.Peek();	// NAME 1: neutral (D10)
	switch (key) {
		case 1:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Entry->Unknown_74(false);
			Unknown_5C();
			Unknown98 = 1;
			return(true);
		case 2:
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			if (Entry) {
				Entry->Unknown_74(true);
				Entry->Set_Text("", 0);
				UnknownA8 = true;
			}
			return(true);
		case 3:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Entry->Unknown_74(false);
			Unknown_5C();
			Unknown98 = 1;
			if (chat) {
				chat->Unknown_565F60(2);
			}
			return(true);
		case 4:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Entry->Unknown_74(false);
			Unknown_5C();
			Unknown98 = 1;
			Unknown_5EB2F0();
			return(true);
	}
	return(false);
}

// 0x5EB0B0
void UIRadioJoinClass::Unknown_5EB0B0(RefCountPtr<FontSys> font, const char *text)
{
	if (font) {
		font->Change_Text(text, 0);
	}
}

// 0x5EB140
void UIRadioJoinClass::Unknown_5EB140()
{
	RefCountPtr<FontSys> title = Create_Peek(Unknown_BE67C4->Get_Style("TITLE01_Cht03", Interface_Mesh()));	// NAME 1: neutral (D10)
	RefCountPtr<FontSys> wrap = Create_Peek(Unknown_BE67C4->Get_Style("WRAP_TITLE01_Cht03", Interface_Mesh()));	// NAME 1: neutral (D10)
	ManagedFontClass *font = Unknown_BE67C4->Find(Interface_Mesh());	// NAME 1: neutral (D10)
	switch (State) {
		case 0:
			Unknown_5EB0B0(title, "Join Private Channel");
			font->Unknown_6D0A70("WRAP_TITLE01_Cht03", "Enter the channel number you wish to join.");
			break;
		case 1:
			Unknown_5EB0B0(title, "Channel Does not exist");
			font->Unknown_6D0A70("WRAP_TITLE01_Cht03", "The channel you have selected does not exist. Please enter another channel.");
			break;
	}
}

// 0x5EB2F0
// NOT EXACT: the string temporaries' inline expansions and register choices differ (inline budget)
void UIRadioJoinClass::Unknown_5EB2F0()
{
	SClient *sclient = (SClient *)((ClientClass *)Client)->Unknown_18();	// NAME 1: neutral (D10)
	ChatPanelInterface *chat = sclient->ChatPanel.Peek();	// NAME 1: neutral (D10)
	if (Is_Request_Pending(chat)) {
		((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_74D990(0x13, "Another request is pending. Cannot join a new channel. ", 0);
		return;
	}
	if (chat->Unknown_5682C0() != "") {
		int id = ((ClientClass *)Client)->Unknown1138;	// NAME 1: neutral (D10)
		ClientChatRequestPacketClass packet(7, id);
		packet.Unknown24 = chat->Unknown_5682C0();
		((ClientClass *)Client)->Send_Packet(&packet);
	}
	int id = ((ClientClass *)Client)->Unknown1138;	// NAME 1: neutral (D10)
	ClientChatRequestPacketClass packet(6, id);
	packet.Unknown24 = std::string(Entry->Get_Text());
	packet.Unknown34 = std::string("");
	((ClientClass *)Client)->Send_Packet(&packet);
	chat->Unknown_5680B0(std::string(Entry->Get_Text()));
	chat->Unknown225 = true;
}

/*
**	Notes.
**	Object 0x5EAB40-0x5EB7BF: the constructor 0x5EAB40 opens it, after ui_radio_main.cpp's adjustor thunks
**	(0x5EAB20 / 0x5EAB30); this window's adjustor thunks 0x5EB7A0 / 0x5EB7B0 close it and ui_refinery.cpp's
**	PermStringInitClass initialiser 0x5EB7C0 follows. ui_chat_panel.cpp's 0x566AF0 makes it (states 0, 1) as its
**	stand-in Unknown5EAB40Class. Entry->Get_Text() returns a copy the client never deletes (twice in 0x5EB2F0).
*/
