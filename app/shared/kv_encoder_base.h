/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\kv_encoder_base.h (name assumed from
**	kv_encoder_base.cpp; no assert names it)
**
**	The base of the key/value encoders the Delta fields hand themselves to (DeltaClass's slot 0):
**	a stack of name prefixes and a stack of flag words, and the list the encoded pairs go to. The
**	client doesn't name the classes (D24: named after the file).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __KV_ENCODER_BASE_H  // NAME 3 [kv_encoder_base.h]: header name assumed from the client's kv_encoder_base.cpp
#define __KV_ENCODER_BASE_H

#include <list>
#include <string>
#include "kv_list.h"
#include "kv_pair.h"

class StringValueClass;
class Vector3;

#ifdef UNKNOWN_PACKETDATA_BODIES	// (the login objects' files only: a new template here moves aux_roots.cpp's inlining)
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

// The encoder's state (0x3C bytes, the asserts' Data; its destructor 0x4B3C00 is the compiler's)
class KVEncoderDataClass  // NAME 2: after the asserts' Data
{
public:
	std::list<std::string> PrefixList;	// +0x00
	std::list<int> FlagList;				// +0x0C the saved Flags (Push_Flags)
	int Flags;									// +0x18  // NAME 2: after the asserts' FlagList
	std::string m_Unknown1C;				// +0x1C the prefixes joined (Pop_Prefix rebuilds it)
	std::string m_Unknown2C;				// +0x2C
};

// vtable 0xAEB62C (the destructor only), size 0x0C
class KVEncoderBaseClass  // NAME 3: by convention (D24) from the client's file kv_encoder_base.cpp
{
public:
	KVEncoderBaseClass(KVListClass *list, int flags, const char *prefix);	// 0x4B3850
	virtual ~KVEncoderBaseClass();												// +0x00 0x4B3B90 (deleting 0x4B3B10)

	const char *Make_Key(const char *name);									// 0x4B3D60 the prefixes, then name  // NAME 2: describes what it does
	void Push_Prefix(const char *name);											// 0x4B40F0  // NAME 2: describes what it does
	void Pop_Prefix();																// 0x4B4300  // NAME 2: describes what it does
	void Set_Prefix(const char *prefix);										// 0x4B45B0 (the constructor inlines it)  // NAME 2: describes what it does
	int Get_Flags() const;															// 0x4B3F30  // NAME 2: describes what it does
	void Set_Flags(int flags);														// 0x4B3F70  // NAME 2: describes what it does
	void Push_Flags();																// 0x4B3FC0  // NAME 2: describes what it does
	void Pop_Flags();																	// 0x4B4050  // NAME 2: describes what it does
	// the two bit classes of mask that Flags has (even bits 0x155, odd bits 0x2AA)
	void Unknown_Test(int mask, bool &even, bool &odd);						// 0x4B4820
	// hands value to a new pair if mask has either bit class (deletes it otherwise)
	void Add_String(int mask, const char *name, StringValueClass *value);	// 0x4B48A0  // NAME 2: describes what it does (D38)
	// the same for a typed accessor (inlined for float in 0x4A6930; probably the time field's 0x4A8630)
	template<class T> void Add_Value(int mask, const char *name, KVAccessorClass<T> *accessor)  // NAME 2: describes what it does
	{
		bool even;
		bool odd;
		Unknown_Test(mask, even, odd);
		if (!even && !odd) {
			delete accessor;
			return;
		}
		List->Add(new KVPairTemplateClass<T>(Make_Key(name), accessor, even, odd));
	}
#ifdef UNKNOWN_PACKETDATA_BODIES	// declared for the login objects' files only: any member declared here
								// moves aux_roots.cpp's inlining (0x6FB1E0, 0x702240, 0x7045F0)
	// The typed adds the login objects' visitors call (D104: they were unknown_packetdata.h's
	// LoginKVVisitorClass): a pair over a pointer to the member, kept when the mask has either bit class
	void Add(int mask, const char *name, int *value);					// 0x4C2EB0 (in avatar_desc.cpp's run)
	void Add(int mask, const char *name, char *buffer, int size);		// 0x4B4A70 (kv_encoder_base.cpp)
	void Add(int mask, const char *name, char *value);					// 0x4C2FE0 (in avatar_desc.cpp's run)
	void Add(int mask, const char *name, Vector3 *value);				// 0x4B2BB0
	template<class T> void Add_Typed(int mask, const char *name, T *value);	// the other typed adds (shipcust_desc.cpp's bool one)
	// The adds over a pointer accessor (the character slot's 0x8C0C40 quad, 0x8C0D80 long; shipcust_desc.cpp's
	// profession 0x4F6A80, race 0x4F6BB0, metal 0x4F6CE0)
	template<class T> void Add_Pointer(int mask, const char *name, T *value)	// NAME 2: what it does
	{
		bool even;
		bool odd;
		Unknown_Test(mask, even, odd);
		if (even || odd) {
			List->Add(new KVPairTemplateClass<T>(Make_Key(name), new KVPointerAccessorClass<T>(value), even, odd));
		}
	}
#endif

	KVEncoderDataClass *Data;		// +0x04
	KVListClass *List;				// +0x08 where the pairs go (kv_list.cpp's 0x4B6760 adds one)  // NAME 2: describes what it does
};

#endif //__KV_ENCODER_BASE_H
