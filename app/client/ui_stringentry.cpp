// Earth & Beyond client (reconstruction): app\client\ui_stringentry.cpp, a text entry window ('TRADE01A'): the trade
// window's credits, a guild's name and message, a player to ignore. Interim placement (D137): no __FILE__ string; the
// name is the tooltips' section 'UI_STRINGENTRY' (NAME 2). Notes at the end of the file.
#include <string>
#include "sclient.h"
#include "rendobj.h"
#include "..\..\wwlib\ref_ptr.h"
#include "gadget.h"
#include "unknown_675120.h"
#include "textgadg.h"
#include "fontmanager.h"
#include "local_interface_sort.h"
#include "gd_uiback.h"		// MovableUIBackGadget (0x674310)
#include "..\audio\soundlib.h"
#include "..\shared\unknown_4be9d0.h"	// 0x4BE9D0: a name turned into another

extern FontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4
extern const int Unknown_B08C98;			// 0xB08C98 (4)  // NAME 1: neutral (D10)
extern const int Unknown_B08C9C;			// 0xB08C9C (8)  // NAME 1: neutral (D10)
extern const int Unknown_B08CA8;			// 0xB08CA8 (0x40)  // NAME 1: neutral (D10)

void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl)  // NAME 1 [robj, make_geometry_unique]: as eff_o.cpp's
RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj);	// 0x497300  // NAME 1 [robj]: as ui_avatr.cpp's

// The buttons Unknown_54 makes: their models' names (0xB8BA54), also the tooltips' keys
static const char *Unknown_B8BA54[4] = { "CANCEL", "EXIT", "CONFIRM", "VALUE_BUTTON" };	// NAME 1: neutral (D10)

// 0x649890
UIPTradeCreditsClass::UIPTradeCreditsClass(SClient *client, int a, int b) :
	Unknown5BB320Class(new UIMeshClass("TRADE01A", 1), client),
	Unknown0AC(NULL),
	Unknown0C0(0),
	Unknown0C4(a),
	Unknown0C8(b),
	Unknown0CC(false),
	Unknown0CD(false)
{
	for (int i = 0; i < 4; i++) {
		Unknown0B0[i] = NULL;
	}
	Unknown_BE67BC->Load("FA_TRADE01A.INI");
	Unknown_BE67C4->Add(InterfaceMesh->Peek_Mesh_Object());
}

// 0x649A00
UIPTradeCreditsClass::~UIPTradeCreditsClass()
{
	Unknown_BE67C4->Remove(InterfaceMesh->Peek_Mesh_Object());
	Unknown_BE67BC->Unload("FA_TRADE01A.INI");
}

// 0x649AA0
void UIPTradeCreditsClass::Unknown_54()
{
	for (int i = 0; i < 4; i++) {
		RefCountPtr<RenderObjClass> robj = Create_Get(InterfaceMesh->Peek_Mesh_Object()->Get_Sub_Object_By_Name(Unknown_B8BA54[i]));
		Unknown_496AE0(robj.Peek(), 0);
		Unknown0B0[i] = new ButtonGadgetClass(robj.Peek(), 0x55, 3);
		Unknown64.Unknown_6870E0(Unknown0B0[i], i + 1, View, 1, 0);
		Unknown0B0[i]->Unknown_65FFA0(-1001);
		GadgetClass::Set_Tooltip(Unknown0B0[i], "UI_STRINGENTRY", Unknown_B8BA54[i]);
	}
	RefCountPtr<RenderObjClass> model = Create_Get(InterfaceMesh->Peek_Mesh_Object()->Get_Sub_Object_By_Name("TEXT_BOX"));
	if (Unknown0C4 == 0) {
		Unknown0AC = new TextGadgetClass(this, Unknown0C8, "VALUE_BUTTON01", model.Peek(), Unknown_B08CAC | Unknown_B08CA8 | Unknown_B08C98, 0);
	} else if (Unknown0C4 == 1) {
		Unknown0AC = new TextGadgetClass(this, Unknown0C8, "VALUE_BUTTON01", model.Peek(), Unknown_B08C9C | Unknown_B08CAC, 0);
	} else {
		Unknown0AC = new TextGadgetClass(this, Unknown0C8, "VALUE_BUTTON01", model.Peek(), Unknown_B08CAC, 0);
	}
	Unknown0AC->Set_Text("", 0);
	Unknown64.Unknown_6870E0(Unknown0AC, 6, View, 1, 0);
	GadgetClass *back = new MovableUIBackGadget(Unknown_497300(InterfaceMesh->Peek_Mesh_Object()), Vector3(0.0f, 0.0f, 0.0f));	// NAME 1: neutral (D10)
	Unknown64.Unknown_6870E0(back, 7, View, 1, 0);
	Unknown_10();
}

