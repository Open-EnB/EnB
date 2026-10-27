/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\avatar_desc.h (name assumed from avatar_desc.cpp)
**
**	The avatar's description (0x10C bytes): what the character looks like, sent with the avatar
**	list (112) and the character creation (114), kept five times by LoginTaskClass (+0x350) and
**	once by CharacterViewClass. The members are named by its visitor's keys (0x4BFED0). Moved here
**	from tsk_login.h's LoginUnknown350Class (D90 9b).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef AVATAR_DESC_H  // NAME 3 [avatar_desc.h]: header name assumed from the client's avatar_desc.cpp
#define AVATAR_DESC_H

#include "unknown_packetdata.h"
#include "vector3.h"
#include "vector2.h"
#include <string>

// vtables 0xAEB9D0 (wire: six slots) / 0xAEB9CC (key/value); constructors 0x4BEB50, 0x4BEBF0, 0x4BECC0.
// The destructor is the implicit one (inline in 0x7683F0, out of line at 0x53BB50).
class AvatarDescClass : public LoginPacketDataClass, public LoginInterfaceAEADC8Class	// NAME 3: by convention (D24) from the client's avatar_desc.cpp
{
public:
	AvatarDescClass();									// 0x4BEB50
	AvatarDescClass(int type, int unknown, int unknown2);	// 0x4BEBF0 (Race, Profession, Gender)  // NAME 1 [unknown, unknown2]: neutral (D10)
	AvatarDescClass(int type, const char *name);		// 0x4BECC0  // NAME 2 [name]: Load_Defaults'
	virtual bool Write(LinearBufferClass *buffer);		// +0x00 0x4BF260
	virtual bool Read(LinearBufferClass *buffer);		// +0x04 0x4BF550
	virtual int Get_Size();								// +0x08 0x4BF910
	virtual bool Write_Short(LinearBufferClass *buffer);	// +0x0C 0x4BF930: without +0x0C and the string  // NAME 2: what it does
	virtual bool Read_Short(LinearBufferClass *buffer);	// +0x10 0x4BFBF0  // NAME 2: what it does
	virtual int Get_Short_Size();						// +0x14 0x4BFEB0  // NAME 2: what it does
	virtual void Unknown_00(void *visitor);				// second table +0x00 0x4BFED0 (a LoginKVVisitorClass)

	void Clear(int type);								// 0x4C0310  // NAME 2: what it does
	bool Is_Valid();									// 0x4C0580: a type is set  // NAME 2: what it does
	void Reset();										// 0x4C05A0  // NAME 2: what it does
	void Load_Defaults(int type, const char *name);		// 0x4C05D0: avatardef.ini's section for the type (0 the gender, race and profession, 1 the narrator, 2 the name)  // NAME 2: what it does
	std::string Get_Full_Name();						// 0x4C1020: the first and last names, a space between  // NAME 2: what it does
	std::string &Get_Full_Name(std::string &name);		// 0x4C1200: the same into name  // NAME 2: what it does
	void Unknown_4C1460(const char *name, bool convert);	// 0x4C1460: First_Name (the fast start's 'Fast' + the user name, 1; convert: through 0x4BE9D0)  // NAME 1 [convert]: what it selects
	void Unknown_4C15F0(const char *name, bool convert);	// 0x4C15F0: Last_Name, the same way
	void Get_Body_Morph(Vector3 &scale, int morph);		// 0x4C1780: avatardef.ini's '<gender><race>morph' section, 1-4 short_skinny .. tall_skinny  // NAME 2: what it does
	void Get_Head_Morph(Vector3 &scale, int morph);		// 0x4C1930: the same, short_narrow .. long_narrow (Y and Z)  // NAME 2: what it does
	void Get_Facial_Feature(bool &movable, int &scale);	// 0x4C1AC0: 'FacialFeature%02d' of the tattoo texture  // NAME 2: what it does
	void Get_Hair_Offset(Vector2 &offset);		// 0x4C1B80: avatardef.ini's HairOffset (0.5, 0.5 by default)  // NAME 2: its key
	void Get_Skin_Offset(Vector2 &offset);		// 0x4C1CA0: avatardef.ini's SkinOffset (0.5, 0.5 by default)  // NAME 2: its key
	void Get_Eye_Offset(Vector2 &offset);		// 0x4C1DC0: avatardef.ini's EyeOffset (0.5, 0.5 by default)  // NAME 2: its key
	void Get_Shirt_Primary_Offset(Vector2 &offset);		// 0x4C1EE0: avatardef.ini's ShirtPrimaryOffset (0.5, 0.5 by default)  // NAME 2: its key
	void Get_Shirt_Secondary_Offset(Vector2 &offset);		// 0x4C2000: avatardef.ini's ShirtSecondaryOffset (0.5, 0.5 by default)  // NAME 2: its key
	void Get_Pants_Primary_Offset(Vector2 &offset);		// 0x4C2120: avatardef.ini's PantsPrimaryOffset (0.5, 0.5 by default)  // NAME 2: its key
	void Get_Pants_Secondary_Offset(Vector2 &offset);		// 0x4C2240: avatardef.ini's PantsSecondaryOffset (0.5, 0.5 by default)  // NAME 2: its key
	void Save_Defaults(const Vector2 *offsets);			// 0x4C2360: the description into avatardef.ini (offsets: the seven texture offsets, Y flipped)  // NAME 2: what it does

