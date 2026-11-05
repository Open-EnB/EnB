// Earth & Beyond client (reconstruction): app\client\ui_cinfo.cpp, the character info window (the client's file per its
// asserts, 'e:\build\beta\G\app\Client\ui_cinfo.cpp'). Notes at the end of the file.
#include "sclient.h"
#include "rendobj.h"
#include "..\..\wwlib\ref_ptr.h"
#include "gadget.h"
#include "unknown_675120.h"
#include "fontmanager.h"
#include "..\shared\unknown_debug.h"
#include "..\audio\soundlib.h"
#include "ui_bio.h"
#include "ui_faction.h"
#include "ui_missn.h"
#include "ui_cinfo.h"

extern FontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4

// What InterfaceClass's +0x40 points to, as far as this file calls it (as ui_avatar_cust.cpp declares it)
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (as ui_manu.cpp's)  // NAME 3: the FAIL_IF's in ui_manu.cpp
};

// The window's first three tabs, as far as this file makes them (their files are not known)
class Unknown549640Class : public Unknown5BB320Class	// NAME 1: neutral (D10)
{
public:
	Unknown549640Class(SClient *client);				// 0x549640 (0x124 bytes)  // NAME 1 [client]: as UIBioClass's
	char UnknownAC[0x124 - 0xAC];						// +0xAC
};











#include "unknown_674260.h"		// the gadget 0x674260 makes (0x60 bytes)






static const char *ButtonNames[UICinfoClass::MAX_BUTTONS] = { "not used", "TSI15A.BUTTON00", "TSI15A.BUTTON01", "TSI15A.BUTTON02", "TSI15A.EXIT" };	// 0xB7DA4C  // NAME 1: neutral (D10)
static const char *MenuNames[] = { "ATTRIBUTES_MENU", "MISSIONS_MENU", "FACTION_MENU", NULL };	// 0xB7DA60 (the tooltips of buttons 1..3 and the menu fonts)  // NAME 1: neutral (D10)

// 0x5734D0
UICinfoClass::UICinfoClass(SClient *client) :
	Unknown5BB320Class(new UIMeshClass("TSI15A", 2), client),
	UnknownAC(0),
	UnknownB0(1)
{
	for (int i = 1; i < MAX_BUTTONS; i++) {
		ButtonGadgetClass *button = new ButtonGadgetClass(Mesh, ButtonNames[i], 0x55, 3);
		UnknownC4[i] = button;
		Unknown64.Unknown_6870E0(button, i, View, 1, 0);
		Unknown_5BB4F0(button);
		if (i == 4) {
			GadgetClass::Set_Tooltip(button, "UI_CINFO", "EXIT");
		} else {
			GadgetClass::Set_Tooltip(button, "UI_CINFO", MenuNames[i - 1]);
		}
	}
	GadgetClass *gadget = new Unknown_674260Class(Create_Peek(Mesh->Peek_Object()));
	Unknown64.Unknown_6870E0(gadget, 6, View, 1, 0);
	Unknown_BE67BC->Load("fa_cinfo.ini");
}

// 0x573770
UICinfoClass::~UICinfoClass()
{
	Unknown_BE67BC->Unload("fa_cinfo.ini");
}

// 0x5738B0
void UICinfoClass::Unknown_54()
{
	InterfaceClass::Unknown_54();
	UnknownB4 = Create_NEW(new Unknown549640Class(Client->As_Space_Client()));
	UnknownB4->Unknown_5417D0(this);
	UnknownB4->Unknown_54();
	UnknownB8 = Create_NEW(new UIMissnClass(Client->As_Space_Client()));
	UnknownB8->Unknown_5417D0(this);
	UnknownB8->Unknown_54();
	UnknownBC = Create_NEW(new UIFactionClass(Client->As_Space_Client()));
	UnknownBC->Unknown_5417D0(this);
	UnknownBC->Unknown_54();
	UnknownC0 = Create_NEW(new UIBioClass(Client->As_Space_Client()));
	UnknownC0->Unknown_5417D0(this);
	UnknownC0->Unknown_54();
}

// 0x573C70
void UICinfoClass::Unknown_7C(UIViewClass *view, bool arg)
{
	Unknown5BB320Class::Unknown_7C(view, arg);
	UnknownB4->Unknown_7C(view, false);
	UnknownB8->Unknown_7C(view, false);
	UnknownBC->Unknown_7C(view, false);
	UnknownC0->Unknown_7C(view, false);
}

// 0x573CE0
void UICinfoClass::Unknown_58()
{
	if (!Active) {
		Unknown5BB320Class::Unknown_58();
		Unknown_BE67C4->Add(Mesh->Unknown1C);
		Unknown_573E80(UnknownB0);
		UnknownC4[4]->Unknown_40(true);
	}
}

// 0x573D50
void UICinfoClass::Unknown_5C()
{
	if (Active) {
		UnknownB0 = UnknownAC;
		Unknown_573E80(0);
		Unknown_BE67C4->Remove(Mesh->Unknown1C);
		Unknown5BB320Class::Unknown_5C();
	}
}

// 0x573DA0
void UICinfoClass::Unknown_60()
{
	Unknown5BB320Class::Unknown_60();
}

