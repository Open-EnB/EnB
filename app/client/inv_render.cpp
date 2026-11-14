// Earth & Beyond client (reconstruction): app\client\inv_render.cpp, what draws an inventory item's slot
// (the client's file per its asserts, 'e:\build\beta\G\app\Client\inv_render.cpp' at 0xB90220). Notes at the end.
#include <string>
#include "..\shared\g_assert.h"
#include "inv_trade_render.h"
#include "assetmgr.h"
#include "rendobj.h"
#include "fontmanager.h"
#include "fontdatabase.h"
#include "..\audio\soundlib.h"
#include <stdio.h>
#include "..\shared\money.h"
#include "..\shared\auxdata\permstr.h"	// (its PermStringInitClass static: 0x68FB10)
#include "ui_item_examination_property_router.h"

#include "inv_data_peek.h"	// InvDataPeekClass (InvDataClass::Unknown_688830 / 688850)
class Unknown688830Item { public: int Get_Type() { return(Unknown0C); } const std::string &Get_Name() { return(Unknown14); } char Unknown00[4]; int Unknown04; int Unknown08; int Unknown0C; char Unknown10[4]; std::string Unknown14; char Unknown24[0x48 - 0x24]; int Unknown48; char Unknown4C[4]; int Unknown50; char Unknown54[4]; int Unknown58; };	// NAME 1: neutral (D10)
typedef InvDataPeekClass Unknown688830Class;	// inv_data_peek.h's view of an inventory list, under this file's old name  // NAME 1: neutral (D10)
#include "sclient.h"	// SClient (Get_Target 0x744FE0, the slot +0x28 Get_Avatar, the clock +0x10BC) and co.h's ClientObjectClass
#include "distance_calc_client.h"	// 0x7E6060 / 0x7E6130

class LineDisplayClientClass { public: virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C(); virtual void Unknown_10(); virtual void Unknown_14(); virtual SClient *As_Space_Client(); };	// NAME 1: neutral (D10), as ui_cinfo.cpp declares it
#include "slot_data.h"	// SlotDataClass (InvDataClass's base: Unknown_6A7990 / 6A7A00, Unknown_10)
#include "inv_data.h"	// InvDataClass (Peek_Inv_Data)
// (InvDataClass's client is SClient: its slot +0x18 Unknown_18 answers itself, 0x740AC0)
extern FontStringFilesClass *Unknown_BE67BC;	// NAME 1: neutral (D10)
#include "shortcut_range_gauge.h"	// ShortcutRangeGaugeClass (0x6A3D60)
// the zero 0x68E360 compares with: its destructor is the empty 0x68EAC0
class Unknown68E360Class : public DurationClass { public: Unknown68E360Class(__int64 time = 0) : DurationClass(time) {} ~Unknown68E360Class() {} };	// NAME 1: neutral (D10)
// ro_funcs.cpp's
void Unknown_496730(RenderObjClass *robj, int a, bool b);	// NAME 1: neutral (D10); NAME 1 [robj, a, b]: neutral
bool operator!=(const MoneyClass &a, const MoneyClass &b);	// 0x4F0640: money.cpp's inequality
// 0x521B80: what walks an item's properties
class Unknown521B80Class { public: Unknown521B80Class(SClient *space_client, Unknown688830Item *item, const RefCountPtr<InvDataClass> &data, int slot); ~Unknown521B80Class(); void Unknown_521BD0(UIItemExaminationPropertyRouterBaseClass &visitor); int Unknown00[4]; };	// NAME 1: neutral (D10); NAME 1 [space_client, item, data, slot, visitor]: neutral
#include "item\client_item_property_special.h"	// ClientItemPropertySpecialClass (0x68C680 asks Is_Usable)

const char *Get_Roman_Numeral(int value);

static const char *_RomanNumerals[] = {	// 0xB90138  // NAME 3: the assert at line 1107
	" ", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X", "XI", "XII", "XIII", "XIV", "XV", "XVI", "XVII", "XVIII", "XIX", "XX"
};

