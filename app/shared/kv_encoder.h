// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (name PROVISIONAL, D137, from kv_encoder_base.cpp): see kv_encoder.cpp's notes.
*/
#ifndef __KV_ENCODER_H	// NAME 3 [kv_encoder.h]: PROVISIONAL
#define __KV_ENCODER_H

#include "auxdata\delta.h"

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

// (its typed adds are virtual in the client: delta.h's note; declared on the base for now)
// The visitor that adds pairs (vtable 0xAEB6B8, 0x1C bytes), leaving out the children whose key starts with one of its
// prefixes. The constructor 0x4B5AD0 has no caller in the client.
class KVFilterEncoderClass : public DeltaChildVisitorClass	// NAME 1: neutral, what it does (D10)
{
public:
	KVFilterEncoderClass(KVListClass *list, int flags, const char *prefix);	// 0x4B5AD0

	virtual void Add_Child(int mask, const char *name, DeltaInterfaceClass *child);	// +0x04 0x4B5BE0
	void Add_Exclude(const char *prefix);														// 0x4B5C60  // NAME 2: what it does
	void Clear_Excludes();																			// 0x4B5D70  // NAME 2: what it does

	std::vector<std::string> Excludes;				// +0x0C (its allocator's byte, then +0x10-0x18)  // NAME 2: what it holds
};

#endif //__KV_ENCODER_H
