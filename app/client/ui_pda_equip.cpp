// Earth & Beyond client (reconstruction): app\client\ui_pda_equip.cpp, the PDA's equipment window (the client's __FILE__
// 'e:\build\beta\G\app\Client\ui_pda_equip.cpp', 0x5D10C0-0x5D2FE1 by its asserts and its table 0xAF4C74). Notes at the end.
#define __INTRFACE_MEMBERS	// intrface.h's members (+0x58's list, +0x64's gadgets)
#define __DELTA_ARRAY_MEMBERS	// delta_array.h's Get_Value
#include <stdio.h>
#include <string>
#include "pdaint.h"			// (before sclient.h: its PDAInterfaceClass)
#include "ui_pda_equip.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "rendobj.h"
#include "assetmgr.h"
#include "gadget.h"
#include "gadget_list_manager.h"
#include "unknown_675120.h"
#include "gd_slot_data.h"
#include "fontdatabase.h"
#include "fontmanager.h"
#include "local_interface_sort.h"
#include "texture.h"
#include "aux_roots.h"
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\g_assert.h"

RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj);	// 0x497300 (cdecl, ro_funcs.cpp's neighbourhood)  // NAME 1 [robj]: as ui_avatr.cpp's
extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)

// What InterfaceClass's +0x40 points to, as far as this file calls it (as pdaint.cpp declares it)
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (as ui_manu.cpp's)  // NAME 3: the FAIL_IF's in ui_manu.cpp
	virtual void Unknown_1C(); virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual ClientObjectClass *Get_Ship();				// +0x30 (client.h's)  // NAME 2: ClientClass's slot +0x30
};

// NOT EXACT: functional from the pseudocode (D64)
// 0x5D10C0
UIPdaEquipClass::UIPdaEquipClass(SClient *client) :
	UIInvClass(client, 11),
	ShipPip(NULL),
	Stats(NULL),
	Exit(NULL)
{
	for (int i = 0; i < 9; i++) {
		Unknown124[i] = 0;
	}
	Unknown_BE67BC->Load("fa_tsi07a.ini");
}


















































// NOT EXACT: functional from the pseudocode (D64)
// 0x5D1250
void UIPdaEquipClass::Unknown_54()
{
	InterfaceSortScopeClass scope(this);
	UIInvClass::Unknown_54();
	InterfaceMesh = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj("tsi07a"));
	FAIL_IF(!InterfaceMesh) return;
	bool weapons = Unknown_C0(InterfaceMesh, "w%02d", NUM_WEAPONS);
	bool devices = Unknown_C0(InterfaceMesh, "s%02d", NUM_DEVICES);
	bool components = Unknown_C0(InterfaceMesh, "c%02d", NUM_COMPONENTS);
	SlotDataGadgetClass *first = Buttons.size() ? Buttons[0] : NULL;
	if (!weapons || !devices || !components || !first) {
		InterfaceMesh = NULL;
		return;
	}
	char name[16];
	int i;
	for (i = 0; i < NUM_WEAPONS; i++) {
		sprintf(name, "w%02df", i);
		Frames[i] = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name(name));
	}
	for (i = 0; i < NUM_DEVICES; i++) {
		sprintf(name, "s%02df", i);
		Frames[FIRST_SYSTEM_SLOT + i] = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name(name));
	}
	for (i = 0; i < NUM_COMPONENTS; i++) {
		sprintf(name, "c%02df", i);
		Frames[SHIELD_SLOT + i] = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name(name));
	}

	RefCountPtr<RenderObjClass> skills = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name("skills01"));
	Unknown_675120Class *stats = new Unknown_675120Class(skills.Peek(), 0x11);
	stats->Unknown_65FFC0(1, (GadgetClass *)first);
	Unknown64.Unknown_6870E0(stats, 0x10, View, 1, 0);
	stats->Unknown5C_03 = 0;
	GadgetClass::Set_Tooltip(stats, "UI_PDA_EQUIP", "STATS_PANEL");

	RefCountPtr<RenderObjClass> exit = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name("exit"));
	Exit = new ButtonGadgetClass(exit.Peek(), 0x55, 3);
	Exit->Unknown_65FFC0(1, (GadgetClass *)first);
	GadgetClass::Set_Tooltip(Exit, "UI_PDA_EQUIP", "EXIT");
	Unknown64.Unknown_6870E0(Exit, 0x11, View, 1, 1);

	Unknown_62F4D0(InterfaceMesh, 0xFFF);
	RenderObjClass *mesh = InterfaceMesh.Peek();
	Unknown58.Unknown_686DA0(mesh, NULL);
	Unknown_BE67C4->Add(InterfaceMesh);
	Stats = new Unknown6A2ED0Class(Client->As_Space_Client(), InterfaceMesh);
	ShipPip = new Unknown541DE0Class(Client->As_Space_Client());
	Unknown66DA50Class *pip = new Unknown66DA50Class(ShipPip);
	Unknown64.Unknown_6870E0(pip, 0x12, View, 1, 0);
	pip->Unknown_65FFC0(1, (GadgetClass *)first);
}

