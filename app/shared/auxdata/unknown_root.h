/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	// NAME 1 [unknown_root.h]: provisional file name, the object has no __FILE__
**	Original location: unknown (PROVISIONAL name, M1.3: the object 0x4A3990-0x4A4780 between
**	strconv_lib.cpp and bitset.cpp holds no assert and no __FILE__; renaming it later is fine)
**
**	The aux-data root: the DeltaCompoundClass every root tree (Ship, Player, ... in co.cpp) derives
**	from. The client names it nowhere (D24, D10). Its constructor 0x4A3BD0 inlines
**	DeltaCompoundClass's and keeps one int argument at +0x9C (the CREATE factory 0x6F6000 passes
**	its second argument through).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __UNKNOWN_ROOT_H
#define __UNKNOWN_ROOT_H

#include "delta.h"

// vtable 0xAEAB88 (DeltaCompoundClass's 0xAEABE4 with its own destructor), size 0xA0
// NAME 1: neutral; the root of every aux-data tree (vtable 0xAEAB88). Hints: C# AuxRoot (the reference game model; a schema object, not this class), Net-7 has none (its roots derive from AuxBase directly)
class UnknownRootClass : public DeltaCompoundClass
{
public:
	UnknownRootClass(int unknown, const char *name);		// 0x4A3BD0
	virtual ~UnknownRootClass();								// +0x04 0x4A3D30 (deleting 0x4A3D00)

	int m_Unknown9C;					// +0x9C the constructor's int (the factory's second argument)
};

// The aux-data clock 0x4A3B40: milliseconds since its first call (timeGetTime).
unsigned int Unknown_Get_Ticks();

#endif //__UNKNOWN_ROOT_H
