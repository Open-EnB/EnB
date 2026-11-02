// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\intrface.h (name assumed from intrface.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\client\intrface.cpp'; the client holds no string of this header)
**
**	The UI kit's interface base (kernel-map-step9.md section 3.2): gclass.h's GClass (two bases with tables at
**	+0x00 and +0x04; e:\build\beta\G\app\shared\gclass.cpp, constructor 0x4A1880), and
**	intrface.cpp's InterfaceClass (constructors 0x540A90 / 0x540D30, tables 0xAEEF98 / 0xAEEF8C, a
**	RefCountClass at +0x38 with table 0xAEEF80). Every screen and window of the client derives from it:
**	the views (view.cpp) keep them in a list, reference counted. Declared as far as the rebuilt files use
**	it (D10); one class for what sclient.h (UIUnknownBaseClass, UIInterfaceBaseClass) and
**	ui_line_display.h (GClassUnknownClass, InterfaceClass: now GClass) declared apart until UI1 (P-LD4-1).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __INTRFACE_H  // NAME 3 [intrface.h]: header name assumed from the client's intrface.cpp
#define __INTRFACE_H

#include "..\..\wwlib\refcount.h"
#include "..\shared\gclass.h"

class UIViewClass;
class INIClass;
class ControlKeysManagerClass;  // control_keys_manager.cpp's (0x71A960 / 0x71AB40)
class LineDisplayClientClass;  // what +0x40 points to (slot +0x18 gives the sclient)
class GadgetClass;  // gadget.cpp's
class LoginUnknown0CClass;  // sclient.h's: the views' +0x04 (the input dispatcher)
class InterfaceStaticSortListClass;  // interface_static_sort_list.cpp's: the views' scene's +0x9C ('ssl')
class InterfaceClass;

// What InterfaceClass holds at +0x64 (0x0C bytes, constructor 0x686FA0; intrlist.cpp's functions), as far as ui_group.cpp calls it
class InterfaceUnknown64Class  // NAME 1: neutral (D10)
{
public:
#ifdef __INTRFACE_MEMBERS
	InterfaceUnknown64Class();																// 0x686FA0
	~InterfaceUnknown64Class();																// 0x686FD0
	void Unknown_6871D0(LoginUnknown0CClass *dispatcher, RefCountClass *handler, InterfaceClass *parent);	// 0x6871D0 (activated: the view's dispatcher, +0x70, +0x78)  // NAME 1 [dispatcher, handler, parent]: what intrface.cpp passes
	void Unknown_6872A0(LoginUnknown0CClass *dispatcher, InterfaceClass *parent);			// 0x6872A0 (deactivated)  // NAME 1 [dispatcher, parent]: what intrface.cpp passes
	void Unknown_687360(UIViewClass *view);													// 0x687360 (onto a new view)  // NAME 1 [view]: what intrface.cpp passes
#endif
	void Unknown_6870E0(GadgetClass *gadget, int id, UIViewClass *view, int unknown1, int unknown2);	// 0x6870E0: the gadget put in (ui_group.cpp: an id, the view, 1, 1)  // NAME 1 [gadget, id, view, unknown1, unknown2]: what ui_group.cpp passes
	void Unknown_687150(GadgetClass *gadget);												// 0x687150: the gadget taken out (before its delete)  // NAME 1 [gadget]: what ui_group.cpp passes
	char Unknown00[0x0C];																	// +0x00
};

#ifdef __INTRFACE_MEMBERS
class KeysSavedEvent;
class ControlKeysManager;
class GadgetGroupToFrontEvent;

// control_keys_manager.cpp's key list (InterfaceClass +0x4C, 0x0C bytes): control_keys_manager.h's ControlKeyListClass
class InterfaceKeyListClass  // NAME 1: neutral (D10)
{
public:
	InterfaceKeyListClass();																// 0x721D50
	~InterfaceKeyListClass();																// 0x721D80
	void Unknown_721F40(LoginUnknown0CClass *dispatcher, RefCountClass *handler, InterfaceClass *iface);	// 0x721F40 (activated)  // NAME 1 [dispatcher, handler, iface]: what intrface.cpp passes
	void Unknown_721F80(LoginUnknown0CClass *dispatcher, InterfaceClass *iface);			// 0x721F80 (deactivated)  // NAME 1 [dispatcher, iface]: what intrface.cpp passes

