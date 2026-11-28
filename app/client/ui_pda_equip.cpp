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
#include "gd_uimesh.h"
#include "gd_slot_data.h"
#include "fontdatabase.h"
#include "fontmanager.h"
#include "local_interface_sort.h"
#include "texture.h"
#include "aux_roots.h"
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\g_assert.h"
#include "ship_stats_display.h"
#include "ship_equip_pip.h"
RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj);	// 0x497300 (cdecl, ro_funcs.cpp's neighbourhood)  // NAME 1 [robj]: as ui_avatr.cpp's
extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)
#include "vector2i.h"
static Vector2i Unknown_BE5D28(0x100, 0x100); class LineDisplayClientClass  // NAME 1: neutral (D10); what InterfaceClass's +0x40 points to, as far as this file calls it (as pdaint.cpp declares it); Unknown_BE5D28: 0xBE5D28 (0x5D10A0), as starmap_data.cpp's LargeTextureSize, nothing reads it (NAME 1: neutral, D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SpaceClientClass *As_Space_Client();					// +0x18 (as ui_manu.cpp's)  // NAME 3: the FAIL_IF's in ui_manu.cpp
	virtual void Unknown_1C(); virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual ClientObjectClass *Get_Ship();				// +0x30 (client.h's)  // NAME 2: ClientClass's slot +0x30
};
template<class C> inline C *Unknown_Find_Valid(DeltaContainer *root, const AuxDataName &name) { C *t = Unknown_Find_Field<C>(root, name); if (t && t->Unknown70) return(t); return(NULL); }	// NAME 1: neutral (D10): as co.cpp's
#include "..\shared\auxdata\delta_numbering.h"	// 0x5D32D0 numbers the int field's and the slot array's globals
// @0x5D10C0
UIPdaEquipClass::UIPdaEquipClass(SpaceClientClass *client) :
	UIInvClass(client, 11),
	ShipPip(NULL),
	Unknown124(0), Unknown128(0), Unknown12C(0), Unknown130(0), Unknown134(0), Unknown138(0), Unknown13C(0), Unknown140(0), Unknown144(0),
	Stats(NULL),
	Exit(NULL)
{
	memset(Frames, 0, sizeof(Frames));
	Unknown_BE67BC->Load("fa_tsi07a.ini");
}
#include "..\shared\auth_product.h"



















































// @0x5D1250
void UIPdaEquipClass::Unknown_54()
{
	InterfaceSortScopeClass scope(this);
	UIInvClass::Unknown_54();
	InterfaceMesh = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj("tsi07a"));
	FAIL_IF(!InterfaceMesh) return;
	bool failed = false;
	if (!Unknown_C0(InterfaceMesh, "w%02d", NUM_WEAPONS)) failed = true;
	if (!Unknown_C0(InterfaceMesh, "s%02d", NUM_DEVICES)) failed = true;
	if (!Unknown_C0(InterfaceMesh, "c%02d", NUM_COMPONENTS)) failed = true;
	SlotDataGadget *first = Buttons[0];
	if (failed || !first) {
		InterfaceMesh.Clear();
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
	RefCountPtr<RenderObjClass> robj;
	robj = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name("skills01"));
	UIMeshGadgetClass *stats = new UIMeshGadgetClass(robj.Peek(), 0x11);
	stats->Unknown_65FFC0(1, (GadgetClass *)first);
	Unknown64.Unknown_6870E0(stats, 0x10, View, 1, 0);
	stats->Unknown5C_03 = 0;
	GadgetClass::Set_Tooltip(stats, "UI_PDA_EQUIP", "STATS_PANEL");
	robj = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name("exit"));
	Exit = new ButtonGadgetClass(robj.Peek(), 0x55, 3);
	Exit->Unknown_65FFC0(1, (GadgetClass *)first);
	GadgetClass::Set_Tooltip(Exit, "UI_PDA_EQUIP", "EXIT");
	Unknown64.Unknown_6870E0(Exit, 0x11, View, 1, 1);

	Unknown_62F4D0(InterfaceMesh, 0xFFF);
	Unknown58.Unknown_686DA0(*(RenderObjClass **)&InterfaceMesh, NULL);

	Unknown_BE67C4->Add(InterfaceMesh);
	Stats = new ShipStatsDisplayClass(Client->As_Space_Client(), InterfaceMesh);
	ShipPip = new ShipEquipPipClass(Client->As_Space_Client());
	GadgetShipEquipPipClass *pip = new GadgetShipEquipPipClass(ShipPip);
	Unknown64.Unknown_6870E0(pip, 0x12, View, 1, 0);
	pip->Unknown_65FFC0(1, (GadgetClass *)first);
}

