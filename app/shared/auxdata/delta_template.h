// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\auxdata\delta_template.h (the client's __FILE__
**	string, from the float field's Read 0x4A6DF0, line 110)
**
**	A leaf field holding one value of type T (the float field's vtable 0xAEAEB0 overrides slots 0,
**	1, 0x2C and 0x30 of DeltaClass's). Only what the delta files use so far: the constructor,
**	Set_Value, and the generic Write and Read (the float field's 0x4A6E60 and 0x4A6DF0, asserts on
**	the client's lines 91 and 110, D25). The client doesn't name the template (D24).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __DELTA_TEMPLATE_H
#define __DELTA_TEMPLATE_H

#include "delta.h"
#include "..\duration.h"
#include "linearbu.h"
#include "..\unknown_debug.h"

template<class T> class DeltaTemplateClass : public DeltaClass  // NAME 3: by convention (D24) from the client's file delta_template.h
{
public:
	// float's is inlined in 0x4A6110 (DeltaClass, Value, the vtable, then +0x04); the time and money
	// fields' are out of line (0x4A8150, 0x4A7080: see the extern template below)
	DeltaTemplateClass(const char *name, const T &value = T());

	virtual void Unknown_00(DeltaChildVisitorClass *visitor);					// +0x00 (float: 0x4A6C90)
	virtual bool Read(LinearBufferClass &buf, const DurationClass &base);	// +0x2C (float: 0x4A6DF0)
	virtual bool Write(LinearBufferClass &buf);				// +0x30 (float: 0x4A6E60)

	// stores the value and reports the change through Unknown_34 (inlined in 0x4A6590)
	void Set_Value(const T &value)  // NAME 2: describes what it does
	{
		if (Value != value) {
			Value = value;
			Unknown_34(this);
		}
	}

	T Value;			// +0x84, or +0x88 for an 8-aligned T (MoneyClass)  // NAME 2: describes what it does

	static int UnknownGlobal;	// the per-class global at +0x04 (float: 0xC416F0, time: 0xBE3AA4)
};

template<class T> int DeltaTemplateClass<T>::UnknownGlobal;

template<class T> DeltaTemplateClass<T>::DeltaTemplateClass(const char *name, const T &value) :
	DeltaClass(name),
	Value(value)
{
	m_Unknown04 = UnknownGlobal;
}

// The interpolator's constructor 0x4A6110 calls the time field's constructor (0x4A8150) where it
// inlines the float fields': VC6's extern template (C4231) gives that; delta_time.cpp's object
// holds the instance.
extern template class DeltaTemplateClass<DurationClass>;

// (the lines down to Write hold code of the client's header not rebuilt yet: its asserts are on
// lines 91 and 110)




















template<class T> bool DeltaTemplateClass<T>::Write(LinearBufferClass &buf)
{
	if (buf.Write(&Value, sizeof(Value)) != sizeof(Value)) {
		G_ASSERT(0);
		return false;
	}
	return true;
}










template<class T> bool DeltaTemplateClass<T>::Read(LinearBufferClass &buf, const DurationClass &)
{
	T value;
	if (buf.Read(&value, sizeof(value)) != sizeof(value)) {
		G_ASSERT(0);
		return false;
	}
	Value = value;
	return true;
}

// The field's value for the encoder (float: vtable 0xAEAF1C)
template<class T> class DeltaTemplateAccessorClass : public KVAccessorClass<T>  // NAME 2: describes what it does
{
public:
	DeltaTemplateAccessorClass(DeltaTemplateClass<T> *owner) : Owner(owner) {}
	virtual const T &Get() { return Owner->Value; }								// float: 0x4A6EE0
	virtual void Set(const T &value) { Owner->Set_Value(value); }			// float: 0x4A6F00
	virtual KVAccessorClass<T> *Clone() const { return new DeltaTemplateAccessorClass<T>(Owner); }	// float: 0x4A6F40

	DeltaTemplateClass<T> *Owner;		// +0x04  // NAME 2: describes what it does
};

template<class T> void DeltaTemplateClass<T>::Unknown_00(DeltaChildVisitorClass *visitor)
{
	visitor->Add_Value(m_Unknown80, Name.Get_String(), new DeltaTemplateAccessorClass<T>(this));
}

#endif //__DELTA_TEMPLATE_H