// NOT EXACT: functional from the pseudocode (D64)
// 0x5D1A00
UIPdaEquipClass::~UIPdaEquipClass()
{
	if (Stats) {
		delete Stats;
	}
	Stats = NULL;
	if (ShipPip) {
		delete ShipPip;
	}
	ShipPip = NULL;
	for (std::vector<UnknownMountSlotStruct *>::iterator it = Mounts.begin(); it != Mounts.end(); it++) {
		delete *it;
	}
	Mounts.erase(Mounts.begin(), Mounts.end());
	for (int i = 0; i < NUM_FRAMES; i++) {
		Frames[i] = NULL;
	}
	if (Exit) {
		GadgetClass::Destroy(Exit);
		Exit = NULL;
	}
	Unknown_BE67C4->Remove(InterfaceMesh.Peek());
	Unknown_BE67BC->Unload("fa_tsi07a.ini");
}


// 0x5D1C60
void UIPdaEquipClass::Unknown_7C(UIViewClass *view, bool arg)
{
	UIInvClass::Unknown_7C(view, arg);
	InterfaceMesh->Get_Sub_Object_By_Name("ship");
}

























// NOT EXACT: functional from the pseudocode (D64)
// 0x5D1CA0
void UIPdaEquipClass::Unknown_58()
{
	FAIL_IF(!InterfaceMesh) return;
	if (Active) return;
	InterfaceSortScopeClass scope(this);
	SlotDataInterface::Unknown_58();
	Stats->Unknown_6A34E0(true);
	Exit->Unknown_40(true);
	RefCountPtr<RenderObjClass> attach = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name("ship"));







	G_ASSERT(attach);
	if (attach) {
		ShipPip->Unknown_7BACD0(attach, false);
		ShipPip->Unknown_7BB080(View, 0x9C4, attach->Get_Sort_Level() + 1);
	}
	if (Client) {
		ClientObjectClass *ship = Client->Get_Ship();
		if (ship) {
			RefCountPtr<FontSys> font = Unknown_BE67C4->Unknown_6D2700("Ship_Name", InterfaceMesh);
			if (font) {
				std::string name = ship->DecalName;
				font->Change_Text(name.c_str(), 0);
			}
		}
		((GadgetListManagerClass *)Client->As_Space_Client()->Unknown1278)->Unknown_661610(this);
	}
}





// 0x5D20E0
void UIPdaEquipClass::Unknown_5C()
{
	FAIL_IF(!InterfaceMesh) return;
	if (Active) { ShipPip->Unknown_7BB110(); UIInvClass::Unknown_5C(); }
}


// 0x5D2150
void UIPdaEquipClass::Unknown_60()
{
	UIInvClass::Unknown_60();
	Stats->Unknown_6A34E0(false);
	Unknown_5D2B90();
}

// NOT EXACT: functional from the pseudocode (D64)
// 0x5D2180
bool UIPdaEquipClass::Unknown_68(int key)
{
	FAIL_IF(!InterfaceMesh) return(false);
	if (key == 0x10) {
		Stats->Unknown_6A3C10();
	} else if (key == 0x11) {
		Client->As_Space_Client()->PDAInterface->Unknown_695A00(1, true);
		return(true);
	} else if (key != 0xFFF) {
		return(UIInvClass::Unknown_68(key));
	}
	return(true);
}






// NOT EXACT: functional from the pseudocode (D64)
// 0x5D2210
void UIPdaEquipClass::Unknown_B0()
{
	FAIL_IF(!InterfaceMesh) return;
	Unknown_5D2C50();
	int i;
	for (i = 0; i < NUM_FRAMES; i++) {
		Unknown_B4(-1, i);
		if (Frames[i]) {
			Frames[i]->Set_Hidden(1);
		}
	}
	int weapon_idx = 0;
	int device_idx = FIRST_SYSTEM_SLOT;
	int weapons = 0;
	int devices = 0;
	int count = Mounts.size();
	for (int m = 0; m < count; m++) {
		UnknownMountSlotStruct *mount = Mounts[m];
		int bttn_idx;
		switch (mount->Type) {
			case 0:
				continue;
			case 4:
				bttn_idx = SHIELD_SLOT;
				break;
			case 0x40:
				bttn_idx = ENGINE_SLOT;
				break;
			case 0x80:
				bttn_idx = POWER_SLOT;
				break;
			default:
				if (mount->Type & 0x1C022) {











					FAIL_IF(weapon_idx > LAST_WEAPON_SLOT) continue;
					bttn_idx = weapon_idx++;
					weapons++;
				} else {
					FAIL_IF(device_idx > LAST_SYSTEM_SLOT) continue;
					bttn_idx = device_idx++;
					devices++;
				}
				break;
		}
		Unknown_B4(mount->Index, bttn_idx);
		if (Frames[bttn_idx]) {
			Frames[bttn_idx]->Set_Hidden(0);
		}
	}

	ClientObjectClass *ship = Client->Get_Ship();
	if (!ship || !ship->AuxData) return;
	DeltaCompoundClass *aux = (DeltaCompoundClass *)ship->AuxData;
	NameClass weapons_name("Inventory.FutureWeapons");
	DeltaTemplateClass<int> *future_weapons = Unknown_Find_Field<DeltaTemplateClass<int> >(aux, weapons_name);
	if (future_weapons && !future_weapons->Unknown70) future_weapons = NULL;

	FAIL_IF(!future_weapons) return;
	NameClass devices_name("Inventory.FutureDevices");
	DeltaTemplateClass<int> *future_devices = Unknown_Find_Field<DeltaTemplateClass<int> >(aux, devices_name);
	if (future_devices && !future_devices->Unknown70) future_devices = NULL;
	FAIL_IF(!future_devices) return;
	int future_weapon_count = future_weapons->Value;
	int future_device_count = future_devices->Value;
	RefCountPtr<TextureClass> blank = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("im_blank.tga", MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true));
	for (i = weapons; i < future_weapon_count; i++, weapon_idx++) {

		FAIL_IF(weapon_idx > LAST_WEAPON_SLOT) break;
		if (Frames[weapon_idx]) {
			Frames[weapon_idx]->Set_Hidden(0);
			Buttons[weapon_idx]->Unknown_90(blank.Peek());
			Buttons[weapon_idx]->Unknown_8C(false);
			GadgetClass::Set_Tooltip(Buttons[weapon_idx], "UI_PDA_EQUIP", "FUTURE_WEAPON");
		}
	}
	for (i = devices; i < future_device_count; i++, device_idx++) {




		FAIL_IF(device_idx > LAST_SYSTEM_SLOT) break;
		if (Frames[device_idx]) {
			Frames[device_idx]->Set_Hidden(0);
			Buttons[device_idx]->Unknown_90(blank.Peek());
			Buttons[device_idx]->Unknown_8C(false);
			GadgetClass::Set_Tooltip(Buttons[device_idx], "UI_PDA_EQUIP", "FUTURE_DEVICE");
		}
	}
}



