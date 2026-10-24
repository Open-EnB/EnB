/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\auxdata\permstr.h (name assumed from permstr.cpp)
**
**	An interned string: one pointer into permstr.cpp's table (s_Table), so equal strings compare
**	by pointer. The class name is the client's ("PermString: %d slots, ..." in 0x4ADFF0). Only
**	what name.cpp uses so far: permstr.cpp is not rebuilt yet.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __PERMSTR_H
#define __PERMSTR_H

class PermString
{
public:
	PermString() : String(0) {}
	PermString(const char *string) : String(0) { String = Find(string); }

	PermString &operator=(const char *string) { String = Find(string); return *this; }

	bool operator==(const PermString &that) const { return String == that.String; }
	bool operator!=(const PermString &that) const { return String != that.String; }

	const char *Get_String() const { return String; }

	// the table's copy of string, added if it isn't there (0x4ADE20)
	static const char *Find(const char *string);

	const char *String;		// +0x00
};

#endif //__PERMSTR_H
