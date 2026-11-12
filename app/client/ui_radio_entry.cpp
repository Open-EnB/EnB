// Earth & Beyond client (reconstruction): app\Client\ui_radio_entry.cpp, the private channel's password window
// ('cht04'). The name is the client's (its FAIL_IF's __FILE__). Notes at the end.
#include <stdio.h>
#include <string.h>
#include <string>
#include "ui_radio_entry.h"
#include "ui_chat_panel.h"
#include "client.h"
#include "gadget.h"
#include "gd_robj.h"
#include "textgadg.h"
#include "local_interface_sort.h"
#include "..\packets\pkclientchat.h"
#include "..\audio\soundlib.h"
#include "..\shared\g_assert.h"

extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)

// The buttons Unknown_54 makes: their models' names (0xB853D0)
static const char *Unknown_B853D0[5] = { "EXIT", "TEXT_FIELD00", "TEXT_FIELD01", "BUTTON02", "BUTTON03" };	// NAME 1: neutral (D10)

// 0x5E6290
UIRadioEntryClass::UIRadioEntryClass(SClient *client, int state) :
	ChatPanelUIBase(client),
	State(state),
	PasswordGadget(NULL),
	ConfirmGadget(NULL),
	UnknownAC(false)
{
}

// 0x5E6330
UIRadioEntryClass::~UIRadioEntryClass()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
	}
}

// 0x5E63C0
void UIRadioEntryClass::Unknown_54()
{
	if (Unknown_98("cht04", "cht04.ini")) {
		for (int i = 0; i < 5; i++) {
			Buttons[i] = Add_Button(i + 1, Unknown_B853D0[i], NULL);
		}
		PasswordGadget = new TextGadgetClass(this, 8, "TEXTFIELD00_Cht04", Interface_Mesh(), Unknown_B08CC4, 0);
		PasswordGadget->Set_Text("", 0);
		TextGadgetClass *gadget = PasswordGadget;	// NAME 1: neutral (D10); a local as the client's registers (0x5E63C0 exact)
		gadget->Unknown5C = true;
		Unknown64.Unknown_6870E0(PasswordGadget, 7, View, 1, 0);
		if (State == 2) {
			ConfirmGadget = new TextGadgetClass(this, 8, "TEXTFIELD01_Cht04", Interface_Mesh(), Unknown_B08CC4, 0);
			ConfirmGadget->Set_Text("", 0);
			ConfirmGadget->Unknown5C = true;
			Unknown64.Unknown_6870E0(ConfirmGadget, 8, View, 1, 0);
		} else {
			((RObjGadgetClass *)Buttons[2])->Unknown_8C(true);
			Buttons[2]->Unknown_40(false);
		}
		Add_Back_Gadget(0, Vector3(0.0f, 0.0f, 0.0f));
		Unknown_5E6BA0();
	}
}

// 0x5E6610
void UIRadioEntryClass::Unknown_58()
{
	if (!Active) {
		InterfaceSortScopeClass sort(this);	// NAME 1: neutral (D10)
		ChatPanelUIBase::Unknown_58();
		if (Buttons[0]) {
			((RObjGadgetClass *)Buttons[0])->Unknown_66C360();
		}
		PasswordGadget->Unknown_74(true);
		PasswordGadget->Set_Text("", 0);
		if (State != 2) {
			UnknownAC = true;
		}
	}
}

// 0x5E66E0
void UIRadioEntryClass::Unknown_5C()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
	}
}

// 0x5E6700
void UIRadioEntryClass::Unknown_60()
{
	if (PasswordGadget) {
		PasswordGadget->Unknown_6AAD10();
		if (State == 2 && PasswordGadget->Unknown39) {
			PasswordGadget->Unknown39 = false;
			PasswordGadget->Unknown_74(false);
			ConfirmGadget->Unknown_74(true);
			ConfirmGadget->Set_Text(NULL, 0);
			UnknownAC = true;
		}
	}
	if (ConfirmGadget) {
		ConfirmGadget->Unknown_6AAD10();
		if (ConfirmGadget->Unknown39) {
			ConfirmGadget->Unknown39 = false;
			ConfirmGadget->Unknown_74(false);
		}
	}
	if (UnknownAC) {
		if (State == 2 && ConfirmGadget && !ConfirmGadget->Active) {
			PasswordGadget->Unknown_74(false);
			ConfirmGadget->Unknown_74(false);
			Unknown_5E6A20(5);
			UnknownAC = false;
		} else if (State != 2 && PasswordGadget && !PasswordGadget->Active) {
			PasswordGadget->Unknown_74(false);
			Unknown_5E6A20(5);
			UnknownAC = false;
		}
	}
}

