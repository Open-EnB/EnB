// Earth & Beyond client (reconstruction)
/*
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

#ifndef __KV_PAIR_STRING_H  // NAME 3 [kv_pair_string.h]: header name assumed from the client's kv_pair_string.cpp
#define __KV_PAIR_STRING_H

#include "kv_pair.h"

// The string accessor (vtable 0xAEB150: the destructor and three pure slots); DeltaStringClass's
// is DeltaStringValueClass (delta_string.cpp).
class StringValueClass  // NAME 2: describes what it does (D38)
{
public:
	virtual ~StringValueClass() {}									// +0x00 0x4A7F00
	virtual const char *Get() const = 0;							// +0x04  // NAME 2: describes what it does
	virtual void Set(const char *value) = 0;						// +0x08  // NAME 2: describes what it does
	virtual StringValueClass *Clone() const = 0;					// +0x0C  // NAME 2: describes what it does
};

// vtable 0xAEB810, size 0x14
class KVPairStringClass : public KVPairClass  // NAME 3: by convention (D24) from the client's file kv_pair_string.cpp
{
public:
	KVPairStringClass(const char *key, StringValueClass *accessor, bool even = true, bool odd = true);	// 0x4B7FC0
	KVPairStringClass(const KVPairStringClass &that);							// 0x4B8170 (line 44)
	virtual ~KVPairStringClass();													// +0x00 0x4B80E0 (deleting 0x4B80B0)
	virtual KVPairClass *Clone() const;											// +0x04 0x4B8250
	virtual const char *Get_Type_Name() const;									// +0x08 0x4B8090
	virtual char *Get_Value_String(char *buf, int size) const;				// +0x0C 0x4B8360
	virtual bool Set_Value_String(const char *text);							// +0x10 0x4B83A0
	virtual bool Is_Changed() const;												// +0x14 0x4B83D0
	virtual void Update_Saved();													// +0x18 0x4B8450

	StringValueClass *Accessor;		// +0x0C  // NAME 3: after the assert's parameter accessor (kv_pair_string.cpp)
	char *m_Unknown10;					// +0x10 the saved value (strdup)
};

#endif //__KV_PAIR_STRING_H