static DurationClass _Unknown_BE6588(3);	// 0xBE6588 ($E 0x68C130)  // NAME 1: neutral (D10)

class Unknown68C5D0Class	// NAME 1: neutral (D10): table 0xAFCC74, the visitor with empty slots
{
public:
	virtual void Unknown_00(ClientItemPropertyOptionClass *property) {}						// +0x00 0x68C580
	virtual void Unknown_04(ClientItemPropertySpecialClass *property) {}					// +0x04 0x68C5D0
	virtual void Unknown_08(ClientItemPropertyAttributeClass *property) {}					// +0x08 0x68C560
	virtual void Unknown_0C(ClientItemPropertyDescriptionClass *property) {}			// +0x0C 0x68C540
	virtual void Unknown_10(ClientItemPropertyTypeClass *property) {}				// +0x10 0x68C520
	virtual void Unknown_14(ClientItemPropertyUnknown529810Class *property) {}				// +0x14 0x68C500
	virtual void Unknown_18(ClientItemPropertyIconClass *property) {}					// +0x18 0x68C4E0
	virtual void Unknown_1C(ClientItemPropertyNameClass *property) {}						// +0x1C 0x68C4C0
	virtual void Unknown_20(void *property) {}												// +0x20 0x68C4A0
};

class Unknown68C5A0Class : public UIItemExaminationPropertyRouterBaseClass, public Unknown68C5D0Class	// NAME 1: neutral (D10): tables 0xAFCC68, 0xAFCC3C
{
public:
	Unknown68C5A0Class();																	// 0x68C5A0
	virtual void Clear();																	// 0x68C5F0
	virtual void Route(const RefCountPtr<ClientItemPropertyClass> &property);				// 0x68C610
	virtual void Unknown_04(ClientItemPropertySpecialClass *property);						// 0x68C680

	bool Usable;																			// +0x08  // NAME 1: what 0x68C6B0 does with it
};

// 0x68BF10
InvRenderClass::InvRenderClass(bool arg) :
	LevelText(0),
	CountText(0),
	Unknown38(0),
	Unknown44("All", *(std::allocator<char> *)&arg)
{
	if (arg) LevelText = new FontSys("INVENTORY_LEVEL", 0, 0);
	CountText = new FontSys("INVENTORY_COUNT", 0, 0);
}

// 0x68C190
InvRenderClass::~InvRenderClass()
{
	if (CountText) {
		CountText->Remove();
		REF_PTR_RELEASE(CountText);
	}
	if (LevelText) {
		LevelText->Remove();
		REF_PTR_RELEASE(LevelText);
	}
	if (CannotUseIndicator) {
		CannotUseIndicator->Remove();
		CannotUseIndicator = NULL;
	}
}


















// 0x68C2D0
bool InvRenderClass::Unknown_08(SlotDataClass *sd, RenderObjClass *robj)
{
	CannotUseIndicator = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj("item_noskill"));
	G_ASSERT(CannotUseIndicator);
	return(SlotDataRenderClass::Unknown_08(sd, robj));
}

// 0x68C3E0
bool Unknown_68C3E0(SClient *space_client, Unknown688830Item *item, const RefCountPtr<InvDataClass> &data, int slot)	// NAME 1: neutral (D10); NAME 1 [space_client, item, data, slot]: neutral
{
	Unknown68C5A0Class visitor;	// NAME 1 [visitor]: neutral
	Unknown521B80Class info(space_client, item, data, slot);	// NAME 1 [info]: neutral
	info.Unknown_521BD0(visitor);
	return(visitor.Usable);
}

// 0x68C5A0
Unknown68C5A0Class::Unknown68C5A0Class() : Usable(true)
{
}

// 0x68C5F0
void Unknown68C5A0Class::Clear()
{
	Usable = true;
}






















// 0x68C610
void Unknown68C5A0Class::Route(const RefCountPtr<ClientItemPropertyClass> &property)
{
	FAIL_IF(!property) return;
	property->Accept((ClientItemPropertyVisitorClass *)(Unknown68C5D0Class *)this);
}

