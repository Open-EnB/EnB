// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\radar_system.h (name assumed from radar_system.cpp, whose __FILE__ is the
**	client's 0xB9F270)
**
**	The radar (the main view's +0x80): each frame it walks the client's objects for the gutter and the starmap, and holds
**	the hovered and the targeted object. 0x50 bytes, no virtuals. Included before sclient.h, whose stand-in of the same
**	layout gives way to this one (interim placement, D137: the rest of the tree still reads sclient.h's).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __RADAR_SYSTEM_H  // NAME 3 [radar_system.h]: header name assumed from the client's radar_system.cpp
#define __RADAR_SYSTEM_H

#include "ref_ptr.h"
#include "ui_cpit.h"

class SClient;
class GutterRadarClass;
class NavListBuilderClass;
class NavigationListClass;
class StarmapPipClass;
class RadarSystemClass;
class RadarSystemDataCacheClass;
class RefCountClass;

// The radar's watch of a client object (vtable 0xB06858)
class RadarSystemWatchClass : public UnknownObjectWatchClass	// NAME 1: neutral (D10)
{
public:
	RadarSystemWatchClass(RadarSystemClass *owner) : Owner(owner) {}	// (inline: 0x7BC770)  // NAME 2 [owner]: what it holds
	virtual void Unknown_Watch_0C();					// +0x0C 0x7BDDF0: the object goes
	void operator = (ClientObjectClass *object) { Unknown_08(object); }	// (the base's: the implicit copy assignment hides it)

	RadarSystemClass *Owner;							// +0x18  // NAME 2: what it holds
};

class RadarSystemClass	// NAME 3: class-from-file (D24) for radar_system.cpp
{
public:
	RadarSystemClass(SClient *client);					// 0x7BC770 (the main view's 0x50BA60)  // NAME 2 [client]: what it stores
	~RadarSystemClass();								// 0x7BC930 (the main view's Shutdown 0x50C2F0)
	void Unknown_7BD920(int index);						// 0x7BD920 (cpacket.cpp: WARP_INDEX 156)  // NAME 1 [index]: the packet's Index
	void Unknown_7BD6E0(ClientObjectClass *target);		// 0x7BD6E0 (the main view's 0x513630 / 0x512400: the target)  // NAME 2 [target]: what 0x512400 passes
	void Unknown_7BCA40(bool arg);						// 0x7BCA40 (the main view's slot +0x18): both paths cleared  // NAME 1 [arg]: never read
	bool Unknown_7BD980();								// 0x7BD980: the warp path is built
	const NavigationListClass &Unknown_7BD960();		// 0x7BD960
	void Unknown_7BCA70(GutterRadarClass *object, StarmapPipClass *arg, const RefCountPtr<RefCountClass> &scene);	// 0x7BCA70 (the main view's 0x512A00, every frame)  // NAME 2 [object, arg, scene]: the gutter, the starmap, the scene

	ClientObjectClass *Unknown_7BD4C0();				// 0x7BD4C0: the next object on the starmap after the target
	ClientObjectClass *Unknown_7BD5D0();				// 0x7BD5D0: the one before it
	void Unknown_7BD860(ClientObjectClass *hover);		// 0x7BD860  // NAME 2 [hover]: what it sets
	bool Unknown_7BDBF0(RadarSystemDataCacheClass &entry);	// 0x7BDBF0: shows in the gutter  // NAME 2 [entry]: as the gutter's Update
	bool Unknown_7BDC70(RadarSystemDataCacheClass &entry);	// 0x7BDC70: shows on the starmap  // NAME 2 [entry]: as the gutter's Update
	void Unknown_7BDD50(RadarSystemDataCacheClass &entry, const RefCountPtr<RefCountClass> &scene);	// 0x7BDD50  // NAME 2 [entry, scene]: as 0x7BCA70's

	SClient *Client;									// +0x00 (the G_ASSERT's name)
	RadarSystemWatchClass Hover;						// +0x04  // NAME 2: 0x7BD860 sets it
	RadarSystemWatchClass Target;						// +0x20  // NAME 2: 0x7BD6E0 sets it
	NavListBuilderClass *WarpPath;						// +0x3C  // NAME 2: the starmap's FAIL_IF '!WarpPath'
	NavListBuilderClass *TargetPath;					// +0x40  // NAME 2: the starmap's FAIL_IF '!TargetPath'
	float ScanRange;									// +0x44 (the avatar's aux field)  // NAME 2: what it holds
	int GutterCount;									// +0x48  // NAME 2: what it holds
	bool Unknown4C;										// +0x4C: the mouse is on the starmap
	bool Unknown4D;										// +0x4D: names of the objects under the mouse ('/art name', 0x74ABC0)
};

#endif //__RADAR_SYSTEM_H
