// Earth & Beyond client (reconstruction): notes at the end of the file
#include "kv_encoder.h"
#include "vector3.h"
#include "matrix3d.h"

extern int Unknown_BE350C;	// NAME 1: neutral (D10): SClient::Do_Frame's frame toggle (sclient.cpp)

// strconv_lib.cpp's conversions of the wwmath types
char *Value_To_String(char *buf, int size, const Vector3 &value);		// 0x4A3750
bool String_To_Value(const char *text, Vector3 *value);					// 0x4A3810
char *Value_To_String(char *buf, int size, const Matrix3D &value);		// 0x4A3850
bool String_To_Value(const char *text, Matrix3D *value);					// 0x4A3990
inline const char *Type_Name(Vector3 *) { return "Vector3"; }
inline const char *Type_Name(Matrix3D *) { return "Matrix3D"; }

// accel.cpp's conversions (accel.h)
inline const char *Type_Name(AccelClass *) { return "AccelClass"; }

// A pair's accessor over a member kept twice, one per frame (the frame toggle 0xBE350C)
template<class T> class KVFrameAccessorClass : public KVAccessorClass<T>	// NAME 2: what it does
{
public:
	KVFrameAccessorClass(T *other, T *current) :
		Value0(Unknown_BE350C ? other : current),
		Value1(Unknown_BE350C ? current : other)
	{
	}
	virtual const T &Get() { return *(Unknown_BE350C ? Value1 : Value0); }
	virtual void Set(const T &value) { *Value0 = *Value1 = value; }
	virtual KVAccessorClass<T> *Clone() const { return new KVFrameAccessorClass<T>(*this); }

	T *Value0;	// +0x04  // NAME 2: what it holds
	T *Value1;	// +0x08  // NAME 2: what it holds
};

// the matrix elements' keys, 0xB737A4: by column (0x4B1D40, 0x4B2210)
static const char *Matrix_Keys[12] = { "Xx", "Xy", "Xz", "Yx", "Yy", "Yz", "Zx", "Zy", "Zz", "Tx", "Ty", "Tz" };	// NAME 2: what it holds

// @0x4B1790
KVEncoderClass::KVEncoderClass(KVListClass *list, int flags, const char *prefix) :
	KVEncoderBaseClass(list, flags, prefix)
{
}

#pragma auto_inline(off)
// @0x4B1820
void KVEncoderClass::Add_Child(int mask, const char *name, DeltaInterfaceClass *child)
{
	bool even;
	bool odd;
	Unknown_Test(mask, even, odd);
	if ((even || odd) && (child != NULL)) {
		Push_Prefix(name);
		Push_Flags();
		Set_Flags(Get_Flags() & mask);
		child->Unknown_00(this);
		Pop_Flags();
		Pop_Prefix();
	}
}
#pragma auto_inline(on)

// @0x4B18C0
void KVFilterEncoderClass::Add_Vector3(int mask, const char *name, Vector3 *value)
{
	Add(mask, name, value);
}

// @0x4B18F0
void KVFilterEncoderClass::Add_Matrix3D(int mask, const char *name, Matrix3D *value)
{
	Add(mask, name, value);
}

// @0x4B1920
void KVFilterEncoderClass::Add_Accel(int mask, const char *name, AccelClass *value)
{
	Add(mask, name, value);
}

// @0x4B1950
void KVFilterEncoderClass::Add_Vector3_Frame(int mask, const char *name, Vector3 *value)
{
	bool even;
	bool odd;
	Unknown_Test(mask, even, odd);
	if (even || odd) {
		List->Add(new KVPairTemplateClass<Vector3>(Make_Key(name), new KVFrameAccessorClass<Vector3>(&value[Unknown_BE350C ^ 1], &value[Unknown_BE350C]), even, odd));
	}
}

// @0x4B1AE0
void KVFilterEncoderClass::Add_Matrix3D_Frame(int mask, const char *name, Matrix3D *value)
{
	bool even;
	bool odd;
	Unknown_Test(mask, even, odd);
	if (even || odd) {
		List->Add(new KVPairTemplateClass<Matrix3D>(Make_Key(name), new KVFrameAccessorClass<Matrix3D>(&value[Unknown_BE350C ^ 1], &value[Unknown_BE350C]), even, odd));
	}
}

// @0x4B1CC0
void KVEncoderClass::Add_Vector3_Elements(int mask, const char *name, Vector3 *value)
{
	Push_Prefix(name);
	Add(mask, "X", &value->X);
	Add(mask, "Y", &value->Y);
	Add(mask, "Z", &value->Z);
	Pop_Prefix();
}

// @0x4B1D40
void KVEncoderClass::Add_Matrix3D_Elements(int mask, const char *name, Matrix3D *value)
{
	Push_Prefix(name);
	for (int column = 0; column < 4; column++) {
		for (int row = 0; row < 3; row++) {
			Add(mask, Matrix_Keys[column * 3 + row], &(*value)[row][column]);
		}
	}
	Pop_Prefix();
}

// @0x4B1DC0
void KVEncoderClass::Add_Accel_Elements(int mask, const char *name, AccelClass *value)
{
	bool even;
	bool odd;
	Unknown_Test(mask, even, odd);
	if (even || odd) {
		Push_Prefix(name);
		Add(mask, "Target", &value->Target);
		Add(mask, "Accel", &value->Accel);
		Add(mask, "Current", &value->Current);
		Pop_Prefix();
	}
}

