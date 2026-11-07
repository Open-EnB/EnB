// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (name PROVISIONAL, D137, as kv_acc_array.h): the string accessors
**	KVEncoderBaseClass's adds make (kv_encoder_base.cpp's 0x4B4A70-0x4B5370). Their copies come first in the
**	client's run 0x4B0A90-0x4B1110, kv_acc.cpp's object; they derive from kv_pair_string.h's StringValueClass.
*/
#ifndef __KV_ACC_STRING_H	// NAME 3 [kv_acc_string.h]: PROVISIONAL, as kv_acc_array.h
#define __KV_ACC_STRING_H

#include <string>
#include <string.h>
#include <stdlib.h>
#include "kv_pair_string.h"
#include "kv_encoder_base.h"

char *nstrdup(const char *str);

// a fixed text of its own (vtable 0xAEB338, 8 bytes): Set does nothing
class KVFixedStringAccessorClass : public StringValueClass	// NAME 2: what it does
{
public:
	KVFixedStringAccessorClass(const char *value);					// 0x4B0A90
	virtual ~KVFixedStringAccessorClass() { delete [] Value; Value = NULL; }	// 0x4B0B50
	virtual const char *Get() const { return Value; }				// 0x4B0B90
	virtual void Set(const char *) {}									// 0x4B0BB0
	virtual StringValueClass *Clone() const { return new KVFixedStringAccessorClass(Value); }	// 0x4B0BD0

	char *Value;	// +0x04
};

// a caller's buffer (vtable 0xAEB34C, 12 bytes)
class KVBufferAccessorClass : public StringValueClass	// NAME 2: what it does
{
public:
	KVBufferAccessorClass(char *buffer, int size) : Buffer(buffer), Size(size) {}
	virtual const char *Get() const { return Buffer; }				// 0x4B0D10
	virtual void Set(const char *value) { strncpy(Buffer, value ? value : "", Size); }	// 0x4B0C90
	virtual StringValueClass *Clone() const { return new KVBufferAccessorClass(Buffer, Size); }	// 0x4B0CD0

	char *Buffer;	// +0x04
	int Size;		// +0x08
};

// a caller's pointer to text from new (vtable 0xAEB360, 8 bytes)
class KVNewStringAccessorClass : public StringValueClass	// NAME 2: what it does
{
public:
	KVNewStringAccessorClass(char **value) : Value(value) {}
	virtual const char *Get() const { return *Value; }				// 0x4B0E40
	virtual void Set(const char *value)									// 0x4B0D80
	{
		delete [] *Value;
		if (value == NULL) value = "";
		int size = strlen(value) + 1;
		*Value = new char[size];
		memcpy(*Value, value, size);
	}
	virtual StringValueClass *Clone() const { return new KVNewStringAccessorClass(Value); }	// 0x4B0E00

	char **Value;	// +0x04
};

// a caller's pointer to text from strdup (vtable 0xAEB374, 8 bytes)
class KVDupStringAccessorClass : public StringValueClass	// NAME 2: what it does
{
public:
	KVDupStringAccessorClass(char **value) : Value(value) {}
	virtual const char *Get() const { return *Value; }				// 0x4B0F30
	virtual void Set(const char *value)									// 0x4B0EB0
	{
		free(*Value);
		*Value = strdup(value ? value : "");
	}
	virtual StringValueClass *Clone() const { return new KVDupStringAccessorClass(Value); }	// 0x4B0EF0

	char **Value;	// +0x04
};

// a caller's std::string (vtable 0xAEB388, 8 bytes)
class KVStdStringAccessorClass : public StringValueClass	// NAME 2: what it does
{
public:
	KVStdStringAccessorClass(std::string *value) : Value(value) {}
	virtual const char *Get() const { return Value->c_str(); }		// 0x4B0FA0
	virtual void Set(const char *value) { *Value = value; }			// 0x4B0FC0
	virtual StringValueClass *Clone() const { return new KVStdStringAccessorClass(Value); }	// 0x4B10A0

	std::string *Value;	// +0x04
};

// KVEncoderBaseClass's string adds, kv_encoder_base.cpp's 0x4B4C50-0x4B51F0 (0x4B4A70 is the base's Add). Their declarations
// stay out of kv_encoder_base.h for aux_roots.cpp's symbol pad (no pad is exact for all six with them there, workflow.md 3.3):
// interim, D137; the same object layout, `this` the encoder.
class KVStringEncoderClass : public KVEncoderBaseClass	// NAME 1: interim, what it adds (D10)
{
public:
	void Add_New_String(int mask, const char *name, char **value);		// 0x4B4C50 (text from new)  // NAME 2: what it does
	void Add_Dup_String(int mask, const char *name, char **value);		// 0x4B4E30 (text from strdup)  // NAME 2: what it does
	void Add_Fixed_String(int mask, const char *name, const char *value);	// 0x4B5010 (a copy, Set does nothing)  // NAME 2: what it does
	void Add_Std_String(int mask, const char *name, std::string *value);	// 0x4B51F0  // NAME 2: what it does
};

#endif //__KV_ACC_STRING_H
