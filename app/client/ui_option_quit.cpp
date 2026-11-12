// Earth & Beyond client (reconstruction): app\client\ui_option_quit.cpp, the options' quit page ('opt02a').
// No string names the file (no asserts): the name is provisional (NAME 1, D137). Notes at the end.
#include <stdio.h>
#include <string.h>
#include "ui_option_quit.h"
#include "ui_chat_panel.h"
#include "client.h"
#include "gadget.h"
#include "option.h"
#include "fontmanager.h"
#include "control_keys_manager.h"
#include "local_interface_sort.h"
#include "..\audio\soundlib.h"

extern FontManagerClass *Unknown_BE67C4;	// NAME 1 [Unknown_BE67C4]: 0xBE67C4, as ui_login.cpp's

// The buttons Unknown_54 makes: their models' names and their texts (0xB835F0 / 0xB83608)
static const char *Unknown_B835F0[5] = { "exit", "bttn03", "bttn00", "bttn01", "bttn02" };	// NAME 1: neutral (D10)
static const char *Unknown_B83608[5] = { NULL, "cancel_opt02a", "Quit_opt02a", "Login_opt02a", NULL };	// NAME 1: neutral (D10)

// kernel.h's input event, as ui_starbase.cpp's UIStarbaseKeyEventStruct (Unknown_54's keys)
struct UIOptionQuitKeyEventStruct	// NAME 1: neutral (D10)
{
	UIOptionQuitKeyEventStruct(short key, short modifiers) : Key(key), Unknown02(modifiers), Unknown04(0), Unknown08(0), Unknown0C(0) {}	// NAME 1 [key, modifiers]: neutral
	short Key;											// +0x00 (0x1B, 'Q', 'C', 'L')  // NAME 1: neutral (D10)
	short Unknown02;									// +0x02 (0x400 with the letters)
	float Unknown04;									// +0x04
	float Unknown08;									// +0x08
	int Unknown0C;										// +0x0C
};

// 0x5CE700
UIOptionQuitClass::UIOptionQuitClass(SClient *client, bool mode) :
	ChatPanelUIBase(client),
	Mode(mode)
{
	memset(Buttons, 0, sizeof(Buttons));
}

// 0x5CE7A0
UIOptionQuitClass::~UIOptionQuitClass()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
	}
}

// 0x5CE830
void UIOptionQuitClass::Unknown_54()
{
	if (Unknown_98("opt02a", "fa_opt02a.ini")) {
		for (int i = 0; i < 5; i++) {
			Buttons[i] = Add_Button(i + 1, Unknown_B835F0[i], Unknown_B83608[i]);
			GadgetClass::Set_Tooltip(Buttons[i], "UI_OPTION_QUIT", Unknown_B835F0[i]);
		}
		Add_Back_Gadget(0, Vector3(0.0f, 0.0f, 0.0f));
		((ControlKeyListClass *)&Unknown4C)->Add(*(const ControlKeyComboStruct *)&UIOptionQuitKeyEventStruct(0x1B, 0), (void *)1);
		((ControlKeyListClass *)&Unknown4C)->Add(*(const ControlKeyComboStruct *)&UIOptionQuitKeyEventStruct('Q', 0x400), (void *)3);
		((ControlKeyListClass *)&Unknown4C)->Add(*(const ControlKeyComboStruct *)&UIOptionQuitKeyEventStruct('C', 0x400), (void *)4);
		((ControlKeyListClass *)&Unknown4C)->Add(*(const ControlKeyComboStruct *)&UIOptionQuitKeyEventStruct('L', 0x400), (void *)5);
	}
}

// 0x5CE9C0
void UIOptionQuitClass::Unknown_58()
{
	if (!Active) {
		InterfaceSortScopeClass sort(this);	// NAME 1: neutral (D10)
		ChatPanelUIBase::Unknown_58();
		Unknown_5CECB0();
	}
}

// 0x5CEAE0
void UIOptionQuitClass::Unknown_5C()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
	}
}

// 0x5CEB00
bool UIOptionQuitClass::Unknown_68(int key)
{
	SClient *sclient = (SClient *)((ClientClass *)Client)->Unknown_18();	// NAME 1: neutral (D10)
	ChatPanelInterface *chat = sclient->ChatPanel.Peek();	// NAME 1: neutral (D10)
	SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
	switch (key) {
		case 1:
			Unknown_5C();
			Closed = 1;
			return(true);
		case 2:
			Unknown_5C();
			Closed = 1;
			if (!Mode) {
				chat->Unknown_565F30(1);
			}
			return(true);
		case 3:
			OptionClass::Instance->Save();
			sclient->Unknown_74BB50(1);
			Unknown_5C();
			Unknown_5C();
			Closed = 1;
			return(true);
		case 4:
			OptionClass::Instance->Save();
			sclient->Unknown_74BB50(3);
			Unknown_5C();
			Unknown_5C();
			Closed = 1;
			return(true);
		case 5:
			OptionClass::Instance->Save();
			sclient->Unknown_74BB50(2);
			Unknown_5C();
			Unknown_5C();
			Closed = 1;
			return(true);
	}
	return(false);
}

// 0x5CEC30
void UIOptionQuitClass::Unknown_7C(UIViewClass *view, bool arg)
{
	if (View != view) {
		InterfaceClass::Unknown_7C(view, arg);
		Unknown_5CECB0();
	}
}

// 0x5CEC70
void UIOptionQuitClass::Unknown_5CEC70(bool on)
{
	for (int i = 0; i < 5; i++) {
		if (Buttons[i]) {
			Buttons[i]->Unknown_40(on);
		}
	}
}

// 0x5CECB0
void UIOptionQuitClass::Unknown_5CECB0()
{
	RefCountPtr<FontSys> note = Unknown_BE67C4->Unknown_6D2700("Note_opt02a", InterfaceMesh);	// NAME 1: neutral (D10)
	SClient *sclient = (SClient *)((ClientClass *)Client)->Unknown_18();	// NAME 1: neutral (D10)
	if (sclient->Unknown_744B60()) {
		note->Set_Hidden(true);
	} else {
		note->Set_Hidden(false);
	}
}

/*
**	Notes.
**	Object 0x5CE700-0x5CEDCF: the adjustor thunks 0x5CEDB0 / 0x5CEDC0 of the second and third tables close it; the
**	slider pages' constructor 0x5CEDD0 follows. The ui_option_account.cpp pattern (ChatPanelUIBase); Unknown_58 is
**	ui_option_check.cpp's (a sort scope around the base's). Keys 3-5 ('Q', 'C', 'L' with 0x400) leave the game:
**	SClient's 0x74BB50 takes 1 (desktop), 3 (character selection), 2 (login screen); the client calls slot +0x5C twice.
*/
