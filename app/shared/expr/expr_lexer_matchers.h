// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Interim placement (D137): the lexer's number, name and white-space matchers. Their object
**	(client 0x4C56B0-0x4C592F, between expr_lexer_impl.cpp and expr_lexer_match_list.cpp) has no
**	__FILE__ or assert, so the file name is unknown.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __EXPR_LEXER_MATCHERS_H	// NAME 3 [expr_lexer_matchers.h]: interim header, no client name
#define __EXPR_LEXER_MATCHERS_H

#include "expr_lexer_match.h"

// Digits; with flag 2 one decimal point among them. 8 bytes, vtable 0xAEBB24.
class ExprLexerMatchNumber : public ExprLexerMatch  // NAME 5: thunk order, binary.md 5.11; NAME 3 [ExprLexerMatchNumber]: lexer type 2
{
public:
	ExprLexerMatchNumber(int flags);						// 0x4C56B0: 0 counts as 1
	virtual int Match(const char *text);						// 0x4C5760

protected:
	int m_flags;												// +4  // NAME 3 [m_flags]
};

// A letter, then letters and digits; single dots (not "..") restart the name unless flag 1 is set.
// 8 bytes, vtable 0xAEBB3C.
class ExprLexerMatchName : public ExprLexerMatch  // NAME 5: thunk order, binary.md 5.11; NAME 3 [ExprLexerMatchName]: lexer type 1
{
public:
	ExprLexerMatchName(int flags);						// 0x4C57D0
	virtual int Match(const char *text);						// 0x4C5850

protected:
	int m_flags;												// +4  // NAME 3 [m_flags]
};

// White space. 4 bytes; its vtable (0xAEC0D4) and destructors are expr_math_lexer_factory.cpp's.
class ExprLexerMatchSpace : public ExprLexerMatch  // NAME 5: thunk order, binary.md 5.11; NAME 3 [ExprLexerMatchSpace]: lexer type 0
{
public:
	ExprLexerMatchSpace() {}
	virtual int Match(const char *text);						// 0x4C58F0
};

#endif