	int Avatar_Type;									// +0x08 NAME 2: its key 'avatar_type' (Clear's argument)
	char Unknown0C;										// +0x0C (written, not a key)
	char Avatar_Version;								// +0x0D NAME 2: its key 'avatar_version' (2)
	int Race;											// +0x10 NAME 2: its key 'race'
	int Profession;										// +0x14 NAME 2: its key 'profession'
	int Gender;											// +0x18 NAME 2: its key 'gender'
	int Mood_Type;										// +0x1C NAME 2: its key 'mood_type'
	char Personality;									// +0x20 NAME 2: its key 'personality'
	char NLP;											// +0x21 NAME 2: its key 'nlp'
	char Body_Type;										// +0x22 NAME 2: its key 'body_type'
	char Pants_Type;									// +0x23 NAME 2: its key 'pants_type'
	char Head_Type;										// +0x24 NAME 2: its key 'head_type'
	char Hair_Num;										// +0x25 NAME 2: its key 'hair_num'
	char Ear_Num;										// +0x26 NAME 2: its key 'ear_num'
	char Goggle_Num;									// +0x27 NAME 2: its key 'goggle_num'
	char Beard_Num;										// +0x28 NAME 2: its key 'beard_num'
	char Weapon_Hip_Num;								// +0x29 NAME 2: its key 'weapon_hip_num'
	char Weapon_Unique_Num;								// +0x2A NAME 2: its key 'weapon_unique_num'
	char Weapon_Back_Num;								// +0x2B NAME 2: its key 'weapon_back_num'
	char Head_Texture_Num;								// +0x2C NAME 2: its key 'head_texture_num'
	char Tattoo_Texture_Num;							// +0x2D NAME 2: its key 'tattoo_texture_num'
	Vector3 Tattoo_Offset;								// +0x30 NAME 2: its key 'tattoo_offset'
	Vector3 Hair_Color;									// +0x3C NAME 2: its key 'hair_color'
	Vector3 Beard_Color;								// +0x48 NAME 2: its key 'beard_color'
	Vector3 Eye_Color;									// +0x54 NAME 2: its key 'eye_color'
	Vector3 Skin_Color;									// +0x60 NAME 2: its key 'skin_color'
	Vector3 Shirt_Primary_Color;						// +0x6C NAME 2: its key 'shirt_primary_color'
	Vector3 Shirt_Secondary_Color;						// +0x78 NAME 2: its key 'shirt_secondary_color'
	Vector3 Pants_Primary_Color;						// +0x84 NAME 2: its key 'pants_primary_color'
	Vector3 Pants_Secondary_Color;						// +0x90 NAME 2: its key 'pants_secondary_color'
	int Shirt_Primary_Metal;							// +0x9C NAME 2: its key 'shirt_primary_metal'
	int Shirt_Secondary_Metal;							// +0xA0 NAME 2: its key 'shirt_secondary_metal'
	int Pants_Primary_Metal;							// +0xA4 NAME 2: its key 'pants_primary_metal'
	int Pants_Secondary_Metal;							// +0xA8 NAME 2: its key 'pants_secondary_metal'
	float Body_Weight[5];								// +0xAC NAME 2: its key 'body_weight'
	float Head_Weight[5];								// +0xC0 NAME 2: its key 'head_weight'
	char First_Name[0x14];								// +0xD4 NAME 2: its key 'avatar_first_name'
	char Last_Name[0x14];								// +0xE8 NAME 2: its key 'avatar_last_name'
	std::string Skeleton;								// +0xFC NAME 2: avatardef.ini's key 'Skeleton'
};

#endif
