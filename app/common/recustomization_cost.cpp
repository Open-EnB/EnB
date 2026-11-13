// Earth & Beyond client (reconstruction): recustomization_cost.cpp (the client's __FILE__ e:\build\beta\G\app\Common\recustomization_cost.cpp),
// client 0x8492F0-0x84A256: the avatar's and the ship's recustomization costs (notes at the end).
#include "recustomization_cost.h"
#include "..\shared\avatar_desc.h"
#include "..\shared\shipcust_desc.h"
#include <math.h>
#include <string.h>
#include "..\shared\g_assert.h"

// 0x8492F0
RecustomizationCostClass::RecustomizationCostClass()
{
	memset(&Costs, 0, sizeof(Costs));
}

// 0x849320
RecustomizationCostClass::RecustomizationCostClass(const RecustomizationCostDataStruct &data)
{
	for (int slot = 0; slot < 14; slot++) {
		Costs.Per_Cost[slot] = data.Per_Cost[slot];
	}
}
























// 0x849360
int RecustomizationCostClass::Unknown_849360(int slot, AvatarDescClass *oldavatar, AvatarDescClass *newavatar)
{
	FAIL_IF(!oldavatar) return(0);
	FAIL_IF(!newavatar) return(0);
	FAIL_IF(Costs.Per_Cost[slot] == 0) return(0);
	return(Costs.Per_Cost[slot]);
}

// 0x8493F0
int RecustomizationCostClass::Unknown_8493F0(AvatarDescClass *oldavatar, AvatarDescClass *newavatar)
{

	FAIL_IF(!oldavatar) return(0);
	FAIL_IF(!newavatar) return(0);
	int cost = 0;
	for (int slot = 0; slot < 14; slot++) {
		if (Unknown_8494C0(slot, oldavatar, newavatar)) {
			cost += Unknown_849360(slot, oldavatar, newavatar);
		}
	}
	return(cost);
}

// A colour moved by more than the tolerance
static inline bool Color_Moved(const Vector3 &newcolor, const Vector3 &oldcolor)	// NAME 1: what it does
{
	if (fabs(oldcolor.X - newcolor.X) > 0.0001 || fabs(oldcolor.Y - newcolor.Y) > 0.0001 || fabs(oldcolor.Z - newcolor.Z) > 0.0001) return(true);
	return(false);
}

// A colour with its metal: the same metal (not none) is no change
static inline bool Metal_Changed(const Vector3 &newcolor, int newmetal, const Vector3 &oldcolor, int oldmetal)	// NAME 1: what it does
{
	if (oldmetal == newmetal && oldmetal != -1) return(false);
	if (oldcolor != newcolor || oldmetal != newmetal) return(true);
	return(false);
}

// 0x8494C0
bool RecustomizationCostClass::Unknown_8494C0(int slot, AvatarDescClass *oldavatar, AvatarDescClass *newavatar)
{
	switch (slot) {
		case 0:
			return((oldavatar->Hair_Num != newavatar->Hair_Num) ? true : false);
		case 1:
			return((oldavatar->Beard_Num != newavatar->Beard_Num) ? true : false);
		case 2:
			if (Color_Moved(newavatar->Hair_Color, oldavatar->Hair_Color)) return(true);
			break;
		case 3:
			if (Color_Moved(newavatar->Skin_Color, oldavatar->Skin_Color)) return(true);
			break;
		case 4:
			if (Color_Moved(newavatar->Eye_Color, oldavatar->Eye_Color)) return(true);
			break;
		case 5:
			if (oldavatar->Tattoo_Texture_Num != newavatar->Tattoo_Texture_Num || (oldavatar->Tattoo_Texture_Num != 0 && Color_Moved(newavatar->Tattoo_Offset, oldavatar->Tattoo_Offset))) return(true);
			break;
		case 6:
			return((oldavatar->Goggle_Num != newavatar->Goggle_Num) ? true : false);
		case 7:
			return((oldavatar->Ear_Num != newavatar->Ear_Num) ? true : false);
		case 8:
			return((oldavatar->Body_Type != newavatar->Body_Type) ? true : false);
		case 9:
			if (Metal_Changed(newavatar->Shirt_Primary_Color, newavatar->Shirt_Primary_Metal, oldavatar->Shirt_Primary_Color, oldavatar->Shirt_Primary_Metal)) return(true);
			break;
		case 10:
			if (Metal_Changed(newavatar->Shirt_Secondary_Color, newavatar->Shirt_Secondary_Metal, oldavatar->Shirt_Secondary_Color, oldavatar->Shirt_Secondary_Metal)) return(true);
			break;
		case 11:
			return((oldavatar->Pants_Type != newavatar->Pants_Type) ? true : false);
		case 12:
			if (Metal_Changed(newavatar->Pants_Primary_Color, newavatar->Pants_Primary_Metal, oldavatar->Pants_Primary_Color, oldavatar->Pants_Primary_Metal)) return(true);
			break;
		case 13:
			if (Metal_Changed(newavatar->Pants_Secondary_Color, newavatar->Pants_Secondary_Metal, oldavatar->Pants_Secondary_Color, oldavatar->Pants_Secondary_Metal)) return(true);
			break;
	}
	return(false);
}

// 0x849920
bool RecustomizationCostClass::Write(LinearBufferClass *buffer)
{
	for (int slot = 0; slot < 14; slot++) {
		buffer->Write(&Costs.Per_Cost[slot], sizeof(int));
	}
	return(true);
}

