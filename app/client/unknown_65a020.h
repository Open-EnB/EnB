// Earth & Beyond client (reconstruction): app\client\unknown_65a020.h, the starbase's web window over the mesh 'JWEB00'
// (the object 0x65A020-0x65A6C0 between ui_verb.cpp and uiview.cpp; it has no assert, so the client names neither the
// file nor the class: interim placement, D137, NAME 1). Laid out from the constructor 0x65A020 (NF65, 2026-11-17).
#ifndef UNKNOWN_65A020_H  // NAME 1 [unknown_65a020.h]: provisional name (D137): the client's file sorts between ui_verb.cpp and uiview.cpp
#define UNKNOWN_65A020_H

#include "sclient.h"

class ButtonGadgetClass;		// unknown_675120.h's buttons (+0xAC)

// The web window (0xB8 bytes, tables 0xAF96B0 / 0xAF96A4 / 0xAF9698; made by 0x69F8F0, sbaseview.cpp, case 9: the
// starbase view's +0x198). It opens the settings store's browser 3 (BROWSER_PIT) over its mesh's 'BROWSER' box 20
// frames after it is shown, and shuts it when it is hidden.
class Unknown65A020Class : public Unknown5BB320Class	// NAME 1: neutral (D10)
{
public:
	Unknown65A020Class(SpaceClientClass *client);				// 0x65A020 (the starbase view's 0x69F8F0, case 9)  // NAME 1 [client]: what 0x69F8F0 passes
	virtual ~Unknown65A020Class();						// 0x65A160 (slot +0x5C's body inline; deleting 0x65A130)
	virtual void Unknown_54();							// 0x65A250 (the buttons 'exit' / 'exit_button', Escape, the back 'Box01')
	virtual void Unknown_58();							// 0x65A4A0 (the browser opened 20 frames on)
	virtual void Unknown_5C();							// 0x65A4E0 (the browser shut)
	virtual void Unknown_60();							// 0x65A530 (the countdown: the browser opened over 'BROWSER')
	virtual bool Unknown_68(int key);					// 0x65A620 (1, 2: back to the starbase view, 0x69F440)  // NAME 1 [key]: InterfaceClass's

	ButtonGadgetClass *Unknown0AC[2];					// +0x0AC slot +0x54's buttons ('exit', 'exit_button')
	int Unknown0B4;										// +0x0B4 frames until the browser opens (slot +0x58 sets 20)
};

#endif // UNKNOWN_65A020_H
