/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\auxdata\delta.h (name assumed from delta.cpp)
**
**	The aux-data field model: a tree of named fields. DeltaClass is one field (leaf types derive
**	from it: float 0xAEAEB0, string 0xAEB0E4, time 0xAEB1EC...), DeltaCompoundClass a field with
**	children. The client names neither class (D24); the roles follow the reference decoder's notes
**	(docs/research/client-re-notes.md: DeltaBase, DeltaCompound), checked against the code here.
**	Virtuals whose purpose isn't known are named after their vtable offset (D10).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __DELTA_H  // NAME 3 [delta.h]: header name assumed from the client's delta.cpp
#define __DELTA_H

#include <vector>
#include "name.h"
#include "..\kv_encoder_base.h"

class LinearBufferClass;
class DurationClass;

class DeltaClass;
class DeltaInterfaceClass;
class Vector3;
class DeltaCompoundClass;
class LookupClass;

// Double dispatch (DeltaClass::Accept 0x4A4FE0 calls slot 0, DeltaCompoundClass::Accept 0x4A5650
// slot 1). VC6 lays out overloaded virtuals in reverse declaration order, so this order gives
// Visit(DeltaClass *) slot 0.
class DeltaVisitorClass  // NAME 2: describes what it does
{
public:
	virtual void Visit(DeltaCompoundClass *delta) = 0;		// +0x04  // NAME 2: describes what it does
	virtual void Visit(DeltaClass *delta) = 0;				// +0x00

	// 0x4B0860 (in an object not attributed yet; called from encode.cpp, lookup.cpp and
	// 0x4AB7D0/0x4ABA70): each child's Accept(*this), in order
	void Visit_Children(DeltaCompoundClass *delta);  // NAME 2: describes what it does
};

// The argument of DeltaClass's slot 0, an encoder (the leaves call KVEncoderBaseClass's members on
// it). DeltaCompoundClass::Unknown_00 (0x4A55D0) calls its slot 1 once per child: a slot the
// encoder base's vtable (0xAEB62C, the destructor only) doesn't have, so the encoders the fields
// are given derive from it.
// The login objects hand it their own children and colours too (D104: slots +0x04-+0x10).
class DeltaChildVisitorClass : public KVEncoderBaseClass  // NAME 2: describes what it does
{
public:
	virtual void Add_Child(int mask, const char *name, DeltaInterfaceClass *child) = 0;	// +0x04 a key/value object under the name  // NAME 2: what it does
	// the login objects' slots (a virtual more here moves aux_roots.cpp's VC6 symbol count: its pad
	// absorbs these, workflow.md 3.3)
	virtual void Unknown_08() = 0;
	virtual void Unknown_0C() = 0;
	virtual void Add_Color(int mask, const char *name, Vector3 *value) = 0;	// +0x10 (shipcust_desc.cpp's colours)  // NAME 1: its users' keys

};

// The base DeltaClass's constructor stores first (vtable 0xAEADC8: one slot, _purecall).
class DeltaInterfaceClass  // NAME 2: describes what it does
{
public:
	virtual void Unknown_00(DeltaChildVisitorClass *visitor) = 0;
};

// Per-class globals stored at +0x04 (E&B's own type information, rtti.h?): DeltaClass 0xBE3A78,
// DeltaCompoundClass 0xBE3A74 (its static UnknownGlobal: rtti.h's cast 0x598D80 compares it).
extern int DeltaUnknownGlobal;  // NAME 1: neutral (D10)

