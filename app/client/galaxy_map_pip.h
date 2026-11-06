// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\galaxy_map_pip.h (name assumed from galaxy_map_pip.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\client\galaxy_map_pip.cpp'; the client holds no string of this header)
**
**	The galaxy map picture-in-picture: the map's renderer and a focal point the camera eases between two
**	others, over a map PIP (map_pip.h). Constructor 0x53F390, table 0xAEEE94, 0x11C bytes.
**	Callers: ui_starmap.cpp's 0x7C63D0 news it and 0x7C8980 runs it, gd_galaxy_map.cpp's gadget steers it.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __GALAXY_MAP_PIP_H  // NAME 3 [galaxy_map_pip.h]: header name assumed from the client's galaxy_map_pip.cpp
#define __GALAXY_MAP_PIP_H

#include "always.h"
#include "ref_ptr.h"
#include "vector2.h"
#include "sphere.h"
#include "matrix3d.h"
#include "map_pip.h"
#include "galaxy_map_focal_point.h"

class GalaxyMapClientClass;
class SceneClass;

// galaxy_map_render.cpp (not in the tree yet), as far as this file calls it. 0xC0 bytes.
// PLACEMENT: provisional (D137): the client's galaxy_map_render.h gives it a header; no other user in the tree yet
class GalaxyMapRenderClass  // NAME 3: class-from-file (D24) for app\Client\galaxy_map_render.cpp
{
public:
	GalaxyMapRenderClass(GalaxyMapClientClass *map);	// 0x7A6020 (FAIL_IF 'GalaxyMap')  // NAME 2 [map]: the assert's
	~GalaxyMapRenderClass();							// 0x7A64F0

	void Unknown_7A6A70();								// 0x7A6A70
	void Unknown_7A6AB0(const RefCountPtr<SceneClass> &scene, const RefCountPtr<CameraClass> &camera);	// 0x7A6AB0  // NAME 1 [scene, camera]: what the caller passes
	void Unknown_7A6DD0();								// 0x7A6DD0
	void Unknown_7A7070(bool arg);						// 0x7A7070 (FAIL_IF '!GalaxyMap')  // NAME 1 [arg]: neutral
	void Unknown_7A72C0(float x, float y);				// 0x7A72C0 (FAIL_IF '!Camera')  // NAME 1 [x, y]: neutral
	void Unknown_7A7450();								// 0x7A7450
	SphereClass Unknown_7A83D0(bool arg);					// 0x7A83D0  // NAME 1 [arg]: neutral
	void Unknown_7A8770(int arg);						// 0x7A8770 (the aux data's SectorNum)  // NAME 1 [arg]: what the caller passes
	SphereClass Unknown_7A89A0();							// 0x7A89A0

	char Unknown_Pad00[0x14];
	Vector2 Unknown14;									// +0x14 (0x53FD10)
	Vector2 Unknown1C;									// +0x1C (0x53FD10)
	char Unknown_Pad24[0xB4 - 0x24];
	float UnknownB4;									// +0xB4 (0x53FCA0)
	char Unknown_PadB8[0xC0 - 0xB8];
};

// gd_galaxy_map.cpp's gadget (not in the tree yet), as far as this file calls it. 0x50 bytes.
// PLACEMENT: provisional (D137): the client's gd_galaxy_map.cpp gives it a file; no other user in the tree yet
class GalaxyMapPipClass;
class GadgetGalaxyMapClass : public GadgetPipSceneClass  // NAME 3: class-from-file (D24) for app\client\gd_galaxy_map.cpp
{
public:
	GadgetGalaxyMapClass(GalaxyMapPipClass *pip);		// 0x668E30 (G_ASSERT 'GalaxyMap', +0x4C)  // NAME 1 [pip]: what the caller passes

	virtual bool Unknown_30(float x, float y);			// +0x30 (GadgetClass's pure slot; table 0xAFA120)  // NAME 1 [x, y]: GadgetClass's

	GalaxyMapPipClass *GalaxyMap;						// +0x4C  // NAME 3: the assert's
};

class GalaxyMapPipClass : public MapPipClass  // NAME 3: class-from-file (D24) for app\client\galaxy_map_pip.cpp
{
public:
	GalaxyMapPipClass(SClient *client);					// 0x53F390  // NAME 1 [client]: neutral
	virtual ~GalaxyMapPipClass();						// +0x00 0x53F540 (deleting 0x53F510)
	virtual bool Unknown_04(const RefCountPtr<InterfaceClass> &iface_ptr, const RefCountPtr<RenderObjClass> &vp, int arg);	// +0x04 0x53F770
	virtual bool Unknown_08();							// +0x08 0x53F7E0
	virtual bool Unknown_0C(bool on);					// +0x0C 0x53F800
	virtual GadgetClass *Unknown_18();					// +0x18 0x53F6F0 (news the 0x50-byte gadget 0x668E30)
	virtual void Unknown_1C(int arg);					// +0x1C 0x53F920 (the grid in or out of the scene)
	virtual void Unknown_20(int arg);					// +0x20 0x53F4F0
	virtual void Unknown_24(int arg);					// +0x24 0x5408B0 (FAIL_IF '!map': toggles the map's ShowAll and says so)

	void Unknown_53F9E0();								// 0x53F9E0 (per frame: the ease, the renderer; ui_starmap.cpp's 0x7C8980)
	void Unknown_53FCA0(float arg);						// 0x53FCA0 (ui_starmap's 0x7C8980, gd_galaxy_map's 0x669460)  // NAME 1 [arg]: neutral
	void Unknown_53FD10(const Vector2 &arg);			// 0x53FD10 (gd_galaxy_map's 0x668F20, 0x6693A0)  // NAME 1 [arg]: neutral
	void Unknown_53FD60(const Vector2 &arg);			// 0x53FD60 (gd_galaxy_map's 0x668F20, 0x6692E0)  // NAME 1 [arg]: neutral
	void Unknown_53FDA0(bool arg);						// 0x53FDA0 (gd_galaxy_map's 0x668F20)  // NAME 1 [arg]: neutral
	void Unknown_53FE10();								// 0x53FE10 (gd_galaxy_map's 0x668F20)
	void Unknown_53FE30();								// 0x53FE30 (loads the background and the grid)
	void Unknown_5404E0(bool arg);						// 0x5404E0  // NAME 1 [arg]: neutral
	void Unknown_5406A0(const SphereClass &sphere, float angle);	// 0x5406A0 (where the ease goes)  // NAME 2 [sphere]: Center, then Radius / tan(fov / 2)  // NAME 1 [angle]: what it does with it

	bool Debug;											// +0x50 (-GALAXY_MAP_DEBUG)  // NAME 2: the command line's
	GalaxyMapRenderClass *Render;						// +0x54  // NAME 2: what it holds
	GalaxyMapFocalPointClass FocalPoint;				// +0x58 (the camera's, this frame)  // NAME 2: what it holds
	GalaxyMapFocalPointClass Target;					// +0x94 (the ease's end)  // NAME 2: what it holds
	GalaxyMapFocalPointClass Start;						// +0xD0 (the ease's start)  // NAME 2: what it holds
	float Ease;											// +0x10C (0 .. 1 while easing, else -1)  // NAME 2: what it holds
	float EaseRate;										// +0x110 (0x5406A0: 0.001)  // NAME 2: what it holds
	RefCountPtr<RenderObjClass> GalaxyBackground;		// +0x114  // NAME 3: the FAIL_IF's
	RefCountPtr<RenderObjClass> Grid;					// +0x118  // NAME 3: the FAIL_IF's
};

#endif // __GALAXY_MAP_PIP_H
