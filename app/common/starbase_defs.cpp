// Earth & Beyond client (reconstruction): app\Common\starbase_defs.cpp (__FILE__ 0xBB0E54). Notes at the end of the file.
#include <string.h>
#include "starbase_defs.h"
#include "..\shared\unknown_4a1590.h"
#include "..\shared\g_assert.h"

static const char *_RoomNames[] = { "Hangar", "MainHall", "Bazaar", "NPCLounge", "PlayerLounge" };	// 0xBB0C98  // NAME 1: what it holds
static const char *_FacilityNames[] = { "Refinery", "Analyze", "Manufacturing", "News", "Web", "Training", "Customize", "Repair", "Bank" };	// 0xBB0CAC  // NAME 1: what it holds
static const char *_RoomDecorationNames[] = { "Dividers", "Rafters", "Bar", "Tables", "Monitors", "Eyecandy", "SmallRoom", "AltRoom", "Fog" };	// 0xBB0CD0  // NAME 1: what it holds
static const char *_ArtNames[] = { "Terran", "Jenquai", "Progen", "Solsec", "Black11", "Highport", "Net7", "PleasurePort", "Pirates", "Thule" };	// 0xBB0CF4  // NAME 1: what it holds

// 0x84A640
const char *Type_Name(StarbaseRoomEnum value)
{
	return("StarbaseRoomEnum");
}

// 0x84A660
bool String_To_Value(const char *buf, StarbaseRoomEnum *r)
{
	for (int i = 0; i < sizeof(_RoomNames) / sizeof(_RoomNames[0]); i++) {
		if (stricmp(buf, _RoomNames[i]) == 0) {
			*r = (StarbaseRoomEnum)i;
			return(true);
		}
	}
	return(false);
}








// 0x84A6B0
char *Value_To_String(char *buf, int size, StarbaseRoomEnum r)
{
	FAIL_IF(r <= STARBASE_ROOM_INVALID) return("invalid");
	FAIL_IF(r >= STARBASE_ROOM_NUM) return("invalid");
	return(Bounded_Strcpy(buf, size, _RoomNames[r]));
}

// 0x84A730
const char *Type_Name(StarbaseFacilityEnum value)
{
	return("StarbaseFacilityEnum");
}

// 0x84A750
bool String_To_Value(const char *buf, StarbaseFacilityEnum *r)
{
	for (int i = 0; i < sizeof(_FacilityNames) / sizeof(_FacilityNames[0]); i++) {
		if (stricmp(buf, _FacilityNames[i]) == 0) {
			*r = (StarbaseFacilityEnum)i;
			return(true);
		}
	}
	return(false);
}

















// 0x84A7A0
char *Value_To_String(char *buf, int size, StarbaseFacilityEnum r)
{
	FAIL_IF(r <= STARBASE_FACILITY_INVALID) return("invalid");
	FAIL_IF(r >= STARBASE_FACILITY_NUM) return("invalid");
	return(Bounded_Strcpy(buf, size, _FacilityNames[r]));
}

// 0x84A820
const char *Type_Name(StarbaseRoomDecorationEnum value)
{
	return("StarbaseRoomDecorationEnum");
}

// 0x84A840
bool String_To_Value(const char *buf, StarbaseRoomDecorationEnum *r)
{
	for (int i = 0; i < sizeof(_RoomDecorationNames) / sizeof(_RoomDecorationNames[0]); i++) {
		if (stricmp(buf, _RoomDecorationNames[i]) == 0) {
			*r = (StarbaseRoomDecorationEnum)i;
			return(true);
		}
	}
	return(false);
}
















// 0x84A890
char *Value_To_String(char *buf, int size, StarbaseRoomDecorationEnum r)
{
	FAIL_IF(r <= STARBASE_ROOM_DECORATION_INVALID) return("invalid");
	FAIL_IF(r >= STARBASE_ROOM_DECORATION_NUM) return("invalid");
	return(Bounded_Strcpy(buf, size, _RoomDecorationNames[r]));
}

// 0x84A910
const char *Type_Name(StarbaseArtEnum value)
{
	return("StarbaseArtEnum");
}

// 0x84A930
bool String_To_Value(const char *buf, StarbaseArtEnum *r)
{
	for (int i = 0; i < sizeof(_ArtNames) / sizeof(_ArtNames[0]); i++) {
		if (stricmp(buf, _ArtNames[i]) == 0) {
			*r = (StarbaseArtEnum)i;
			return(true);
		}
	}
	return(false);
}

















// 0x84A980
char *Value_To_String(char *buf, int size, StarbaseArtEnum r)
{
	FAIL_IF(r <= STARBASE_ART_INVALID) return("invalid");
	FAIL_IF(r >= STARBASE_ART_NUM) return("invalid");
	return(Bounded_Strcpy(buf, size, _ArtNames[r]));
}

/*
**	Reconstruction notes
**
**	The object: 0x84A640-0x84A9E2 (after skill_level_availability.cpp's 0x84A600; StrEditClass's
**	constructor 0x84AA10 follows), its data the four name tables 0xBB0C98-0xBB0D1B (contiguous, ahead of
**	every string: defined first), their texts, the type names, the FAIL_IF texts and the __FILE__ string
**	0xBB0E54. No guarded <string> initialiser. The FAIL_IFs pin lines 40/41, 82/83, 123/124 and 165/166.
**	The facility names are nine while the client's bound STARBASE_FACILITY_NUM is 10 (Value_To_String
**	of 9 reads the decoration table's first name). 'invalid' is pooled with an earlier object's (0xBAF6D0).
**	No function here has a caller in the client; the Type_Name signatures follow guild_defs.cpp's.
*/
