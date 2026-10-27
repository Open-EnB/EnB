/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\kv_list.h (name assumed from kv_list.cpp; no
**	assert names it)
**
**	The list an encoder's key/value pairs go to: the pairs in the order added, and an index by
**	key. The client doesn't name the class (D24); the method names are guesses (D10), their
**	asserts' arguments are the client's (key, buf, value, item).
**
**	The index is SGI STL's hash_map<const char *, KVPairClass *> (libs/sgistl, D87): at +0x10, its
**	buckets +0x18, its count +0x24; the hash __stl_hash_string 0x4B6970 (this file's copy), the
**	keys compared with strcmp; the default constructor's 100 buckets come from this file's copy of
**	__stl_prime_list (0xAEB764).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __KV_LIST_H
#define __KV_LIST_H

#include <string.h>
#include <vector>
#include <hash_map.h>

class KVPairClass;

class KVListClass  // NAME 3: by convention (D24) from the client's file kv_list.cpp
{
public:
	KVListClass();																// 0x4B62B0
	~KVListClass();															// 0x4B6520 (deletes the pairs)

	void Add(KVPairClass *item);											// 0x4B6760 (line 23): replaces (and deletes) a pair with the same key  // NAME 2: describes what it does
	KVPairClass *Find(const char *key);									// 0x4B69A0 (line 51)  // NAME 2: describes what it does
	const char *Get_Type_Name(const char *key);						// 0x4B6A80 (line 68)  // NAME 2: describes what it does
	const char *Get_Value_String(const char *key);					// 0x4B6B60 (line 82; a static 0x1000-byte buffer, 0xBE3B60)  // NAME 2: describes what it does
	char *Get_Value_String(const char *key, char *buf, int size);	// 0x4B6BF0 (line 97)
	bool Set_Value_String(const char *key, const char *value);		// 0x4B6CE0 (line 113)  // NAME 2: describes what it does

	int Get_Count() const { return Pairs.size(); }  // NAME 2: describes what it does
	KVPairClass *Get(int index) const { return Pairs[index]; }  // NAME 2: describes what it does

	struct KeyEqual  // NAME 2: describes what it does (the SGI documentation's eqstr)
	{
		bool operator()(const char *a, const char *b) const { return strcmp(a, b) == 0; }
	};

	std::vector<KVPairClass *> Pairs;											// +0x00 (the client's too)  // NAME 2 [Pairs]: describes what it does
	std::hash_map<const char *, KVPairClass *, std::hash<const char *>, KeyEqual> Index;	// +0x10  // NAME 2 [Index]: describes what it does
};

#endif //__KV_LIST_H
