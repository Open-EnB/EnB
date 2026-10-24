/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\auxdata\delta_string.h (name assumed from
**	delta_string.cpp; no assert names it)
**
**	A leaf field holding a heap string (strdup/free). Not a DeltaTemplateClass: its vtable 0xAEB0E4
**	overrides slots 0, 4, 0x2C and 0x30 of DeltaClass's with delta_string.cpp's code. The client
**	doesn't name the classes (D24).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __DELTA_STRING_H
#define __DELTA_STRING_H

#include "delta.h"

// The string value interface kv_encoder_base.cpp's 0x4B48A0 takes (vtable 0xAEB150: the destructor
// and three pure slots); probably a kv header's, kept here until that code is rebuilt.
class StringValueClass
{
public:
	virtual ~StringValueClass() {}									// +0x00 0x4A7F00
	virtual const char *Get() const = 0;							// +0x04
	virtual void Set(const char *value) = 0;						// +0x08
	virtual StringValueClass *Clone() const = 0;					// +0x0C
};

// vtable 0xAEB0E4, size 0x88
class DeltaStringClass : public DeltaClass
{
public:
	DeltaStringClass(const char *name, const char *string);					// 0x4A78F0
	virtual ~DeltaStringClass();														// +0x04 0x4A79E0
	virtual void Unknown_00(DeltaChildVisitorClass *visitor);					// +0x00 0x4A7E60
	virtual bool Read(LinearBufferClass &buf, const DurationClass &base);	// +0x2C 0x4A7CA0
	virtual bool Write(LinearBufferClass &buf);									// +0x30 0x4A7BB0

	void Assign(const char *s);														// 0x4A7A10
	void Set(const char *s);															// 0x4A7AA0 (Assign if it differs)
	DeltaStringClass &operator=(const DeltaStringClass &that);				// 0x4A7DC0
	const char *Get() const { return String; }

	char *String;						// +0x84 (the asserts' name)

	static int UnknownGlobal;		// the per-class global at +0x04 (0xBE3A9C)
};

#endif //__DELTA_STRING_H