	char Unknown00[0x0C];
};

// intrlist.cpp's (InterfaceClass +0x58, 0x0C bytes)
class InterfaceUnknown58Class  // NAME 1: neutral (D10)
{
public:
	InterfaceUnknown58Class();																// 0x686B50
	~InterfaceUnknown58Class();																// 0x686B80
	void Unknown_686E90();																	// 0x686E90 (activated)
	void Unknown_686EE0();																	// 0x686EE0 (deactivated)
	void Unknown_686F10(int scene);															// 0x686F10 (onto a new view: its scene)  // NAME 1 [scene]: what intrface.cpp passes

	char Unknown00[0x0C];
};

class InterfaceKeyHandlerClass;
#endif

// intrface.cpp's class (0x84 bytes; 34 slots in 0xAEEF98): +0x00..+0x50 the gclass run 0x4A1800-0x4A2010
// (+0x44 / +0x48 gclass.cpp's two asserted functions), +0x64..+0x78 the one-line defaults 0x515690-0x515750
// emitted next to smouse.cpp, +0x58 / +0x5C / +0x60 / +0x7C / +0x80 / +0x84 intrface.cpp's (activate /
// deactivate). The views release them through the RefCountClass at +0x38 (dec [p+0x3C], then [[p+0x38]]'s slot 0)
class InterfaceClass : public GClass, public RefCountClass  // NAME 3: by convention (D24) from the client's intrface.cpp (constructors 0x540A90 / 0x540D30, table 0xAEEF98)
{
public:
	InterfaceClass(UIViewClass *view, int unknown);										// 0x540A90 (+0x48 the view, +0x44 the second, referenced)  // NAME 2 [view]: the member's assert '!View'; NAME 1 [unknown]: neutral
	virtual ~InterfaceClass();																// 0x540E90 (slot 0x540D00; G_ASSERT '!View')
	virtual void Unknown_54() { Unknown_10(); }												// 0x515690 (the slot +0x10; inline: emitted next to smouse.cpp); Init calls it after construction
	virtual void Unknown_58();																// 0x541010 (activates: G_ASSERT '!PartialDeactivatedGadget')
	virtual void Unknown_5C();																// 0x541210 (deactivates when Active)
	virtual void Unknown_60();																// 0x540C70 (the views' slot +0x14 under a sort object when Active)
	virtual void Unknown_64() {}															// 0x5156B0 (the views' slot +0x14 otherwise)
	virtual bool Unknown_68(int key) { return(false); }										// 0x5156D0
	virtual void Unknown_6C(INIClass &ini) {}												// 0x5156F0 (the views' 0x6B4900: SClient 0x745080 saves settings.ini through it)  // NAME 1 [ini]: what 0x745080 passes
	virtual void Unknown_70(ControlKeysManagerClass *manager) {}							// 0x515710
	virtual void Unknown_74(ControlKeysManagerClass *manager) {}							// 0x515730
	virtual bool Unknown_78(void *arg) { return(false); }									// 0x515750 ( tsk_login.h's screens: the global error packet)  // NAME 1 [arg]: neutral
	virtual void Unknown_7C(UIViewClass *view, bool arg);									// 0x5412F0 (onto the view, off the old one; the views' slot +0x10 passes NULL, 0)  // NAME 1 [view, arg]: what the callers pass
	virtual void Unknown_80();																// 0x541550 (FAIL_IF 'PartialDeactivatedGadget', '!Active')
	virtual void Unknown_84();																// 0x541690 (FAIL_IF '!PartialDeactivatedGadget', '!Active')
	InterfaceStaticSortListClass *Unknown_541800();											// 0x541800 (the view's scene's +0x9C, the asserts' 'ssl'; local_interface_sort.cpp's 0x6E0E80 calls it)
	void Unknown_5417D0(InterfaceClass *parent);											// 0x5417D0 (+0x78: the parent's, or this)  // NAME 1 [parent]: neutral
#ifdef __INTRFACE_MEMBERS
	InterfaceClass(LineDisplayClientClass *client);											// 0x540D30 (+0x40 the client; no view)  // NAME 1 [client]: the member's
	void Unknown_541830(KeysSavedEvent *event, ControlKeysManager *key_manager);	// (not a slot: ControlKeysManager's KeysSavedEvent callback, FAIL_IF '!key_manager')  // NAME 2 [key_manager]: the FAIL_IF's; NAME 1 [event]: neutral
	void Unknown_5418C0(GadgetGroupToFrontEvent *event, void *sender);						// 0x5418C0 (the gadgets' GadgetGroupToFrontEvent callback, FAIL_IF '!ssl')  // NAME 1 [event, sender]: neutral
	void Unknown_541930();																	// 0x541930 (FAIL_IF '!ssl': the static sort list's 0x6DB950)
#endif
	bool Is_Active() { return(Active); }													// (ui_group.cpp's asserts 'Get_Interface()->Is_Active()')  // NAME 3: the asserts'
	bool Is_Partially_Deactivated() { return(PartialDeactivatedGadget != NULL); }			// (ui_group.cpp's G_ASSERT line 403)  // NAME 3: the assert's

