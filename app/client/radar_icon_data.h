// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\radar_icon_data.h (name assumed from radar_icon_data.cpp)
**
**	The base of the gutter and starmap data (constructor 0x7BC040, which calls ClientObjectDataClass's);
**	its own size and members are not rebuilt: declared only as their base.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __RADAR_ICON_DATA_H  // NAME 3 [radar_icon_data.h]: header name assumed from the client's radar_icon_data.cpp
#define __RADAR_ICON_DATA_H

#include "co_data.h"

class RadarIconDataClass : public ClientObjectDataClass  // NAME 3: by convention (D24) from the client's radar_icon_data.cpp
{
public:
	RadarIconDataClass(ClientObjectClass *co);			// 0x7BC040 (radar_icon_data.cpp)
};

#endif //__RADAR_ICON_DATA_H
