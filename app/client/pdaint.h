// Earth & Beyond client (reconstruction): app\client\pdaint.h, the PDA (the client's pdaint.cpp per its asserts,
// 'e:\build\beta\G\app\Client\pdaint.cpp'; this header's name is provisional, NAME 1). Laid out from the constructor
// 0x693140, the destructor 0x693950 and the set-up 0x693230 (PD1, 2026-11-08).
//
// sclient.h holds a stand-in view of this class for the files that only call it (SClient, the main view, the
// windows): it is skipped where this header comes first (#ifndef PDAINT_H), so pdaint.cpp includes this one first.
#ifndef PDAINT_H  // NAME 1 [pdaint.h]: provisional header name for pdaint.cpp
#define PDAINT_H

#include "intrface.h"
#include "ui_inv.h"			// UIInvClass, the base of the cargo, equipment and vault windows
#include "ui_pda_cargo.h"	// UIPdaCargoClass, the cargo window (window 0)
#include "ui_pda_equip.h"	// UIPdaEquipClass, the equipment window (window 1)
#include "..\..\wwlib\ref_ptr.h"

class SClient;
class RenderObjClass;		// ww3d2's
class ButtonGadgetClass;	// unknown_675120.h's
class ControlKeysManager;	// control_keys_manager.h's
class UICinfoClass;			// ui_cinfo.h's character window (0x5734D0)
class UIStarmapClass;		// ui_starmap.h's star map window (0x7C63D0)
class UIPingClass;			// ui_ping.h's ping window (0x5D7630)

#ifdef POC_WINDOW_CTOR	// the POC (poc_world3.cpp) keeps InterfaceClass's own slots for its empty windows
#define POC_INV_WINDOW_SLOTS virtual void Unknown_54() { InterfaceClass::Unknown_54(); } virtual void Unknown_58() { InterfaceClass::Unknown_58(); } virtual void Unknown_5C() { InterfaceClass::Unknown_5C(); } virtual void Unknown_60() { InterfaceClass::Unknown_60(); } virtual void Unknown_64() { InterfaceClass::Unknown_64(); } virtual bool Unknown_68(int key) { return(InterfaceClass::Unknown_68(key)); } virtual void Unknown_7C(UIViewClass *view, bool arg) { InterfaceClass::Unknown_7C(view, arg); }
#else
#define POC_INV_WINDOW_SLOTS
#endif

// The vault window (0xFC bytes, constructor 0x656FF0; the key 'Vault', window 3)
class Unknown656FF0Class : public UIInvClass	// NAME 1: neutral (D10)
{
public:
	POC_INV_WINDOW_SLOTS
	Unknown656FF0Class(SClient *client);				// 0x656FF0  // NAME 1 [client]: neutral
	char UnknownD0[0xFC - 0xD0];
};

// The PDA's experience display (0x28 bytes, constructor 0x58C450, ui_experience.cpp): slot +0x08 takes the client's
// slot +0x28 when +0x04 differs, slot +0x10 every frame
class Unknown58C450Class	// NAME 1: neutral (D10)
{
public:
	Unknown58C450Class(RenderObjClass *mesh, InterfaceClass *parent);	// 0x58C450 (the PDA, 0x693230; ui_attrb.cpp's panel, 0x546DD0)  // NAME 1 [mesh, parent]: what its callers pass
	virtual ~Unknown58C450Class();
	virtual void Unknown_04();
	virtual void Unknown_08(int value);					// +0x08  // NAME 1 [value]: neutral
	virtual void Unknown_0C();
	virtual void Unknown_10();							// +0x10 (every frame)
	void Unknown_58CF60();								// 0x58CF60 (before its delete)

	int Get_Unknown04() { return(Unknown04); }			// (inline: 0x693CA0 loads +0x04 before the client's slot +0x28 call)  // NAME 1: neutral
	int Unknown04;										// +0x04
	char Unknown08[0x28 - 0x08];
};

