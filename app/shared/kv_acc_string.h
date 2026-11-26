// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (name PROVISIONAL, D137, as kv_acc_array.h): the string accessors
**	KVEncoderBaseClass's adds make (kv_encoder_base.cpp's 0x4B4A70-0x4B5370); they derive from kv_pair_string.h's
**	StringValueClass. The fixed string's constructor is kv_acc.cpp's (0x4B0A90); the other four's Set and Clone (and
**	the std::string one's Get, before its Set) are kv_acc_string.cpp's out-of-line members (0x4B0C90-0x4B1110: each
**	Clone's new brings its vtable's inline Get, deleting destructor and destructor right after it, in that order).
*/
#ifndef __KV_ACC_STRING_H	// NAME 3 [kv_acc_string.h]: PROVISIONAL, as kv_acc_array.h
#define __KV_ACC_STRING_H

#include <string>
#include <string.h>
#include <stdlib.h>
#include "string_value.h"

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
	virtual void Set(const char *value);									// 0x4B0C90
	virtual StringValueClass *Clone() const;							// 0x4B0CD0

	char *Buffer;	// +0x04
	int Size;		// +0x08
};

// a caller's pointer to text from new (vtable 0xAEB360, 8 bytes)
class KVNewStringAccessorClass : public StringValueClass	// NAME 2: what it does
{
public:
	KVNewStringAccessorClass(char **value) : Value(value) {}
	virtual const char *Get() const { return *Value; }				// 0x4B0E40
	virtual void Set(const char *value);									// 0x4B0D80
	virtual StringValueClass *Clone() const;							// 0x4B0E00

	char **Value;	// +0x04
};

// a caller's pointer to text from strdup (vtable 0xAEB374, 8 bytes)
class KVDupStringAccessorClass : public StringValueClass	// NAME 2: what it does
{
public:
	KVDupStringAccessorClass(char **value) : Value(value) {}
	virtual const char *Get() const { return *Value; }				// 0x4B0F30
	virtual void Set(const char *value);									// 0x4B0EB0
	virtual StringValueClass *Clone() const;							// 0x4B0EF0

	char **Value;	// +0x04
};

// a caller's std::string (vtable 0xAEB388, 8 bytes)
class KVStdStringAccessorClass : public StringValueClass	// NAME 2: what it does
{
public:
	KVStdStringAccessorClass(std::string *value) : Value(value) {}
	virtual const char *Get() const;									// 0x4B0FA0
	virtual void Set(const char *value);									// 0x4B0FC0
	virtual StringValueClass *Clone() const;							// 0x4B10A0

	std::string *Value;	// +0x04
};


#endif //__KV_ACC_STRING_H
