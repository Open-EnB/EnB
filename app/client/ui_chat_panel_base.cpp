// Earth & Beyond client (reconstruction): app\Client\ui_chat_panel_base.cpp. Notes at the end of the file.
#define __INTRFACE_MEMBERS
#include <string.h>
#include "ui_chat_panel.h"
#include "client.h"
#include "rendobj.h"
#include "assetmgr.h"
#include "fontmanager.h"
#include "fontdatabase.h"
#include "gadget.h"
#include "gd_slider.h"
#include "unknown_675120.h"
#include "unknown_signalbase.h"
#include "..\shared\g_assert.h"

void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl: effect.ini's ResetMappers)  // NAME 1 [robj, make_geometry_unique]: as dynamic_window_background.cpp
extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)
extern FontStringFilesClass *Unknown_BE67BC;	// NAME 1: neutral (D10)

#define InterfaceMesh Unknown84		// the asserts' name for +0x84 (the header's Unknown84 serves the files that read it)

// 0x56D280
UIChatPanelBaseClass::UIChatPanelBaseClass(SClient *client) :
	InterfaceClass((LineDisplayClientClass *)client),
	InterfaceMesh(NULL),
	Unknown98(0),
	Unknown9C(0)
{
}

// NOT EXACT: functional (WE1)
// 0x56D330
UIChatPanelBaseClass::~UIChatPanelBaseClass()
{
	if (Unknown9C != NULL) {
		GadgetClass::Get_Signal_Map().disconnect((GadgetClass *)Unknown9C, this, &UIChatPanelBaseClass::Unknown_56DEE0, DestroyGadgetEvent());
	}
	const char *name = Unknown88.c_str();
	Unknown_BE67C4->Remove(InterfaceMesh.Peek());
	Unknown_BE67BC->Unload(name);
	if (InterfaceMesh->Get_Container() != NULL) {
		InterfaceMesh->Remove();
	}
	Unknown88.erase();
}

// 0x56D4F0
void UIChatPanelBaseClass::Unknown_58()
{
	Unknown98 = 0;
	InterfaceClass::Unknown_58();
}





































// NOT EXACT: functional (WE1)
// 0x56D510
GadgetClass *UIChatPanelBaseClass::Unknown_56D510(int id, const char *bttn_name, const char *text)
{
	FAIL_IF(!bttn_name || !*bttn_name) return(NULL);

	RefCountPtr<RenderObjClass> robj = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name(bttn_name));
	FAIL_IF(!robj) return(NULL);
	Unknown_496AE0(robj.Peek(), false);
	ButtonGadgetClass *gadget = new ButtonGadgetClass(robj.Peek(), 0x55, 3);
	Unknown64.Unknown_6870E0(gadget, id, View, true, false);
	if (text != NULL) {
		gadget->Unknown_98(Unknown_BE67C4->Get_Style(text, InterfaceMesh.Peek()));
	}
	return(gadget);
}



















// NOT EXACT: functional (WE1)
// 0x56D8D0
GadgetSlider *UIChatPanelBaseClass::Unknown_56D8D0(int id, int unknown)
{
	GadgetSlider *gadget = new GadgetSlider(unknown, 0);
	Unknown64.Unknown_6870E0(gadget, id, View, true, false);
	return(gadget);
}
























// NOT EXACT: functional (WE1)
// 0x56DB20
bool UIChatPanelBaseClass::Unknown_98(const char *name, const char *ini)
{
	InterfaceMesh = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj(name));
	FAIL_IF(!InterfaceMesh) return(false);
	if (InterfaceMesh->Get_Container() == NULL && InterfaceMesh->Peek_Scene() == NULL) {
		Unknown58.Unknown_686CC0(InterfaceMesh.Peek(), NULL);
	}
	if (ini == NULL) {
		return(true);
	}
	Unknown88 = ini;
	Unknown_BE67BC->Load(ini);
	Unknown_BE67C4->Add(InterfaceMesh.Peek());
	return(true);
}


// 0x56DD80
void UIChatPanelBaseClass::Unknown_88(const RefCountPtr<RenderObjClass> &robj, const char *bone_name)
{
	FAIL_IF(!robj) return;
	FAIL_IF(!bone_name || !*bone_name) return;
	Unknown58.Unknown_686DD0(InterfaceMesh.Peek());
	InterfaceMesh->Remove();
	int bone_idx = robj->Add_Sub_Object_To_Bone(InterfaceMesh.Peek(), bone_name);






	G_ASSERT(bone_idx);
}
// 0x56DE60
void UIChatPanelBaseClass::Unknown_8C(const RefCountPtr<RenderObjClass> &robj)
{
	FAIL_IF(robj.Peek() != InterfaceMesh->Get_Container()) return;
	InterfaceMesh->Remove();
	Unknown58.Unknown_686CC0(InterfaceMesh.Peek(), NULL);
}

// 0x56DEE0
void UIChatPanelBaseClass::Unknown_56DEE0(DestroyGadgetEvent *event, GadgetClass *sender)
{
	if (sender == (GadgetClass *)Unknown9C) {
		Unknown_9C();
	}
}
/*
** Reconstruction notes (WE1, 2026-11-06)
**
** The chat panel's base and the base of ui_friends.cpp's, ui_playeropt.cpp's and ui_starbase.cpp's lights panel:
** InterfaceClass and a model at +0x84, its string file's name at +0x88, +0x98, and the movable back gadget at +0x9C.
** 0x56D6F0 and 0x56D970 (asserts on lines 113, 116, 141) are not written yet: their lines are blank.
*/