// @0x4B1E60
void KVEncoderClass::Add_Vector3_Elements_Frame(int mask, const char *name, Vector3 *value)
{
	bool even;
	bool odd;
	Unknown_Test(mask, even, odd);
	if (even || odd) {
		Push_Prefix(name);
		List->Add(new KVPairTemplateClass<float>(Make_Key("X"), new KVFrameAccessorClass<float>(&value[Unknown_BE350C ^ 1].X, &value[Unknown_BE350C].X), even, odd));
		List->Add(new KVPairTemplateClass<float>(Make_Key("Y"), new KVFrameAccessorClass<float>(&value[Unknown_BE350C ^ 1].Y, &value[Unknown_BE350C].Y), even, odd));
		List->Add(new KVPairTemplateClass<float>(Make_Key("Z"), new KVFrameAccessorClass<float>(&value[Unknown_BE350C ^ 1].Z, &value[Unknown_BE350C].Z), even, odd));
		Pop_Prefix();
	}
}

// @0x4B2210
void KVEncoderClass::Add_Matrix3D_Elements_Frame(int mask, const char *name, Matrix3D *value)
{
	bool even;
	bool odd;
	Unknown_Test(mask, even, odd);
	if (even || odd) {
		Push_Prefix(name);
		for (int column = 0; column < 4; column++) {
			for (int row = 0; row < 3; row++) {
				const char *key = Matrix_Keys[column * 3 + row];
				List->Add(new KVPairTemplateClass<float>(Make_Key(key), new KVFrameAccessorClass<float>(&value[Unknown_BE350C ^ 1][row][column], &value[Unknown_BE350C][row][column]), even, odd));
			}
		}
		Pop_Prefix();
	}
}

// @0x4B5AD0
KVFilterEncoderClass::KVFilterEncoderClass(KVListClass *list, int flags, const char *prefix) :
	KVEncoderClass(list, flags, prefix)
{
}

// @0x4B5BE0
void KVFilterEncoderClass::Add_Child(int mask, const char *name, DeltaInterfaceClass *child)
{
	const char *key = Make_Key(name);
	for (std::vector<std::string>::iterator i = Excludes.begin(); i != Excludes.end(); ++i) {
		if (strncmp(i->c_str(), key, i->length()) == 0) {
			return;
		}
	}
	KVEncoderClass::Add_Child(mask, name, child);
}

// @0x4B5C60
void KVFilterEncoderClass::Add_Exclude(const char *prefix)
{
	Excludes.push_back(std::string(prefix));
}

// @0x4B5D70
// NOT EXACT: inline budget: the client's string destructor calls _Refcnt and deallocate out of line (0x4B5E40, 0x4B5E60) here; attempts.csv
void KVFilterEncoderClass::Clear_Excludes()
{
	Excludes.clear();
}

#pragma auto_inline(off)
// @0x4B2BB0
void KVEncoderBaseClass::Add(int mask, const char *name, Vector3 *value)
{
	bool even;
	bool odd;
	Unknown_Test(mask, even, odd);
	if (even || odd) {
		List->Add(new KVPairTemplateClass<Vector3>(Make_Key(name), new KVPointerAccessorClass<Vector3>(value), even, odd));
	}
}

// @0x4B2CF0
void KVEncoderBaseClass::Add(int mask, const char *name, Matrix3D *value)
{
	bool even;
	bool odd;
	Unknown_Test(mask, even, odd);
	if (even || odd) {
		List->Add(new KVPairTemplateClass<Matrix3D>(Make_Key(name), new KVPointerAccessorClass<Matrix3D>(value), even, odd));
	}
}

// @0x4B2E80
void KVEncoderBaseClass::Add(int mask, const char *name, AccelClass *value)
{
	bool even;
	bool odd;
	Unknown_Test(mask, even, odd);
	if (even || odd) {
		List->Add(new KVPairTemplateClass<AccelClass>(Make_Key(name), new KVPointerAccessorClass<AccelClass>(value), even, odd));
	}
}
#pragma auto_inline(on)

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown; no assert, no __FILE__: the name is PROVISIONAL (D137), from the base's
**	kv_encoder_base.cpp. Client 0x4B1740 (permstr.h's static opens the object) to 0x4B37EF (<string>'s initialiser
**	ends it); kv_encoder_base.cpp's own static follows at 0x4B3800.
**
**	KVEncoderClass's members (auxdata\delta.h; vtable 0xAEB460, whose slots +0x08-+0x18 are _purecall) and five typed adds
**	of KVFilterEncoderClass, the bodies of those slots in its vtable 0xAEB6B8 (0x4B18C0-0x4B1950, D206), for Vector3, Matrix3D
**	and AccelClass, each over one value or over a value kept per frame (the frame toggle 0xBE350C); KVEncoderClass's own
**	five (0x4B1CC0-0x4B2210) add one float pair per element. The typed pairs and
**	accessors this object instantiates follow them: 0x4B2420-0x4B3760. KVFilterEncoderClass's constructor, Add_Child, Add_Exclude
**	and Clear_Excludes (0x4B5AD0-0x4B5D70) sit in the client after kv_encoder_base.cpp's templates and before a <string> initialiser
**	0x4B5E80, with no static of their own: probably kv_encoder_base.cpp's object. Placed here with their class (interim, D137).
*/
