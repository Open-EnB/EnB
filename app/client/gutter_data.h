/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\gutter_data.h (name assumed from gutter_data.cpp)
**
**	A client object's gutter data (vtable 0xB06068, base radar_icon_data.cpp's 0x7BC040), bound to the
**	object through co_data.cpp; declared as far as co_data.cpp needs it (its constructor and size).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __GUTTER_DATA_H  // NAME 3 [gutter_data.h]: header name assumed from the client's gutter_data.cpp
#define __GUTTER_DATA_H

#include "radar_icon_data.h"

class GutterDataClass : public RadarIconDataClass  // NAME 5: the client's typeid name .PAVGutterDataClass@@ (co_data.cpp's registry)
{
public:
	GutterDataClass(ClientObjectClass *co);			// 0x7B4F30 (gutter_data.cpp)

	char Unknown08[0x2C - 0x08];		// +0x08: not rebuilt (0x2c bytes: co_data.cpp's new)
};

#endif //__GUTTER_DATA_H
