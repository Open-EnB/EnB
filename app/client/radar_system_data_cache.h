// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\radar_system_data_cache.h (name assumed from radar_system_data_cache.cpp,
**	whose __FILE__ is the client's 0xB9F32C)
**
**	One object's cached view for one radar pass: radar_system.cpp's 0x7BCA70 makes one on its stack (0x34 bytes) and
**	moves it from object to object. 0x34 bytes, no virtuals.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __RADAR_SYSTEM_DATA_CACHE_H  // NAME 3 [radar_system_data_cache.h]: header name assumed from the client's radar_system_data_cache.cpp
#define __RADAR_SYSTEM_DATA_CACHE_H

#include "vector3.h"

class ClientObjectClass;
class GutterDataClass;
class NavigationDataClass;
class RadarDataClass;
class StarmapDataClass;

class RadarSystemDataCacheClass	// NAME 3: class-from-file (D24) for radar_system_data_cache.cpp
{
public:
	RadarSystemDataCacheClass(ClientObjectClass *player, float extra_range);	// 0x7BDFC0  // NAME 2 [extra_range]: 0x7BE3A0 adds it; radar_system.cpp's 0x7BCA70 passes 0
	~RadarSystemDataCacheClass();							// 0x7BE060

	void Set_Object(ClientObjectClass *object);				// 0x7BE080  // NAME 2: what it does
	RadarDataClass *Get_Radar_Data(bool create);			// 0x7BE0D0  // NAME 2 [Get_Radar_Data, create]: co_data.h's
	GutterDataClass *Get_Gutter_Data(bool create);			// 0x7BE120 (gutter_radar.cpp's 0x7B7040)  // NAME 2 [Get_Gutter_Data, create]: co_data.h's
	StarmapDataClass *Get_Starmap_Data(bool create);		// 0x7BE170  // NAME 2 [Get_Starmap_Data, create]: co_data.h's
	NavigationDataClass *Get_Navigation_Data();				// 0x7BE1C0  // NAME 2: co_data.h's
	void Remove_Radar_Data();								// 0x7BE210  // NAME 2: co_data.h's
	void Remove_Gutter_Data();								// 0x7BE260  // NAME 2: co_data.h's
	void Remove_Starmap_Data();								// 0x7BE2B0  // NAME 2: co_data.h's
	void Remove_Navigation_Data();							// 0x7BE300  // NAME 2: co_data.h's
	bool Get_Is_Targetable();								// 0x7BE350 (cached)  // NAME 2: the FAIL_IF's IsTargetable
	bool Get_Is_In_Range();									// 0x7BE3A0 (cached)  // NAME 2: what it computes
	const Vector3 &Get_Player_Position();					// 0x7BE520  // NAME 2: what it returns
	float Get_Player_Radius();								// 0x7BE560  // NAME 2: what it returns

	ClientObjectClass *Player;								// +0x00 (the FAIL_IF's name)
	Vector3 PlayerPosition;									// +0x04  // NAME 2: what it holds
	float PlayerRadius;										// +0x10  // NAME 2: what it holds
	bool IsPlayer;											// +0x14  // NAME 2: what it holds
	float ExtraRange;										// +0x18  // NAME 2: what it does
	ClientObjectClass *ClientObject;						// +0x1C (the FAIL_IF's name)
	RadarDataClass *RadarData;								// +0x20  // NAME 2: what it caches
	GutterDataClass *GutterData;							// +0x24  // NAME 2: what it caches
	StarmapDataClass *StarmapData;							// +0x28  // NAME 2: what it caches
	NavigationDataClass *NavigationData;					// +0x2C  // NAME 2: what it caches
	bool IsTargetable;										// +0x30 (the FAIL_IF's name)
	bool IsTargetableValid;									// +0x31  // NAME 2: what it holds
	bool IsInRange;											// +0x32  // NAME 2: what it holds
	bool IsInRangeValid;									// +0x33  // NAME 2: what it holds
};

#endif //__RADAR_SYSTEM_DATA_CACHE_H
