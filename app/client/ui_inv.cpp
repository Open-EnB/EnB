// Earth & Beyond client (reconstruction): app\client\ui_inv.cpp, the inventory windows' base (the client's file
// per its asserts, 'e:\build\beta\G\app\Client\ui_inv.cpp' at 0xB803B0). Notes at the end.
#include <stdio.h>
#include "ui_inv.h"
#include "sclient.h"
#include "fontdatabase.h"
#include "gd_slot_data.h"
#include "slot_data.h"
#include "gadget.h"
#include "ui_scut_gadgets.h"
#include "ui_talkt.h"
#include "..\packets\pkequipuse.h"
#include "..\audio\soundlib.h"
#include "..\shared\g_assert.h"

// An item of the inventory as this file reads it (inv_data_peek.cpp's Get_Item)
struct InvDataPeekItemStruct	// NAME 1: neutral (D10)
{
	char Unknown00[0x0C];
	int Unknown0C;										// +0x0C (the item's type: a bit of +0xC8's mask)  // NAME 1: neutral (D10)
	char Unknown10[0x14 - 0x10];
	std::string Name;									// +0x14 (0x5A00C0's name of the buy)  // NAME 2: as ui_trade.cpp's
	char Unknown24[0x50 - 0x24];
	int Unknown50;										// +0x50 (the stack bought)  // NAME 1: neutral (D10)
	unsigned char Unknown54;							// +0x54  // NAME 1: neutral (D10)
};

#include "inv_data.h"	// InvDataClass and its lists (inv_data_peek.h's InvDataPeekClass)
// The client at InterfaceClass +0x40, as this file calls it
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18
};

// drag_slot_data.cpp's drag of a slot (the slot window's +0xA0), as this file calls it
class DragSlotDataClass	// NAME 3: class-from-file (D24) for drag_slot_data.cpp (0x6851B0, 0x685500, 0x685610)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04();
	virtual bool Unknown_08();							// +0x08 (0x5A0740: a drop onto a gadget)
	virtual void Unknown_0C();							// +0x0C
	char Unknown04[0x10 - 0x04];
	int Unknown10;										// +0x10
	char Unknown14[0x1C - 0x14];
	int Unknown1C;										// +0x1C (1, 2, 3: how much is moved)
	bool Unknown20;										// +0x20
	void Unknown_685500(int slot, int mode, bool arg);	// 0x685500 (drag_slot_data.cpp)  // NAME 1 [slot, mode, arg]: what 0x5A05B0 passes
	int Get_Slot();										// 0x685610 (ui_skills.cpp's 'from_slot != SlotDrag->Get_Slot()')  // NAME 3: the assert's
};

struct UIInvSlotGadgetStruct	// NAME 1: neutral (D10): a slot gadget's +0x68, as 0x5A0740 reads it
{
	char Unknown00[0x68];
	bool Unknown68;										// +0x68
};

struct UIInvEventStruct	// NAME 1: neutral (D10): the event 0x59FE30 and 0x5A05B0 test, as far as they read it
{
	unsigned short Unknown00;
	unsigned short Flags;								// +0x02 (0x100, 0x200, 0x1000)  // NAME 2: what it holds
};

// A slot gadget's table as far as 0x59FE30 calls it (slot +0xA4)
class UIInvGadgetClass	// NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(); virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C();
	virtual void Unknown_50(); virtual void Unknown_54(); virtual void Unknown_58(); virtual void Unknown_5C();
	virtual void Unknown_60(); virtual void Unknown_64(); virtual void Unknown_68(); virtual void Unknown_6C();
	virtual void Unknown_70(); virtual void Unknown_74(); virtual void Unknown_78(); virtual void Unknown_7C();
	virtual void Unknown_80(); virtual void Unknown_84(); virtual void Unknown_88(); virtual void Unknown_8C();
	virtual void Unknown_90(); virtual void Unknown_94(); virtual void Unknown_98(); virtual void Unknown_9C();
	virtual void Unknown_A0();
	virtual class SlotDataShortcutGadgetClass *As_SlotDataShortcutGadget();	// +0xA4  // NAME 3: the FAIL_IF's
};

struct UIInvViewUnknown44Struct	// NAME 1: neutral (D10): the view's slot +0x44, as this file reads it
{
	char Unknown00[0x2C];
	DragSlotDataClass *Unknown2C;				// +0x2C
};

#include "ui_mesh.h"	// the strips' models
#include "rendobj.h"
void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (avatar.cpp declares it the same)