// 0x573DC0
bool UICinfoClass::Unknown_68(int key)
{
	switch (key) {
	case 1:
	case 2:
	case 3:
		SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
		Unknown_573E80(key);
		return(true);
	case 4:
		SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
		{
			SClient *client = Client->As_Space_Client();
			client->PDAInterface->Unknown_695A00(2, 1);
		}
		return(true);
	case 6:
		return(true);
	}
	return(false);
}






























































// 0x573E80
// NOT EXACT: register (the menu fonts' Get_Style arguments load in another register order; 0x5741A0's loop inlined three times)
void UICinfoClass::Unknown_573E80(int tab)
{
	if (tab == UnknownAC) {
		return;
	}
	FontSys *font = NULL;
	switch (UnknownAC) {
	case 1:
		UnknownB4->Unknown_5C();
		font = Unknown_BE67C4->Get_Style("ATTRIBUTES_MENU", Mesh->Unknown1C);
		break;
	case 2:
		font = Unknown_BE67C4->Get_Style("MISSIONS_MENU", Mesh->Unknown1C);
		UnknownB8->Unknown_5C();
		break;
	case 3:
		font = Unknown_BE67C4->Get_Style("FACTION_MENU", Mesh->Unknown1C);
		UnknownBC->Unknown_5C();
		break;
	}
	if (font) {
		font->Unknown301 = false;
		font = NULL;
	}
	UnknownAC = tab;
	FAIL_IF(!Get_Client()) return;
	SClient *client = Get_Client()->As_Space_Client();
	switch (UnknownAC) {
	case 1:
		UnknownB4->Unknown_58();
		font = Unknown_BE67C4->Get_Style("ATTRIBUTES_MENU", Mesh->Unknown1C);
		Unknown_5741A0();
		client->Unknown_744560(4);
		break;
	case 2:
		UnknownB8->Unknown_58();
		font = Unknown_BE67C4->Get_Style("MISSIONS_MENU", Mesh->Unknown1C);
		Unknown_5741A0();
		client->Unknown_744560(5);
		break;
	case 3:
		UnknownBC->Unknown_58();
		font = Unknown_BE67C4->Get_Style("FACTION_MENU", Mesh->Unknown1C);
		Unknown_5741A0();
		client->Unknown_744560(6);
		break;
	}
	if (font) {
		font->Unknown301 = true;
	}
}

// 0x5741A0
// NOT EXACT: register (this and the index swap ebx / edi; four loop forms tried)
void UICinfoClass::Unknown_5741A0()
{
	for (int i = 0; i < 4; i++) {
		const char *name = MenuNames[i];
		if (name) {
			FontSys *font = Unknown_BE67C4->Get_Style(name, Mesh->Unknown1C);
			if (font) {
				if (UnknownAC - 1 == i) {
					font->Unknown_6CE7D0(0);
				} else {
					font->Unknown_6CE7D0(1);
				}
			}
		}
	}
}


























// 0x574220
bool UICinfoClass::Unknown_574220(int index)
{
	int tab;
	switch (index) {
	case 0:
		tab = 1;
		break;
	case 1:
		tab = 2;
		break;
	case 2:
		tab = 3;
		break;
	default:
		G_ASSERT(false);
		return(false);
	}
	if (Active) {
		return(tab == UnknownAC);
	}
	return(tab == UnknownB0);
}









// 0x5742B0
void UICinfoClass::Unknown_5742B0(int index)
{
	if (Unknown_574220(index)) {
		return;
	}
	int tab;
	switch (index) {
	case 0:
		tab = 1;
		break;
	case 1:
		tab = 2;
		break;
	case 2:
		tab = 3;
		break;
	default:
		G_ASSERT(false);
		return;
	}
	if (Active) {
		Unknown_573E80(tab);
	} else {
		UnknownB0 = tab;
	}
}

/*
**	Notes (BF1, 2026-11-05)
**
**	The client's object 0x5734D0-0x5745AF: the constructor 0x5734D0 uses 'fa_cinfo.ini' and the 'TSI15A' button names, which
**	come before the __FILE__ string 0xB7DCA4 (first used by 0x573E80's FAIL_IF); ui_chat_window_split.cpp's code ends before
**	it, ui_class.cpp's 0x574710 follows. Vtable 0xAF06E4: the deleting destructor 0x573740, slots +0x54 0x5738B0, +0x58
**	0x573CE0, +0x5C 0x573D50, +0x60 0x573DA0, +0x68 0x573DC0, +0x7C 0x573C70. 0x5741A0 is defined after 0x573E80 and
**	still inlined into it, three times. 0x5742B0 inlines 0x574220 (its G_ASSERT on line 336). After the code the COMDATs:
**	Create_NEW for each tab (0x5743A0, 0x574410, 0x574480, 0x5744F0) and the adjustor thunks 0x574590 / 0x5745A0.
**
**	Forms the client needs: the constructor's tooltip and 0x5741A0's font switch are if / else with a call in each
**	branch (a ?: gives a cmov-like load); key 4 reads the SClient into a local before the PDA call; 0x5742B0 tests
**	Active first.
*/
