// Earth & Beyond client (reconstruction): app\common\starbase_defs.h
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Common\starbase_defs.h (name assumed from starbase_defs.cpp,
**	whose __FILE__ is 0xBB0E54; no assert names the header)
**
**	A starbase's rooms, facilities, room decorations and art styles (the client's Get_Type_Name strings
**	'StarbaseRoomEnum', 'StarbaseFacilityEnum', 'StarbaseRoomDecorationEnum', 'StarbaseArtEnum'), with
**	their names and text converters (starbase_defs.cpp, 0x84A640-0x84A9E2; none has a caller in the client).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef STARBASE_DEFS_H  // NAME 3 [starbase_defs.h]: header name assumed from the client's starbase_defs.cpp
#define STARBASE_DEFS_H

// The bounds are the client's FAIL_IF texts ('r <= STARBASE_ROOM_INVALID', 'r >= STARBASE_ROOM_NUM' ...).
enum StarbaseRoomEnum { STARBASE_ROOM_INVALID = -1, STARBASE_ROOM_NUM = 5 };	// 'Hangar' .. 'PlayerLounge'
enum StarbaseFacilityEnum { STARBASE_FACILITY_INVALID = -1, STARBASE_FACILITY_NUM = 10 };	// 'Refinery' .. 'Bank': nine names, the client's bound is 10
enum StarbaseRoomDecorationEnum { STARBASE_ROOM_DECORATION_INVALID = -1, STARBASE_ROOM_DECORATION_NUM = 9 };	// 'Dividers' .. 'Fog'
enum StarbaseArtEnum { STARBASE_ART_INVALID = -1, STARBASE_ART_NUM = 10 };	// 'Terran' .. 'Thule'

const char *Type_Name(StarbaseRoomEnum value);										// 0x84A640
bool String_To_Value(const char *buf, StarbaseRoomEnum *r);							// 0x84A660
char *Value_To_String(char *buf, int size, StarbaseRoomEnum r);						// 0x84A6B0
const char *Type_Name(StarbaseFacilityEnum value);									// 0x84A730
bool String_To_Value(const char *buf, StarbaseFacilityEnum *r);						// 0x84A750
char *Value_To_String(char *buf, int size, StarbaseFacilityEnum r);					// 0x84A7A0
const char *Type_Name(StarbaseRoomDecorationEnum value);							// 0x84A820
bool String_To_Value(const char *buf, StarbaseRoomDecorationEnum *r);				// 0x84A840
char *Value_To_String(char *buf, int size, StarbaseRoomDecorationEnum r);			// 0x84A890
const char *Type_Name(StarbaseArtEnum value);										// 0x84A910
bool String_To_Value(const char *buf, StarbaseArtEnum *r);							// 0x84A930
char *Value_To_String(char *buf, int size, StarbaseArtEnum r);						// 0x84A980

#endif
