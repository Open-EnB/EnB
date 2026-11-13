// Earth & Beyond client (reconstruction): app\client\ui_symbolchat.cpp (the client's __FILE__
// 'e:\build\beta\G\app\Client\ui_symbolchat.cpp', 0x64A330-0x64B6D7). Notes at the end.
#define __INTRFACE_MEMBERS	// intrface.h's members (the constructor 0x540D30 with the client)
#include <stdio.h>
#include <string.h>
#include "ui_symbolchat.h"
#include "gadget.h"
#include "gd_dynamicwindow.h"
#include "dynamic_window_class.h"
#include "unknown_51c800.h"
#include "fontmanager.h"
#include "local_interface_sort.h"
#include "..\audio\soundlib.h"
#include "vector2.h"
#include "sclient.h"
#include "ini.h"
#include "wwstring.h"
#include "nstrdup.h"
#include "..\shared\message.h"

void Unknown_42ECF0(float *x, float *y);			// 0x42ECF0 (the pointer, 0..1)  // NAME 1 [x, y]: as ui_avatar_cust.cpp's
bool Unknown_42EC60(int a, int b);					// 0x42EC60 (cdecl): *0xBDAA70's slot +0x0C  // NAME 1 [a, b]: as ui_chat_panel.cpp's

// 0xB8BAE0: the symbols' sections of symbolchatdef.ini ('%s_INFO_%d'), NULL-ended
static const char *UISymbolChatSections[4] = { "GENERIC", "TEAM", "CONTEXT", NULL };	// NAME 1: neutral (D10)
// (blank lines: the Message_Error below sits on the client's __LINE__ 152, D25)










































// 0x64A330
UISymbolChatOptionStruct::UISymbolChatOptionStruct() :
	Window(NULL),
	Text(NULL),
	Gadget(NULL),
	Unknown0C(NULL),
	Unknown10(false)
{
}

// 0x64A360
UISymbolChatOptionStruct::~UISymbolChatOptionStruct()
{
	if (Window) {
		Window->Remove_All(false);
		Window = NULL;
	}
	if (Text) {
		Text = NULL;
	}
	if (Gadget) {
		GadgetClass::Destroy(Gadget);
		Gadget = NULL;
	}
	if (Unknown0C) {
		delete [] Unknown0C;
		Unknown0C = NULL;
	}
}

// 0x64A480
UISymbolChatClass::UISymbolChatClass(SClient *sclient) :
	InterfaceClass((LineDisplayClientClass *)sclient),
	Unknown84(0),
	Window(NULL),
	Gadget(NULL),
	Options(0)
{
	Unknown_BE67BC->Load("fc_symbolchat.ini");
	Window = Create_NEW(new DynamicWindowClass("SYMBOL_CHAT_WINDOW", 3));
	Window->Set_Level(8);
	for (int i = 0; i < 3; i++) {
		Sections[i] = Create_NEW(new DynamicWindowClass("SYMBOL_CHAT_SECTION", 1));
		Window->Add_Object(Sections[i].Peek(), i);
	}
	Gadget = new DynamicWindowGadgetClass(Window.Peek(), 0x55, 3, false);
	Unknown64.Unknown_6870E0(Gadget, 0, View, true, true);
	Gadget->Unknown_65FFA0(-1);
}

// 0x64A7D0
UISymbolChatClass::~UISymbolChatClass()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
		Unknown84 = 0;
		Window->Remove_From_Scene();
	}
	Unknown_64AF80();
	if (Gadget) {
		Unknown64.Unknown_687150(Gadget);
		GadgetClass::Destroy(Gadget);
		Gadget = NULL;
	}
	for (int i = 0; i < 3; i++) {
		if (Sections[i]) {
			Sections[i]->Remove_All(false);
			Sections[i] = NULL;
		}
	}
	if (Window) {
		Window->Remove_All(false);
		Window = NULL;
	}
	Unknown_BE67BC->Unload("fc_symbolchat.ini");
}

// NOT EXACT: written from the client's code (register only: this in ebp, the client's esi; see attempts.csv)
// 0x64A9B0
void UISymbolChatClass::Unknown_64A9B0()
{
	INIClass ini("symbolchatdef.ini");
	if (!ini.Is_Loaded()) {
		Message_Error("Symbol Chat", "", __FILE__, __LINE__, "symbolchatdef.ini file not found");
	} else {
		int count = 0;
		for (int category = 0; UISymbolChatSections[category] != NULL; category++) {
			int i = 0;
			while (true) {
				char section[32];
				sprintf(section, "%s_INFO_%d", UISymbolChatSections[category], i);
				if (!ini.Find_Section(section)) {
					break;
				}
				StringClass symbol_string;
				ini.Get_String(symbol_string, section, "SymbolString", "");
				StringClass symbol_asset;
				ini.Get_String(symbol_asset, section, "SymbolAsset", "");
				RefCountPtr<FontSys> text = Create_NEW(new FontSys("Symbol_Chat_Default", NULL, NULL));
				text->Change_Text(symbol_string, 0);
				UISymbolChatOptionStruct *option = new UISymbolChatOptionStruct;
				option->Window = Create_NEW(new DynamicWindowClass("SYMBOL_CHAT_OPTION", 1));
				i++;
				Sections[category]->Add_Object(option->Window.Peek(), i);
				option->Text = Create_NEW(new Unknown51C800Class(text.Peek(), 6));
				option->Window->Add_Object(option->Text.Peek(), 0);
				option->Gadget = new DynamicWindowGadgetClass(option->Window.Peek(), 5, 3, false);
				Unknown64.Unknown_6870E0(option->Gadget, count + 1, View, true, true);
				option->Gadget->Unknown_65FFA0(-1);
				option->Unknown0C = nstrdup(symbol_asset);
				option->Unknown10 = category == 2 ? true : false;
				Options.push_back(option);
				count++;
			}
		}
	}
}

