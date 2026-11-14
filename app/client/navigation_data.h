// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\navigation_data.h (name assumed from navigation_data.cpp)
**
**	A client object's navigation data (vtable 0xB065C8, 0x18 bytes), bound to the object through
**	co_data.cpp; NAVIGATION 153 sets it (navigation_data.cpp, 0x7BA8B0-0x7BA9B0).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __NAVIGATION_DATA_H  // NAME 3 [navigation_data.h]: header name assumed from the client's navigation_data.cpp
#define __NAVIGATION_DATA_H

#include "co_data.h"

class NavigationPacketClass;

class NavigationDataClass : public ClientObjectDataClass  // NAME 5: the client's typeid name .PAVNavigationDataClass@@ (co_data.cpp's registry)
{
public:
	NavigationDataClass(ClientObjectClass *co);			// 0x7BA8B0 (navigation_data.cpp)
	virtual ~NavigationDataClass();						// +0x04 0x7BA920 (0x7BA8F0)

	void Unknown_7BA940(NavigationPacketClass *packet);	// 0x7BA940 (NAVIGATION 153: copies the packet's four fields)  // NAME 1 [packet]: the client's FAIL_IF '!packet'

	float Unknown08;					// +0x08: a range (radar_system_data_cache.cpp's 0x7BE3A0; the packet's Signature)  // NAME 1: neutral (D10)
	bool Unknown0C;						// +0x0C (starmap_data.cpp's 0x7BE970: false shows 'm_unknown'; the packet's PlayerHasVisited)  // NAME 1: neutral (D10)
	char Unknown0D[0x10 - 0x0D];		// +0x0D
	int Unknown10;						// +0x10 (nav_list_builder.cpp's 0x7B8210: 1 makes the object a node of the search; the packet's NavType, -1 from the constructor)  // NAME 1: neutral (D10)
	bool Unknown14;						// +0x14 (0x7BE3A0: in range whatever the distance; the packet's IsHuge)  // NAME 1: neutral (D10)
	char Unknown15[0x18 - 0x15];		// +0x15
};

#endif //__NAVIGATION_DATA_H
