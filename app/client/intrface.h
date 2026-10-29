/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\intrface.h (name assumed from intrface.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\client\intrface.cpp'; the client holds no string of this header)
**
**	The UI kit's interface base (kernel-map-step9.md section 3.2): an inline base with tables at +0x00 and
**	+0x04, gclass.cpp's class over it (e:\build\beta\G\app\shared\gclass.cpp, constructor 0x4A1880), and
**	intrface.cpp's InterfaceClass (constructors 0x540A90 / 0x540D30, tables 0xAEEF98 / 0xAEEF8C, a
**	RefCountClass at +0x38 with table 0xAEEF80). Every screen and window of the client derives from it:
**	the views (view.cpp) keep them in a list, reference counted. Declared as far as the rebuilt files use
**	it (D10); one class for what sclient.h (UIUnknownBaseClass, UIInterfaceBaseClass) and
**	ui_line_display.h (GClassUnknownClass, InterfaceClass) declared apart until UI1 (P-LD4-1).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __INTRFACE_H  // NAME 3 [intrface.h]: header name assumed from the client's intrface.cpp
#define __INTRFACE_H

#include "..\..\wwlib\refcount.h"

class UIViewClass;
class INIClass;
class ControlKeysManagerClass;  // control_keys_manager.cpp's (0x71A960 / 0x71AB40)
class LineDisplayClientClass;  // what +0x40 points to (slot +0x18 gives the sclient)

// The inline base under gclass.cpp's class (tables 0xAEAB48 at +0x00, 0xAEAB3C at +0x04)
class UnknownGadgetBase00Class  // NAME 1: neutral (D10): the inline base's first table (0xAEAB48)
{
public:
	virtual ~UnknownGadgetBase00Class();
	virtual void Unknown_04();
	virtual void Unknown_08();
	virtual void Unknown_0C();
	virtual void Unknown_10();
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
	virtual void Unknown_40();
	virtual void Unknown_44();
	virtual void Unknown_48();
	virtual void Unknown_4C();
	virtual void Unknown_50();
};

// The second table (two slots; its +0x04 is 0x4A1930 in every derived table, intrface.cpp's 0xAEEF8C included,
// so it is named for its address: a derived class's Unknown_04 overrides only the first table's)
class UnknownGadgetBase04Class  // NAME 1: neutral (D10): the inline base's second table (0xAEAB3C)
{
public:
	virtual ~UnknownGadgetBase04Class();
	virtual void Unknown_4A1930();															// +0x04 0x4A1930  // NAME 1: neutral (D10): its address
};

class GClassUnknownClass : public UnknownGadgetBase00Class, public UnknownGadgetBase04Class  // NAME 1: neutral (D10): gclass.cpp's class (constructor 0x4A1880)
{
public:
	char Unknown08[0x38 - 0x08];															// +0x08
};

// intrface.cpp's class (0x84 bytes; 34 slots in 0xAEEF98): +0x00..+0x50 the gclass run 0x4A1800-0x4A2010
// (+0x44 / +0x48 gclass.cpp's two asserted functions), +0x64..+0x78 the one-line defaults 0x515690-0x515750
// emitted next to smouse.cpp, +0x58 / +0x5C / +0x60 / +0x7C / +0x80 / +0x84 intrface.cpp's (activate /
// deactivate). The views release them through the RefCountClass at +0x38 (dec [p+0x3C], then [[p+0x38]]'s slot 0)
class InterfaceClass : public GClassUnknownClass, public RefCountClass  // NAME 3: by convention (D24) from the client's intrface.cpp (constructors 0x540A90 / 0x540D30, table 0xAEEF98)
{
public:
	InterfaceClass(UIViewClass *view, int unknown);										// 0x540A90 (+0x48 the view, +0x44 the second, referenced)  // NAME 2 [view]: the member's assert '!View'; NAME 1 [unknown]: neutral
	virtual ~InterfaceClass();																// 0x540E90 (slot 0x540D00; G_ASSERT '!View')
	virtual void Unknown_54();																// 0x515690 (the slot +0x10); Init calls it after construction
	virtual void Unknown_58();																// 0x541010 (activates: G_ASSERT '!PartialDeactivatedGadget')
	virtual void Unknown_5C();																// 0x541210 (deactivates when Active)
	virtual void Unknown_60();																// 0x540C70 (the views' slot +0x14 under a sort object when Active)
	virtual void Unknown_64();																// 0x5156B0 (the views' slot +0x14 otherwise)
	virtual bool Unknown_68(int key);														// 0x5156D0 (false)
	virtual void Unknown_6C(INIClass &ini);													// 0x5156F0 (the views' 0x6B4900: SClient 0x745080 saves settings.ini through it)  // NAME 1 [ini]: what 0x745080 passes
	virtual void Unknown_70(ControlKeysManagerClass *manager);								// 0x515710
	virtual void Unknown_74(ControlKeysManagerClass *manager);								// 0x515730
	virtual bool Unknown_78(void *arg);														// 0x515750 (false; tsk_login.h's screens: the global error packet)  // NAME 1 [arg]: neutral
	virtual void Unknown_7C(UIViewClass *view, int arg);									// 0x5412F0 (onto the view, off the old one; the views' slot +0x10 passes NULL, 0)  // NAME 1 [view, arg]: what the callers pass
	virtual void Unknown_80();																// 0x541550 (FAIL_IF 'PartialDeactivatedGadget', '!Active')
	virtual void Unknown_84();																// 0x541690 (FAIL_IF '!PartialDeactivatedGadget', '!Active')
	void *Unknown_541800();																	// 0x541800

	LineDisplayClientClass *Client;															// +0x40 (ui_line_display.cpp's assert's name; the constructor: the view's slot +0x20's +0x4C)
	int Unknown44;																			// +0x44: nothing is shown while it is 0
	UIViewClass *View;																		// +0x48 (the destructor's G_ASSERT '!View')  // NAME 3: the assert's
	char Unknown4C[0x7C - 0x4C];															// +0x4C: what the control keys are registered with
	bool Active;																			// +0x7C (FAIL_IF '!Active'; the views' slot +0x14 tests it)  // NAME 3: the FAIL_IF's
	void *PartialDeactivatedGadget;															// +0x80 (0x541550 news it, 0x40 bytes, 0x660D30)  // NAME 3: the assert's
};

#endif // __INTRFACE_H
