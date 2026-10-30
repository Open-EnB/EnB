// Earth & Beyond client (reconstruction): app\shared\kv_pair_string.cpp. Notes at the end of the file.
#include <string.h>
#include <stdlib.h>
#include "kv_pair_string.h"
#include "unknown_debug.h"











KVPairStringClass::KVPairStringClass(const char *key, StringValueClass *accessor, bool even, bool odd) :
	KVPairClass(key, even, odd),
	Accessor(accessor),
	Unknown10(NULL)
{
	G_ASSERT(key && accessor);
	const char *value = Accessor->Get();
	if (value == NULL) {
		value = "";
	}
	Unknown10 = strdup(value);
}

const char *KVPairStringClass::Get_Type_Name() const
{
	return "char *";
}
KVPairStringClass::~KVPairStringClass()
{
	delete Accessor;
	free(Unknown10);
}
// NOT EXACT (also Clone, which inlines it): the client passes the two flags through byte
// temporaries on the stack (stored, then pushed as dwords), as for a by-value argument
KVPairStringClass::KVPairStringClass(const KVPairStringClass &that) : KVPairClass(that.Key.Get_String(), that.Unknown08, that.Unknown09), Accessor(NULL), Unknown10(NULL)
{
	Accessor = that.Accessor->Clone();
	G_ASSERT(Accessor);
	Unknown10 = strdup(that.Unknown10);
}

KVPairClass *KVPairStringClass::Clone() const
{
	return new KVPairStringClass(*this);
}

char *KVPairStringClass::Get_Value_String(char *buf, int size) const
{
	const char *value = Accessor->Get();
	if (value == NULL) {
		value = "";
	}
	strncpy(buf, value, size);
	return buf;
}

bool KVPairStringClass::Set_Value_String(const char *text)
{
	Accessor->Set(text);
	return true;
}

bool KVPairStringClass::Is_Changed() const
{
	const char *value = Accessor->Get();
	if (value == NULL) {
		value = "";
	}
	return strcmp(value, Unknown10) != 0;
}

void KVPairStringClass::Update_Saved()
{
	free(Unknown10);
	const char *value = Accessor->Get();
	if (value == NULL) {
		value = "";
	}
	Unknown10 = strdup(value);
}

/*
**	Reconstruction notes
**
**	client.exe 0x4B7FC0-0x4B84A0: KVPairStringClass (D24), vtable 0xAEB810, size 0x14: the string
**	field's key/value pair. The constructor 0x4B7FC0 (assert on the client's line 22, D25) keeps a
**	strdup copy of the value ("" for none); the copy constructor 0x4B8170 (line 44) clones the
**	accessor, and Clone 0x4B8250 inlines it. The slots (kv_pair.h): Get_Type_Name 0x4B8090 ("char *"),
**	Get_Value_String 0x4B8360 (strncpy), Set_Value_String 0x4B83A0, Is_Changed 0x4B83D0 (strcmp
**	with the copy), Update_Saved 0x4B8450 (a new copy); the destructor 0x4B80E0 deletes the accessor.
*/
