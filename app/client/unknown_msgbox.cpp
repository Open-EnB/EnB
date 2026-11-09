// Earth & Beyond client (reconstruction): the views' message box, sclient.h's ViewUnknown34Class (client 0x587570-0x589D20).
// File PROVISIONAL (D137): no string; after the gadget run 0x585780- and before ui_dynamicwindow.cpp (0x58A030). Notes at the end.
#include "sclient.h"
#include "dynamic_window_class.h"
// fontmanager.h (with fontdatabase.h; its 0xBE67B8 is tsk_game.h's since P-H12-3)

#include "fontmanager.h"

#include "gadget.h"
#include "gd_dynamicwindow.h"
#include "unknown_51c800.h"
#include "local_interface_sort.h"
#include "..\input\input.h"
#include "..\input\inpdsp.h"
#include "..\audio\soundlib.h"
#include "..\shared\unknown_font_split_string.h"
#include "ini.h"
#include "wwstring.h"
#include "nstrdup.h"
#include "matrix3d.h"
#include "aabox.h"
#include "layer.h"
#include <string.h>
#include <stdio.h>

// InterfaceClass +0x4C's key list (control_key_list.cpp): as ui_avsel.cpp's
struct ControlKeyComboStruct; class ControlKeyListClass { public: void Add(const ControlKeyComboStruct &combo, void *handler); void Remove(const ControlKeyComboStruct &combo); };	// InterfaceClass +0x4C: control_keys_manager.h's key list (Add 0x721E60, Remove 0x721EF0), as far as this file calls it
// tsk_game.h's settings store: the web browser windows (0x7CA570 asks DX8WebBrowser::Is_Browser_Open)
class MsgBoxBrowsersClass { public: bool Unknown_7CA570(int index); void Unknown_7CAD00(int index, const AABoxClass &box, const char *url); void Unknown_7CB240(int index); };	// NAME 1: neutral (D10)  // NAME 1 [index, box, url]: what the box passes
void Unknown_497240(RenderObjClass *robj, bool on);	// 0x497240 (ro_funcs.cpp: MeshGeometryClass::ALLOW_NPATCHES on every mesh's model, through the sub-objects)  // NAME 1 [Unknown_497240, robj, on]: neutral (D10)

// (inline: the destructor and 0x588A70 call the message's destructor 0x587570 and the deque's _Freefront 0x589B30 out of line)
inline void ViewUnknown34Class::Flush_Queue()
{
	while (!Queue.empty()) {
		delete Queue.front();
		Queue.pop_front();
	}
}

// 0x587570
ViewUnknown34MessageStruct::~ViewUnknown34MessageStruct()
{
	if (Type) {
		delete Type;
		Type = NULL;
	}
	if (Text) {
		delete Text;
		Text = NULL;
	}
}

// 0x5875C0
ViewUnknown34Class::ViewUnknown34Class(UIViewClass *view, const LayerClass &layer) :
	InterfaceClass(view, (int)layer.Peek_Scene()),
	HasOptions(false),
	FollowMouse(false),
	HideOnKey(false),
	Layer(layer),
	OptionCount(0),
	Button(0),
	Window(NULL),
	Info(NULL),
	HTML(false),
	MaxLines(10)
{
	int i;
	for (i = 0; i < 16; i++) {
		Headers[i] = NULL;
	}
	for (i = 0; i < 5; i++) {
		Options[i] = NULL;
		OptionTexts[i] = NULL;
		OptionGadgets[i] = NULL;
		Keys[i].Key = 0;
	}
	Type[0] = 0;
	Unknown_BE67BC->Load("fc_dialogbox.ini");
	Window = new DynamicWindowClass("DIALOG_BOX_WINDOW", 3);
	Window->Set_Level(3);
	Unknown_497240(Window->Get_Render_Object().Peek(), false);
	Info = new DynamicWindowClass("DIALOG_BOX_INFO", 2);
	Unknown_497240(Info->Get_Render_Object().Peek(), false);
	Window->Add_Object(Info, 0);
	Gadget = new DynamicWindowGadgetClass(Window, 0x55, 3, false);
	Gadget->Set_First_Sort_Group(true);
	Unknown64.Unknown_6870E0(Gadget, 0, View, 1, 1);
}

