/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\kv_pair.h (name assumed from kv_pair.cpp; no
**	assert names it)
**
**	One key/value pair of an encoder's list (kv_encoder_base.h): the key interned, two flags
**	(the encoder's two bit classes of the field's mask), and the value in the derived classes
**	(kv_pair_string.cpp: a string accessor). The client doesn't name the classes (D24).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __KV_PAIR_H
#define __KV_PAIR_H

#include "auxdata\permstr.h"

// vtable 0xAEB7EC (the destructor, then six pure slots), size 0x0C
class KVPairClass
{
public:
	KVPairClass(const char *key, bool even, bool odd);						// 0x4B7EB0
	virtual ~KVPairClass();															// +0x00 0x4B7F50 (deleting 0x4B7F20)
	virtual void Unknown_04() = 0;
	virtual const char *Unknown_08() const = 0;									// the value's type name ("char *": 0x4B8090)
	virtual void Unknown_0C() = 0;
	virtual void Unknown_10() = 0;
	virtual void Unknown_14() = 0;
	virtual void Unknown_18() = 0;

	PermString Key;					// +0x04
	bool m_Unknown08;					// +0x08 the encoder's even bits (KVEncoderBaseClass::Unknown_Test)
	bool m_Unknown09;					// +0x09 the odd bits
};

// A typed value reached through the pair (the float field's interface: vtable 0xAE78D0, the destructor
// and three pure slots). The header the client has these in is not known.
template<class T> class KVAccessorClass
{
public:
	virtual ~KVAccessorClass() {}
	virtual const T &Get() = 0;
	virtual void Set(const T &value) = 0;
	virtual KVAccessorClass<T> *Clone() const = 0;
};

// A pair holding a typed accessor and the value it had when the pair was made (float: vtable
// 0xAE7860, size 0x14; the constructor is inline)
template<class T> class KVPairTemplateClass : public KVPairClass
{
public:
	KVPairTemplateClass(const char *key, KVAccessorClass<T> *accessor, bool even, bool odd) :
		KVPairClass(key, even, odd),
		Accessor(accessor),
		Value(accessor->Get())
	{
	}
	virtual ~KVPairTemplateClass();
	virtual void Unknown_04();
	virtual const char *Unknown_08() const;
	virtual void Unknown_0C();
	virtual void Unknown_10();
	virtual void Unknown_14();
	virtual void Unknown_18();

	KVAccessorClass<T> *Accessor;		// +0x0C
	T Value;									// +0x10
};

#endif //__KV_PAIR_H
