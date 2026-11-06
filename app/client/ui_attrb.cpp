// Earth & Beyond client (reconstruction): app\Client\ui_attrb.cpp, the character info window's skill panel. Notes at the
// end of the file.
#include "sclient.h"
#include "client.h"
#include "rendobj.h"
#include "..\..\wwlib\ref_ptr.h"
#include "gadget.h"
#include "unknown_675120.h"
#include "fontmanager.h"
#include "..\packets\pkskill.h"
#include "..\audio\soundlib.h"
#include "ui_attrb.h"
#include "..\shared\auxdata\permstr.h"	// (its PermStringInitClass static: 0x5495F0)

extern FontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4

#include "unknown_674260.h"		// the gadget 0x674260 makes (0x60 bytes)

// @0x5482B0
Unknown548D60Class::Unknown548D60Class(SClient *client) :
	Unknown5BB320Class(new UIMeshClass("TSI12C", 1), client),
	UnknownAC(0),
	UnknownB0(0),
	UnknownB4(NULL),
	UnknownB8(NULL),
	UnknownBC(NULL)
{
	Unknown_BE67BC->Load("fa_tsi12c.ini");
	Unknown_BE67C4->Add(InterfaceMesh->Peek_Mesh_Object());
}

// @0x548440
Unknown548D60Class::~Unknown548D60Class()
{
	Unknown_BE67C4->Remove(InterfaceMesh->Peek_Mesh_Object());
	Unknown_BE67BC->Unload("fa_tsi12c.ini");
}

// @0x548610
void Unknown548D60Class::Unknown_54()
{
	UnknownB8 = new ButtonGadgetClass(InterfaceMesh, "EXIT", 0x55, 3);
	Unknown64.Unknown_6870E0(UnknownB8, 1, View, 1, 0);
	UnknownB4 = new ButtonGadgetClass(InterfaceMesh, "UPGRADE", 0x55, 3);
	Unknown64.Unknown_6870E0(UnknownB4, 2, View, 1, 0);
	UnknownBC = Unknown_BE67C4->Find(InterfaceMesh->Peek_Mesh_Object());
	UnknownC0 = Create_Peek(Unknown_BE67C4->Get_Style("LVL_ATT", InterfaceMesh->Peek_Mesh_Object()));
	UnknownC4 = Create_Peek(Unknown_BE67C4->Get_Style("NOT_ACQUIRED_ATT", InterfaceMesh->Peek_Mesh_Object()));
	UnknownC8 = Create_Peek(Unknown_BE67C4->Get_Style("NEXT_LVL_ATT", InterfaceMesh->Peek_Mesh_Object()));
	UnknownCC = Create_Peek(Unknown_BE67C4->Get_Style("UPGRADE_BUTTON_ATT", InterfaceMesh->Peek_Mesh_Object()));
	UnknownD0 = Create_Peek(Unknown_BE67C4->Get_Style("POINTS_AVAILABLE_ATT", InterfaceMesh->Peek_Mesh_Object()));
	UnknownD4 = Create_Peek(Unknown_BE67C4->Get_Style("UPGRADE_REQUIRES_ATT", InterfaceMesh->Peek_Mesh_Object()));
	UnknownD8 = Create_Peek(Unknown_BE67C4->Get_Style("UPGRADE_COSTS_ATT", InterfaceMesh->Peek_Mesh_Object()));
	GadgetClass *gadget = new Unknown_674260Class(Create_Peek(InterfaceMesh->Peek_Object()));
	Unknown64.Unknown_6870E0(gadget, 3, View, 1, 0);
	Unknown_10();
}

// @0x548C50
bool Unknown548D60Class::Unknown_68(int key)
{
	switch (key) {
		case 1:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_5C();
			return(true);
		case 2:
			SoundLibraryClass::Play_2D_Sound("skill_train_1.wav", 2);
			{
				int id = ((ClientClass *)Client)->AvatarID;
				((ClientClass *)Client)->Send_Packet(&SkillPacketClass(id, 1, UnknownAC));
			}
			return(true);
	}
	return(false);
}

// @0x5494E0
void Unknown548D60Class::Unknown_5494E0(bool on)
{
	if (on) {
		UnknownB4->Unknown_40(true);
		UnknownB4->Unknown_8C(false);
		UnknownCC->Set_Hidden(false);
		UnknownD4->Set_Hidden(true);
		return;
	}
	UnknownB4->Unknown_40(false);
	UnknownB4->Unknown_8C(true);
	UnknownCC->Set_Hidden(true);
	UnknownD4->Set_Hidden(false);
}

/*
**	Notes (AT2, 2026-11-06)
**
**	The client's object ui_attrb.obj runs 0x546A80-0x54962F (after ui_analyze.obj, before ui_attrb_skills.obj 0x549640):
**	two classes. First the attributes panel at ui_attrb_skills.cpp's +0xC0 (constructor 0x546A80, 0xB0 bytes; its
**	G_ASSERTs 'avatar' at lines 237 / 382, 0x547570 / 0x547E40), not in this file yet: when it comes, its functions go
**	above these (the panel's have no asserts, so their lines are free). Then the skill panel at ui_attrb_skills.cpp's
**	+0xFC (Unknown548D60Class): the constructor 0x5482B0, the deleting destructor 0x548410, the destructor 0x548440, slots
**	+0x54 0x548610 and +0x68 0x548C50, 0x5494E0 exact; 0x548D60 (the selected skill's texts, called by ui_attrb_skills.cpp)
**	not yet. pkskill.h's inline ~SkillPacketClass 0x548D40 is emitted here between them. After the slots: the adjustor
**	thunks 0x5495D0 (-4) / 0x5495E0 (-0x38) of the deleting destructor and permstr.h's PermStringInitClass static
**	(0x5495A0 here; ui_attrb_skills.cpp's notes name 0x5495F0 / 0x549620).
*/
