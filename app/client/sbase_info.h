// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\sbase_info.h (name assumed from sbase_info.cpp,
**	whose __FILE__ is 'e:\build\beta\G\app\Client\sbase_info.cpp')
**
**	What the server says a starbase holds (LOUNGE_NPC, opcode 82): its rooms by type, and in each
**	room the facilities (the terminals) by terminal and the NPCs by slot. Three SGI hash_maps of
**	pointers keyed by an unsigned (libs/sgistl, D87); the room owns its facilities and NPCs, the
**	starbase its rooms. The class names StarbaseRoomClass and StarbaseFacilityClass and their
**	NOT_ACTIVE / INVALID are the client's (sbase_info.cpp's asserts); the rest are guesses (D10, D24).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __SBASE_INFO_H  // NAME 3 [sbase_info.h]: header name assumed from the client's sbase_info.cpp
#define __SBASE_INFO_H

#include <string>
#include <hash_map.h>
#include "vector3.h"
#include "..\shared\avatar_desc.h"

class LoungeNPCPacketClass;

// A terminal of a room: 0xC bytes
class StarbaseFacilityClass	// the client's name (the assert 'facility_type == StarbaseFacilityClass::INVALID')
{
public:
	// The facility types: ui_starbase.cpp's terminal icons by type (0xB90C2C: ico_refinery, ico_analyze, ico_manufacture, ico_jobs,
	// ico_browser, ico_ship_con, ico_ava_con, ico_encyclo), which are LOUNGE_NPC's own numbers (0x6987A0 passes 0..7 and 9 through)
	enum {
		INVALID = -1,			// the client's name; the value: 0x634140 draws no icon for -1
		REFINERY = 0,			// NAME 2: the icon model's name
		ANALYZE,					// NAME 2: the icon model's name
		MANUFACTURE,			// NAME 2: the icon model's name
		JOBS,						// NAME 2: the icon model's name
		BROWSER,					// NAME 2: the icon model's name
		SHIP_CUSTOMIZE,		// NAME 2: the icon model's name (ico_ship_con)
		AVATAR_CUSTOMIZE,		// NAME 2: the icon model's name (ico_ava_con)
		ENCYCLOPEDIA,			// NAME 2: the icon model's name
		UNKNOWN_8,				// NAME 1: neutral (D10): no LOUNGE_NPC number gives it
		UNKNOWN_9,				// NAME 1: neutral (D10): LOUNGE_NPC's 9 (an ico_encyclo icon)
	};

	StarbaseFacilityClass(int terminal, int type, int unknown08);	// 0x6976F0  // NAME 1 [unknown08]: neutral

	unsigned int Terminal;	// +0x00 the key in the room's table (0x634140 asks for the terminals by index)  // NAME 2: what it holds
	int Type;					// +0x04 (the enum above)  // NAME 2: the assert's 'facility_type'
	int Unknown08;				// +0x08 (0x6987A0 passes 0)
};

// An NPC of a room: 0x12C bytes
class StarbaseNPCClass	// NAME 3: by the other two classes' names and the assert '!npc'
{
public:
	// The booths: ui_starbase.cpp's VB_ models by Get_Booth_Index() (0xB8AA18)
	enum {
		BOOTH_NONE = 0,			// NAME 2: what it means (0x6987A0's default)
		BOOTH_UNKNOWN_1,			// NAME 1: neutral (D10): no LOUNGE_NPC number gives it
		BOOTH_WEAPON,				// NAME 2: the VB_ model's name
		BOOTH_SYSTEM,				// NAME 2: the VB_ model's name
		BOOTH_CORE,					// NAME 2: the VB_ model's name
		BOOTH_CONSUMABLE,			// NAME 2: the VB_ model's name
		BOOTH_JUNK,					// NAME 2: the VB_ model's name
		BOOTH_COMPONENT,			// NAME 2: the VB_ model's name
		BOOTH_RESOURCE,			// NAME 2: the VB_ model's name
		BOOTH_BLACKMKT,			// NAME 2: the VB_ model's name (a booth with no avatar)
	};

	StarbaseNPCClass(int slot, int unknown04, int booth, int unknown1c, const AvatarDescClass &desc, const char *name);	// 0x6974F0  // NAME 1 [unknown04, unknown1c]: neutral
	int Get_Booth_Index();	// 0x6976C0: Booth - 2 for a booth with a VB_ model (2..9), else -1  // NAME 2: what it returns
	std::string Get_Name() { return(Name); }	// 0x636520 (ui_starbase.cpp's out-of-line copy)  // NAME 2: what it returns

