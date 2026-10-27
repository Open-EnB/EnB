// Earth & Beyond client (reconstruction): app\shared\kv_tripl.cpp. Notes at the end of the file.
#include <string.h>
#include "kv_tripl.h"
#include "unknown_debug.h"

char *Unknown_4A1590(char *buffer, int size, const char *string);	// 0x4A1590: a bounded strcpy  // NAME 1: neutral placeholder (D10)
























void KVTripleClass::Copy_Member(char *member, unsigned int member_size, const char *value)
{
	if (value != 0) {
		G_ASSERT(strlen(value) < member_size);
		Unknown_4A1590(member, member_size, value);
	} else {
		member[0] = 0;
	}
}

KVTripleClass::KVTripleClass(const char *key, const char *value, const char *type)
{
	Copy_Member(Key, sizeof(Key), key);
	Copy_Member(Value, sizeof(Value), value);
	Copy_Member(Type, sizeof(Type), type);
}

KVTripleClass::KVTripleClass(const KVTripleClass &src)
{
	Copy_Member(Key, sizeof(Key), src.Key);
	Copy_Member(Value, sizeof(Value), src.Value);
	Copy_Member(Type, sizeof(Type), src.Type);
}

KVTripleListClass::KVTripleListClass() {}
// NOT EXACT: inline budget, the client inlines the triple's copy constructor here (Copy_Member out of line); ours calls 0x4B85F0
KVTripleListClass::KVTripleListClass(const KVTripleListClass &src)
{
	*this = src;
}
// NOT EXACT: frame (0x38 vs 0x3C) and inline budget, the client calls the bucket vector's constructor 0x4BA530 out of line and keeps match in ebx
KVTripleListClass::KVTripleListClass(const KVTripleListClass &src, const char *prefix)
{
	char *match = new char[strlen(prefix) + 2];
	strcpy(match, prefix);
	strcat(match, ".");
	int len = strlen(match);
	for (unsigned int i = 0; i < src.V.size(); i++) {
		KVTripleClass *triple = src.V[i];
		G_ASSERT(triple);
		if (strncmp(match, triple->Key, len) == 0) {
			Add(new KVTripleClass(triple->Key + len, triple->Value, 0));
		}
	}
	delete [] match;
}
// NOT EXACT: inline budget, the client calls the vector's erase pieces (0x4BAC30, 0x4B9D60), bucket size() and _M_delete_node out of line
KVTripleListClass::~KVTripleListClass()
{
	for (unsigned int i = 0; i < V.size(); i++) {
		delete V[i];
	}
	V.clear();
	Index.clear();
}




KVTripleListClass &KVTripleListClass::operator=(const KVTripleListClass &src)
{
	Clear();
	for (unsigned int i = 0; i < src.V.size(); i++) {
		KVTripleClass *entry = src.Get(i);
		FAIL_IF(!entry) return *this;
		Add(new KVTripleClass(*entry));
	}
	return *this;
}




void KVTripleListClass::Add(KVTripleClass *item)
{
	G_ASSERT(item);
	Index[std::string(item->Key)] = item;

	V.push_back(item);
}

void KVTripleListClass::Clear()
{
	for (unsigned int i = 0; i < V.size(); i++) {
		delete V[i];
	}
	V.clear();
	Index.clear();
}




KVTripleClass *KVTripleListClass::Find(const char *key)
{
	G_ASSERT(key);
	std::hash_map<std::string, KVTripleClass *>::iterator i = Index.find(key);
	if (i == Index.end()) return 0;
	return (*i).second;
}





bool KVTripleListClass::Set(const char *key, const char *value, const char *type)
{
	KVTripleClass *triple = Find(key);
	if (triple == 0) {
		return false;
	}
	triple->Copy_Member(triple->Value, sizeof(triple->Value), value);
	triple->Copy_Member(triple->Type, sizeof(triple->Type), type);
	return true;
}





const char *KVTripleListClass::Get_Type(const char *key)
{
	G_ASSERT(key);
	KVTripleClass *triple = Find(key);
	if (triple != 0) return triple->Type;
	return 0;
}




const char *KVTripleListClass::Get_Value(const char *key)
{
	G_ASSERT(key);
	KVTripleClass *triple = Find(key);
	if (triple != 0) return triple->Value;
	return 0;
}

/*
**	Reconstruction notes
**
**	client.exe 0x4B84C0-0x4B9BB0: KVTripleClass and KVTripleListClass (D24; the file follows
**	kv_pair_string.cpp, kv_util.cpp follows at 0x4BB0E0), then the index's instances to 0x4BAEB0. The
**	asserts sit on the client's lines (34 'strlen(value) < member_size', 70 'triple', 95 '!entry',
**	106 'item', 126 / 153 / 164 'key'; kv_tripl.h's 92 'index < V.size()', D25).
**	Copy_Member 0x4B8730 is a member that ignores this (ret 0xC); both triple constructors inline it.
**	The list is KVListClass's twin on a std::string key: the default constructor 0x4B87B0 (identical
**	bytes to KVListClass's 0x4B62B0), Add 0x4B9690 (`Index[std::string(item->Key)] = item`, the
**	temporary dying before the push_back), Find 0x4B9930 (`if (i == Index.end()) return 0;`),
**	Set / Get_Type / Get_Value. operator= 0x4B9340 holds the copy loop (Get's assert merged with the
**	loop's test); the copy constructor 0x4B8A20 is `*this = src`. The prefix constructor 0x4B8DC0
**	copies the triples whose key starts with "prefix." under the rest of the key (callers: kv_util.cpp
**	0x4BB3F0 uses the 3-argument constructor and Add; 0x814A30 the default constructor, Find and the
**	destructor). Three functions stay inexact, marked at their definitions (attempts.csv).
*/
