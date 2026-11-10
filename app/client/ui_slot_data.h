// Earth & Beyond client (reconstruction): app\client\ui_slot_data.h, the slot windows' base (the client's ui_slot_data.cpp
// per its asserts, 'e:\build\beta\G\app\Client\ui_slot_data.cpp'; this header's name is provisional, NAME 1, D137). The
// class's name is the client's RTTI (MemberCallback<SlotDataClass, SlotDataInterface, ...> 0xB8A5F0); laid out from the
// constructor 0x62DE80 (UI1, 2026-11-09), its functions in ui_slot_data.cpp (SK1, 2026-11-09).
#ifndef UI_SLOT_DATA_H  // NAME 1 [ui_slot_data.h]: provisional header name for ui_slot_data.cpp
#define UI_SLOT_DATA_H

#include <vector>
#include <string>
#include "intrface.h"
#include "..\..\wwlib\ref_ptr.h"

class SClient;
class RenderObjClass;
class SlotDataGadgetClass;		// gd_slot_data.h's slot gadgets (+0x88)
class SlotDataClass;			// slot_data.h's (+0x98)
class SlotDataEventDataChange;	// slot_data.h's (the signal's event)
struct KernelEventClass;
class UIScutGadgetsClass;		// ui_scut_gadgets.h's (+0x84)
class DragSlotDataClass;		// drag_slot_data.cpp's (+0xA0)
class UISlotInfoClass;			// ui_slot_info.h's (+0x9C)

// ui_slot_data.cpp's slot window (0xBC bytes, constructors 0x62DE80 / 0x62DD20, destructor 0x62E320 (deleting 0x62DE50);
// tables 0xAF7F5C / 0xAF7F50 / 0xAF7F44; slots +0x88, +0xA4 and +0xA8 are _purecall)
class SlotDataInterface : public InterfaceClass	// NAME 5: the client's RTTI (MemberCallback<SlotDataClass, SlotDataInterface, ...>)
{
public:
#ifdef POC_WINDOW_CTOR
	SlotDataInterface(LineDisplayClientClass *client) : InterfaceClass(client), ShortcutUI(0), SlotData(0), Unknown9C(0), SlotDrag(0), SelectedHilight(0), UnknownA8(0), VisibleButtons(0), UnknownB0(-1), UnknownB4(-1)
		{ UnknownB8_0 = 0; UnknownB8_1 = 0; UnknownB8_2 = 0; UnknownB8_3 = 1; }	// (the POC's empty windows, poc_world3.cpp)
#endif
	SlotDataInterface(SClient *client);					// 0x62DE80  // NAME 1 [client]: neutral
	SlotDataInterface(UIViewClass *view, int unknown);	// 0x62DD20 (no caller found)  // NAME 1 [view, unknown]: InterfaceClass's
	virtual ~SlotDataInterface();						// 0x62E320
	virtual void Unknown_54();							// 0x62DF90 (FAIL_IF '!SlotData' l.149, G_ASSERT 'SelectedHilight' l.160)
	virtual void Unknown_58();							// 0x62E7C0
	virtual void Unknown_5C();							// 0x62E800
	virtual void Unknown_60();							// 0x62E870
	virtual void Unknown_64();							// 0x62E890
	virtual bool Unknown_68(int key);					// 0x62E850 (true)  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// 0x62E660  // NAME 1 [view, arg]: InterfaceClass's
	virtual SlotDataClass *Unknown_88() = 0;			// +0x88 (ui_inv.cpp's 0x59FA00: a new InvDataClass; ui_skills.cpp's 0x62D270)
	virtual InterfaceClass *Unknown_8C() { return(this); }	// +0x8C 0x549760 (inline)
	virtual InterfaceClass *Unknown_90() { return(NULL); }	// +0x90 0x549780 (inline: the inventory windows' answer, slot_data_render.cpp's factory)
	virtual InterfaceClass *Unknown_94() { return(NULL); }	// +0x94 0x585160 (inline: the skill windows' answer, slot_data_render.cpp's factory)
	virtual bool Unknown_98(SlotDataGadgetClass *gadget, KernelEventClass *event, unsigned int mask);	// +0x98 0x62E8B0 (a right press: the slot's information)  // NAME 1 [gadget, event, mask]: gd_slot_data.h's SlotDataIFaceClass
	virtual bool Unknown_9C(int slot, bool arg);		// +0x9C 0x62DE30 (false)  // NAME 1 [slot, arg]: what ui_inv.cpp's 0x59FA80 does with them
	virtual bool Unknown_A0(SlotDataGadgetClass *gadget) { return(true); }	// +0xA0 0x585180 (inline)  // NAME 1 [gadget]: what 0x62E8B0 passes
	virtual void Unknown_A4(std::string &name, int index, int arg) = 0;	// +0xA4 (ui_inv.cpp's 0x59FB20: the name and '_%d'; 0x62EB90 passes the slot and the button)  // NAME 1 [name, index, arg]: what 0x59FB20 does with them (arg unused)
	virtual const char *Unknown_A8() = 0;				// +0xA8 (ui_inv.cpp's 0x5851C0: its +0xCC; the skill windows' 'Skills')
	virtual SlotDataGadgetClass *Unknown_AC(RenderObjClass *robj, const int *slot);	// +0xAC 0x62EF80 (FAIL_IFs l.701 / 704 / 711: a button made for the model)  // NAME 1 [robj, slot]: neutral (ui_mount.cpp passes a button's sub-object and NULL)
	virtual void Unknown_B0();							// +0xB0 0x62ED40 (every button's slot from UnknownA8 on)
	virtual void Unknown_B4(int slot, int index);		// +0xB4 0x62EB90 (FAIL_IF '!gadget' l.565: one button's slot and tooltip)  // NAME 1 [slot, index]: neutral
	virtual void Unknown_B8(bool arg);					// +0xB8 0x62EE10 (FAIL_IF '!gadget' l.655: every button's +0xA8)  // NAME 1 [arg]: neutral
	virtual void Unknown_BC();							// +0xBC 0x62EEA0 (the buttons destroyed)
	virtual bool Unknown_C0(RefCountPtr<RenderObjClass> &robj, const char *format, int count);	// +0xC0 0x62F260 (+0xC4's)  // NAME 1 [robj, format, count]: neutral
	virtual bool Unknown_C4(RenderObjClass *robj, const char *format, int count);	// +0xC4 0x62F190 (FAIL_IF '!bttn' l.761: a button for each named sub-object)  // NAME 1 [robj, format, count]: neutral
	virtual void Unknown_C8();							// +0xC8 0x62F300 (the selection's highlight; the trade window calls it after setting +0xB4 to -1)
	virtual void Unknown_CC();							// +0xCC 0x62E580 (the slots changed: from the top)
	virtual void Unknown_D0();							// +0xD0 0x62E5B0 (+0xB8(false))
	void Unknown_62E150();								// 0x62E150 (FAIL_IF 'ShortcutUI' l.172, G_ASSERT '!Is_Active()' l.173: the shortcut bar)
	void Slot_Data_Changed(SlotDataEventDataChange *event, SlotDataClass *slot_data);	// 0x62E5D0 (FAIL_IFs '!event' l.236, 'SlotData != slot_data' l.237)  // NAME 2: as ui_slot_scut.cpp's; NAME 3 [event, slot_data]: the FAIL_IFs'
	bool Unknown_62E940(unsigned int notches, unsigned int notch_size);	// 0x62E940 (FAIL_IFs l.452 / 453)  // NAME 3 [notch_size]: the FAIL_IFs'; NAME 1 [notches]: neutral
	bool Unknown_62E9E0(int notches, unsigned int notch_size);	// 0x62E9E0 (FAIL_IFs l.479 / 480: up)  // NAME 3 [notch_size]: the FAIL_IFs'; NAME 1 [notches]: neutral
	bool Unknown_62EAA0(unsigned int notches, unsigned int notch_size);	// 0x62EAA0 (FAIL_IFs l.511 / 512: down)  // NAME 3 [notch_size]: the FAIL_IFs'; NAME 1 [notches]: neutral
	void Unknown_62EDC0(int count);						// 0x62EDC0 (VisibleButtons, at most the buttons)  // NAME 1 [count]: neutral
	SlotDataGadgetClass *Unknown_62F290(unsigned int idx);	// 0x62F290 (FAIL_IF 'idx >= Buttons.size()' l.780)  // NAME 3 [idx]: the FAIL_IF's
	void Unknown_62F4D0(RefCountPtr<RenderObjClass> &frame, int id);	// 0x62F4D0 (FAIL_IF '!robj' l.822: a back gadget; the attributes tab passes its 'new_frame' and 0x138C)  // NAME 1 [frame, id]: neutral
	void Unknown_62F5B0(bool on);						// 0x62F5B0 (UnknownB8_2 and every button's 0x670C90)  // NAME 1 [on]: neutral
	void Unknown_62F620(const char *name);				// 0x62F620 (a name into every button's render)  // NAME 1 [name]: neutral

