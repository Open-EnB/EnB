// Earth & Beyond client (reconstruction): app\client\inv_render.cpp, what draws an inventory item's slot
// (the client's file per its asserts, 'e:\build\beta\G\app\Client\inv_render.cpp' at 0xB90220). Notes at the end.
#include <string>
#include "..\shared\g_assert.h"
#include "inv_render.h"
#include "assetmgr.h"
#include "rendobj.h"
#include "fontmanager.h"
#include "fontdatabase.h"
#include "client.h"
#include "ui_item_examination_property_router.h"

// inv_data.cpp's, as far as this file calls it
class Unknown688830Item { public: TextureClass *Unknown_831840(); char Unknown00[0x48]; int Unknown48; char Unknown4C[4]; int Unknown50; };	// NAME 1: neutral (D10)
class Unknown688830Class { public: Unknown688830Item *Unknown_68AAA0(int slot, bool flag); int Unknown_689CA0(int slot); bool Unknown_689C30(); int Unknown_689D20(int id); char Unknown00[0x80]; int Unknown80; };	// NAME 1: neutral (D10); NAME 1 [slot, flag]: neutral
class SClient;
class LineDisplayClientClass { public: virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C(); virtual void Unknown_10(); virtual void Unknown_14(); virtual SClient *As_Space_Client(); };	// NAME 1: neutral (D10), as ui_cinfo.cpp declares it
struct InvDataBaseStruct { virtual void Unknown_00(); char Unknown04[0x18 - 4]; };	// NAME 1: neutral (D10); ui_slot_info.h's UISlotInfoItemBaseStruct
class InvDataClass : public InvDataBaseStruct, public RefCountClass { public: LineDisplayClientClass *Get_Client() { return(Client); } Unknown688830Class *Unknown_688830(); Unknown688830Class *Unknown_688850(); int Unknown_6A7990(); int Unknown_6A7A00(int index); LineDisplayClientClass *Client; };	// NAME 3: Peek_Inv_Data, inv_data's __FILE__ 0xB90004; Client: the FAIL_IF at line 182
class SlotDataClass { public: virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C(); virtual InvDataClass *Unknown_10(); };	// NAME 3: slot_data_render.h's
extern FontStringFilesClass *Unknown_BE67BC;	// NAME 1: neutral (D10)
class Unknown6A3D60Class { public: Unknown6A3D60Class(RenderObjClass *robj); ~Unknown6A3D60Class(); char Unknown00[0x18]; };	// NAME 1: neutral (D10); skill_render.cpp's
// ro_funcs.cpp's
void Unknown_496730(RenderObjClass *robj, int a, bool b);	// NAME 1: neutral (D10); NAME 1 [robj, a, b]: neutral
// 0x521B80: what walks an item's properties
class Unknown521B80Class { public: Unknown521B80Class(SClient *space_client, Unknown688830Item *item, const RefCountPtr<InvDataClass> &data, int slot); ~Unknown521B80Class(); void Unknown_521BD0(UIItemExaminationPropertyRouterBaseClass &visitor); int Unknown00[4]; };	// NAME 1: neutral (D10); NAME 1 [space_client, item, data, slot, visitor]: neutral
class Unknown68C680PropertyClass : public RefCountClass { public: virtual void Unknown_08(); virtual void Unknown_0C(); virtual void Unknown_10(); virtual void Unknown_14(); virtual bool Unknown_18(); };	// NAME 1: neutral (D10): ClientItemPropertySpecialClass as far as 0x68C680 calls it

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
	virtual void Unknown_0C(UIItemExaminationDescriptionPropertyStruct *property) {}		// +0x0C 0x68C540
	virtual void Unknown_10(UIItemExaminationTitleSubtitlesClass *property) {}				// +0x10 0x68C520
	virtual void Unknown_14(void *property) {}												// +0x14 0x68C500
	virtual void Unknown_18(UIItemExaminationTitleIconStruct *property) {}					// +0x18 0x68C4E0
	virtual void Unknown_1C(UIItemExaminationTitleNameClass *property) {}					// +0x1C 0x68C4C0
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
	bool usable = ((Unknown68C680PropertyClass *)property)->Unknown_18();	// NAME 1 [usable]: neutral
	if (!usable) Usable = usable;
}

