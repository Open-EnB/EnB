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
#include "vector3.h"

class LightClass;

class Unknown_764480Class	// NAME 1 [Unknown_764480Class]: neutral (D10)
{
public:
	Unknown_764480Class();
	~Unknown_764480Class();

	void Unknown_764530();								// 0x764530 (ui_starbase.cpp 0x646A00, before the lights are read)
	void Unknown_764550();								// 0x764550 (ui_starbase.cpp 0x646E00, after the lights are stored: the INI saved)
	LightClass *Unknown_764570(int a, int b, int c, int index);	// 0x764570 (ui_starbase.cpp 0x640810: a new light from the INI)  // NAME 1 [a, b, c, index]: neutral
	void Unknown_764880(int a, int b, int c, int index, LightClass *light);	// 0x764880 (0x646E00: each light)  // NAME 1 [a, b, c, index, light]: neutral
	void Unknown_764C30(int a, int b, int c, int index, const Vector3 &point);	// 0x764C30 (0x646E00: a pad point; index 1000 the ambient light's)  // NAME 1 [a, b, c, index, point]: neutral
	Vector3 Unknown_764CF0(int a, int b, int c);		// 0x764CF0 (0x646A00: the ambient colour)  // NAME 1 [a, b, c]: neutral
	void Unknown_764E20(int a, int b, int c, const Vector3 &color);	// 0x764E20 (0x646E00: the ambient colour)  // NAME 1 [a, b, c, color]: neutral
	float Unknown_764EA0(int a, int b, int c);			// 0x764EA0 (0x646A00: the fill)  // NAME 1 [a, b, c]: neutral
	void Unknown_764F60(int a, int b, int c, float fill);	// 0x764F60 (0x646E00: the fill)  // NAME 1 [a, b, c, fill]: neutral
	Vector3 Unknown_764AB0(int a, int b, int c, int index);	// 0x764AB0 (ui_starbase.cpp 0x648190: a light's pad point; index 1000 the ambient light's)  // NAME 1 [a, b, c, index]: neutral (0x640D80's three values and the light)

	INIClass INI;			// +0x00 ('starbase_lights.ini')  // NAME 2 [INI]: env_map_manager.h's
};

void Unknown_764510(Unknown_764480Class *manager);	// NAME 1 [Unknown_764510]: sets 0xBE82D0  // NAME 1 [manager]: neutral
extern Unknown_764480Class *Unknown_BE82D0;			// NAME 1 [Unknown_BE82D0]: neutral (D10)

#endif