// 0x849960
bool RecustomizationCostClass::Read(LinearBufferClass *buffer)
{
	for (int slot = 0; slot < 14; slot++) {
		buffer->Read(&Costs.Per_Cost[slot], sizeof(int));
	}
	return(true);
}

// 0x8499A0
int RecustomizationCostClass::Get_Size()
{
	return(sizeof(Costs));
}

// 0x8499C0
ShipRecustomizationCostClass::ShipRecustomizationCostClass()
{
	memset(&Costs, 0, sizeof(Costs));
}

// 0x8499F0
ShipRecustomizationCostClass::ShipRecustomizationCostClass(const ShipRecustomizationCostDataStruct &data)
{
	for (int slot = 0; slot < 12; slot++) {
		Costs.Per_Cost[slot] = data.Per_Cost[slot];
	}
}
















































































// 0x849A30
int ShipRecustomizationCostClass::Unknown_849A30(int slot, ShipCustDescClass *oldship, ShipCustDescClass *newship)
{
	FAIL_IF(!oldship) return(0);
	FAIL_IF(!newship) return(0);
	FAIL_IF(Costs.Per_Cost[slot] == 0) return(0);
	return(Costs.Per_Cost[slot]);
}

// 0x849AC0
int ShipRecustomizationCostClass::Unknown_849AC0(ShipCustDescClass *oldship, ShipCustDescClass *newship)
{
	FAIL_IF(!oldship) return(0);
	FAIL_IF(!newship) return(0);
	int cost = 0;
	for (int slot = 0; slot < 12; slot++) {
		if (Unknown_849B90(slot, oldship, newship)) {
			cost += Unknown_849A30(slot, oldship, newship);
		}
	}
	return(cost);
}

// 0x849B90
bool ShipRecustomizationCostClass::Unknown_849B90(int slot, ShipCustDescClass *oldship, ShipCustDescClass *newship)
{
	switch (slot) {
		case 0:
			return((oldship->Hull_Style != newship->Hull_Style) ? true : false);
		case 1:
			return((oldship->Wing_Style != newship->Wing_Style) ? true : false);
		case 2:
			return(Unknown_84A150(oldship->Parts[0].Settings[0], newship->Parts[0].Settings[0]) ? true : false);
		case 3:
			return(Unknown_84A150(oldship->Parts[0].Settings[1], newship->Parts[0].Settings[1]) ? true : false);
		case 4:
			return(Unknown_84A150(oldship->Parts[2].Settings[0], newship->Parts[2].Settings[0]) ? true : false);
		case 5:
			return(Unknown_84A150(oldship->Parts[2].Settings[1], newship->Parts[2].Settings[1]) ? true : false);
		case 6:
			return(Unknown_84A150(oldship->Parts[1].Settings[0], newship->Parts[1].Settings[0]) ? true : false);
		case 7:
			return(Unknown_84A150(oldship->Parts[1].Settings[1], newship->Parts[1].Settings[1]) ? true : false);
		case 8:
			return(Unknown_84A150(oldship->Parts[3].Settings[0], newship->Parts[3].Settings[0]) ? true : false);
		case 9:
			return(Unknown_84A150(oldship->Parts[3].Settings[1], newship->Parts[3].Settings[1]) ? true : false);
		case 10:
			if (strncmp(oldship->Ship_Name, newship->Ship_Name, 0x19) != 0 || oldship->Ship_Name_Color != newship->Ship_Name_Color) return(true);
			break;
		case 11:
			return((oldship->Selected_Decal != newship->Selected_Decal) ? true : false);
	}
	return(false);
}

// 0x84A150
bool ShipRecustomizationCostClass::Unknown_84A150(ShipCustSettingsClass oldsettings, ShipCustSettingsClass newsettings)
{
	if (oldsettings.Metal == newsettings.Metal && oldsettings.Metal != METAL_NONE) return(false);
	if (oldsettings.Color != newsettings.Color || oldsettings.Is_Matte != newsettings.Is_Matte || oldsettings.Metal != newsettings.Metal) return(true);
	return(false);
}

// 0x84A1D0
bool ShipRecustomizationCostClass::Write(LinearBufferClass *buffer)
{
	for (int slot = 0; slot < 12; slot++) {
		buffer->Write(&Costs.Per_Cost[slot], sizeof(int));
	}
	return(true);
}

// 0x84A210
bool ShipRecustomizationCostClass::Read(LinearBufferClass *buffer)
{
	for (int slot = 0; slot < 12; slot++) {
		buffer->Read(&Costs.Per_Cost[slot], sizeof(int));
	}
	return(true);
}

// 0x84A250
int ShipRecustomizationCostClass::Get_Size()
{
	return(sizeof(Costs));
}

/*
**	Reconstruction notes
**
**	The object runs from race.cpp's last function (0x8491B0) to sector_types.cpp's first (0x84A2A0):
**	the avatar's class (vtable 0xB08C6C) first, then the ship's (vtable 0xB08C7C), each in the same
**	order (constructors, the slot's cost, the sum, the slot comparison, Write / Read / Get_Size). The
**	FAIL_IFs pin lines 50-52 / 60-61 and 250-252 / 259-260; the slot's cost (0x849360, 0x849A30) is
**	inlined into the sum (only its line 52 / 252 stays there). Slots: the avatar's 14 are the hair,
**	beard, hair / skin / eye colours, tattoo, goggles, ears, body, shirt and pants colours with their
**	metals and the pants; the ship's 12 are hull, wings, the eight part settings and the name, decal.
**	The two helpers take the new value first: only that order gives 0x8494C0's registers (old in ecx /
**	esi). The single-byte and int comparisons return through '? true : false' (the client's bare setne).
*/
