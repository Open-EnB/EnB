/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\auxdata\lookup.h (name assumed from lookup.cpp)
**
**	A compound field's lookup table (DeltaCompoundClass::Lookup, made by Get_Lookup 0x4A5510).
**	Only what delta.cpp uses so far: the constructor is lookup.cpp's (0x4AC250, not rebuilt yet).
**	The layout comes from it and from ~DeltaCompoundClass (0x4A5070): a std::vector at +0x00 and a
**	std::map at +0x10, size 0x20. Their element types are placeholders until lookup.cpp is rebuilt.
**	The client doesn't name the class (D24).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __LOOKUP_H
#define __LOOKUP_H

#include <vector>
#include <map>

class DeltaCompoundClass;

class LookupClass
{
public:
	LookupClass(DeltaCompoundClass *owner);				// 0x4AC250

	std::vector<int> m_Unknown00;							// +0x00 element type not known yet
	std::map<int, int> m_Unknown10;						// +0x10 key and value types not known yet
};

#endif //__LOOKUP_H
