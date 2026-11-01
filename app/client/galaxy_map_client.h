// Earth & Beyond client (reconstruction): app\client\galaxy_map_client.h. Notes at the end of the file.
/*
**	The client's galaxy map (SClient +0x12A0, made by SClient::Init 0x741180): galaxy_map.h's
**	GalaxyMapClass filled from the GALAXY_MAP packets (cpacket.cpp's 0x73AC60 hands each type
**	to one of the Unknown_ functions), the player's explored sectors and the galaxy_map_c.ini
**	scales the galaxy map's renderer reads.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __GALAXY_MAP_CLIENT_H  // NAME 3 [galaxy_map_client.h]: header name assumed from the client's galaxy_map_client.cpp
#define __GALAXY_MAP_CLIENT_H

#include <string>
#include "vector2.h"
#include "vector3.h"
#include "..\common\galaxy_map.h"

class SClient;
class LinearBufferClass;
class BitSetClass;

// vtable 0xB057E8 (the deleting destructor 0x7A4970, GalaxyMapClass::Clear), 0xCC bytes
class GalaxyMapClientClass : public GalaxyMapClass	// NAME 3: class-from-file (D24) for app\client\galaxy_map_client.cpp
{
public:
	GalaxyMapClientClass();																// 0x7A4830
	virtual ~GalaxyMapClientClass();													// 0x7A49A0

	void Load_Ini();																	// 0x7A4B10  // NAME 2: reads galaxy_map_c.ini's [GalaxyMap]
	void Unknown_7A4C50(SClient *sclient, LinearBufferClass &buffer);				// 0x7A4C50 (type 4)  // NAME 1 [sclient, buffer]: what the handler passes
	void Unknown_7A4FC0(LinearBufferClass &buffer);									// 0x7A4FC0 (type 5: a system)  // NAME 1 [buffer]: the packet's data
	void Unknown_7A50A0(LinearBufferClass &buffer);									// 0x7A50A0 (types 6, 7, 8, 11: a sector)  // NAME 1 [buffer]: the packet's data
	void Unknown_7A5180(LinearBufferClass &buffer);									// 0x7A5180 (type 9: a gate)  // NAME 1 [buffer]: the packet's data
	void Unknown_7A5260(LinearBufferClass &buffer);									// 0x7A5260 (type 2: ids to remove)  // NAME 1 [buffer]: the packet's data
	void Unknown_7A52F0(LinearBufferClass &buffer);									// 0x7A52F0 (type 3: the explored bits)  // NAME 1 [buffer]: the packet's data
	bool Unknown_7A5400(const RefCountPtr<GalaxyMapSectorClass> &sector);			// 0x7A5400 (0x7A8B70)  // NAME 1 [sector]: the assert's '!sector'
	Vector3 Unknown_7A5470(const Vector3 &position);								// 0x7A5470 (0x7AAB60: SystemPosScale, GalaxyShift)  // NAME 1 [position]: what it scales
	Vector3 Unknown_7A54F0(const Vector3 &position);								// 0x7A54F0 (SectorPosScale)  // NAME 1 [position]: what it scales
	bool Unknown_7A5540(SClient *client);												// 0x7A5540 (galaxy_map_pip.cpp's 0x5408B0: toggles ShowAll)  // NAME 1 [client]: the assert's '!client'

	int Version;																		// +0x78
	int Unknown7C;																		// +0x7C (0x7A4C50 counts its requests)
	BitSetClass *Unknown80;																// +0x80 (0x7A52F0's bits; 0x7A5400 tests a sector's GalaxyMapID)
	int Size;																			// +0x84 (100)  // NAME 2: the ini's 'Size'
	float SystemPosScale;																// +0x88 (1.0f)  // NAME 2: the ini's
	float SectorPosScale;																// +0x8C (1.0f)  // NAME 2: the ini's
	Vector2 GalaxyShift;																				// +0x90 (the constructor's inline Vector2(0, 0): float members leave its stores out of order)  // NAME 2: the ini's GalaxyShiftX / GalaxyShiftY
	bool ShowAll;																		// +0x98  // NAME 2: the option 'Galaxymap_Show_All'
	std::string Unknown9C;																// +0x9C (0x7A4C50 reads it)
	std::string UnknownAC;																// +0xAC (0x7A4C50 reads it)
	std::string UnknownBC;																// +0xBC (0x7A4C50 reads it)
};

#endif

/*
**	Reconstruction notes
**
**	The layout from the constructor 0x7A4830 (GalaxyMapClass's 0x835DD0, then +0x78..+0xC8, then its
**	vtable) and the destructor 0x7A49A0 (writes ShowAll back to the option, frees the three strings,
**	then GalaxyMapClass's 0x836240; Unknown80 isn't freed). The client's header name isn't known.
*/
