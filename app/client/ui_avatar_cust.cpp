// Earth & Beyond client (reconstruction): app\Client\ui_avatar_cust.cpp. Notes at the end of the file.
#include "sclient.h"

// gadget.cpp's gadget, as far as this file calls it
class GadgetClass	// NAME 3: class-from-file (D24) for app\client\gadget.cpp
{
public:
	void Unknown_6603A0(int arg, UIViewClass *view);	// 0x6603A0  // NAME 1 [arg, view]: neutral
	void Unknown_6607D0();								// 0x6607D0

	char Unknown00[0x2C];								// +0x00
	unsigned int Unknown2CLow : 11;						// +0x2C
	unsigned int Unknown2C800 : 1;						// +0x2C bit 0x800 (0x550670 sets it with 0x6603A0, clears it with 0x6607D0)
};

// avatar.cpp's avatar, as far as this file calls it (the window's +0xBC)
class UIAvatarCustAvatarClass	// NAME 1: neutral (D10)
{
public:
	void Unknown_780790(int part, int value);			// 0x780790 (0x550970: 1-4 for the window's parts 3-6)  // NAME 1 [part, value]: neutral
	void Unknown_780810(SClientUnknown1338Class *unknown);	// 0x780810 (0x550970 passes the client's Unknown1338)  // NAME 1 [unknown]: neutral
};

// The avatar customisation window (ui_avatar_cust.cpp's __FILE__), as far as this file's functions so far use it
class UIAvatarCustClass : public InterfaceClass	// NAME 3: class-from-file (D24)
{
public:
	void Unknown_550670();								// 0x550670 (the mode and part buttons' state)
	void Unknown_550970(int value);						// 0x550970  // NAME 1 [value]: neutral
	void Unknown_551BF0();								// 0x551BF0 (recustomization_cost.cpp's 0x8493F0)

	char Unknown84[0xB0 - 0x84];						// +0x84
	int Unknown0B0;										// +0xB0 (the mode: 1, 2)
	Unknown65B810ViewClass *Unknown0B4;					// +0xB4 (its Client's Unknown1338)
	char Unknown0B8[0xBC - 0xB8];						// +0xB8
	UIAvatarCustAvatarClass *Unknown0BC;				// +0xBC
	char Unknown0C0[0x1CC - 0xC0];						// +0xC0
	GadgetClass *Unknown1CC;							// +0x1CC (on in mode 1)
	GadgetClass *Unknown1D0;							// +0x1D0 (on in mode 2)
	char Unknown1D4[0x22C - 0x1D4];						// +0x1D4
	GadgetClass *Unknown22C;							// +0x22C (part 0)
	GadgetClass *Unknown230;							// +0x230 (part 1)
	GadgetClass *Unknown234;							// +0x234 (part 2)
	char Unknown238[0x250 - 0x238];						// +0x238
	GadgetClass *Unknown250;							// +0x250 (part 3)
	GadgetClass *Unknown254;							// +0x254 (part 4)
	GadgetClass *Unknown258;							// +0x258 (part 5)
	GadgetClass *Unknown25C;							// +0x25C (part 6)
	char Unknown260[0x358 - 0x260];						// +0x260
	int Unknown358;										// +0x358 (the part, 0-6)
	int Unknown35C;										// +0x35C
};

// 0x550670
void UIAvatarCustClass::Unknown_550670()
{
	if (Unknown0B0 == 1) {
		Unknown1CC->Unknown2C800 = true;
		Unknown1CC->Unknown_6603A0(0, NULL);
	} else {
		Unknown1CC->Unknown2C800 = false;
		Unknown1CC->Unknown_6607D0();
	}
	if (Unknown0B0 == 2) {
		Unknown1D0->Unknown2C800 = true;
		Unknown1D0->Unknown_6603A0(0, NULL);
	} else {
		Unknown1D0->Unknown2C800 = false;
		Unknown1D0->Unknown_6607D0();
	}
	if (Unknown358 == 0) {
		Unknown22C->Unknown_6607D0();
		Unknown22C->Unknown2C800 = true;
		Unknown22C->Unknown_6603A0(0, NULL);
	} else {
		Unknown22C->Unknown2C800 = false;
		Unknown22C->Unknown_6607D0();
	}
	if (Unknown358 == 1) {
		Unknown230->Unknown2C800 = true;
		Unknown230->Unknown_6603A0(0, NULL);
	} else {
		Unknown230->Unknown2C800 = false;
		Unknown230->Unknown_6607D0();
	}
	if (Unknown358 == 2) {
		Unknown234->Unknown2C800 = true;
		Unknown234->Unknown_6603A0(0, NULL);
	} else {
		Unknown234->Unknown2C800 = false;
		Unknown234->Unknown_6607D0();
	}
	if (Unknown358 == 3) {
		Unknown250->Unknown_6607D0();
		Unknown250->Unknown2C800 = true;
		Unknown250->Unknown_6603A0(0, NULL);
	} else {
		Unknown250->Unknown2C800 = false;
		Unknown250->Unknown_6607D0();
	}
	if (Unknown358 == 4) {
		Unknown254->Unknown2C800 = true;
		Unknown254->Unknown_6603A0(0, NULL);
	} else {
		Unknown254->Unknown2C800 = false;
		Unknown254->Unknown_6607D0();
	}
	if (Unknown358 == 5) {
		Unknown258->Unknown2C800 = true;
		Unknown258->Unknown_6603A0(0, NULL);
	} else {
		Unknown258->Unknown2C800 = false;
		Unknown258->Unknown_6607D0();
	}
	if (Unknown358 == 6) {
		Unknown25C->Unknown2C800 = true;
		Unknown25C->Unknown_6603A0(0, NULL);
	} else {
		Unknown25C->Unknown2C800 = false;
		Unknown25C->Unknown_6607D0();
	}
}

// 0x550970
void UIAvatarCustClass::Unknown_550970(int value)
{
	if (Unknown0B0 != 2) {
		return;
	}
	Unknown35C = value;
	switch (Unknown358) {
		case 3: Unknown0BC->Unknown_780790(1, value); break;
		case 4: Unknown0BC->Unknown_780790(2, value); break;
		case 5: Unknown0BC->Unknown_780790(3, value); break;
		case 6: Unknown0BC->Unknown_780790(4, value); break;
	}
	Unknown0BC->Unknown_780810(Unknown0B4->Get_Client()->Unknown1338);
	Unknown_551BF0();
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ui_avatar_cust.cpp (its __FILE__, 0x551320's FAIL_IFs on lines 2210 / 2214)
**
**	The avatar customisation window: so far 0x550670 and 0x550970 (TG4, claim #63). The file's
**	evidence: both sit between ui_attrb_skills.cpp's last functions (0x54B370) and the FAIL_IFs of
**	0x551320; their callers (0x54CAE0, 0x54CBC0, 0x54E610, 0x54FAA0, 0x550140) all lie in the same
**	stretch, and they call 0x551BF0 (after 0x551320) like 0x54CBC0, 0x54FAA0 and 0x550550 do. The
**	class is declared only as far as these two use it; the rest of the file (about 2,200 lines before
**	the FAIL_IFs) is not rebuilt. Not yet rebuilt in the claim: 0x550A10 (the models' turn, 0x6A8 bytes).
*/