// 0x649E90
void UIPTradeCreditsClass::Unknown_58()
{
	if (!Active) {
		InterfaceSortScopeClass sort(this);	// NAME 1: neutral (D10)
		Unknown0C0 = 0;
		for (int i = 0; i < 4; i++) {
			Unknown0B0[i]->Unknown_40(true);
		}
		Unknown5BB320Class::Unknown_58();
		Unknown0AC->Unknown_74(true);
		Unknown0AC->Set_Text("", 0);
		Unknown0CC = true;
	}
}

// 0x649F70
void UIPTradeCreditsClass::Unknown_5C()
{
	if (Active) {
		if (Unknown0AC) {
			Unknown0AC->Unknown_74(false);
		}
		for (int i = 0; i < 4; i++) {
			Unknown0B0[i]->Unknown_40(false);
		}
		Unknown5BB320Class::Unknown_5C();
	}
}

// 0x649FE0
void UIPTradeCreditsClass::Unknown_60()
{
	if (Unknown0AC) {
		if (Unknown0CD) {
			Unknown0AC->Set_Text(Unknown_4BE9D0(Unknown0AC->Get_Text()).c_str(), 0);
		}
		Unknown0AC->Changed = true;
		Unknown0AC->Unknown_6AAD10();
	}
	if (Unknown0CC && !Unknown0AC->Active && Unknown0C0 == 0) {
		Unknown0C0 = 2;
	}
	Unknown5BB320Class::Unknown_60();
}

// 0x64A110
bool UIPTradeCreditsClass::Unknown_68(int key)
{
	switch (key) {
		case 1:
		case 2:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown0AC->Unknown_74(false);
			Unknown0C0 = 1;
			return(true);
		case 3:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown0AC->Unknown_74(false);
			Unknown0C0 = 2;
			return(true);
		case 4:
			SoundLibraryClass::Play_2D_Sound("message_1.wav", 2);
			Unknown0AC->Unknown_74(true);
			Unknown0AC->Set_Text("", 0);
			Unknown0CC = true;
			return(true);
	}
	return(false);
}

// 0x64A220
int UIPTradeCreditsClass::Unknown_64A220()
{
	return(Unknown0C0);
}

// 0x64A240
const char *UIPTradeCreditsClass::Unknown_64A240()
{
	if (Unknown0AC) {
		return(Unknown0AC->Get_Text());
	}
	return(NULL);
}

// 0x64A270
void UIPTradeCreditsClass::Unknown_64A270(const char *text)
{
	FontSys *font = Unknown_BE67C4->Get_Style("TITLE01", InterfaceMesh->Peek_Mesh_Object());
	if (font) {
		font->Change_Text(text, 0);
	}
}

// 0x64A2B0
void UIPTradeCreditsClass::Unknown_64A2B0(const char *text)
{
	if (Unknown0AC) {
		Unknown0AC->Set_Text(text, 0);
	}
}

/*
** Reconstruction notes
**
** The object: 0x649890-0x64A2C8, then <string>'s guarded initialiser 0x64A2E0 (guard 0xC416FC, atexit 0x42B320) and
** the deleting destructor's two adjustor thunks 0x64A310 / 0x64A320 (tables 0xAF88B0, 0xAF88A4); sbaseview.json's
** 0x64A330 follows. No __FILE__ or assert string: the name is the tooltips' section (NAME 2, interim, D137); the class
** keeps the name its first user gave it (UIPTradeCreditsClass) until the coordinator renames it (D24 would give
** UIStringEntryClass).
** 0x649FE0: Get_Text's copy is never deleted (the client leaks it too).
*/
