// Earth & Beyond client (reconstruction): app\client\ui_option_main.cpp, the options' main page ('opt01a').
// No string names the file (no asserts): the name is provisional (NAME 1, D137). Notes at the end.
#include <stdio.h>
#include <string.h>
#include "ui_option_main.h"
#include "ui_chat_panel.h"
#include "client.h"
#include "gadget.h"
#include "option.h"
#include "control_keys_manager.h"
#include "..\audio\soundlib.h"

// The buttons Unknown_54 makes: their models' names and their texts (0xB834B0 / 0xB834D4)
static const char *Unknown_B834B0[8] = { "exit", "bttn00", "bttn01", "bttn02", "bttn03", "bttn04", "bttn05", "bttn06" };	// NAME 1: neutral (D10)
static const char *Unknown_B834D4[8] = { NULL, "Camera_opt01a", "Control_opt01a", "Interface_opt01a", "Graphics_opt01a", "Sound_opt01a", "Credits_opt01a", "Fonts_opt01a" };	// NAME 1: neutral (D10)

// kernel.h's input event, as ui_option_account.cpp's (Unknown_54's Escape)










// 0x5CE240
UIOptionMainClass::UIOptionMainClass(SClient *client) :
	ChatPanelUIBase(client)
{
	memset(Buttons, 0, sizeof(Buttons));
}

// 0x5CE2C0
UIOptionMainClass::~UIOptionMainClass()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
	}
}

// 0x5CE350
void UIOptionMainClass::Unknown_54()
{
	if (Unknown_98("opt01a", "fa_opt01a.ini")) {
		for (int i = 0; i < 8; i++) {
			Buttons[i] = Add_Button(i + 1, Unknown_B834B0[i], Unknown_B834D4[i]);
			GadgetClass::Set_Tooltip(Buttons[i], "UI_OPTION_MAIN", Unknown_B834B0[i]);
		}
		Add_Back_Gadget(0, Vector3(0.0f, 0.0f, 0.0f));
		((ControlKeyListClass *)&Unknown4C)->Add(KernelEventClass(0x1B), (void *)1);
	}
}

// 0x5CE460
void UIOptionMainClass::Unknown_58()
{
	if (!Active) {
		ChatPanelUIBase::Unknown_58();
	}
}

// 0x5CE480
void UIOptionMainClass::Unknown_5C()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
	}
}

// 0x5CE4A0
bool UIOptionMainClass::Unknown_68(int key)
{
	SClient *sclient = (SClient *)((ClientClass *)Client)->Unknown_18();	// NAME 1: neutral (D10)
	ChatPanelInterface *chat = sclient->ChatPanel.Peek();	// NAME 1: neutral (D10)
	SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
	switch (key) {
		case 1:
			OptionClass::Instance->Save();
			Unknown_5C();
			Closed = 1;
			return(true);
		case 2:
			Unknown_5C();
			Closed = 1;
			chat->Unknown_565F30(4);
			return(true);
		case 3:
			Unknown_5C();
			Closed = 1;
			chat->Unknown_565F30(8);
			return(true);
		case 4:
			Unknown_5C();
			Closed = 1;
			chat->Unknown_565F30(7);
			return(true);
		case 5:
			Unknown_5C();
			Closed = 1;
			chat->Unknown_565F30(5);
			return(true);
		case 6:
			Unknown_5C();
			Closed = 1;
			chat->Unknown_565F30(6);
			return(true);
		case 7:
			Unknown_5C();
			Closed = 1;
			chat->Unknown_565F30(11);
			return(true);
		case 8:
			Unknown_5C();
			Closed = 1;
			chat->Unknown_565F30(12);
			return(true);
	}
	return(false);
}

// 0x5CE670
void UIOptionMainClass::Unknown_5CE670(bool on)
{
	for (int i = 0; i < 8; i++) {
		if (Buttons[i]) {
			Buttons[i]->Unknown_40(on);
		}
	}
}

/*
**	Notes.
**	Object 0x5CE240-0x5CE6FF: the adjustor thunks 0x5CE6E0 / 0x5CE6F0 of the second and third tables close it; the quit
**	page's constructor 0x5CE700 follows. The ui_option_account.cpp pattern (ChatPanelUIBase). The pages the buttons open
**	are the chat panel's 0x565F30 requests: 4 the camera checks, 8 the controls (0x5CA010), 7 / 5 / 6 the slider pages
**	(interface, graphics, sound), 11 the credits, 12 the fonts.
*/