// 0x5E6840
bool UIRadioEntryClass::Unknown_68(int key)
{
	switch (key) {
		case 1:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			if (PasswordGadget) {
				PasswordGadget->Unknown_74(false);
			}
			if (ConfirmGadget) {
				ConfirmGadget->Unknown_74(false);
			}
			Unknown_5C();
			Closed = 1;
			return(true);
		case 2:
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			if (PasswordGadget) {
				PasswordGadget->Unknown_74(true);
			}
			if (ConfirmGadget) {
				ConfirmGadget->Unknown_74(false);
			}
			PasswordGadget->Set_Text("", 0);
			if (State != 2) {
				UnknownAC = true;
			} else {
				UnknownAC = false;
			}
			return(true);
		case 3:
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			if (PasswordGadget) {
				PasswordGadget->Unknown_74(false);
			}
			if (ConfirmGadget) {
				ConfirmGadget->Unknown_74(true);
			}
			ConfirmGadget->Set_Text("", 0);
			UnknownAC = true;
			return(true);
		case 4:
		case 5:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			if (PasswordGadget) {
				PasswordGadget->Unknown_74(false);
			}
			if (ConfirmGadget) {
				ConfirmGadget->Unknown_74(false);
			}
			Unknown_5E6A20(key);
			return(true);
	}
	return(false);
}

// 0x5E6A20
void UIRadioEntryClass::Unknown_5E6A20(int key)
{
	SClient *sclient = (SClient *)((ClientClass *)Client)->As_Space_Client();	// NAME 1: neutral (D10)
	ChatPanelInterface *chat = sclient->ChatPanel.Peek();	// NAME 1: neutral (D10)
	switch (State) {
		case 0:
		case 1:
			if (key == 4) {
				Unknown_5C();
				Closed = 1;
				if (chat) {
					chat->Unknown_565F60(3);
				}
			} else if (key == 5) {
				Unknown_5C();
				Closed = 1;
				Unknown_5E6EC0();
			}
			break;
		case 2:
			if (key == 4) {
				Unknown_5C();
				Closed = 1;
				if (chat) {
					chat->Unknown_565F60(8);
				}
			} else if (key == 5) {
				Unknown_5E70A0();
			}
			break;
	}
}

// 0x5E6B10
void UIRadioEntryClass::Unknown_5E6B10(RefCountPtr<FontSys> font, const char *text)
{
	if (font) {
		font->Change_Text(text, 0);
	}
}

// 0x5E6BA0
void UIRadioEntryClass::Unknown_5E6BA0()
{
	RefCountPtr<FontSys> title = Create_Peek(Unknown_BE67C4->Get_Style("TITLE01_Cht04", Interface_Mesh()));	// NAME 1: neutral (D10)
	RefCountPtr<FontSys> password = Create_Peek(Unknown_BE67C4->Get_Style("PASSWORD_Cht04", Interface_Mesh()));	// NAME 1: neutral (D10)
	RefCountPtr<FontSys> verify = Create_Peek(Unknown_BE67C4->Get_Style("VARIFY_Cht04", Interface_Mesh()));	// NAME 1: neutral (D10)
	ManagedFontClass *font = Unknown_BE67C4->Find(Interface_Mesh());	// NAME 1: neutral (D10)
	switch (State) {
		case 0:
			Unknown_5E6B10(title, "Enter Password");
			font->Unknown_6D0A70("WRAP_MAIN_Cht04", "This Channel Requires a password.");
			Unknown_5E6B10(password, "Password");
			Unknown_5E6B10(verify, " ");
			break;
		case 1:
			Unknown_5E6B10(title, "Password Incorrect");
			font->Unknown_6D0A70("WRAP_MAIN_Cht04", "The password you entered is not correct. Please try reentering the password again.");
			Unknown_5E6B10(password, "Password");
			Unknown_5E6B10(verify, " ");
			break;
		case 2:
			Unknown_5E6B10(title, "Create Password");
			font->Unknown_6D0A70("WRAP_MAIN_Cht04", " ");
			Unknown_5E6B10(password, "Enter Password");
			Unknown_5E6B10(verify, "Verify Password");
			break;
	}
}

