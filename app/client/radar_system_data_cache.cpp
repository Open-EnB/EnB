// radar_system_data_cache.cpp: one object's cached view for a radar pass (0x7BDFC0-0x7BE590); notes at the end
#include <string>
#include <float.h>
#include "radar_system_data_cache.h"
#include "co.h"
#include "co_data.h"
#include "navigation_data.h"
#include "..\shared\g_assert.h"

extern "C" bool Unknown_831F90(const Vector3 &a, float a_radius, const Vector3 &b, float b_radius, float range);	// 0x831F90 (distance_calc.h, C linkage): the two spheres within range; the client reads al, the definition returns an int  // NAME 1: neutral (D10)

// @0x7BDFC0
RadarSystemDataCacheClass::RadarSystemDataCacheClass(ClientObjectClass *player, float extra_range) :
	Player(player),
	PlayerPosition(0.0f, 0.0f, 0.0f),
	ExtraRange(extra_range),
	PlayerRadius(0.0f),
	IsPlayer(false),
	ClientObject(NULL),
	RadarData(NULL),
	GutterData(NULL),
	StarmapData(NULL),
	NavigationData(NULL),
	IsTargetable(false),
	IsTargetableValid(false),
	IsInRange(false),
	IsInRangeValid(false)
{
	if (Player) {
		PlayerPosition = Player->Unknown_24();
		PlayerRadius = Player->Unknown_30();
	}
}

// @0x7BE060
RadarSystemDataCacheClass::~RadarSystemDataCacheClass()
{
}

// @0x7BE080
void RadarSystemDataCacheClass::Set_Object(ClientObjectClass *object)
{
	ClientObject = object;
	IsPlayer = (object && (object == Player));
	RadarData = NULL;
	GutterData = NULL;
	StarmapData = NULL;
	NavigationData = NULL;
	IsTargetable = false;
	IsTargetableValid = false;
	IsInRange = false;
	IsInRangeValid = false;
}

// @0x7BE0D0
RadarDataClass *RadarSystemDataCacheClass::Get_Radar_Data(bool create)
{
	if (RadarData) return(RadarData);


































	FAIL_IF(!ClientObject) return(NULL);
	return(::Get_Radar_Data(ClientObject, create));
}
// @0x7BE120
GutterDataClass *RadarSystemDataCacheClass::Get_Gutter_Data(bool create)
{
	if (GutterData) return(GutterData);
	FAIL_IF(!ClientObject) return(NULL);
	return(::Get_Gutter_Data(ClientObject, create));
}
// @0x7BE170
StarmapDataClass *RadarSystemDataCacheClass::Get_Starmap_Data(bool create)
{
	if (StarmapData) return(StarmapData);
	FAIL_IF(!ClientObject) return(NULL);
	return(::Get_Starmap_Data(ClientObject, create));
}

// @0x7BE1C0
NavigationDataClass *RadarSystemDataCacheClass::Get_Navigation_Data()
{
	if (NavigationData) return(NavigationData);

	FAIL_IF(!ClientObject) return(NULL);
	return(::Get_Navigation_Data(ClientObject, false));
}
// @0x7BE210
void RadarSystemDataCacheClass::Remove_Radar_Data()
{
	FAIL_IF(!ClientObject) return;
	::Remove_Radar_Data(ClientObject);
	RadarData = NULL;
}
// @0x7BE260
void RadarSystemDataCacheClass::Remove_Gutter_Data()
{
	FAIL_IF(!ClientObject) return;
	::Remove_Gutter_Data(ClientObject);
	GutterData = NULL;
}
// @0x7BE2B0
void RadarSystemDataCacheClass::Remove_Starmap_Data()
{
	FAIL_IF(!ClientObject) return;
	::Remove_Starmap_Data(ClientObject);
	StarmapData = NULL;
}
// @0x7BE300
void RadarSystemDataCacheClass::Remove_Navigation_Data()
{
	FAIL_IF(!ClientObject) return;
	::Remove_Navigation_Data(ClientObject);
	NavigationData = NULL;
}
// @0x7BE350
bool RadarSystemDataCacheClass::Get_Is_Targetable()
{
	if (IsTargetableValid) return(IsTargetable);

	FAIL_IF(!ClientObject) return(false);
	IsTargetable = ClientObject->Unknown_6F5980();
	IsTargetableValid = true;
	return(IsTargetable);
}
// NOT EXACT: order (the client's always-in-range path returns on its own, before the distance test; ours shares the tail)
// @0x7BE3A0
bool RadarSystemDataCacheClass::Get_Is_In_Range()
{
	if (!IsInRangeValid) {



		FAIL_IF(!IsTargetable) return(false);
		FAIL_IF(!ClientObject) return(false);
		FAIL_IF(!Player) return(false);
		float range;
		if ((ClientObject->Type == 37) && ClientObject->AuxData) {
			range = *(float *)(ClientObject->AuxData + 0x234);
		} else {
			NavigationDataClass *nav = Get_Navigation_Data();
			if (!nav || nav->Unknown14) {
				goto always;
			}
			range = nav->Unknown08 + ExtraRange;
		}
		if (range == FLT_MAX) {
always:
			IsInRange = true;
		} else {
			Vector3 pos = ClientObject->Unknown_24();
			float radius = ClientObject->Unknown_30();
			IsInRange = Unknown_831F90(PlayerPosition, PlayerRadius, pos, radius, range);
		}
		IsInRangeValid = true;
	}
	return(IsInRange);
}

// @0x7BE520
const Vector3 &RadarSystemDataCacheClass::Get_Player_Position()
{








	G_ASSERT(Player);
	return(PlayerPosition);
}
// @0x7BE560
float RadarSystemDataCacheClass::Get_Player_Radius()
{
	G_ASSERT(Player);
	return(PlayerRadius);
}

/*
**	Notes
**
**	The object: 0x7BDFC0 (the constructor; the client's __FILE__ 0xB9F32C) to 0x7BE590; its fields' names are the
**	FAIL_IFs' ('!ClientObject', '!IsTargetable', '!Player').
*/
