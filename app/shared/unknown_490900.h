// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (PROVISIONAL, D137: a neutral name, beside unknown_490900.cpp in app\shared,
**	between enbassetmgr.cpp and proj_man.cpp)
**
**	The viewer settings scriptedmovie.cpp's view (+0x68 ViewerSettings) and sbaseview.cpp's starbase view (+0x68) make:
**	0x1C bytes, constructor 0x490900, destructor 0x4909A0, the INI loader 0x4909D0 ('Settings': AmbientLight,
**	SceneLight..., Background; sbaseview's lights loader 0x6A0590 calls it). Its first member is the scene light
**	(a LightClass: slot 101 Set_Force_Visible and the far attenuation at +0xC8 / +0xCC, 0x6A0590).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UNKNOWN_490900_H  // NAME 1 [unknown_490900.h]: neutral (D137)
#define UNKNOWN_490900_H

#include "vector3.h"

class LightClass;

struct ScriptedMovieViewerSettingsStruct	// NAME 1: neutral (D10), scriptedmovie.cpp's
{
	ScriptedMovieViewerSettingsStruct();	// 0x490900
	~ScriptedMovieViewerSettingsStruct();	// 0x4909A0
	bool Unknown_4909D0(const char *name, const Vector3 &position);	// 0x4909D0: the INI name's settings; false without them (0x7F0460 tests it and passes its light mesh's position)  // NAME 1 [name, position]: neutral / what 0x7F0460 passes

	LightClass *Unknown00;	// +0x00 the scene light (0x6A0540 / the scripted movie's shutdown remove it)
	Vector3 BackgroundColor;	// +0x04  // NAME 2 [BackgroundColor]: what Begin_Render gets (scriptedmovie.cpp)
	Vector3 Unknown10;	// +0x10 (the lights loader 0x6A0590 hands it to Set_Ambient_Light)
};

#endif
