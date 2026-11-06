// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\common\stredit.h (name assumed from stredit.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\common\stredit.cpp'; the client holds no string of this header)
**
**	The text gadgets' edit buffer (0x20 bytes; constructor 0x84AA10, destructor 0x84AAB0): a string of MaxLen
**	characters with a cursor, the check character STRING_CHECK_VALUE ('~') after the terminator. Declared as
**	far as textgadg.cpp uses it (D10); stredit.cpp has its functions to 0x84B1B0 so far.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __STREDIT_H  // NAME 3 [stredit.h]: header name assumed from the client's stredit.cpp
#define __STREDIT_H

#include <string.h>

struct KernelEventClass;

// std::string, declared only: <string> would change the code of the files that include this one (stredit.cpp, textgadg.cpp)
namespace std { template<class _E> struct char_traits; template<class _Ty> class allocator; template<class _E, class _Tr, class _A> class basic_string; }

class StrEditClass  // NAME 3: class-from-file (D24) for app\common\stredit.cpp
{
public:
	StrEditClass(int maxlen, const char *string, unsigned int flags, const char *chars);	// 0x84AA10 (String gets maxlen + 2 bytes)  // NAME 3 [maxlen]: the asserts' MaxLen; NAME 1 [string, flags, chars]: neutral
	~StrEditClass();													// 0x84AAB0
	bool Process_Key(KernelEventClass *event);							// 0x84AAD0 (G_ASSERT 'String[MaxLen + 1] == STRING_CHECK_VALUE': true when the key was taken)  // NAME 2: what it does
	bool Insert(const char *text);										// 0x84B020 (G_ASSERT 'StrLen == (int)strlen(String)': at the cursor)  // NAME 2: what it does
	char *Get_String(int length = -1, char *buffer = NULL);				// 0x84B1B0 (-1: StrLen + 1; no buffer: a new one the caller deletes)  // NAME 2: what it does; NAME 1 [length, buffer]: neutral

	void Set_String(const std::basic_string<char, std::char_traits<char>, std::allocator<char> > &text);							// 0x84B200 (Clear, then Insert of the text)  // NAME 2: what it does; NAME 1 [text]: neutral
	void Insert_String(const std::basic_string<char, std::char_traits<char>, std::allocator<char> > &text);						// 0x84B270 (Insert with InsertMode on)  // NAME 2: what it does; NAME 1 [text]: neutral
	bool Set_Cursor(unsigned int position);								// 0x84B2B0 (true when it moved)  // NAME 2: what it does; NAME 1 [position]: neutral
	bool Move_Cursor(int delta);										// 0x84B2F0 (true when it moved)  // NAME 2: what it does; NAME 1 [delta]: neutral

	int Get_Max_Len() { return(MaxLen); }								// (inline: gd_text_editor.cpp 0x6730B0)  // NAME 1 [Get_Max_Len]: the member it returns
	bool Get_Insert_Mode() { return(InsertMode); }						// (inline: gd_text_editor.cpp 0x672B00)  // NAME 1 [Get_Insert_Mode]: the member it returns
	int Get_Length() { return(strlen(String)); }						// (inline: textgadg.cpp 0x6AA690 / 0x6AA6C0)  // NAME 2: what it does
	void Clear() { memset(String, 0, MaxLen); Cursor = String; StrLen = 0; }	// (inline: textgadg.cpp 0x6AA570)  // NAME 2: what it does

	char *String;														// +0x00  // NAME 5: the asserts'
	int Unknown04;														// +0x04
	int MaxLen;															// +0x08  // NAME 5: the asserts'
	int StrLen;															// +0x0C  // NAME 5: the asserts'
	const char *Chars;													// +0x10 (the constructor's fourth argument: the characters Process_Key allows, or with flag 0x20 refuses)  // NAME 2: what it holds
	unsigned int Flags;													// +0x14 (the constructor's third argument: 1 upper case, 2 lower, 4 digits, 8 letters, 0x10 both, 0x20 Chars refused, 0x40 enter, 0x200 space)  // NAME 1: neutral (D10)
	char *Cursor;														// +0x18 (into String)  // NAME 2: what it points at
	bool InsertMode;													// +0x1C (the constructor's true; textgadg.cpp hands it to the cursor)  // NAME 2: what it holds
};

#endif // __STREDIT_H
