// Earth & Beyond client (reconstruction)
/*
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
#include "vector3.h"

class RadarDataClass : public ClientObjectDataClass  // NAME 5: the client's typeid name .PAVRadarDataClass@@ (co_data.cpp's registry)
{
public:
	RadarDataClass(ClientObjectClass *co);			// 0x7BB1D0 (radar_data.cpp)

	void Unknown_7BB570();								// 0x7BB570 (radar_system.cpp's 0x7BCA70)
	void Unknown_7BB8C0(int state);						// 0x7BB8C0: 0 none, 1 hovered, 2 targeted (radar_system.cpp)  // NAME 2 [state]: what 0x7BD6E0 / 0x7BD860 pass

	int Unknown08;						// +0x08 (gutter_data.cpp's 0x7B63C0: 1 and 2 show the labels; 0x7c bytes: co_data.cpp's new)  // NAME 1: neutral (D10)
	Vector3 Color;						// +0x0C (target_reticle.cpp 0x7C5690 colours the corners with it)  // NAME 2: what it does
	char Unknown18[0x78 - 0x18];		// +0x18: not rebuilt
	bool Unknown78;						// +0x78 (0x7C4940 tests it)
	char Unknown79[0x7C - 0x79];		// +0x79
};

#endif //__RADAR_DATA_H
