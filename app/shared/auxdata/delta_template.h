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
#include "..\g_assert.h"

template<class T> class DeltaTemplateClass : public DeltaClass  // NAME 3: by convention (D24) from the client's file delta_template.h
{
public:
	// in class, as the client's objects show: an object numbers the types whose constructors it compiles (the
	// guard-less initialiser of the types reached while parsing, workflow.md 3.4); float's is inlined in 0x4A6110
	DeltaTemplateClass(const char *name, const T &value = T()) : DeltaClass(name), Value(value) { Unknown04 = UnknownGlobal; }

	virtual void Unknown_00(KVEncoderClass *visitor);					// +0x00 (float: 0x4A6C90)
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

// UnknownGlobal is defined (and numbered at start-up) in delta_numbering.h, by the objects that include it

// The time field: a whole-class specialisation (the interpolator's constructor 0x4A6110 calls its constructor
// 0x4A8150 out of line where it inlines the float fields'; delta_time.cpp's object holds its members and global)
template<> class DeltaTemplateClass<DurationClass> : public DeltaClass
{
public:
	DeltaTemplateClass(const char *name, const DurationClass &value = DurationClass());	// 0x4A8150

	virtual void Unknown_00(KVEncoderClass *visitor);					// +0x00
	virtual bool Read(LinearBufferClass &buf, const DurationClass &base);	// +0x2C 0x4A81D0
	virtual bool Write(LinearBufferClass &buf);				// +0x30 0x4A8270

	void Set_Value(const DurationClass &value)  // NAME 2: describes what it does
	{
		if (Value != value) {
			Value = value;
			Unknown_34(this);
		}
	}

	const DurationClass &Get_Value() const;	// out of line in ui_scan.cpp 0x60E6A0 (0x60ECD0)  // NAME 2: describes what it does
	DurationClass Value;	// +0x88 (8-aligned)  // NAME 2: describes what it does

	static int UnknownGlobal;	// 0xBE3AA4
};

// (the lines down to Write hold code of the client's header not rebuilt yet: its asserts are on
// lines 91 and 110)
inline const DurationClass &DeltaTemplateClass<DurationClass>::Get_Value() const { return Value; }






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

template<class T> void DeltaTemplateClass<T>::Unknown_00(KVEncoderClass *visitor)
{
	visitor->Add_Value(Unknown80, Name.Get_String(), new DeltaTemplateAccessorClass<T>(this));
}

#endif //__DELTA_TEMPLATE_H
