// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\mouse_targeting.h (name assumed from mouse_targeting.cpp, whose __FILE__
**	is 'e:\build\beta\G\app\client\mouse_targeting.cpp'; the client holds no string of this header)
**
**	The object under the mouse, picked over one radar pass: radar_system.cpp's 0x7BCA70 makes one on its stack, hands it
**	to the gutter radar and the starmap, and they test every object against it. 0x48 bytes, no virtuals.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __MOUSE_TARGETING_H  // NAME 3 [mouse_targeting.h]: header name assumed from the client's mouse_targeting.cpp
#define __MOUSE_TARGETING_H

#include "ref_ptr.h"
#include "vector2.h"
#include "vector3.h"
#include "layer.h"

class ClientObjectClass;
class RadarIconDataClass;

class MouseTargetClass	// NAME 2: gutter_radar.cpp's and starmap_pip.cpp's G_ASSERT name their member MouseTarget
{
public:
	MouseTargetClass(ClientObjectClass *self);					// 0x7B77C0 (radar_system.cpp's 0x7BCA70)  // NAME 2 [self]: 0x7B7880 keeps its result apart

	void Set(float x, float y, float width, float height, const LayerClass &layer);	// 0x7B7810  // NAME 2 [Set, x, y, width, height, layer]: what it stores
	void Test(RadarIconDataClass *rdata, const Vector3 &pos, float size, float scale);	// 0x7B7880: rdata is the FAIL_IF's name  // NAME 2 [Test, pos, size, scale]: what it does, what 0x7B7040 passes

private:
	ClientObjectClass *Self;									// +0x00  // NAME 2: 0x7B7880 records its priority apart
	ClientObjectClass *Target;									// +0x04  // NAME 2: what it holds
	LayerClass Layer;											// +0x08: 0x24 bytes  // NAME 2: Intersect_Sub_Object's argument
	Vector2 Mouse;												// +0x2C  // NAME 2: what it holds
	Vector2 HalfSize;											// +0x34: of the box round the mouse  // NAME 2: what it holds
	int TargetPriority;											// +0x3C: 3 icon under the box, 2 model under the mouse, 1 within scale  // NAME 2: what it holds
	float TargetDistance;										// +0x40  // NAME 2: what it holds
	int SelfPriority;											// +0x44  // NAME 2: what it holds
};

#endif //__MOUSE_TARGETING_H
