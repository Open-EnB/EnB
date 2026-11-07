// Earth & Beyond client (reconstruction): notes at the end of the file
#include "auxdata\delta.h"
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

// accel.cpp's value (0x498C10-0x499180; not in the tree yet, interim here, D137)
class AccelClass	// NAME 3: the pair's type name "AccelClass"
{
public:
	AccelClass();											// 0x498C10
	AccelClass &operator=(const AccelClass &that);	// 0x498FD0
	bool operator!=(const AccelClass &that) const;	// 0x499000

	bool Unknown00;										// +0x00  // NAME 1: neutral (D10)
	float Target;											// +0x04 (the key's)
	float Accel;											// +0x08 (the key's)
	float Current;											// +0x0C (the key's)
};
char *Value_To_String(char *buf, int size, const AccelClass &value);	// 0x499070
bool String_To_Value(const char *text, AccelClass *value);				// 0x4990C0
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
DeltaChildVisitorClass::DeltaChildVisitorClass(KVListClass *list, int flags, const char *prefix) :
	KVEncoderBaseClass(list, flags, prefix)
{
}

// @0x4B1820
void DeltaChildVisitorClass::Add_Child(int mask, const char *name, DeltaInterfaceClass *child)
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

// @0x4B18C0
void DeltaChildVisitorClass::Add_Vector3(int mask, const char *name, Vector3 *value)
{
	Add(mask, name, value);
}

// @0x4B18F0
void DeltaChildVisitorClass::Add_Matrix3D(int mask, const char *name, Matrix3D *value)
{
	Add(mask, name, value);
}

// @0x4B1920
void DeltaChildVisitorClass::Add_Accel(int mask, const char *name, AccelClass *value)
{
	Add(mask, name, value);
}

// @0x4B1950
void DeltaChildVisitorClass::Add_Vector3_Frame(int mask, const char *name, Vector3 *value)
{
	bool even;
	bool odd;
	Unknown_Test(mask, even, odd);
	if (even || odd) {
		List->Add(new KVPairTemplateClass<Vector3>(Make_Key(name), new KVFrameAccessorClass<Vector3>(&value[Unknown_BE350C ^ 1], &value[Unknown_BE350C]), even, odd));
	}
}

// @0x4B1AE0
void DeltaChildVisitorClass::Add_Matrix3D_Frame(int mask, const char *name, Matrix3D *value)
{
	bool even;
	bool odd;
	Unknown_Test(mask, even, odd);
	if (even || odd) {
		List->Add(new KVPairTemplateClass<Matrix3D>(Make_Key(name), new KVFrameAccessorClass<Matrix3D>(&value[Unknown_BE350C ^ 1], &value[Unknown_BE350C]), even, odd));
	}
}

// @0x4B1CC0
void DeltaChildVisitorClass::Add_Vector3_Elements(int mask, const char *name, Vector3 *value)
{
	Push_Prefix(name);
	Add(mask, "X", &value->X);
	Add(mask, "Y", &value->Y);
	Add(mask, "Z", &value->Z);
	Pop_Prefix();
}

// @0x4B1D40
void DeltaChildVisitorClass::Add_Matrix3D_Elements(int mask, const char *name, Matrix3D *value)
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
void DeltaChildVisitorClass::Add_Accel_Elements(int mask, const char *name, AccelClass *value)
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
void DeltaChildVisitorClass::Add_Vector3_Elements_Frame(int mask, const char *name, Vector3 *value)
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
void DeltaChildVisitorClass::Add_Matrix3D_Elements_Frame(int mask, const char *name, Matrix3D *value)
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
**	The members of DeltaChildVisitorClass (auxdata\delta.h; vtable 0xAEB460): typed adds for Vector3, Matrix3D
**	and AccelClass, each over one value or over a value kept per frame (the frame toggle 0xBE350C), as one pair or one
**	float pair per element. The typed pairs and accessors this object instantiates follow them: 0x4B2420-0x4B3760.
*/
