// Earth & Beyond client (reconstruction)
/*
**	PROVISIONAL file name (no assert or __FILE__ names it): the object 0x8462A0-0x846A3F, between
**	manufacturing_defs.cpp and nav_type_enum.cpp. It holds the music types' name table (0xBAF63C) and
**	the navigation list: the nav ids of a warp path with a cursor (vtable 0xB08C60, 0x18 bytes), sent
**	in WARP 155 (WarpPacketClass prints its size as 'NavigationListSize') and kept by starmap_pip.cpp
**	('!WarpPath', '!TargetPath') and radar_system.cpp.
**
**	Rebuilt in unknown_navlist.cpp (same provisional name).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UNKNOWN_NAVLIST_H  // NAME 1 [unknown_navlist.h]: neutral file name (D37, D10)
#define UNKNOWN_NAVLIST_H

#include <vector>

class RawPacketClass;

bool Music_Type_From_Name(const char *name, int *type);		// 0x8462A0 (no caller)  // NAME 2: what it does
char *Music_Type_Name(char *buffer, int len, int type);		// 0x8462F0 (no caller)  // NAME 2: what it does

class NavigationListClass	// NAME 3: WARP's Print text 'NavigationListSize='
{
public:
	NavigationListClass();								// 0x846330
	NavigationListClass(const NavigationListClass &src);	// 0x846390
	NavigationListClass(RawPacketClass *raw);			// 0x846430
	virtual ~NavigationListClass();						// +0x00 0x846360 (0x8466F0)
	virtual void Clear();								// +0x04 0x846870  // NAME 2: what it does

	void Add(RawPacketClass *raw);						// 0x846730: a 2-byte count, then the ids  // NAME 2: what it does
	int Get_Data_Size();								// 0x8467B0  // NAME 2: what it returns
	unsigned long Get_Current();						// 0x8467F0 (-1 past the end)  // NAME 2: what it returns
	bool Is_Last();										// 0x846830  // NAME 2: what it returns
	bool Set_Index(unsigned int index);					// 0x8468C0  // NAME 2: what it does
	bool Next();										// 0x846900  // NAME 2: what it does
	bool Contains(unsigned long id, bool from_current);	// 0x846940  // NAME 2: what it does
	unsigned long Get(int index);						// 0x8469B0 (< 0 from the end, 0 out of range)  // NAME 2: what it returns

	std::vector<unsigned long> List;	// +0x04  // NAME 2: what it holds
	int Index;							// +0x14 the cursor (-1 none)  // NAME 2: what it holds
};

#endif