// 0x59F060
void UIInvStripClass::Add(RenderObjClass *robj)
{
	FAIL_IF(!robj) return;
	robj->Add_Ref();
	Robjs.push_back(robj);
}

// 0x59F2C0
void UIInvStripClass::Unknown_54()
{
	UIInvClass::Unknown_54();
	int count = Robjs.size();
	for (int i = 0; i < count; i++) {
		Unknown_496AE0(Robjs[i], true);
		Unknown_AC(Robjs[i], NULL);
		REF_PTR_RELEASE(Robjs[i]);
	}
	Robjs.clear();
}

// 0x59F3D0
UIInvClass::UIInvClass(SClient *client, int kind) :
	SlotDataInterface(client),
	Kind(kind),
	InvType(INV_NONE),
	InvTransaction(NULL),
	UnknownC8(0),
	Name(NULL)
{
}





































// 0x59F470
void UIInvClass::Unknown_54()
{
	Unknown_BE67BC->Load("fc_inv.ini");
	switch (Kind) {
		case 0:
			Name = "SHIP_CARGO";
			UnknownB0 = -1;
			InvType = 1;
			UnknownB8_2 = true;
			UnknownC8 = 0x1000;
			break;
		case 1:
			Name = "SHIP_EQUIP";
			InvType = 2;
			UnknownB0 = -1;
			UnknownB8_2 = true;
			UnknownC8 = 0xFFFFFFFF;
			break;
		case 2:
			Name = "HULK_CARGO";
			InvType = 6;
			UnknownB0 = -1;
			UnknownB8_1 = true;
			break;
		case 3:
			Name = "HULK_EQUIP";
			InvType = 5;
			UnknownB0 = -1;
			UnknownB8_1 = true;
			break;
		case 4:
			Name = "HARVEST_CARGO";
			InvType = 18;
			UnknownB0 = -1;
			UnknownB8_1 = true;
			break;
		case 5:
			Name = "COMMOD_EXCHANGE_CARGO";
			InvType = 1;
			UnknownB8_0 = true;
			UnknownB8_2 = true;
			break;
		case 6:
			Name = "COMMOD_EXCHANGE_VENDOR";
			InvType = 4;
			UnknownB8_0 = true;
			UnknownB8_2 = true;
			break;
		case 7:
			Name = "VENDOR_TRADE";
			UnknownB0 = -1;
			InvType = 4;
			UnknownB8_0 = true;
			UnknownB8_2 = true;
			break;
		case 8:
			Name = "SHIP_EQUIP_CARGO";
			InvType = 1;
			UnknownB8_0 = true;
			UnknownB8_2 = true;
			break;
		case 9:
			Name = "SHIP_EQUIP_EQUIP";
			InvType = 2;
			UnknownB8_0 = true;
			UnknownB8_2 = true;
			break;
		case 10:
			Name = "PDA_CARGO";
			UnknownB0 = -1;
			InvType = 1;
			UnknownC8 = 0x1000;
			UnknownB8_3 = false;
			UnknownB8_2 = true;
			break;
		case 11:
			Name = "PDA_EQUIP";
			InvType = 2;
			UnknownB0 = -1;
			UnknownB8_2 = true;
			UnknownC8 = 0xFFFFFFFF;
			break;
		case 12:
			Name = "MANUFACTURE_DISPLAY";
			InvType = 13;
			UnknownB0 = -1;
			UnknownB8_0 = false;
			UnknownB8_1 = false;
			UnknownB8_2 = false;
			break;
		case 13:
			Name = "MANUFACTURE_SLOT";
			InvType = 12;
			UnknownB0 = -1;
			UnknownB8_0 = false;
			UnknownB8_2 = true;
			break;
		case 14:
			Name = "MANUFACTURING_OVERRIDE";
			InvType = 14;
			UnknownB0 = -1;
			UnknownB8_0 = false;
			UnknownB8_2 = true;
			break;
		case 15:
			Name = "PLAYER_TRADE_DISPLAY";
			UnknownB0 = -1;
			InvType = 16;
			UnknownB8_0 = false;
			UnknownB8_2 = true;
			break;
		case 16:
			Name = "TARGET_TRADE_DISPLAY";
			UnknownB0 = -1;
			InvType = 17;
			UnknownB8_0 = false;
			UnknownB8_2 = false;
			break;
		case 17:
			Name = "PLAYER_VAULT";
			UnknownB0 = -1;
			InvType = 3;
			UnknownB8_2 = true;
			break;
		case 18:
			Name = "REWARD_SLOT";
			UnknownB0 = -1;
			InvType = 19;
			UnknownB8_0 = false;
			UnknownB8_2 = false;
			break;
	}
	G_ASSERT(InvType != INV_NONE);
	SlotDataInterface::Unknown_54();
	if (UnknownC8) {
		Unknown_62E150();
	}
	InvTransaction = new InvTransactionClass;
}

