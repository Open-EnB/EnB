/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\auxdata\lookup.h (name assumed from lookup.cpp)
**
**	A compound field's lookup table (DeltaCompoundClass::Lookup, made by Get_Lookup 0x4A5510).
**	Only what delta.cpp and lookup.cpp use so far (lookup.cpp's constructor 0x4AC250 is not rebuilt yet).
**	The layout comes from it and from ~DeltaCompoundClass (0x4A5070): a std::vector at +0x00 and a
**	std::map at +0x10, size 0x20: the vector holds the fields (lookup.cpp's index accessors), the
**	map's key and value types are placeholders until lookup.cpp's name functions are rebuilt.
**	The client doesn't name the class (D24).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __LOOKUP_H
#define __LOOKUP_H

#include <vector>
#include <map>

class DeltaClass;
class DeltaCompoundClass;
class NameClass;

class LookupClass
{
public:
	LookupClass(DeltaCompoundClass *owner);				// 0x4AC250

	// The name is the asserts'; the signed/unsigned compare is the client's (jl, then jb).
	bool Is_Valid_Index(int index) const { return index >= 0 && index < Entries.size(); }
	DeltaClass *Get_Delta(int index) const;				// 0x4AC850
	bool Unknown_Test_70(int index) const;				// 0x4AC8D0, the field's m_Unknown70 != 0
	bool Unknown_Test_70_Above_1(int index) const;		// 0x4AC960, the field's m_Unknown70 > 1
	DeltaClass *Find(const NameClass &name);				// 0x4AC9F0 (no wildcards)

	std::vector<DeltaClass *> Entries;					// +0x00 (the name is a guess)
	std::map<const char *, DeltaClass *> m_Unknown10;	// +0x10 the fields by their interned names
																// (the key copied to a temporary: an rvalue pointer)
};

#endif //__LOOKUP_H
