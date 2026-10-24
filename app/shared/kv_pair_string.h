/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\kv_pair_string.h (name assumed from
**	kv_pair_string.cpp; no assert names it)
**
**	A pair whose value is a string reached through an accessor (the constructor's assert:
**	"key && accessor"). Only what kv_encoder_base.cpp and the Delta fields use so far. The client
**	doesn't name the classes (D24).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __KV_PAIR_STRING_H
#define __KV_PAIR_STRING_H

#include "kv_pair.h"

// The string accessor (vtable 0xAEB150: the destructor and three pure slots); DeltaStringClass's
// is DeltaStringValueClass (delta_string.cpp).
class StringValueClass
{
public:
	virtual ~StringValueClass() {}									// +0x00 0x4A7F00
	virtual const char *Get() const = 0;							// +0x04
	virtual void Set(const char *value) = 0;						// +0x08
	virtual StringValueClass *Clone() const = 0;					// +0x0C
};

// vtable 0xAEB810, size 0x14
class KVPairStringClass : public KVPairClass
{
public:
	KVPairStringClass(const char *key, StringValueClass *accessor, bool even = true, bool odd = true);	// 0x4B7FC0
	virtual ~KVPairStringClass();													// +0x00 0x4B80E0 (deleting 0x4B80B0)
	virtual void Unknown_04();														// +0x04 0x4B8250
	virtual const char *Unknown_08() const;										// +0x08 0x4B8090
	virtual void Unknown_0C();														// +0x0C 0x4B8360
	virtual void Unknown_10();														// +0x10 0x4B83A0
	virtual void Unknown_14();														// +0x14 0x4B83D0
	virtual void Unknown_18();														// +0x18 0x4B8450

	StringValueClass *Accessor;		// +0x0C
	char *m_Unknown10;					// +0x10 a strdup copy
};

#endif //__KV_PAIR_STRING_H
