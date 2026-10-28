/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	PROVISIONAL file name (no assert or __FILE__ names it): the two interfaces the login's data
**	objects implement (docs/kernel-map-step9.md 5), moved here from tsk_login.h (D90 9b):
**	the wire interface 0xAEB9EC (now pk.h's Unknown_PacketDataClass itself, included) and
**	the key/value visitor interface 0xAEADC8 (delta.h's DeltaInterfaceClass has the same table),
**	the KV encoder's typed adds are kv_encoder_base.h's (D104).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UNKNOWN_PACKETDATA_H  // NAME 1 [unknown_packetdata.h]: neutral file name (D10)
#define UNKNOWN_PACKETDATA_H

#ifdef UNKNOWN_PACKETDATA_BODIES	// the key/value adds' bodies: unknown_login_data.cpp only (kerneltest's stand-ins
								// and tsk_login.cpp see the declaration; the KV headers pulled app.lib objects into kerneltest)
#include "kv_encoder_base.h"
#include "strconv_lib.h"

// strconv_lib.cpp's converters for the character slot's pairs (here, not in strconv_lib.h: a new
// overload there moves aux_roots.cpp's inlining, the header canary 0x6FB1E0)
char *Value_To_String(char *buf, int size, long value);					// 0x4A34D0 (the "long" pairs)
bool String_To_Value(const char *text, long *value);					// 0x4A3500
char *Value_To_String(char *buf, int size, __int64 value);				// 0x4A3A80 (the "quad" pairs)
bool String_To_Value(const char *text, __int64 *value);					// 0x4A3AB0
inline const char *Type_Name(long *) { return "long"; }					// 0x8C1190
inline const char *Type_Name(__int64 *) { return "quad"; }				// 0x8C0F70

#endif

// The wire interface 0xAEB9EC (three pure slots) is pk.h's Unknown_PacketDataClass (P-T5-2)
#include "..\packets\pk.h"

class Vector3;

// The KV encoder's typed adds and the visitor's slots are kv_encoder_base.h's KVEncoderBaseClass and
// delta.h's DeltaChildVisitorClass (D104; LoginKVVisitorClass until 2026-10-28)

// The one-slot interface vtable 0xAEADC8 is delta.h's DeltaInterfaceClass (P-T5-2)
#include "auxdata\delta.h"

#endif
