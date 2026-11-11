// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\eff_robj.cpp (the client's __FILE__)
**
**	The effects that play a render object (client 0x6EEEA0-0x6F0200): see eff_robj.h. Notes at the end.
*/
#include "eff_robj.h"
#include "client.h"
#include "co.h"
#include "rendobj.h"
#include "scene.h"
#include "camera.h"
#include "mapper.h"
#include "wwmath.h"
#include "..\shared\g_assert.h"



















// 0x6EEEA0
RObjEffectClass::RObjEffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *robj) :
	UnknownB00060EffectClass(client, id, start_time, duration, cycle_time)
{
	RObj = robj; FAIL_IF(!RObj) return;
	RObj->Add_Ref();
	RObj->Set_Transform(Matrix3D(1));
	Set_Flag(0x80, true);
	Transform.Get_Translation();	// dead read (D130; not the client's): it spends VC6's inline budget, so 0x6EF4C0 calls this constructor as the client does
}

// 0x6EF040
RObjEffectClass::~RObjEffectClass()
{
	REF_PTR_RELEASE(RObj);
}

// 0x6EF120
Vector3 RObjEffectClass::Unknown_08(const Vector3 &)
{
	return(Transform.Get_Translation());
}

// 0x6EF160
void RObjEffectClass::Set_Source_Object_ID(int game_id)
{
	if (Get_Flag(9) && SourceObject) {
		RObj->Scale(1.0f / SourceObject->Unknown_30());
	}
	UnknownB00060EffectClass::Set_Source_Object_ID(game_id);
	if (Get_Flag(9) && SourceObject) {
		RObj->Scale(SourceObject->Unknown_30());
	}
}

// 0x6EF200
void RObjEffectClass::Unknown_18()
{
	UnknownB00060EffectClass::Unknown_18();
	if (Get_Flag(12)) {
		Reset_All_Texture_Mappers(RObj, false);
	}
	RObj->Add(Scene);
}










// 0x6EF250
void RObjEffectClass::Unknown_1C()
{
	UnknownB00060EffectClass::Unknown_1C();
	FAIL_IF(!Camera) return;
	if (Get_Flag(10) && SourceObject) {
		Vector3 dir = Camera->Get_Position();
		Vector3 pos = Transform.Get_Translation();
		dir = dir - pos;
		dir.Normalize();
		pos += dir * SourceObject->Unknown_30();
		Transform.Set_Translation(pos);
	}
	if (Get_Flag(11)) {
		const Matrix3D &tm = Camera->Get_Transform();
		Transform[0][0] = tm[0][2];
		Transform[1][0] = tm[1][2];
		Transform[2][0] = tm[2][2];
		Transform[0][1] = tm[0][0];
		Transform[1][1] = tm[1][0];
		Transform[2][1] = tm[2][0];
		Transform[0][2] = tm[0][1];
		Transform[1][2] = tm[1][1];
		Transform[2][2] = tm[2][1];
	}
	RObj->Set_Transform(Transform);
}

// 0x6EF4A0
void RObjEffectClass::Unknown_20()
{
	RObj->Remove();
}

// 0x6EF4C0
Unknown6EF4C0EffectClass::Unknown6EF4C0EffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *robj, const Vector2 &offset, const Vector3 &radius_offset) :
	RObjEffectClass(client, id, start_time, duration, cycle_time, robj),
	UnknownDC(offset),
	UnknownE4(radius_offset)
{
	Set_Flag(0x200, false);
	Set_Flag(0x400, false);
	Set_Flag(0x800, false);
}

// 0x6EF660
void Unknown6EF4C0EffectClass::Unknown_18()
{
	RObjEffectClass::Unknown_18();
	if (SourceObject) {
		Transform.Adjust_Translation(UnknownE4 * SourceObject->Unknown_30());
	}
	RObj->Add(Scene);
}












// NOT EXACT: order-only (the frame and the 96 instructions are the client's; two of the identity matrix's zero stores and pos.Y's store sit a few instructions later); attempts.py show 0x6EF740
// 0x6EF740
void Unknown6EF4C0EffectClass::Unknown_1C()
{
	RObjEffectClass::Unknown_1C();
	FAIL_IF(!Camera) return;
	if (SourceObject && Get_Flag(7)) {
		Transform.Adjust_Translation(UnknownE4 * SourceObject->Unknown_30());
	}
	Vector3 screen;
	if (Camera->Project(screen, Transform.Get_Translation()) == CameraClass::INSIDE_FRUSTUM) {
		Vector3 pos(screen.X + UnknownDC.X, screen.Y + UnknownDC.Y, 0);
		RObj->Set_Transform(Matrix3D(1));
		RObj->Set_Position(pos);
		RObj->Set_Hidden(false);
	} else {
		RObj->Set_Hidden(true);
	}
}

