// Earth & Beyond client (reconstruction): notes at the end of the file
#include "kv_acc_string.h"

// @0x4B0C90
void KVBufferAccessorClass::Set(const char *value)
{
	strncpy(Buffer, value ? value : "", Size);
}

// @0x4B0CD0
StringValueClass *KVBufferAccessorClass::Clone() const
{
	return new KVBufferAccessorClass(Buffer, Size);
}

// @0x4B0D80
void KVNewStringAccessorClass::Set(const char *value)
{
	delete [] *Value;
	if (value == NULL) value = "";
	int size = strlen(value) + 1;
	*Value = new char[size];
	memcpy(*Value, value, size);
}

// @0x4B0E00
StringValueClass *KVNewStringAccessorClass::Clone() const
{
	return new KVNewStringAccessorClass(Value);
}

// @0x4B0EB0
void KVDupStringAccessorClass::Set(const char *value)
{
	free(*Value);
	*Value = strdup(value ? value : "");
}

// @0x4B0EF0
StringValueClass *KVDupStringAccessorClass::Clone() const
{
	return new KVDupStringAccessorClass(Value);
}

// @0x4B0FA0
const char *KVStdStringAccessorClass::Get() const
{
	return Value->c_str();
}

// @0x4B0FC0
void KVStdStringAccessorClass::Set(const char *value)
{
	*Value = value;
}

// @0x4B10A0
StringValueClass *KVStdStringAccessorClass::Clone() const
{
	return new KVStdStringAccessorClass(Value);
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown; no assert, no __FILE__: the name is PROVISIONAL (D137), after kv_acc_string.h. The string
**	accessors' object, client 0x4B0C90-0x4B114F: the buffer, new, dup and std::string accessors' out-of-line members in
**	this order (each Clone followed by the inline members its vtable brings) and <string>'s guarded initialiser 0x4B1130
**	(CRT entry 80, the second of the two between delta_visitor.cpp's 78 and kv_encoder.cpp's 81); split from
**	kv_acc.cpp's object (DE, 2026-11-18). Their copies elsewhere (kv_encoder_base.cpp) were COMDATs before.
*/
