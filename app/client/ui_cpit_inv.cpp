// Earth & Beyond client (reconstruction): app\client\ui_cpit_inv.cpp, the cockpit's hulk and harvest inventory windows.
// Notes at the end of the file.
#include "ui_cpit_inv.h"
#include "sclient.h"
#include "client.h"
#include "ui_mesh.h"
#include "gadget.h"
#include "gadget_list_manager.h"
#include "unknown_675120.h"
#include "gd_inv_slider.h"
#include "local_interface_sort.h"
#include "..\shared\g_assert.h"

// What InterfaceClass's +0x40 points to, as far as this file calls it (as pdaint.cpp declares it)
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (as ui_manu.cpp's)  // NAME 3: the FAIL_IF's in ui_manu.cpp
};

// @0x5850E0
UICpitInvClass::UICpitInvClass(UICpitClass *cpit, SClient *sclient, int kind) :
	UIInvClass(sclient, kind),
	Cpit(cpit),
	UIMesh(NULL),
	Unknown0D8(NULL),
	Slider(NULL),
	Unknown0E0(false),
	Unknown0E1(false)
{
}

// @0x585210
UICpitInvClass::~UICpitInvClass()
{
	delete UIMesh;
	UIMesh = NULL;
	Cpit = NULL;
	Unknown0D8 = NULL;
	Slider = NULL;
}

// @0x5852D0
void UICpitInvClass::Unknown_58()
{
	InterfaceSortScopeClass scope(this);
	SlotDataInterface::Unknown_58();


























	FAIL_IF(!View) return;
	UIMesh->Unknown_5B9290((SimpleSceneClass *)View->Unknown_50());
	if (!Unknown0E0) {
		Unknown_585840();
	}
	Unknown0D8->Unknown_40(true);
	((GadgetListManagerClass *)Client->As_Space_Client()->Unknown1278)->Unknown_661610(this);
}

// @0x585440
void UICpitInvClass::Unknown_5C()
{
	Unknown0D8->Unknown_40(false);
	if (Unknown0E0) {
		Unknown_585840();
	}
	UIMesh->Unknown_5B92B0();
	UIInvClass::Unknown_5C();
}

// @0x585500
void UICpitInvClass::Unknown_54()
{
	UIInvClass::Unknown_54();
	UIMesh = new UIMeshClass("PSI09A", 1);
	RefCountPtr<RenderObjClass> robj = Create_Peek(UIMesh->Peek_Object());
	FAIL_IF(!robj) return;
	Unknown_C4(robj.Peek(), "PSI09A.s%02d", 8);
	Unknown0D8 = new ButtonGadgetClass(UIMesh, "X_SPECIALS", 0x55, 3);
	GadgetClass::Set_Tooltip(Unknown0D8, "UI_CPIT", "X_SPECIALS");
	Unknown64.Unknown_6870E0(Unknown0D8, 9, View, 1, 0);
	Slider = new Unknown669FB0Class(this, NULL);
	Slider->Unknown_66E460(robj, true, 0);
	Slider->Unknown_66A300(4, 2);
	Unknown64.Unknown_6870E0(Slider, 10, View, 1, 0);
	Unknown_62F4D0(robj, 0xFFF);
}

// @0x585780
bool UICpitInvClass::Unknown_68(int key)
{
	switch (key) {
		case 9:
			Cpit->Unknown_581CE0(this);
			break;
		case 10:
			break;
		default:
			return(SlotDataInterface::Unknown_68(key));
	}
	return(true);
}

// @0x5857C0
void UICpitInvClass::Unknown_60()
{
	UIMesh->Unknown_5B9410();
	if (Unknown0E1) {
		if (UIMesh->Unknown_5BA320(0, !Unknown0E0)) {
			Slider->Unknown_66FD80(false);
			Unknown0E1 = false;
		}
	}
	UIInvClass::Unknown_60();
}

// @0x585840
void UICpitInvClass::Unknown_585840()
{
	UIMesh->Unknown_5B9930(-1);
	UIMesh->Unknown_5B9830();
	Unknown0E0 = !Unknown0E0;
	if (Unknown0E0) {
		UIMesh->Unknown_5B8EE0(0, 0, "PSI09A.PSI09A-1", false, false, false, 0, -1);
	} else {
		UIMesh->Unknown_5B8EE0(0, 1, "PSI09A.PSI09A-1", false, false, true, 0, -1);
	}
	Unknown0E1 = true;
}

/*
**	Notes (O15, 2026-11-13)
**
**	The client's object 0x5850E0-0x58591A: the constructor 0x5850E0, UIInvClass's inline slots +0x90 0x5851A0 / +0xA8 0x5851C0,
**	the deleting destructor 0x5851E0, the destructor 0x585210, the slots +0x58 0x5852D0 (FAIL_IF l.76), +0x5C 0x585440,
**	+0x54 0x585500 (FAIL_IF l.102), +0x68 0x585780, +0x60 0x5857C0, 0x585840 (inlined into +0x58 / +0x5C, defined after
**	them), the guarded <string> initialiser 0x5858D0 and the adjustor thunks 0x585900 (-4) / 0x585910 (-0x38). The cockpit
**	makes two (ui_cpit.cpp: the hulk window, kind 2, and the harvest window, kind 4). 0x585840's two calls with constant
**	arguments (an if / else) give the client's one call with branched pushes; '!Unknown0E0' as the arguments gives setcc.
*/
