// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (PROVISIONAL: app\Client\starbase_lights.h, named after its INI file, D137)
**
**	Unknown_764480Class: the starbase lights manager (0x10 bytes). WinMain makes one ("initializing starbase
**	lights manager") and hands it to 0x764510, which keeps it in 0xBE82D0; 0x640810 and 0x646A00 read its INI.
**	Its constructor calls INIClass::INIClass and stores no vtable of its own, so the INIClass is a member at
**	+0x00, not a base.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef STARBASE_LIGHTS_H  // NAME 3 [starbase_lights.h]: its INI file's name
#define STARBASE_LIGHTS_H

#include "..\..\wwlib\ini.h"

class Unknown_764480Class	// NAME 1 [Unknown_764480Class]: neutral (D10)
{
public:
	Unknown_764480Class();
	~Unknown_764480Class();

	INIClass INI;			// +0x00 ('starbase_lights.ini')  // NAME 2 [INI]: env_map_manager.h's
};

void Unknown_764510(Unknown_764480Class *manager);	// NAME 1 [Unknown_764510]: sets 0xBE82D0  // NAME 1 [manager]: neutral
extern Unknown_764480Class *Unknown_BE82D0;			// NAME 1 [Unknown_BE82D0]: neutral (D10)

#endif
