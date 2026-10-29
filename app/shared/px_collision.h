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

#include "vector3.h"

class DynObj;
class MeshGeometryClass;

/*
** What a dynamic object's boxes belong to (0x14 bytes; 0x4557F0-0x455860, after px_collision.cpp's functions and
** before px_process.cpp; no string places them, so px_collision.cpp keeps them provisionally): the owner and the
** position and radius PxWrapper::Update last set.
*/
class PxObject  // NAME 2: the boxes' owner (PxBox::Owner)
{
public:
	PxObject(DynObj *owner);			// 0x4557F0  // NAME 2 [owner]: stored at +0x00
	PxObject(DynObj *owner, const PxObject &src);	// 0x455820 (no caller)  // NAME 2 [owner, src]: as PxBox's
	~PxObject();						// 0x455860 (empty, not inline)

	DynObj *Owner;						// +0x00  // NAME 2: the constructor's argument
	Vector3 Pos;						// +0x04  // NAME 2: Update's pos
	float Radius;						// +0x10  // NAME 2: Update's radius
};

class PxCollision
{
public:
	PxCollision(DynObj *a, DynObj *b) :						// NAME 2 [a, b]: _Collision_Callback's arguments
		A(a), B(b), Unknown08(1.0f, 0.0f, 0.0f), Unknown14(0.0f, 0.0f, 0.0f), Unknown20(false), Unknown24(0.0f) {}
	bool Process();											// 0x453A40 'PxCollision::Process'
	bool Unknown_453B10();									// 0x453B10 (the spheres' test, then 0x454730 on the collision representations)
	void Unknown_453C20();									// 0x453C20 (the response: an impulse along Unknown08 to each object with slot +0x88 set, through slot +0xA0)
	bool Unknown_454730(MeshGeometryClass *aobj_coll_rep, MeshGeometryClass *bobj_coll_rep);	// 0x454730 (FAIL_IF '!(aobj_coll_rep || bobj_coll_rep)' l.234; the meshes' OBBox tests, NOT EXACT; the client calls MeshGeometryClass::Cast_OBBox / Intersect_OBBox on them)  // NAME 3 [aobj_coll_rep, bobj_coll_rep]: the FAIL_IF's

	DynObj *A;												// +0x00  // NAME 2: the constructor's argument
	DynObj *B;												// +0x04  // NAME 2: as above
	Vector3 Unknown08;										// +0x08 (1, 0, 0): 0x453C20's direction (the velocities' components along it, the impulses along it)
	Vector3 Unknown14;										// +0x14 (0, 0, 0): 0x453C20 passes it to slot +0xA0
	bool Unknown20;											// +0x20 false
	float Unknown24;										// +0x24 0
};

#endif
