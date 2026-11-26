// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (name PROVISIONAL, D137): PermString and its counted initialiser's class without
**	permstr.h's static PermStringInit. The client's CRT table: unknown_445740.cpp's object (0x445740-0x448BB0)
**	builds KVPairTemplateClass<float> / <bool> (kv_pair.h: a PermString key) and opens with no PermStringInit,
**	while kv_pair.cpp, kv_list.cpp, kv_pair_string.cpp, kv_encoder_base.cpp, kv_time_adds.cpp and msgserv.cpp's
**	objects each open with one (0x4B7E60, 0x4B6260, 0x4B7F70, 0x4B3800, 0x4B5EB0, 0x8B2080): kv_pair.h takes the
**	class from here and those files include permstr.h themselves.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __PERMSTRING_H  // NAME 1 [permstring.h]: PROVISIONAL (D137), the class without permstr.h's static
#define __PERMSTRING_H

class PermString
{
public:
	PermString() : String(0) {}
	PermString(const char *string) : String(0) { String = Find(string); }

	PermString &operator=(const char *string) { String = Find(string); return *this; }
	PermString &operator+=(const PermString &that);			// 0x4AE070

	bool operator==(const PermString &that) const { return String == that.String; }
	bool operator!=(const PermString &that) const { return String != that.String; }

	const char *Get_String() const { return String; }  // NAME 2: describes what it does

	// the table's copy of string, added if it isn't there (0x4ADE20)
	static const char *Find(const char *string);  // NAME 2: describes what it does
	static void Print_Statistics();								// 0x4ADFF0  // NAME 2: describes what it does

	const char *String;		// +0x00  // NAME 2: describes what it does
};

// Makes the table on the first construction and frees it on the last destruction (s_Count).
class PermStringInitClass  // NAME 2: describes what it does (D38)
{
public:
	PermStringInitClass();				// 0x4AE1E0
	~PermStringInitClass();				// 0x4AE2D0
};

#endif //__PERMSTRING_H
