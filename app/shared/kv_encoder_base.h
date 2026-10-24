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

#ifndef __KV_ENCODER_BASE_H
#define __KV_ENCODER_BASE_H

#include <list>
#include <string>
#include "kv_list.h"
#include "kv_pair.h"

class StringValueClass;

// The encoder's state (0x3C bytes, the asserts' Data; its destructor 0x4B3C00 is the compiler's)
class KVEncoderDataClass
{
public:
	std::list<std::string> PrefixList;	// +0x00
	std::list<int> FlagList;				// +0x0C the saved Flags (Push_Flags)
	int Flags;									// +0x18
	std::string m_Unknown1C;				// +0x1C the prefixes joined (Pop_Prefix rebuilds it)
	std::string m_Unknown2C;				// +0x2C
};

// vtable 0xAEB62C (the destructor only), size 0x0C
class KVEncoderBaseClass
{
public:
	KVEncoderBaseClass(KVListClass *list, int flags, const char *prefix);	// 0x4B3850
	virtual ~KVEncoderBaseClass();												// +0x00 0x4B3B90 (deleting 0x4B3B10)

	const char *Make_Key(const char *name);									// 0x4B3D60 the prefixes, then name
	void Push_Prefix(const char *name);											// 0x4B40F0
	void Pop_Prefix();																// 0x4B4300
	void Set_Prefix(const char *prefix);										// 0x4B45B0 (the constructor inlines it)
	int Get_Flags() const;															// 0x4B3F30
	void Set_Flags(int flags);														// 0x4B3F70
	void Push_Flags();																// 0x4B3FC0
	void Pop_Flags();																	// 0x4B4050
	// the two bit classes of mask that Flags has (even bits 0x155, odd bits 0x2AA)
	void Unknown_Test(int mask, bool &even, bool &odd);						// 0x4B4820
	// hands value to a new pair if mask has either bit class (deletes it otherwise)
	void Add_String(int mask, const char *name, StringValueClass *value);	// 0x4B48A0
	// the same for a typed accessor (inlined for float in 0x4A6930; probably the time field's 0x4A8630)
	template<class T> void Add_Value(int mask, const char *name, KVAccessorClass<T> *accessor)
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

	KVEncoderDataClass *Data;		// +0x04
	KVListClass *List;				// +0x08 where the pairs go (kv_list.cpp's 0x4B6760 adds one)
};

#endif //__KV_ENCODER_BASE_H
