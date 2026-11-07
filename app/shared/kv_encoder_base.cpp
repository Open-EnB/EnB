// Earth & Beyond client (reconstruction): app\shared\kv_encoder_base.cpp. Notes at the end of the file.
#include "kv_encoder_base.h"
#include "kv_list.h"
#include "kv_acc_string.h"
#include "g_assert.h"

// the prefixes joined, each followed by a dot (0x4B43E0; Push_Prefix and Set_Prefix inline it)
static void Unknown_Join(std::list<std::string> &list, std::string &out)
{
	out = "";
	std::list<std::string>::iterator i;	// declared before the loop: the constructor 0x4B3850 then keeps this in ebx as the client
	for (i = list.begin(); i != list.end(); ++i) {
		out += *i + ".";
	}
}
























KVEncoderBaseClass::KVEncoderBaseClass(KVListClass *list, int flags, const char *prefix) :
	Data(0),
	List(list)
{
	Data = new KVEncoderDataClass;
	G_ASSERT(Data);
	Data->Flags = flags;
	Set_Prefix(prefix);
}



KVEncoderBaseClass::~KVEncoderBaseClass()
{
	G_ASSERT(Data);
	delete Data;
	Data = 0;
}


const char *KVEncoderBaseClass::Make_Key(const char *name)
{
	G_ASSERT(Data);
	Data->Unknown2C = Data->Unknown1C + name;
	return Data->Unknown2C.c_str();
}


int KVEncoderBaseClass::Get_Flags() const
{
	G_ASSERT(Data);
	return Data->Flags;
}


void KVEncoderBaseClass::Set_Flags(int flags)
{
	G_ASSERT(Data);
	Data->Flags = flags;
}


void KVEncoderBaseClass::Push_Flags()
{
	G_ASSERT(Data);
	Data->FlagList.push_back(Data->Flags);
}


void KVEncoderBaseClass::Pop_Flags()
{
	G_ASSERT(Data);
	G_ASSERT(Data->FlagList.size() > 0);
	Data->Flags = Data->FlagList.back();
	Data->FlagList.pop_back();
}


void KVEncoderBaseClass::Push_Prefix(const char *name)
{
	G_ASSERT(Data);
	Data->PrefixList.push_back(std::string(name));
	Unknown_Join(Data->PrefixList, Data->Unknown1C);
}


void KVEncoderBaseClass::Pop_Prefix()
{
	G_ASSERT(Data);
	G_ASSERT(Data->PrefixList.size() > 0);
	Data->PrefixList.pop_back();
	Unknown_Join(Data->PrefixList, Data->Unknown1C);
}


void KVEncoderBaseClass::Set_Prefix(const char *prefix)
{
	G_ASSERT(Data);
	Data->PrefixList.clear();
	if (prefix == 0 || *prefix == 0) {
		Data->Unknown1C = "";
	} else {
		Data->PrefixList.push_back(prefix);
		Unknown_Join(Data->PrefixList, Data->Unknown1C);
	}
}


void KVEncoderBaseClass::Unknown_Test(int mask, bool &even, bool &odd)
{
	G_ASSERT(Data);
	even = (Data->Flags & mask & 0x155) != 0;
	odd = (Data->Flags & mask & 0x2AA) != 0;
}

void KVEncoderBaseClass::Add_String(int mask, const char *name, StringValueClass *value)
{
	bool even;
	bool odd;
	Unknown_Test(mask, even, odd);
	if (!even && !odd) {
		delete value;
		return;
	}
	List->Add(new KVPairStringClass(Make_Key(name), value));
}

void KVEncoderBaseClass::Add(int mask, const char *name, char *buffer, int size)
{
	bool even;
	bool odd;
	Unknown_Test(mask, even, odd);
	if (even || odd) {
		List->Add(new KVPairStringClass(Make_Key(name), new KVBufferAccessorClass(buffer, size), even, odd));
	}
}

void KVStringEncoderClass::Add_New_String(int mask, const char *name, char **value)
{
	bool even;
	bool odd;
	Unknown_Test(mask, even, odd);
	if (even || odd) {
		List->Add(new KVPairStringClass(Make_Key(name), new KVNewStringAccessorClass(value), even, odd));
	}
}

void KVStringEncoderClass::Add_Dup_String(int mask, const char *name, char **value)
{
	bool even;
	bool odd;
	Unknown_Test(mask, even, odd);
	if (even || odd) {
		List->Add(new KVPairStringClass(Make_Key(name), new KVDupStringAccessorClass(value), even, odd));
	}
}

void KVStringEncoderClass::Add_Fixed_String(int mask, const char *name, const char *value)
{
	bool even;
	bool odd;
	Unknown_Test(mask, even, odd);
	if (even || odd) {
		odd = false;
		List->Add(new KVPairStringClass(Make_Key(name), new KVFixedStringAccessorClass(value), even, odd));
	}
}

void KVStringEncoderClass::Add_Std_String(int mask, const char *name, std::string *value)
{
	bool even;
	bool odd;
	Unknown_Test(mask, even, odd);
	if (even || odd) {
		List->Add(new KVPairStringClass(Make_Key(name), new KVStdStringAccessorClass(value), even, odd));
	}
}

/*
**	Reconstruction notes
**
**	client.exe 0x4B3850-0x4B4A0B: KVEncoderBaseClass (D24, after the file), vtable 0xAEB62C (the
**	destructor only), size 0x0C; its state KVEncoderDataClass (the asserts' Data, 0x3C bytes: the
**	two lists the asserts name, Flags, two strings). Rebuilt: the destructor 0x4B3B90 (line 54; the
**	deleting one 0x4B3B10 and KVEncoderDataClass's 0x4B3C00 are the compiler's), Make_Key 0x4B3D60
**	(62), Get_Flags 0x4B3F30 (70), Set_Flags 0x4B3F70 (77), Push_Flags 0x4B3FC0 (84), Pop_Flags
**	0x4B4050 (91, 92), Push_Prefix 0x4B40F0 (100, Unknown_Join inlined), Pop_Prefix 0x4B4300 (108,
**	109), Unknown_Join 0x4B43E0 (static: called directly, without a thunk), Set_Prefix 0x4B45B0
**	(117), Unknown_Test 0x4B4820 (130), Add_String 0x4B48A0 (Unknown_Test and Make_Key inlined).
**	The constructor 0x4B3850 (line 45, Set_Prefix inlined: line 117) is not exact: the same code
**	with `this` in ebx where ours has ebp, and the new's result through eax. The other adds 0x4B4A70-0x4B5370
**	make the accessors of kv_acc_string.h (vtables 0xAEB338-0xAEB388; their copies at 0x4B0A90-0x4B1110).
*/
