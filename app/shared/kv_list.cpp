// Earth & Beyond client (reconstruction): app\shared\kv_list.cpp. Notes at the end of the file.
// NOT EXACT, the whole file: functional, on a std::map instead of the client's hash_map (kv_list.h).
#include "kv_list.h"
#include "kv_pair.h"
#include "unknown_debug.h"

KVListClass::KVListClass()
{
}

KVListClass::~KVListClass()
{
	for (unsigned int i = 0; i < Pairs.size(); i++) {
		delete Pairs[i];
	}
}




void KVListClass::Add(KVPairClass *item)
{
	G_ASSERT(item);
	if (item == 0) {
		return;
	}
	const char *key = item->Key.Get_String();
	std::map<const char *, KVPairClass *, KeyLess>::iterator i = Index.find(key);
	if (i != Index.end()) {
		KVPairClass *old = (*i).second;
		for (unsigned int n = 0; n < Pairs.size(); n++) {
			if (Pairs[n] == old) {
				Pairs[n] = item;
				break;
			}
		}
		Index.erase(i);
		Index[key] = item;
		delete old;
		return;
	}
	Index[key] = item;
	Pairs.push_back(item);
}



KVPairClass *KVListClass::Find(const char *key)
{
	if (key == 0) {
		G_ASSERT(key);
		return 0;
	}
	std::map<const char *, KVPairClass *, KeyLess>::iterator i = Index.find(key);
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
	return pair != 0 ? pair->Get_Type_Name() : 0;
}




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
	return pair != 0 ? pair->Get_Value_String(buf, size) : 0;
}







bool KVListClass::Set_Value_String(const char *key, const char *value)
{
	G_ASSERT(key && value);
	if (key == 0 || value == 0) {
		return false;
	}
	KVPairClass *pair = Find(key);
	return pair != 0 ? pair->Set_Value_String(value) : false;
}

/*
**	Reconstruction notes
**
**	client.exe 0x4B62B0-0x4B6D90: KVListClass (D24), functional (milestones.md 2.2, M1.2). The asserts
**	sit on the client's lines (23 'item', 51 / 68 / 82 'key', 97 'key && buf', 113 'key && value',
**	D25). Add 0x4B6760 replaces a pair with the same key in place and deletes the old one, otherwise
**	appends (the vector insert 0x4B7130); the lookups go through the index (0x4B7870 / 0x4B7030 in
**	the client's hash_map); the one-argument Get_Value_String writes into a static 0x1000-byte
**	buffer (0xBE3B60). Not byte-exact: the client's index is an SGI-style hash_map (kv_list.h).
*/
