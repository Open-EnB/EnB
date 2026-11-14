// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\gd_galaxy_map.h (name assumed from gd_galaxy_map.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\client\gd_galaxy_map.cpp'; the client holds no string of this header)
**
**	The gadget over the galaxy map's PIP (galaxy_map_pip.cpp's slot +0x18 news it): gd_pip_scene.h's gadget with
**	the galaxy map's PIP (G_ASSERT 'GalaxyMap'); a click with the first or second button goes to the map, a drag
**	with the first button pans it (with Ctrl held, the other move), with the second zooms it. Table 0xAFA120
**	(29 slots: GadgetPipSceneClass's, +0x2C and +0x30 its own), constructor 0x668E30, 0x50 bytes.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef GD_GALAXY_MAP_H  // NAME 3 [gd_galaxy_map.h]: header name assumed from the client's gd_galaxy_map.cpp
#define GD_GALAXY_MAP_H

#include "gd_pip_scene.h"

class GalaxyMapPipClass;	// galaxy_map_pip.h's: the galaxy map's PIP

class GadgetGalaxyMapClass : public GadgetPipSceneClass  // NAME 2: class-from-file (D24) for app\client\gd_galaxy_map.cpp, as gd_pip_scene.cpp's GadgetPipSceneClass
{
public:
	GadgetGalaxyMapClass(GalaxyMapPipClass *pip);		// 0x668E30 (G_ASSERT 'GalaxyMap' l.61)  // NAME 1 [pip]: the base's
	virtual ~GadgetGalaxyMapClass();					// +0x00 0x668EF0 (deleting 0x668EC0)
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x2C 0x668F20 (FAIL_IF '!GalaxyMap' l.76: buttons 1 / 5 hold the gadget; a click goes to the map, a drag pans or zooms)  // NAME 1 [event, x, y, mask]: GadgetClass's
	virtual bool Unknown_30(float x, float y);			// +0x30 0x6692B0 (the base's only)  // NAME 1 [x, y]: GadgetClass's

	void Unknown_6692E0();								// 0x6692E0 (FAIL_IF '!GalaxyMap' l.158: the drag with Ctrl held, 0x53FD60)
	void Unknown_6693A0();								// 0x6693A0 (FAIL_IF '!GalaxyMap' l.178: the first button's drag, 0x53FD10)
	void Unknown_669460();								// 0x669460 (FAIL_IF '!GalaxyMap' l.198: the second button's drag, the zoom by its vertical move)

	GalaxyMapPipClass *GalaxyMap;						// +0x4C (G_ASSERT 'GalaxyMap')  // NAME 3: the asserts'
};

#endif
