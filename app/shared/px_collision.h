/*
**	Earth & Beyond client (reconstruction)
**	Original location: e:\build\beta\G\app\shared\. px_collision.cpp is the client's (its __FILE__, client
**	0x453A40-0x4557F0); this header's name is provisional: no string names it.
**
**	PxCollision, the client's name ('PxCollision::Process'): a pair of overlapping objects (_Collision_Callback's,
**	the loose boxes' DynObjs) tested by distance, then by their collision representations, and pushed apart.
*/
#if defined(_MSC_VER)
#pragma once
#endif
#ifndef PX_COLLISION_H  // NAME 1 [px_collision.h]: provisional header name (px_collision.cpp's)
#define PX_COLLISION_H

class DynObj;

class PxCollision
{
public:
	PxCollision(DynObj *a, DynObj *b) :						// NAME 2 [a, b]: _Collision_Callback's arguments
		A(a), B(b), Unknown08(1.0f), Unknown0C(0.0f), Unknown10(0.0f), Unknown14(0.0f), Unknown18(0.0f), Unknown1C(0.0f),
		Unknown20(false), Unknown24(0.0f) {}
	bool Process();											// 0x453A40 'PxCollision::Process'
	bool Unknown_453B10();									// 0x453B10 (the spheres' test, then 0x454730 on the collision representations)
	void Unknown_453C20();									// 0x453C20 (the response: slots +0x88 / +0x8C / +0x94 / +0xA0 of both; not rebuilt)
	bool Unknown_454730(int aobj_coll_rep, int bobj_coll_rep);	// 0x454730 (FAIL_IF '!(aobj_coll_rep || bobj_coll_rep)' l.234; OBBox tests; not rebuilt)  // NAME 3 [aobj_coll_rep, bobj_coll_rep]: the FAIL_IF's

	DynObj *A;												// +0x00  // NAME 2: the constructor's argument
	DynObj *B;												// +0x04  // NAME 2: as above
	float Unknown08;										// +0x08 1.0 (0x453C20 reads +0x08..+0x10 as a direction)
	float Unknown0C;										// +0x0C 0
	float Unknown10;										// +0x10 0
	float Unknown14;										// +0x14 0 (0x453C20 passes &+0x14 to slot +0xA0)
	float Unknown18;										// +0x18 0
	float Unknown1C;										// +0x1C 0
	bool Unknown20;											// +0x20 false
	float Unknown24;										// +0x24 0
};

#endif
