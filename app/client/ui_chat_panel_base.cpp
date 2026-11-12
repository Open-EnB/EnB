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
#include "gd_uiback.h"
#include "unknown_7e7e00.h"
#include "unknown_675120.h"
#include "unknown_signalbase.h"
#include "..\shared\g_assert.h"

void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl: effect.ini's ResetMappers)  // NAME 1 [robj, make_geometry_unique]: as dynamic_window_background.cpp
extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)
extern FontStringFilesClass *Unknown_BE67BC;	// NAME 1: neutral (D10)



// 0x56D280
ChatPanelUIBase::ChatPanelUIBase(SClient *client) :
	InterfaceClass((LineDisplayClientClass *)client),
	InterfaceMesh(NULL),
	Closed(0),
	MovableBackGadget(0)
{
}

// 0x56D330
ChatPanelUIBase::~ChatPanelUIBase()
{
	if (MovableBackGadget != NULL) {
		GadgetClass::Get_Signal_Map().disconnect((GadgetClass *)MovableBackGadget, this, &ChatPanelUIBase::On_Back_Gadget_Moved, DestroyGadgetEvent());
	}
	const char *name = IniName.c_str();
	if (name != NULL) {
		Unknown_BE67C4->Remove(InterfaceMesh.Peek());
		Unknown_BE67BC->Unload(name);
	}
	if (InterfaceMesh->Get_Container() != NULL) {
		InterfaceMesh->Remove();
	}
}

// 0x56D4F0
void ChatPanelUIBase::Unknown_58()
{
	Closed = 0;
	InterfaceClass::Unknown_58();
}




































// 0x56D510
GadgetClass *ChatPanelUIBase::Add_Button(int id, const char *bttn_name, const char *text)
{
	FAIL_IF(!bttn_name || !*bttn_name) return(NULL);

	RefCountPtr<RenderObjClass> robj = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name(bttn_name));
	FAIL_IF(!robj) return(NULL);
	Unknown_496AE0(robj.Peek(), false);
	ButtonGadgetClass *gadget = new ButtonGadgetClass(robj.Peek(), 0x55, 3);
	Unknown64.Unknown_6870E0(gadget, id, View, true, false);
	if (text != NULL) {
		FontSys *style = Unknown_BE67C4->Get_Style(text, InterfaceMesh.Peek());
		gadget->Unknown_98(style);
	}
	return(gadget);
}



// 0x56D6F0
CheckBoxGadgetClass *ChatPanelUIBase::Add_Check_Button(int id, const char *bttn_name, const char *text)
{
	FAIL_IF(!bttn_name || !*bttn_name) return(NULL);

	RefCountPtr<RenderObjClass> robj = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name(bttn_name));
	FAIL_IF(!robj) return(NULL);
	Unknown_496AE0(robj.Peek(), false);
	CheckBoxGadgetClass *gadget = new CheckBoxGadgetClass(robj.Peek(), 0x55);
	Unknown64.Unknown_6870E0(gadget, id, View, true, false);
	gadget->Unknown_65FFA0(-1);
	if (text != NULL) {
		FontSys *style = Unknown_BE67C4->Get_Style(text, InterfaceMesh.Peek());
		gadget->Unknown_98(style);
	}
	return(gadget);
}

// 0x56D8D0
GadgetSlider *ChatPanelUIBase::Add_Slider(int id, bool unknown)
{
	GadgetSlider *gadget = new GadgetSlider(unknown, 0);
	Unknown64.Unknown_6870E0(gadget, id, View, true, false);
	return(gadget);
}



// 0x56D970
UIBackGadget *ChatPanelUIBase::Add_Back_Gadget(bool movable, const Vector3 &offset)
{
	FAIL_IF(MovableBackGadget) return(NULL);
	UIBackGadget *gadget;
	if (movable) {
		gadget = MovableBackGadget = new MovableUIBackGadget(InterfaceMesh, offset);
		if (MovableBackGadget) {
			GadgetClass::Get_Signal_Map().connect((GadgetClass *)MovableBackGadget, this, &ChatPanelUIBase::On_Back_Gadget_Moved);
		}
	} else {
		gadget = new UIBackGadget(InterfaceMesh);
	}
	Unknown64.Unknown_6870E0(gadget, 0, View, true, false);
	gadget->Unknown_65FFA0(1000);
	return(gadget);
}



// 0x56DB20
bool ChatPanelUIBase::Unknown_98(const char *name, const char *ini)
{
	InterfaceMesh = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj(name));
	FAIL_IF(!InterfaceMesh) return(false);
	if (InterfaceMesh->Get_Container() == NULL && InterfaceMesh->Peek_Scene() == NULL) {
		Unknown58.Unknown_686CC0(InterfaceMesh.Peek(), NULL);
	}
	if (ini != NULL) {
		IniName = ini;
		Unknown_BE67BC->Load(ini);
		Unknown_BE67C4->Add(InterfaceMesh.Peek());
	}
	return(true);
}



// 0x56DD80
void ChatPanelUIBase::Unknown_88(const RefCountPtr<RenderObjClass> &robj, const char *bone_name)
{
	FAIL_IF(!robj) return;
	FAIL_IF(!bone_name || !*bone_name) return;
	Unknown58.Unknown_686DD0(InterfaceMesh.Peek());
	InterfaceMesh->Remove();
	int bone_idx = robj->Add_Sub_Object_To_Bone(InterfaceMesh.Peek(), bone_name);






	G_ASSERT(bone_idx);
}
// 0x56DE60
void ChatPanelUIBase::Unknown_8C(const RefCountPtr<RenderObjClass> &robj)
{
	FAIL_IF(robj.Peek() != InterfaceMesh->Get_Container()) return;
	InterfaceMesh->Remove();
	Unknown58.Unknown_686CC0(InterfaceMesh.Peek(), NULL);
}

// 0x56DEE0
void ChatPanelUIBase::On_Back_Gadget_Moved(DestroyGadgetEvent *event, GadgetClass *sender)
{
	if (sender == (GadgetClass *)MovableBackGadget) {
		Unknown_9C();
	}
}
/*
** Reconstruction notes (WE1, 2026-11-06; L13, 2026-11-11)
**
** The base of the chat panel, the chat window and every window the chat panel opens (33 constructors call
** 0x56D280): InterfaceClass and the window's model InterfaceMesh at +0x84, the name of its font .ini at +0x88,
** Closed at +0x98 (a window sets it when it is done; the chat panel releases it) and the movable back gadget at
** +0x9C. A window's Unknown_98 override loads the model, then makes its gadgets from the model's sub-objects by name
** (0x56D510 a button, 0x56D6F0 a check button, 0x56D8D0 a slider) and its back gadget (0x56D970).
** The FAIL_IFs fix the lines: 94 / 97 (0x56D510), 113 / 116 (0x56D6F0), 141 (0x56D970), 162 (0x56DB20).
** 0x56DEE0 handles MovableUIBackGadgetMoved (the client's RTTI 0xB7D5E8 names the callback
** MemberCallback<MovableUIBackGadget, ChatPanelUIBase, ..., MovableUIBackGadgetMoved>); the tree's parameter types
** (DestroyGadgetEvent, GadgetClass) are an earlier guess that compiles to the same code: retyping them renames the
** callback's nine short functions 0x56DF10-0x56E130, which symbols.json holds under the old names.
*/
