// Earth & Beyond client (reconstruction): app\client\ui_radio_main.cpp, the chat channel window ('cht01').
// No string names the file (no asserts): the name is provisional (NAME 1, D137). Notes at the end.
#include <stdio.h>
#include <string.h>
#include <string>
#include "ui_radio_main.h"
#include "ui_chat_panel.h"
#include "client.h"
#include "gadget.h"
#include "gd_robj.h"
#include "local_interface_sort.h"
#include "..\packets\pkclientchat.h"
#include "..\audio\soundlib.h"

extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)

// The chat panel's request flag (+0x225), read through an inline as the client does (al loaded, then tested)
static inline bool Is_Request_Pending(ChatPanelInterface *chat) { return(chat->Unknown225); }	// NAME 1: neutral (D10)

// The buttons Unknown_54 makes: their models' names (0xB85774)
static const char *Unknown_B85774[4] = { "EXIT", "BUTTON01", "BUTTON02", "BUTTON03" };	// NAME 1: neutral (D10)

// 0x5E9410
UIRadioMainClass::UIRadioMainClass(SClient *client, int state, std::string channel) :
	ChatPanelUIBase(client),
	State(state),
	Channel(channel)
{
}

// 0x5E9600
UIRadioMainClass::~UIRadioMainClass()
{
	if (Active) {
		Unknown_5EA260(false);
		InterfaceClass::Unknown_5C();
	}
}

// 0x5E96F0
void UIRadioMainClass::Unknown_54()
{
	if (Unknown_98("cht01", "cht01.ini")) {
		for (int i = 0; i < 4; i++) {
			Buttons[i] = Unknown_56D510(i + 1, Unknown_B85774[i], NULL);
			GadgetClass::Set_Tooltip(Buttons[i], "UI_RADIO_MAIN", Unknown_B85774[i]);
		}
		Unknown_56D970(0, Vector3(0.0f, 0.0f, 0.0f));
		Unknown_5E9BB0();
	}
}

// 0x5E97C0
void UIRadioMainClass::Unknown_58()
{
	if (!Active) {
		InterfaceSortScopeClass sort(this);	// NAME 1: neutral (D10)
		ChatPanelUIBase::Unknown_58();
		Unknown_5EA260(true);
		if (Buttons[0]) {
			((RObjGadgetClass *)Buttons[0])->Unknown_66C360();
		}
	}
}

// 0x5E9860
void UIRadioMainClass::Unknown_5C()
{
	if (Active) {
		Unknown_5EA260(false);
		InterfaceClass::Unknown_5C();
	}
}

// 0x5E9890
bool UIRadioMainClass::Unknown_68(int key)
{
	switch (key) {
		case 1:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_5C();
			Unknown98 = 1;
			return(true);
		case 2:
		case 3:
		case 4:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_5E9910(key);
			return(true);
	}
	return(false);
}

// 0x5E9910
void UIRadioMainClass::Unknown_5E9910(int key)
{
	SClient *sclient = (SClient *)((ClientClass *)Client)->Unknown_18();	// NAME 1: neutral (D10)
	ChatPanelInterface *chat = sclient->ChatPanel.Peek();	// NAME 1: neutral (D10)
	switch (State) {
		case 0:
			Unknown_5C();
			Unknown98 = 1;
			if (key == 2) {
				Unknown_5EA7F0();
			} else if (key == 3) {
				if (chat) {
					chat->Unknown_565F60(10);
				}
			} else if (key == 4) {
				if (chat) {
					chat->Unknown_565F60(2);
				}
			}
			break;
		case 1:
			Unknown_5C();
			Unknown98 = 1;
			if (key == 2) {
				if (chat) {
					chat->Unknown_565F60(3);
				}
			} else if (key == 4) {
				Unknown_5EA7F0();
				Unknown_5EA370();
			}
			break;
		case 2:
			if (key == 2) {
				Unknown_5C();
				Unknown98 = 1;
				if (chat) {
					chat->Unknown_565F60(2);
				}
			}
			break;
		case 3:
			if (key == 2) {
				Unknown_5C();
				Unknown98 = 1;
				Unknown_5EA550();
			} else if (key == 4) {
				Unknown_5C();
				Unknown98 = 1;
				if (chat) {
					chat->Unknown_565F60(9);
				}
			}
			break;
	}
}

