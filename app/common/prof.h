/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Common\prof.h (name assumed from prof.cpp; no assert
**	names it)
**
**	A character's profession, alone and as a bit field, and its text converters (the key/value
**	pairs' Type_Name / String_To_Value / Value_To_String, prof.cpp 0x848CA0-0x848FA0).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PROF_H  // NAME 3 [prof.h]: header name assumed from the client's prof.cpp
#define PROF_H

enum ProfessionType {};			// NAME 3: the client's Type_Name 'ProfessionType'
enum ProfessionBitFieldType {};	// NAME 3: the client's Type_Name 'ProfessionBitFieldType'

const char *Type_Name(ProfessionType value);								// 0x848CA0
bool String_To_Value(const char *text, ProfessionType *value);			// 0x848CC0 ('Merchant' is 1 too)
char *Value_To_String(char *buf, int size, ProfessionType value);		// 0x848D40
const char *Type_Name(ProfessionBitFieldType value);						// 0x848D80
bool String_To_Value(const char *text, ProfessionBitFieldType *value);	// 0x848DA0 (names joined by '|')
char *Value_To_String(char *buf, int size, ProfessionBitFieldType value);	// 0x848EB0

#endif