// pdaint.cpp's PDA (SClient +0x1284; Init news it, 0xC0 bytes; tables 0xAFD148 / 0xAFD13C / 0xAFD130): the 'TSI14A'
// mesh with four buttons, and its windows 0 cargo, 1 equipment, 2 character, 3 vault, 4 star map (0x6956C0), the ping
// window and the experience display
class PDAInterfaceClass : public InterfaceClass	// NAME 3: class-from-file (D24) for app\Client\pdaint.cpp
{
public:
	enum { NUM_BUTTONS = 4 };							// NAME 1: the set-up's loop over 0xB90A38
	PDAInterfaceClass(SClient *client);					// 0x693140  // NAME 1 [client]: neutral
	virtual ~PDAInterfaceClass();						// 0x693950 (deleting 0x693200)
	virtual void Unknown_54();							// +0x54 0x693230 (the mesh, the windows, the buttons)
	virtual void Unknown_58();							// +0x58 0x694AD0 (the base's)
	virtual void Unknown_5C();							// +0x5C 0x694AF0 (the base's when Active)
	virtual void Unknown_60();							// +0x60 0x693CA0 (every frame: the experience, the window asked for)
	virtual bool Unknown_68(int key);					// +0x68 0x693E70 (buttons 1..4, keys 6..13)  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_70(ControlKeysManager *manager);	// +0x70 0x694BE0 (FAIL_IF '!manager')  // NAME 3 [manager]: the FAIL_IF's
	virtual void Unknown_74(ControlKeysManager *manager);	// +0x74 0x695110 (FAIL_IF '!manager')  // NAME 3 [manager]: the FAIL_IF's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// +0x7C 0x694B10 (the windows onto the view)  // NAME 1 [view, arg]: InterfaceClass's
	RefCountPtr<UIPdaCargoClass> Unknown_695630();	// 0x695630 (the cargo window when Active)
	RefCountPtr<UIPdaCargoClass> Unknown_695680();	// 0x695680 (the cargo window)
	RefCountPtr<InterfaceClass> Unknown_6956C0(int index);	// 0x6956C0 (G_ASSERT 'false' past 4)  // NAME 1 [index]: neutral
	bool Unknown_695780(int index);						// 0x695780 (asks for window 'index', the others closed)  // NAME 1 [index]: neutral
	bool Unknown_695A00(int index, bool arg);			// 0x695A00 (closes window 'index')  // NAME 1 [index, arg]: neutral
	bool Unknown_695C50(int index);						// 0x695C50 (window 'index' is Active)  // NAME 1 [index]: neutral
	bool Unknown_695CE0();								// 0x695CE0 (any window Active or asked for)
	bool Unknown_695D70(int index);						// 0x695D70 (toggles window 'index')  // NAME 1 [index]: neutral
	void Unknown_695DC0(bool on);						// 0x695DC0 (the cargo window's 0x5D0A20, then open or close it)  // NAME 1 [on]: neutral
	void Unknown_695E10(int type, int *num_items, int *num_slots);	// 0x695E10 (the three inventories' sums)  // NAME 3 [num_items, num_slots]: the FAIL_IFs'; NAME 1 [type]: neutral
	void Unknown_695F40();								// 0x695F40 (SClient 0x742530, ui_chat_panel.cpp, on every change of view: all closed)
	void Unknown_695FE0(bool on);						// 0x695FE0 (+0xBC; the starbase view's 0x69CDE0 passes true)  // NAME 1 [on]: neutral
	LineDisplayClientClass *Get_Client() { return(Client); }	// (inline: the FAIL_IFs '!Get_Client()')  // NAME 3: the FAIL_IFs'

	ButtonGadgetClass *Buttons[NUM_BUTTONS];			// +0x84 ('MAP_BUT', 'CHARACTER_BUT', 'SHIP_BUT', 'OPTIONS_BUT': gadgets 1..4)  // NAME 1: neutral
	RefCountPtr<RenderObjClass> InterfaceMesh;			// +0x94 ('TSI14A')  // NAME 1: as ui_starmap.h's
	RefCountPtr<UIPdaEquipClass> Equipment;			// +0x98 (window 1)  // NAME 2: its key's
	RefCountPtr<UIPdaCargoClass> Cargo;				// +0x9C (window 0)  // NAME 2: its key's
	RefCountPtr<Unknown656FF0Class> Vault;				// +0xA0 (window 3)  // NAME 2: its key's
	RefCountPtr<UICinfoClass> Character;				// +0xA4 (window 2)  // NAME 2: its key's
	RefCountPtr<UIStarmapClass> Starmap;				// +0xA8 (window 4; the main view's 0x512A00 calls its 0x7C8DB0)  // NAME 1: its class's
	RefCountPtr<InterfaceClass> OpenedCargoInterface;	// +0xAC (the equipment or vault window opened with the cargo window)  // NAME 3: the assert's
	RefCountPtr<InterfaceClass> UnknownB0;				// +0xB0 (the window asked for: Active on the next frame without a bridge request)
	Unknown58C450Class *Experience;						// +0xB4 (0x58C450)  // NAME 1: its file's
	RefCountPtr<UIPingClass> Ping;						// +0xB8 (the key 'Toggle Ping')  // NAME 2: its key's
	bool UnknownBC;										// +0xBC (the keys taken; the constructor: true)
};

#endif // PDAINT_H
