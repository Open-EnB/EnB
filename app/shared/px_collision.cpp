// Earth & Beyond client (reconstruction): app\shared\px_collision.cpp. Notes at the end of the file.
#define ENABLE_WWPROFILE
#include "px_collision.h"
#include "dynamic_obj.h"
#include "..\shared\unknown_debug.h"
#include "wwprofile.h"
#include "wwmath.h"
#include "meshgeometry.h"
#include "coltest.h"
#include "inttest.h"
#include "castres.h"
#include "obbox.h"
#include "matrix3d.h"
#include <math.h>

// 0x453A40
bool PxCollision::Process()
{
	WWPROFILE("PxCollision::Process");
	if (!Unknown_453B10()) {
		return(false);
	}
	Unknown_453C20();
	return(true);
}

// 0x453B10
// NOT EXACT: shape (the client spills the summed radius and dz to the stack and sums dy*dy + dz*dz + dx*dx; attempts.py show 0x453B10)
bool PxCollision::Unknown_453B10()
{
	if (!A->Unknown_Dyn2C() && !B->Unknown_Dyn2C()) {
	Vector3 apos = A->Unknown_24();
	Vector3 bpos = B->Unknown_24();
	float dist = A->Unknown04 + B->Unknown04;
	Vector3 delta = bpos - apos;
	if (delta.Length2() <= dist * dist) {
	int aobj_coll_rep = A->Unknown_Dyn84();
	int bobj_coll_rep = B->Unknown_Dyn84();
	if (aobj_coll_rep || bobj_coll_rep) {
		return(Unknown_454730((MeshGeometryClass *)aobj_coll_rep, (MeshGeometryClass *)bobj_coll_rep));
	} } }
	return(false);
}

// 0x453C20
void PxCollision::Unknown_453C20()
{
	float aimpulse = 0.0f;
	float bimpulse = 0.0f;
	float amass = A->Unknown_Dyn88();
	float bmass = B->Unknown_Dyn88();
	switch ((amass != 0.0f) + (bmass != 0.0f) * 2) {
		case 1:
			aimpulse = B->Unknown_Dyn8C();
			break;
		case 2:
			bimpulse = A->Unknown_Dyn8C();
			break;
		case 3:
		{
			float adot = A->Unknown_2C() * Unknown08;
			float bdot = B->Unknown_2C() * Unknown08;
			float closing = adot - bdot;
			float speed = WWMath::Max(-0.25f, closing);
			if (speed < 0.0f) {
				if (Unknown24 == 0.0f) {
					return;
				}
				speed = (float)fabs(speed * 0.5f);
			}
			if (speed <= 0.0f) {
				return;
			}
			aimpulse = bimpulse = ((B->Unknown_Dyn94() + A->Unknown_Dyn94()) * 0.5f + 1.0f) * (bmass * amass / (bmass + amass)) * speed;
			break;
		}
		default:
			return;
	}
	if (aimpulse > 0.0f) {
		Vector3 impulse = Unknown08 * -aimpulse;
		if (A->Unknown_DynA0(B, impulse, Unknown14, Unknown20)) {
			A->Unknown_Dyn64();
		}
	}
	if (bimpulse > 0.0f) {
		Vector3 impulse = Unknown08 * bimpulse;
		if (B->Unknown_DynA0(A, impulse, Unknown14, Unknown20)) {
			B->Unknown_Dyn64();
		}
	}
}

// 0x453F10 (an OBBox separating-axis test, no caller): not rebuilt







































































































































