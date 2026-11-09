// Earth & Beyond client (reconstruction): app\client\ui_option_account.cpp, the options' account page ('opt05a').
// No string names the file (no asserts): the name is provisional (NAME 1, D137). Notes at the end.
#include <stdio.h>
#include <string.h>
#include "ui_option_account.h"
#include "ui_chat_panel.h"
#include "client.h"
#include "gadget.h"
#include "fontmanager.h"
#include "control_keys_manager.h"
#include "..\audio\soundlib.h"

extern FontManagerClass *Unknown_BE67C4;	// NAME 1 [Unknown_BE67C4]: 0xBE67C4, as ui_login.cpp's

// The buttons Unknown_54 makes: their models' names and their texts (0xB82B7C / 0xB82B88)
static const char *Unknown_B82B7C[2] = { "exit", "ok" };	// NAME 1: neutral (D10)
static const char *Unknown_B82B88[2] = { NULL, "ok_opt05a" };	// NAME 1: neutral (D10)

// kernel.h's input event, as ui_starbase.cpp's UIStarbaseLightsKeyEventStruct (Unknown_54's Escape)
struct UIOptionAccountKeyEventStruct	// NAME 1: neutral (D10)
{
	UIOptionAccountKeyEventStruct(short key) : Key(key), Unknown02(0), Unknown04(0), Unknown08(0), Unknown0C(0) {}	// NAME 1 [key]: neutral
	short Key;											// +0x00 (0x1B)  // NAME 1: neutral (D10)
	short Unknown02;									// +0x02
	float Unknown04;									// +0x04
	float Unknown08;									// +0x08
	int Unknown0C;										// +0x0C
};

// 0x5C8870
UIOptionAccountClass::UIOptionAccountClass(SClient *client, int mode) :
	ChatPanelUIBase(client),
	Mode(mode)
{
	memset(Buttons, 0, sizeof(Buttons));
}

// 0x5C88F0
UIOptionAccountClass::~UIOptionAccountClass()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
	}
}

// 0x5C8980
void UIOptionAccountClass::Unknown_54()
{
	if (Unknown_98("opt05a", "fa_opt05a.ini")) {
		for (int i = 0; i < 2; i++) {
			Buttons[i] = Unknown_56D510(i + 1, Unknown_B82B7C[i], Unknown_B82B88[i]);
			GadgetClass::Set_Tooltip(Buttons[i], "UI_OPTION_ACCOUNT", Unknown_B82B7C[i]);
		}
		Unknown_56D970(0, Vector3(0.0f, 0.0f, 0.0f));
		Unknown_5C8B90();
		((ControlKeyListClass *)&Unknown4C)->Add(*(const ControlKeyComboStruct *)&UIOptionAccountKeyEventStruct(0x1B), (void *)1);
	}
}

// 0x5C8A90
void UIOptionAccountClass::Unknown_58()
{
	if (!Active) {
		ChatPanelUIBase::Unknown_58();
	}
}

// 0x5C8AB0
void UIOptionAccountClass::Unknown_5C()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
	}
}

// 0x5C8AD0
bool UIOptionAccountClass::Unknown_68(int key)
{
	SClient *sclient = (SClient *)((ClientClass *)Client)->Unknown_18();	// NAME 1: neutral (D10)
	ChatPanelInterface *chat = sclient->ChatPanel.Peek();	// NAME 1: neutral (D10)
	SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
	switch (key) {
		case 1:
			Unknown_5C();
			Unknown98 = 1;
			return(true);
		case 2:
			Unknown_5C();
			Unknown98 = 1;
			if (Mode == 1) {
				chat->Unknown_565F30(5);
			} else {
				chat->Unknown_565F30(1);
			}
			return(true);
	}
	return(false);
}

// 0x5C8B90
void UIOptionAccountClass::Unknown_5C8B90()
{
	if (Mode == 1) {
		RefCountPtr<FontSys> title = Unknown_BE67C4->Unknown_6D2700("title_opt05a", Unknown84);	// NAME 1: neutral (D10)
		title->Change_Text("Screen Resolution Info", 0);
		ManagedFontClass *font = Unknown_BE67C4->Find(Unknown84.Peek());	// NAME 1: neutral (D10)
		font->Unknown_6D0A70("WRAP_message_opt05a", "Please exit the game and run EBConfig.exe to reconfigure the resolution settings for the game.");
	}
}

/*
**	Notes.
**	Object 0x5C8870-0x5C8CBF: the adjustor thunks 0x5C8CA0 / 0x5C8CB0 of the second and third tables close it; the 'opt06a'
**	page's constructor 0x5C8DB0 follows. Unknown_54, Unknown_58, Unknown_5C and Unknown_68 follow ui_starbase.cpp's lights
**	panel (UIStarbaseLightsClass), the same ChatPanelUIBase pattern.
*/
