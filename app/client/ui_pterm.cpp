// Earth & Beyond client (reconstruction): app\client\ui_pterm.cpp (PROVISIONAL file name, D137). Notes at the end of the file.
#include <stdio.h>
#include <string.h>
#include "sclient.h"
#include "rendobj.h"
#include "refcount.h"
#include "unknown_675120.h"
#include "fontmanager.h"
#include "local_interface_sort.h"
#include "control_key_list.h"
#include "ui_mesh.h"
#include "..\audio\soundlib.h"
#include "ui_ptmis.h"
#include "ui_ptnew.h"
#include "ui_ptppl.h"
#include "unknown_5de6e0.h"
#include "ui_pterm.h"

extern FontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4

static const char *Unknown_B84C30[UIPTermClass::MAX_BUTTONS] = { "not used", "TLI10A.BUTTON00", "TLI10A.BUTTON01", "TLI10A.BUTTON02", "TLI10A.EXIT" };	// NAME 1: neutral (D10)

// 0x5DDCA0
UIPTermClass::UIPTermClass(UIViewClass *view, LayerClass *layer) :
	Unknown5BB320Class(new UIMeshClass("TLI10A", 2), view, *layer),
	Unknown0AC(0),
	Unknown0D0(NULL),
	Unknown0D4(false)
{
	for (int i = 1; i < MAX_BUTTONS; i++) {
		int flags = (i != 4) ? 3 : 0;
		ButtonGadgetClass *button = new ButtonGadgetClass(InterfaceMesh, Unknown_B84C30[i], Layer, 0x55, flags);
		Unknown0BC[i] = button;
		Unknown64.Unknown_6870E0(button, i, View, 1, 0);
		Unknown_5BB4F0(button);
	}
	((ControlKeyListClass *)&Unknown4C)->Add(KernelEventClass(0x1B), (void *)4);
	Unknown_BE67BC->Load("fa_pterm.ini");
	Unknown_BE67BC->Load("fa_ptmis.ini");
	Unknown_BE67BC->Load("fa_ptppl.ini");
	Unknown_BE67BC->Load("fa_ptnews.ini");
}

// 0x5DDEF0
UIPTermClass::~UIPTermClass()
{
	REF_PTR_RELEASE(Unknown0B4);
	REF_PTR_RELEASE(Unknown0B0);
	REF_PTR_RELEASE(Unknown0B8);
	Unknown_BE67BC->Unload("fa_pterm.ini");
	Unknown_BE67BC->Unload("fa_ptmis.ini");
	Unknown_BE67BC->Unload("fa_ptppl.ini");
	Unknown_BE67BC->Unload("fa_ptnews.ini");
}

// 0x5DE030
void UIPTermClass::Unknown_54()
{
	InterfaceClass::Unknown_54();
	Unknown0B4 = new UIPTNewClass(View, &Layer);
	Unknown0B4->Unknown_54();
	View->Unknown_6B4940(Unknown0B4);
	Unknown0B0 = new UIPTMisClass(View, &Layer);
	Unknown0B0->Unknown_54();
	View->Unknown_6B4940(Unknown0B0);
	Unknown0B8 = new UIPTPplClass(View, &Layer);
	Unknown0B8->Unknown_54();
	View->Unknown_6B4940(Unknown0B8);
}

// 0x5DE1C0
void UIPTermClass::Unknown_7C(UIViewClass *view, bool arg)
{
	InterfaceClass::Unknown_7C(view, arg);
	Unknown0B4->Unknown_7C(view, false);
	Unknown0B0->Unknown_7C(view, false);
	Unknown0B8->Unknown_7C(view, false);
}

// 0x5DE230
void UIPTermClass::Unknown_58()
{
	if (!Active) {
		InterfaceSortScopeClass scope(this);
		Unknown0D4 = false;
		Unknown_BE67C4->Add(InterfaceMesh->Peek_Mesh_Object());
		SoundLibraryClass::Play_2D_Sound("SN_SCIFI_BEEP_6.wav", 2);
		Unknown0D0 = new Unknown5DE6E0Class("icl01a", Layer.Scene);
		Unknown0D0->Unknown_5DE7F0();
		Unknown_5DE500(1);
		InterfaceClass::Unknown_58();
	}
}

