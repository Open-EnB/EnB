// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\starmap_pip_support.h (name assumed from
**	starmap_pip_support.cpp, whose __FILE__ is 'e:\build\beta\G\app\client\starmap_pip_support.cpp';
**	the client holds no string of this header)
**
**	A line drawn on the star map PIP from a centre point, its length a share of a range. No RTTI: tables
**	0xB06D60 / 0xB06D48 / 0xB06D44, constructor 0x7C3AC0, destructor 0x7C3BC0 (deleting 0x7C3B90), 0x118 bytes.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __STARMAP_PIP_SUPPORT_H  // NAME 3 [starmap_pip_support.h]: header name assumed from the client's starmap_pip_support.cpp
#define __STARMAP_PIP_SUPPORT_H

#include "always.h"
#include "vector2.h"
#include "..\shared\unknown_466920.h"	// Unknown_466920Class (constructor 0x466920)

class StarmapPipSupportClass : public Unknown_466920Class  // NAME 3: class-from-file (D24) for app\client\starmap_pip_support.cpp
{
public:
	StarmapPipSupportClass();							// 0x7C3AC0
	virtual ~StarmapPipSupportClass();					// 0x7C3BC0 (deleting 0x7C3B90)

	void Unknown_7C3C60(const Vector2 &arg1, float arg2);	// 0x7C3C60 (+0x10C, +0x114)  // NAME 1 [arg1, arg2]: neutral
	void Unknown_7C3CA0(float total_range, float arg1, float arg2, bool arg3);	// 0x7C3CA0 (FAIL_IF '!total_range')  // NAME 2 [total_range]: the FAIL_IF's  // NAME 1 [arg1, arg2, arg3]: neutral

	Vector2 Unknown10C;									// +0x10C (0x692EF0 hands it the rectangle's corner)
	float Unknown114;									// +0x114 (0x692EF0 hands it the rectangle's size)
};

#endif //__STARMAP_PIP_SUPPORT_H
