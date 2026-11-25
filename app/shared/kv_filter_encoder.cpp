// Earth & Beyond client (reconstruction): app\shared\kv_filter_encoder.cpp, PROVISIONAL file name (D137; notes at the end)
#include <string.h>
#include "kv_encoder.h"

// @0x4B5AD0
KVFilterEncoderClass::KVFilterEncoderClass(KVListClass *list, int flags, const char *prefix) :
	KVEncoderClass(*list, flags, prefix)
{
}

// @0x4B5BE0
void KVFilterEncoderClass::Add_Child(int mask, const char *name, DeltaInterfaceClass *child)
{
	const char *key = Make_Key(name);
	for (std::vector<std::string>::iterator i = Excludes.begin(); i != Excludes.end(); ++i) {
		if (strncmp(i->c_str(), key, i->length()) == 0) {
			return;
		}
	}
	KVEncoderClass::Add_Child(mask, name, child);
}

// @0x4B5C60
void KVFilterEncoderClass::Add_Exclude(const char *prefix)
{
	Excludes.push_back(std::string(prefix));
}

// @0x4B5D70
void KVFilterEncoderClass::Clear_Excludes()
{
	Excludes.clear();
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown; no assert, no __FILE__, no RTTI: the name is PROVISIONAL (D137), from its class
**	and the link order (after kv_encoder_base.cpp, before kv_time_adds.cpp).
**
**	Client 0x4B5A80-0x4B5E9F, an object of its own by the CRT initialiser table (PS, run 2026-11-18): entries 83-84
**	(permstr.h's static 0x4B3800, <string>'s initialiser 0x4B5A50) are kv_encoder_base.cpp's; entries 85-86 open and
**	close this one: permstr.h's PermStringInitClass static 0x4B5A80 (atexit 0x4B5AB0, the static 0xBE3B54) and
**	<string>'s guarded initialiser 0x4B5E80. Between them KVFilterEncoderClass's constructor 0x4B5AD0, its deleting
**	destructor 0x4B5B20 and destructor 0x4B5B50 (compiler-generated, vtable 0xAEB6B8), Add_Child 0x4B5BE0, Add_Exclude
**	0x4B5C60 and Clear_Excludes 0x4B5D70, then the string helpers this object calls out of line (_Refcnt 0x4B5E40,
**	deallocate 0x4B5E60). The class's five typed adds (its vtable's slots) stay in kv_encoder.cpp's object.
*/
