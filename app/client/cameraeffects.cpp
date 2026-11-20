// Earth & Beyond client (reconstruction): app\Client\cameraeffects.cpp. Notes at the end of the file.
#define __CAMERAEFFECTS_MEMBERS	// sclient.h's CameraEffectsClass as an SList (the other files see slots by number)
#include "co.h"
#include "sclient.h"
#include "ww3d.h"
#include "random.h"
#include "client.h"
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\auxdata\delta_interpolator.h"
#include "..\shared\auxdata\unknown_root.h"	// Unknown_Get_Ticks (0x6E32E0, 0x6E3F00)
#include "..\shared\g_assert.h"
#include "camera.h"
#include "bridge_int.h"	// the bridge's Get_Hit (0x6E2CC0)

// The file's own generator (0x6E2BA0 constructs it at 0xBE67E8, seeded with rand(); only this file's effects call it)
static Random2Class CameraEffectsRandom(rand());	// NAME 1: neutral (D10)
#include "aux_shake.h"	// the shake's fields: T<float> numbered after CameraEffectsRandom (CRT entry 533)
// The effects' base (table 0xAFFE6C, one slot: the shared return-false copy 0x9CCD10); each effect
// registers itself with the camera effects object it gets (that object's slot +0x10)
class UnknownAFFE6CClass	// NAME 1: neutral (D10)
{
public:
	UnknownAFFE6CClass(CameraEffectsClass *owner);	// 0x6E3530 (SList's slot +0x10, 0x6E4250)  // NAME 1 [owner]: neutral
	~UnknownAFFE6CClass() { Owner->Remove(this); }	// (inline in 0x6E2CC0: SList's slot +0x1C, 0x6E4440; not virtual)
	virtual bool Unknown_00() { return(false); }	// +0x00 (true while the effect still moves the camera)
	CameraEffectsClass *Owner;						// +0x04
};

// The shake 0x6E2CC0 makes (0x24 bytes, table 0xAFFE64): the shake's force, ramped up and down
class UnknownAFFE64Class : public UnknownAFFE6CClass	// NAME 1: neutral (D10)
{
public:
	UnknownAFFE64Class(CameraEffectsClass *owner, const Vector3 &force);	// 0x6E3590 (inline in 0x6E2CC0)  // NAME 1 [owner, force]: neutral
	virtual bool Unknown_00();						// +0x00 0x6E3710
	Vector3 Unknown08;								// +0x08 (the force, 'Shake.ForceX'-'Z')
	AccelClass Unknown14;							// +0x14 (accel.h's)
};

// The shield hit's jitter (0x1C bytes, table 0xAFFE84): its strength from the ship's shield
class UnknownAFFE84Class : public UnknownAFFE6CClass	// NAME 1: neutral (D10)
{
public:
	UnknownAFFE84Class(CameraEffectsClass *owner, float damage);	// 0x6E37F0  // NAME 1 [owner, damage]: neutral
	virtual bool Unknown_00();						// +0x00 0x6E3B60
	float Unknown08;								// +0x08
	AccelClass Unknown0C;							// +0x0C (accel.h's)
};

// The kick 0x6E32E0 makes for type 2 (0x1C bytes, table 0xAFFE70): the amount, ramped up over a second
class UnknownAFFE70Class : public UnknownAFFE6CClass	// NAME 1: neutral (D10)
{
public:
	UnknownAFFE70Class(CameraEffectsClass *owner, float amount);	// 0x6E3C70 (inline in 0x6E32E0)  // NAME 1 [owner, amount]: neutral
	virtual bool Unknown_00();						// +0x00 0x6E3D30
	float Unknown08;								// +0x08 (0x6E32E0's amount)
	AccelClass Unknown0C;							// +0x0C (accel.h's)
};

// The timed shake 0x6E32E0 makes for type 3 (0x10 bytes, table 0xAFFE74): the amount until the end time
class UnknownAFFE74Class : public UnknownAFFE6CClass	// NAME 1: neutral (D10)
{
public:
	UnknownAFFE74Class(CameraEffectsClass *owner, float amount, float time);	// 0x6E3E40 (inline in 0x6E32E0; +0x15 cleared)  // NAME 1 [owner, amount, time]: neutral
	virtual bool Unknown_00();						// +0x00 0x6E3F00
	float Unknown08;								// +0x08 (0x6E32E0's amount)
	unsigned Unknown0C;								// +0x0C (the tick count it ends at: time seconds from now)
};

