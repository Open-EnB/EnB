// Earth & Beyond client (reconstruction): app\client\ui_vault.cpp, the PDA's vault window (the client's __FILE__
// 'e:\build\beta\G\app\Client\ui_vault.cpp', 0x656FF0-0x6580B0 by its asserts and its table 0xAF92F0). Notes at the end.
#define __INTRFACE_MEMBERS	// intrface.h's members (+0x58's list, +0x64's gadgets)
#include <stdio.h>
#include <string>
#include "ui_vault.h"
#include "sclient.h"
#include "client.h"
#include "option.h"
#include "rendobj.h"
#include "assetmgr.h"
#include "gadget.h"
#include "gadget_list_manager.h"
#include "unknown_675120.h"
#include "gd_slot_data.h"
#include "gd_inv_slider.h"
#include "ui_combobox.h"
#include "fontdatabase.h"
#include "fontmanager.h"
#include "..\packets\pkinventorymove.h"
#include "..\audio\soundlib.h"
#include "..\shared\g_assert.h"

void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl)  // NAME 1 [robj, make_geometry_unique]: as eff_o.cpp's
extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)

// What InterfaceClass's +0x40 points to, as far as this file calls it (as pdaint.cpp declares it)
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (as ui_manu.cpp's)  // NAME 3: the FAIL_IF's in ui_manu.cpp
};

// 0xB8C2EC: the buttons, gadgets 37..39 (tooltips from section 'UI_VAULT'; 'BUTTON01' twice: sort and reverse)
static const char *VaultButtonNames[UIVaultClass::NUM_BUTTONS] = {	// NAME 1: neutral (D10)
	"EXIT",
	"BUTTON01",
	"BUTTON01"
};

// 0xB8C2F8: the filter's items (the cargo window's list)
static const char *VaultFilterNames[] = {	// NAME 1: neutral (D10)
	"All",
	"Beams",
	"Missile Launchers",
	"Projectile Launchers",
	"Ammo",
	"Shields",
	"Reactors",
	"Engines",
	"Devices",
	"Components",
	"Resources",
	"Trade Goods",
	NULL
};

// NOT EXACT: functional from the pseudocode (D64)
// 0x656FF0
UIVaultClass::UIVaultClass(SClient *client) :
	UIInvClass(client, 17),
	SortIndex(0),
	SortReverse(false), FilterName("")
{
	for (int i = 0; i < NUM_BUTTONS; i++) {
		Buttons3[i] = NULL;
	}
	OptionClass *options = OptionClass::Instance; if (options) {
		SortIndex = options->Get_Bool("Inv_Sort_Vault");
		SortReverse = options->Get_Bool("Inv_Sort_Vault_Reverse");
		FilterName = options->Get_String("Inv_Filter_Vault");
	}
	Unknown_BE67BC->Load("fa_vault00a.ini");
}


// 0x6572F0
UIVaultClass::~UIVaultClass()
{
	Unknown_BE67C4->Remove(InterfaceMesh.Peek());
	Unknown_BE67BC->Unload("fa_vault00a.ini");
}
























// NOT EXACT: functional from the pseudocode (D64)
// 0x657430
void UIVaultClass::Unknown_54()
{
	UIInvClass::Unknown_54();
	InterfaceMesh = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj("VAULT00"));
	FAIL_IF(!InterfaceMesh) return;
	Unknown_BE67C4->Add(InterfaceMesh.Peek());
	Unknown58.Unknown_686CC0(InterfaceMesh.Peek(), NULL);
	bool made = Unknown_C4(InterfaceMesh.Peek(), "VAULT00.C%02d", 36);
	SlotDataGadgetClass *first_bttn = made ? Buttons[0] : NULL;






	FAIL_IF(!first_bttn) {
		InterfaceMesh = NULL;
		return;
	}
	int first = ((GadgetClass *)first_bttn)->Unknown0C;
	for (int i = 0; i < NUM_BUTTONS; i++) {
		int id = i + 37;
		RenderObjClass *robj = InterfaceMesh->Get_Sub_Object_By_Name(VaultButtonNames[i]);
		Unknown_496AE0(robj, 0);
		if (id == 38) {
			Buttons3[i] = new ButtonGadgetClass(robj, 5, 3);
		} else if (id == 39) {
			Buttons3[i] = new ButtonGadgetClass(robj, 0x50, 3);
		} else {
			Buttons3[i] = new ButtonGadgetClass(robj, 0x55, 3);
		}
		GadgetClass::Set_Tooltip(Buttons3[i], "UI_VAULT", VaultButtonNames[i]);
		Buttons3[i]->Unknown_65FFA0(first);
		robj->Release_Ref();
		Unknown64.Unknown_6870E0(Buttons3[i], id, View, 1, 0);
	}
	Filter = Create_NEW(new UIComboBoxClass(this, Client->As_Space_Client(), InterfaceMesh.Peek(), "filter", 0x2B, 0x2A, 0x19, 0xC, "Combo01"));
	Filter->Unknown_5417D0(this);
	Filter->Unknown_54();
	Filter->Add_Items(VaultFilterNames);
	Filter->Select_Text(FilterName.c_str());
	Filter->Hide_Arrow(true);
	Filter->Set_Label("Filter");
	Unknown_62F620(FilterName.c_str());
	Unknown669FB0Class *slider = new Unknown669FB0Class(this, NULL);
	slider->Unknown_65FFA0(first);
	slider->Unknown_66E460(InterfaceMesh, false, 0);
	slider->Unknown_66A300(6, 6);
	Unknown64.Unknown_6870E0(slider, 0x29, View, 1, 0);
	Unknown_62F4D0(InterfaceMesh, 0xFFF);
}











