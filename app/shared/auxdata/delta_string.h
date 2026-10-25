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

#ifndef __DELTA_STRING_H  // NAME 3 [delta_string.h]: header name assumed from the client's delta_string.cpp
#define __DELTA_STRING_H

#include "delta.h"
#include "..\kv_pair_string.h"

// vtable 0xAEB0E4, size 0x88
class DeltaStringClass : public DeltaClass  // NAME 3: by convention (D24) from the client's file delta_string.cpp
{
public:
	DeltaStringClass(const char *name, const char *string);					// 0x4A78F0
	virtual ~DeltaStringClass();														// +0x04 0x4A79E0
	virtual void Unknown_00(DeltaChildVisitorClass *visitor);					// +0x00 0x4A7E60
	virtual bool Read(LinearBufferClass &buf, const DurationClass &base);	// +0x2C 0x4A7CA0
	virtual bool Write(LinearBufferClass &buf);									// +0x30 0x4A7BB0

	void Assign(const char *s);														// 0x4A7A10  // NAME 2: describes what it does
	void Set(const char *s);															// 0x4A7AA0 (Assign if it differs)  // NAME 2: describes what it does
	DeltaStringClass &operator=(const DeltaStringClass &that);				// 0x4A7DC0
	const char *Get() const { return String; }  // NAME 2: describes what it does

	char *String;						// +0x84 (the asserts' name)

	static int UnknownGlobal;		// the per-class global at +0x04 (0xBE3A9C)
};

#endif //__DELTA_STRING_H