template<class C> inline C *Unknown_Find_Valid(DeltaCompoundClass *root, const NameClass &name) { C *t = Unknown_Find_Field<C>(root, name); if (t && t->Unknown70) return(t); return(NULL); }	static inline float Unknown_Accel_Value(AccelClass &accel, int time) { if (!accel.IsActive) return(accel.Current); return(accel.Update((float)time)); }	// NAME 1: neutral (D10): as co.cpp's; Unknown_Accel_Value NAME 1: neutral (D10), the form 0x6E3710 / 0x6E3B60 inline (an int time, converted after the test; AccelClass::Get_Value takes a float)

// 0x6E2BD0
CameraEffectsClass::CameraEffectsClass(MainViewClass *view, SClient *client) :
	View(view),
	Client(client),
	Unknown14(false),
	Unknown15(true),
	Unknown24(false)
{
}

// a shake field of the ship's aux data (0x6E2CC0's four finds, one NameClass slot)
static inline DeltaTemplateClass<float> *Unknown_Shake_Field(DeltaCompoundClass *aux, const char *field) { NameClass name(field); return(Unknown_Find_Field<DeltaTemplateClass<float> >(aux, name)); }	// NAME 1: neutral (D10)
void Unknown_491550(float *x, float *y);	// 0x491550 (unknown_490900.cpp: the point snapped to whole device pixels)  // NAME 1: neutral (D10)

// NOT EXACT: functional (D64), register and scheduling (the camera move: the client keeps &Unknown18 in ebx for X and builds the translation from the camera's own matrix for X / Z, the copy for Y)
// 0x6E2CC0: the ship's 'Shake.Force*' and 'Shake.Damage' fields start a shake and a jitter; every effect adds to
// +0x18 (the finished ones deleted), the camera moved by it (each axis wrapped into +-12, X / Y snapped to pixels)
// and the bridge told of the hit
void CameraEffectsClass::Unknown_34()
{
	if (Unknown14) {
		View->Unknown_40();
		Unknown14 = false;
	}
	SClient *client = (SClient *)Client->As_Space_Client();
	if (client && client->Get_Ship()) {
		DeltaCompoundClass *aux = (DeltaCompoundClass *)client->Get_Ship()->AuxData;
		if (aux) {
			DeltaTemplateClass<float> *field = Unknown_Shake_Field(aux, "Shake.ForceX");
			if (field && field->Unknown70 > 1) {
				Vector3 force;
				force.X = field->Value;
				force.Y = 0.0f;
				force.Z = 0.0f;
				field = Unknown_Shake_Field(aux, "Shake.ForceY");
				if (field && field->Unknown70) force.Y = field->Value;
				field = Unknown_Shake_Field(aux, "Shake.ForceZ");
				if (field && field->Unknown70) force.Z = field->Value;
				if (force.Y + force.X + force.Z != 0.0) {
					new UnknownAFFE64Class(this, force);
				}
			}
			field = Unknown_Shake_Field(aux, "Shake.Damage");
			if (field && field->Unknown70 > 1) {
				float damage = field->Value;
				new UnknownAFFE84Class(this, damage);
			}
		}
	}
	SLNode<UnknownAFFE6CClass> *node = Head();
	if (node) {
		Unknown18.Set(0.0f, 0.0f, 0.0f);
		Unknown24 = false;
		do {
			if (!node->Data()->Unknown_00()) {
				SLNode<UnknownAFFE6CClass> *done = node;
				node = node->Next();
				delete done->Data();
			} else {
				node = node->Next();
			}
		} while (node);
		CameraClass *camera = (CameraClass *)View->Unknown_54();
		if (Unknown24) {
			while (Unknown18.X > 12.0f) Unknown18.X -= 12.0f;
			while (Unknown18.X < -12.0f) Unknown18.X += 12.0f;
			while (Unknown18.Y > 12.0f) Unknown18.Y -= 12.0f;
			while (Unknown18.Y < -12.0f) Unknown18.Y += 12.0f;
			while (Unknown18.Z > 12.0f) Unknown18.Z -= 12.0f;
			while (Unknown18.Z < -12.0f) Unknown18.Z += 12.0f;
			Unknown_491550(&Unknown18.X, &Unknown18.Y);
			Matrix3D tm = camera->Get_Transform();
			tm.Set_Translation(Vector3(tm.Get_X_Translation() + Unknown18.X, tm.Get_Y_Translation() + Unknown18.Y, tm.Get_Z_Translation()));
			camera->Set_Transform(tm);
			Unknown14 = true;
		}
		BridgeInterface *bridge;
		if (client && (bridge = client->Bridge) != NULL) {
			Vector3 hit = Unknown18;
			float z = camera->Get_Transform().Get_Z_Translation();
			if (z > 0.0001f) {
				hit /= z;
			}
			bridge->Get_Hit(hit);
		}
	}
	// (budget-device audit 2026-11-19: two dead finds here changed nothing, the residue the same without them)
}












































































