// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\auxdata\name.h (name assumed from name.cpp)
**
**	A dotted field name (DeltaClass::Name, FullName): the whole name interned, and its parts
**	between the dots once Split has run. Names with a '*' match any part there (operator==,
**	Is_Parent_Of). Members from name.cpp's asserts (Count, MAX_NAME_LENGTH). The client doesn't
**	name the class (D24).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __NAME_H  // NAME 3 [name.h]: header name assumed from the client's name.cpp
#define __NAME_H

#include "permstr.h"

#define MAX_NAME_LENGTH	10

// size 0x34 (DeltaClass's Name at +0x08, FullName at +0x3C)
class NameClass  // NAME 3: by convention (D24) from the client's file name.cpp
{
public:
	NameClass(const char *name);											// 0x4AD380
	NameClass(const NameClass &that);									// 0x4AD3F0
	NameClass &operator=(const NameClass &that);						// 0x4AD460
	NameClass &operator=(const char *name);								// 0x4AD4B0
	bool operator==(const NameClass &that) const;					// 0x4AD500
	bool Is_Parent_Of(const NameClass &that) const;					// 0x4AD610 (fewer parts, all matching)  // NAME 2: describes what it does (D38)
	bool Is_Parent_Or_Same(const NameClass &that) const;			// 0x4AD6F0  // NAME 2: describes what it does (D38)
	bool Match_Parts(const NameClass &that) const;					// 0x4AD7D0 (this name's parts against that's)  // NAME 2: describes what it does (D38)
	NameClass &Append(const NameClass &rhs);							// 0x4AD9F0 ("this.rhs")  // NAME 2: describes what it does
	PermString Get_Rest() const;											// 0x4ADBB0 (after the first dot, or "")  // NAME 2: describes what it does
	PermString Get_First() const;											// 0x4ADB80  // NAME 2: describes what it does

	// DeltaCompoundClass::Unknown_00 (0x4A55D0) passes the whole name on as a string.
	const char *Get_String() const { return Name.Get_String(); }  // NAME 2: describes what it does
	bool Is_Wildcard() const { return HasWildcard; }				// (LookupClass's asserts)

	void Split() const;														// 0x4AD890  // NAME 2: describes what it does

	// the part that matches any part (0xBE3B24, made on first use)
	static const PermString &Wildcard()  // NAME 2: after the asserts' Is_Wildcard
	{
		static PermString wildcard("*");
		return wildcard;
	}

	PermString Name;										// +0x00  // NAME 2: describes what it does
	mutable PermString Parts[MAX_NAME_LENGTH];	// +0x04 the parts between the dots (Split)  // NAME 2: describes what it does
	mutable int Count;									// +0x2C the number of Parts, -1 until Split
	bool HasWildcard;										// +0x30 Name has a '*'  // NAME 2: after the asserts' Is_Wildcard
};

#endif //__NAME_H