// 0x68C680
void Unknown68C5A0Class::Unknown_04(ClientItemPropertySpecialClass *property)
{
	bool usable = property->Is_Usable();	// NAME 1 [usable]: neutral
	if (!usable) Usable = usable;
}

// NOT EXACT: 0x68C6B0, 31 rows: the client calls Get_Roman_Numeral and 0x68EAE0 where this inlines them, and loads level after the release
// 0x68C6B0
void InvRenderClass::Unknown_2C(bool force)
{
	FAIL_IF(!Peek_Inv_Data()->Get_Client()) return;
	SClient *space_client = (SClient *)Peek_Inv_Data()->Get_Client()->As_Space_Client();	// NAME 2 [space_client]: its slot
	int slot = Get_Slot();
	int count = 0;	// NAME 1 [count, usable, item, level, data, other, item_level, now, inv_data, done]: neutral
	bool usable = true;
	Unknown688830Item *item = 0;
	int level = 0;
	if (slot != -1) {
		Unknown688830Class *data = Peek_Inv_Data()->Unknown_688830();
		item = (Unknown688830Item *)data->Get_Item(slot, false);
		if (!data->Status) {
			force = true;
		}
		if (item) {
			count = data->Get_Stack_Count(slot);
			if (!data->Status) {
				force = true;
			}
			data = Peek_Inv_Data()->Unknown_688850();
			Unknown688830Item *other = (Unknown688830Item *)data->Get_Item(slot, false);
			if (!data->Status) {
				force = true;
			}
			if (other) {
				count = data->Get_Stack_Count(slot);
				if (!data->Status) {
					force = true;
				}
			}
			int item_level = item->Unknown48;
			DurationClass now((unsigned)space_client->Unknown10BC);
			if (!force && Unknown38.Time <= now.Time) {
				goto done;
			}
			RefCountPtr<InvDataClass> inv_data = Create_Get(Peek_Inv_Data());
			usable = Unknown_68C3E0(space_client, item, inv_data, slot);
			Unknown38 = now.Time + _Unknown_BE6588.Time * 1000;
			level = item_level;
		}
	}
	if (force) {
		Unknown_68D700(level);
		Unknown_50(count);
		Unknown_68EAE0(!usable, CannotUseIndicator);
		Unknown_68EB10(item);
	}
done:
	SlotDataRenderClass::Unknown_2C(force);
}

// 0x68CBA0
TextureClass *InvRenderClass::Unknown_34()
{
	int slot = Unknown20;
	if (slot != -1) {
		Unknown688830Class *data = Peek_Inv_Data()->Unknown_688850();
		Unknown688830Item *item = (Unknown688830Item *)data->Get_Item(slot, false);
		if (item) {
			return(((Unknown831340Class *)item)->Unknown_831840());
		}
		data = Peek_Inv_Data()->Unknown_688830();
		item = (Unknown688830Item *)data->Get_Item(slot, false);
		if (item) {
			return(((Unknown831340Class *)item)->Unknown_831840());
		}
	}
	return(0);
}