// vtable 0xAEAD70, size 0x84
class DeltaClass : public DeltaInterfaceClass  // NAME 3: by convention (D24) from the client's file delta.cpp (D37; the reference decoder notes: DeltaBase)
{
public:
	DeltaClass(const char *name);														// 0x4A4D40
	virtual void Unknown_00(DeltaChildVisitorClass *visitor);					// +0x00 0x4A4E10
	virtual ~DeltaClass() {}															// +0x04 0x4A5150
	virtual void Accept(DeltaVisitorClass &visitor);								// +0x08 0x4A4FE0  // NAME 2: describes what it does
	virtual bool Unknown_0C(LinearBufferClass &buf);							// +0x0C 0x4A4EC0, Unknown_38 then Write
	virtual bool Unknown_10(LinearBufferClass &buf, const DurationClass &base);	// +0x10 0x4A4E80, Read then Unknown_3C
	virtual void Set_Parent(DeltaClass *parent, int slot, int &index);		// +0x14 0x4A5000  // NAME 2: describes what it does
	virtual void Unknown_18();															// +0x18 0x4A4DB0
	virtual void Unknown_1C();															// +0x1C 0x4A4DD0
	virtual void Unknown_20();															// +0x20 0x4A4DF0
	virtual DeltaClass *As_Delta() { return this; }								// +0x24 0x4A3C50  // NAME 2: describes what it does
	virtual DeltaCompoundClass *As_Compound();									// +0x28 0x4A4E30  // NAME 2: describes what it does
	virtual bool Read(LinearBufferClass &, const DurationClass &) { return true; }	// +0x2C 0x4A3C70 (the leaves read their value;
																								// the time field subtracts the 8-byte base from its value, 0x4A81D0)
	virtual bool Write(LinearBufferClass &) { return true; }				// +0x30 0x4A3C90 (the leaves write it)  // NAME 2: describes what it does
	virtual void Unknown_34(DeltaClass *delta);									// +0x34 0x4A4EF0
	virtual void Unknown_38(DeltaClass *delta);									// +0x38 0x4A4FB0
	virtual void Unknown_3C(DeltaClass *delta);									// +0x3C 0x4A4F20
	virtual void Unknown_40(DeltaClass *delta);									// +0x40 0x4A4F50
	virtual void Unknown_44(DeltaClass *delta);									// +0x44 0x4A4F80

	int m_Unknown04;					// +0x04 the class's global (see above)
	NameClass Name;					// +0x08 the constructor's name  // NAME 2: describes what it does
	NameClass FullName;				// +0x3C the parent's FullName, then Name (Set_Parent)  // NAME 2: describes what it does
	unsigned int m_Unknown70;		// +0x70 0, 1 or 2 (Unknown_18 sets 2, Unknown_1C 0, Unknown_20 caps at 1)
	DeltaClass *Parent;				// +0x74  // NAME 2: describes what it does
	int m_Unknown78;					// +0x78 the position in the parent's Children
	int m_Unknown7C;					// +0x7C a running index over the tree (Set_Parent)
	int m_Unknown80;					// +0x80 0x3FF
};

// vtable 0xAEABE4, size 0x9C
class DeltaCompoundClass : public DeltaClass  // NAME 3: by convention (D24) from the client's file delta.cpp (D37; the reference decoder notes: DeltaCompound)
{
public:
	DeltaCompoundClass(const char *name) : DeltaClass(name), Lookup(0), m_Unknown98(0)
	{
		m_Unknown04 = UnknownGlobal;
	}
	static int UnknownGlobal;	// the per-class global at +0x04 (0xBE3A74)
	virtual void Unknown_00(DeltaChildVisitorClass *visitor);					// +0x00 0x4A55D0
	virtual ~DeltaCompoundClass();													// +0x04 0x4A5070
	virtual void Accept(DeltaVisitorClass &visitor);								// +0x08 0x4A5650
	virtual bool Unknown_0C(LinearBufferClass &buf);							// +0x0C 0x4A5270
	virtual bool Unknown_10(LinearBufferClass &buf, const DurationClass &base);	// +0x10 0x4A5300
	virtual void Set_Parent(DeltaClass *parent, int slot, int &index);		// +0x14 0x4A51A0
	virtual void Unknown_18();															// +0x18 0x4A5410
	virtual void Unknown_1C();															// +0x1C 0x4A5490
	virtual void Unknown_20();															// +0x20 0x4A53A0
	virtual DeltaCompoundClass *As_Compound() { return this; }				// +0x28 0x4A3CB0
	virtual void Unknown_48();															// +0x48 0x4A5170

	void Add(DeltaClass *child);														// 0x4A5670  // NAME 2: describes what it does
	LookupClass *Get_Lookup();															// 0x4A5510  // NAME 2: after the asserts' Lookup
	// 0x4A3FA0: one byte picks the decoder (0 the record form, else the tree form, encode.cpp).
	// Defined in the root's object 0x4A3990-0x4A4780 (unknown_root.cpp, UnknownRootClass's, derived
	// from this one); which class the client declared it in isn't known
	bool Decode(LinearBufferClass &buf, const DurationClass &base);  // NAME 2: describes what it does

	std::vector<DeltaClass *> Children;		// +0x84  // NAME 2 [Children]: describes what it does
	LookupClass *Lookup;							// +0x94 (assert name), made on first use
	int m_Unknown98;								// +0x98 the number of indices Set_Parent gave the subtree
};

#endif //__DELTA_H
