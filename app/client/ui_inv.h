// Earth & Beyond client (reconstruction): app\client\ui_inv.h, the inventory windows' base (the client's ui_inv.cpp per
// its asserts, 'e:\build\beta\G\app\Client\ui_inv.cpp' at 0xB803B0; this header's name is provisional, NAME 1, D137).
// Laid out from the constructor 0x59F3D0, the destructor 0x59F940 and the set-up 0x59F470 (UI1, 2026-11-09).
#ifndef UI_INV_H  // NAME 1 [ui_inv.h]: provisional header name for ui_inv.cpp
#define UI_INV_H

#include <vector>
#include <string>
#include "ui_slot_data.h"
#include "inv_transaction.h"

class SClient;
class RenderObjClass;
class SlotDataGadgetClass;		// gd_slot_data.h's slot gadgets
class SlotDataClass;			// slot_data.h's
class InvDataClass;				// client_item_property_builder.h's (0x59FE10)

// ui_inv.cpp's inventory window (0xD0 bytes, constructor 0x59F3D0, destructor 0x59F940 (deleting 0x59F440); tables
// 0xAF252C / 0xAF2520 / 0xAF2514): the base of the trade window (ui_trade.cpp), the mount points window (ui_mount.cpp)
// and the PDA's cargo, equipment and vault windows (pdaint.h)
class UIInvClass : public UISlotDataClass	// NAME 3: class-from-file (D24) for app\Client\ui_inv.cpp
{
public:
	enum { INV_NONE = 0, INV_VENDOR = 4 };				// InvType's (G_ASSERTs 'InvType != INV_NONE' l.306, 'InvType == INV_VENDOR' l.543 / 570)  // NAME 3: the asserts'
#ifdef POC_WINDOW_CTOR
	UIInvClass(LineDisplayClientClass *client) : UISlotDataClass(client), Kind(0), InvType(0), InvTransaction(0), UnknownC8(0), Name(0) {}	// (the POC's empty windows, poc_world3.cpp)
#endif
	UIInvClass(SClient *client, int kind);				// 0x59F3D0 (the kind into +0xBC: the trade window passes 7)  // NAME 1 [client]: neutral; NAME 2 [kind]: what it chooses
	virtual ~UIInvClass();								// 0x59F940 (deleting 0x59F440)
	virtual void Unknown_54();							// 0x59F470 (InvType and the flags by the kind; G_ASSERT 'InvType != INV_NONE' l.306)
	virtual void Unknown_5C();							// 0x59FD70 (the transaction cancelled, then the base's)
	virtual void Unknown_60();							// 0x59FDA0 (the transaction's steps)
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// 0x59FD00 (FAIL_IF '!InvTransaction' l.383)  // NAME 1 [view, arg]: InterfaceClass's
	virtual SlotDataClass *Unknown_88();				// 0x59FA00 (a new InvDataClass of InvType)
	virtual InterfaceClass *Unknown_90() { return(this); }	// +0x90 0x5851A0 (inline)
	virtual bool Unknown_98(SlotDataGadgetClass *slot_gadget, KernelEventClass *event, unsigned int mask);	// 0x59FE30 (FAIL_IF '!gadget->As_SlotDataShortcutGadget()' l.486)  // NAME 1 [slot_gadget, event, mask]: gd_slot_data.h's
	virtual bool Unknown_9C(int slot, bool arg);		// 0x59FA80 (the slot's item type within +0xC8)  // NAME 1 [slot, arg]: what it does with them
	virtual void Unknown_A4(std::string &name, int index, int arg);	// 0x59FB20 (Name, or 'Unknown', and '_%d')  // NAME 1 [name, index, arg]: what it does with them (arg unused)
	virtual const char *Unknown_A8() { return(Name); }	// +0xA8 0x5851C0 (inline)
	InvDataClass *Unknown_59FE10();						// 0x59FE10 (SlotData's slot +0x10; ui_trade.cpp calls it on the cargo window)
	void Unknown_5A00C0(int *count, std::string *name);	// 0x5A00C0 (G_ASSERT 'InvType == INV_VENDOR' l.543, FAIL_IF 'slot == InvDataClass::NO_SLOT' l.546; the trade window's 0x652890: the count and the name of the buy)  // NAME 1 [count, name]: what 0x652890 prints
	bool Unknown_5A02E0(bool arg);						// 0x5A02E0 (G_ASSERT l.570, FAIL_IF l.573: the selected slot bought; the trade window's 0x652FF0 passes true)  // NAME 1 [arg]: neutral
	bool Unknown_5A03B0(bool arg);						// 0x5A03B0 (FAIL_IF l.603: the selected slot sold; ui_trade.cpp's 0x652FF0 passes false, true)  // NAME 1 [arg]: neutral
	void Unknown_5A0450(int type, int *num_items, int *num_slots);	// 0x5A0450 (FAIL_IFs l.629 / 630; pdaint.cpp's 0x695E10 adds the three windows')  // NAME 3 [num_items, num_slots]: the FAIL_IFs; NAME 1 [type]: neutral
	void Unknown_5A05B0(SlotDataGadgetClass *gadget, KernelEventClass *event, bool arg);	// 0x5A05B0 (0x59FE30: a click, 'TextClick03.wav')  // NAME 1 [gadget, event, arg]: neutral
	void Unknown_5A06C0(SlotDataGadgetClass *gadget);	// 0x5A06C0 (no direct caller: 0x5A0740 when idle and the view's drag is ours)  // NAME 1 [gadget]: neutral
	bool Unknown_5A0740(SlotDataGadgetClass *gadget);	// 0x5A0740 (FAIL_IFs '!sclient' l.744, 'from_slot == InvDataClass::NO_SLOT' l.751: the drop)  // NAME 1 [gadget]: neutral
	bool Unknown_5A0D00(int slot, SlotDataGadgetClass *inv_gadget);	// 0x5A0D00 (FAIL_IFs '!inv_gadget' l.942, '!to' l.969)  // NAME 3 [inv_gadget]: the FAIL_IF's; NAME 1 [slot]: neutral
	bool Unknown_5A0EB0(int slot, int mode);			// 0x5A0EB0 (0x5A0740)  // NAME 1 [slot, mode]: neutral
	bool Is_Busy() { return(InvTransaction->State != 0 || InvTransaction->Unknown_6910C0()); }	// (inline: 0x59FDA0, 0x59FE30, 0x5A05B0, 0x5A06C0)  // NAME 2: what it returns

	int Kind;											// +0xBC (the constructor's: 0 'SHIP_CARGO' .. 18 'REWARD_SLOT')  // NAME 2: what it chooses
	int InvType;										// +0xC0 (by the kind; the asserts')  // NAME 3: the asserts'
	InvTransactionClass *InvTransaction;				// +0xC4 (0x59F470 news it; FAIL_IF '!InvTransaction')  // NAME 3: the FAIL_IF's
	unsigned int UnknownC8;								// +0xC8 (the item types taken, a mask: 0x1000, -1)  // NAME 1: neutral (D10)
	const char *Name;									// +0xCC (the kind's: 'SHIP_CARGO', ...)  // NAME 2: what it holds
};

#endif // UI_INV_H