// NOT EXACT: 0x68CCC0, 109 rows: register only, the client adds _Len + _N of the nine inlined appends through eax
// 0x68CCC0
const char *InvRenderClass::Unknown_28(std::string *string)
{
	FAIL_IF(!string) return(0);
	SlotDataRenderClass::Unknown_28(string);
	Unknown688830Class *data = Peek_Inv_Data()->Unknown_688830();
	int slot = Get_Slot();
	Unknown688830Item *item = (Unknown688830Item *)data->Get_Item(slot, false);
	if (item) {
		*string += item->Get_Name();
		*string += "\n";
		char buffer[32];	// NAME 1 [buffer, structure, quality, price, cost, type]: neutral
		float structure = data->Get_Structure(slot);
		if (structure != -1.0f) {
			sprintf(buffer, "Structure %0.0f%%\n", structure * 100.0f);
			*string += buffer;
		}
		float quality = data->Get_Quality(slot);
		if (structure != -1.0f) {
			sprintf(buffer, "Quality %0.0f%%\n", quality * 100.0f);
			*string += buffer;
		}
		MoneyClass price = data->Get_Price(slot);
		if (operator!=(price, MoneyClass(-1)) && operator!=(price, MoneyClass(0))) {
			_snprintf(buffer, sizeof(buffer), "Price %s\n", price.Amount_As_Text(true).c_str());
			*string += buffer;
		}
		if (item->Get_Type() == 13) {
			float cost = data->Get_Ave_Cost(slot);
			if (cost != 0.0f) {
				_snprintf(buffer, sizeof(buffer), "Avg. Cost %1.1f\n", cost);
				*string += buffer;
			}
		}
	} else {
		int type = data->Get_Mount_I00(slot);
		switch (type) {
			case 0:
				*string += "Unused Slot\n";
				break;
			case 4:
				*string += "Shields Slot\n";
				break;
			case 0x40:
				*string += "Engine Slot\n";
				break;
			case 0x80:
				*string += "Reactor Slot\n";
				break;
			case -1:
				*string += "Empty Slot\n";
				break;
			default:
				if (type & 0x1C022) {
					*string += "Weapon Slot\n";
				} else if (type & 0x20809) {
					*string += "Device Slot\n";
				} else {
					*string += "Empty Slot\n";
				}
				break;
		}
	}
	return("NORMAL_SLOT");
}




















// 0x68D620
InvDataClass *InvRenderClass::Peek_Inv_Data()
{
	SlotDataClass *sd = SlotData;
	FAIL_IF(!sd) return(0);

	InvDataClass *id = sd->Unknown_10();
	G_ASSERT(id);
	return(id);
}

// 0x68D690
int InvRenderClass::Unknown_30(int level)
{
	int result = SlotDataRenderClass::Unknown_30(level);
	if (result) {
		result--;
	}
	Unknown_496730(LevelText, result, 0);
	Unknown_496730(CountText, result, 0);
	Unknown_496730(CannotUseIndicator.Peek(), result, 0);
	return(result);
}

// 0x68D700
void InvRenderClass::Unknown_68D700(int value)
{
	if (LevelText) {
		if (value > 0) {
			LevelText->Unknown_6CE420(0, "%s", Get_Roman_Numeral(value));
			Unknown_6A8F00(LevelText);
		} else {
			Unknown_6A8FE0(LevelText);
		}
	}
}

// 0x68D790
void InvRenderClass::Unknown_50(int count)
{
	if (count > 1) {
		CountText->Unknown_6CE420(0, "%d", count);
		Unknown_6A8F00(CountText);
	} else {
		Unknown_6A8FE0(CountText);
	}
}

// 0x68D7E0
int InvRenderClass::Unknown_68D7E0(int slot)
{
	Unknown688830Class *data = Peek_Inv_Data()->Unknown_688830();
	Unknown688830Item *item = (Unknown688830Item *)data->Get_Item(slot, false);
	if (item) {
		return(item->Unknown50);
	}
	return(1);
}

// 0x68D870
Unknown68D870Class::Unknown68D870Class() :
	InvRenderClass(true),
	Unknown58(0),
	Unknown5C(-1)
{
}

// 0x68D8E0
Unknown68D870Class::~Unknown68D870Class()
{
}

// 0x68D900
void Unknown68D870Class::Unknown_50(int count)
{
	if (Unknown58 > 1) {
		CountText->Unknown_6CE420(0, "%d", Unknown58);
		Unknown_6A8F00(CountText);
	} else {
		Unknown_6A8FE0(CountText);
	}
}

// 0x68D950
void Unknown68D870Class::Unknown_40(int slot)
{
	Unknown5C = Peek_Inv_Data()->Unknown_688830()->Get_Item_Template_ID(slot);
	SlotDataRenderClass::Unknown_40(slot);
}

// 0x68D9E0
void Unknown68D870Class::Unknown_2C(bool force)
{
	if (Unknown5C != -1) {
		if (Unknown_68DA20(force)) {
			force = true;
		}
		InvRenderClass::Unknown_2C(force);
	}
}

