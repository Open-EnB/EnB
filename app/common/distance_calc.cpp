// Earth & Beyond client (reconstruction): the sphere distances (PROVISIONAL file, D137). Notes at the end of the file.
#include "distance_calc.h"
#include "vector3.h"
#include "wwmath.h"

float Unknown_831E90(const Vector3 &a, float a_radius, const Vector3 &b, float b_radius)
{
	Vector3 d = b - a;
	float distance = WWMath::Sqrt(d.Y * d.Y + d.X * d.X + d.Z * d.Z) - (a_radius + b_radius);	// NAME 1: neutral (D10)
	if (distance <= 0.0f) {
		distance = 0.0f;
	}
	return(distance);
}

float Unknown_831F10(const Vector3 &a, float a_radius, const Vector3 &b, float b_radius)
{
	Vector3 d = b - a;
	float distance = WWMath::Sqrt(d.Y * d.Y + d.Z * d.Z + d.X * d.X) - (a_radius + b_radius);	// NAME 1: neutral (D10)
	if (distance <= 0.0f) {
		distance = 0.0f;
	}
	return(distance * distance);
}

int Unknown_831F90(const Vector3 &a, float a_radius, const Vector3 &b, float b_radius, float range)
{
	Vector3 d = a - b;
	float r = a_radius + b_radius + range;	// NAME 1: neutral (D10)
	if (d.Y * d.Y + d.Z * d.Z + d.X * d.X < r * r) {
		return(1);
	}
	return(0);
}

int Unknown_832010(const Vector3 &pos1, float radius1, const Vector3 &pos2, float radius2, float range1, float range2, float *distance)
{
	int flags = 0;	// NAME 1: neutral (D10)
	Vector3 d = pos1 - pos2;
	float d2 = d.Y * d.Y + d.X * d.X + d.Z * d.Z;	// NAME 1: neutral (D10)
	float r1 = radius1 + range1 + range2;	// NAME 1: neutral (D10)
	if (d2 < r1 * r1) {
		flags = 1;
	}
	float r2 = radius2 + range1 + range2;	// NAME 1: neutral (D10)
	if (d2 < r2 * r2) {
		flags |= 2;
	}
	if (flags != 0 && distance != NULL) {
		*distance = WWMath::Sqrt(d2);
	}
	return(flags);
}

/*
**	Notes.
**	Object 0x831E90-0x83209F: it opens after color_common.cpp's guarded std::ctype<unsigned short>::id initialiser
**	0x831E60 and holds no __FILE__, assert or string. Its name is PROVISIONAL (D137): it links between color_common
**	and effect_description, and distance_calc_client.cpp (__FILE__ 0xBA137C) wraps 0x831F10 / 0x831F90 (0x7E6130,
**	0x7E6200). The sums of squares are written in the client's order (each function has its own); C linkage: see
**	distance_calc.h.
*/
