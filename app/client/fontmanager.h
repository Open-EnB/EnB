// Earth & Beyond client (reconstruction)
/*
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
#include "unknown_robj_hash.h"									// hash<RenderObjClass *>: the fonts are keyed by their render object
#include "..\shared\g_assert.h"
#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\wwlib\wwstring.h"
#include "fontdatabase.h"
#include <string.h>
#include "..\..\wwlib\point.h"
#include "..\..\ww3d2\dynamesh.h"

#include "fontsys.h"	// FontStyleColorStruct, Unknown_6D8B90Class, FontSys

class ManagedFontClass  // NAME 2 [ManagedFontClass]: what the manager holds per id (0x24 bytes; its +4 is the id)
{
public:
	ManagedFontClass(RenderObjClass *object);							// 0x6CE990: the render object's styles from the font database
	ManagedFontClass(const char *name, RenderObjClass *object);					// 0x6CF520 (no caller), inlined in 0x6D1FD0  // NAME 2 [object]: the render object Unknown_6CF930 gets
	void Unknown_6CF930(StringInfo *def, RenderObjClass *object);				// 0x6CF930: a 'W' template's lines as styles on the render object  // NAME 1 [Unknown_6CF930, object]: neutral (D10)
	~ManagedFontClass();												// 0x6CF730
	void Unknown_All_Styles(int index, float time);					// 0x6D0950, inlined in 0x6D1C10 (its own begin / ++ called out of line there)  // NAME 1 [Unknown_All_Styles]: neutral (D10)
	void Unknown_Set_Styles(int value);									// 0x6D0710, inlined in 0x6D28C0 (its own begin / ++ called out of line there)  // NAME 1 [Unknown_Set_Styles]: neutral (D10)
	void Unknown_6D07D0(int unknown);									// 0x6D07D0 (no caller): each style's 0x6CE7D0  // NAME 1 [Unknown_6D07D0]: neutral (D10)
	void Unknown_6D0890();												// 0x6D0890 (no caller): each style's 0x6D5D40  // NAME 1 [Unknown_6D0890]: neutral (D10)
	void Unknown_6D10B0(const char *name, Vector3 color);			// 0x6D10B0 (no caller): the color of every style named name  // NAME 1 [Unknown_6D10B0]: neutral (D10)
	void Unknown_6D1220(FontSys *style);							// 0x6D1220: adds a style under its +0xF8 name, else its name  // NAME 1 [Unknown_6D1220]: neutral (D10)
	FontSys *Unknown_6D1480(std::string name);					// 0x6D1480: the style named name + "_0"  // NAME 1 [Unknown_6D1480]: neutral (D10)
	FontSys *Unknown_6D1600(std::string name);					// 0x6D1600: the named style, else 0x6D1480's  // NAME 1 [Unknown_6D1600]: neutral (D10)
	void Unknown_6D1010(const char *name, Vector3 color);			// 0x6D1010: the named style's color (+0x494, +0x4A4)  // NAME 1 [Unknown_6D1010]: neutral (D10)
	void Unknown_6D01C0(int level, const char *name);					// 0x6D01C0: Set_Sort_Level of every style (name NULL) or the styles named name  // NAME 1 [Unknown_6D01C0]: neutral (D10)
	void Unknown_6D0390(const char *name);								// 0x6D0390: shows every style (name NULL) or the styles named name (Set_Hidden(false))  // NAME 1 [Unknown_6D0390]: neutral (D10)
	void Unknown_6D0550(const char *name);								// 0x6D0550: hides them (Set_Hidden(true))  // NAME 1 [Unknown_6D0550]: neutral (D10)
	void Unknown_6D0A70(const char *name, const char *text);	// 0x6D0A70: reloads the styles named name with the text  // NAME 1 [Unknown_6D0A70]: neutral (D10)
	void Unknown_6D0A20(const char *name, const char *text, int unknown);	// 0x6D0A20: the named style's Change_Text  // NAME 1 [Unknown_6D0A20]: neutral (D10)

	StringClass Name;													// +0x00  // NAME 2: the name 0x6D1FD0 looks up in the font database
	RenderObjClass *Id;												// +0x04: the manager's key, the render object (0x6CE990 stores it)  // NAME 2: what it holds
	int Unknown08;														// +0x08  // NAME 1: neutral (D10)
	std::hash_map<std::string, FontSys *> Styles;				// +0x0C: its styles by name (0x6D1C10 walks them)  // NAME 2: what it holds
};

class FontManagerClass  // NAME 3 [FontManagerClass]: by convention (D24) from the client's fontmanager.cpp
{
public:
	FontManagerClass();													// 0x6D16E0
	~FontManagerClass();												// 0x6D1920
	void Remove_All();													// 0x6D1AE0  // NAME 2: describes what it does
	void Unknown_6D1C10(int index, float time);					// 0x6D1C10: each font's 0x6CD950  // NAME 1 [Unknown_6D1C10]: neutral (D10)
	ManagedFontClass *Add(RenderObjClass *object);							// 0x6D1D80: a new font for the render object, under its address  // NAME 2: describes what it does
	ManagedFontClass *Add(const RefCountPtr<RenderObjClass> &object);		// 0x6D1FB0  // NAME 2: describes what it does
	ManagedFontClass *Create(const char *name, RenderObjClass *object);			// 0x6D1FD0: a font from the font database, added under its id  // NAME 2: describes what it does
	void Remove(RenderObjClass *object);										// 0x6D2220  // NAME 2: describes what it does
	void Unknown_6D2310(const RefCountPtr<RenderObjClass> &object);						// 0x6D2310: Remove through a reference (a screen's member)  // NAME 1 [Unknown_6D2310]: neutral (D10)
	void Unknown_6D2400(RenderObjClass *object);								// 0x6D2400  // NAME 1 [Unknown_6D2400]: neutral (D10)
	void Unknown_6D2480(RenderObjClass *object);								// 0x6D2480  // NAME 1 [Unknown_6D2480]: neutral (D10)
	void Unknown_6D2500(const char *name, const char *text, RenderObjClass *object, int unknown);	// 0x6D2500: the font's 0x6D0A20								// 0x6D2500  // NAME 1 [Unknown_6D2500]: neutral (D10)
	void Unknown_6D2580(const char *name, Vector3 color, RenderObjClass *object);	// 0x6D2580: a font's style color  // NAME 1 [Unknown_6D2580]: neutral (D10)
	FontSys *Get_Style(const char *name, RenderObjClass *object);		// 0x6D25D0: a font's named style, else the debug font  // NAME 2: describes what it does
	RefCountPtr<FontSys> Unknown_6D2700(const char *name, const RefCountPtr<RenderObjClass> &object);	// 0x6D2700: a font's named style, else the debug font  // NAME 1 [Unknown_6D2700]: neutral (D10)
	void Unknown_6D28C0(int value, RenderObjClass *object);					// 0x6D28C0: each style's +0x250 of a font  // NAME 1 [Unknown_6D28C0]: neutral (D10)
	void Add(ManagedFontClass *font);									// 0x6D29A0: under its id  // NAME 2: describes what it does
	ManagedFontClass *Peek_Font(RenderObjClass *object);						// inline only: Find's body where the client inlines it (the trio 0x6D2400, Get_Style, 0x6D28C0)  // NAME 1 [Peek_Font]: neutral (D10)
	ManagedFontClass *Find(RenderObjClass *object);							// 0x6D2A70  // NAME 2: describes what it does
	ManagedFontClass *Unknown_6D2AE0(const RefCountPtr<RenderObjClass> &object);			// 0x6D2AE0: Find through a reference (no caller in the client)  // NAME 1 [Unknown_6D2AE0]: neutral (D10)

	std::hash_map<RenderObjClass *, ManagedFontClass *> Fonts;				// +0x00: 100 buckets  // NAME 2: what it holds
	RefCountPtr<FontSys> DebugFont;								// +0x18: "DebugFont"  // NAME 2: what it holds
};

#endif //__FONTMANAGER_H