// 0x587A10
ViewUnknown34Class::~ViewUnknown34Class()
{
	Flush_Queue();
	Unknown_5C();
	Unknown_588340();
	if (Gadget) {
		Unknown64.Unknown_687150(Gadget);
		GadgetClass::Destroy(Gadget);
		Gadget = NULL;
	}
	if (Info) {
		Info->Remove_All(false);
		Info->Release_Ref();
		Info = NULL;
	}
	if (Window) {
		Window->Remove_All(false);
		Window->Release_Ref();
		Window = NULL;
	}
	Unknown_BE67BC->Unload("fc_dialogbox.ini");
}

// 0x587D50
void ViewUnknown34Class::Unknown_587D50()
{
	char name[32];
	char info[32];
	INIClass ini("dialogbox.ini");
	if (ini.Is_Loaded()) {
		StringClass str;
		if (ini.Find_Section(Type)) {
			const char *font = Unknown_589740(ini);
			int i;
			for (i = 0; i < MaxLines; i++) {
				sprintf(name, "Header%d", i);
				if (ini.Find_Entry(Type, name)) {
					FontSys *text = new FontSys(font, 0, ini.Get_String(str, Type, name));
					Headers[i] = new Unknown51C800Class(text, 6);
					Info->Add_Object(Headers[i], i);
					if (HTML) {
						text->Set_Hidden(1);
					}
					text->Release_Ref();
				}
			}
			OptionCount = 0;
			bool more = true;	// (the client's loop is not rotated: a flag never cleared; for(;;), do-while and while(comma) rotate)
			while (more) {
				sprintf(name, "Option%d", OptionCount);
				if (!ini.Find_Entry(Type, name)) {
					break;
				}
				Options[OptionCount] = new DynamicWindowClass("DIALOG_BOX_OPTION", 1);
				Window->Add_Object(Options[OptionCount], 1);
				sprintf(info, "OPTION_INFO_%d", OptionCount);
				FontSys *text = new FontSys(info, 0, NULL);
				text->Change_Text(ini.Get_String(str, Type, name), 0);
				OptionTexts[OptionCount] = new Unknown51C800Class(text, 6);
				Options[OptionCount]->Add_Object(OptionTexts[OptionCount], 0);
				text->Release_Ref();
				sprintf(info, "Key%d", OptionCount);
				Keys[OptionCount] = Unknown_42A1A0(ini, Type, info);
				KernelEventClass key = Keys[OptionCount];
				if (key.Key != 0x1B && key.Key != 0x0D) {
					Keys[OptionCount].Flags = 0x400;
				}
				OptionGadgets[OptionCount] = new DynamicWindowGadgetClass(Options[OptionCount], 0x55, 3, true);
				OptionGadgets[OptionCount]->Unknown_65FFA0(-1002);
				OptionGadgets[OptionCount]->Set_First_Sort_Group(true);
				Unknown64.Unknown_6870E0(OptionGadgets[OptionCount], OptionCount + 1, View, 1, 1);
				OptionCount++;
			}
			if (OptionCount) {
				HasOptions = true;
			}
		}
	}
}

// 0x588340
void ViewUnknown34Class::Unknown_588340()
{
	int i;
	for (i = 0; i < OptionCount; i++) {
		if (Options[i]) {
			Window->Remove_Object(Options[i], -1);
			Options[i]->Remove_All(false);
			Options[i]->Release_Ref();
			Options[i] = NULL;
		}
		if (OptionTexts[i]) {
			OptionTexts[i]->Release_Ref();
			OptionTexts[i] = NULL;
		}
		if (OptionGadgets[i]) {
			Unknown64.Unknown_687150(OptionGadgets[i]);
			GadgetClass::Destroy(OptionGadgets[i]);
			OptionGadgets[i] = NULL;
		}
		Keys[i].Key = 0;
	}
	if (Info) {
		Info->Remove_All(false);
	}
	for (i = 0; i < 16; i++) {
		if (Headers[i]) {
			Headers[i]->Release_Ref();
			Headers[i] = NULL;
		}
	}
	OptionCount = 0;
	HasOptions = false;
}