// functional from the pseudocode (D64); 0x5D2EB0 inlined
// @0x5D1A00
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
	Unknown_5D2EB0();
	for (int i = 0; i < NUM_FRAMES; i++) {
		Frames[i] = NULL;
	}
	if (Exit) {
		GadgetClass::Destroy(Exit);
		Exit = NULL;
	}
	Unknown_BE67C4->Unknown_6D2310(InterfaceMesh);
	Unknown_BE67BC->Unload("fa_tsi07a.ini");
}





// @0x5D1C60
void UIPdaEquipClass::Unknown_7C(UIViewClass *view, bool arg)
{
	UIInvClass::Unknown_7C(view, arg);
	InterfaceMesh->Get_Sub_Object_By_Name("ship");
}

























// @0x5D1CA0
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
		UIViewClass *view = View;
		ShipPip->Unknown_7BB080(view, 0x9C4, attach->Get_Sort_Level() + 1);
	}
	if (Client) {
		ClientObjectClass *ship = Client->Get_Ship();
		if (ship) {
			RefCountPtr<FontSys> font = Unknown_BE67C4->Unknown_6D2700("Ship_Name", InterfaceMesh);
			if (font) {
				font->Change_Text(ship->Get_Decal_Name().c_str(), 0);
			}
		}
		((GadgetListManagerClass *)Client->As_Space_Client()->Unknown1278)->Unknown_661610(this);
	}
}





// @0x5D20E0
void UIPdaEquipClass::Unknown_5C()
{
	FAIL_IF(!InterfaceMesh) return;
	if (Active) { ShipPip->Unknown_7BB110(); UIInvClass::Unknown_5C(); }
}


// @0x5D2150
void UIPdaEquipClass::Unknown_60()
{
	UIInvClass::Unknown_60();
	Stats->Unknown_6A34E0(false);
	Unknown_5D2B90();
}


// @0x5D2180
template <class T> static inline void Close_PDA_Of(T *space) { space->PDAWindow->Unknown_695A00(1, true); } bool UIPdaEquipClass::Unknown_68(int key)
{
	FAIL_IF(!InterfaceMesh) return(false);
	switch (key) {
		case 0x10: Stats->Unknown_6A3C10(); break;
		case 0x11:
			Close_PDA_Of(Get_Client()->As_Space_Client());
			return(true);
		case 0xFFF: break;
		default: return(UIInvClass::Unknown_68(key));
	}
	return(true);
}







// @0x5D2210
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
			case 0x40:
				bttn_idx = ENGINE_SLOT;
				break;
			case 0x80:
				bttn_idx = POWER_SLOT;
				break;
			case 4:
				bttn_idx = SHIELD_SLOT;
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
	if (!ship) return;
	DeltaContainer *aux = (DeltaContainer *)ship->AuxData;
	if (!aux) return;
	AuxDataName weapons_name("Inventory.FutureWeapons");
	DeltaTemplateClass<int> *future_weapons = Unknown_Find_Valid<DeltaTemplateClass<int> >(aux, weapons_name);

	FAIL_IF(!future_weapons) return;
	int future_weapon_count = future_weapons->Value;
	AuxDataName devices_name("Inventory.FutureDevices");
	DeltaTemplateClass<int> *future_devices = Unknown_Find_Valid<DeltaTemplateClass<int> >(aux, devices_name);
	FAIL_IF(!future_devices) return;
	int future_device_count = future_devices->Value;

	RefCountPtr<TextureClass> blank = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("im_blank.tga", MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true));
	for (i = weapons; i < future_weapon_count; ) {

		FAIL_IF(weapon_idx > LAST_WEAPON_SLOT) break;
		if (Frames[weapon_idx]) {
			Frames[weapon_idx]->Set_Hidden(0);
			Buttons[weapon_idx]->RenderObjGadgetClass::Unknown_90(blank.Peek());
			Buttons[weapon_idx]->Unknown_8C(false);
			GadgetClass::Set_Tooltip(Buttons[weapon_idx], "UI_PDA_EQUIP", "FUTURE_WEAPON");
		} weapon_idx++; i++;
	}
	for (i = devices; i < future_device_count; ) {




		FAIL_IF(device_idx > LAST_SYSTEM_SLOT) break;
		if (Frames[device_idx]) {
			Frames[device_idx]->Set_Hidden(0);
			Buttons[device_idx]->RenderObjGadgetClass::Unknown_90(blank.Peek());
			Buttons[device_idx]->Unknown_8C(false);
			GadgetClass::Set_Tooltip(Buttons[device_idx], "UI_PDA_EQUIP", "FUTURE_DEVICE");
		} device_idx++; i++;
	}
}