// 0x68DA20
bool Unknown68D870Class::Unknown_68DA20(bool force)
{
	Unknown688830Class *data = Peek_Inv_Data()->Unknown_688830();
	if (force || data->Changed()) {
		int old_count = Unknown58;
		int old_slot = Unknown20;
		int new_slot = -1;
		Unknown58 = 0;
		int count = Peek_Inv_Data()->Unknown_6A7990();
		for (int i = 0; i < count; i++) {
			int id = Peek_Inv_Data()->Unknown_6A7A00(i);
			if (data->Get_Item_Template_ID(id) == Unknown5C) {
				Unknown58 = data->Get_Stack_Count(id) + Unknown58;
				new_slot = id;
			}
		}
		if (Unknown58 != old_count || new_slot != old_slot) {
			SlotDataRenderClass::Unknown_40(new_slot);
			return(true);
		}
	}
	return(false);
}

// 0x68DC20
Unknown68DC20Class::Unknown68DC20Class() :
	InvRenderClass(false),
	Unknown6C(0),
	Unknown70(0),
	Unknown74(-1.0f), Unknown78(0), Unknown80(0),
	Unknown84(0x100),
	Unknown88(true)
{
}

// 0x68DCC0
bool Unknown68DC20Class::Unknown_08(SlotDataClass *sd, RenderObjClass *robj)
{
	if (!InvRenderClass::Unknown_08(sd, robj)) {
		return(false);
	}













































	OnIndicator = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj("itemon"));
	G_ASSERT(OnIndicator);

	OutOfAmmoIndicator = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj("item_noammo"));
	G_ASSERT(OutOfAmmoIndicator);

	ValidityIndicator = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj("item_dark"));
	G_ASSERT(ValidityIndicator);

	WrongTargetTypeIndicator = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj("item_noskill"));
	G_ASSERT(ValidityIndicator);

	RechargeIndicator = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj("item_timer"));
	G_ASSERT(RechargeIndicator);
	RenderObjClass *mesh = Mesh;
	if (mesh) {
		Unknown6C = new ShortcutRangeGaugeClass(mesh);
	}
	return(true);
}

// 0x68E120
Unknown68DC20Class::~Unknown68DC20Class()
{
	delete Unknown6C;
	Unknown6C = 0;
	if (OnIndicator) {
		OnIndicator->Remove();
		OnIndicator = NULL;
	}
	if (OutOfAmmoIndicator) {
		OutOfAmmoIndicator->Remove();
		OutOfAmmoIndicator = NULL;
	}
	if (ValidityIndicator) {
		ValidityIndicator->Remove();
		ValidityIndicator = NULL;
	}
	if (WrongTargetTypeIndicator) {
		WrongTargetTypeIndicator->Remove();
		WrongTargetTypeIndicator = NULL;
	}
	if (RechargeIndicator) {
		RechargeIndicator->Remove();
		RechargeIndicator = NULL;
	}
}




















































