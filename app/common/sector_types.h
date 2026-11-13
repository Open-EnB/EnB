// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Common\sector_types.h (name assumed from sector_types.cpp, whose
**	__FILE__ is 0xBB0B04)
**
**	A sector's type (space, planet, gas giant: the names of 0xBB0AC0) and its text converters
**	(sector_types.cpp 0x84A2A0-0x84A45F). The packets' Print (SET_CLIENT_TYPE, STARBASE_SET) name a type.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef SECTOR_TYPES_H  // NAME 3 [sector_types.h]: header name assumed from the client's sector_types.cpp
#define SECTOR_TYPES_H

enum SectorType {	// NAME 3: the client's 'InvalidSectorType'
	SECTOR_SPACE,		// the client's FAIL_IF text 's < SECTOR_SPACE || s >= SECTOR_COUNT'; name 'SectorSpace'
	SECTOR_PLANET,		// NAME 3: the client's name 'SectorPlanet'
	SECTOR_GAS_GIANT,	// NAME 3: the client's name 'SectorGasGiant'
	SECTOR_COUNT		// the client's FAIL_IF text
};

bool String_To_Value(const char *buf, SectorType *s);	// 0x84A2A0: the type of a name (stricmp; FAIL_IF '!buf' l.36)  // NAME 3: race.h's converters' names
char *Value_To_String(char *buf, int size, SectorType s);	// 0x84A320: copies the type's name into buf (FAIL_IF '!buf' l.51)  // NAME 3: race.h's converters' names
char *Sector_Type_Name(int s);							// 0x84A3B0: the type's name, 'InvalidSectorType' out of range (l.60; also declared by pksetclienttype.h / pkstarbasetow.h)  // NAME 2: what it returns

class UnknownSectorPairClass {	// NAME 1: neutral (D10): two words, constructed in sector_types.cpp (no caller in the client)
public:
	UnknownSectorPairClass();					// 0x84A3F0: both 0
	UnknownSectorPairClass(int a, int b);		// 0x84A410  // NAME 1 [a, b]: neutral (D10)

	int Unknown00;	// +0x00
	int Unknown04;	// +0x04
};

#endif