// 0x59F940
UIInvClass::~UIInvClass()
{
	InvTransactionClass *transaction = InvTransaction;
	if (transaction) {
		transaction->Unknown_6908C0();
		operator delete(transaction);
	}
	InvTransaction = NULL;
	Unknown_BE67BC->Unload("fc_inv.ini");
}

// 0x59FA00
SlotDataClass *UIInvClass::Unknown_88()
{
	return(new InvDataClass(InvType, Client->As_Space_Client()));
}

// 0x59FA80
bool UIInvClass::Unknown_9C(int slot, bool arg)
{
	InvDataClass *inv = SlotData->Unknown_10();
	InvDataPeekItemStruct *item = inv->Unknown_688830()->Get_Item(slot, false);
	if (!item) return(false);
	unsigned int bit = (item->Unknown0C == -1) ? 0 : (1 << item->Unknown0C);
	if (arg && !(bit & 0x1C022) && !(item->Unknown54 & 5)) return(false);
	return((UnknownC8 & bit) == bit);
}

// 0x59FB20
void UIInvClass::Unknown_A4(std::string &name, int index, int arg)
{
	char buf[32];
	sprintf(buf, "_%d", index);
	name = Name ? Name : "Unknown";
	name += buf;
}




























// 0x59FD00
void UIInvClass::Unknown_7C(UIViewClass *view, bool arg)
{
	SlotDataInterface::Unknown_7C(view, arg);
	FAIL_IF(!InvTransaction) return;
	InvTransaction->Unknown_6908E0((Unknown6908E0Class *)view);
}

// 0x59FD70
void UIInvClass::Unknown_5C()
{
	InvTransaction->Unknown_690FA0();
	SlotDataInterface::Unknown_5C();
}

// 0x59FDA0
void UIInvClass::Unknown_60()
{
	SlotDataInterface::Unknown_60();
	if (Is_Busy()) {
		InvTransaction->Unknown_690D20();
		if (!Is_Busy()) {
			SlotDrag->Unknown_0C();
		}
	}
}

// 0x59FE10
InvDataClass *UIInvClass::Unknown_59FE10()
{
	return(SlotData->Unknown_10());
}


























































// NOT EXACT: 0.62: the client keeps the slot in edi and the button test in the event's argument slot, this the gadget in edi and a frame slot (register allocation; tried: no shadowed parameter, the event's bit as a bitfield)
// 0x59FE30
bool UIInvClass::Unknown_98(SlotDataGadgetClass *slot_gadget, KernelEventClass *event, unsigned int mask)
{
	bool down = ((UIInvEventStruct *)event)->Flags >> 12 & 1;
	bool button = (mask & 0xF0) != 0;
	int slot = slot_gadget->Unknown_670910();
	if (down && button && SlotDrag->Unknown10 != 2) {
		switch (SlotDrag->Unknown1C) {
			case 0:
			case 2:
				SlotDrag->Unknown_0C();
				return(SlotDataInterface::Unknown_98(slot_gadget, event, mask));
		}
	}
	if (mask & 0x1000) {
		UIInvGadgetClass *gadget = (UIInvGadgetClass *)slot_gadget;
		FAIL_IF(!gadget->As_SlotDataShortcutGadget()) return(false);
		if (down) {
			((ClientClass *)Client)->Send_Packet(&EquipUsePacketClass(((ClientClass *)Client)->ShipID, InvType, slot));
		}
		return(true);
	}
	if (UnknownB8_1) {
		if (!down) return(true);
		if (slot < 0) return(true);
		if (Is_Busy()) {
			Client->As_Space_Client()->Unknown_74D990(0x11, "Unable to tractor.  Dialog open.", 0);
			SoundLibraryClass::Play_2D_Sound("On_Fail", 2);
			return(true);
		}
		InvTransaction->Unknown_690AA0(InvType, slot, -1, true, 0, -1);
		return(true);
	}
	if (down) {
		if (UnknownB8_2 && !Is_Busy() && ((UIInvViewUnknown44Struct *)View->Unknown_44())->Unknown2C == SlotDrag && Unknown_5A0740(slot_gadget)) {
			SlotDrag->Unknown_0C();
		}
	} else {
		Unknown_5A05B0(slot_gadget, event, button);
	}
	return(false);
}




























