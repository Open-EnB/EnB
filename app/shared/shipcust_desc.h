/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\shipcust_desc.h (name assumed from shipcust_desc.cpp)
**
**	The ship's customisation (0x160 bytes): race, profession, hull and wing styles, the ship's name
**	and its colour, and four parts of two colour settings each; sent with the avatar list (112) and
**	the character creation (114), kept five times by LoginTaskClass (+0x88C) and once by
**	CharacterViewClass. Members named by the visitors' keys (0x4F6750, 0x4F6890, 0x4F6900). Moved
**	here from tsk_login.h's LoginUnknown88CClass / LoginUnknown44Class / LoginUnknown20Class (D90 9b;
**	the constructors as e76cfe6 modelled them).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef SHIPCUST_DESC_H  // NAME 3 [shipcust_desc.h]: header name assumed from the client's shipcust_desc.cpp
#define SHIPCUST_DESC_H

#include "unknown_packetdata.h"
#include "vector3.h"
#include "vector2.h"

// The value types the visitors hand to their own key/value adds (0x4F6BB0 race, 0x4F6A80 profession,
// 0x4F6CE0 metal: three instantiations besides the int one)
enum ShipCustRaceType {};								// NAME 1: neutral (D10)
enum ShipCustProfessionType {};							// NAME 1: neutral (D10)
enum ShipCustomizationMetal { METAL_NONE = -1, METAL_CHROME, METAL_BRONZE, METAL_COPPER };	// NAME 5: its pairs' type name; NAME 2 [METAL_*]: the converters' names
#ifdef UNKNOWN_PACKETDATA_BODIES	// the converters the key/value pair template calls (shipcust_desc.cpp only)
class LinearBufferClass;
char *Value_To_String(char *buf, int size, const ShipCustomizationMetal &value);	// 0x4F6970 (line 384)
bool String_To_Value(const char *text, ShipCustomizationMetal *value);			// 0x4F6A00 (line 401)
inline const char *Type_Name(ShipCustomizationMetal *) { return "ShipCustomizationMetal"; }	// 0x4F72B0
#endif

// A colour setting (0x20 bytes, vtable 0xAEC91C over 0xAEADC8; constructor 0x618500 out of line), two in
// each part. Its assignment is the implicit one (0x4F6230 copies the race's defaults with it).
class ShipCustSettingsClass : public LoginInterfaceAEADC8Class	// NAME 2: the parts' key 'Settings%d'
{
public:
	ShipCustSettingsClass();							// 0x618500
	virtual void Unknown_00(void *visitor);				// 0x4F6900 (vtable 0xAEC91C)

	Vector3 Color;										// +0x04 NAME 2: its key 'Color'
	float Unknown10;									// +0x10 (copied, not a key)
	float Unknown14;									// +0x14
	bool Is_Matte;										// +0x18 NAME 2: its key 'IsMatte'
	ShipCustomizationMetal Metal;							// +0x1C NAME 2: its key 'Metal'
};

// A part (0x44 bytes, vtable 0xAF78BC over 0xAEADC8), four in each description. Its constructor is
// inline (in 0x7688F0: the two settings through ??_H with 0x618500) and out of line at 0x6191F0
// (another file, where 0x618500 is inlined); the LoginTaskClass constructors call 0x6191F0.
class ShipCustPartClass : public LoginInterfaceAEADC8Class	// NAME 2: the description's key 'PartAttributes%d'
{
public:
	ShipCustPartClass() {}
	virtual void Unknown_00(void *visitor);				// 0x4F6890 (vtable 0xAF78BC)

	ShipCustSettingsClass Settings[2];					// +0x04 NAME 2: its keys 'Settings%d'
};

// vtables 0xAF78A0 (wire: six slots) / 0xAF789C (key/value). Its constructor is inline: in both
// LoginTaskClass constructors, in 0x76A370, and out of line at 0x7688F0 after its first caller 0x7683F0.
class ShipCustDescClass : public LoginPacketDataClass, public LoginInterfaceAEADC8Class	// NAME 3: by convention (D24) from the client's shipcust_desc.cpp
{
public:
	ShipCustDescClass() :
		Tech_Level(0), Hull_Style(0), Wing_Style(0), Ship_Name_Color(1.0F, 1.0F, 1.0F),
		Unknown3C(0.5F, 0.5F), Selected_Decal(0x7FFFFFF)
	{
		Ship_Name_Empty = false;
		Set_Race(0);
		Set_Profession(0);
		Reset_Parts();
	}
	virtual bool Write(LinearBufferClass *buffer);		// +0x00 0x4F6330 (thunk 0x40C7F2)
	virtual bool Read(LinearBufferClass *buffer);		// +0x04 0x4F6420
	virtual int Get_Size();								// +0x08 0x4F6510
	virtual bool Write_Short(LinearBufferClass *buffer);	// +0x0C 0x4F6530: the name one byte shorter  // NAME 2: what it does
	virtual bool Read_Short(LinearBufferClass *buffer);	// +0x10 0x4F6620  // NAME 2: what it does
	virtual int Get_Short_Size();						// +0x14 0x4F6730  // NAME 2: what it does
	virtual void Unknown_00(void *visitor);				// second table +0x00 0x4F6750 (thunk 0x40AEE8)
	ShipCustDescClass &operator=(const ShipCustDescClass &that);	// 0x55D8A0 (in ui_avsel.cpp's window)
	void Set_Race(int race);							// 0x4F6210  // NAME 2: what it does
	void Reset_Parts();									// 0x4F6230: the race's default settings  // NAME 2: what it does
	void Set_Profession(int profession);				// 0x4F62F0: with the race's hull and wing  // NAME 2: what it does

	int Tech_Level;										// +0x08 NAME 2: its key 'TechLevel'
	int Hull_Style;										// +0x0C NAME 2: its key 'HullStyle'
	int Wing_Style;										// +0x10 NAME 2: its key 'WingStyle'
	union {												// +0x14 (the constructors clear the first byte as a bool: tsk_login.cpp's 0x767240 matches only so)
		bool Ship_Name_Empty;							// NAME 1: what the constructor stores
		char Ship_Name[0x1A];							// NAME 2: its key 'ShipName'
	};
	Vector3 Ship_Name_Color;							// +0x30 NAME 2: its key 'ShipNameColor'
	Vector2 Unknown3C;									// +0x3C (neither written nor a key)
	int Selected_Decal;									// +0x44 NAME 2: its key 'SelectedDecal'
	ShipCustPartClass Parts[4];							// +0x48 NAME 2: its keys 'PartAttributes%d'
	ShipCustRaceType Race;								// +0x158 NAME 2: its key 'Race'
	ShipCustProfessionType Profession;					// +0x15C NAME 2: its key 'Profession'
};

#endif
