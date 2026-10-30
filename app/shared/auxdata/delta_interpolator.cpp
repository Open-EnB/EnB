// Earth & Beyond client (reconstruction): app\shared\auxdata\delta_interpolator.cpp. Notes at the end of the file.
#include <float.h>
#include "delta_interpolator.h"
#include "..\duration.h"
#include "..\unknown_debug.h"

// the time LastTime starts from (0xBE3A88, set to 0 by this file's initialiser 0x4A60F0)
static DurationClass UnknownTime(0);

DeltaInterpolatorClass::DeltaInterpolatorClass(const char *name) :
	DeltaCompoundClass(name),
	EndTime("EndTime", DurationClass(0)),
	ChangePerTick("ChangePerTick"),
	StartValue("StartValue"),
	LastTime(UnknownTime),
	LastValue(0),
	Unknown24C(false)
{
	Add(&EndTime);
	Add(&ChangePerTick);
	Add(&StartValue);
}

float DeltaInterpolatorClass::Get_Change() const
{
	return ChangePerTick.Value;
}

float DeltaInterpolatorClass::Update(const DurationClass &time)
{
	float change = ChangePerTick.Value;
	DurationClass end;
	end = EndTime.Value;
	if (change == 0.0f) {
		return StartValue.Value;
	}
	if (time >= end) {
		Unknown24C = true;
		if (change >= 0.0f) {
			LastValue = 1.0f;
		} else {
			LastValue = 0.0f;
		}
		return LastValue;
	}
	float step = (end - time).As_Int() * change;
	if (change >= 0.0f) {
		LastValue = 1.0f - step;
		if (LastValue < 0.0f) {
			LastValue = 0.0f;
		}
		if (LastValue < StartValue.Value) {
			LastValue = StartValue.Value;
		}
	} else {
		LastValue = -step;
		if (LastValue > 1.0f) {
			LastValue = 1.0f;
		}
		if (LastValue > StartValue.Value) {
			LastValue = StartValue.Value;
		}
	}
	LastTime = time;
	return LastValue;
}

// (the lines down to Set hold code of the client's file not rebuilt yet: its asserts are on lines 111-113)








































void DeltaInterpolatorClass::Set(float value, float change, const DurationClass &time)
{
	G_ASSERT(value <= 1.0f);
	G_ASSERT(value >= 0.0f);
	G_ASSERT(_finite(value));

	DurationClass end;
	if (change == 0.0f) {
		end.Time = 0;
	} else {
		end.Time = (__int64)(((change >= 0.0f) ? 1.0f - value : -value) / change + 0.5) + time.Time;
	}
	ChangePerTick.Set_Value(change);
	EndTime.Set_Value(end);
	StartValue.Set_Value(value);
	Unknown24C = (change == 0.0f || end.Time == time.Time);
	LastTime = time;
	LastValue = value;
}

void DeltaInterpolatorClass::Set(float value, const DurationClass &time)
{
	Set(value, ChangePerTick.Value, time);
}

// (Set_Change's assert is on the client's line 159)






















void DeltaInterpolatorClass::Set_Change(float change, const DurationClass &time)
{
	G_ASSERT(_finite(change));
	if (ChangePerTick.Value != change) {
		Set(Get_Value(time), change, time);
	}
}

void DeltaInterpolatorClass::Unknown_3C(DeltaClass *delta)
{
	Unknown24C = false;
	LastTime = UnknownTime;
	LastValue = 0;
	DeltaCompoundClass::Unknown_3C(delta);
}

// the time since the first call (0x4A3B40, timeGetTime through a pointer; not rebuilt yet)
unsigned int Unknown_Get_Ticks();

// The interpolator's value for the encoder (vtable 0xAEAF08): read-only
class DeltaInterpolatorValueClass : public KVAccessorClass<float>  // NAME 2: describes what it does
{
public:
	DeltaInterpolatorValueClass(DeltaInterpolatorClass *owner) : Owner(owner) {}
	virtual const float &Get();
	virtual void Set(const float &) {}
	virtual KVAccessorClass<float> *Clone() const;

	DeltaInterpolatorClass *Owner;		// +0x04  // NAME 2: describes what it does
	float Value;								// +0x08 the value Get read last  // NAME 2: describes what it does
};

void DeltaInterpolatorClass::Unknown_00(DeltaChildVisitorClass *visitor)
{
	visitor->Push_Prefix(Name.Get_String());
	visitor->Push_Flags();
	visitor->Set_Flags(visitor->Get_Flags() & Unknown80);
	((DeltaClass &)EndTime).Unknown_00(visitor);
	((DeltaClass &)ChangePerTick).Unknown_00(visitor);
	visitor->Add_Value(4, "CurrentValue", new DeltaInterpolatorValueClass(this));
	visitor->Pop_Flags();
	visitor->Pop_Prefix();
}

KVAccessorClass<float> *DeltaInterpolatorValueClass::Clone() const
{
	return new DeltaInterpolatorValueClass(Owner);
}

const float &DeltaInterpolatorValueClass::Get()
{
	Value = Owner->Get_Value(DurationClass(Unknown_Get_Ticks()));
	return Value;
}

/*
**	Reconstruction notes
**
**	client.exe 0x4A6110-0x4A6913: DeltaInterpolatorClass (D24), vtable 0xAEAE54 (slot 0 0x4A6930
**	and slot 0x3C 0x4A68E0 overridden), size 0x250. Rebuilt: the constructor 0x4A6110 (the field
**	names are its strings), the destructor 0x4A6330 (implicit), Get_Change 0x4A6360, Update 0x4A6380
**	(As_Int's FAIL_IF from duration.h line 190), Set 0x4A6590 (asserts on the client's lines 111-113),
**	Set 0x4A67A0, Set_Change 0x4A67D0 (line 159, and Get_Value's on delta_interpolator.h line 54),
**	Unknown_3C 0x4A68E0. The forms the bytes decide: DurationClass's user-defined operator= (Set
**	stores both halves of EndTime before the vtable load; Update loads EndTime before the test of
**	the change), `change >= 0.0f` tested first in Update.
**	Unknown_00 0x4A6930: the encoder's prefix and flags pushed, EndTime and ChangePerTick through
**	their vtables (the casts give the virtual calls; StartValue isn't encoded), then "CurrentValue"
**	as a DeltaInterpolatorValueClass (vtable 0xAEAF08; KVEncoderBaseClass::Add_Value inlined):
**	Clone 0x4A6AF0, Get 0x4A6B30 (Get_Value inlined, at the time 0x4A3B40 gives), Set 0x4A6BF0
**	empty; its destructors 0x4A6C10/0x4A6C40 are the compiler's. Not rebuilt yet: this object's
**	initialisers 0x4A6070 (a guarded static, atexit 0x42B320), 0x4A60A0 (a permstr.cpp object at
**	0xBE3A90, 0x4AE1E0/0x4AE2D0).
*/