// NOT EXACT: 0x68E360, about 120 rows: register and order only, in the compares of the duration and the second switch
// 0x68E360
void Unknown68DC20Class::Unknown_2C(bool force)
{
	static Unknown68E360Class _Zero;	// 0xBE65A8  // NAME 1: neutral (D10)
	DurationClass previous = Unknown78;	// NAME 1 [previous, flag, sclient, slot, target, self, data, state, moved, kind, type, ammo, weapon, device, now, done, on, wrong, sound]: neutral
	Unknown78 = _Zero;
	Unknown74 = -1.0f;
	bool flag = false;
	FAIL_IF(!Peek_Inv_Data()->Get_Client()) return;
	SClient *sclient = (SClient *)Peek_Inv_Data()->Get_Client()->As_Space_Client();
	FAIL_IF(!sclient) return;
	InvRenderClass::Unknown_2C(force);
	int slot = Unknown20;
	if (slot == -1) {
		if (force) {
			Unknown_68EAE0(false, OnIndicator);
			Unknown_68EAE0(false, ValidityIndicator);
			Unknown_68EAE0(false, OutOfAmmoIndicator);
			Unknown_68EAE0(false, WrongTargetTypeIndicator);
			Unknown_68EAE0(false, RechargeIndicator);
		}
		return;
	}
	ClientObjectClass *target = sclient->Get_Target();
	if (target) {
		ClientObjectClass *self = sclient->Get_Avatar();
		if (self) {
			Unknown74 = Unknown_7E6060(target, self);
		}
		if (target->Unknown_6F5A10() && !target->Unknown_6F5A40()) {
			flag = true;
		}
	}
	Unknown688830Class *data = Peek_Inv_Data()->Unknown_688830();
	unsigned state = data->Get_Item_State(slot);
	unsigned moved = data->Get_Effect_Validity(slot);
	unsigned kind = data->Get_Effect_Targets(slot);
	Unknown688830Item *item = (Unknown688830Item *)data->Get_Item(slot, true);
	int type = item ? item->Unknown0C : -1;
	bool weapon = false;
	bool device = false;
	int mode;
























	switch (type) {
		case 1:
		case 5:
		case 14:
		case 15:
		case 16:
		case 17:
			weapon = true;
			Unknown70 = data->Get_Target_Range(slot);
			if (!(state & 0x36)) {
				mode = (state & 0x800) ? 1 : 2;
				goto have;
			}
			break;
		case 0:
		case 2:
		case 6:
		case 7:
		case 8:
		case 11:
		case 12:
			if (data->Get_Effect_Usage(slot) & 5) {
				device = true;
				if (data->Get_Effect_Targets(slot) & 0x44) {
					Unknown70 = data->Get_Effect_Range(slot);
				}
				if (!(moved & 0x3F8)) {
					mode = ((kind & 0x34) != 0) ? 2 : 0;
					goto have;
				}
			}
			break;
		case -1:
			break;
		default:
			G_ASSERT(false);
			break;
	}
	mode = 0;
have:
	Unknown6C->Unknown_6A4020(Unknown74, Unknown70, mode, flag);
	DurationClass now((unsigned)sclient->Unknown10BC);
	Unknown78 = data->Get_Ready_Time(slot);
	bool update = force;	// NAME 1 [update, on]: neutral
	bool done;
	if (Unknown78.Time <= now.Time || (Unknown78 == _Zero)) {
		Unknown78 = _Zero;
		if (previous != _Zero) {
			update = true;
		}
		done = true;
	} else {
		Unknown78.Time = Unknown78.Time - now.Time;
		if (previous == _Zero) {
			update = true;
		}
		done = false;
	}
	if (Unknown80 != state) {
		update = true;
	}
	if ((!device || Unknown84 == moved) && !update) {
		return;
	}
	unsigned char on = (unsigned char)(state & 1);
	if (Unknown88 && (unsigned char)(Unknown80 & 1) != on) {
		Unknown88 = false;
		switch (type) {
			case 1:
			case 5:
			case 14:
			case 15:
			case 16:
			case 17:
				SoundLibraryClass::Play_2D_Sound(on ? "shortcut_activate_weapon.wav" : "shortcut_deactivate_weapon.wav", 2);
				break;
			case 0:
			case 12:
				SoundLibraryClass::Play_2D_Sound(on ? "shortcut_activate_ability_sn_1.wav" : "shortcut_deactivate_ability.wav", 2);
				break;
		}
	}
	Unknown80 = state;
	Unknown84 = moved;
	Unknown_68EAE0(on != 0, OnIndicator);
	Unknown_68EAE0((Unknown80 & 8) == 8, OutOfAmmoIndicator);
	Unknown_68EAE0(!done, RechargeIndicator);
	bool ready = false;	// NAME 1 [ready]: neutral
	bool wrong = false;
	if (weapon) {
		ready = (Unknown80 & 0x680) == 0;
		wrong = ((Unknown80 >> 1) & 1) != 0;
	} else if (device) {
		unsigned flags = data->Get_Effect_Validity(slot);	// NAME 1 [flags]: neutral
		ready = (flags == 0 && Unknown78 == _Zero);
		wrong = (flags & 0x70) != 0;
	}
	Unknown_68EAE0(!ready, ValidityIndicator);
	Unknown_68EAE0(wrong, WrongTargetTypeIndicator);
}