// 0x5E9AD0
void UIRadioMainClass::Unknown_5E9AD0(RefCountPtr<FontSys> font, const char *text, int id)
{
	if (font) {
		font->Change_Text(text, 0);
	}
	if (id) {
		if (text && *text && *text != ' ') {
			GadgetClass::Set_Tooltip(Buttons[id - 1], "UI_RADIO_MAIN", text);
		} else {
			GadgetClass::Set_Tooltip(Buttons[id - 1], "UI_RADIO_MAIN", "EMPTY");
		}
	}
}

// 0x5E9BB0
void UIRadioMainClass::Unknown_5E9BB0()
{
	RefCountPtr<FontSys> title = Create_Peek(Unknown_BE67C4->Get_Style("title_Cht01", Interface_Mesh()));	// NAME 1: neutral (D10)
	RefCountPtr<FontSys> button1 = Create_Peek(Unknown_BE67C4->Get_Style("BUTTON01_CHT01", Interface_Mesh()));	// NAME 1: neutral (D10)
	RefCountPtr<FontSys> button2 = Create_Peek(Unknown_BE67C4->Get_Style("BUTTON02_CHT01", Interface_Mesh()));	// NAME 1: neutral (D10)
	RefCountPtr<FontSys> button3 = Create_Peek(Unknown_BE67C4->Get_Style("BUTTON03_CHT01", Interface_Mesh()));	// NAME 1: neutral (D10)
	ManagedFontClass *font = Unknown_BE67C4->Find(Interface_Mesh());	// NAME 1: neutral (D10)
	char buffer[256];	// NAME 1: neutral (D10)
	switch (State) {
		case 0:
			if (Channel != "") {
				_snprintf(buffer, sizeof(buffer), "Channel %s", Channel.c_str());
				Unknown_5E9AD0(title, buffer, 0);
				_snprintf(buffer, sizeof(buffer), "You are currently monitoring Channel %s.", Channel.c_str());
				font->Unknown_6D0A70("WRAP_MESSAGE_Cht01", buffer);
				Unknown_5E9AD0(button1, "Leave", 2);
				Unknown_5E9AD0(button2, "Members", 3);
				Unknown_5E9AD0(button3, "Change", 4);
				break;
			}
			Unknown_5E9AD0(title, "No Channel", 0);
			font->Unknown_6D0A70("WRAP_MESSAGE_Cht01", "You are not monitoring a Channel. Click 'join' to join or create a Channel.");
			Unknown_5E9AD0(button1, "Cancel", 2);
			Unknown_5E9AD0(button2, " ", 3);
			Unknown_5E9AD0(button3, "Join", 4);
			break;
		case 1:
			Unknown_5E9AD0(title, "Private Channels", 0);
			font->Unknown_6D0A70("WRAP_MESSAGE_Cht01", "Which Channel do you wish to join?");
			Unknown_5E9AD0(button1, "Existing", 2);
			Unknown_5E9AD0(button2, " ", 3);
			Unknown_5E9AD0(button3, "New", 4);
			break;
		case 2:
			Unknown_5E9AD0(title, "No Channels Available", 0);
			font->Unknown_6D0A70("WRAP_MESSAGE_Cht01", "Sorry, there are no private channels available. Please try again later.");
			Unknown_5E9AD0(button1, "OK", 2);
			Unknown_5E9AD0(button2, " ", 3);
			Unknown_5E9AD0(button3, " ", 4);
			break;
		case 3:
			_snprintf(buffer, sizeof(buffer), "Channel %s Created!", Channel.c_str());
			Unknown_5E9AD0(title, buffer, 0);
			_snprintf(buffer, sizeof(buffer), "You are the first person in channel %s. Do you wish to create a password?", Channel.c_str());
			font->Unknown_6D0A70("WRAP_MESSAGE_Cht01", buffer);
			Unknown_5E9AD0(button1, "No", 2);
			Unknown_5E9AD0(button2, " ", 3);
			Unknown_5E9AD0(button3, "Yes", 4);
			break;
	}
}

