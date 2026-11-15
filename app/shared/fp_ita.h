// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\fp_ita.h (PROVISIONAL, D137: fp_ita.cpp's header, after its __FILE__
**	'e:\build\beta\G\app\shared\fp_ita.cpp' 0xB705B4; the client holds no string of this header)
**
**	fp_ita.cpp's flight path: a turn-rate limited steer of a transform pair's X axis toward a target point, the Y axis
**	kept level (0x0C bytes, table 0xAE78FC over the fp_*.cpp family's base table 0xAE7910).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef FP_ITA_H  // NAME 3 [fp_ita.h]: the header of fp_ita.cpp
#define FP_ITA_H

#include "fp_tt.h"	// FpBaseClass

class FpItaClass : public FpBaseClass  // NAME 3 [FpItaClass]: class-from-file (D24) for fp_ita.cpp
{
public:
	FpItaClass(float max_turn);																	// 0x448C30 (the angle clamped to 0..pi)  // NAME 1 [max_turn]: what it does with it
	virtual ~FpItaClass();																		// 0x448CE0 (deleting 0x448CB0)
	virtual int Unknown_04(const Vector3 &target, Matrix3D *transforms, float dist, float &yaw, float &pitch);	// 0x448D00 (G_ASSERT 'y_axis.Z < 0.0001f' l.151): 2
	virtual int Unknown_08(const Vector3 &target, Matrix3D *transforms, float dist, float &yaw, float &pitch, PxWrapper &proxim, float radius, DynObj *self, Vector3 *aims);	// 0x449330 (G_ASSERT '0' l.171): 1
	virtual void Unknown_0C(float &yaw, float &pitch);											// 0x449300: both 0

	float MaxTurnCos;						// +0x04  // NAME 1: the cosine of the angle the X axis turns by
	float MaxTurnSin;						// +0x08  // NAME 1: its sine
};

#endif
