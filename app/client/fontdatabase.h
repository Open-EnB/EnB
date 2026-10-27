/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\fontdatabase.h (name assumed from
**	fontdatabase.cpp, whose __FILE__ is 'e:\build\beta\G\app\Client\fontdatabase.cpp')
**
**	The font database: fontsys.ini's INIT-n sections (one per screen resolution, with the chat fonts'
**	ids, font files, scales and spacings), the font files the client knows (AllFonts) and the eight
**	fonts in use (Font[MAXFONTS]). The client's asserts name AllFonts, Font, font32Ptr, MAXFONTS,
**	Find_Font_File_Index and the locals info, chat_index, idx, fdb; the rest are D10 / D24 names.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __FONTDATABASE_H  // NAME 3 [fontdatabase.h]: header name assumed from the client's fontdatabase.cpp
#define __FONTDATABASE_H

#include <string>
#include <vector>
#include <hash_map.h>
#include "..\..\wwlib\wwstring.h"

#define MAXFONTS 8

class FontDatabaseClass;

struct FontInitStruct  // NAME 2: one INIT-n section of fontsys.ini (a screen resolution's fonts)
{
	bool Unknown_6C5C60(unsigned int idx, FontDatabaseClass *fdb);			// 0x6C5C60  // NAME 1 [Unknown_6C5C60]: neutral (D10)
	void Unknown_6C57B0(FontDatabaseClass *fdb);							// 0x6C57B0  // NAME 1 [Unknown_6C57B0]: neutral (D10)

	int ResWidth;															// +0x00, ResWidth  // NAME 2: the ini key
	int ResHeight;															// +0x04, ResHeight  // NAME 2: the ini key
	std::string Name;														// +0x08, "WxH:"  // NAME 2: what it holds
	StringClass FontID[MAXFONTS];											// +0x18, FontID%d  // NAME 2: the ini key
	StringClass Font[MAXFONTS];												// +0x38, Font%d  // NAME 2: the ini key
	float Scale[MAXFONTS];													// +0x58, Scale%d  // NAME 2: the ini key
	int Spacing[MAXFONTS];													// +0x78, Spacing%d  // NAME 2: the ini key
	int Unknown98[MAXFONTS];												// +0x98  // NAME 1 [Unknown98]: neutral (D10)
	StringClass DefaultFont[MAXFONTS];										// +0xB8  // NAME 2: what 0x6C3D40 restores Font from
	float DefaultScale[MAXFONTS];											// +0xD8  // NAME 2: what 0x6C3D40 restores Scale from
	int DefaultSpacing[MAXFONTS];											// +0xF8  // NAME 2: what 0x6C3D40 restores Spacing from
	int DefaultUnknown98[MAXFONTS];											// +0x118  // NAME 1 [DefaultUnknown98]: neutral (D10)
};

struct FontStruct  // NAME 2: one of the fonts in use (0xC bytes)
{
	void *font32Ptr;														// +0x00, the client's name (its assert)
	int Unknown04;															// +0x04  // NAME 1 [Unknown04]: neutral (D10)
	float Scale;															// +0x08  // NAME 2: Scale%d is copied in
};

class FontDatabaseClass  // NAME 3: by convention (D24) from the client's fontdatabase.cpp
{
public:
	int Find_Font_File_Index(const char *name);							// 0x6C3FB0, the client's name (its assert)
	int Get_Font_File_Index(const char *chat_name);							// 0x6C4070  // NAME 2: what it does
	const char *Get_Font_File_Name(unsigned int index);						// 0x6C4270  // NAME 2: what it does
	float Get_Scale(const char *chat_name);									// 0x6C42E0  // NAME 2: what it returns
	int Get_Spacing(const char *chat_name);									// 0x6C4450  // NAME 2: what it returns
	int Get_Unknown98(const char *chat_name);								// 0x6C45B0  // NAME 1 [Get_Unknown98]: neutral (D10)
	FontInitStruct *Get_Info();												// 0x6C4720  // NAME 2: the INIT section for the resolution
	void Unknown_6C49C0();													// 0x6C49C0  // NAME 1 [Unknown_6C49C0]: neutral (D10)

	std::vector<FontInitStruct *> Infos;									// +0x00  // NAME 2: the INIT sections, by resolution
	int Unknown10[18];														// +0x10: three SGI hash_maps keyed by std::string  // NAME 1 [Unknown10]: neutral (D10)
	std::vector<std::string> AllFonts;										// +0x58, the client's name (its assert)
	FontStruct Font[MAXFONTS];												// +0x68, the client's name (its assert)
};

#endif //__FONTDATABASE_H