// 0x68EAE0
void InvRenderClass::Unknown_68EAE0(bool on, const RefCountPtr<RenderObjClass> &robj)
{
	if (on) {
		Unknown_6A8F00(robj.Peek());
	} else {
		Unknown_6A8FE0(robj.Peek());
	}
}

// 0x68EB10
void InvRenderClass::Unknown_68EB10(Unknown688830Item *item)
{
	int type = -1;	// NAME 1 [type, texture, group, kind, sub_kind, name]: neutral
	if (item) {
		type = item->Get_Type();
	}
	Unknown_6A8AB0(false);
	RefCountPtr<TextureClass> texture;
	switch (type) {
		case 10:
			texture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("icon_red.tga", MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true));
			if (Get_Filter() != "All" && Get_Filter() != "Ammo") {
				Unknown_6A8AB0(true);
			}
			break;
		case 14:
			texture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("icon_red.tga", MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true));
			if (Get_Filter() != "All" && Get_Filter() != "Beams") {
				Unknown_6A8AB0(true);
			}
			break;
		case 15:
			texture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("icon_red.tga", MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true));
			if (Get_Filter() != "All" && Get_Filter() != "Missile Launchers") {
				Unknown_6A8AB0(true);
			}
			break;
		case 16:
			texture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("icon_red.tga", MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true));
			if (Get_Filter() != "All" && Get_Filter() != "Projectile Launchers") {
				Unknown_6A8AB0(true);
			}
			break;
		case 6:
			texture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("icon_grey.tga", MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true));
			if (Get_Filter() != "All" && Get_Filter() != "Engines") {
				Unknown_6A8AB0(true);
			}
			break;
		case 7:
			texture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("icon_grey.tga", MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true));
			if (Get_Filter() != "All" && Get_Filter() != "Reactors") {
				Unknown_6A8AB0(true);
			}
			break;
		case 2:
			texture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("icon_grey.tga", MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true));
			if (Get_Filter() != "All" && Get_Filter() != "Shields") {
				Unknown_6A8AB0(true);
			}
			break;
		case 11:
			texture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("icon_grey.tga", MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true));
			if (Get_Filter() != "All" && Get_Filter() != "Devices") {
				Unknown_6A8AB0(true);
			}
			break;
		case 18:
			texture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("icon_blue.tga", MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true));
			if (Get_Filter() != "All" && Get_Filter() != "Overrides") {
				Unknown_6A8AB0(true);
			}
			break;
		case 13:
		{
			int group = item->Unknown04;
			int kind = item->Unknown08;
			int sub_kind = item->Unknown58;
			std::string name(item->Unknown14);
			if (sub_kind == 0x88) {
				texture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("icon_green.tga", MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true));
				if (Get_Filter() != "All" && Get_Filter() != "Trade Goods") {
					Unknown_6A8AB0(true);
				}
			} else if (sub_kind == 0x80 && (group == 0x50 || (group == -1 && kind == -1))) {
				texture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("icon_yellow.tga", MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true));
				if (Get_Filter() != "All" && Get_Filter() != "Resources") {
					Unknown_6A8AB0(true);
				}
			} else {
				texture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("icon_brown.tga", MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true));
				if (Get_Filter() != "All" && Get_Filter() != "Components") {
					Unknown_6A8AB0(true);
				}
			}
			break;
		}
	}
	Unknown_6A8A40(texture.Peek());
}


















































































































