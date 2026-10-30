/*
**	Earth & Beyond client (reconstruction)
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\split_string.h (name assumed from
**	split_string.cpp, whose __FILE__ is 'e:\build\beta\G\app\shared\split_string.cpp')
**
**	A text split into lines no wider than a width (0x28 bytes, vtable 0xAECB9C): breaks at the last
**	space before the width, at '\n', "\r\n" and (optionally) a written "\n"; the widths come from two
**	virtuals a font-measured subclass overrides (unknown_font_split_string.h). The client's assert
**	names buff; the rest are D10 / D24 names.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __SPLIT_STRING_H  // NAME 3: header name assumed from the client's split_string.cpp
#define __SPLIT_STRING_H

#include <string>
#include <vector>

class SplitStringClass  // NAME 3: D24, from split_string.cpp
{
public:
	SplitStringClass(const char *text = NULL, float width = 0.0f, int cursor = 0);	// 0x4F9BF0
	virtual ~SplitStringClass();												// 0x4F9CC0 (deleting 0x4F9C90)

	bool Split(const char *text, float width, int cursor);						// 0x4F9D90: false when one word is wider than width  // NAME 2: what it does
	unsigned int Count();														// 0x4FA710: the number of lines  // NAME 2: what it returns
	const char *Get(unsigned int index);										// 0x4FA740: one line, NULL past the end  // NAME 2: what it returns

	virtual float Get_Text_Width(const char *buff);								// slot +0x04, 0x4FA790: strlen  // NAME 2: what Split measures a line with
	virtual float Get_Char_Width(char c);										// slot +0x08, 0x4FA800: 1 or 0  // NAME 2: what Split measures a character with

protected:
	std::vector<std::string> Lines;												// +0x04  // NAME 2: what it holds
	float Width;																// +0x14: the widest line, -1 when Split fails  // NAME 2: what it holds
	int CursorLine;																// +0x18: the line of Split's cursor offset  // NAME 2: what it holds
	int CursorColumn;															// +0x1C: its column  // NAME 2: what it holds
	int NewLines;																// +0x20: the line breaks in the text  // NAME 2: what it counts
	bool EscapedNewLines;														// +0x24: a written "\n" breaks too  // NAME 2: what it does
	bool TrimLast;																// +0x25: the last line loses its trailing spaces  // NAME 2: what it does
	bool TrimLines;																// +0x26: the other lines lose theirs  // NAME 2: what it does
};

#endif // __SPLIT_STRING_H
