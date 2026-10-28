/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Common\race.h (name assumed from race.cpp; no assert
**	names it)
**
**	A character's race, alone and as a bit field, and its text converters (the key/value
**	pairs' Type_Name / String_To_Value / Value_To_String, race.cpp 0x848FF0-0x8492A6; the rest of race.cpp is not rebuilt).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef RACE_H  // NAME 3 [race.h]: header name assumed from the client's race.cpp
#define RACE_H

enum RaceType {};			// NAME 3: the client's Type_Name 'RaceType'
enum RaceBitFieldType {};	// NAME 3: the client's Type_Name 'RaceBitFieldType'

const char *Type_Name(RaceType value);								// 0x848FF0
bool String_To_Value(const char *text, RaceType *value);			// 0x849010
char *Value_To_String(char *buf, int size, RaceType value);		// 0x849060
const char *Type_Name(RaceBitFieldType value);						// 0x8490A0
bool String_To_Value(const char *text, RaceBitFieldType *value);	// 0x8490C0 (names joined by '|')
char *Value_To_String(char *buf, int size, RaceBitFieldType value);	// 0x8491B0

#endif
