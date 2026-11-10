// Earth & Beyond client (reconstruction): app\client\ui_pda_cargo.cpp, the PDA's cargo window (the client's __FILE__
// 'e:\build\beta\G\app\Client\ui_pda_cargo.cpp', 0x5CF9B0-0x5D1040 by its asserts and its table 0xAF4AD4). Notes at the end.
#define __INTRFACE_MEMBERS	// intrface.h's members (+0x58's list, +0x64's gadgets)
#include <stdio.h>
#include <string>
#include "pdaint.h"			// (before sclient.h: its PDAInterfaceClass)
#include "ui_pda_cargo.h"
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
#include "local_interface_sort.h"
#include "..\packets\pkinventorymove.h"
#include "..\audio\soundlib.h"
#include "..\shared\money.h"
#include "..\shared\auxdata\auxdata.h"
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

// 0xB83820: the buttons, gadgets 13..17 (tooltips from section 'UI_PDA_CARGO'; 'BUTTON03' twice: sort and reverse)
static const char *CargoButtonNames[UIPdaCargoClass::NUM_BUTTONS] = {	// NAME 1: neutral (D10)
	"BUTTON01",
	"BUTTON02",
	"BUTTON03",
	"BUTTON03",
	"EXIT"
};

// 0xB83834: the filter's items
static const char *CargoFilterNames[] = {	// NAME 1: neutral (D10)
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
// 0x5CF9B0
UIPdaCargoClass::UIPdaCargoClass(SClient *client, PDAInterfaceClass *pda) :
	UIInvClass(client, 10),
	PDA(pda),
	Slider(NULL),
	SortIndex(0),
	SortReverse(false)
{
	for (int i = 0; i < NUM_BUTTONS; i++) {
		Buttons5[i] = NULL;
	}
	if (OptionClass::Instance) {
		SortIndex = OptionClass::Instance->Get_Bool("Inv_Sort_Cargo");
		SortReverse = OptionClass::Instance->Get_Bool("Inv_Sort_Cargo_Reverse");
		FilterName = OptionClass::Instance->Get_String("Inv_Filter_Cargo");
	}
	Unknown_BE67BC->Load("fa_pdainv.ini");
}


// 0x5CFCE0
UIPdaCargoClass::~UIPdaCargoClass()
{
	Unknown_BE67C4->Remove(InterfaceMesh.Peek());
	Unknown_BE67BC->Unload("fa_pdainv.ini");
	Slider = NULL;
}


















// NOT EXACT: functional from the pseudocode (D64)
// 0x5CFE40
void UIPdaCargoClass::Unknown_54()
{
	UIInvClass::Unknown_54();
	InterfaceMesh = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj("TSI04A"));
	FAIL_IF(!InterfaceMesh) return;
	Unknown_BE67C4->Add(InterfaceMesh.Peek());
	Unknown58.Unknown_686CC0(InterfaceMesh.Peek(), NULL);
	bool made = Unknown_C4(InterfaceMesh.Peek(), "TSI04A.C%02d", 12);
	SlotDataGadgetClass *first_bttn = made ? Buttons[0] : NULL;






	FAIL_IF(!first_bttn) {
		InterfaceMesh = NULL;
		return;
	}
	int first = ((GadgetClass *)first_bttn)->Unknown0C;
	for (int i = 0; i < NUM_BUTTONS; i++) {
		int id = i + 13;
		RenderObjClass *robj = InterfaceMesh->Get_Sub_Object_By_Name(CargoButtonNames[i]);
		Unknown_496AE0(robj, 0);
		if (id == 15) {
			Buttons5[i] = new ButtonGadgetClass(robj, 5, 3);
		} else if (id == 16) {
			Buttons5[i] = new ButtonGadgetClass(robj, 0x50, 3);
		} else {
			Buttons5[i] = new ButtonGadgetClass(robj, 0x55, 3);
		}
		GadgetClass::Set_Tooltip(Buttons5[i], "UI_PDA_CARGO", CargoButtonNames[i]);
		Buttons5[i]->Unknown_65FFA0(first);
		robj->Release_Ref();
		Unknown64.Unknown_6870E0(Buttons5[i], id, View, 1, 0);
	}

	Filter = Create_NEW(new UIComboBoxClass(this, Client->As_Space_Client(), InterfaceMesh.Peek(), "filter", 0x15, 0x14, 0x19, 0xC, "Combo01"));
	Filter->Unknown_5417D0(this);
	Filter->Unknown_54();
	Filter->Unknown_57B1D0(CargoFilterNames);
	Filter->Unknown_57AD50(FilterName.c_str());
	Filter->Unknown_57B570(true);
	Filter->Unknown_57B480("Filter");
	Unknown_62F620(FilterName.c_str());

	FontSys *font = Unknown_BE67C4->Get_Style("Vault_inv", InterfaceMesh.Peek());






	G_ASSERT(font);
	Buttons5[0]->Unknown_98(font);

	Slider = new Unknown669FB0Class(this, NULL);
	Slider->Unknown_65FFA0(first);
	Slider->Unknown_66E460(InterfaceMesh, true, 0);
	Slider->Unknown_66A300(4, 3);
	Unknown64.Unknown_6870E0(Slider, 0x13, View, 1, 0);
	Unknown_62F4D0(InterfaceMesh, 0xFFF);
}



