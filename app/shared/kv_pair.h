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
#include "strconv_lib.h"

// vtable 0xAEB7EC (the destructor, then six pure slots), size 0x0C
class KVPairClass
{
public:
	KVPairClass(const char *key, bool even, bool odd);						// 0x4B7EB0
	virtual ~KVPairClass();															// +0x00 0x4B7F50 (deleting 0x4B7F20)
	// The slots' purposes from the string and float pairs (kv_pair_string.cpp; the float pair's
	// 0x4484C0-0x448610); the names are descriptive guesses (D10).
	virtual KVPairClass *Clone() const = 0;									// +0x04 a copy (accessor cloned)
	virtual const char *Get_Type_Name() const = 0;							// +0x08 ("char *": 0x4B8090, "float")
	virtual char *Get_Value_String(char *buf, int size) const = 0;		// +0x0C the value as text, into buf
	virtual bool Set_Value_String(const char *text) = 0;					// +0x10 parses text into the value
	virtual bool Is_Changed() const = 0;										// +0x14 the value differs from the saved one
	virtual void Update_Saved() = 0;											// +0x18 saves the current value

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
	// Clone's copy: the accessor cloned, the saved value kept (the time pair's 0x4A8770)
	KVPairTemplateClass(const KVPairTemplateClass<T> &that) :
		KVPairClass(that.Key.Get_String(), that.m_Unknown08, that.m_Unknown09),
		Accessor(that.Accessor->Clone()),
		Value(that.Value)
	{
	}
	virtual ~KVPairTemplateClass();
	virtual KVPairClass *Clone() const;
	virtual const char *Get_Type_Name() const;
	virtual char *Get_Value_String(char *buf, int size) const;
	virtual bool Set_Value_String(const char *text);
	virtual bool Is_Changed() const;
	virtual void Update_Saved();

	KVAccessorClass<T> *Accessor;		// +0x0C
	T Value;									// +0x10 the saved value
};

// The bodies (the time field's unsigned int pair in delta_time.cpp's object: 0x4A8770-0x4A8990,
// vtable 0xAEB26C; the float pair's in a client object, 0x4484C0-0x448B10, vtable 0xAE7860)
template<class T> KVPairTemplateClass<T>::~KVPairTemplateClass()
{
	delete Accessor;
}

template<class T> KVPairClass *KVPairTemplateClass<T>::Clone() const
{
	return new KVPairTemplateClass<T>(*this);
}

template<class T> const char *KVPairTemplateClass<T>::Get_Type_Name() const
{
	return Type_Name((T *)0);
}

template<class T> char *KVPairTemplateClass<T>::Get_Value_String(char *buf, int size) const
{
	return Value_To_String(buf, size, Accessor->Get());
}

template<class T> bool KVPairTemplateClass<T>::Set_Value_String(const char *text)
{
	T value;
	if (String_To_Value(text, &value)) {
		Accessor->Set(value);
		return true;
	}
	return false;
}

template<class T> bool KVPairTemplateClass<T>::Is_Changed() const
{
	return Value != Accessor->Get();
}

template<class T> void KVPairTemplateClass<T>::Update_Saved()
{
	Value = Accessor->Get();
}

#endif //__KV_PAIR_H
