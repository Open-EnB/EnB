// Earth & Beyond client (reconstruction): app\client\ui_pda_equip.h, the PDA's equipment window (the client's
// ui_pda_equip.cpp per its asserts, 'e:\build\beta\G\app\Client\ui_pda_equip.cpp' at 0xB83B00; this header's name is
// provisional, NAME 1, D137). Laid out from the constructor 0x5D10C0, the destructor 0x5D1A00 and the set-up 0x5D1250
// (PW1, 2026-11-09).
#ifndef UI_PDA_EQUIP_H  // NAME 1 [ui_pda_equip.h]: provisional header name for ui_pda_equip.cpp
#define UI_PDA_EQUIP_H

#include <vector>
#include <string>
#include "ui_inv.h"		// UIInvClass, ui_inv.cpp's inventory window base
#include "pip_scene.h"	// PipSceneClass, the ship's PIP's base
#include "gadget.h"
#include "gd_ship_equip_pip.h"	// GadgetShipEquipPipClass, the ship's PIP's gadget
#include "..\..\wwlib\ref_ptr.h"

class SClient;
class RenderObjClass;
class ButtonGadgetClass;		// unknown_675120.h's buttons

// D137 interim placement (PW1, 2026-11-09): one of the classes the equipment window makes, not rebuilt; its file by
// its asserts' neighbourhood is not confirmed (triage's hint: ship_equip_pip.cpp; the stats display is O18's
// ship_stats_display.h; the
// third, the PIP's gadget, is gd_ship_equip_pip.h's). Declared as far as ui_pda_equip.cpp and its gadget call them.

// The ship's PIP (0xB0 bytes, constructor 0x541DE0; a PipSceneClass)
class Unknown541DE0Class : public PipSceneClass	// NAME 1: neutral (D10)
{
public:
	Unknown541DE0Class(SClient *client);				// 0x541DE0 (not rebuilt)  // NAME 1 [client]: what the window passes
	void Unknown_542250(const RefCountPtr<RenderObjClass> &robj);	// 0x542250 (not rebuilt: the ship's model, 0x5D2B90)  // NAME 1 [robj]: neutral
	void Unknown_542A10(const Vector2 &point, bool on);	// 0x542A10 (not rebuilt: gd_ship_equip_pip.cpp's drag, from the point)  // NAME 1 [point, on]: neutral
	void Unknown_542EE0(float zoom);					// 0x542EE0 (not rebuilt: gd_ship_equip_pip.cpp's wheel, +-0.05)  // NAME 1 [zoom]: what the wheel passes
	char Unknown34[0xB0 - 0x34];
};

// The ship's statistics on the window: ship_stats_display.h's
class ShipStatsDisplayClass;

// One mount of the ship on the window (0x10 bytes; 0x5D2C50 news it from an 'Inventory.Mounts' element)
struct UnknownMountSlotStruct	// NAME 1: neutral (D10)
{
	int Type;											// +0x00 (the element's I00: 4 shield, 0x80 power, 0x40 engine, 0x1C022 a weapon's bits)  // NAME 2: what 0x5D2210 tests
	int Unknown04;										// +0x04 (the element's I08)  // NAME 1: neutral (D10)
	int Unknown08;										// +0x08 (the element's I04)  // NAME 1: neutral (D10)
	int Index;											// +0x0C (the element's index: the slot)  // NAME 2: what it holds
};

