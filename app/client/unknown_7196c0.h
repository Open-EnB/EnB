// Earth & Beyond client (reconstruction): unknown_7196c0.h, PROVISIONAL header name (D137) beside unknown_7196c0.cpp
#ifndef UNKNOWN_7196C0_H
#define UNKNOWN_7196C0_H

#include "..\..\wwmath\vector3.h"

class ClientObjectClass;

// a picking candidate of MainViewClass::Unknown_60 (0x512BB0): 0x20 bytes
struct PickInfoStruct	// NAME 1: neutral (D10)
{
	Vector3 Dir;									// +0x00 towards the object, normalised
	float Dist2;									// +0x0C the squared distance
	float Cos;										// +0x10 the cosine between Dir and the ray
	float CosSize;									// +0x14 the cosine of the occluder's half angle, -1 for the others  // NAME 3: the G_ASSERT's 'cos_occluder_size'
	ClientObjectClass *Object;						// +0x18
	bool Occluder;									// +0x1C (types 2, 3 and 10)
};

extern PickInfoStruct Info[64];		// 0xBE71E0  // NAME 2: InfoCount's
extern int InfoCount;				// 0xBE79E0  // NAME 3: mainview.cpp's G_ASSERT

int __cdecl Unknown_7196C0(const void *a, const void *b);	// 0x7196C0 (qsort: the indices by distance)  // NAME 1 [a, b]: qsort's
int __cdecl Unknown_719730(const void *a, const void *b);	// 0x719730 (qsort: the indices by angle)  // NAME 1 [a, b]: qsort's

#endif
