// Earth & Beyond client (reconstruction): app\client\ui_cinfo.cpp, the character info window (the client's file per its
// asserts, 'e:\build\beta\G\app\Client\ui_cinfo.cpp'). Notes at the end of the file.
#include "sclient.h"
#include "..\..\wwlib\ref_ptr.h"
#include "ui_bio.h"
#include "ui_cinfo.h"

// What InterfaceClass's +0x40 points to, as far as this file calls it (as ui_avatar_cust.cpp declares it)
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (as ui_manu.cpp's)  // NAME 3: the FAIL_IF's in ui_manu.cpp
};

// The window's first three tabs, as far as this file makes them (their files are not known)
class Unknown549640Class : public Unknown5BB320Class	// NAME 1: neutral (D10)
{
public:
	Unknown549640Class(SClient *client);				// 0x549640 (0x124 bytes)  // NAME 1 [client]: as UIBioClass's
	char UnknownAC[0x124 - 0xAC];						// +0xAC
};
class Unknown5C4170Class : public Unknown5BB320Class	// NAME 1: neutral (D10)
{
public:
	Unknown5C4170Class(SClient *client);				// 0x5C4170 (0xEC bytes)  // NAME 1 [client]: as UIBioClass's
	char UnknownAC[0xEC - 0xAC];						// +0xAC
};
class Unknown58D080Class : public Unknown5BB320Class	// NAME 1: neutral (D10)
{
public:
	Unknown58D080Class(SClient *client);				// 0x58D080 (0xE4 bytes)  // NAME 1 [client]: as UIBioClass's
	char UnknownAC[0xE4 - 0xAC];						// +0xAC
};

// 0x5738B0
void UICinfoClass::Unknown_54()
{
	InterfaceClass::Unknown_54();
	UnknownB4 = Create_NEW(new Unknown549640Class(Client->As_Space_Client()));
	UnknownB4->Unknown_5417D0(this);
	UnknownB4->Unknown_54();
	UnknownB8 = Create_NEW(new Unknown5C4170Class(Client->As_Space_Client()));
	UnknownB8->Unknown_5417D0(this);
	UnknownB8->Unknown_54();
	UnknownBC = Create_NEW(new Unknown58D080Class(Client->As_Space_Client()));
	UnknownBC->Unknown_5417D0(this);
	UnknownBC->Unknown_54();
	UnknownC0 = Create_NEW(new UIBioClass(Client->As_Space_Client()));
	UnknownC0->Unknown_5417D0(this);
	UnknownC0->Unknown_54();
}

/*
**	Notes (BF1, 2026-11-05)
**
**	The client's object starts at the constructor 0x5734D0 (its 'fa_cinfo.ini' and the 'TSI15A' button names come before
**	the __FILE__ string 0xB7DCA4, first used by the asserts of 0x573E80; ui_chat_window_split.cpp's code ends before it)
**	and runs to about 0x5746C0 (ui_class.cpp's 0x574710 follows). Only the tabs' builder 0x5738B0 (slot +0x54) is
**	written so far; the constructor, the destructor 0x573770 (deleting 0x573740), 0x573C70, the slots +0x58 0x573CE0,
**	+0x5C 0x573D50, +0x60 0x573DA0, +0x68 0x573DC0, the asserted 0x573E80 / 0x574220 / 0x5742B0 and 0x5741A0 are not.
**	After them the COMDATs: Create_NEW for each tab (0x5743A0, 0x574410, 0x574480, 0x5744F0) and the adjustor thunks.
*/
