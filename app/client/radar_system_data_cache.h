// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\radar_system_data_cache.h (name assumed from radar_system_data_cache.cpp,
**	whose __FILE__ is the client's 0xB9F32C)
**
**	One object's cached view for one radar pass: radar_system.cpp's 0x7BCA70 makes one on its stack (0x34 bytes) and
**	moves it from object to object. Declared as far as radar_system.cpp and gutter_radar.cpp use it; not rebuilt.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __RADAR_SYSTEM_DATA_CACHE_H  // NAME 3 [radar_system_data_cache.h]: header name assumed from the client's radar_system_data_cache.cpp
#define __RADAR_SYSTEM_DATA_CACHE_H

#include "vector3.h"

class ClientObjectClass;
class GutterDataClass;
class RadarDataClass;
class RadarSystemClass;
class StarmapDataClass;

class RadarSystemDataCacheClass	// NAME 3: class-from-file (D24) for radar_system_data_cache.cpp
{
public:
	RadarSystemDataCacheClass(ClientObjectClass *avatar, RadarSystemClass *radar);	// 0x7BDFC0  // NAME 2 [avatar, radar]: what 0x7BCA70 passes
	~RadarSystemDataCacheClass();							// 0x7BE060

	void Set_Object(ClientObjectClass *object);				// 0x7BE080  // NAME 2: what it does
	RadarDataClass *Get_Radar_Data(bool create);			// 0x7BE0D0  // NAME 2 [Get_Radar_Data, create]: co_data.h's
	GutterDataClass *Get_Gutter_Data(bool create);			// 0x7BE120  // NAME 2 [Get_Gutter_Data, create]: co_data.h's
	StarmapDataClass *Get_Starmap_Data(bool create);		// 0x7BE170  // NAME 2 [Get_Starmap_Data, create]: co_data.h's
	int Unknown_7BE1C0();									// 0x7BE1C0
	bool Unknown_7BE350();									// 0x7BE350
	bool Unknown_7BE3A0();									// 0x7BE3A0

	ClientObjectClass *Avatar;								// +0x00  // NAME 2: what it holds
	Vector3 AvatarPosition;									// +0x04  // NAME 2: what it holds
	float AvatarRadius;										// +0x10  // NAME 2: what it holds
	bool Is_Avatar;											// +0x14: Object == Avatar  // NAME 2: what it holds
	RadarSystemClass *Radar;								// +0x18  // NAME 2: what it holds
	ClientObjectClass *Object;								// +0x1C  // NAME 2: what it holds
	char Unknown20[0x34 - 0x20];							// +0x20: not rebuilt
};

#endif //__RADAR_SYSTEM_DATA_CACHE_H