// @0x5D2850
void UIPdaEquipClass::Unknown_A4(std::string &name, int index, int bttn_idx)
{
	switch (bttn_idx) {
		case SHIELD_SLOT:
			name = "SHIELD_SLOT";
			break;
		case POWER_SLOT:
			name = "POWER_SLOT";
			break;
		case ENGINE_SLOT:
			name = "ENGINE_SLOT";
			break;
		default: {
			char text[32];
			if (bttn_idx >= 0 && bttn_idx <= LAST_WEAPON_SLOT) {
				name = "WEAPON_SLOT";
			} else {
				G_ASSERT((bttn_idx >= FIRST_SYSTEM_SLOT) && (bttn_idx <= LAST_SYSTEM_SLOT));
				name = "SYSTEM_SLOT";
			}
			sprintf(text, "_%d", index);
			name += text;
			break;
		}
	}
}


// @0x5D2B90
void UIPdaEquipClass::Unknown_5D2B90()
{
	ClientObjectClass *ship = Client->Get_Ship();
	ShipPip->Unknown_542250(Unknown_497300(ship ? ship->RenderObject : NULL));
}







// functional from the pseudocode (D64); 0x5D2EB0 inlined
// @0x5D2C50
void UIPdaEquipClass::Unknown_5D2C50()
{
	if (Mounts.size()) {
		Unknown_5D2EB0(); Mounts.size();	// (the size() read dead, D272: it spends 0x5D2C50's nested budget, so erase 0x5D2F90 is called out of line under the inlined clear)
	}



	FAIL_IF(!Get_Client()) return;
	ClientObjectClass *ship = Client->Get_Ship();
	if (!ship || !ship->RenderObject) return;
	DeltaContainer *aux = (DeltaContainer *)ship->AuxData;
	if (!aux) return;
	AuxDataName mounts_name("Inventory.Mounts");
	DeltaArrayClass<InvSlotStruct> *mounts = Unknown_Find_Valid<DeltaArrayClass<InvSlotStruct> >(aux, mounts_name);
	if (!mounts) return;
	int count = mounts->Children.size();
	for (int i = 0; i < count; i++) {
		InvSlotStruct &value = mounts->Get_Value(i);
		UnknownMountSlotStruct *mount = new UnknownMountSlotStruct;
		mount->Type = value.I00;
		mount->Unknown04 = value.I08;
		mount->Unknown08 = value.I04;
		mount->Index = i;
		Mounts.push_back(mount);
	}
}


// @0x5D2EB0
void UIPdaEquipClass::Unknown_5D2EB0()
{
	for (std::vector<UnknownMountSlotStruct *>::iterator it = Mounts.begin(); it != Mounts.end(); it++) {
		delete *it;
	}
	Mounts.clear();	// (a plain clear(): 0x5D1A00 inlines it with erase's copy loop inline and _Destroy 0x5D2FE0 out of line; 0x5D2C50 spends its own budget beside its call)
}

/*
**	Reconstruction notes (PW1, 2026-11-09)
**
**	Every function here is functional from Ghidra's pseudocode (D64). 0x5D2EB0, 0x5D2F50, 0x5D2F90 and 0x5D3000 are
**	std::vector<UnknownMountSlotStruct *>'s helpers (Mounts). The PIP (0x541DE0) and the stats display (0x6A2ED0) are not
**	rebuilt (the POC's stand-ins); the PIP's gadget is gd_ship_equip_pip.cpp's. 0x5D1F60 is co.h's DecalName copied (the ship's
**	name, emitted here). Slot +0x7C looks up the 'ship' sub-object and drops it (the client's: a reference not
**	released). Slot +0x68's key 0xFFF is the back gadget's (0x62F4D0).
**
**	O4, 2026-11-14: 0x5D2EB0 is not a vector helper but a member with no caller (Mounts deleted and emptied), the
**	out-of-line copy of what 0x5D1A00 and 0x5D2C50 inline (defined after them). 0x5D2850 is a switch (the client's
**	sub/dec chain on 12, 13, 14) with a 32-byte buffer (its 0x20 frame). 0x5D2210 calls RenderObjGadgetClass's +0x90
**	qualified (a direct call: the slot gadget's own +0x90 asserts), its case bodies in the order 0x40, 0x80, 4, and
**	the weapons' count read before the devices' find. The finds go through a file-local Unknown_Find_Valid (co.cpp's
**	form), which sends Cast out of line as the client has it. The destructor removes the mesh's fonts through
**	0x6D2310 (the reference form). The FAIL_IF '!future_weapons' (l.363) is the client's own test: it fires when the
**	ship's 'Inventory.FutureWeapons' is missing or its Unknown70 is clear (0x5D2210's bytes test the same).
*/
