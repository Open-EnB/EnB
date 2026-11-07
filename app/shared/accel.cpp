// Earth & Beyond client (reconstruction): notes at the end of the file
#include "accel.h"
#include "wwmath.h"
#include "g_assert.h"
#include <stdio.h>

// @0x498C10
AccelClass::AccelClass() :
	IsActive(false),
	Target(0.0f),
	Accel(0.0f),
	Current(0.0f)
{
}

// @0x498C30
void AccelClass::Set_Target(float target, float accel)
{
	Target = target;
	Accel = accel;
	IsActive = (WWMath::Fabs(Current - target) > 0.0001f);
	if (target == 0.0f && !IsActive) {
		Current = 0.0f;
	}
}

// @0x498CC0
void AccelClass::Set_Target(float target)
{
	Target = target;
	IsActive = (WWMath::Fabs(Current - target) > 0.0001f);
	if (target == 0.0f && !IsActive) {
		Current = 0.0f;
	}
}

// @0x498D40
void AccelClass::Set_Value(float value, bool stop)
{
	IsActive = false;
	Target = value;
	Current = value;
	if (stop) {
		Accel = 0.0f;
	}
}

// @0x498D80
void AccelClass::Adjust_Target(float delta, float accel, float max)
{
	Target += delta;
	if (Target > max) {
		Target = max;
	}
	Accel += accel;
	IsActive = (WWMath::Fabs(Current - Target) > 0.0001f);
}




























// @0x498E00
float AccelClass::Update(float time)
{
	G_ASSERT(IsActive);
	float step = time * Accel;
	if (Get_Current() < Target) {
		Current += step;
		if (Current >= Target) {
			Current = Target;
			IsActive = false;
		}
	} else {
		Current -= step;
		if (Current <= Target) {
			Current = Target;
			IsActive = false;
		}
	}
	return Get_Current();
}

// @0x498E90
bool AccelClass::Update_Changed(float time)
{
	float old = Current;
	return old != Get_Value(time);
}

// @0x498EE0
void AccelClass::Reset()
{
	Current = 0.0f;
	Target = 0.0f;
	Accel = 0.0f;
	IsActive = false;
}

// @0x498F00
void AccelClass::Set(float current, float target, float accel)
{
	Current = current;
	Target = target;
	Accel = accel;
	IsActive = (WWMath::Fabs(current - target) > 0.0001f);
}

// @0x498F80
bool AccelClass::operator==(const AccelClass &that) const
{
	return Target == that.Target && Accel == that.Accel && Current == that.Current;
}

// @0x498FD0
AccelClass &AccelClass::operator=(const AccelClass &that)
{
	Current = that.Current;
	Target = that.Target;
	Accel = that.Accel;
	IsActive = that.IsActive;
	return *this;
}

// @0x499000
bool AccelClass::operator!=(const AccelClass &that) const
{
	return !(*this == that);
}

// @0x499070
char *Value_To_String(char *buf, int size, const AccelClass &value)
{
	sprintf(buf, "%f,%f,%f", value.Target, value.Current, value.Accel);
	return buf;
}

// @0x4990C0
// NOT EXACT: the inlined Set keeps current and target on the FPU in the client (fld st(0), fst, fxch); see the notes
bool String_To_Value(const char *text, AccelClass *value)
{
	float target, current, accel;
	if (sscanf(text, "%f,%f,%f", &target, &current, &accel) != 3) {
		return false;
	}
	value->Set(current, target, accel);
	return true;
}

/*
**	Reconstruction notes
**
**	client.exe 0x498C10-0x499148: __FILE__ e:\build\beta\G\app\shared\accel.cpp, the one assert (IsActive) on line 89.
**	The object starts at the constructor 0x498C10 (0x498B90-0x498BF0 are the previous object's COMDATs) and ends with
**	String_To_Value; boolpars.cpp follows (its "TreeRoot != NULL" at 0xB726B4 sits after accel.cpp's strings).
**	The class name is the pair type name "AccelClass" (kv_encoder.cpp), IsActive the assert's; the other names are
**	what the code does (NAME 2). 0.0001f at 0xAE7840 and 0.0f at 0xAE7244 are literals shared with other objects.
**	* Update 0x498E00 is exact, and stays out of line in Update_Changed 0x498E90, only with the Get_Current() reads
**	  (without them VC6 inlines it there); a return in the first arm gives a forwarded fst instead of the client's mov.
**	* String_To_Value 0x4990C0 (0.71): the client keeps current and target on the FPU through the inlined Set
**	  (fld st(0), fst Target, fxch, fsub st(1)); ours reloads them. `const float &t = target` gave 0.83, both
**	  references 0.77; helpers, casts, direct stores and a second inline layer did not (attempts.csv).
*/
