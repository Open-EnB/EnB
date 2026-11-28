// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Interim placement (D137): the string accessor, out of kv_pair_string.h: the client's kv_acc, kv_acc_string and kv_acc_time objects have no PermStringInit, so they never
**	see kv_pair.h (its PermString Key brings permstr.h's static).
*/
#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __STRING_VALUE_H	// NAME 1 [string_value.h]: neutral, what it declares (D10)
#define __STRING_VALUE_H

// The string accessor (vtable 0xAEB150: the destructor and three pure slots); DeltaString's
// is DeltaStringValueClass (delta_string.cpp).
class StringValueClass  // NAME 2: describes what it does (D38)
{
public:
	virtual ~StringValueClass() {}									// +0x00 0x4A7F00
	virtual const char *Get() const = 0;							// +0x04  // NAME 2: describes what it does
	virtual void Set(const char *value) = 0;						// +0x08  // NAME 2: describes what it does
	virtual StringValueClass *Clone() const = 0;					// +0x0C  // NAME 2: describes what it does
};

#endif