	LineDisplayClientClass *Client;															// +0x40 (ui_line_display.cpp's assert's name; the constructor: the view's slot +0x20's +0x4C)
	int Unknown44;																			// +0x44: nothing is shown while it is 0 (the view's scene, slot +0x50: referenced)
	UIViewClass *View;																		// +0x48 (the destructor's G_ASSERT '!View')  // NAME 3: the assert's
#ifdef __INTRFACE_MEMBERS
	// (typed here only for intrface.cpp until the windows' casts of the array, (X *)Unknown4C, take its address)
	InterfaceKeyListClass Unknown4C;														// +0x4C (0x721D50 / 0x721D80): what the control keys are registered with
	InterfaceUnknown58Class Unknown58;														// +0x58 (0x686B50 / 0x686B80; intrlist.cpp)
#else
	char Unknown4C[0x64 - 0x4C];															// +0x4C: what the control keys are registered with
#endif
	InterfaceUnknown64Class Unknown64;														// +0x64 (constructor 0x686FA0; ui_group.cpp takes its gadgets out of it, 0x687150)
#ifdef __INTRFACE_MEMBERS
	InterfaceKeyHandlerClass *Unknown70;													// +0x70 (0x0C bytes, table 0xAEEF70: the keys go to slot +0x68)
	int Unknown74;																			// +0x74
	InterfaceClass *Unknown78;																// +0x78 (this, or the parent's: 0x5417D0)
#else
	char Unknown70[0x7C - 0x70];															// +0x70
#endif
	bool Active;																			// +0x7C (FAIL_IF '!Active'; the views' slot +0x14 tests it)  // NAME 3: the FAIL_IF's
	GadgetClass *PartialDeactivatedGadget;															// +0x80 (0x541550 news it, 0x40 bytes, 0x660D30)  // NAME 3: the assert's
};

#ifdef __INTRFACE_MEMBERS
// What InterfaceClass holds at +0x70 (0x0C bytes, table 0xAEEF70: Delete_This, the deleting destructor 0x540CB0, 0x540C90):
// the interface's key handler, registered with the key list and +0x64
class InterfaceKeyHandlerClass : public RefCountClass  // NAME 1: neutral (D10)
{
public:
	InterfaceKeyHandlerClass(InterfaceClass *owner) { Owner = owner; }						// NAME 1 [owner]: neutral
	virtual bool Unknown_08(int key) { return(Owner->Unknown_68(key)); }					// +0x08 0x540C90  // NAME 1 [key]: InterfaceClass's

	InterfaceClass *Owner;																	// +0x08  // NAME 1: neutral (D10)
};
#endif

#endif // __INTRFACE_H