// 0x5E6EC0
void UIRadioEntryClass::Unknown_5E6EC0()
{
	SClient *sclient = (SClient *)((ClientClass *)Client)->As_Space_Client();	// NAME 1: neutral (D10)
	ChatPanelInterface *chat = sclient->ChatPanel.Peek();	// NAME 1: neutral (D10)
	int id = ((ClientClass *)Client)->Unknown1138;	// NAME 1: neutral (D10)
	ClientChatRequestPacketClass packet(6, id);
	packet.Unknown24 = chat->Unknown_5682C0();
	packet.Unknown34 = std::string(PasswordGadget->Get_Text());
	((ClientClass *)Client)->Send_Packet(&packet);
}

































// 0x5E70A0
void UIRadioEntryClass::Unknown_5E70A0()
{


	FAIL_IF(!PasswordGadget || !ConfirmGadget) return;
	SClient *sclient = (SClient *)((ClientClass *)Client)->As_Space_Client();	// NAME 1: neutral (D10)
	ChatPanelInterface *chat = sclient->ChatPanel.Peek();	// NAME 1: neutral (D10)
	if (strcmp(PasswordGadget->Get_Text(), ConfirmGadget->Get_Text()) == 0 && PasswordGadget->Get_Text_Length() != 0) {
		int id = ((ClientClass *)Client)->Unknown1138;	// NAME 1: neutral (D10)
		ClientChatRequestPacketClass packet(0x16, id);
		packet.Unknown34 = std::string(PasswordGadget->Get_Text());
		packet.Unknown24 = chat->Unknown_5682C0();
		((ClientClass *)Client)->Send_Packet(&packet);
		Unknown_5C();
		Closed = 1;
		return;
	}
	RefCountPtr<FontSys> title = Create_Peek(Unknown_BE67C4->Get_Style("TITLE01_Cht04", Interface_Mesh()));	// NAME 1: neutral (D10)
	ManagedFontClass *font = Unknown_BE67C4->Find(Interface_Mesh());	// NAME 1: neutral (D10)
	if (PasswordGadget->Get_Text_Length() != 0) {
		Unknown_5E6B10(title, "Passwords do not match");
		font->Unknown_6D0A70("WRAP_MAIN_Cht04", "The passwords did not match. Please try re-entering them again.");
	} else {
		Unknown_5E6B10(title, "Invalid Passwords entered");
		font->Unknown_6D0A70("WRAP_MAIN_Cht04", "You have entered invalid passwords. Please try re-entering them again.");
	}
	PasswordGadget->Set_Text("", 0);
	ConfirmGadget->Set_Text("", 0);
	SoundLibraryClass::Play_2D_Sound("cost.wav", 2);
}

/*
**	Notes.
**	Object 0x5E6290-0x5E74FF: the constructor 0x5E6290 opens it, after the 'cht05' window's adjustor thunks
**	0x5E6270 / 0x5E6280; this window's adjustor thunks 0x5E74E0 / 0x5E74F0 close it and the 'cht02' window's
**	constructor 0x5E7500 follows. 0x5E7480 is RefCountPtr<FontSys>'s copy constructor, emitted in this object.
**	ui_chat_panel.cpp's 0x566AF0 makes it (states 0-2) as its stand-in Unknown5E6290Class. Get_Text returns a copy
**	the client never deletes (three in 0x5E70A0, one in 0x5E6EC0). Requests: 6 joins the channel with the password
**	(Unknown34), 22 (0x16) sets the new channel's password.
*/
