// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (name PROVISIONAL, D137, from kv_encoder_base.cpp): see kv_encoder.cpp's notes.
*/
#ifndef __KV_ENCODER_H	// NAME 3 [kv_encoder.h]: PROVISIONAL
#define __KV_ENCODER_H

#include "auxdata\delta.h"
#include "kv_encoder_base.h"
#include "accel.h"

// The argument of DeltaClass's slot 0, an encoder (the leaves call KVEncoderBaseClass's members on
// it). DeltaContainer::Unknown_00 (0x4A55D0) calls its slot 1 once per child: a slot the
// encoder base's vtable (0xAEB62C, the destructor only) doesn't have, so the encoders the fields
// are given derive from it.
// The login objects hand it their own children and colours too (D104: slots +0x04-+0x10).
class KVEncoderClass : public KVEncoderBaseClass  // NAME 2: kv_encoder_base.cpp's base's encoder, in kv_encoder.cpp (D206)
{
public:
	KVEncoderClass(KVListClass &list, int flags, const char *prefix);	// 0x4B1790 (kv_encoder.cpp; vtable 0xAEB460)
	virtual void Add_Child(int mask, const char *name, DeltaInterfaceClass *child);	// +0x04 0x4B1820 a key/value object under the name  // NAME 2: what it does
	// the typed adds, pure here (_purecall in 0xAEB460); KVFilterEncoderClass (kv_encoder.h; vtable 0xAEB6B8) has
	// their bodies 0x4B18C0-0x4B1950 (D206). shipcust_desc.cpp's colours go through +0x10.
	virtual void Add_Accel(int mask, const char *name, AccelClass *value) = 0;			// +0x08  // NAME 2: what it does
	virtual void Add_Matrix3D(int mask, const char *name, Matrix3D *value) = 0;		// +0x0C  // NAME 2: what it does
	virtual void Add_Vector3(int mask, const char *name, Vector3 *value) = 0;			// +0x10  // NAME 2: what it does
	virtual void Add_Matrix3D_Frame(int mask, const char *name, Matrix3D *value) = 0;	// +0x14  // NAME 2: what it does
	virtual void Add_Vector3_Frame(int mask, const char *name, Vector3 *value) = 0;	// +0x18  // NAME 2: what it does

	// the typed adds of kv_encoder.cpp that hold no slot (no caller found): one float pair per element
	void Add_Vector3_Elements(int mask, const char *name, Vector3 *value);			// 0x4B1CC0  // NAME 2: what it does
	void Add_Matrix3D_Elements(int mask, const char *name, Matrix3D *value);		// 0x4B1D40  // NAME 2: what it does
	void Add_Accel_Elements(int mask, const char *name, AccelClass *value);			// 0x4B1DC0  // NAME 2: what it does
	void Add_Vector3_Elements_Frame(int mask, const char *name, Vector3 *value);	// 0x4B1E60  // NAME 2: what it does
	void Add_Matrix3D_Elements_Frame(int mask, const char *name, Matrix3D *value);	// 0x4B2210  // NAME 2: what it does
};

// The visitor that adds pairs (vtable 0xAEB6B8, 0x1C bytes), leaving out the children whose key starts with one of its
// prefixes. The constructor 0x4B5AD0 has no caller in the client.
class KVFilterEncoderClass : public KVEncoderClass	// NAME 1: neutral, what it does (D10)
{
public:
	KVFilterEncoderClass(KVListClass *list, int flags, const char *prefix);	// 0x4B5AD0

	virtual void Add_Child(int mask, const char *name, DeltaInterfaceClass *child);	// +0x04 0x4B5BE0
	virtual void Add_Accel(int mask, const char *name, AccelClass *value);				// +0x08 0x4B1920
	virtual void Add_Matrix3D(int mask, const char *name, Matrix3D *value);			// +0x0C 0x4B18F0
	virtual void Add_Vector3(int mask, const char *name, Vector3 *value);				// +0x10 0x4B18C0
	virtual void Add_Matrix3D_Frame(int mask, const char *name, Matrix3D *value);		// +0x14 0x4B1AE0
	virtual void Add_Vector3_Frame(int mask, const char *name, Vector3 *value);		// +0x18 0x4B1950
	void Add_Exclude(const char *prefix);														// 0x4B5C60  // NAME 2: what it does
	void Clear_Excludes();																			// 0x4B5D70  // NAME 2: what it does

	std::vector<std::string> Excludes;				// +0x0C (its allocator's byte, then +0x10-0x18)  // NAME 2: what it holds
};

#endif //__KV_ENCODER_H