// 0x64AF80
void UISymbolChatClass::Unknown_64AF80()
{
	for (std::vector<UISymbolChatOptionStruct *>::iterator it = Options.begin(); it != Options.end(); it++) {
		UISymbolChatOptionStruct *option = *it;
		Unknown64.Unknown_687150(option->Gadget);
		delete option;
	}
	Options.clear();
	for (int i = 0; i < 3; i++) {
		if (Sections[i]) {
			Sections[i]->Remove_All(false);
		}
	}
}

// 0x64B050
void UISymbolChatClass::Unknown_58()
{
	if (!Is_Active()) {
		InterfaceSortScopeClass scope(this);
		Unknown84 = 0;
		float x;
		float y;
		Unknown_42ECF0(&x, &y);
		x = x * 640.0f - 320.0f;
		y = 240.0f - y * 480.0f;
		Window->Set_Center(Vector2(x, y));
		Window->Add_To_Scene((SceneClass *)View->Unknown_50());
		InterfaceClass::Unknown_58();
		SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
	}
}

// 0x64B180
void UISymbolChatClass::Unknown_5C()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
		Unknown84 = 0;
		Window->Remove_From_Scene();
	}
}

// 0x64B1C0
bool UISymbolChatClass::Unknown_68(int key)
{
	if (key) {
		Unknown84 = key;
		SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
		return(true);
	}
	return(false);
}

// 0x64B200
void UISymbolChatClass::Unknown_7C(UIViewClass *view, bool arg)
{
	if (View != view) {
		InterfaceClass::Unknown_7C(view, arg);
	}
}

// 0x64B230
void UISymbolChatClass::Unknown_60()
{
	if (Active) {
		float x;
		float y;
		Unknown_42ECF0(&x, &y);
		if (!Gadget->Unknown_30(x, y)) {
			if (Unknown_42EC60(1, 0) || Unknown_42EC60(2, 0)) {
				Unknown_5C();
			}
		}
	}
}

// 0x64B2C0
void UISymbolChatClass::Unknown_64B2C0()
{
	if (!Active) {
		Unknown_64AF80();
		Unknown_64A9B0();
	}
}

// 0x64B2F0
int UISymbolChatClass::Unknown_64B2F0(char *text, bool *leader)
{
	int option = Unknown84;
	if (option && Options[option - 1]) {
		strcpy(text, Options[option - 1]->Unknown0C);
		*leader = Options[option - 1]->Unknown10;
	}
	Unknown84 = 0;
	return(option);
}

/*
**	Notes (O12, 2026-11-12)
**
**	The object: the __FILE__ (0xB8BB48) and line 152 of 0x64A9B0's Message_Error; 'fc_symbolchat.ini' loaded by
**	the constructor 0x64A480 and unloaded by the destructor 0x64A7D0. Its range 0x64A330-0x64B6D7: the previous
**	object ends with its guarded std::ctype<unsigned short>::id initialiser 0x64A2E0 and table thunks (0x64A320),
**	so the rows' constructor 0x64A330 opens this one; this one ends with the guarded initialiser 0x64B690 and the
**	table thunks 0x64B6C0 / 0x64B6D0. Compiler output in the range: the deleting destructor 0x64A7A0, RefCountPtr's
**	out-of-line default constructor 0x64B390 (the eh vector constructor's element for Sections), and the rows'
**	vector members 0x64B3B0 (destructor) and 0x64B3F0 (insert).
**
**	The menu is the twin of the chat panel's reply list (unknown_56e170.cpp) and ui_starbase.cpp's symbol chat
**	menu, with three sections and its symbols read from symbolchatdef.ini ('%s_INFO_%d' of GENERIC, TEAM,
**	CONTEXT: SymbolString shown, SymbolAsset kept). Forms the client's bytes settle: Unknown_58 tests
**	!Is_Active() (the flag loaded into al before the compare, where 0x56ED10's !Active compares in memory); the
**	symbols' inner loop is a while (true) with the index raised before Add_Object (a for loop duplicates the
**	sprintf / Find_Section test at its bottom).
*/