// NOT EXACT: 0x68F510, 22 rows: the client multiplies by -3600000 and adds; it calls max_size and _Eos in the last append
// 0x68F510
const char *Unknown68DC20Class::Unknown_28(std::string *string)
{
	static Unknown68E360Class _Zero;	// 0xBE65B8  // NAME 1: neutral (D10)
	FAIL_IF(!string) return(0);
	InvRenderClass::Unknown_28(string);
	char buffer[256];	// NAME 1 [buffer, seconds, whole, ms, hours, rest, minutes, s, m, h]: neutral
	if (Unknown74 > 0.0f) {
		sprintf(buffer, "Range: %.2fk / %.2fk\n", Unknown74 * 0.001f, Unknown70 * 0.001f);
		*string += buffer;
	}
	if (Unknown78 != _Zero) {
		static Unknown68E360Class _One(1);	// 0xBE6598  // NAME 1: neutral (D10)
		__int64 whole;
		const __int64 *seconds;
		if (Unknown78.Time < _One.Time * 1000) {
			seconds = &_One.Time;
		} else {
			whole = Unknown78.Time / 1000;
			seconds = &whole;
		}
		__int64 ms = *seconds * 1000;
		__int64 hours = ms / 3600000;
		__int64 rest = ms + hours * -3600000;
		__int64 minutes = rest / 60000;
		DurationClass secs((rest - minutes * 60000) / 1000);
		int s = secs.Get();
		int m = DurationClass(minutes).Get();
		int h = DurationClass(hours).Get();
		if (h) {
			sprintf(buffer, "Time: %d:02%d:02%d\n", h, m, s);
		} else if (m) {
			sprintf(buffer, "Time: %d:02%d\n", m, s);
		} else if (s == 1) {
			sprintf(buffer, "Time: 1 second\n");
		} else {
			sprintf(buffer, "Time: %d seconds\n", s);
		}
		*string += buffer;
	}
	return("WEAPON_ON");
}














































// 0x68FA30
const char *Get_Roman_Numeral(int value)
{
	FAIL_IF(value < 0) return("?");
	FAIL_IF(value >= (sizeof(_RomanNumerals) / sizeof(char *))) return("?");
	return(_RomanNumerals[value]);
}

/*
**	Reconstruction notes. The client's file per its asserts ('e:\build\beta\G\app\Client\inv_render.cpp' at 0xB90220;
**	lines 119-1107), its object 0x68BF10-0x68FB4A: `<string>`'s initialiser and its atexit (0x68FB10, 0x68FB40) end it.
**	0x68FB60 on is inv_trade_render.cpp's (0x68FE10 has its __FILE__), where its constructor and destructor now are (D209, P-IR3-1 (b)).
**	0x68BF10 copies its allocator from the argument's own slot (`mov al,[esp+0x28]`), then tests that byte: hence the cast
**	in Unknown44's initialiser, and no stack slot of the temporary. Get_Roman_Numeral (0x68FA30) sits on the asserts' lines and inlines
**	into the earlier 0x68D700, where `value > 0` folds the first FAIL_IF away (line 1106 has no string reference there).
**	Unknown38 and Unknown78 are DurationClass: as a __int64 member 0x68BF10's store hoists; Unknown78 at +0x78 keeps +0x74's store in order at 0x68DC20.
**	0x68C3E0 has no caller: 0x68C6B0 inlines it. Unknown68C5D0Class: the visitor's table 0xAFCC74 has empty slots, 0xAEE6DC's forward; its
**	declared base (D137) is not ClientItemPropertyVisitorClass, whose table 0x68C5A0 does not store: hence the cast in Route.
**	Accessors matter here: VC6's inline budget in the long functions moved to the client's only with Get_Slot, Get_Type, Get_Name (0x68CCC0)
**	and Get_Filter (0x68EB10, exact): without them the early compares and appends inline where the client calls them.
**	The statics with the empty destructors 0x68EAC0, 0x68FA10: Unknown68E360Class. 0x68FAA0 and 0x68FAE0 are another header's initialisers, not rebuilt
**	(0x68FAE0 was credited by the range while 0x68FB60 / 0x68FC80 sat here; its loss is the user's (D209, P-IR3-1 (b)). Lead: the
**	header of the enum statics 0xBE3B44 / 0xC416A8 / 0xC41658).
*/