// 0x5A00C0
void UIInvClass::Unknown_5A00C0(int *count, std::string *name)
{
	G_ASSERT(InvType == INV_VENDOR);
	int slot = UnknownB4;

	FAIL_IF(slot == InvDataClass::NO_SLOT) return;
	InvDataClass *inv = SlotData->Unknown_10();
	InvDataPeekItemStruct *item = inv->Unknown_688830()->Get_Item(slot, false);
	if (item) {
		*count = item->Unknown50;
		*name = item->Name;
	}
}













// 0x5A02E0
bool UIInvClass::Unknown_5A02E0(bool arg)
{
	G_ASSERT(InvType == INV_VENDOR);
	int slot = UnknownB4;

	FAIL_IF(slot == InvDataClass::NO_SLOT) return(false);
	int num = 1;
	if (arg) {
		InvDataClass *inv = SlotData->Unknown_10();
		InvDataPeekItemStruct *item = inv->Unknown_688830()->Get_Item(slot, false);
		if (item) num = item->Unknown50; item->Name.size();	// (a dead read: the cost D165's FAIL_IF form took from this function, so that 0x5A0740 calls it out of line as the client does; workflow.md 3.1.2)
	}
	return(InvTransaction->Unknown_690AA0(InvType, slot, num, true, 1, -1));
}

















// 0x5A03B0
bool UIInvClass::Unknown_5A03B0(bool arg)
{
	int slot = UnknownB4;
	FAIL_IF(slot == InvDataClass::NO_SLOT) return(false);
	int num = 1;
	if (arg) {
		InvDataPeekClass *peek = SlotData->Unknown_10()->Unknown_688830();
		num = peek->Get_Stack_Count(slot);
	}
	return(InvTransaction->Unknown_690AA0(InvType, slot, num, true, 4, -1));
}















// 0x5A0450
void UIInvClass::Unknown_5A0450(int type, int *num_items, int *num_slots)
{
	FAIL_IF(!num_items) return;
	FAIL_IF(!num_slots) return;
	*num_items = 0;
	*num_slots = 0;
	InvDataPeekClass *peek = SlotData->Unknown_10()->Unknown_688830();
	unsigned int i;
	for (i = 0; i < peek->Count; i++) {
		if (peek->Get_Item_Template_ID(i) == type) {
			*num_items += peek->Get_Stack_Count(i);
			(*num_slots)++;
		}
	}
	InvDataPeekClass *peek2 = SlotData->Unknown_10()->Unknown_688850();
	for (i = 0; i < peek->Count; i++) {		// (the client's: the first list's count)
		if (peek2->Get_Item_Template_ID(i) == type) {
			*num_items += peek2->Get_Stack_Count(i);
			(*num_slots)++;
		}
	}
}

// 0x5A05B0
void UIInvClass::Unknown_5A05B0(SlotDataGadgetClass *gadget, KernelEventClass *event, bool arg)
{
	if (!Is_Busy() && ((UIInvViewUnknown44Struct *)View->Unknown_44())->Unknown2C == NULL) {
		int slot = gadget->Unknown_670910();
		if (UnknownB8_0) {
			InvDataClass *inv = SlotData->Unknown_10();
			int id = inv->Unknown_688830()->Get_Item_Template_ID(slot);
			if (id == -1 || id == -2) return;
			UnknownB4 = slot;
			Unknown_C8();
			SoundLibraryClass::Play_2D_Sound("TextClick03.wav", 2);
		}
		if (UnknownB8_2) {
			int mode = 2;
			if (((UIInvEventStruct *)event)->Flags & 0x200) {
				mode = 1;
			} else if (((UIInvEventStruct *)event)->Flags & 0x100) {
				mode = 3;
			}
			SlotDrag->Unknown_685500(slot, mode, arg);
		}
	}
}

// 0x5A06C0
void UIInvClass::Unknown_5A06C0(SlotDataGadgetClass *gadget)
{
	if (UnknownB8_2 && !Is_Busy() && ((UIInvViewUnknown44Struct *)View->Unknown_44())->Unknown2C == SlotDrag && Unknown_5A0740(gadget)) {
		SlotDrag->Unknown_0C();
	}
}

























































