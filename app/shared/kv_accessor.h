// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Interim placement (D137): a typed value reached through a key/value pair, out of kv_pair.h:
**	the client's kv_acc, kv_acc_string and kv_acc_time objects have no _PermStringInit, so they never
**	see kv_pair.h (its PermString Key brings permstr.h's static).
*/
#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __KV_ACCESSOR_H	// NAME 1 [kv_accessor.h]: neutral, what it declares (D10)
#define __KV_ACCESSOR_H

// A typed value reached through the pair (the float field's interface: vtable 0xAE78D0, the destructor
// and three pure slots). The header the client has these in is not known.
template<class T> class KVAccessorClass  // NAME 2: after the assert's parameter accessor (kv_pair.cpp); the class name describes it
{
public:
	virtual ~KVAccessorClass() {}
	virtual const T &Get() = 0;  // NAME 2: describes what it does
	virtual void Set(const T &value) = 0;  // NAME 2: describes what it does
	virtual KVAccessorClass<T> *Clone() const = 0;  // NAME 2: describes what it does
};

#endif
