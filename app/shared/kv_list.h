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
**	NOT THE CLIENT'S LAYOUT: the client's index is an SGI-style hash_map<const char *, KVPairClass *>
**	(at +0x10: buckets +0x18, count +0x24, hash h * 5 + c, 0x4B6970; its constructor 0x4B62B0 asks for
**	100 buckets, a prime from the table at 0xAEB764), which the tree has no header for. Here it is a
**	std::map; nothing outside kv_list.cpp reaches the members.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __KV_LIST_H
#define __KV_LIST_H

#include <string.h>
#include <vector>
#include <map>

class KVPairClass;

class KVListClass
{
public:
	KVListClass();																// 0x4B62B0
	~KVListClass();															// 0x4B6520 (deletes the pairs)

	void Add(KVPairClass *item);											// 0x4B6760 (line 23): replaces (and deletes) a pair with the same key
	KVPairClass *Find(const char *key);									// 0x4B69A0 (line 51)
	const char *Get_Type_Name(const char *key);						// 0x4B6A80 (line 68)
	const char *Get_Value_String(const char *key);					// 0x4B6B60 (line 82; a static 0x1000-byte buffer, 0xBE3B60)
	char *Get_Value_String(const char *key, char *buf, int size);	// 0x4B6BF0 (line 97)
	bool Set_Value_String(const char *key, const char *value);		// 0x4B6CE0 (line 113)

	int Get_Count() const { return Pairs.size(); }
	KVPairClass *Get(int index) const { return Pairs[index]; }

	struct KeyLess
	{
		bool operator()(const char *a, const char *b) const { return strcmp(a, b) < 0; }
	};

	std::vector<KVPairClass *> Pairs;											// +0x00 (the client's too)
	std::map<const char *, KVPairClass *, KeyLess> Index;						// (the client's hash_map, see above)
};

#endif //__KV_LIST_H
