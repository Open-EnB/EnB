// Earth & Beyond client (reconstruction): app\shared\px_collision.cpp. Notes at the end of the file.
#define ENABLE_WWPROFILE
#include "px_collision.h"
#include "dynamic_obj.h"
#include "..\shared\unknown_debug.h"
#include "wwprofile.h"

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
		return(Unknown_454730(aobj_coll_rep, bobj_coll_rep));
	} } }
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
** '!(aobj_coll_rep || bobj_coll_rep)' on line 234 (asserts.txt files it under px_callback_type.cpp). Only Process
** and its first test are rebuilt; the response 0x453C20, the OBBox tests 0x453F10 / 0x454730 (MeshGeometryClass's
** Cast_OBBox / Intersect_OBBox), the std::string members 0x454590 / 0x4552B0 and the inlined helpers
** 0x455590-0x455760 are not. PxObject's three functions 0x4557F0-0x455860 follow them in the client; no string says
** whose file they are (px_collision.cpp's tail or an object of their own), so they sit here provisionally.
**
** Live or dormant: Process is called only by px_wrapper.cpp's _Collision_Callback, which only the loose box's
** PxPollCallback::Process calls, which only PxProcessClass::Process (no reference in the client) would call: the
** collision code never runs in the retail client.
*/
