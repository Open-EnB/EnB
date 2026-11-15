// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (PROVISIONAL: app\Client\proj_decal_manager.h beside proj_decal_manager.cpp, D137)
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PROJ_DECAL_MANAGER_H	// NAME 3 [proj_decal_manager.h]: beside the client's proj_decal_manager.cpp
#define PROJ_DECAL_MANAGER_H

#include <vector>
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\ww3d2\texture.h"

class DecalGeneratorClass;
class RenderObjClass;
struct ProjectedDecalClass;

class Unknown758F40Class		// NAME 1: neutral (D10): 0x0C bytes, destructor 0x758F40
{
public:
	Unknown758F40Class() : BaseSize(0.0f), Lighting(false) {}

	float BaseSize;								// +0x00 (the assert's 'asset.BaseSize > 0.0f')
	RefCountPtr<TextureClass> Texture;			// +0x04 (the assert's 'asset.Texture')
	bool Lighting;								// +0x08  // NAME 2: from INI key 'Lighting'
};

class ProjDecalManagerClass		// NAME 3: class-from-file (D24) for proj_decal_manager.cpp (0x763050 loads 'proj_dec.ini' into it; 0x10 bytes, one vector)
{
public:
	void Load(const char *ini_filename);		// 0x763050: 'proj_dec.ini'  // NAME 2: what it does
	void Unknown_7633E0(DecalGeneratorClass *generator, const ProjectedDecalClass &proj_decal, RenderObjClass *model);	// 0x7633E0

	std::vector<Unknown758F40Class> ProjectedDecalAssetTable;	// +0x00 (the FAIL_IF's name)
};

#endif //PROJ_DECAL_MANAGER_H
