/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\soptions.h (name assumed from soptions.cpp)
**
**	SOptionsClass, the client's options: SClient's second base (+0x1150). 0x48 bytes, constructor
**	0x75A280 (the render budgets from the registry key 'Software\Westwood Studios\Earth and
**	Beyond\Render', read only), destructor 0x75A4E0, vtable 0xB03C00 (one slot; SClient's copy
**	0xB0323C). soptions.cpp isn't rebuilt: declared as far as SClient uses it
**	(docs/kernel-map-step9.md 2.2, D90 9d).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef SOPTIONS_H  // NAME 3 [soptions.h]: header name assumed from the client's soptions.cpp
#define SOPTIONS_H

#include "vector.h"

class INIClass;

// One camera mode (0x14 bytes): 0x75A5F0 reads them from an INI section by these key names
struct CameraModeStruct		// NAME 2: what it holds
{
	float Heading;										// +0x00 'Heading' (degrees in the INI, radians here)
	float Distance;										// +0x04 'Distance'
	float Tilt;											// +0x08 'Tilt' (radians)
	float FOV;											// +0x0C 'FOV' (radians; 90 degrees by default)
	float ElevationOffset;								// +0x10 'ElevationOffset'

	bool operator==(const CameraModeStruct &that) const;	// (VectorClass's virtuals compare elements; not seen in the client)
	bool operator!=(const CameraModeStruct &that) const;
};

class SOptionsClass		// NAME 3: class-from-file (D24) for soptions.cpp
{
public:
	SOptionsClass();									// 0x75A280
	~SOptionsClass();									// 0x75A4E0 (not virtual: it stores its vtable first)

	virtual void Set_Camera_Mode(unsigned cameramode);	// +0x00 0x75A530 (assert 'cameramode >=0 && cameramode < CameraModeCount', l.53)  // NAME 2: what it does

	void Unknown_75A570();								// 0x75A570
	void Unknown_75A5A0();								// 0x75A5A0
	void Unknown_75A5D0();								// 0x75A5D0 (SClient::Init without 'Bridge_Mode')
	void Load_Camera_Mode(INIClass &ini, const char *section);	// 0x75A5F0 (assert 'CameraModeCount <= (unsigned int)CameraModes.Length()', l.93)  // NAME 2: what it does  // NAME 1 [ini, section]: the arguments' order is unchecked

	bool Unknown04;										// +0x04
	bool Unknown05;										// +0x05
	bool Unknown06;										// +0x06 (true)
	int PolyBudget;										// +0x08 the registry's 'PolyBudget'  // NAME 3: the registry value's name
	int StarBudget;										// +0x0C 'StarBudget'  // NAME 3: the registry value's name
	int LandPolyBudget;									// +0x10 'LandPolyBudget'  // NAME 3: the registry value's name
	int LandLOD;										// +0x14 'LandLOD'  // NAME 3: the registry value's name
	int TextureReduction;								// +0x18 'TextureReduction'  // NAME 3: the registry value's name
	int TextureFilter;									// +0x1C 'TextureFilter' (WW3D::Set_Texture_Filter)  // NAME 3: the registry value's name
	bool Unknown20;										// +0x20
	float Unknown24;									// +0x24 (2.0)
	bool Unknown28;										// +0x28 (true)
	unsigned CameraMode;								// +0x2C (Set_Camera_Mode's; SClient::Init indexes CameraModes with it)  // NAME 3: the assert's 'cameramode'
	unsigned CameraModeCount;							// +0x30 (the assert's name)
	unsigned LastCameraMode;							// +0x34 (1; Set_Camera_Mode keeps the last mode that isn't 0)  // NAME 2: what it holds
	VectorClass<CameraModeStruct> CameraModes;			// +0x38 vtable 0xB03C04 (the assert's name)
};

#endif
