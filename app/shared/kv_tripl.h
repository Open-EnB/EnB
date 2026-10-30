/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\kv_tripl.h (the client's assert at line 92,
**	'index < V.size()', names it)
**
**	Triples of fixed-size strings (a key, a value and a short third string) and a list of them,
**	indexed by key. The client doesn't name the classes (D24); the method names are guesses
**	(D10), the vector's name V and the asserts' arguments (key, item, triple, entry) are the
**	client's.
**
**	The index is SGI STL's hash_map<std::string, KVTripleClass *> (libs/sgistl, D87) at +0x10,
**	like KVListClass's: its buckets at +0x14, its count at +0x24; the string hash 0x4BA580 (the
**	bucket function, the key taken by value) and 0x42DB00, the keys compared with
**	std::string's operator== (0x441480).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __KV_TRIPL_H  // NAME 3 [kv_tripl.h]: by convention from the client's kv_tripl.cpp and this header's assert
#define __KV_TRIPL_H

#include <string.h>
#include <vector>
#include <string>
#include <hash_map.h>
#include "unknown_debug.h"

/*
**	One triple: three strings copied into fixed buffers (0x340 bytes, from operator new). Each copy
**	asserts that the string fits ('strlen(value) < member_size', kv_tripl.cpp line 34).
*/
class KVTripleClass  // NAME 3: by convention (D24) from the client's file kv_tripl.cpp
{
public:
	KVTripleClass(const char *key, const char *value, const char *type);	// 0x4B84C0
	KVTripleClass(const KVTripleClass &src);										// 0x4B85F0

	void Copy_Member(char *member, unsigned int member_size, const char *value);	// 0x4B8730 (line 34; this unused, ret 0xC)  // NAME 2: describes what it does

	char Key[0x100];				// +0x000 (the index's key)  // NAME 2: describes what it does
	char Value[0x200];			// +0x100  // NAME 2: describes what it does (KVPairClass's analogue)
	char Type[0x40];				// +0x300  // NAME 1: neutral guess, a short string beside the value
};

/*
**	The list: the triples in the order added (V, the client's name) and an index by key.
*/
class KVTripleListClass  // NAME 3: by convention (D24) from kv_tripl.cpp, KVListClass's sibling
{
public:
	KVTripleListClass();																				// 0x4B87B0
	KVTripleListClass(const KVTripleListClass &src);											// 0x4B8A20
	KVTripleListClass(const KVTripleListClass &src, const char *prefix);				// 0x4B8DC0 (line 70)  // NAME 2: describes what it does
	~KVTripleListClass();																			// 0x4B9180
	KVTripleListClass &operator=(const KVTripleListClass &src);							// 0x4B9340 (line 95)

	void Add(KVTripleClass *item);																// 0x4B9690 (line 106)  // NAME 2: describes what it does
	void Clear();																						// 0x4B9850  // NAME 2: describes what it does
	KVTripleClass *Find(const char *key);														// 0x4B9930 (line 126)  // NAME 2: describes what it does
	bool Set(const char *key, const char *value, const char *type);					// 0x4B9A70  // NAME 2: describes what it does
	const char *Get_Type(const char *key);														// 0x4B9B60 (line 153)  // NAME 1: neutral guess
	const char *Get_Value(const char *key);													// 0x4B9BB0 (line 164)  // NAME 2: describes what it does

	int Get_Count() const { return V.size(); }  // NAME 2: describes what it does
	KVTripleClass *Get(int index) const;  // NAME 2: describes what it does

	std::vector<KVTripleClass *> V;														// +0x00
	std::hash_map<std::string, KVTripleClass *> Index;									// +0x10  // NAME 2 [Index]: describes what it does
};

/*
**	The client's only header assert (line 92); kv_list.h's Get has the same one at its line 73.
*/











inline KVTripleClass *KVTripleListClass::Get(int index) const
{
	G_ASSERT(index < V.size());
	return V[index];
}

#endif //__KV_TRIPL_H
