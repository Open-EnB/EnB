// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\gutter_radar.h (name assumed from gutter_radar.cpp, whose __FILE__
**	is 'e:\build\beta\G\app\client\gutter_radar.cpp'; the client holds no string of this header)
**
**	The icons round the edge of the screen for objects off it, and the two target reticles: the main view's +0x84,
**	0x24 bytes, no virtuals.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __GUTTER_RADAR_H  // NAME 3 [gutter_radar.h]: header name assumed from the client's gutter_radar.cpp
#define __GUTTER_RADAR_H

#include "ref_ptr.h"
#include "vector3.h"

class SClient;
class SimpleSceneClass;
class ClientObjectClass;
class TargetReticleClass;

class MouseTargetClass;
class RadarSystemDataCacheClass;

class GutterRadarClass	// NAME 3: class-from-file (D24) for gutter_radar.cpp
{
public:
	GutterRadarClass(SClient *client, const RefCountPtr<SimpleSceneClass> &scene);	// 0x7B6BD0  // NAME 2 [client, scene]: what it stores
	~GutterRadarClass();															// 0x7B6D80

	void Set_Targets(ClientObjectClass *first, ClientObjectClass *second, MouseTargetClass *mouse_target);	// 0x7B6E40 (radar_system.cpp's 0x7BCA70)  // NAME 2: what it does
	void Update(RadarSystemDataCacheClass *entry);									// 0x7B7040 (radar_system.cpp's 0x7BCA70)  // NAME 2: what it does
	void Clear_Mouse_Target();														// 0x7B71B0 (radar_system.cpp's 0x7BCA70)  // NAME 2: what it does

private:
	bool Project(const Vector3 &pos, float radius);									// 0x7B71D0: the object's place on the screen, or on its edge; true when it is on the screen  // NAME 2: what it does
	float Get_Icon_Scale(const Vector3 &pos, float radius);							// 0x7B75E0: from the Gutter_Icon_* options  // NAME 2: its options

	SClient *Client;								// +0x00 (the FAIL_IF's name)
	RefCountPtr<SimpleSceneClass> Scene;			// +0x04  // NAME 2: what it holds
	TargetReticleClass *Reticle08;					// +0x08: size 0x44, hud01a at sort level 0x1C, no text  // NAME 1: neutral (D10)
	TargetReticleClass *Reticle0C;					// +0x0C: sort level 0x1A, with text  // NAME 1: neutral (D10)
	Vector3 Position;								// +0x10: X, Y 0..1 of the screen, the icon's centre; Z the view-space depth  // NAME 2: what it holds
	float Size;										// +0x1C: in the same units  // NAME 2: what it holds
	MouseTargetClass *MouseTarget;					// +0x20 (the G_ASSERT's name)
};

#endif //__GUTTER_RADAR_H
