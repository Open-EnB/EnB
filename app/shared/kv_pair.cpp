// Earth & Beyond client (reconstruction): app\shared\kv_pair.cpp. Notes at the end of the file.
#include "kv_pair.h"
#include "unknown_debug.h"











KVPairClass::KVPairClass(const char *key, bool even, bool odd) :
	Key(key),
	Unknown08(even),
	Unknown09(odd)
{
	G_ASSERT(key);
}

KVPairClass::~KVPairClass()
{
}

/*
**	Reconstruction notes
**
**	client.exe 0x4B7EB0-0x4B7F56: KVPairClass (D24, after the file), vtable 0xAEB7EC (the destructor
**	and six pure slots), size 0x0C. Rebuilt: the constructor 0x4B7EB0 (line 20; the key interned
**	through PermString, permstr.cpp's Find 0x4ADE20) and the destructor 0x4B7F50 (out of line:
**	the derived pairs' destructors call it, e.g. kv_pair_string.cpp's 0x4B80E0; the deleting one
**	0x4B7F20 is the compiler's). Not rebuilt yet: the functions before 0x4B7EB0 (0x4B7DE0,
**	0x4B7E00, 0x4B7E30: small template helpers kv_list.cpp's 0x4B62B0 calls) and this object's
**	initialiser (permstr.h's static).
*/
