// Earth & Beyond client (reconstruction): app\Client\ui_faction_panel.cpp (__FILE__ 0xB7F1F8). Notes at the end of the file.
#include <string>
#include "sclient.h"
#include "rendobj.h"
#include "..\..\wwlib\ref_ptr.h"
#include "gadget.h"
#include "unknown_675120.h"
#include "gd_uiback.h"		// UIBackGadget (0x674260)
#include "fontmanager.h"
#include "..\audio\soundlib.h"
#include "cfaction_database.h"
#include "ui_faction_panel.h"
#include "..\shared\g_assert.h"

extern FontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4



UIFactionPanelClass::UIFactionPanelClass(SClient *client) :
	Unknown5BB320Class(new UIMeshClass("TSI30B", 1), client),
	UnknownBC(0),
	ExitButton(NULL),
	WrapGroup(NULL)
{
	Unknown_BE67BC->Load("fa_tsi30a.ini");
	Unknown_BE67C4->Add(InterfaceMesh->Peek_Mesh_Object());
}

UIFactionPanelClass::~UIFactionPanelClass()
{
	Unknown_BE67C4->Remove(InterfaceMesh->Peek_Mesh_Object());
	Unknown_BE67BC->Unload("fa_tsi30a.ini");
}










































void UIFactionPanelClass::Unknown_54()
{
	ExitButton = new ButtonGadgetClass(InterfaceMesh, "EXIT01", 0x55, 3);
	Unknown64.Unknown_6870E0(ExitButton, 1, View, 1, 0);
	WrapGroup = Unknown_BE67C4->Find(InterfaceMesh->Peek_Mesh_Object());
	G_ASSERT(WrapGroup);
	GadgetClass *gadget = new UIBackGadget(Create_Peek(InterfaceMesh->Peek_Object()));
	Unknown64.Unknown_6870E0(gadget, 2, View, 1, 0);
	Unknown_10();
}

bool UIFactionPanelClass::Unknown_68(int key)
{
	switch (key) {
		case 1:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_5C();
			return(true);
	}
	return(false);
}

void UIFactionPanelClass::Set_Faction(const std::string &faction)
{
	if (WrapGroup) {
		WrapGroup->Unknown_6D0A70("WRAP_NAME_TSI30B", CFactionDatabaseClass::Get_Name(faction).c_str());
		WrapGroup->Unknown_6D0A70("WRAP_DESC_TSI30B", CFactionDatabaseClass::Get_Description(faction).c_str());
	}
	Unknown_58();
}

/*
**	Reconstruction notes (O1, 2026-11-10)
**
**	Client 0x58EB50-0x58F08F (__FILE__ 0xB7F1F8; G_ASSERT 'WrapGroup' on the client's line 81, D25): the constructor
**	0x58EB50 (after ui_faction.cpp's adjustor thunks 0x58EB30 / 0x58EB40), the deleting destructor 0x58EC80, the
**	destructor 0x58ECB0, slot +0x54 0x58EDC0, slot +0x68 0x58EF80, Set_Faction 0x58EFC0 (ui_faction.cpp's 0x58D400
**	family calls it with a faction button's name) and the deleting destructor's adjustor thunk 0x58F080 (-4).
**	The panel keeps the mesh interface base's slots +0x58 / +0x5C / +0x60 / +0x7C (table 0xAF1988).
*/