// NOT EXACT: 0.68, 1174 bytes against the client's 1154 (shape): the client's one 'return true' block and the epilogue sit after the single item's 'Transferring' line, where the stack's 'Selling' line joins its call; here they follow the first FAIL_IF (the result variable, early returns in the shortcut part, a ?: FAIL_IF and a result set in the drop part change nothing or lose)
// 0x5A0740
bool UIInvClass::Unknown_5A0740(SlotDataGadgetClass *slot_gadget)
{
	SClient *sclient = Client->As_Space_Client();
	FAIL_IF(!sclient) return(true);
	int from_slot = SlotDrag->Get_Slot();





	FAIL_IF(from_slot == InvDataClass::NO_SLOT) return(true);
	UIInvGadgetClass *gadget = (UIInvGadgetClass *)slot_gadget;
	if (!SlotDrag->Unknown_08()) {
		if (gadget->As_SlotDataShortcutGadget()) {
			if (sclient->Unknown_750B60()) {
				sclient->Unknown_74D990(0x11, "Cannot perform action while incapacitated.", 0);
			} else {
				sclient->Send_Packet(&EquipUsePacketClass(sclient->ShipID, InvType, from_slot));
			}
		} else if (SlotDrag->Unknown1C == 1 || SlotDrag->Unknown1C == 3) {
			if (SlotDrag->Unknown20) {
				if (sclient->CurrentView == sclient->Unknown135C) {
					int to_inv;
					const char *to_name;
					switch (InvType) {
						case 3:
							to_inv = 1;
							to_name = "cargo";
							break;
						case 1:
							to_inv = 3;
							to_name = "vault";
							break;
						default:
							return(true);
					}
					InvDataPeekClass *peek = SlotData->Unknown_10()->Unknown_688830();
					int num = peek->Get_Stack_Count(from_slot);
					if (num) {
						if (SlotDrag->Unknown1C == 1) num = 1;
						InvTransaction->Unknown_690AA0(InvType, from_slot, num, true, to_inv, -1);
						InvDataPeekItemStruct *item = peek->Get_Item(from_slot, false);
						if (item) {
							if (num == 1) {
								sclient->Print(0, "Transferring %s to %s.", item->Name.c_str(), to_name);
							} else {
								sclient->Print(0, "Transferring stack of %d %s to %s.", num, item->Name.c_str(), to_name);
							}
						}
					}
				}
			} else {
				InvDataPeekClass *peek = SlotData->Unknown_10()->Unknown_688830();
				InvDataPeekItemStruct *item = peek->Get_Item(from_slot, true);
				switch (InvType) {
					case INV_VENDOR:
						switch (SlotDrag->Unknown1C) {
							case 3:
								Unknown_5A02E0(true);
								sclient->Print(0, "Buying stack of %s.", item->Name.c_str());
								break;
							case 1:
								Unknown_5A02E0(false);
								sclient->Print(0, "Buying %s.", item->Name.c_str());
								break;
						}
						break;
					case 1:
						if (sclient->Unknown1298 && sclient->Unknown1298->Unknown_64EEA0()) {
							switch (SlotDrag->Unknown1C) {
								case 3: {
									InvDataPeekClass *peek = SlotData->Unknown_10()->Unknown_688830();
									int num = peek->Get_Stack_Count(from_slot);
									if (num) {
										InvTransaction->Unknown_690AA0(InvType, from_slot, num, true, 4, 0);
										sclient->Print(0, "Selling stack of %d %s.", num, item->Name.c_str());
									}
									break;
								}
								case 1:
									InvTransaction->Unknown_690AA0(InvType, from_slot, 1, true, 4, 0);
									sclient->Print(0, "Selling %s.", item->Name.c_str());
									break;
							}
						}
						break;
				}
			}
		}
	} else {
		GadgetClass *target = GadgetClass::Unknown_65F5F0((GadgetListManagerClass *)Client->As_Space_Client()->Unknown1278, true, false);
		Unknown66CB20Class *button;
		if (target && (button = (Unknown66CB20Class *)target->Unknown_14()) != NULL) {
			if (ShortcutUI) {
				ShortcutUI->Create_Shortcut(from_slot, button);
				ShortcutUI->Save_Current();
			}
		} else if (!gadget->As_SlotDataShortcutGadget() && target) {
			SlotDataGadgetClass *inv_gadget = (SlotDataGadgetClass *)target->Unknown_10();
			if (inv_gadget && inv_gadget->Unknown_670950() && ((SlotDataClass *)inv_gadget->Unknown_670950())->Unknown_10()) {
				if (!((UIInvSlotGadgetStruct *)inv_gadget)->Unknown68) return(true);
				Unknown_5A0D00(from_slot, inv_gadget);
			} else {
				if (!target->Unknown_1C() && !target->Unknown_18()) return(true);
				Unknown_5A0EB0(from_slot, SlotDrag->Unknown1C);
			}
			return(!Is_Busy());
		}
	}
	return(true);
}






















































































