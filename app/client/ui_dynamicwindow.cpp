// Earth & Beyond client (reconstruction): app\Client\ui_dynamicwindow.cpp (__FILE__ 0xB7EF24). Notes at the end of the file.
#define __INTRFACE_MEMBERS	// intrface.h's members (the constructor 0x540D30)
#include "ui_dynamicwindow.h"
#include "sclient.h"
#include "dynamic_window_class.h"
#include "dynamic_window_text.h"
#include "gd_dynamicwindow.h"
#include "gadget.h"
#include "local_interface_sort.h"
#include "unknown_signalbase.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\shared\g_assert.h"
#include <string>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <stdarg.h>

// 0x589D30
UIDynamicWindowClass::UIDynamicWindowClass(LineDisplayClientClass *client, const char *name, int level) :
	InterfaceClass(client),
	Unknown8C(NULL),
	Unknown90(0x457),
	UnknownA4(0),
	UnknownA8(true),
	UnknownA9(true)
{
	if (name == NULL || *name == '\0') {
		name = "STANDARD_BLANK_WINDOW";
	}
	MainWindow = Create_Get(new DynamicWindowClass(name, 4));
	MainWindow->Set_Level(level);
	Unknown8C = new DynamicWindowGadgetClass(MainWindow, 0x55, 3, false);
	Unknown8C->Unknown_65FFA0(1000);
	Unknown8C->Set_Dynamic_Object(MainWindow);
	Unknown64.Unknown_6870E0(Unknown8C, 0xFFFFFF, View, 1, 1);
}

// 0x58A0B0
UIDynamicWindowClass::~UIDynamicWindowClass()
{
	Unknown_58ADA0();
	MainWindow->Remove_All(false);
	if (Unknown8C) {
		Unknown64.Unknown_687150(Unknown8C);
		GadgetClass::Destroy(Unknown8C);
		Unknown8C = NULL;
	}
	MainWindow = NULL;
}

// 0x58A2C0
void UIDynamicWindowClass::Unknown_58()
{
	if (!Is_Active()) {
		InterfaceSortScopeClass sort(this);
		UnknownA4 = 0;
		Vector2 size = MainWindow->Get_Min_Size();
		MainWindow->Layout_Size(size);
		MainWindow->Add_To_Scene((SceneClass *)View->Unknown_50());
		InterfaceClass::Unknown_58();
	}
}

// 0x58A390
void UIDynamicWindowClass::Unknown_5C()
{
	MainWindow->Remove_From_Scene();
	InterfaceClass::Unknown_5C();
}

// 0x58A3C0
bool UIDynamicWindowClass::Unknown_68(int key)
{
	if (key != 0xFFFFFF) {
		InterfaceSortScopeClass sort(this);
		GadgetEvent event;
		event.Id = key;
		Unknown_4F7EF0().emit((GadgetCollectionInterface *)this, event);
		return(false);
	}
	if (UnknownA9) {
		Unknown_5C();
	}
	return(true);
}

// 0x58A4B0
void UIDynamicWindowClass::Unknown_60()
{
	if (UnknownA8) {
		if ((bool)Unknown84_04()->Unknown2C_02) {
			UnknownA4 = 0;
			return;
		}
		UnknownA4 += WW3D::Get_Frame_Time();
		if (UnknownA4 > 1000) {
			Unknown_5C();
		}
	}
}

// 0x58A530
RefCountPtr<RenderObjClass> UIDynamicWindowClass::Get_Render_Object()
{
	if (!MainWindow) {
		return(NULL);
	}
	return(MainWindow->Get_Render_Object());
}

// 0x58A580
void UIDynamicWindowClass::Unknown_58A580(Vector2 position)
{
	if (View) {
		InterfaceSortScopeClass sort(this);
		MainWindow->Set_Center(position);
		return;
	}
	MainWindow->Set_Center(position);
}

// 0x58A640
void UIDynamicWindowClass::Get_Center(Vector2 &center)
{
	MainWindow->Get_Center(center);
}

























































































































// 0x58A670
void UIDynamicWindowClass::Add_Object(DynamicWindowElementClass *object, int row_index)
{
	FAIL_IF(!object) return;
	MainWindow->Add_Object(object, row_index);
}
// 0x58A6C0
void UIDynamicWindowClass::Remove_Object(DynamicWindowElementClass *object)
{
	FAIL_IF(!object) return;
	RefCountPtr<DynamicWindowElementClass> element = RefCountPtr<DynamicWindowElementClass>(Create_Peek(object));
	MainWindow->Remove_Child(element);
}
// 0x58A7E0
void UIDynamicWindowClass::Add_Object(DynamicWindowRenderObjectClass *object, int row_index)
{
	FAIL_IF(!object) return;
	MainWindow->Add_Object(object, row_index);
}
// 0x58A830
void UIDynamicWindowClass::Remove_Object(DynamicWindowRenderObjectClass *object)
{
	FAIL_IF(!object) return;
	RefCountPtr<DynamicWindowElementClass> element = Create_Peek(object);
	MainWindow->Remove_Child(element);
}
// 0x58A950
void UIDynamicWindowClass::Add_Element(const RefCountPtr<DynamicWindowElementClass> &element, int row_index)
{
	FAIL_IF(!MainWindow) return;
	MainWindow->Add_Element(element, row_index);
}
// 0x58A9A0
void UIDynamicWindowClass::Remove_All(bool recursive)
{
	FAIL_IF(!MainWindow) return;
	MainWindow->Remove_All(recursive);
}
// 0x58A9F0
void UIDynamicWindowClass::Remove_Element(const RefCountPtr<DynamicWindowElementClass> &element)
{
	FAIL_IF(!MainWindow) return;
	MainWindow->Remove_Element(element, -1);
}