// 0x454730
// NOT EXACT: order (the first inlined OBBoxClass::Transform, A's box with both reps, multiplies centre by matrix where ours loads the matrix first: 18 rows; frame and every other instruction match; attempts.py show 0x454730)
bool PxCollision::Unknown_454730(MeshGeometryClass *aobj_coll_rep, MeshGeometryClass *bobj_coll_rep)
{
	FAIL_IF(!(aobj_coll_rep || bobj_coll_rep)) return(false);
	if (aobj_coll_rep && bobj_coll_rep) {
		Matrix3D atm = A->Unknown_Dyn3C();
		AABoxClass abox = A->Unknown_Dyn7C();
		OBBoxClass aobox(abox.Center, abox.Extent);
		OBBoxClass awbox;
		OBBoxClass::Transform(atm, aobox, &awbox);
		OBBoxIntersectionTestClass atest(awbox, COLLISION_TYPE_ALL);
		Matrix3D btm = B->Unknown_Dyn3C();
		AABoxClass bbox = B->Unknown_Dyn7C();
		OBBoxClass bobox(bbox.Center, bbox.Extent);
		OBBoxClass bwbox;
		OBBoxClass::Transform(btm, bobox, &bwbox);
		OBBoxIntersectionTestClass btest(bwbox, COLLISION_TYPE_ALL);
		Unknown08.Set(1.0f, 0.0f, 0.0f);
		Matrix3D ainv;
		atm.Get_Orthogonal_Inverse(ainv);
		OBBoxIntersectionTestClass blocal(btest, ainv);
		if (!aobj_coll_rep->Intersect_OBBox(blocal)) {
			return(false);
		}
		Matrix3D binv;
		btm.Get_Orthogonal_Inverse(binv);
		OBBoxIntersectionTestClass alocal(atest, binv);
		if (bobj_coll_rep->Intersect_OBBox(alocal)) {
			return(true);
		}
		return(false);
	}
	if (aobj_coll_rep) {
		CastResultStruct result;
		Matrix3D btm = B->Unknown_Dyn3C();
		AABoxClass bbox = B->Unknown_Dyn7C();
		OBBoxClass bobox(bbox.Center, bbox.Extent);
		OBBoxClass bwbox;
		OBBoxClass::Transform(btm, bobox, &bwbox);
		OBBoxCollisionTestClass btest(bwbox, B->Unknown_2C(), &result, COLLISION_TYPE_ALL);
		Matrix3D atm = A->Unknown_Dyn3C();
		Matrix3D ainv;
		atm.Get_Orthogonal_Inverse(ainv);
		OBBoxCollisionTestClass blocal(btest, ainv);
		if (aobj_coll_rep->Cast_OBBox(blocal)) {
			Matrix3D::Rotate_Vector(atm, result.Normal, &Unknown08);
			Unknown20 = false;
			return(true);
		}
		return(false);
	}
	CastResultStruct result;
	Matrix3D atm = A->Unknown_Dyn3C();
	AABoxClass abox = A->Unknown_Dyn7C();
	OBBoxClass aobox(abox.Center, abox.Extent);
	OBBoxClass awbox;
	OBBoxClass::Transform(atm, aobox, &awbox);
	OBBoxCollisionTestClass atest(awbox, A->Unknown_2C(), &result, COLLISION_TYPE_ALL);
	Matrix3D btm = B->Unknown_Dyn3C();
	Matrix3D binv;
	btm.Get_Orthogonal_Inverse(binv);
	OBBoxCollisionTestClass alocal(atest, binv);
	if (bobj_coll_rep->Cast_OBBox(alocal)) {
		Matrix3D::Rotate_Vector(btm, result.Normal, &Unknown08);
		Unknown08 = -Unknown08;
		Unknown20 = false;
		return(true);
	}
	return(false);
}

// 0x4557F0
PxObject::PxObject(DynObj *owner) :
	Owner(owner),
	Pos(0.0f, 0.0f, 0.0f),
	Radius(0.0f)
{
}

// 0x455820
PxObject::PxObject(DynObj *owner, const PxObject &src) :
	Owner(owner),
	Pos(src.Pos),
	Radius(src.Radius)
{
}

// 0x455860
PxObject::~PxObject()
{
}

/*
** Notes
**
** px_collision.cpp is client 0x453A40-0x4557F0, after px_callback_type.cpp and before PxObject's functions
** 0x4557F0-0x455860 and px_process.cpp; its __FILE__ is pushed by one FAIL_IF, 0x454730's
** '!(aobj_coll_rep || bobj_coll_rep)' on line 234 (asserts.txt files it under px_callback_type.cpp). Process, its
** first test and the response 0x453C20 are rebuilt (PX3: each object with slot +0x88 set gets an impulse along
** Unknown08 through slot +0xA0; with both, the closing speed, clamped at -0.25 and halved when negative, times the
** reduced mass and the mean of slot +0x94 plus one). 0x454730 (PX4, NOT EXACT) tests the objects' world OBBoxes
** (each DynObj's slot +0x7C box turned by its slot +0x3C transform) against the meshes: with both meshes an
** OBBoxIntersectionTestClass each way (Unknown08 set to (1, 0, 0)); with one, the other object's box swept by its
** slot +0x40 velocity (OBBoxCollisionTestClass) cast in the mesh's space, the hit normal turned back into
** Unknown08 (negated when the mesh is B's). Its frame (0x588) needs the B-only case's locals at function scope.
** The ww3d2 inlines the client calls out of line follow it (CastResultStruct::Reset 0x4555B0, OBBoxClass's copy
** 0x4555F0, OBBoxClass::Transform 0x455680, update_bounding_box 0x455760, Vector3() 0x455590 for ??_H). Not
** rebuilt: the separating-axis test 0x453F10 and the std::string members 0x454590 / 0x4552B0.
** PxObject's three functions 0x4557F0-0x455860 follow them in the client; no string says
** whose file they are (px_collision.cpp's tail or an object of their own), so they sit here provisionally.
**
** Live or dormant: Process is called only by px_wrapper.cpp's _Collision_Callback, which only the loose box's
** PxPollCallback::Process calls, which only PxProcessClass::Process (no reference in the client) would call: the
** collision code never runs in the retail client.
*/