// 0x6EF910
Unknown6EF910EffectClass::Unknown6EF910EffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *robj) :
	UnknownB0009CEffectClass(client, id, start_time, duration, cycle_time),
	RObj(robj)
{
	RObj->Add_Ref();
	RObj->Set_Transform(Matrix3D(1));
}





















// 0x6EFA70
Unknown6EF910EffectClass::~Unknown6EF910EffectClass()
{
	G_ASSERT(RObj->Get_Scene() == NULL);
	REF_PTR_RELEASE(RObj);
}

// 0x6EFBB0
void Unknown6EF910EffectClass::Unknown_18()
{
	UnknownB0009CEffectClass::Unknown_18();
	if (Get_Flag(15)) {
		Reset_All_Texture_Mappers(RObj, false);
	}
	RObj->Add(Scene);
}

// 0x6EFC00
void Unknown6EF910EffectClass::Unknown_20()
{
	RObj->Remove();
}

// 0x6EFC20
void Unknown6EF910EffectClass::Unknown_3C(UnknownB00060EffectClass *unary, int next)
{
	if (next == 1 && Get_Flag(14)) {
		unary->Unknown_2C(RObj->Get_Transform());
	} else {
		UnknownB0009CEffectClass::Unknown_3C(unary, next);
	}
}

// 0x6EFC70
RObjLookAtEffectClass::RObjLookAtEffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *robj) :
	Unknown6EF910EffectClass(client, id, start_time, duration, cycle_time, robj)
{
	Set_Flag(0x80, true);
	Set_Flag(0x100, true);
}

// 0x6EFF20
Vector3 RObjLookAtEffectClass::Unknown_08(const Vector3 &)
{
	if (Get_Flag(17)) {
		return(SourcePosition);
	}
	return(TargetPosition);
}

// 0x6EFF90
void RObjLookAtEffectClass::Set_Source_Object_ID(int game_id)
{
	if (Get_Flag(16) && Get_Flag(17) && SourceObject) {
		RObj->Scale(1.0f / SourceObject->Unknown_30());
	}
	UnknownB0009CEffectClass::Set_Source_Object_ID(game_id);
	if (Get_Flag(16) && Get_Flag(17) && SourceObject) {
		RObj->Scale(SourceObject->Unknown_30());
	}
}

// 0x6F0060
void RObjLookAtEffectClass::Set_Target_Object_ID(int game_id)
{
	if (Get_Flag(16) && !Get_Flag(17) && TargetObject) {
		RObj->Scale(1.0f / TargetObject->Unknown_30());
	}
	UnknownB0009CEffectClass::Set_Target_Object_ID(game_id);
	if (Get_Flag(16) && !Get_Flag(17) && TargetObject) {
		RObj->Scale(TargetObject->Unknown_30());
	}
}

// 0x6F0130
void RObjLookAtEffectClass::Unknown_1C()
{
	UnknownB0009CEffectClass::Unknown_1C();
	Matrix3D tm;
	if (Get_Flag(17)) {
		tm.Obj_Look_At(SourcePosition, TargetPosition, 0);
	} else if (Get_Flag(18) && TargetObject) {
		tm.Obj_Look_At(TargetObject->Unknown_24(), TargetPosition, 0);
	} else {
		tm.Obj_Look_At(TargetPosition, SourcePosition, 0);
	}
	RObj->Set_Transform(tm);
}

/*
**	Reconstruction notes (L2, 2026-11-10)
**
**	The window: the guarded <string> static 0x6EEE70 ends eff_pass.cpp's object (the three functions before
**	it, 0x6EEDF0 / 0x6EEE20 / 0x6EEE40, a reference pointer's out-of-line members, are called only from
**	eff_pass.cpp's constructor 0x6ED6C0 and its unwind code), so this file's first function is the
**	constructor 0x6EEEA0, whose FAIL_IF carries line 40. Its own guarded <string> static is 0x6F01D0, right
**	after RObjLookAtEffectClass's slot +0x1C (0x6F0130, 127 bytes: the matcher's 192 include the static).
**	The render-state effect 0x6F0200-0x6F0550 is the next object (its static 0x6F0550), a file between
**	eff_robj.cpp and eff_seek.cpp with no assert: not this file's.
**	RObj is a plain pointer: the constructors add the reference without a null test (the unary one after
**	its FAIL_IF, which tests the member it has just stored: the store is in the body, on the FAIL_IF's line)
**	and the destructors release it in their bodies (REF_PTR_RELEASE). The implicit destructors 0x6EF580
**	and 0x6EFDE0 repeat the bases' bodies (line 208 twice). The screen effect's constructor calls
**	RObjEffectClass's where RObjLookAtEffectClass's inlines its base's: the unary constructor is one
**	budget unit short of the client's source (the dead read), which is not found.
*/
