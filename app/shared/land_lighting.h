/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\ (file name unknown: no assert names it; land_lighting.h is
**	a proposal)
**
**	The land's lighting record, out of land.h so that sclient.h can hold it as a member: the main view's
**	+0x178..+0x1A4 (its constructor 0x50B360 stores these defaults; Update 0x50F1B0 fills it and hands it to
**	LandClass::Update_Lightmap_*).
*/

#if defined(_MSC_VER)
#pragma once
#endif

// NAME 1 [land_lighting.h]: no assert or __FILE__ names the file
#ifndef __LAND_LIGHTING_H
#define __LAND_LIGHTING_H

#include "vector3.h"

// NAME 2: VIDTEST's name for the 0x30-byte lighting record its Update_Lighting 0x4250B0 hands the land (esp+0x74)
struct LandLightingStruct
{
	Vector3		LightColor;		// clamped LightColor x LightIntensity
	Vector3		SunDirection;	// towards the sun
	float		SunAngle;		// the sun's elevation (VIDTEST: pi/2 - acos(sun.z)); Prepare_Lighting compares the horizons with it
	float		ShadowBias;
	Vector3		SkyColor;		// clamped SkyColor x SkyIntensity
	bool		Valid;
	LandLightingStruct(void) :
		LightColor(0.5f, 0.5f, 0.5f),
		SunDirection(0.0f, 0.0f, 1.0f),
		SunAngle(0.0f),
		ShadowBias(0.05f),
		SkyColor(0.1f, 0.1f, 0.5f),
		Valid(true)
	{
	}
};

#endif //__LAND_LIGHTING_H
