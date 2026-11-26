// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\auxdata\permstr.h (name assumed from permstr.cpp)
**
**	An interned string: one pointer into permstr.cpp's hash table (s_Table), so equal strings
**	compare by pointer. The class name is the client's ("PermString: %d slots, ..." in 0x4ADFF0).
**	The table is made and freed by a counted initialiser that every file including this header
**	holds (the client's objects each construct one: 0x4A60A0 at 0xBE3A90, 0x4AE3B0 at 0xBE3B40).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __PERMSTR_H  // NAME 3 [permstr.h]: header name assumed from the client's permstr.cpp
#define __PERMSTR_H

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

static PermStringInitClass PermStringInit;  // NAME 2: describes what it does

#endif //__PERMSTR_H
