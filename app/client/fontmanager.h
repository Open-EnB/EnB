/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\fontmanager.h (name assumed from fontmanager.cpp,
**	whose __FILE__ is 'e:\build\beta\G\app\Client\fontmanager.cpp')
**
**	The font manager (0x1C bytes, created by 0x526DE0 into 0xBE67C4): the fonts by id (an SGI
**	hash_map of 100 buckets) and the debug font. Declared as far as the rebuilt functions need.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __FONTMANAGER_H  // NAME 3 [fontmanager.h]: header name assumed from the client's fontmanager.cpp
#define __FONTMANAGER_H

#include <string>
#include <hash_map.h>
#include "..\shared\unknown_debug.h"
#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"

class StringClass;

class FontStyleClass : public RefCountClass  // NAME 1 [FontStyleClass]: neutral (D10): the debug font's class (0x4E8 bytes, reference counted)
{
public:
	FontStyleClass(const char *name, int unknown1, int unknown2);		// 0x6D47C0  // NAME 1 [unknown1]: neutral (D10)

	void Unknown_6CD950(int unknown1, int unknown2);						// 0x6CD950  // NAME 1 [Unknown_6CD950]: neutral (D10)

	char Unknown08[0x4E0];											// +0x08  // NAME 1: neutral (D10)
};

class ManagedFontClass  // NAME 2 [ManagedFontClass]: what the manager holds per id (0x24 bytes; its +4 is the id)
{
public:
	ManagedFontClass(const char *name);									// 0x6CE990
	~ManagedFontClass();												// 0x6CF730
	void Unknown_All_Styles(int unknown1, int unknown2)					// inlined in 0x6D1C10 (its own begin / ++ called out of line)  // NAME 1 [Unknown_All_Styles]: neutral (D10)
	{
		for (std::hash_map<std::string, FontStyleClass *>::iterator style = Styles.begin(); style != Styles.end(); ++style) {
			style->second->Unknown_6CD950(unknown1, unknown2);
		}
	}
	void Unknown_6D0390(void *unknown);									// 0x6D0390  // NAME 1 [Unknown_6D0390]: neutral (D10)
	void Unknown_6D0550(void *unknown);									// 0x6D0550  // NAME 1 [Unknown_6D0550]: neutral (D10)
	void Unknown_6D0A20(void *unknown);									// 0x6D0A20  // NAME 1 [Unknown_6D0A20]: neutral (D10)

	char *Unknown00;													// +0x00 (a StringClass in the client)  // NAME 1: neutral (D10)
	unsigned int Id;													// +0x04: the manager's key  // NAME 2: what it holds
	int Unknown08;														// +0x08  // NAME 1: neutral (D10)
	std::hash_map<std::string, FontStyleClass *> Styles;				// +0x0C: its styles by name (0x6D1C10 walks them)  // NAME 2: what it holds
};

class FontManagerClass  // NAME 3 [FontManagerClass]: by convention (D24) from the client's fontmanager.cpp
{
public:
	FontManagerClass();													// 0x6D16E0
	~FontManagerClass();												// 0x6D1920
	void Remove_All();													// 0x6D1AE0  // NAME 2: describes what it does
	void Unknown_6D1C10(int unknown1, int unknown2);					// 0x6D1C10: each font's 0x6CD950  // NAME 1 [Unknown_6D1C10]: neutral (D10)
	void Add(const char *name);											// 0x6D1D80: a new font by name, under its id  // NAME 2: describes what it does
	void Add(const StringClass &name);									// 0x6D1FB0  // NAME 2: describes what it does
	void Remove(unsigned int id);										// 0x6D2220  // NAME 2: describes what it does
	void Unknown_6D2400(unsigned int id);								// 0x6D2400  // NAME 1 [Unknown_6D2400]: neutral (D10)
	void Unknown_6D2480(unsigned int id);								// 0x6D2480  // NAME 1 [Unknown_6D2480]: neutral (D10)
	void Unknown_6D2500(unsigned int id);								// 0x6D2500  // NAME 1 [Unknown_6D2500]: neutral (D10)
	void Add(ManagedFontClass *font);									// 0x6D29A0: under its id  // NAME 2: describes what it does
	ManagedFontClass *Find(unsigned int id);							// 0x6D2A70  // NAME 2: describes what it does

	std::hash_map<unsigned int, ManagedFontClass *> Fonts;				// +0x00: 100 buckets  // NAME 2: what it holds
	RefCountPtr<FontStyleClass> DebugFont;								// +0x18: "DebugFont"  // NAME 2: what it holds
};

#endif //__FONTMANAGER_H
