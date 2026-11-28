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

#include "string_value.h"	// StringValueClass

// vtable 0xAEB810, size 0x14
class KVStringPairClass : public KVPairClass  // NAME 5: thunk order, binary.md 5.11
{
public:
	KVStringPairClass(const char *key, StringValueClass *accessor, bool even = true, bool odd = true);	// 0x4B7FC0
	KVStringPairClass(const KVStringPairClass &that);							// 0x4B8170 (line 44)
	virtual ~KVStringPairClass();													// +0x00 0x4B80E0 (deleting 0x4B80B0)
	virtual KVPairClass *Clone() const;											// +0x04 0x4B8250
	virtual const char *Get_Type_Name() const;									// +0x08 0x4B8090
	virtual char *Get_Value_String(char *buf, int size) const;				// +0x0C 0x4B8360
	virtual bool Set_Value_String(const char *text);							// +0x10 0x4B83A0
	virtual bool Is_Changed() const;												// +0x14 0x4B83D0
	virtual void Update_Saved();													// +0x18 0x4B8450

	StringValueClass *Accessor;		// +0x0C  // NAME 3: after the assert's parameter accessor (kv_pair_string.cpp)
	char *Unknown10;					// +0x10 the saved value (strdup)
};

#endif //__KV_PAIR_STRING_H