// 0x5EA260
void UIRadioMainClass::Unknown_5EA260(bool on)
{
	for (int i = 0; i < 4; i++) {
		if (Buttons[i]) {
			Buttons[i]->Unknown_40(on);
		}
	}
	if (State == 1 || State == 3 || Channel == "") {
		((RObjGadgetClass *)Buttons[2])->Unknown_8C(true);
		Buttons[2]->Unknown_40(false);
	}
	if (State == 2) {
		((RObjGadgetClass *)Buttons[3])->Unknown_8C(true);
		Buttons[3]->Unknown_40(false);
	}
}

// 0x5EA370
void UIRadioMainClass::Unknown_5EA370()
{
	SClient *sclient = (SClient *)((ClientClass *)Client)->Unknown_18();	// NAME 1: neutral (D10)
	ChatPanelInterface *chat = sclient->ChatPanel.Peek();	// NAME 1: neutral (D10)
	if (Is_Request_Pending(chat)) {
		((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_74D990(0x13, "Another request is pending. Cannot create a new channel.", 0);
		return;
	}
	int id = ((ClientClass *)Client)->Unknown1138;	// NAME 1: neutral (D10)
	ClientChatRequestPacketClass packet(4, id);
	packet.Unknown24 == "";	// a dead comparison (D130; not the client's): it spends VC6's inline budget, so the assignment's assign and the temporary's _Eos stay out of line as the client's
	packet.Unknown24 = std::string("");
	((ClientClass *)Client)->Send_Packet(&packet);
	chat->Unknown225 = true;
}

// 0x5EA550
void UIRadioMainClass::Unknown_5EA550()
{
	SClient *sclient = (SClient *)((ClientClass *)Client)->Unknown_18();	// NAME 1: neutral (D10)
	ChatPanelInterface *chat = sclient->ChatPanel.Peek();	// NAME 1: neutral (D10)
	int id = ((ClientClass *)Client)->Unknown1138;	// NAME 1: neutral (D10)
	ClientChatRequestPacketClass packet(0x15, id);
	packet.Unknown34 = std::string("none");
	packet.Unknown24 = chat->Unknown_5682C0();
	((ClientClass *)Client)->Send_Packet(&packet);
}

// 0x5EA7F0
void UIRadioMainClass::Unknown_5EA7F0()
{
	SClient *sclient = (SClient *)((ClientClass *)Client)->Unknown_18();	// NAME 1: neutral (D10)
	ChatPanelInterface *chat = sclient->ChatPanel.Peek();	// NAME 1: neutral (D10)
	if (Is_Request_Pending(chat)) {
		((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_74D990(0x13, "Another request is pending. Cannot leave the channel.", 0);
		return;
	}
	if (chat->Unknown_5682C0() != "") {
		int id = ((ClientClass *)Client)->Unknown1138;	// NAME 1: neutral (D10)
		ClientChatRequestPacketClass packet(7, id);
		packet.Unknown24 = chat->Unknown_5682C0();
		((ClientClass *)Client)->Send_Packet(&packet);
	}
}

/*
**	Notes.
**	Object 0x5E9410-0x5EAB3F: the constructor 0x5E9410 opens it, after the 'cht02' window's adjustor thunks
**	0x5E93F0 / 0x5E9400 (that window's file is unknown: ui_radio_entry.cpp's one assert is in the 'cht04' window);
**	this window's adjustor thunks 0x5EAB20 / 0x5EAB30 close it and the 'cht03' window's constructor 0x5EAB40
**	(ui_radio_join.cpp) follows. The pattern is ui_option_channel.cpp's (ChatPanelUIBase). ui_chat_panel.cpp's
**	0x566AF0 makes it (states 0-3) as its stand-in Unknown5E9410Class.
*/
