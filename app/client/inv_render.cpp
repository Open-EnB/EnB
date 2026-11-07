// Earth & Beyond client (reconstruction): app\client\inv_render.cpp, what draws an inventory item's slot
// (the client's file per its asserts, 'e:\build\beta\G\app\Client\inv_render.cpp' at 0xB90220). Notes at the end.
#include <string>
#include "..\shared\g_assert.h"
#include "inv_render.h"
#include "assetmgr.h"
#include "rendobj.h"
#include "fontmanager.h"
#include "fontdatabase.h"

// inv_data.cpp's, as far as this file calls it
class Unknown688830Item { public: TextureClass *Unknown_831840(); char Unknown00[0x50]; int Unknown50; };	// NAME 1: neutral (D10)
class Unknown688830Class { public: Unknown688830Item *Unknown_68AAA0(int slot, bool flag); int Unknown_689CA0(int slot); bool Unknown_689C30(); int Unknown_689D20(int id); };	// NAME 1: neutral (D10); NAME 1 [slot, flag]: neutral
class InvDataClass { public: Unknown688830Class *Unknown_688830(); Unknown688830Class *Unknown_688850(); int Unknown_6A7990(); int Unknown_6A7A00(int index); };	// NAME 3: Peek_Inv_Data, inv_data's __FILE__ 0xB90004
class SlotDataClass { public: virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C(); virtual InvDataClass *Unknown_10(); };	// NAME 3: slot_data_render.h's
extern FontStringFilesClass *Unknown_BE67BC;	// NAME 1: neutral (D10)
// ro_funcs.cpp's
void Unknown_496730(RenderObjClass *robj, int a, bool b);	// NAME 1: neutral (D10); NAME 1 [robj, a, b]: neutral

static const char *Get_Roman_Numeral(int value);

static const char *_RomanNumerals[] = {	// 0xB90138  // NAME 3: the assert at line 1107
	" ", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X", "XI", "XII", "XIII", "XIV", "XV", "XVI", "XVII", "XVIII", "XIX", "XX"
};

// 0x68BF10
InvRenderClass::InvRenderClass(bool arg) :
	LevelText(0),
	CountText(0),
	Unknown38(0),
	Unknown3C(0),
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

// 0x68DC20
Unknown68DC20Class::Unknown68DC20Class() :
	InvRenderClass(false),
	Unknown58(0),
	Unknown5C(0),
	Unknown60(0),
	Unknown64(0),
	Unknown68(0),
	Unknown6C(0),
	Unknown70(0),
	Unknown74(-1.0f),
	Unknown84(0x100),
	Unknown88(true)
{
}

// 0x68EAE0
void Unknown68DC20Class::Unknown_68EAE0(bool on, const RefCountPtr<RenderObjClass> &robj)
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


















































































































































































































































































































































































































































































































































































































































static const char *Get_Roman_Numeral(int value)
{
	FAIL_IF(value < 0) return("?");
	FAIL_IF(value >= (sizeof(_RomanNumerals) / sizeof(char *))) return("?");
	return(_RomanNumerals[value]);
}

/*
**	Reconstruction notes. The client's file per its asserts ('e:\build\beta\G\app\Client\inv_render.cpp' at 0xB90220;
**	lines 119-1107), its object 0x68BF10-0x68FC60 and on. Blank stretches keep __LINE__ for the functions not rebuilt yet:
**	0x68C3E0-0x68C680 (the property visitor classes), 0x68C6B0, 0x68CCC0, 0x68DCC0-0x68E120, 0x68E360, 0x68EB10, 0x68F510,
**	0x68FA30, 0x68FC80 on.
**	0x68BF10 copies its allocator from the argument's own slot (`mov al,[esp+0x28]`), then tests that byte: hence the cast
**	in Unknown44's initialiser, and no stack slot of the temporary. Get_Roman_Numeral sits on the assert's line and inlines
**	into the earlier 0x68D700, where `value > 0` folds the first FAIL_IF away (line 1106 has no string reference there).
**	Unknown78Struct: three zeros with a constructor of their own keep +0x74's store in order at 0x68DC20.
*/
