// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\distance_calc_client.h (name assumed from distance_calc_client.cpp,
**	whose __FILE__ is 0xBA137C)
**
**	Two client objects' sphere distances (distance_calc_client.cpp 0x7E6060-0x7E62DF): each object's position and
**	radius through distance_calc.h's functions.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef DISTANCE_CALC_CLIENT_H  // NAME 3 [distance_calc_client.h]: header name assumed from the client's distance_calc_client.cpp
#define DISTANCE_CALC_CLIENT_H

class ClientObjectClass;

float Unknown_7E6060(ClientObjectClass *obj_a, ClientObjectClass *obj_b);	// 0x7E6060: the gap between the two objects' spheres (0x831E90)  // NAME 1: neutral (D10); NAME 3 [obj_a, obj_b]: its FAIL_IFs
float Unknown_7E6130(ClientObjectClass *obj_a, ClientObjectClass *obj_b);	// 0x7E6130: the gap squared (0x831F10)  // NAME 1: neutral (D10); NAME 3 [obj_a, obj_b]: its FAIL_IFs
char Unknown_7E6200(ClientObjectClass *obj_a, ClientObjectClass *obj_b, float range);	// 0x7E6200: the two spheres within range (0x831F90; no caller)  // NAME 1: neutral (D10); NAME 3 [obj_a, obj_b]: its FAIL_IFs; NAME 2 [range]: 0x831F90's

#endif
