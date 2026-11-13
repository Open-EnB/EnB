// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\fp_tt.h (PROVISIONAL, D137: fp_tt.cpp's header, after its __FILE__
**	'e:\build\beta\G\app\shared\fp_tt.cpp' 0xB70600; the client holds no string of this header)
**
**	fp_tt.cpp's flight path: a turn-rate and pitch-rate limited steer of a transform pair toward a target point
**	(0x48 bytes, table 0xAE7934 over the fp_*.cpp family's base table 0xAE7910: a deleting destructor and three pure
**	slots; fp_ita.cpp holds the base's own copies, 0x448CE0 / 0x4492D0).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef FP_TT_H  // NAME 3 [fp_tt.h]: the header of fp_tt.cpp
#define FP_TT_H

#include "vector2.h"
#include "vector3.h"
#include "matrix3d.h"

class PxWrapper;

// The fp_*.cpp family's base (table 0xAE7910): the slots fp_tt.cpp fills
class FpBaseClass  // NAME 1 [FpBaseClass]: neutral (D10), the base of the fp_*.cpp classes
{
public:
	virtual ~FpBaseClass() {}																	// 0x4492D0 (deleting, fp_ita.obj's copy)
	virtual int Unknown_04(const Vector3 &target, Matrix3D *transforms, float dist, float &yaw, float &pitch) = 0;	// +0x04  // NAME 1 [all]: what fp_tt.cpp's does with them
	virtual int Unknown_08(const Vector3 &target, Matrix3D *transforms, float dist, float &yaw, float &pitch, PxWrapper &proxim, float radius, int unknown8, int unknown9) = 0;	// +0x08  // NAME 1 [all but proxim]: as Unknown_04; [proxim]: the FAIL_IF 'proxim.Get_Avoidance()' 0xB705E4
	virtual void Unknown_0C(float &yaw, float &pitch) = 0;										// +0x0C  // NAME 1 [yaw, pitch]: the angles fp_tt.cpp's returns
};

class FpTTClass : public FpBaseClass  // NAME 3 [FpTTClass]: class-from-file (D24) for fp_tt.cpp
{
public:
	FpTTClass(float max_yaw, float max_pitch, float max_slope, bool unknown38);	// 0x449360 (the angles clamped to 0..pi)  // NAME 1 [all]: what it does with them
	virtual ~FpTTClass();																		// 0x449520 (deleting 0x4494F0)
	virtual int Unknown_04(const Vector3 &target, Matrix3D *transforms, float dist, float &yaw, float &pitch);	// 0x449540: 0 when Unknown39, else 2
	virtual int Unknown_08(const Vector3 &target, Matrix3D *transforms, float dist, float &yaw, float &pitch, PxWrapper &proxim, float radius, int unknown8, int unknown9);	// 0x449EA0 (FAIL_IF 'proxim.Get_Avoidance()' l.323; not rebuilt yet)
	virtual void Unknown_0C(float &yaw, float &pitch);											// 0x44AC40: atan2 of +0x3C / +0x40 and of +0x44

	float MaxYaw;							// +0x04 (0..pi)  // NAME 1: the angle the yaw turns by
	float MaxYawCos;						// +0x08  // NAME 1: its cosine
	float MaxYawSin;						// +0x0C  // NAME 1: its sine
	float MaxYawInv;						// +0x10 (1 / MaxYaw, -1 at 0)  // NAME 1: its inverse
	float MaxPitch;							// +0x14 (0..pi)  // NAME 1: the angle the pitch turns by
	float MaxPitchCos;						// +0x18  // NAME 1: its cosine
	float MaxPitchSin;						// +0x1C  // NAME 1: its sine
	float MaxSlope;							// +0x20 (tan of the third angle)  // NAME 1: the clamp of the target's slope
	float MaxYawHalfCos;					// +0x24 (cos(MaxYaw / 2))  // NAME 1: what it is
	int Unknown28;							// +0x28 (0, 1, 2: 0x449540's switch)
	int Unknown2C;							// +0x2C
	Vector2 Unknown30;						// +0x30 (the constructor: 1, 0)
	bool Unknown38;							// +0x38 (the constructor's last argument)
	bool Unknown39;							// +0x39
	Vector2 Unknown3C;						// +0x3C (the last heading)
	float Unknown44;						// +0x44 (the last slope)
};

#endif
