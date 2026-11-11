// Earth & Beyond client (reconstruction): app\client\ui_option_check.cpp, the options' check pages ('opt03a': the chat
// messages or the cameras). No string names the file (no asserts): the name is provisional (NAME 1, D137). Notes at the end.
#include <stdio.h>
#include <string.h>
#include "ui_option_check.h"
#include "ui_chat_panel.h"
#include "client.h"
#include "gadget.h"
#include "gd_robj.h"
#include "option.h"
#include "control_keys_manager.h"
#include "local_interface_sort.h"
#include "..\audio\soundlib.h"

// The buttons Unknown_54 makes (0xB82D68), each page's ini (0xB82D78) and texts (0xB82D80)
static const char *Unknown_B82D68[3] = { "exit", "ok", "cancel" };	// NAME 1: neutral (D10)
static const char *Unknown_B82D78[2] = { "fa_opt03a-chat.ini", "fa_opt03a-camera.ini" };	// NAME 1: neutral (D10)
static const char *Unknown_B82D80[2][3] = {	// NAME 1: neutral (D10)
	{ NULL, "ok_opt03a", "cancel_opt03a" },
	{ NULL, "ok_opt03a", "cancel_opt03a" },
};

// The option each page's checks show (0xB82D98; option.cpp's bools), NULL for an unused check
static const char *Unknown_B82D98[2][12] = {	// NAME 1: neutral (D10)
	{ "Chat_Tell", "Chat_NPC", "Chat_Tips", "Chat_Status_IC", "Chat_Warnings", "Chat_Errors_IC", "Chat_Attention" },
	{ "Camera_Ship", "Camera_Warp", "Camera_Loot", "Camera_Gate", "Camera_Land", "Camera_Dock", "Camera_Bridge" },
};

// The check box (unknown_7e7e00.h: 0x68 bytes, table 0xB075E4)
#include "unknown_7e7e00.h"








// kernel.h's input event, as ui_option_account.cpp's (Unknown_54's Escape)
struct UIOptionCheckKeyEventStruct	// NAME 1: neutral (D10)
{
	UIOptionCheckKeyEventStruct(short key) : Key(key), Unknown02(0), Unknown04(0), Unknown08(0), Unknown0C(0) {}	// NAME 1 [key]: neutral
	short Key;											// +0x00 (0x1B)  // NAME 1: neutral (D10)
	short Unknown02;									// +0x02
	float Unknown04;									// +0x04
	float Unknown08;									// +0x08
	int Unknown0C;										// +0x0C
};

// 0x5C9910
UIOptionCheckClass::UIOptionCheckClass(SClient *client, int page, bool mode) :
	ChatPanelUIBase(client),
	Page(page),
	Mode(mode)
{
	memset(Buttons, 0, sizeof(Buttons));
	memset(Checks, 0, sizeof(Checks));
}

// 0x5C99C0
UIOptionCheckClass::~UIOptionCheckClass()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
	}
}

// 0x5C9A50
void UIOptionCheckClass::Unknown_54()
{
	if (Unknown_98("opt03a", Unknown_B82D78[Page])) {
		int i;
		for (i = 0; i < 3; i++) {
			Buttons[i] = Add_Button(i + 1, Unknown_B82D68[i], Unknown_B82D80[Page][i]);
			GadgetClass::Set_Tooltip(Buttons[i], "UI_OPTION_CHECK", Unknown_B82D68[i]);
		}
		for (i = 0; i < 12; i++) {
			char name[32];
			_snprintf(name, sizeof(name), "check%02d", i);
			Checks[i] = Add_Check_Button(i + 5, name, NULL);
			if (Unknown_B82D98[Page][i]) {
				Checks[i]->Unknown_40(true);
				((RObjGadgetClass *)Checks[i])->Unknown_8C(false);
			} else {
				Checks[i]->Unknown_40(false);
				((RObjGadgetClass *)Checks[i])->Unknown_8C(true);
			}
		}
		Add_Back_Gadget(0, Vector3(0.0f, 0.0f, 0.0f));
		((ControlKeyListClass *)&Unknown4C)->Add(*(const ControlKeyComboStruct *)&UIOptionCheckKeyEventStruct(0x1B), (void *)1);
	}
}

// 0x5C9C00
void UIOptionCheckClass::Unknown_58()
{
	if (!Active) {
		InterfaceSortScopeClass sort(this);	// NAME 1: neutral (D10)
		Unknown_5C9EB0();
		ChatPanelUIBase::Unknown_58();
	}
}

// 0x5C9CE0
void UIOptionCheckClass::Unknown_5C()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
	}
}

// 0x5C9D00
bool UIOptionCheckClass::Unknown_68(int key)
{
	SClient *sclient = (SClient *)((ClientClass *)Client)->Unknown_18();	// NAME 1: neutral (D10)
	ChatPanelInterface *chat = sclient->ChatPanel.Peek();	// NAME 1: neutral (D10)
	switch (key) {
		case 1:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_5C();
			Closed = 1;
			return(true);
		case 2:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			if (!Mode) {
				chat->Unknown_565F30(1);
			}
			Unknown_5C9F20();
			Unknown_5C();
			Closed = 1;
			return(true);
		case 3:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			if (!Mode) {
				chat->Unknown_565F30(1);
			}
			break;
		case 5: case 6: case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16:
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			return(true);
		default:
			return(false);
	}
	Unknown_5C();
	Closed = 1;
	return(true);
}

// 0x5C9E40
void UIOptionCheckClass::Unknown_5C9E40(bool on)
{
	int i;
	for (i = 0; i < 3; i++) {
		if (Buttons[i]) {
			Buttons[i]->Unknown_40(on);
		}
	}
	for (i = 0; i < 12; i++) {
		if (Checks[i]) {
			Checks[i]->Unknown_40(on);
		}
	}
}

// 0x5C9EB0
void UIOptionCheckClass::Unknown_5C9EB0()
{
	OptionClass *options = OptionClass::Instance;	// NAME 1: neutral (D10)
	for (int i = 0; i < 12; i++) {
		if (Unknown_B82D98[Page][i]) {
			BoolOptionStruct *option = options->Find_Bool(Unknown_B82D98[Page][i]);	// NAME 1: neutral (D10)
			if (option) {
				Checks[i]->Unknown_7E8030(option->Get_Value());
			}
		}
	}
}

// 0x5C9F20
void UIOptionCheckClass::Unknown_5C9F20()
{
	OptionClass *options = OptionClass::Instance;	// NAME 1: neutral (D10)
	for (int i = 0; i < 12; i++) {
		if (Unknown_B82D98[Page][i]) {
			BoolOptionStruct *option = options->Find_Bool(Unknown_B82D98[Page][i]);	// NAME 1: neutral (D10)
			if (option) {
				option->Set_Value(Checks[i]->Checked);
			}
		}
	}
	options->Save();
	if (Page == 0) {
		SClient *sclient = (SClient *)((ClientClass *)Client)->Unknown_18();	// NAME 1: neutral (D10)
		ChatPanelInterface *chat = sclient->ChatPanel.Peek();	// NAME 1: neutral (D10)
		if (chat) {
			chat->Unknown_5689D0(0);
		}
	}
}

/*
**	Notes.
**	Object 0x5C9910-0x5CA00F: the constructor 0x5C9910 opens it, after ui_option_channel.cpp's adjustor thunks; this
**	page's adjustor thunks 0x5C9FF0 / 0x5CA000 close it and 0x5CA010 follows. The pattern is ui_option_channel.cpp's.
**	0x5C9E40 has no caller; 0x5C9EB0 has none either and is inlined into Unknown_58.
*/
