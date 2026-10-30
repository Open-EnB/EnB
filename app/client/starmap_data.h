// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\starmap_data.h (name assumed from starmap_data.cpp)
**
**	A client object's starmap data (vtable 0xB06980, base radar_icon_data.cpp's 0x7BC040), bound to the
**	object through co_data.cpp; declared as far as co_data.cpp needs it (its constructor and size).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __STARMAP_DATA_H  // NAME 3 [starmap_data.h]: header name assumed from the client's starmap_data.cpp
#define __STARMAP_DATA_H

#include "radar_icon_data.h"

class StarmapDataClass : public RadarIconDataClass  // NAME 5: the client's typeid name .PAVStarmapDataClass@@ (co_data.cpp's registry)
{
public:
	StarmapDataClass(ClientObjectClass *co);			// 0x7BE640 (starmap_data.cpp)

	char Unknown08[0x3C - 0x08];		// +0x08: not rebuilt (0x3c bytes: co_data.cpp's new)
};

#endif //__STARMAP_DATA_H