// 0x58AA40
GadgetClass *UIDynamicWindowClass::Unknown84_04()
{
	return(Unknown8C);
}

// 0x58AA60
void UIDynamicWindowClass::Unknown84_08(GadgetClass *gadget, int arg1, int arg2)
{
	FAIL_IF(Is_Active()) return;

	FAIL_IF(!gadget) return;
	gadget->Unknown_65FFC0(arg2, Unknown8C);
	Unknown64.Unknown_6870E0(gadget, arg1, View, 1, 1);
	Unknown94.push_back(gadget);
}

// 0x58AD00
void UIDynamicWindowClass::Unknown84_0C(GadgetClass *gadget)
{
	std::vector<GadgetClass *>::iterator i = std::find(Unknown94.begin(), Unknown94.end(), gadget);
	if (i != Unknown94.end()) {
		Unknown94.erase(i);
	}
	Unknown64.Unknown_687150(gadget);
}

// 0x58AD80
int UIDynamicWindowClass::Unknown84_10()
{
	return(Unknown90++);
}









// 0x58ADA0
void UIDynamicWindowClass::Unknown_58ADA0()
{
	FAIL_IF(Is_Active()) return;
	for (std::vector<GadgetClass *>::iterator i = Unknown94.begin(); i != Unknown94.end(); ++i) {
		Unknown64.Unknown_687150(*i);
		GadgetClass::Destroy(*i);
	}
	Unknown94.clear();
}

// 0x58AE70
void UIDynamicWindowClass::Update_Layout()
{
	MainWindow->Update_Layout();
}
// NOT EXACT: 0x58AE90, inline-budget: the client calls std::string::_Eos out of line for the font name's string where this inlines it (docs/attempts/58AE90)
void UIDynamicWindowClass::Add_Text(DynamicWindowClass *window, const char *font_name, int row_index, unsigned int flags, const char *format, ...)
{
	FAIL_IF(!window) return;
	char text[512];
	text[0] = '\0';
	if (format) {
		va_list args;
		va_start(args, format);
		_vsnprintf(text, 511, format, args);
		va_end(args);
	}
	RefCountPtr<DynamicWindowTextClass> element = DynamicWindowTextClass::Create(font_name, text, flags | 6);
	window->Add_Element(element, row_index);
}


// NOT EXACT: inline-budget: the client calls std::string::_Eos out of line for both strings where this inlines them (docs/attempts/58B160)
// 0x58B160
void UIDynamicWindowClass::Add_Text(const char *font_name, int row_index, unsigned int flags, const char *format, ...)
{
	FAIL_IF(!MainWindow) return;
	char text[512];
	text[0] = '\0';
	if (format) {
		va_list args;
		va_start(args, format);
		_vsnprintf(text, 511, format, args);
		va_end(args);
	}
	RefCountPtr<DynamicWindowTextClass> element = DynamicWindowTextClass::Create(font_name, text, flags | 6);
	MainWindow->Add_Element(element, row_index);
}


/*
**	Reconstruction notes (ui_dynamicwindow.cpp, client 0x589D30-0x58B62F)
**
**	The object: the constructor 0x589D30 opens it (ViewUnknown34Class's adjustor thunks 0x589D10 / 0x589D20 close the
**	message box's object before it); GadgetCollectionInterface's inline destructor 0x58A030 / 0x58A050, the destructor
**	0x58A080 / 0x58A0B0, the members in source order, the template instances 0x58B420-0x58B5A0 (the two Create_Peek,
**	vector<GadgetClass *>'s destructor and helpers, std::copy, _Construct), the guarded <string> initialiser 0x58B5D0
**	and the adjustor thunks 0x58B600 / 0x58B610 / 0x58B620 (+0x04, +0x38, +0x84) close it; 0x58B630's PermStringInitClass
**	static opens the next object. The asserts sit on the client's lines 252-373 (blank lines above 0x58A670 keep them).
**	Unknown_68 emits a GadgetEvent with the gadget id on Unknown_4F7EF0's map, GadgetCollectionInterface as sender
**	(dynamic_window_toggle.cpp's ToggleButtonDynamicWindow listens). 0x58B160 is a variadic member (cdecl, this first):
**	ui_starmap.cpp's 'STARMAP_HOW_TO' lines.
*/
