// Earth & Beyond client (reconstruction): app\client\ui_slot_data.h, the slot windows' base (the client's ui_slot_data.cpp
// per its asserts, 'e:\build\beta\G\app\Client\ui_slot_data.cpp'; this header's name is provisional, NAME 1, D137), as far
// as ui_inv.cpp's UIInvClass uses it. Laid out from the constructor 0x62DE80 (UI1, 2026-11-09); its functions are not
// rebuilt. The skill windows' base (ui_attrb_skills.h's Unknown62D1F0Class) is over it too, still flattened there.
#ifndef UI_SLOT_DATA_H  // NAME 1 [ui_slot_data.h]: provisional header name for ui_slot_data.cpp
#define UI_SLOT_DATA_H

#include <vector>
#include <string>
#include "intrface.h"

class SClient;
class RenderObjClass;
class SlotDataGadgetClass;		// gd_slot_data.h's slot gadgets (+0x88)
class SlotDataClass;			// slot_data.h's (+0x98)
struct KernelEventClass;
class UIScutGadgetsClass;		// ui_scut_gadgets.h's (+0x84)
class DragSlotDataClass;		// drag_slot_data.cpp's (+0xA0)

// ui_slot_data.cpp's slot window (0xBC bytes, constructor 0x62DE80, destructor 0x62E320 (deleting 0x62DE50); tables
// 0xAF7F5C / 0xAF7F50 / 0xAF7F44; slots +0x88, +0xA4 and +0xA8 are _purecall)
class UISlotDataClass : public InterfaceClass	// NAME 3: class-from-file (D24) for app\Client\ui_slot_data.cpp
{
public:
#ifdef POC_WINDOW_CTOR
	UISlotDataClass(LineDisplayClientClass *client) : InterfaceClass(client), ShortcutUI(0), SlotData(0), Unknown9C(0), SlotDrag(0), UnknownA4(0), UnknownA8(0), UnknownAC(0), UnknownB0(-1), UnknownB4(-1)
		{ UnknownB8_0 = 0; UnknownB8_1 = 0; UnknownB8_2 = 0; UnknownB8_3 = 1; }	// (the POC's empty windows, poc_world3.cpp)
#endif
	UISlotDataClass(SClient *client);					// 0x62DE80  // NAME 1 [client]: neutral
	virtual ~UISlotDataClass();							// 0x62E320
	virtual void Unknown_54();							// 0x62DF90 (FAIL_IF '!SlotData' l.149, G_ASSERT 'SelectedHilight' l.160)
	virtual void Unknown_58();							// 0x62E7C0
	virtual void Unknown_5C();							// 0x62E800
	virtual void Unknown_60();							// 0x62E870
	virtual void Unknown_64();							// 0x62E890
	virtual bool Unknown_68(int key);					// 0x62E850  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// 0x62E660  // NAME 1 [view, arg]: InterfaceClass's
	virtual SlotDataClass *Unknown_88() = 0;			// +0x88 (ui_inv.cpp's 0x59FA00: a new InvDataClass)
	virtual InterfaceClass *Unknown_8C() { return(this); }	// +0x8C 0x549760 (inline)
	virtual InterfaceClass *Unknown_90() { return(NULL); }	// +0x90 0x549780 (inline)
	virtual InterfaceClass *Unknown_94() { return(NULL); }	// +0x94 0x585160 (inline)
	virtual bool Unknown_98(SlotDataGadgetClass *gadget, KernelEventClass *event, unsigned int mask);	// +0x98 0x62E8B0 (ui_inv.cpp's 0x59FE30 passes its own)  // NAME 1 [gadget, event, mask]: gd_slot_data.h's SlotDataIFaceClass
	virtual bool Unknown_9C(int slot, bool arg);		// +0x9C 0x62DE30 (false)  // NAME 1 [slot, arg]: what ui_inv.cpp's 0x59FA80 does with them
	virtual bool Unknown_A0(int arg) { return(true); }	// +0xA0 0x585180 (inline)  // NAME 1 [arg]: neutral
	virtual void Unknown_A4(std::string &name, int index, int arg) = 0;	// +0xA4 (ui_inv.cpp's 0x59FB20: the name and '_%d')  // NAME 1 [name, index, arg]: what 0x59FB20 does with them (arg unused)
	virtual const char *Unknown_A8() = 0;				// +0xA8 (ui_inv.cpp's 0x5851C0: its +0xCC; the attributes tab's 'Skills')
	virtual void Unknown_AC(RenderObjClass *robj, int arg);	// +0xAC 0x62EF80  // NAME 1 [robj, arg]: neutral (ui_mount.cpp passes a button's sub-object and 0)
	virtual void Unknown_B0();							// +0xB0 0x62ED40
	virtual void Unknown_B4();							// +0xB4 0x62EB90
	virtual void Unknown_B8();							// +0xB8 0x62EE10
	virtual void Unknown_BC();							// +0xBC 0x62EEA0
	virtual void Unknown_C0();							// +0xC0 0x62F260
	virtual void Unknown_C4();							// +0xC4 0x62F190
	virtual void Unknown_C8();							// +0xC8 0x62F300 (the trade window calls it after setting +0xB4 to -1)
	virtual void Unknown_CC();							// +0xCC 0x62E580
	virtual void Unknown_D0();							// +0xD0 0x62E5B0
	void Unknown_62E150();								// 0x62E150 (G_ASSERTs 'ShortcutUI' l.172, '!Is_Active()' l.173: the shortcut bar)

	UIScutGadgetsClass *ShortcutUI;					// +0x84 (0x62E150 news it: G_ASSERT 'ShortcutUI' l.172; ui_inv.cpp's 0x5A0740 makes the shortcut)  // NAME 3: the assert's
	std::vector<SlotDataGadgetClass *> Unknown88;		// +0x88 (the slots' gadgets: the trade window's 0x652890 reads 0..4 as '!bttn')  // NAME 1: neutral (D10)
	SlotDataClass *SlotData;							// +0x98 (FAIL_IF '!SlotData' l.149)  // NAME 3: the FAIL_IF's
	int Unknown9C;										// +0x9C
	DragSlotDataClass *SlotDrag;						// +0xA0 (0x62DF90 news it, drag_slot_data.cpp's 0x6851B0)  // NAME 3: ui_skills.cpp's assert 'from_slot != SlotDrag->Get_Slot()'
	int UnknownA4;										// +0xA4
	int UnknownA8;										// +0xA8
	int UnknownAC;										// +0xAC
	int UnknownB0;										// +0xB0 (-1)
	int UnknownB4;										// +0xB4 (the selected slot, -1 none: the trade window sets -1 before slot +0xC8)  // NAME 1: neutral (D10)
	unsigned char UnknownB8_0 : 1;						// +0xB8 bit 0 (ui_inv.cpp's 0x5A05B0: a click selects)  // NAME 1: neutral (D10)
	unsigned char UnknownB8_1 : 1;						// +0xB8 bit 1 (ui_inv.cpp's 0x59FE30: a drop tractors)  // NAME 1: neutral (D10)
	unsigned char UnknownB8_2 : 1;						// +0xB8 bit 2 (ui_inv.cpp's 0x5A05B0: a drag starts)  // NAME 1: neutral (D10)
	unsigned char UnknownB8_3 : 1;						// +0xB8 bit 3 (the constructor: set)  // NAME 1: neutral (D10)
};

#endif // UI_SLOT_DATA_H
