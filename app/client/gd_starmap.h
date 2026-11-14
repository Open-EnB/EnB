// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\gd_starmap.h (name assumed from gd_starmap.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\client\gd_starmap.cpp'; the client holds no string of this header)
**
**	The gadget over the star map window's PIP (starmap_pip.cpp's slot +0x18 news it): gd_pip_scene.h's gadget with
**	the star map's PIP (G_ASSERT 'StarmapPIP'); the first button over an object targets it, elsewhere centres the
**	map on the pointer, a drag with the first button pans the map, with the second zooms it. Table 0xB05F54 (29
**	slots: GadgetPipSceneClass's, +0x2C its own), constructor 0x7B4700, 0x50 bytes.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef GD_STARMAP_H  // NAME 3 [gd_starmap.h]: header name assumed from the client's gd_starmap.cpp
#define GD_STARMAP_H

#include "gd_pip_scene.h"

class StarmapPipClass;	// starmap_pip.h's: the star map's PIP

class GadgetStarmapClass : public GadgetPipSceneClass  // NAME 2: class-from-file (D24) for app\client\gd_starmap.cpp, as gd_pip_scene.cpp's GadgetPipSceneClass
{
public:
	GadgetStarmapClass(StarmapPipClass *pip);			// 0x7B4700 (G_ASSERT 'StarmapPIP' l.60)  // NAME 1 [pip]: the base's
	virtual ~GadgetStarmapClass();						// +0x00 0x7B47C0 (deleting 0x7B4790)
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x2C 0x7B47F0 (FAIL_IF '!StarmapPIP' l.75: buttons 1 / 5 hold the gadget; a click targets the hovered object or centres the map, a drag pans or zooms)  // NAME 1 [event, x, y, mask]: GadgetClass's

	void Unknown_7B4BD0();								// 0x7B4BD0 (FAIL_IF '!StarmapPIP' l.153: the map centred on Position)
	void Unknown_7B4C50();								// 0x7B4C50 (the first button's drag: the map panned by the pointer's move)
	void Unknown_7B4D80();								// 0x7B4D80 (FAIL_IF '!StarmapPIP' l.193: the second button's drag, the zoom by its vertical move)

	StarmapPipClass *StarmapPIP;						// +0x4C (G_ASSERT 'StarmapPIP')  // NAME 3: the asserts'
};

#endif
