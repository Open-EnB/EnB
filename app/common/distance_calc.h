// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (PROVISIONAL, D137): app\common\distance_calc.h, named after its client-side wrapper
**	file distance_calc_client.cpp (__FILE__ 0xBA137C, 0x7E6060-0x7E62FF), whose functions call these.
**
**	The sphere distances: the gap between two spheres (0x831E90), its square (0x831F10), two spheres within a range
**	(0x831F90) and the two radii's ranges at once (0x832010). C linkage: 0x831F90 returns an int in eax while
**	radar_system_data_cache.cpp's caller 0x7BE3A0 reads al as a bool, which only declarations that do not mangle the
**	return type can link (radar_system_data_cache.cpp keeps its own bool view).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef DISTANCE_CALC_H  // NAME 2 [distance_calc.h]: after distance_calc_client.cpp (PROVISIONAL, D137)
#define DISTANCE_CALC_H

class Vector3;

extern "C" {
float Unknown_831E90(const Vector3 &a, float a_radius, const Vector3 &b, float b_radius);	// 0x831E90: the distance between two spheres (0 when they touch)  // NAME 1: neutral (D10)
float Unknown_831F10(const Vector3 &a, float a_radius, const Vector3 &b, float b_radius);	// 0x831F10: 0x831E90's distance squared  // NAME 1: neutral (D10)
int Unknown_831F90(const Vector3 &a, float a_radius, const Vector3 &b, float b_radius, float range);	// 0x831F90: the two spheres within range  // NAME 1: neutral (D10)
int Unknown_832010(const Vector3 &pos1, float radius1, const Vector3 &pos2, float radius2, float range1, float range2, float *distance);	// 0x832010: bit 0 within radius1 + both ranges, bit 1 within radius2 + both ranges; the distance when either  // NAME 1: neutral (D10)
}

#endif
