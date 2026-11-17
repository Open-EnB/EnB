// Earth & Beyond client (reconstruction): unknown_65a020.cpp (PROVISIONAL, D137). Notes at the end of the file.
#include <string.h>
#include "unknown_65a020.h"
#include "sbaseview.h"
#include "gadget.h"
#include "gd_uiback.h"
#include "unknown_675120.h"
#include "rendobj.h"
#include "control_keys_manager.h"
#include "tasks\tsk_login.h"
#include "tasks\tsk_game.h"
#include "..\audio\soundlib.h"
#include "fontmanager.h"	// FontManagerClass (0xBE67C4) and fontdatabase.h's FontStringFilesClass (0xBE67BC)

extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)

// The client at InterfaceClass +0x40, as this file calls it (as ui_jobs.cpp declares it)
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (ClientClass::As_Space_Client; SClient's returns itself)  // NAME 3: ui_manu.cpp's FAIL_IF's
};

void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl)  // NAME 1 [robj, make_geometry_unique]: as ui_manu.cpp's

// The window's buttons (0xB8C58C: slot +0x54's ids 1, 2)
static const char *Unknown_B8C58C[] = { "exit", "exit_button" };	// NAME 1: neutral (D10)

// @0x65A020
Unknown65A020Class::Unknown65A020Class(SClient *client) :
	Unknown5BB320Class(new UIMeshClass("JWEB00", true), client),
	Unknown0B4(0)
{
	memset(Unknown0AC, 0, sizeof(Unknown0AC));
	Unknown_BE67BC->Load("FA_JWEB00.INI");
	Unknown_BE67C4->Add(InterfaceMesh->Peek_Mesh_Object());
}

// @0x65A160
Unknown65A020Class::~Unknown65A020Class()
{
	Unknown_5C();
	Unknown_BE67C4->Remove(InterfaceMesh->Peek_Mesh_Object());
	Unknown_BE67BC->Unload("FA_JWEB00.INI");
}

// @0x65A250
void Unknown65A020Class::Unknown_54()
{
	for (int id = 0; id < 2; id++) {
		RefCountPtr<RenderObjClass> robj = Create_Get(InterfaceMesh->Peek_Object()->Get_Sub_Object_By_Name(Unknown_B8C58C[id]));
		if (robj) {
			Unknown_496AE0(robj.Peek(), 0);
			Unknown0AC[id] = new ButtonGadgetClass(robj.Peek(), 0x55, 3);
			Unknown64.Unknown_6870E0(Unknown0AC[id], id + 1, View, 1, 0);
			Unknown_5BB4F0(Unknown0AC[id]);
		}
	}
	((ControlKeyListClass *)&Unknown4C)->Add(KernelEventClass(0x1B), (void *)1);
	RefCountPtr<RenderObjClass> box = Create_Get(InterfaceMesh->Peek_Object()->Get_Sub_Object_By_Name("Box01"));
	Unknown64.Unknown_6870E0(new UIBackGadget(box), 4, View, 1, 0);
	Unknown_10();
}

// @0x65A4A0
void Unknown65A020Class::Unknown_58()
{
	Client->As_Space_Client()->Mouse->Unknown_5153B0();
	Unknown0B4 = 20;
	Unknown5BB320Class::Unknown_58();
}

// @0x65A4E0
void Unknown65A020Class::Unknown_5C()
{
	if (Unknown_7CA2D0()->Unknown_7CA570(BROWSER_PIT)) {
		Unknown_7CA2D0()->Unknown_7CB240(BROWSER_PIT);
	}
	Unknown5BB320Class::Unknown_5C();
	Client->As_Space_Client()->Mouse->Unknown_515400();
}

// @0x65A530
void Unknown65A020Class::Unknown_60()
{
	if (Unknown0B4 && --Unknown0B4 == 0) {
		RefCountPtr<RenderObjClass> browser = Create_Get(InterfaceMesh->Peek_Object()->Get_Sub_Object_By_Name("BROWSER"));
		Unknown_7CA2D0()->Unknown_7CA5C0(BROWSER_PIT, browser.Peek(), NULL);
	}
	Unknown5BB320Class::Unknown_60();
}

// @0x65A620
bool Unknown65A020Class::Unknown_68(int key)
{
	if (key > 0 && key <= 2) {
		SoundLibraryClass::Play_2D_Sound("INTERFACE_MenuSlide01C.wav", 2);
		((SBaseViewClass *)View->Unknown_30())->Unknown_69F440();
		return(true);
	}
	return(false);
}

/*
**	Reconstruction notes (client 0x65A020-0x65A6C0; PROVISIONAL file, D137).
**
**	* The object: UIVerbClass's destructor 0x65A010 ends ui_verb.cpp's; this one's functions 0x65A020-0x65A657, the
**	  guarded std::ctype<unsigned short>::id initialiser 0x65A670 (the end of an object including <string>), then the
**	  adjustors 0x65A6A0 / 0x65A6B0 of the tables 0xAF96A4 / 0xAF9698; uiview.cpp's UIViewClass constructor
**	  follows at 0x65A6C0. Its data: the button names 0xB8C58C, 'FA_JWEB00.INI' 0xB8C598, 'JWEB00' 0xB8C5A8,
**	  'Box01' 0xB8C5B0. No assert or string names the file; by link order it sorts between ui_verb.cpp and
**	  uiview.cpp (the mesh 'JWEB00' suggests ui_web.cpp, not taken without the client's own name).
**	* The sibling is ui_jobs.cpp's UIJobsClass (mesh 'JNEWS00'): the same slots overridden (+0x54, +0x58, +0x5C,
**	  +0x60, +0x68) over Unknown5BB320Class.
**	* The destructor's body is slot +0x5C's (the browser shut, the base's +0x5C, the mouse released): a virtual
**	  call in a destructor binds statically and inlines.
**	* The browser is the settings store's id 3 (tsk_game.h's BROWSER_PIT; Net-7's URL 'http://www.net-7.org/browser').
*/