// NOT EXACT: functional from the pseudocode (D64)
// 0x5D03E0
void UIPdaCargoClass::Unknown_58()
{
	FAIL_IF(!InterfaceMesh) return;
	FAIL_IF(!Get_Client()) return;
	SClient *sc = Client->As_Space_Client();
	FAIL_IF(!sc) return;
	if (!Active) {
		InterfaceSortScopeClass scope(this);
		float value = Slider->Value;
		SlotDataInterface::Unknown_58();
		Slider->Unknown_A4(value, false);
		bool here = View == sc->Unknown135C;
		Buttons5[0]->Unknown_40(here);
		Buttons5[0]->Unknown_8C(!here);
		Buttons5[4]->Unknown_40(true);
		Unknown_5D0A50(true);
		Filter->Unknown_58();
		((GadgetListManagerClass *)Client->As_Space_Client()->Unknown1278)->Unknown_661610(this);
	}
}


// 0x5D05E0
void UIPdaCargoClass::Unknown_5C()
{
	if (Active) {
		if (OptionClass::Instance) {
			OptionClass::Instance->Save();
		}
		Filter->Unknown_5C();
		UIInvClass::Unknown_5C();
	}
}

// NOT EXACT: functional from the pseudocode (D64)
// 0x5D0620
bool UIPdaCargoClass::Unknown_68(int key)
{
	switch (key) {
		case 13:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			PDA->Unknown_695D70(3);
			return(true);
		case 14:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			PDA->Unknown_695D70(1);
			return(true);
		case 15:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_5D0D20(false);
			Buttons5[2]->Unknown_6607D0();
			return(true);
		case 16:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_5D0D20(true);
			Buttons5[3]->Unknown_6607D0();
			return(true);
		case 17:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			PDA->Unknown_695A00(0, true);
			return(true);
		case 20:
			SoundLibraryClass::Play_2D_Sound("TextClick03.wav", 2);
			return(true);
		case 21:
			break;
		default:
			return(UIInvClass::Unknown_68(key));
	}
	SoundLibraryClass::Play_2D_Sound("TextClick03.wav", 2);
	FilterName = Filter->Unknown_Item_Text(Filter->Selected);
	if (FilterName.size() == 0) {
		return(true);
	}
	StringOptionStruct *option = OptionClass::Instance->Find_String("Inv_Filter_Cargo");
	if (option) {
		option->Value = FilterName.c_str();
	}
	Unknown_62F620(FilterName.c_str());
	Unknown_B0();
	return(true);
}


// 0x5D09B0
void UIPdaCargoClass::Unknown_60()
{
	Unknown_5D0A50(false);
	Filter->Unknown_60();
	UIInvClass::Unknown_60();
}


