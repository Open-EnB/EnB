/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\auxdata\name.h (name assumed from name.cpp)
**
**	A field name (DeltaClass::Name, FullName). Only what delta.cpp uses so far: the functions are
**	name.cpp's (0x4AD380-...), not rebuilt yet, and the layout past the first member is open.
**	The client doesn't name the class (D24).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __NAME_H
#define __NAME_H

// size 0x34 (DeltaClass's Name at +0x08, FullName at +0x3C)
class NameClass
{
public:
	NameClass(const char *name);							// 0x4AD380
	NameClass &operator=(const NameClass &that);		// 0x4AD460
	void Append(const NameClass &that);					// 0x4AD9F0

	// DeltaCompoundClass::Unknown_00 (0x4A55D0) passes the first member on as a string.
	const char *Get_String() const { return m_Unknown00; }

	const char *m_Unknown00;		// +0x00
	char m_Unknown04[0x30];			// +0x04 not known yet
};

#endif //__NAME_H