	UIScutGadgetsClass *ShortcutUI;					// +0x84 (0x62E150 news it: FAIL_IF 'ShortcutUI' l.172; ui_inv.cpp's 0x5A0740 makes the shortcut)  // NAME 3: the assert's
	std::vector<SlotDataGadgetClass *> Buttons;		// +0x88 (the slots' gadgets: the trade window's 0x652890 reads 0..4 as '!bttn')  // NAME 3: the FAIL_IF 'idx >= Buttons.size()'
	SlotDataClass *SlotData;							// +0x98 (referenced; FAIL_IF '!SlotData' l.149)  // NAME 3: the FAIL_IF's
	UISlotInfoClass *Unknown9C;							// +0x9C (referenced: ui_slot_info.cpp's 0x630280 makes it)  // NAME 1: neutral (D10)
	DragSlotDataClass *SlotDrag;						// +0xA0 (0x62DF90 news it, drag_slot_data.cpp's 0x6851B0)  // NAME 3: ui_skills.cpp's assert 'from_slot != SlotDrag->Get_Slot()'
	RenderObjClass *SelectedHilight;					// +0xA4 ('select01', referenced)  // NAME 3: the G_ASSERT's
	int UnknownA8;										// +0xA8 (the first slot shown)  // NAME 1: neutral (D10)
	int VisibleButtons;									// +0xAC  // NAME 3: the FAIL_IFs '(VisibleButtons % notch_size) != 0'
	int UnknownB0;										// +0xB0 (-1: the buttons' 0x65FFA0)
	int UnknownB4;										// +0xB4 (the selected slot, -1 none: the trade window sets -1 before slot +0xC8)  // NAME 1: neutral (D10)
	unsigned char UnknownB8_0 : 1;						// +0xB8 bit 0 (ui_inv.cpp's 0x5A05B0: a click selects)  // NAME 1: neutral (D10)
	unsigned char UnknownB8_1 : 1;						// +0xB8 bit 1 (ui_inv.cpp's 0x59FE30: a drop tractors)  // NAME 1: neutral (D10)
	unsigned char UnknownB8_2 : 1;						// +0xB8 bit 2 (ui_inv.cpp's 0x5A05B0: a drag starts)  // NAME 1: neutral (D10)
	unsigned char UnknownB8_3 : 1;						// +0xB8 bit 3 (the constructor: set; 0x62EB90: empty buttons hidden)  // NAME 1: neutral (D10)
};

#endif // UI_SLOT_DATA_H