// 0x5D09E0
void UIPdaCargoClass::Unknown_7C(UIViewClass *view, bool arg)
{
	if (View != view) {
		UIInvClass::Unknown_7C(view, arg);
		if (Filter) {
			Filter->Unknown_7C(view, arg);
		}
	}
}


// 0x5D0A20
void UIPdaCargoClass::Unknown_5D0A20(bool on)
{
	UnknownB8_0 = on;
}

// NOT EXACT: functional from the pseudocode (D64): ui_attrb.cpp's 0x547C20 for this window's 'Credits'
// 0x5D0A50
void UIPdaCargoClass::Unknown_5D0A50(bool all)
{
	DeltaCompoundClass *aux = (DeltaCompoundClass *)Client->As_Space_Client()->AuxData;
	NameClass name("Money");
	DeltaTemplateClass<MoneyClass> *field = NULL;
	if (all) {
		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
	} else {
		if (!aux) return;
		field = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(aux, name);
		if (!field || field->Unknown70 < 2) return;
	}
	FontSys *font = Unknown_BE67C4->Get_Style("Credits", InterfaceMesh.Peek());
	if (font) {
		MoneyClass money;
		if (field) {
			money = field->Value;
		}
		std::string text = "Credits: ";
		text += money.Unknown_4F00C0(true);
		font->Change_Text(text.c_str(), 0);
	}
}

// NOT EXACT: functional from the pseudocode (D64)
// 0x5D0D20
void UIPdaCargoClass::Unknown_5D0D20(bool reverse)
{
	if (!OptionClass::Instance) return;
	if (!reverse) {
		SortIndex = (SortIndex + 1) % 3;
	} else {
		SortReverse = !SortReverse;
	}
	char buffer[0x40];
	_snprintf(buffer, sizeof(buffer), "Inv_Sort_%d_Name", SortIndex);
	std::string sort_name = OptionClass::Instance->Get_String(buffer);
	_snprintf(buffer, sizeof(buffer), "Inv_Sort_%d_primary", SortIndex);
	int sort1 = OptionClass::Instance->Get_Int(buffer);
	_snprintf(buffer, sizeof(buffer), "Inv_Sort_%d_secondary", SortIndex);
	int sort2 = OptionClass::Instance->Get_Int(buffer);
	_snprintf(buffer, sizeof(buffer), "Inv_Sort_%d_tertiary", SortIndex);
	int sort3 = OptionClass::Instance->Get_Int(buffer);
	_snprintf(buffer, sizeof(buffer), SortReverse ? "Reverse Sorting Cargo by %s" : "Sorting Cargo by %s", sort_name.c_str());
	Client->As_Space_Client()->Unknown_74D990(0, buffer, 0);
	((ClientClass *)Client)->Send_Packet(&InventorySortPacketClass(((ClientClass *)Client)->AvatarID, (InventoryEnum)1, sort1, sort2, sort3, SortReverse));
	if (!reverse) {
		IntOptionStruct *option = OptionClass::Instance->Find_Int("Inv_Sort_Cargo");
		if (option) {
			option->Value = SortIndex;
		}
	} else {
		BoolOptionStruct *option = OptionClass::Instance->Find_Bool("Inv_Sort_Cargo_Reverse");
		if (option) {
			option->Value = SortReverse;
		}
	}
}

/*
**	Reconstruction notes (PW1, 2026-11-09)
**
**	Every function here is functional from Ghidra's pseudocode (D64), none matched yet; the asserts' lines (118, 129,
**	167, 184, 185, 187) are not laid out. The constructor reads 'Inv_Sort_Cargo' through Get_Bool (0x414FB0, its result
**	masked to a byte) though 0x5D0D20 keeps it as an index 0..2 and writes it back through Find_Int: the client's.
**	The filter is the combo box's second constructor 0x5789E0 (ui_combobox.cpp, not rebuilt: the POC's stand-in) and
**	the slider gd_inv_slider.cpp's 0x669FB0 (not rebuilt: the POC's stand-in).
*/
