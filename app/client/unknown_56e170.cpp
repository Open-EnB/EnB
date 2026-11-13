// Earth & Beyond client (reconstruction): app\client\unknown_56e170.cpp, the chat panel's reply list (0x56E170-0x56F40F).
// Notes at the end.
#define __INTRFACE_MEMBERS	// intrface.h's members (the constructor 0x540D30 with the client)
#include "unknown_56e170.h"
#include "gadget.h"
#include "gd_dynamicwindow.h"
#include "dynamic_window_class.h"
#include "unknown_51c800.h"
#include "fontmanager.h"
#include "local_interface_sort.h"
#include "..\audio\soundlib.h"
#include "vector2.h"
#include "sclient.h"

void Unknown_42ECF0(float *x, float *y);			// 0x42ECF0 (the pointer, 0..1)  // NAME 1 [x, y]: as ui_avatar_cust.cpp's

// 0x56E170
UIChatReplyListOptionStruct::UIChatReplyListOptionStruct() :
	Window(NULL),
	Text(NULL),
	Gadget(NULL)
{
}

// 0x56E190
UIChatReplyListOptionStruct::~UIChatReplyListOptionStruct()
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
}

// 0x56E2A0
UIChatReplyListClass::UIChatReplyListClass(SClient *client) :
	InterfaceClass((LineDisplayClientClass *)client),
	Unknown84(0),
	Window(NULL),
	Section(NULL),
	Gadget(NULL),
	Options(0)
{
	Unknown_BE67BC->Load("fc_symbolchat.ini");
	Window = Create_NEW(new DynamicWindowClass("SYMBOL_CHAT_WINDOW", 3));
	Window->Set_Level(8);
	Section = Create_NEW(new DynamicWindowClass("SYMBOL_CHAT_SECTION", 1));
	Window->Add_Object(Section.Peek(), 0);
	Gadget = new DynamicWindowGadgetClass(Window.Peek(), 0x55, 3, false);
	Unknown64.Unknown_6870E0(Gadget, 0, View, true, true);
}

// 0x56E5A0
UIChatReplyListClass::~UIChatReplyListClass()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
		Window->Remove_From_Scene();
	}
	Unknown_56EC50();
	if (Gadget) {
		Unknown64.Unknown_687150(Gadget);
		GadgetClass::Destroy(Gadget);
		Gadget = NULL;
	}
	if (Section) {
		Section->Remove_All(false);
		Section = NULL;
	}
	if (Window) {
		Window->Remove_All(false);
		Window = NULL;
	}
	Unknown_BE67BC->Unload("fc_symbolchat.ini");
}

// 0x56E7F0
void UIChatReplyListClass::Unknown_56E7F0(std::list<std::string> &names, int arg)
{
	int i = 1;
	for (std::list<std::string>::iterator it = names.begin(); it != names.end(); it++) {
		std::string name = *it;
		RefCountPtr<FontSys> text;
		text = Create_NEW(new FontSys("Symbol_Chat_Default", NULL, NULL));
		text->Change_Text(name.c_str(), 0);
		UIChatReplyListOptionStruct *option = new UIChatReplyListOptionStruct;
		option->Window = Create_NEW(new DynamicWindowClass("SYMBOL_CHAT_OPTION", 1));
		Section->Add_Object(option->Window.Peek(), i);
		option->Text = Create_NEW(new Unknown51C800Class(text.Peek(), 6));
		option->Window->Add_Object(option->Text.Peek(), 0);
		option->Gadget = new DynamicWindowGadgetClass(option->Window.Peek(), 0x55, 3, false);
		Unknown64.Unknown_6870E0(option->Gadget, i, View, true, true);
		option->Gadget->Unknown_65FFA0(-1001);
		Options.push_back(option);
		i++;
	}
}

// 0x56EC50
void UIChatReplyListClass::Unknown_56EC50()
{
	for (std::vector<UIChatReplyListOptionStruct *>::iterator it = Options.begin(); it != Options.end(); it++) {
		UIChatReplyListOptionStruct *option = *it;
		Unknown64.Unknown_687150(option->Gadget);
		delete option;
	}
	Options.clear();
	if (Section) {
		Section->Remove_All(false);
	}
}

// 0x56ED10
void UIChatReplyListClass::Unknown_58()
{
	if (!Active) {
		InterfaceSortScopeClass scope(this);
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

// 0x56EE40
void UIChatReplyListClass::Unknown_5C()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
		Window->Remove_From_Scene();
	}
}

// 0x56EE70
bool UIChatReplyListClass::Unknown_68(int key)
{
	if (key) {
		Unknown84 = key;
		SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
		return(true);
	}
	return(false);
}

// 0x56EEB0
void UIChatReplyListClass::Unknown_56EEB0()
{
}

// 0x56EED0
void UIChatReplyListClass::Unknown_56EED0(std::list<std::string> &names, int arg)
{
	if (!Active) {
		Unknown_56EC50();
		Unknown_56E7F0(names, arg);
	}
}

// 0x56EFB0
int UIChatReplyListClass::Unknown_56EFB0()
{
	int option = Unknown84;
	Unknown84 = 0;
	return(option);
}

/*
**	Notes (O12, 2026-11-12)
**
**	The object: no __FILE__, assert or string names it, so the file name is provisional (NAME 1, the first
**	address). Its range 0x56E170-0x56F417: after ui_chat_panel_base.cpp's ChatPanelUIBase (its deleting
**	destructors 0x56E150 / 0x56E160) and before ChatWindowInterface's constructor 0x56F420 ('fc_chat.ini'); it
**	ends with the guarded std::ctype<unsigned short>::id initialiser 0x56F3D0 and the table thunks 0x56F400 /
**	0x56F410. Compiler output in the range: the deleting destructor 0x56E570, RefCountPtr's Create_NEW / release
**	0x56EFE0-0x56F070 (first linked here, shared), and the options' vector members 0x56F0A0 (destructor),
**	0x56F0E0 (clear) and 0x56F130 (insert).
**
**	The class is the twin of ui_starbase.cpp's symbol chat menu (UIStarbaseUnknown1F8Class, 0x644F80): the same
**	34-slot table, layout and resources ('fc_symbolchat.ini', SYMBOL_CHAT_WINDOW / _SECTION / _OPTION), its rows
**	made from a list of names instead of four fixed options. Forms the client's bytes settle: the gadgets are
**	made over the window's raw pointer (Window.Peek(): the RefCountPtr overload costs a temporary and a frame
**	slot); the destructor calls Unknown_56EC50 (inlined, its vector clear left out of line, as 0x56E63F calls
**	0x56F0E0).
*/
