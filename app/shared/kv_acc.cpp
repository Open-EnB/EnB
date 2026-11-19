// Earth & Beyond client (reconstruction): notes at the end of the file
#include "kv_acc_string.h"

// @0x4B0A90
KVFixedStringAccessorClass::KVFixedStringAccessorClass(const char *value)
{
	Value = nstrdup(value);
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown; no assert, no __FILE__: the name is PROVISIONAL (D137). The fixed string accessor's
**	object, client 0x4B0A90-0x4B0C7F: its constructor, the virtuals its vtable brings and <string>'s guarded
**	initialiser 0x4B0C60 (CRT entry 79). The client's CRT table has two guards here (79, 80) before kv_encoder.cpp's
**	entry 81, so the accessors are three objects: this, kv_acc_string.cpp (guard 0x4B1130) and kv_acc_time.cpp (no
**	initialiser); split by DE, 2026-11-18. delta_visitor.cpp's 0x4B0630-0x4B0A29 comes before (int3 filler to 0x4B0A90).
*/
