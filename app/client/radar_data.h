/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\radar_data.h (name assumed from radar_data.cpp)
**
**	A client object's radar data (vtable 0xB06720), bound to the object through co_data.cpp; declared
**	as far as co_data.cpp needs it (its constructor and size).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __RADAR_DATA_H  // NAME 3 [radar_data.h]: header name assumed from the client's radar_data.cpp
#define __RADAR_DATA_H

#include "co_data.h"

class RadarDataClass : public ClientObjectDataClass  // NAME 5: the client's typeid name .PAVRadarDataClass@@ (co_data.cpp's registry)
{
public:
	RadarDataClass(ClientObjectClass *co);			// 0x7BB1D0 (radar_data.cpp)

	char Unknown08[0x7C - 0x08];		// +0x08: not rebuilt (0x7c bytes: co_data.cpp's new)
};

#endif //__RADAR_DATA_H
