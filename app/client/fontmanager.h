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
#include "..\..\wwlib\wwstring.h"
#include "fontdatabase.h"
#include <string.h>
#include "..\..\wwlib\point.h"

struct FontStringStruct;
class FontDatabaseClass;

struct FontStyleColorStruct  // NAME 1 [FontStyleColorStruct]: neutral (D10): 16 bytes, a color and one more word
{
	TPoint3D<int> Color;												// +0x00  // NAME 2: what 0x6D1010 stores
	int Unknown0C;														// +0x0C  // NAME 1: neutral (D10)
};

class FontStyleClass : public RefCountClass  // NAME 1 [FontStyleClass]: neutral (D10): the debug font's class (0x4E8 bytes, reference counted)
{
public:
	FontStyleClass(const char *name, int unknown1, int unknown2);		// 0x6D47C0  // NAME 1 [unknown1]: neutral (D10)
	void Set_Color(const TPoint3D<int> &color) { Unknown494.Color = color; Unknown4A4 = Unknown494; }	// inlined in 0x6D10B0  // NAME 2: describes what it does

	void Unknown_6CD950(int unknown1, int unknown2);						// 0x6CD950  // NAME 1 [Unknown_6CD950]: neutral (D10)

	char Unknown08[0xEC];												// +0x08  // NAME 1: neutral (D10)
	StringClass Name;													// +0xF4: what 0x6D10B0 compares  // NAME 2: what it holds
	StringClass Unknown0F8;												// +0xF8  // NAME 1: neutral (D10)
	char Unknown0FC[0x154];											// +0xFC  // NAME 1: neutral (D10)
	int Unknown250;														// +0x250: 0x6D28C0 sets it in every style of a font  // NAME 1: neutral (D10)
	char Unknown254[0x240];											// +0x254  // NAME 1: neutral (D10)
	FontStyleColorStruct Unknown494;									// +0x494: 0x6D1010's color  // NAME 1: neutral (D10)
	FontStyleColorStruct Unknown4A4;									// +0x4A4: a copy of +0x494 (0x6D1010)  // NAME 1: neutral (D10)
	char Unknown4B4[0x34];												// +0x4B4  // NAME 1: neutral (D10)
};

class ManagedFontClass  // NAME 2 [ManagedFontClass]: what the manager holds per id (0x24 bytes; its +4 is the id)
{
public:
	ManagedFontClass(const char *name);									// 0x6CE990
	ManagedFontClass(const char *name, int unknown)						// inlined in 0x6D1FD0  // NAME 1 [unknown]: neutral (D10)
	{
		FontStringStruct *def = Unknown_BE67B8->Find_String(name);
		Styles.clear();
		if (def) {
			Unknown_6CF930(def, unknown);
		}
	}
	void Unknown_6CF930(FontStringStruct *def, int unknown);				// 0x6CF930: loads the font from its database entry  // NAME 1 [Unknown_6CF930]: neutral (D10)
	~ManagedFontClass();												// 0x6CF730
	void Unknown_All_Styles(int unknown1, int unknown2)					// inlined in 0x6D1C10 (its own begin / ++ called out of line)  // NAME 1 [Unknown_All_Styles]: neutral (D10)
	{
		for (std::hash_map<std::string, FontStyleClass *>::iterator style = Styles.begin(); style != Styles.end(); ++style) {
			style->second->Unknown_6CD950(unknown1, unknown2);
		}
	}
	void Unknown_Set_Styles(int value)									// inlined in 0x6D28C0 (its own begin / ++ called out of line)  // NAME 1 [Unknown_Set_Styles]: neutral (D10)
	{
		for (std::hash_map<std::string, FontStyleClass *>::iterator style = Styles.begin(); style != Styles.end(); ++style) {
			style->second->Unknown250 = value;
		}
	}
	void Unknown_6D10B0(const char *name, TPoint3D<int> color);			// 0x6D10B0 (no caller): the color of every style named name  // NAME 1 [Unknown_6D10B0]: neutral (D10)
	FontStyleClass *Unknown_6D1600(std::string name);					// 0x6D1600: the named style  // NAME 1 [Unknown_6D1600]: neutral (D10)
	void Unknown_6D1010(const char *name, TPoint3D<int> color);			// 0x6D1010: the named style's color (+0x494, +0x4A4)  // NAME 1 [Unknown_6D1010]: neutral (D10)
	void Unknown_6D0390(void *unknown);									// 0x6D0390  // NAME 1 [Unknown_6D0390]: neutral (D10)
	void Unknown_6D0550(void *unknown);									// 0x6D0550  // NAME 1 [Unknown_6D0550]: neutral (D10)
	void Unknown_6D0A20(void *unknown);									// 0x6D0A20  // NAME 1 [Unknown_6D0A20]: neutral (D10)

	StringClass Name;													// +0x00  // NAME 2: the name 0x6D1FD0 looks up in the font database
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
	ManagedFontClass *Create(const char *name, int unknown);			// 0x6D1FD0: a font from the font database, added under its id  // NAME 2: describes what it does
	void Remove(unsigned int id);										// 0x6D2220  // NAME 2: describes what it does
	void Unknown_6D2310(const unsigned int &id);						// 0x6D2310: Remove through a reference (a screen's member)  // NAME 1 [Unknown_6D2310]: neutral (D10)
	void Unknown_6D2400(unsigned int id);								// 0x6D2400  // NAME 1 [Unknown_6D2400]: neutral (D10)
	void Unknown_6D2480(unsigned int id);								// 0x6D2480  // NAME 1 [Unknown_6D2480]: neutral (D10)
	void Unknown_6D2500(unsigned int id);								// 0x6D2500  // NAME 1 [Unknown_6D2500]: neutral (D10)
	void Unknown_6D2580(const char *name, TPoint3D<int> color, unsigned int id);	// 0x6D2580: a font's style color  // NAME 1 [Unknown_6D2580]: neutral (D10)
	FontStyleClass *Get_Style(const char *name, unsigned int id);		// 0x6D25D0: a font's named style, else the debug font  // NAME 2: describes what it does
	void Unknown_6D28C0(int value, unsigned int id);					// 0x6D28C0: each style's +0x250 of a font  // NAME 1 [Unknown_6D28C0]: neutral (D10)
	void Add(ManagedFontClass *font);									// 0x6D29A0: under its id  // NAME 2: describes what it does
	ManagedFontClass *Find(unsigned int id);							// 0x6D2A70  // NAME 2: describes what it does

	std::hash_map<unsigned int, ManagedFontClass *> Fonts;				// +0x00: 100 buckets  // NAME 2: what it holds
	RefCountPtr<FontStyleClass> DebugFont;								// +0x18: "DebugFont"  // NAME 2: what it holds
};

#endif //__FONTMANAGER_H
