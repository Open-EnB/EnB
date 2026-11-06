// Earth & Beyond client (reconstruction): app\client\ui_slot_scut.h. The header's name is provisional (NAME 2: the client has
// only ui_slot_scut.cpp's __FILE__ 0xB8A74C). The class is laid out from its constructor 0x630490 and destructor 0x6307B0.
#ifndef UI_SLOT_SCUT_H
#define UI_SLOT_SCUT_H

#include <set>
#include "intrface.h"
#include "gadget.h"
#include "gd_robj.h"
#include "unknown_signalbase.h"
#include "ui_scut_gadgets.h"

class SlotDataInterface;

// The event the slot data signals (the client's RTTI, 0xB8A690)
class SlotDataEventDataChange {};	// NAME 5: the client's RTTI

// slot_data.cpp's class, as far as this file calls it
class SlotDataClass	// NAME 5: the client's RTTI, 0xB8A5C8
{
public:
	static SignalMap &Unknown_6A7AB0();					// 0x6A7AB0 (the function-local static 0xBE6688)
	static SignalMap &Signal() { return(Unknown_6A7AB0()); }
	int Unknown_6A7990();								// 0x6A7990 (the slots: +0x44's vector's size)
	int Unknown_6A7A00(unsigned int index);				// 0x6A7A00 (one of them, or -1)  // NAME 1 [index]: neutral

	char Unknown00[0x24];								// +0x00
	int Unknown24;										// +0x24 (0x6309F0 stops while it is zero)
};

// slot_data_render.cpp's render (reference counted), as far as this file calls it
class SlotDataRenderClass : public RefCountClass	// NAME 2: slot_data_render.cpp's file, the asserts' 'slot_render'
{
public:
	virtual void Unknown_08();
	virtual void Unknown_0C(std::string &key);			// +0x0C (0x631540: the slot's %d_Key)  // NAME 2 [key]: what 0x614FB0 makes of it
	virtual int Unknown_10(const char *key); 			// +0x10 (0x631270: restores the slot from a %d_Key)  // NAME 2 [key]: the FAIL_IF's
	virtual void Unknown_14();
	virtual void Unknown_18();
	virtual void Unknown_1C();
	virtual void Unknown_20();
	virtual void Unknown_24();
	virtual void Unknown_28();
	virtual void Unknown_2C();
	virtual void Unknown_30();
	virtual void Unknown_34();
	virtual void Unknown_38();
	virtual void Unknown_3C();
	virtual void Unknown_40(int slot);					// +0x40 (0x631000)  // NAME 2 [slot]: what 0x62DA30 passes

	static SlotDataRenderClass *Unknown_6A90A0(int shortcut, SlotDataInterface *iface, int arg);	// 0x6A90A0 (FAIL_IF '!iface')  // NAME 3 [iface]: the FAIL_IF's; NAME 1 [shortcut, arg]: neutral
};

// gd_slot_data.cpp's gadget, as far as this file calls it
class SlotDataGadgetClass : public RObjGadgetClass	// NAME 2: gd_slot_data.cpp's file, the asserts' 'sdata_gadg'
{
public:
	virtual SlotDataGadgetClass *Unknown_A4();			// +0xA4 (0x6309F0: 'scut_gadg')
	virtual void Unknown_A8(int arg);					// +0xA8 (0x6309F0 passes 0)  // NAME 1 [arg]: neutral
	int Unknown_670910();								// 0x670910 (its render's slot)

	char Unknown60[0x64 - sizeof(RObjGadgetClass)];
	SlotDataRenderClass *SlotRender;					// +0x64 (0x670910's FAIL_IF '!SlotRender')  // NAME 3: the FAIL_IF's
};

// The 0x70-byte gadget of a shortcut (table 0xAFB1A4)
class SlotDataShortcutGadgetClass : public SlotDataGadgetClass	// NAME 3: ui_inv.cpp's assert 'As_SlotDataShortcutGadget'
{
public:
	SlotDataShortcutGadgetClass(SlotDataInterface *slot_iface, SlotDataRenderClass *slot_render, const LayerClass &layer);	// 0x670E30  // NAME 2 [slot_iface, slot_render, layer]: what this file passes
	char Unknown68[0x70 - 0x68];
};

