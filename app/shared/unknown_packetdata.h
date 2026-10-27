/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	PROVISIONAL file name (no assert or __FILE__ names it): the two interfaces the login's data
**	objects implement (docs/kernel-map-step9.md 5), moved here from tsk_login.h (D90 9b):
**	the wire interface 0xAEB9EC (pk.h's Unknown_PacketDataClass with a third slot, the size) and
**	the key/value visitor interface 0xAEADC8 (delta.h's DeltaInterfaceClass has the same table),
**	and the KV encoder's typed adds their visitors call, declared only.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UNKNOWN_PACKETDATA_H  // NAME 1 [unknown_packetdata.h]: neutral file name (D10)
#define UNKNOWN_PACKETDATA_H

#ifdef UNKNOWN_PACKETDATA_BODIES	// the key/value adds' bodies: unknown_login_data.cpp only (kerneltest's stand-ins
								// and tsk_login.cpp see the declaration; the KV headers pulled app.lib objects into kerneltest)
#include "kv_encoder_base.h"
#include "strconv_lib.h"

// strconv_lib.cpp's converters for the character slot's pairs (here, not in strconv_lib.h: a new
// overload there moves aux_roots.cpp's inlining, the header canary 0x6FB1E0)
char *Value_To_String(char *buf, int size, long value);					// 0x4A34D0 (the "long" pairs)
bool String_To_Value(const char *text, long *value);					// 0x4A3500
char *Value_To_String(char *buf, int size, __int64 value);				// 0x4A3A80 (the "quad" pairs)
bool String_To_Value(const char *text, __int64 *value);					// 0x4A3AB0
inline const char *Type_Name(long *) { return "long"; }					// 0x8C1190
inline const char *Type_Name(__int64 *) { return "quad"; }				// 0x8C0F70

// A pair's accessor over a member (the character slot's: vtables 0xB0C594 quad, 0xB0C5CC long)
template<class T> class KVPointerAccessorClass : public KVAccessorClass<T>	// NAME 2: what it does
{
public:
	KVPointerAccessorClass(T *value) : Value(value) {}
	virtual const T &Get() { return *Value; }
	virtual void Set(const T &value) { *Value = value; }
	virtual KVAccessorClass<T> *Clone() const { return new KVPointerAccessorClass<T>(Value); }

	T *Value;
};
#endif

class LinearBufferClass;
class Vector3;


// The interface vtable 0xAEB9EC (three pure slots): pk.h's Unknown_PacketDataClass with its third slot
class LoginPacketDataClass	// NAME 1: neutral (D10)
{
public:
	virtual bool Write(LinearBufferClass *buffer) = 0;	// +0x00 NAME 2: pk.h's (the objects return true)
	virtual bool Read(LinearBufferClass *buffer) = 0;	// +0x04 NAME 2: pk.h's
	virtual int Get_Size() = 0;							// +0x08 NAME 1: the bytes Write writes (the galaxy's 0x54)
};

// The KV encoder's typed adds as the visitors call them (ecx the encoder; KVEncoderBaseClass's
// out-of-line members, not rebuilt): a pair over a pointer to the member, kept when the mask
// has either bit class (kv_encoder_base.h's Add_Value).
class LoginKVVisitorClass	// NAME 1: neutral (D10): the visitor argument, a KVEncoderBaseClass
{
public:
	void Add(int mask, const char *name, int *value);					// 0x4C2EB0 (in avatar_desc.cpp's run)
	void Add(int mask, const char *name, char *buffer, int size);		// 0x4B4A70 (kv_encoder_base.cpp)
	void Add(int mask, const char *name, char *value);					// 0x4C2FE0 (in avatar_desc.cpp's run)
	void Add(int mask, const char *name, Vector3 *value);				// 0x4B2BB0
	template<class T> void Add_Typed(int mask, const char *name, T *value);	// the other typed adds (shipcust_desc.cpp: 0x4F6A80, 0x4F6BB0, 0x4F6CE0, the bool one)
	// The adds the character slot's module instantiates (0x8C0C40 quad, 0x8C0D80 long): a pair over a
	// pointer accessor, kept when the mask has either bit class
#ifdef UNKNOWN_PACKETDATA_BODIES
	template<class T> void Add_Pointer(int mask, const char *name, T *value)	// NAME 2: what it does
	{
		KVEncoderBaseClass *encoder = (KVEncoderBaseClass *)this;
		bool even;
		bool odd;
		encoder->Unknown_Test(mask, even, odd);
		if (even || odd) {
			encoder->List->Add(new KVPairTemplateClass<T>(encoder->Make_Key(name), new KVPointerAccessorClass<T>(value), even, odd));
		}
	}
#else
	template<class T> void Add_Pointer(int mask, const char *name, T *value);
#endif
	virtual ~LoginKVVisitorClass();									// +0x00 (the encoder's virtuals: delta.h's DeltaChildVisitorClass)
	virtual void Add_Child(int mask, const char *name, void *child);	// +0x04 a key/value object under the name  // NAME 2: what it does
	virtual void Unknown_08();
	virtual void Unknown_0C();
	virtual void Add_Color(int mask, const char *name, Vector3 *value);	// +0x10 (shipcust_desc.cpp's colours)  // NAME 1: its users' keys
};

// The one-slot interface vtable 0xAEADC8 (delta.h's DeltaInterfaceClass has the same table)
class LoginInterfaceAEADC8Class	// NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(void *visitor) = 0;			// a LoginKVVisitorClass (the encoder)
};

#endif
