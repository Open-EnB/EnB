// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (PROVISIONAL: app\Client\env_map_manager.h beside env_map_manager.cpp, D137)
**
**	EnvironmentMapManagerClass: the environment maps (0x24 bytes): SClient +0x1338 and CharacterViewClass
**	+0x3A4 make one and Load 'envmaps.ini' into it; 0x760770 builds the reflection textures from it.
**	sclient.cpp, charview.cpp and scriptedmovie.cpp still declare their own views of it (same layout).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef ENV_MAP_MANAGER_H  // NAME 3 [env_map_manager.h]: beside the client's env_map_manager.cpp
#define ENV_MAP_MANAGER_H

#include <string>
#include <vector>
#include "..\..\wwlib\ini.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\ww3d2\texture.h"
#include "..\..\ww3d2\surfaceclass.h"
#include "..\..\ww3d2\textureloader.h"
#include "..\..\ww3d2\ww3dformat.h"
#include "..\..\ww3d2\assetmgr.h"
#include "..\..\ww3d2\dx8wrapper.h"

class SurfaceClass;

// One PROCTEX-NN section's base grid texture (0x14 bytes, destructor 0x67D360)
class Unknown67D360Class		// NAME 1: neutral (D10)
{
public:
	std::string Name;									// +0x00 (the section's BaseTexture)  // NAME 2: what it holds
	RefCountPtr<SurfaceClass> Surface;					// +0x10 (the assert's 'BaseGridTextures.back().Surface')
};

class EnvironmentMapManagerClass	// NAME 3: by convention (D24) from the client's env_map_manager.cpp
{
public:
	EnvironmentMapManagerClass() : Unknown10(2) {}
	void Load(const char *env_map_ini_filename);		// 0x760360: 'envmaps.ini'  // NAME 2: what it does
	void Unknown_760770(int base_asset, bool a);		// 0x760770: the reflection textures from BACK-%i  // NAME 1 [base_asset, a]: sclient.cpp's

	std::vector<Unknown67D360Class> BaseGridTextures;	// +0x00 (the assert's name)
	int Unknown10;										// +0x10 (2)  // NAME 1: neutral (D10)
	INIClass INI;										// +0x14  // NAME 2: what it is
};

#endif //ENV_MAP_MANAGER_H
