// Earth & Beyond client (reconstruction): app\shared\kv_list.cpp. Notes at the end of the file.
// The index is SGI STL's hash_map (libs/sgistl, D87).
#include "kv_list.h"
#include "kv_pair.h"
#include "unknown_debug.h"

KVListClass::KVListClass()
{
}
// NOT EXACT: inline budget, the hashtable destructor's clear() inlines _M_delete_node (the client calls 0x4B7B60)
KVListClass::~KVListClass()
{
	for (unsigned int i = 0; i < Pairs.size(); i++) {
		delete Pairs[i];
	}
	Pairs.clear();
	Index.clear();
}


void KVListClass::Add(KVPairClass *item)
{
	G_ASSERT(item);
	if (item == 0) {
		return;
	}
	KVPairClass *old = Index[item->Key.Get_String()];

	if (old != 0) {

		for (unsigned int n = 0; n < Pairs.size(); n++) {
			if (Pairs[n] == old) {
				Pairs[n] = item;
				break;
			}
		}

		Index[item->Key.Get_String()] = item;
		delete old;
		return;
	}
	Index[item->Key.Get_String()] = item;
	Pairs.push_back(item);
}



KVPairClass *KVListClass::Find(const char *key)
{
	if (key == 0) {
		G_ASSERT(key);
		return 0;
	}
	std::hash_map<const char *, KVPairClass *, std::hash<const char *>, KeyEqual>::iterator i = Index.find(key);
	if (i == Index.end()) {
		return 0;
	}
	return (*i).second;
}





const char *KVListClass::Get_Type_Name(const char *key)
{
	if (key == 0) {
		G_ASSERT(key);
		return 0;
	}
	KVPairClass *pair = Find(key);
	if (pair != 0) return pair->Get_Type_Name();
	return 0;
}


// NOT EXACT: inline budget, the client calls the hashtable's find 0x4B7030 out of line here
const char *KVListClass::Get_Value_String(const char *key)
{
	static char buf[0x1000];
	if (key == 0) {
		G_ASSERT(key);
		return 0;
	}
	KVPairClass *pair = Find(key);
	return pair != 0 ? pair->Get_Value_String(buf, sizeof(buf)) : 0;
}







char *KVListClass::Get_Value_String(const char *key, char *buf, int size)
{
	G_ASSERT(key && buf);
	if (key == 0 || buf == 0) {
		return 0;
	}
	KVPairClass *pair = Find(key);
	if (pair != 0) return pair->Get_Value_String(buf, size);
	return 0;
}






bool KVListClass::Set_Value_String(const char *key, const char *value)
{
	G_ASSERT(key && value);
	if (key == 0 || value == 0) {
		return false;
	}
	KVPairClass *pair = Find(key);
	if (pair != 0) return pair->Set_Value_String(value);
	return false;
}
/*
**	Reconstruction notes
**
**	client.exe 0x4B62B0-0x4B6D90: KVListClass (D24; milestones.md 2.2, M1.2). The asserts sit on the
**	client's lines (23 'item', 51 / 68 / 82 'key', 97 'key && buf', 113 'key && value', D25). Add
**	0x4B6760 replaces a pair with the same key in place and deletes the old one, otherwise appends
**	(the vector insert 0x4B7130); the index is SGI STL's hash_map (libs/sgistl, D87): find_or_insert
**	0x4B7870, resize 0x4B7B80, find 0x4B7030, __stl_hash_string 0x4B6970; the one-argument
**	Get_Value_String writes into a static 0x1000-byte buffer (0xBE3B60). Forms: the lookups return
**	through `if (pair != 0) return ...; return 0;` (one shared failure tail, no shrink-wrapped prologue).
**	NOT EXACT: the destructor 0x4B6520 and Get_Value_String 0x4B6B60 (inline budget, attempts.csv).
*/
