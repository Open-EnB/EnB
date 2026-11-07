// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\galaxy_map_focal_point.h (name assumed from
**	galaxy_map_focal_point.cpp, whose __FILE__ is 'e:\build\beta\G\app\Client\galaxy_map_focal_point.cpp';
**	the client holds no string of this header)
**
**	The point the galaxy map's camera looks at: a transform, and how far back from it the camera sits.
**	0x3C bytes, no table. galaxy_map_pip.cpp holds three and is its only user.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __GALAXY_MAP_FOCAL_POINT_H  // NAME 3 [galaxy_map_focal_point.h]: header name assumed from the client's galaxy_map_focal_point.cpp
#define __GALAXY_MAP_FOCAL_POINT_H

#include "always.h"
#include "ref_ptr.h"
#include "vector2.h"
#include "matrix3d.h"

class CameraClass;

class GalaxyMapFocalPointClass  // NAME 3: class-from-file (D24) for app\Client\galaxy_map_focal_point.cpp
{
public:
	GalaxyMapFocalPointClass();							// 0x7A5730 (identity, +0x34 2, +0x38 500)
	~GalaxyMapFocalPointClass();						// 0x7A5780 (empty)

	void Unknown_7A57A0(const Matrix3D &transform, float distance);	// 0x7A57A0 (the transform and +0x30; 0x5406A0)  // NAME 1 [transform]: what it stores  // NAME 1 [distance]: 0x7A5890 backs the camera off by it
	void Unknown_7A5820(const Matrix3D &transform);		// 0x7A5820 (no caller in the client)  // NAME 1 [transform]: what it stores
	void Unknown_7A5890(const RefCountPtr<CameraClass> &camera);	// 0x7A5890 (FAIL_IF '!camera')  // NAME 2 [camera]: the FAIL_IF's
	void Unknown_7A5980(const Vector2 &arg, const Vector2 &min, const Vector2 &max);	// 0x7A5980 (moves in the view's plane, scaled by the distance, then clamps; 0x53FD10 passes the renderer's +0x14 / +0x1C)  // NAME 1 [arg, min, max]: neutral
	void Unknown_7A5AA0(const Vector2 &arg);			// 0x7A5AA0 (rotates by it, then keeps the angles in range)  // NAME 1 [arg]: neutral
	void Unknown_7A5E10(float arg);						// 0x7A5E10 (scales the distance, between +0x34 and +0x38; 0x53FCA0)  // NAME 1 [arg]: neutral
	void Unknown_7A5ED0(const GalaxyMapFocalPointClass &a, const GalaxyMapFocalPointClass &b, float t);	// 0x7A5ED0 (Lerp of the transforms and distances)  // NAME 2 [a, b, t]: what Lerp takes

	bool operator == (const GalaxyMapFocalPointClass &that) const;	// 0x7A5F90 (no caller in the client: +0x30, then the matrix)  // NAME 2 [operator ==, that]: what it compares

	Matrix3D Transform;									// +0x00  // NAME 2: 0x7A5ED0's Lerp
	float Distance;										// +0x30  // NAME 2: 0x7A5890 backs the camera off by it
	float MinDistance;									// +0x34 (2)  // NAME 2: 0x7A5E10's lower bound
	float MaxDistance;									// +0x38 (500; 0x53FCA0: the renderer's +0xB4 * 3)  // NAME 2: 0x7A5E10's upper bound
};

#endif // __GALAXY_MAP_FOCAL_POINT_H