	unsigned int Slot;		// +0x00 the key in the room's table (0x634140: the avatar slot)  // NAME 2: the message 'Duplicate npc slot definition encountered'
	int Unknown04;				// +0x04 (LOUNGE_NPC's number as it is)
	std::string Name;			// +0x08  // NAME 2: what it holds (0x634140 gives it to the slot's avatar)
	int Booth;					// +0x18 (the enum above)  // NAME 2: the VB_ models it picks
	int Unknown1C;				// +0x1C LOUNGE_NPC's 1..3, else 0 (set: 0x642740's avatar, the slot's bone not needed)
	AvatarDescClass Desc;	// +0x20  // NAME 2: what it holds
};

// A room: 0x54 bytes
class StarbaseRoomClass	// the client's name (the assert 'room_type == StarbaseRoomClass::NOT_ACTIVE')
{
public:
	// The room types: ui_starbase.cpp's door signs by type (0xB8A9A0: sn_hangar, sn_maindeck, sn_npclounge, sn_merchants, sn_lounge)
	enum {
		HANGAR = 0,				// NAME 2: the sign model's name
		MAIN_DECK,				// NAME 2: the sign model's name
		NPC_LOUNGE,				// NAME 2: the sign model's name
		MERCHANTS,				// NAME 2: the sign model's name
		LOUNGE,					// NAME 2: the sign model's name
		NOT_ACTIVE,				// the client's name; the value is a guess (0x6987A0's test of it is folded away)
	};

	StarbaseRoomClass(int type);	// 0x697720
	~StarbaseRoomClass();			// 0x697AA0: deletes the facilities and the NPCs
	void Add_Facility(StarbaseFacilityClass *facility);	// 0x697E00  // NAME 2: what it does
	void Add_NPC(StarbaseNPCClass *npc);						// 0x697F70  // NAME 2: what it does
	StarbaseFacilityClass *Find_Facility(unsigned int terminal);	// 0x6980E0, NULL when the room has none  // NAME 2: what it does
	StarbaseNPCClass *Find_NPC(unsigned int slot);						// 0x698150, NULL when the slot is empty  // NAME 2: what it does

	int Type;					// +0x00 the key in the starbase's table  // NAME 2: the assert's 'room_type'
	bool Unknown04;			// +0x04 LOUNGE_NPC's flags, a bit each: 0x001
	bool Unknown05;			// +0x05 0x002
	bool Unknown06;			// +0x06 0x004 (0x634140: the '%sbar05' model on 'bar')
	bool Unknown07;			// +0x07 0x008 (0x634140: '%sbar07' on each 'table%02d' bone)
	bool Unknown08;			// +0x08 0x010
	bool Unknown09;			// +0x09 0x020
	bool Unknown0A;			// +0x0A 0x040 (0x6A2120, 0x640D80: the room's second model variant, when +0x0B is clear)
	bool Unknown0B;			// +0x0B 0x080 (0x6A2120, 0x640D80: the room's first model variant)
	bool Unknown0C;			// +0x0C 0x100
	int Unknown10;				// +0x10 (LOUNGE_NPC's numbers as they are)
	int Unknown14;				// +0x14
	Vector3 Unknown18;		// +0x18 (LOUNGE_NPC's vector)
	std::hash_map<unsigned int, StarbaseFacilityClass *> Facilities;	// +0x24 by Terminal  // NAME 2: the assert's 'facility'
	std::hash_map<unsigned int, StarbaseNPCClass *> NPCs;					// +0x3C by Slot  // NAME 2: the assert's 'npc'
};

// The starbase: 0x1C bytes (the starbase view's +0x1B4)
class StarbaseInfoClass	// NAME 3: by convention (D24) from the client's file sbase_info.cpp and its message category 'Starbase Info'
{
public:
	StarbaseInfoClass(int style);	// 0x6981C0
	~StarbaseInfoClass();			// 0x698370: deletes the rooms
	void Add_Room(StarbaseRoomClass *room);				// 0x6985C0  // NAME 2: what it does
	StarbaseRoomClass *Find_Room(unsigned int type);	// 0x698730, NULL when the starbase has no such room  // NAME 2: what it does

	static StarbaseInfoClass *Create(LoungeNPCPacketClass *packet);	// 0x6987A0: the starbase LOUNGE_NPC describes  // NAME 2: what it does

	std::hash_map<unsigned int, StarbaseRoomClass *> Rooms;	// +0x00 by Type  // NAME 2: the assert's 'room'
	int Style;					// +0x18 LOUNGE_NPC's first number: the column of the views' model names (0 't', 1 'j', 2 'p', 3..9 'n')  // NAME 2: what it picks
};

#endif //__SBASE_INFO_H
