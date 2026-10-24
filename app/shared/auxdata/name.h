/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
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

#ifndef __NAME_H
#define __NAME_H

#include "permstr.h"

#define MAX_NAME_LENGTH	10

// size 0x34 (DeltaClass's Name at +0x08, FullName at +0x3C)
class NameClass
{
public:
	NameClass(const char *name);											// 0x4AD380
	NameClass(const NameClass &that);									// 0x4AD3F0
	NameClass &operator=(const NameClass &that);						// 0x4AD460
	NameClass &operator=(const char *name);								// 0x4AD4B0
	bool operator==(const NameClass &that) const;					// 0x4AD500
	bool Is_Parent_Of(const NameClass &that) const;					// 0x4AD610 (fewer parts, all matching)
	bool Is_Parent_Or_Same(const NameClass &that) const;			// 0x4AD6F0
	bool Match_Parts(const NameClass &that) const;					// 0x4AD7D0 (this name's parts against that's)
	NameClass &Append(const NameClass &rhs);							// 0x4AD9F0 ("this.rhs")
	PermString Get_First() const;											// 0x4ADB80

	// DeltaCompoundClass::Unknown_00 (0x4A55D0) passes the whole name on as a string.
	const char *Get_String() const { return Name.Get_String(); }

	void Split() const;														// 0x4AD890

	// the part that matches any part (0xBE3B24, made on first use)
	static const PermString &Wildcard()
	{
		static PermString wildcard("*");
		return wildcard;
	}

	PermString Name;										// +0x00
	mutable PermString Parts[MAX_NAME_LENGTH];	// +0x04 the parts between the dots (Split)
	mutable int Count;									// +0x2C the number of Parts, -1 until Split
	bool HasWildcard;										// +0x30 Name has a '*'
};

#endif //__NAME_H