// The equipment window (0x154 bytes, tables 0xAF4C74 / 0xAF4C68 / 0xAF4C5C; pdaint.cpp's set-up 0x693230 makes it: the key
// 'Equipment', the PDA's window 1): the 'tsi07a' mesh, six weapon, six device and three component slots, the stats panel,
// the ship's PIP and its name
class UIPdaEquipClass : public UIInvClass	// NAME 3: class-from-file (D24) for app\Client\ui_pda_equip.cpp
{
public:
	enum { NUM_WEAPONS = 6, NUM_DEVICES = 6, NUM_COMPONENTS = 3, NUM_FRAMES = 15 };	// NAME 1: the set-up's loops ('w%02d', 's%02d', 'c%02d')
	enum { LAST_WEAPON_SLOT = 5, FIRST_SYSTEM_SLOT = 6, LAST_SYSTEM_SLOT = 11, SHIELD_SLOT = 12, POWER_SLOT = 13, ENGINE_SLOT = 14 };	// NAME 3: the asserts' (the last three: 0x5D2850's names)
	UIPdaEquipClass(SClient *client);					// 0x5D10C0 (the kind 11; deleting 0x5D1220)  // NAME 1 [client]: what the PDA passes
	virtual ~UIPdaEquipClass();							// 0x5D1A00
	virtual void Unknown_54();							// +0x54 0x5D1250 (FAIL_IF '!InterfaceMesh' l.110)
	virtual void Unknown_58();							// +0x58 0x5D1CA0 (FAIL_IF '!InterfaceMesh' l.221, G_ASSERT 'attach' l.235)
	virtual void Unknown_5C();							// +0x5C 0x5D20E0 (FAIL_IF '!InterfaceMesh' l.260)
	virtual void Unknown_60();							// +0x60 0x5D2150 (every frame)
	virtual bool Unknown_68(int key);					// +0x68 0x5D2180 (FAIL_IF '!InterfaceMesh' l.277: 16 the stats, 17 exit)  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// +0x7C 0x5D1C60 (the base's, the ship's attach point)  // NAME 1 [view, arg]: InterfaceClass's
	virtual void Unknown_A4(std::string &name, int index, int bttn_idx);	// +0xA4 0x5D2850 (G_ASSERT l.417: 'WEAPON_SLOT', 'SYSTEM_SLOT' and '_%d', 'ENGINE_SLOT', 'POWER_SLOT', 'SHIELD_SLOT')  // NAME 3 [bttn_idx]: the assert's; NAME 1 [name, index]: the base's
	virtual void Unknown_B0();							// +0xB0 0x5D2210 (FAIL_IFs l.298, 340, 344, 363, 367, 373, 386: the mounts onto the buttons, the future slots)
	void Unknown_5D2B90();								// 0x5D2B90 (the ship's model into the PIP)
	void Unknown_5D2C50();								// 0x5D2C50 (FAIL_IF '!Get_Client()' l.451: Mounts from 'Inventory.Mounts')
	LineDisplayClientClass *Get_Client() { return(Client); }	// (inline: the FAIL_IF '!Get_Client()')  // NAME 3: the FAIL_IF's

	RefCountPtr<RenderObjClass> InterfaceMesh;			// +0xD0 ('tsi07a'; FAIL_IF '!InterfaceMesh')  // NAME 3: the FAIL_IF's
	Unknown541DE0Class *ShipPip;						// +0xD4 (the ship's PIP)  // NAME 2: what it shows
	std::vector<UnknownMountSlotStruct *> Mounts;		// +0xD8 (0x5D2C50: one per 'Inventory.Mounts' element)  // NAME 2: what it holds
	RefCountPtr<RenderObjClass> Frames[NUM_FRAMES];		// +0xE8 ('w%02df' 0..5, 's%02df' 6..11, 'c%02df' 12..14: shown with a mount)  // NAME 2: what they are
	int Unknown124;									// +0x124 (zeroed)  // NAME 1: neutral (D10)
	int Unknown128;									// +0x128 (zeroed)  // NAME 1: neutral (D10)
	int Unknown12C;									// +0x12C (zeroed)  // NAME 1: neutral (D10)
	int Unknown130;									// +0x130 (zeroed)  // NAME 1: neutral (D10)
	int Unknown134;									// +0x134 (zeroed)  // NAME 1: neutral (D10)
	int Unknown138;									// +0x138 (zeroed)  // NAME 1: neutral (D10)
	int Unknown13C;									// +0x13C (zeroed)  // NAME 1: neutral (D10)
	int Unknown140;									// +0x140 (zeroed)  // NAME 1: neutral (D10)
	int Unknown144;									// +0x144 (zeroed)  // NAME 1: neutral (D10)
	RefCountPtr<RenderObjClass> Unknown148;				// +0x148  // NAME 1: neutral (D10)
	ShipStatsDisplayClass *Stats;							// +0x14C (the 'STATS_PANEL')  // NAME 2: its tooltip's
	ButtonGadgetClass *Exit;							// +0x150 ('exit', gadget 17)  // NAME 2: its sub-object's name
};

#endif // UI_PDA_EQUIP_H
