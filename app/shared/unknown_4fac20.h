// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown, in e:\build\beta\G\app\shared (two objects between
**	unknown_font_split_string.cpp's and symbol_substitution.cpp's, each closed by its own <string>
**	static initialiser: 0x4FAC20-0x4FB800 (unknown_4fac20.cpp: trim, commas, case, join) and
**	0x4FB810-0x4FCBC0 (unknown_4fb810.cpp: the tokenizer, split). No string names either file or a
**	header: unknown_4fac20.h is a neutral name, D10, and one header for both is an assumption.
**
**	String utilities, all free functions over std::string and character ranges: trimming, the
**	thousands separators of a number, lower / upper case, joining and splitting lists of strings,
**	and a small tokenizer (words and "quoted strings"). No client string names any of them: every
**	name here says what the function does (NAME 2).
*/


#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __UNKNOWN_4FAC20_H  // NAME 1: neutral header name (D10)
#define __UNKNOWN_4FAC20_H

#include <string>
#include <vector>

// Trimming: the text without its leading and trailing white space (isspace), or without the leading
// and trailing characters found in characters.
std::string String_Trim(const char *text);												// 0x4FAC20  // NAME 2 [String_Trim]: what it does
std::string String_Trim(const char *text, const char *characters);						// 0x4FAD80  // NAME 2 [String_Trim]: what it does

// Puts a ',' between every three digits, counted from the right ("29597354" -> "29,597,354").
// False, and number untouched, when it is empty or holds anything but digits.
bool String_Insert_Commas(std::string &number);											// 0x4FAEE0  // NAME 2: what it does (money.cpp's 0x4F00C0 calls it)

// Case: a converted copy, or the string converted in place (tolower / toupper per character).
std::string String_To_Lower(const std::string &string);									// 0x4FB150  // NAME 2 [String_To_Lower]: what it does
void String_Make_Lower(std::string &string);											// 0x4FB290  // NAME 2: what it does
std::string String_To_Upper(const std::string &string);									// 0x4FB380  // NAME 2 [String_To_Upper]: what it does
void String_Make_Upper(std::string &string);											// 0x4FB4C0  // NAME 2: what it does

// The strings of list in one string, separator between each two ("a, b, c").
std::string String_Join(const std::vector<std::string> &list, const std::string &separator);	// 0x4FB5B0  // NAME 2 [String_Join]: what it does

// The tokenizer, over the characters [first, last). A token is a word (a run without white space) or,
// when the first character after the white space is '"', everything between two '"'. token (may be
// NULL) receives it; next (may be NULL) receives where to go on: after the token and the white space
// that follows it. False when no token is left (or a quote is not closed).
bool String_Get_Token(const char *first, const char *last, std::string *token, const char **next);		// 0x4FB810  // NAME 2: what it does
int String_Count_Tokens(const char *first, const char *last);											// 0x4FBA70  // NAME 2: what it does
// The next "quoted string" only: skips everything up to the first '"'. next receives the character
// after the closing '"'.
bool String_Get_Quoted(const char *first, const char *last, std::string *token, const char **next);		// 0x4FBAE0  // NAME 2: what it does

// strtok over a copy of text: every piece between delimiters is appended to list. With count, at most
// count pieces: the last one is the rest of the text, delimiters included (count 0 appends nothing,
// count 1 appends the whole text).
void String_Tokenize(const char *text, const char *delimiters, std::vector<std::string> &list);					// 0x4FBD50  // NAME 2: what it does
void String_Tokenize(const char *text, const char *delimiters, unsigned int count, std::vector<std::string> &list);	// 0x4FBEC0  // NAME 2: what it does

// The first character of [first, last) that is not white space / that is white space (last if none).
const char *String_Skip_Whitespace(const char *first, const char *last);				// 0x4FC270  // NAME 2: what it does
const char *String_Find_Whitespace(const char *first, const char *last);				// 0x4FC2B0  // NAME 2: what it does

// Splits string at every separator into list (cleared first). Unlike strtok it keeps empty pieces:
// two separators in a row give an empty string. An empty string gives an empty list.
void String_Split(std::vector<std::string> &list, const std::string &string, const std::string &separator);	// 0x4FC2F0  // NAME 2: what it does (12 callers)

#endif
