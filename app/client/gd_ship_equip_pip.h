// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\gd_ship_equip_pip.h (name assumed from gd_ship_equip_pip.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\Client\gd_ship_equip_pip.cpp'; the client holds no string of this header)
**
**	The gadget over the equipment window's ship PIP (ui_pda_equip.cpp makes it): gd_pip_scene.h's gadget with the
**	ship's PIP (FAIL_IF '!ShipPIP'); a drag with the first or second button turns the ship, the wheel zooms. Table
**	0xAFAE64 (29 slots: GadgetPipSceneClass's, +0x2C and +0x30 its own), constructor 0x66DA50, 0x58 bytes.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef GD_SHIP_EQUIP_PIP_H  // NAME 3 [gd_ship_equip_pip.h]: header name assumed from the client's gd_ship_equip_pip.cpp
#define GD_SHIP_EQUIP_PIP_H

#include "gd_pip_scene.h"

class ShipEquipPipClass;	// ship_equip_pip.h's: the ship's PIP

class GadgetShipEquipPipClass : public GadgetPipSceneClass  // NAME 2: class-from-file (D24) for app\Client\gd_ship_equip_pip.cpp, as gd_pip_scene.cpp's GadgetPipSceneClass
{
public:
	GadgetShipEquipPipClass(ShipEquipPipClass *pip);	// 0x66DA50  // NAME 1 [pip]: the base's
	virtual ~GadgetShipEquipPipClass();					// +0x00 0x66DAC0 (deleting 0x66DA90)
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x2C 0x66DB10 (FAIL_IF '!ShipPIP' l.61, G_ASSERT 'false' l.123: buttons 1 / 5 hold the gadget, a drag turns the ship, the wheel zooms)  // NAME 1 [event, x, y, mask]: GadgetClass's
	virtual bool Unknown_30(float x, float y);			// +0x30 0x66DAE0 (the point kept, then the base's)  // NAME 1 [x, y]: GadgetClass's

	void Unknown_66DE10();								// 0x66DE10 (FAIL_IF '!ShipPIP' l.142: the first button's drag; inline in slot +0x2C, no caller)
	void Unknown_66DED0();								// 0x66DED0 (FAIL_IF '!ShipPIP' l.161: the second button's drag, the zoom by its vertical move)

	ShipEquipPipClass *ShipPIP;						// +0x4C (FAIL_IF '!ShipPIP')  // NAME 3: the FAIL_IFs'
	Vector2 Point;										// +0x50 (slot +0x30's x, y; 0x542A10 gets it)  // NAME 1: what slot +0x30 stores
};

#endif
