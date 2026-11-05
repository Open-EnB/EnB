// Earth & Beyond client (reconstruction): app\Client\ui_ship_cust.cpp. Notes at the end of the file.
#include <vector>
#include <map>
#include <string>
#include "sclient.h"
#include "sbaseview.h"
#include "client.h"
#include "rendobj.h"
#include "texture.h"
#include "vertmaterial.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\shared\unknown_debug.h"
#include "..\shared\shipcust_desc.h"
#include "ui_shipcust.h"
#include "decal.h"

// The starbase's ship customisation window (the buy-ship pages 'fa_buy_ship01_ini' ..), as far as this file's
// functions so far use it. Its first base is the starbase windows' interface base (0xAC bytes, as
// ui_avatar_cust.cpp's StarbaseAvatarCreationInterface), its second the ship being edited (ui_shipcust.h's
// Unknown620D20Class, at +0xAC: these functions call its 0x6211D0 / 0x621390 with ecx = this + 0xAC).
class StarbaseShipCustomizeInterface : public Unknown5BB320Class, public Unknown620D20Class	// NAME 3: the client's RTTI (0xB89BB0, among this file's strings: SignalBaseClass::MemberCallback<GadgetClass,StarbaseShipCustomizeInterface,...>, the FlashingGadgetEvent handler's)
{
public:
	void Unknown_61B0F0();								// 0x61B0F0 (the parts' colours onto the model, then 0x61F180)
	void Unknown_61B130();								// 0x61B130 (the insignia onto the model, then 0x61F180)
	float *Unknown_61D180();							// 0x61D180 (the colour's palette position: as ui_shipcust.cpp's 0x627110)
	Vector3 *Unknown_61D1C0();							// 0x61D1C0 (the colour: as 0x627160)
	bool *Unknown_61D200();								// 0x61D200 (the setting's matte flag: as 0x6271A0)
	ShipCustomizationMetal *Unknown_61D230();			// 0x61D230 (the setting's metal: as 0x6271D0)
	void Unknown_61E8B0();								// 0x61E8B0 (the client's 0x74C960 for its avatar)
	void Unknown_61F180();								// 0x61F180 (after the model changes)

	SBaseViewClass *View;								// +0x230 (its Client's EnvironmentMapManager)  // NAME 1: neutral (D10)
	char Unknown234[0x2E8 - 0x234];						// +0x234
	int Page;											// +0x2E8 (1: the paint page, as ui_shipcust.cpp's +0x234)  // NAME 1: as ui_shipcust.h's
	char Unknown2EC[0x30C - 0x2EC];						// +0x2EC
	int Unknown30C;										// +0x30C the part
	int Unknown310;										// +0x310 the part's setting
	int Unknown314;										// +0x314
	int Unknown318;										// +0x318 the insignia
	char Unknown31C[0x330 - 0x31C];						// +0x31C
	SClientUnknown1334Class *Unknown330;				// +0x330 the decal library
};

// @0x61B0F0
void StarbaseShipCustomizeInterface::Unknown_61B0F0()
{
	Unknown_6211D0(View->Client->Peek_Environment_Map_Manager());
	Unknown_61F180();
}

// @0x61B130
void StarbaseShipCustomizeInterface::Unknown_61B130()
{
	Unknown_621390(Unknown318, Unknown330);
	Unknown_61F180();
}

// @0x61D180
float *StarbaseShipCustomizeInterface::Unknown_61D180()
{
	if (Page == 1) {
		return(&Desc.Parts[Unknown30C].Settings[Unknown310].Unknown10);
	}
	return(&Desc.Unknown3C.X);
}

// @0x61D1C0
Vector3 *StarbaseShipCustomizeInterface::Unknown_61D1C0()
{
	if (Page == 1) {
		return(&Desc.Parts[Unknown30C].Settings[Unknown310].Color);
	}
	return(&Desc.Ship_Name_Color);
}

// @0x61D200
bool *StarbaseShipCustomizeInterface::Unknown_61D200()
{
	return(&Desc.Parts[Unknown30C].Settings[Unknown310].Is_Matte);
}

// @0x61D230
ShipCustomizationMetal *StarbaseShipCustomizeInterface::Unknown_61D230()
{
	return(&Desc.Parts[Unknown30C].Settings[Unknown310].Metal);
}

// NOT EXACT: register (0.62: the client loads the avatar id into eax, VC6 here into edx; the rest is the same)
// @0x61E8B0
void StarbaseShipCustomizeInterface::Unknown_61E8B0()
{
	SClient *client = View->Client;
	client->Unknown_74C960(client->AvatarID);
}

// Notes (CS21, 2026-11-05). The file is the client's ui_ship_cust.cpp (__FILE__ 0xB896B8, asserts at lines 712,
// 1108, 2017 and 2020: the functions holding them are not in yet, so nothing here sits on their lines). Its
// window: the class's functions from about 0x6186A0 (the destructor, called by the deleting one 0x618540) and the
// constructor 0x618CA0 to 0x61E8E0; 0x6181B0 ('BUY_SHIP01', called from sbaseview.cpp) may be its first.
// The class's members past the bases are placed only as far as these functions read them. Still out of line in
// ui_shipcust.cpp (interim, D137): decal.h's inlines whose client copies sit here (0x618A50, 0x618C10, 0x619280,
// 0x61DF40): they move here with their callers (the destructor 0x6186A0, the constructor 0x618CA0, 0x61DBF0).