// 0x588480
void ViewUnknown34Class::Unknown_58()
{
	if (!Active && HasOptions) {
		InterfaceSortClass sort(Layer.Peek_Scene(), true);
		if (FollowMouse) {
			float x;
			float y;
			Unknown_42ECF0(&x, &y);
			x = x * 640.0f - 320.0f;
			y = 240.0f - y * 480.0f;
			Window->Set_Center(Vector2(x, y));
		} else {
			Window->Set_Center(Vector2(0.0f, 0.0f));
		}
		Window->Add_To_Scene(Layer.Peek_Scene());
		for (int i = 0; i < OptionCount; i++) {
			if (Keys[i].Key) {
				((ControlKeyListClass *)Unknown4C)->Add(*(const ControlKeyComboStruct *)&Keys[i], (void *)(i + 1));
			}
		}
		if (HTML) {
			BrowserDelay = 3;
		}
		InterfaceClass::Unknown_58();
	}
}

// 0x588640
void ViewUnknown34Class::Unknown_5C()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
		if (HTML) {
			((MsgBoxBrowsersClass *)Unknown_7CA2D0())->Unknown_7CB240(7);
		}
		for (int i = 0; i < OptionCount; i++) {
			if (Keys[i].Key) {
				((ControlKeyListClass *)Unknown4C)->Remove(*(const ControlKeyComboStruct *)&Keys[i]);
			}
		}
		Window->Remove_From_Scene();
		HideOnKey = false;
		if (!Queue.empty()) {
			ViewUnknown34MessageStruct *message = Queue.front();
			Queue.pop_front();
			if (message) {
				Unknown_588A70(message->Type, message->FollowMouse, message->Text, message->Width, message->HideOnKey, false);
				delete message;
				return;
			}
		}
		Type[0] = 0;
	}
}

// 0x588870
void ViewUnknown34Class::Unknown_588870()
{
	if (Active && HTML) {
		if (BrowserDelay) {
			if (--BrowserDelay == 0) {
				AABoxClass box;
				Info->Get_Bounds(box);
				Vector3 center = box.Center;
				Vector3 extent = box.Extent;
				Matrix3D tm(true);
				tm.Scale(0.93f, 0.9f, 1.0f);
				tm.Transform_Center_Extent_AABox(center, extent, &box.Center, &box.Extent);
				((MsgBoxBrowsersClass *)Unknown_7CA2D0())->Unknown_7CAD00(7, box, URL.c_str());
			}
		} else if (!((MsgBoxBrowsersClass *)Unknown_7CA2D0())->Unknown_7CA570(7)) {
			Unknown_68(1);
		}
	}
}

// 0x588A20
bool ViewUnknown34Class::Unknown_68(int key)
{
	if (key) {
		if (HideOnKey) {
			Unknown_5C();
		} else {
			Button = key;
		}
		SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
		return(true);
	}
	return(false);
}

// 0x588A70
void ViewUnknown34Class::Unknown_588A70(const char *type, bool follow_mouse, const char *text, int width, bool hide_on_key, bool flush)
{
	if (_stricmp(type, "HTML") == 0) {
		HTML = true;
		URL = "about:<html><body onload=\"window.external.EnableNavigation=false;\" onselectstart=\"return false;\" style=\"color:#FFFFFF;background-color:#1b2546\"><table width=\"100%\" height=\"100%\"><td align=\"middle\" style=\"font-family:Arial;font-size:12pt\">";
		URL += text;
		URL += "</td></table></body><html>";
		text = NULL;
		width = 0;
	} else {
		HTML = false;
	}
	if (flush) {
		Flush_Queue();
		Unknown_5C();
	}
	if (Active) {
		Unknown_589140(type, follow_mouse, text, width, hide_on_key);
		return;
	}
	Unknown_588340();
	strcpy(Type, type);
	Unknown_587D50();
	if (text && width) {
		Unknown_588DC0(text, width);
	}
	// (three dead reads of the queue's front: they spend the inline budget the client's source spent, so the appends' _Eos
	// and the flush's ~message / _Freefront stay out of line as in the client; they emit no code)
	Queue.front();
	Queue.front();
	Queue.front();
	FollowMouse = follow_mouse;
	Unknown_58();
	SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
	if (Active) {
		HideOnKey = hide_on_key;
	}
}