// NOT EXACT: functional from the pseudocode (D64)
// 0x5D2850
void UIPdaEquipClass::Unknown_A4(std::string &name, int index, int bttn_idx)
{
	if (bttn_idx == SHIELD_SLOT) {
		name = "SHIELD_SLOT";
		return;
	}
	if (bttn_idx == POWER_SLOT) {
		name = "POWER_SLOT";
		return;
	}
	if (bttn_idx == ENGINE_SLOT) {
		name = "ENGINE_SLOT";
		return;
	}
	if (bttn_idx >= 0 && bttn_idx <= LAST_WEAPON_SLOT) {
		name = "WEAPON_SLOT";
	} else {
		G_ASSERT((bttn_idx >= FIRST_SYSTEM_SLOT) && (bttn_idx <= LAST_SYSTEM_SLOT));
		name = "SYSTEM_SLOT";
	}
	char text[16];
	sprintf(text, "_%d", index);
	name += text;
}


// 0x5D2B90
void UIPdaEquipClass::Unknown_5D2B90()
{
	ClientObjectClass *ship = Client->Get_Ship();
	ShipPip->Unknown_542250(Unknown_497300(ship ? ship->RenderObject : NULL));
}









// NOT EXACT: functional from the pseudocode (D64)
// 0x5D2C50
void UIPdaEquipClass::Unknown_5D2C50()
{
	if (Mounts.size()) {
		for (std::vector<UnknownMountSlotStruct *>::iterator it = Mounts.begin(); it != Mounts.end(); it++) {
			delete *it;
		}
		Mounts.erase(Mounts.begin(), Mounts.end());
	}
	FAIL_IF(!Get_Client()) return;
	ClientObjectClass *ship = Client->Get_Ship();
	if (!ship || !ship->RenderObject || !ship->AuxData) return;
	NameClass mounts_name("Inventory.Mounts");
	DeltaArrayClass<UnknownB016DCType> *mounts = Unknown_Find_Field<DeltaArrayClass<UnknownB016DCType> >((DeltaCompoundClass *)ship->AuxData, mounts_name);
	if (!mounts || !mounts->Unknown70) return;
	int count = mounts->Children.size();
	for (int i = 0; i < count; i++) {
		UnknownB016DCType &value = mounts->Get_Value(i);
		UnknownMountSlotStruct *mount = new UnknownMountSlotStruct;
		mount->Type = value.I00;
		mount->Unknown04 = value.I08;
		mount->Unknown08 = value.I04;
		mount->Index = i;
		Mounts.insert(Mounts.end(), 1, mount);
	}
}

/*
**	Reconstruction notes (PW1, 2026-11-09)
**
**	Every function here is functional from Ghidra's pseudocode (D64). 0x5D2EB0, 0x5D2F50, 0x5D2F90 and 0x5D3000 are
**	std::vector<UnknownMountSlotStruct *>'s helpers (Mounts). The PIP (0x541DE0), the stats display (0x6A2ED0) and the
**	PIP's gadget (0x66DA50) are not rebuilt (the POC's stand-ins). 0x5D1F60 is co.h's DecalName copied (the ship's
**	name, emitted here). Slot +0x7C looks up the 'ship' sub-object and drops it (the client's: a reference not
**	released). Slot +0x68's key 0xFFF is the back gadget's (0x62F4D0).
*/
