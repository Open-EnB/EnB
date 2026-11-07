// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\navigation_data.h (name assumed from navigation_data.cpp)
**
**	A client object's navigation data (vtable 0xB065C8), bound to the object through co_data.cpp;
**	declared as far as co_data.cpp needs it (its constructor and size).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __NAVIGATION_DATA_H  // NAME 3 [navigation_data.h]: header name assumed from the client's navigation_data.cpp
#define __NAVIGATION_DATA_H

#include "co_data.h"

class NavigationDataClass : public ClientObjectDataClass  // NAME 5: the client's typeid name .PAVNavigationDataClass@@ (co_data.cpp's registry)
{
public:
	NavigationDataClass(ClientObjectClass *co);			// 0x7BA8B0 (navigation_data.cpp)

	float Unknown08;					// +0x08: a range (radar_system_data_cache.cpp's 0x7BE3A0)  // NAME 1: neutral (D10)
	char Unknown0C[0x14 - 0x0C];		// +0x0C: not rebuilt (0x18 bytes: co_data.cpp's new)
	bool Unknown14;						// +0x14 (0x7BE3A0: in range whatever the distance)  // NAME 1: neutral (D10)
	char Unknown15[0x18 - 0x15];		// +0x15
};

#endif //__NAVIGATION_DATA_H