// 0x589CA0: the text of a header element (a RefCountPtr of its render object)
RefCountPtr<FontSys> Unknown_589CA0(const RefCountPtr<RenderObjClass> &robj)	// NAME 1: neutral (D10) (ui_ping.cpp's 0x5D85B0 calls it too)
{
	return(Create_Peek((FontSys *)robj.Peek()));
}

// 0x588DC0
void ViewUnknown34Class::Unknown_588DC0(const char *text, int width)
{
	RefCountPtr<FontSys> first = Unknown_589CA0(((DynamicWindowRenderObjectClass *)Headers[0])->Get_Render_Object());
	RefCountPtr<Font32Class> *font = &first->Font;	// (FontSys::Font held as a RefCountPtr: the client loads the font before the text through a local)
	FontSplitStringClass split(*font, text, (float)width, 0);
	INIClass ini("dialogbox.ini");
	unsigned int count = min(split.Count(), (unsigned int)MaxLines);
	unsigned int i;
	for (i = 0; i < count; i++) {
		if (Headers[i]) {
			RefCountPtr<FontSys> line = Unknown_589CA0(((DynamicWindowRenderObjectClass *)Headers[i])->Get_Render_Object());
			line->Change_Text(split.Get(i), 0);
			Headers[i]->Invalidate();
		} else {
			FontSys *line = new FontSys(Unknown_589740(ini), 0, split.Get(i));
			Headers[i] = new Unknown51C800Class(line, 6);
			Info->Add_Object(Headers[i], i);
			line->Release_Ref();
		}
	}
	for (i = count; i < 16; i++) {
		if (Headers[i]) {
			Info->Remove_Object(Headers[i], -1);
			Headers[i]->Release_Ref();
			Headers[i] = NULL;
		}
	}
}

// 0x5890B0
int ViewUnknown34Class::Unknown_5890B0(const char *button)
{
	if (button && strcmp(Type, button) != 0) {
		return(0);
	}
	int result = Button;
	Button = 0;
	return(result);
}

// 0x589140
void ViewUnknown34Class::Unknown_589140(const char *type, bool follow_mouse, const char *text, int width, bool hide_on_key)
{
	ViewUnknown34MessageStruct *message = new ViewUnknown34MessageStruct;
	message->Type = nstrdup(type);
	message->FollowMouse = follow_mouse;
	if (text && width) {
		message->Text = nstrdup(text);
		message->Width = width;
		message->HideOnKey = hide_on_key;
	}
	Queue.push_back(message);
}

// 0x589740
const char *ViewUnknown34Class::Unknown_589740(INIClass &ini)
{
	const char *font = "DIALOG_INFO";
	StringClass str;
	MaxLines = 10;
	if (ini.Is_Loaded() && ini.Find_Entry(Type, "FontSize")) {
		ini.Get_String(str, Type, "FontSize");
		if (strcmp(str, "FONT_MED") == 0) {
			font = "DIALOG_INFO";
		}
		if (strcmp(str, "FONT_SMALL") == 0) {
			font = "DIALOG_INFO_SMALL";
			MaxLines = 16;
		}
	}
	return(font);
}

/*
**	Reconstruction notes (MSG, 2026-11-02)
**
**	Client 0x587570-0x589D20, the message box every view holds at +0x34 (UIViewClass's Init 0x65A820 makes it):
**	the queued message's destructor 0x587570, the constructor 0x5875C0, the deleting destructor 0x587900 and the
**	destructor 0x587A10, the dialogbox.ini reader 0x587D50, the clear 0x588340, the slots +0x58 0x588480 (shown),
**	+0x5C 0x588640 (hidden: the next queued message shown), the browser step 0x588870, slot +0x68 0x588A20, the show
**	0x588A70, the text 0x588DC0, the button 0x5890B0, the queue 0x589140, the font 0x589740; then the std::deque
**	instances 0x589970-0x589C60, the RefCountPtr helper 0x589CA0 and the two adjustor thunks 0x589D10 / 0x589D20.
**	No string names the file (the client's neighbours: the gadgets before, ui_dynamicwindow.cpp after).
*/