// 0x6E32E0: a camera effect of type (1 the shield hit's jitter, 2 a kick, 3 a timed shake; any other asserts)
// at amount (the shake's time in seconds); each registers itself with this object
void CameraEffectsClass::Unknown_6E32E0(int type, float amount, float time)
{
	switch (type) {
		case 2:
			new UnknownAFFE70Class(this, amount);
			break;
		case 1:
			new UnknownAFFE84Class(this, amount);
			break;
		case 3:
			new UnknownAFFE74Class(this, amount, time);
			break;
		default:
			G_ASSERT(false);
			break;
	}
}

// 0x6E34F0: the camera effects of the client's current view (no caller in the client)
CameraEffectsClass *Unknown_6E34F0(SClient *client)	// NAME 1: neutral (D10)
{
	SClient *self = (SClient *)client->As_Space_Client();
	if (self && self->CurrentView) {
		UIViewClass *view = self->CurrentView->Unknown_1C();
		if (view) {
			return(view->CameraEffects);
		}
	}
	return(NULL);
}

// 0x6E3530
UnknownAFFE6CClass::UnknownAFFE6CClass(CameraEffectsClass *owner) : Owner(owner)
{
	Owner->Add_Tail(this);
}

// 0x6E3590
UnknownAFFE64Class::UnknownAFFE64Class(CameraEffectsClass *owner, const Vector3 &force) : UnknownAFFE6CClass(owner), Unknown08(force)
{
	if (Unknown08.X > 12.0f || Unknown08.Y > 12.0f) {
		Unknown08.Normalize();
		Unknown08 *= 12.0f;
	}
	Unknown14.Set_Value(0.0f, true);
	Unknown14.Set_Target(1.0f, 0.0015f);
}

// 0x6E37F0
UnknownAFFE84Class::UnknownAFFE84Class(CameraEffectsClass *owner, float damage) : UnknownAFFE6CClass(owner)
{
	Unknown_Find_Field<DeltaTemplateClass<float> >(NULL, *(NameClass *)this); Unknown_Find_Field<DeltaTemplateClass<float> >(NULL, *(NameClass *)this);	// dead finds (D130; not the client's): they spend VC6's inline budget, so the two finds' casts stay out of line as the client's (with one, 0x6E37F0 is not exact)
	if (damage <= 0.0f) {
		Unknown0C.Set_Value(0.0f, true);
		return;
	}
	SClient *client = Owner->Client;
	SClient *self = (SClient *)client->As_Space_Client();
	if (self == NULL) {
		return;
	}
	DeltaCompoundClass *aux = (DeltaCompoundClass *)self->Get_Ship()->AuxData;
	if (aux == NULL) {
		return;
	}
	NameClass max_name("MaxShieldPower");
	DeltaTemplateClass<float> *max_power = Unknown_Find_Valid<DeltaTemplateClass<float> >(aux, max_name);
	NameClass percent_name("ShieldPercent");
	DeltaInterpolatorClass *percent = Unknown_Find_Valid<DeltaInterpolatorClass>(aux, percent_name);
	float power;
	if (max_power) power = max_power->Value;
	else power = damage * 4.0f;
	float shield;
	if (percent) shield = percent->Get_Value(DurationClass((unsigned)client->Unknown10BC));
	else shield = 1.0f;
	float low = shield * power * 0.15f;
	float high = shield * power * 0.8f;
	if (damage < low) {
		damage = low;
	} else if (damage > high) {
		damage = high;
	}
	Unknown08 = (damage - low) / (high - low);
	if (Unknown08 > 0.0f) {
		float time = (Unknown08 + 1.0f) * 0.5f;
		float rate = 1.0f / (time * 1000.0f);
		Unknown0C.Set_Value(1.0f, true);
		Unknown0C.Set_Target(0.0f, rate);
		Unknown08 = Unknown08 * 0.12f;
	}
}

// 0x6E3710
bool UnknownAFFE64Class::Unknown_00()
{
	float value = Unknown_Accel_Value(Unknown14, WW3D::Get_Frame_Time());
	if (value >= 1.0f) {
		return(false);
	}
	if (value <= 0.33f) {
		value = value * 3.030303f;
	} else {
		value = 1.0 - (value - 0.33f) * 1.4925374f;
	}
	value = value * 10.0f;
	Owner->Unknown18.X += value * Unknown08.X;
	Owner->Unknown18.Y += value * Unknown08.Y;
	Owner->Unknown18.Z += value * Unknown08.Z;
	Owner->Unknown24 = true;
	return(true);
}