// 0x5DE350
void UIPTermClass::Unknown_5C()
{
	if (Active) {
		if (Unknown0AC == 0) {
			InterfaceClass::Unknown_5C();
		} else {
			Unknown_5DE500(0);
			Unknown5BB320Class::Unknown_5C();
		}
		Unknown_BE67C4->Remove(InterfaceMesh->Peek_Mesh_Object());
		SoundLibraryClass::Play_2D_Sound("SN_SCIFI_TEXT_CLICK_2.wav", 2);
		Unknown0D0->Unknown_5DE830();
		delete Unknown0D0;
		Unknown0D0 = NULL;
	}
}

// 0x5DE410
void UIPTermClass::Unknown_60()
{
	Unknown0D0->Unknown_5DE860();
	if (Unknown0AC == 0) {
		Unknown_5DE500(1);
		if (Unknown0AC == 0) return;
		InterfaceClass::Unknown_5C();
		Unknown5BB320Class::Unknown_58();
	}
	Unknown5BB320Class::Unknown_60();
}

// 0x5DE480
bool UIPTermClass::Unknown_68(int key)
{
	switch (key) {
		case 2:
			Unknown_5DE500(2);
			return(true);
		case 1:
			Unknown_5DE500(1);
			return(true);
		case 3:
			Unknown_5DE500(3);
			return(true);
		case 4:
			Unknown0D4 = true;
			return(true);
	}
	return(false);
}

// 0x5DE500
void UIPTermClass::Unknown_5DE500(int page)
{
	if (!Unknown0D0->Unknown_5DE880()) return;
	if (page == Unknown0AC) return;
	FontSys *font = NULL;
	switch (Unknown0AC) {
		case 1:
			font = Unknown_BE67C4->Get_Style("MISSIONS_PTERM", InterfaceMesh->Peek_Mesh_Object());
			Unknown0B0->Unknown_5C();
			break;
		case 2:
			Unknown0B4->Unknown_5C();
			font = Unknown_BE67C4->Get_Style("NEWS_PTERM", InterfaceMesh->Peek_Mesh_Object());
			break;
		case 3:
			font = Unknown_BE67C4->Get_Style("PEOPLE_PTERM", InterfaceMesh->Peek_Mesh_Object());
			Unknown0B8->Unknown_5C();
			break;
	}
	if (font) {
		font->Unknown301 = false;
		font = NULL;
	}
	Unknown0AC = page;
	switch (page) {
		case 1:
			Unknown0B0->Unknown_58();
			font = Unknown_BE67C4->Get_Style("MISSIONS_PTERM", InterfaceMesh->Peek_Mesh_Object());
			break;
		case 2:
			Unknown0B4->Unknown_58();
			font = Unknown_BE67C4->Get_Style("NEWS_PTERM", InterfaceMesh->Peek_Mesh_Object());
			break;
		case 3:
			Unknown0B8->Unknown_58();
			font = Unknown_BE67C4->Get_Style("PEOPLE_PTERM", InterfaceMesh->Peek_Mesh_Object());
			break;
	}
	if (font) {
		font->Unknown301 = true;
	}
}

/*
**	Reconstruction notes (app\client\ui_pterm.cpp, the petition terminal; PROVISIONAL file name, D137: no assert
**	names it; the strings fa_pterm.ini, MISSIONS_PTERM / NEWS_PTERM / PEOPLE_PTERM and the link order between
**	ui_proc.cpp and ui_ptmis.cpp do).
**
**	The object runs from the constructor 0x5DDCA0 (after ui_proc.cpp's thunks 0x5DDC80 / 0x5DDC90) through the
**	guarded <string> initialiser 0x5DE690 and the adjustor thunks 0x5DE6C0 / 0x5DE6D0; the mesh animation it plays
**	(0x5DE6E0-0x5DE88A) is compiled apart, unknown_5de6e0.cpp (none of it is inlined here). Tables 0xAF5FB4 /
**	0xAF5FA8 / 0xAF5F9C over unknown_5bb250.h's base: the destructor, +0x54 (the pages), +0x58 open, +0x5C close,
**	+0x60 every frame, +0x68 the buttons, +0x7C. Nothing in the client makes a UIPTermClass (no reference to the
**	constructor 0x5DDCA0 or its tables outside it).
**
**	Forms: the buttons' table starts at the pooled 'not used' (0xB84C30) and is indexed by id; the buttons' flags
**	are a local computed before the new; slot +0x68 has a return per case, case 2 first (the client's case order in
**	the jump table's targets); 0x5DE500 keeps one style pointer, reset inside the first test; the styles and fonts
**	take the mesh's model through Peek_Mesh_Object() and the animation the layer's Scene directly.
*/
