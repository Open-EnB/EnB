// Earth & Beyond client (reconstruction): app\client\gd_inv_slider.cpp, the inventory windows' slider (the client's
// __FILE__ 'e:\build\beta\G\app\Client\gd_inv_slider.cpp', 0x669FB0-0x66A6D0 by its asserts and its table 0xAFA3F4).
#include "ui_inv.h"
#include "gd_inv_slider.h"
#include "slot_data.h"
#include "unknown_signalbase.h"
#include "..\shared\g_assert.h"

// inv_data.cpp's inventory, as this file calls it (as ui_inv.cpp declares it)
class InvDataClass : public SlotDataClass	// NAME 3: Peek_Inv_Data, inv_data's __FILE__ 0xB90004
{
public:
	InvDataClass(int inv_type, SClient *client);		// 0x688410 (0x18C bytes)  // NAME 1 [inv_type, client]: what 0x59FA00 passes
	virtual bool Unknown_18();
	virtual DeltaCompoundClass *Unknown_1C();
	virtual bool Unknown_20();
};


// NOT EXACT: functional from the pseudocode (D64)
// 0x669FB0
Unknown669FB0Class::Unknown669FB0Class(UIInvClass *inventory, LayerClass *layer) :
	GadgetSlider(false, layer),
	Inventory(inventory),
	SlotData(NULL),
	SlotsPerNotch(0),
	VisibleNotches(0),
	Count(0)
{




	FAIL_IF(!Inventory) return;
	if (Inventory->SlotData) Inventory->SlotData->Add_Ref();
	SlotData = Inventory->SlotData;
	FAIL_IF(!SlotData) return;
	SlotDataClass::Unknown_6A7AB0().connect(SlotData, this, &Unknown669FB0Class::Unknown_66A4E0);
}

// NOT EXACT: functional from the pseudocode (D64)
// 0x66A190
Unknown669FB0Class::~Unknown669FB0Class()
{
	if (SlotData) {
		SlotDataClass::Unknown_6A7AB0().disconnect(SlotData, this, &Unknown669FB0Class::Unknown_66A4E0, SlotDataEventDataChange(0));
	}
	Inventory = NULL;
	if (SlotData) {
		SlotData->Release_Ref();
		SlotData = NULL;
	}
}


// 0x66A280
bool Unknown669FB0Class::Unknown_30(float x, float y)
{
	Unknown_66A3C0(false);
	return(GadgetSlider::Unknown_30(x, y));
}


// 0x66A2B0
void Unknown669FB0Class::Unknown_A4(float val, bool arg2)
{
	GadgetSlider::Unknown_A4(val, arg2);
	if (SlotsPerNotch) {
		Inventory->Unknown_62E940((int)(Value + 0.5f), SlotsPerNotch);
	}
}















// 0x66A300
void Unknown669FB0Class::Unknown_66A300(int a, int b)
{
	FAIL_IF(!Inventory) return;
	FAIL_IF(!Is_Attached()) return;
	if (IsHorizontal) {
		SlotsPerNotch = a;
		VisibleNotches = b;
	} else {
		SlotsPerNotch = b;
		VisibleNotches = a;
	}
	Unknown_66A3C0(true);
}


// 0x66A3C0
void Unknown669FB0Class::Unknown_66A3C0(bool force)
{
	FAIL_IF(!Is_Attached()) return;

	FAIL_IF(!Inventory) return;
	FAIL_IF(SlotsPerNotch <= 0) return;
	FAIL_IF(VisibleNotches <= 0) return;
	int count = ((SlotDataClass *)Inventory->Unknown_59FE10())->Unknown_6A7990();
	if (force || count != Count) {
		Count = count;
		Unknown_66FA90((float)((SlotsPerNotch - 1 + count) / SlotsPerNotch), (float)VisibleNotches, true);
	}
}

// NOT EXACT: functional from the pseudocode (D64)
// 0x66A4E0
void Unknown669FB0Class::Unknown_66A4E0(SlotDataEventDataChange *event, SlotDataClass *slot_data)
{





	FAIL_IF(!event) return;
	if (event->Value == 2) {
		Unknown_A4(Value, true);
	}
}

/*
**	Reconstruction notes (PW1, 2026-11-10)
**
**	Functional from Ghidra's pseudocode (D64), the asserts on the client's lines (34, 37, 90, 91, 106, 108, 109, 110,
**	127). The constructor takes the inventory's SlotData referenced (0x66A530 / 0x66A560 are that reference's
**	helpers) and connects 0x66A4E0 to SlotDataClass's signal map (MemberCallback table 0xAFA3D8); the destructor
**	disconnects it. 0x66A2B0 scrolls the inventory by the slider's rounded value (0x62E940).
*/
