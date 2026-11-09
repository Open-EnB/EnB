// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (name PROVISIONAL, D137, from kv_encoder_base.cpp): see kv_encoder.cpp's notes.
*/
#ifndef __KV_ENCODER_H	// NAME 3 [kv_encoder.h]: PROVISIONAL
#define __KV_ENCODER_H

#include "auxdata\delta.h"
#include "accel.h"

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
