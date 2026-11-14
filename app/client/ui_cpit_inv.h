// Earth & Beyond client (reconstruction): app\client\ui_cpit_inv.h, the cockpit's inventory windows (the client's
// ui_cpit_inv.cpp per its FAIL_IFs, 'e:\build\beta\G\app\Client\ui_cpit_inv.cpp' at 0xB7EB18; this header's name is
// provisional, NAME 1, D137). Laid out from the constructor 0x5850E0 and the destructor 0x585210 (O15, 2026-11-13).
#ifndef UI_CPIT_INV_H  // NAME 1 [ui_cpit_inv.h]: provisional header name for ui_cpit_inv.cpp
#define UI_CPIT_INV_H

#include "ui_inv.h"

class SClient;
class UICpitClass;
class UIMeshClass;
class ButtonGadgetClass;
class Unknown669FB0Class;	// gd_inv_slider.h's

// The cockpit's hulk (+0xC8, kind 2) and harvest (+0xCC, kind 4) windows (0xE4 bytes, tables 0xAF0E80 / 0xAF0E74 /
// 0xAF0E68): an inventory window on the model 'PSI09A' with its eight slots 'PSI09A.s%02d', the button X_SPECIALS
// (id 9: the cockpit's 0x581CE0) and a slider (id 10); its animation 'PSI09A.PSI09A-1' opens and closes it
class UICpitInvClass : public UIInvClass	// NAME 3: class-from-file (D24) for app\Client\ui_cpit_inv.cpp
{
public:
	UICpitInvClass(UICpitClass *cpit, SClient *sclient, int kind);	// 0x5850E0 (the cockpit's 0x57DD20 passes 2 / 4)  // NAME 1 [cpit, sclient]: what the set-up passes; NAME 2 [kind]: UIInvClass's
	virtual ~UICpitInvClass();							// 0x585210 (deleting 0x5851E0)
	virtual void Unknown_54();							// 0x585500 (the model, its slots, the button and the slider; FAIL_IF '!robj' l.102)
	virtual void Unknown_58();							// 0x5852D0 (FAIL_IF '!View' l.76: the model into the view's scene, opened)
	virtual void Unknown_5C();							// 0x585440 (closed, the model out of its scene, then UIInvClass's)
	virtual void Unknown_60();							// 0x5857C0 (the animation's frame; the slider laid out when it ends, then UIInvClass's)
	virtual bool Unknown_68(int key);					// 0x585780 (9: the cockpit's 0x581CE0; 10: nothing)  // NAME 1 [key]: InterfaceClass's
	void Unknown_585840();								// 0x585840 (the animation turned: open / closed)

	UICpitClass *Cpit;									// +0xD0  // NAME 2: what it holds
	UIMeshClass *UIMesh;								// +0xD4 ('PSI09A'; the destructor deletes it)  // NAME 2: its type
	ButtonGadgetClass *Unknown0D8;						// +0xD8 ('X_SPECIALS', id 9)
	Unknown669FB0Class *Slider;							// +0xDC (id 10)  // NAME 2: its type (as ui_pda_cargo.cpp's)
	bool Unknown0E0;									// +0xE0 (open: 0x585840 turns it)
	bool Unknown0E1;									// +0xE1 (the animation running: 0x5857C0 clears it at its end)
};

#endif