// NOT EXACT: 0.58: the client keeps the slot in ebx and spills the first list's peek, this the reverse (register allocation)
// 0x5A0D00
bool UIInvClass::Unknown_5A0D00(int slot, SlotDataGadgetClass *inv_gadget)
{
	FAIL_IF(!inv_gadget) return(false);
	InvDataClass *inv = SlotData->Unknown_10();
	InvDataPeekClass *peek = inv->Unknown_688830();
	int id = peek->Get_Item_Template_ID(slot);
	if (id == -1 || id == -2) return(false);
	int num = 1;
	if (SlotDrag->Unknown1C != 1) {
		num = inv->Unknown_688850()->Get_Stack_Count(slot);
		if (num == 0) num = peek->Get_Stack_Count(slot);
	}
	InvDataClass *to = ((SlotDataClass *)inv_gadget->Unknown_670950())->Unknown_10();
















	FAIL_IF(!to) return(false);
	int to_slot = inv_gadget->Unknown_670910();
	bool all = (SlotDrag->Unknown1C == 3 || (SlotDrag->Unknown1C == 2 && SlotDrag->Unknown20));
	UnknownB4 = -1;
	Unknown_C8();
	return(InvTransaction->Unknown_690AA0(InvType, slot, num, all, to->Unknown50, to_slot));
}

// 0x5A0EB0
bool UIInvClass::Unknown_5A0EB0(int slot, int mode)
{
	if (InvType == INV_VENDOR) return(false);
	int num = 1;
	if (mode != 1) {
		InvDataClass *inv = SlotData->Unknown_10();
		num = inv->Unknown_688850()->Get_Stack_Count(slot);
		if (num == 0) {
			inv = SlotData->Unknown_10();
			num = inv->Unknown_688830()->Get_Stack_Count(slot);
			if (num == 0) return(false);
		}
	}
	bool all = (mode == 3 || SlotDrag->Unknown20);
	if (InvType == 16) {
		return(InvTransaction->Unknown_690AA0(16, slot, num, all, 1, -1));
	}
	return(InvTransaction->Unknown_690AA0(InvType, slot, num, all, 11, -1));
}


// 0x59EA30 (MODEL = UIMeshClass), 0x59EDC0 (MODEL = RenderObjClass): the model's sub-objects named by the format and
// the index, each kept for the set-up (Add inline: its FAIL_IF l.108)
template<class MODEL>
UIInvStripOfClass<MODEL>::UIInvStripOfClass(SClient *client, MODEL *model, int count, const char *format, int kind) :
	UIInvStripClass(client, kind)
{
	for (int i = 0; i < count; i++) {
		char name[32];
		sprintf(name, format, i);
		RenderObjClass *robj = model->Get_Sub_Object_By_Name(name);
		Add(robj);
		REF_PTR_RELEASE(robj);
	}
}

template class UIInvStripOfClass<UIMeshClass>;
template class UIInvStripOfClass<RenderObjClass>;


/*
**	Notes (UI1, 2026-11-09).
**	The object runs from 0x59E9B0 (its static initialisers) to 0x5A1110; 0x59E970 before it is the interrupt
**	display's (ui_scan.h's Unknown59E2C0Class, constructor 0x59E2C0), not this file's. The strips (L9, 2026-11-11):
**	UIInvStripClass and the template over it, 0x59EA30-0x59F2C0, with the vector helpers 0x5A0FC0-0x5A1060 Add
**	uses and the adjustor thunks 0x5A10C0-0x5A1110 of the three tables. The template's constructor is defined at
**	the end of the file and instantiated there for both models: VC6 compiles templates at the end of the file, so
**	the constructors inline Add (l.108) and UIInvClass's constructor though both come later in the client's order.
**	The inline slots +0x90 and +0xA8 are emitted elsewhere (0x5851A0, 0x5851C0). 0x5A0450's second loop counts the
**	second list by the first one's Count, as the client does.
*/