// NOT EXACT: 0x68C6B0, 31 rows: the client calls Get_Roman_Numeral and 0x68EAE0 where this inlines them, and loads level after the release
// 0x68C6B0
void InvRenderClass::Unknown_2C(bool force)
{
	FAIL_IF(!Peek_Inv_Data()->Get_Client()) return;
	SClient *space_client = Peek_Inv_Data()->Get_Client()->As_Space_Client();	// NAME 2 [space_client]: its slot
	int slot = Get_Slot();
	int count = 0;	// NAME 1 [count, usable, item, level, data, other, item_level, now, inv_data, done]: neutral
	bool usable = true;
	Unknown688830Item *item = 0;
	int level = 0;
	if (slot != -1) {
		Unknown688830Class *data = Peek_Inv_Data()->Unknown_688830();
		item = data->Unknown_68AAA0(slot, false);
		if (!data->Unknown80) {
			force = true;
		}
		if (item) {
			count = data->Unknown_689D20(slot);
			if (!data->Unknown80) {
				force = true;
			}
			data = Peek_Inv_Data()->Unknown_688850();
			Unknown688830Item *other = data->Unknown_68AAA0(slot, false);
			if (!data->Unknown80) {
				force = true;
			}
			if (other) {
				count = data->Unknown_689D20(slot);
				if (!data->Unknown80) {
					force = true;
				}
			}
			int item_level = item->Unknown48;
			DurationClass now((unsigned)((ClientClass *)space_client)->Unknown10BC);
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
		Unknown688830Item *item = data->Unknown_68AAA0(slot, false);
		if (item) {
			return(item->Unknown_831840());
		}
		data = Peek_Inv_Data()->Unknown_688830();
		item = data->Unknown_68AAA0(slot, false);
		if (item) {
			return(item->Unknown_831840());
		}
	}
	return(0);
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
	Unknown688830Item *item = data->Unknown_68AAA0(slot, false);
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
	Unknown5C = Peek_Inv_Data()->Unknown_688830()->Unknown_689CA0(slot);
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
	if (force || data->Unknown_689C30()) {
		int old_count = Unknown58;
		int old_slot = Unknown20;
		int new_slot = -1;
		Unknown58 = 0;
		int count = Peek_Inv_Data()->Unknown_6A7990();
		for (int i = 0; i < count; i++) {
			int id = Peek_Inv_Data()->Unknown_6A7A00(i);
			if (data->Unknown_689CA0(id) == Unknown5C) {
				Unknown58 = data->Unknown_689D20(id) + Unknown58;
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
	Unknown74(-1.0f),
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
		Unknown6C = new Unknown6A3D60Class(mesh);
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

// 0x68EAE0
void InvRenderClass::Unknown_68EAE0(bool on, const RefCountPtr<RenderObjClass> &robj)
{
	if (on) {
		Unknown_6A8F00(robj.Peek());
	} else {
		Unknown_6A8FE0(robj.Peek());
	}
}

// 0x68FB60
Unknown68FB60Class::Unknown68FB60Class() :
	InvRenderClass(true),
	Unknown78(0)
{
	Unknown_BE67BC->Load("fa_ce02a.ini");
}

// 0x68FC80
Unknown68FB60Class::~Unknown68FB60Class()
{
	Unknown78 = 0;
	Unknown_BE67BC->Unload("fa_ce02a.ini");
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
**	lines 119-1107), its object 0x68BF10-0x68FB4A: `<string>`'s initialiser and its atexit (0x68FB10, 0x68FB40) end it. Blank stretches keep
**	__LINE__ for the functions not rebuilt yet: 0x68CCC0, 0x68E360, 0x68EB10, 0x68F510. 0x68FB60 on is inv_trade_render.cpp's
**	(0x68FE10 has that __FILE__).
**	0x68BF10 copies its allocator from the argument's own slot (`mov al,[esp+0x28]`), then tests that byte: hence the cast
**	in Unknown44's initialiser, and no stack slot of the temporary. Get_Roman_Numeral (0x68FA30) sits on the asserts' lines and inlines
**	into the earlier 0x68D700, where `value > 0` folds the first FAIL_IF away (line 1106 has no string reference there).
**	Unknown78Struct: three zeros with a constructor of their own keep +0x74's store in order at 0x68DC20.
**	Unknown38 is a DurationClass (a __int64 member hoists its store in 0x68BF10). 0x68C3E0 has no caller: 0x68C6B0 inlines it.
**	Unknown68C5D0Class: the visitor's table 0xAFCC74 stores empty slots, 0xAEE6DC's slots forward; its declared base (D137)
**	is not ClientItemPropertyVisitorClass, whose table 0x68C5A0 does not store: hence the cast at line 168.
*/