// 0x657970
void UIVaultClass::Unknown_58()
{
	FAIL_IF(!InterfaceMesh) return;
	if (!Active) {
		UIInvClass::Unknown_58();
		Filter->Unknown_58();
		((GadgetListManagerClass *)Client->As_Space_Client()->Unknown1278)->Unknown_661610(this);
	}
}

// 0x6579F0
void UIVaultClass::Unknown_5C()
{
	if (Active) {
		if (OptionClass::Instance) {
			OptionClass::Instance->Save();
		}
		Filter->Unknown_5C();
		UIInvClass::Unknown_5C();
	}
}

// 0x657A30
void UIVaultClass::Unknown_60()
{
	Filter->Unknown_60();
	UIInvClass::Unknown_60();
}

// 0x657A60
void UIVaultClass::Unknown_7C(UIViewClass *view, bool arg)
{
	if (View != view) {
		UIInvClass::Unknown_7C(view, arg);
		if (Filter) {
			Filter->Unknown_7C(view, arg);
		}
	}
}

// NOT EXACT: functional from the pseudocode (D64)
// 0x657AA0
bool UIVaultClass::Unknown_68(int key)
{
	switch (key) {
		case 37:
			Unknown_5C();
			return(true);
		case 38:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_657DA0(false);
			Buttons3[1]->Unknown_6607D0();
			return(true);
		case 39:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_657DA0(true);
			Buttons3[2]->Unknown_6607D0();
			return(true);
		case 42:
			SoundLibraryClass::Play_2D_Sound("TextClick03.wav", 2);
			return(true);
		case 43:
			break;
		default:
			return(UIInvClass::Unknown_68(key));
	}
	SoundLibraryClass::Play_2D_Sound("TextClick03.wav", 2);
	FilterName = Filter->Get_Item_Text(Filter->Selected);
	if (FilterName.size() == 0) {
		return(true);
	}
	StringOptionStruct *option = OptionClass::Instance->Find_String("Inv_Filter_Vault");
	if (option) {
		option->Value = FilterName.c_str();
	}
	Unknown_62F620(FilterName.c_str());
	Unknown_B0();
	return(true);
}

// NOT EXACT: functional from the pseudocode (D64)
// 0x657DA0
void UIVaultClass::Unknown_657DA0(bool reverse)
{
	OptionClass *options = OptionClass::Instance;
	if (options) {
		if (reverse) {
			SortReverse = !SortReverse;
		} else {
			SortIndex = (SortIndex + 1) % 3;
		}
		char buffer[0x40];
		_snprintf(buffer, sizeof(buffer), "Inv_Sort_%d_Name", SortIndex);
		std::string sort_name = options->Get_String(buffer);
		_snprintf(buffer, sizeof(buffer), "Inv_Sort_%d_primary", SortIndex);
		int sort1 = options->Get_Int(buffer);
		_snprintf(buffer, sizeof(buffer), "Inv_Sort_%d_secondary", SortIndex);
		int sort2 = options->Get_Int(buffer);
		_snprintf(buffer, sizeof(buffer), "Inv_Sort_%d_tertiary", SortIndex);
		int sort3 = options->Get_Int(buffer);
		if (SortReverse) {
			_snprintf(buffer, sizeof(buffer), "Reverse Sorting Vault by %s", sort_name.c_str());
		} else {
			_snprintf(buffer, sizeof(buffer), "Sorting Vault by %s", sort_name.c_str());
		}
		Client->As_Space_Client()->Unknown_74D990(0, buffer, 0);
		((ClientClass *)Client)->Send_Packet(&InventorySortPacketClass(((ClientClass *)Client)->AvatarID, (InventoryEnum)3, sort1, sort2, sort3, SortReverse));
		if (reverse) {
			BoolOptionStruct *option = options->Find_Bool("Inv_Sort_Vault_Reverse");
			if (option) {
				option->Value = SortReverse;
			}
		} else {
			IntOptionStruct *option = options->Find_Int("Inv_Sort_Vault");
			if (option) {
				option->Value = SortIndex;
			}
		}
	}
}

/*
**	Reconstruction notes (O15, 2026-11-13)
**
**	The window: ui_treem.cpp's statics end at 0x656F30 (its atexit), the this-adjusting thunks 0x656FD0 / 0x656FE0
**	go to ui_treem.cpp's 0x654D80; this file runs from the constructor 0x656FF0 to the thunks 0x658090 / 0x6580A0 of
**	its own deleting destructor 0x6572C0 (the second and third tables), and ui_verb.cpp's constructor 0x6580B0 opens
**	the next object. The cargo window's sibling (ui_pda_cargo.cpp): the same members from +0xD0 without the PDA, three
**	buttons, a slider made and handed to the gadget list without a member.
**	Not exact (one round each, attempts.csv): the constructor inlines string::_Eos in the Get_String assignment and calls
**	it for FilterName("") (the client: the other way round; inline budget, as the cargo window's 0x5CF9B0); Unknown_54's
**	button loop bumps its two pointers in the other order (as 0x5CFE40); Unknown_68 pushes ebx / ebp / edi at entry
**	where the client pushes them late for the filter's tail (shrink-wrapping; the tail inside its case is the same);
**	0x657DA0 loads the packet's arguments in another order (Client first in the client's).
*/
