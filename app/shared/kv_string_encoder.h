// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Interim placement (D137): KVEncoderBaseClass's string adds, out of kv_acc_string.h (kv_acc.cpp and
**	kv_acc_string.cpp do not see kv_encoder_base.h); kv_encoder_base.cpp includes this header in kv_acc_string.h's place.
*/
#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __KV_STRING_ENCODER_H	// NAME 1 [kv_string_encoder.h]: neutral, what it declares (D10)
#define __KV_STRING_ENCODER_H

#include "kv_encoder_base.h"
#include "kv_pair_string.h"	// KVPairStringClass (the adds make them)
#include "kv_acc_string.h"

// KVEncoderBaseClass's string adds, kv_encoder_base.cpp's 0x4B4C50-0x4B51F0 (0x4B4A70 is the base's Add). Their declarations
// stay out of kv_encoder_base.h for aux_roots.cpp's symbol pad (no pad is exact for all six with them there, workflow.md 3.3):
// interim, D137; the same object layout, `this` the encoder.
class KVStringEncoderClass : public KVEncoderBaseClass	// NAME 1: interim, what it adds (D10)
{
public:
	void Add_New_String(int mask, const char *name, char **value);		// 0x4B4C50 (text from new)  // NAME 2: what it does
	void Add_Dup_String(int mask, const char *name, char **value);		// 0x4B4E30 (text from strdup)  // NAME 2: what it does
	void Add_Fixed_String(int mask, const char *name, const char *value);	// 0x4B5010 (a copy, Set does nothing)  // NAME 2: what it does
	void Add_Std_String(int mask, const char *name, std::string *value);	// 0x4B51F0  // NAME 2: what it does
};

#endif