// 0x6E3B60
bool UnknownAFFE84Class::Unknown_00()
{
	float value = Unknown_Accel_Value(Unknown0C, WW3D::Get_Frame_Time());
	if (value <= 0.0f) {
		return(false);
	}
	float amount = value * Unknown08;
	Owner->Unknown18.X += CameraEffectsRandom(0, 100) * amount;
	Owner->Unknown18.Y += CameraEffectsRandom(0, 100) * amount;
	Owner->Unknown18.Z += CameraEffectsRandom(0, 100) * amount;
	Owner->Unknown24 = true;
	return(true);
}

// 0x6E3C70
UnknownAFFE70Class::UnknownAFFE70Class(CameraEffectsClass *owner, float amount) : UnknownAFFE6CClass(owner), Unknown08(amount)
{
	Unknown0C.Set_Value(0.0f, true);
	Unknown0C.Set_Target(1.0f, 0.001f);
}

// 0x6E3D30
bool UnknownAFFE70Class::Unknown_00()
{
	float value = Unknown_Accel_Value(Unknown0C, WW3D::Get_Frame_Time());
	if (value >= 1.0f) {
		return(false);
	}
	if (value <= 0.33f) {
		value = value * 3.030303f;
	} else {
		value = 1.0 - (value - 0.33f) * 1.4925374f;
	}
	float amount = value * Unknown08;
	Owner->Unknown18.X += CameraEffectsRandom(0, 25) * amount;
	Owner->Unknown18.Y += CameraEffectsRandom(0, 25) * amount;
	Owner->Unknown24 = true;
	return(true);
}

// 0x6E3E40
UnknownAFFE74Class::UnknownAFFE74Class(CameraEffectsClass *owner, float amount, float time) : UnknownAFFE6CClass(owner), Unknown08(amount)
{
	owner->Unknown15 = false;
	Unknown0C = Unknown_Get_Ticks() + time * 1000.0f;
}

// 0x6E3F00
bool UnknownAFFE74Class::Unknown_00()
{
	if (Unknown_Get_Ticks() >= Unknown0C) {
		return(false);
	}
	Owner->Unknown18.X += CameraEffectsRandom(-100, 100) * Unknown08;
	Owner->Unknown18.Y += CameraEffectsRandom(-100, 100) * Unknown08;
	Owner->Unknown24 = true;
	return(true);
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\cameraeffects.cpp (its __FILE__, 0x6E32E0's assert)
**
**	The camera effects, client 0x6E2B40-0x6E46A0 (eff_attach.cpp's static initialiser 0x6E46C0 after it):
**	so far the two effects' slot 0x6E3710 and 0x6E3B60 and the jitter's constructor 0x6E37F0 (TG4, claim
**	#62; its shield lookups are co.cpp's form, two dead finds spend the inline budget). The file's evidence: the
**	effects' tables 0xAFFE64 / 0xAFFE6C / 0xAFFE84 sit between the floats 0xAFFE60-0xAFFE80 that
**	0x6E2CC0 and 0x6E37F0 read; 0x6E2CC0 and 0x6E37F0 (called by 0x6E32E0, the assert's) store them;
**	the generator 0xBE67E8 is constructed by the initialiser at 0x6E2BA0 (seeded with rand()) and called only from
**	0x6E3B60, 0x6E3D30 and 0x6E3F00. S1 (2026-11-14): 0x6E32E0 (its G_ASSERT(false) on line 250; its cases in
**	the order of its EH states: 2, 1, 3) with the kick (table 0xAFFE70, 0x6E3D30) and the timed shake (0xAFFE74,
**	0x6E3F00), their constructors inline in it. U3 (2026-11-17): the effects' constructors are .cpp definitions
**	after 0x6E32E0 (VC6 inlines them into 0x6E2CC0 / 0x6E32E0 and keeps the uncalled out-of-line copies the client
**	has, with thunks: the base's 0x6E3530, the shake's 0x6E3590, the kick's 0x6E3C70, the timed shake's 0x6E3E40),
**	and 0x6E34F0, a free function no client code calls: the client's current view's CameraEffects. Then: CameraEffectsClass is an SList<UnknownAFFE6CClass> (its table
**	0xAFFDDC: SList's slots 0x6E4000-0x6E46C0 in this object, slot +0x34 0x6E2CC0); the effects' base adds itself
**	with Add_Tail and its inline destructor (out of line 0x6E3560) takes itself off with Remove. The constructor
**	0x6E2BD0, SList's destructor 0x6E2C40 and the implicit deleting copy 0x6E2C10 exact; 0x6E2CC0 (the shake fields,
**	the effects run, the camera moved, the bridge's Get_Hit) not exact: two dead finds keep its four finds out of
**	line (D130, as 0x6E37F0).
*/