// The slot windows' base (0x62DE80, tables 0xAF7F5C / 0xAF7F50 / 0xAF7F44), as far as this file calls it
class SlotDataInterface : public InterfaceClass	// NAME 5: the client's RTTI (MemberCallback<SlotDataClass, SlotDataInterface, ...> 0xB8A5F0)
{
public:
	virtual void Unknown_88();
	virtual void Unknown_8C();
	virtual int Unknown_90();
	virtual int Unknown_94();							// +0x94 (0x6315C0: the bar)
	virtual void Unknown_98();
	virtual bool Unknown_9C(int slot, bool arg);		// +0x9C (0x631000, 0x6309F0: does the slot belong)  // NAME 2 [slot]: what they pass; NAME 1 [arg]: neutral

	SlotDataClass *Get_Slot_Data() { return(SlotData); }
	char Unknown84[0x98 - 0x84];						// +0x84
	SlotDataClass *SlotData;							// +0x98 (0x62DE80's class's FAIL_IF '!SlotData')  // NAME 3: the FAIL_IF's
};

// The shortcut gadgets of a slot interface's slots (0x60 bytes, table 0xAF82A8; 0x62E150 makes it). The name is the client's RTTI:
// MemberCallback<SlotDataClass, SlotDataShortcutInterface, ...> 0xB8A7C0
class SlotDataShortcutInterface : public UIScutGadgetsClass	// NAME 5: the client's RTTI
{
public:
	SlotDataShortcutInterface(SlotDataInterface *slot_iface, const std::string &name, SClient *sclient);	// 0x630490 (FAIL_IFs l.61, l.64)  // NAME 3 [slot_iface]: the FAIL_IF's; NAME 2 [name, sclient]: the base's
	virtual ~SlotDataShortcutInterface();				// +0x00 0x6307B0 (deleting 0x630650; FAIL_IF l.72)
	virtual void Set_UI(const RefCountPtr<UIScutClass> &ui);	// +0x04 0x630980 (FAIL_IF l.83)  // NAME 2: the base's
	virtual RObjGadgetClass *Unknown_0C(int slot);		// +0x0C 0x631000 (FAIL_IFs l.247-256)  // NAME 2 [slot]: what the render's slot +0x40 gets
	virtual bool Unknown_10(RObjGadgetClass *gadget, std::string &key);	// +0x10 0x631540 (FAIL_IFs l.295, l.298)  // NAME 2 [gadget, key]: the base's
	virtual RObjGadgetClass *Unknown_14(const char *key);	// +0x14 0x631270 (FAIL_IFs l.269-276)  // NAME 3 [key]: the FAIL_IF's
	virtual int Unknown_18();							// +0x18 0x6315C0 (FAIL_IF l.306)

	void Slot_Data_Changed(SlotDataEventDataChange *event, SlotDataClass *slot_data);	// 0x6309D0 (the SlotDataEventDataChange callback)  // NAME 2: what it does; NAME 2 [event, slot_data]: the callback's types
	void Update(SlotDataClass *slotdata);				// 0x6309F0 (FAIL_IFs l.95, l.96, l.179, l.182, l.226; G_ASSERT 'restored' l.138): a shortcut for each new slot  // NAME 2: what it does; NAME 3 [slotdata]: the FAIL_IF's

	SlotDataInterface *SlotIFace;						// +0x48 (FAIL_IF '!SlotIFace' l.61)  // NAME 3: the FAIL_IF's
	bool Updated;										// +0x4C (0x6309F0: set on its first full pass)  // NAME 2: what it holds
	std::set<int> Slots;								// +0x50 (the slots shortcuts have been made for)  // NAME 2: what it holds
};

#endif // UI_SLOT_SCUT_H
