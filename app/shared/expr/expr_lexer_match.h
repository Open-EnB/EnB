// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Interim placement (D137): the interface of the lexer's matchers, out of expr_math_lexer.h because the
**	client's expr_lexer_matchers.obj has no <string> initialiser (its header does not reach <string>).
*/
#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __EXPR_LEXER_MATCH_H	// NAME 3 [expr_lexer_match.h]: interim header, named after expr_lexer_match_list.cpp
#define __EXPR_LEXER_MATCH_H

// A matcher: how many characters at text form one token of its kind (0: none). 4 bytes, vtable 0xAEBB30
// (destructor 0x4C5730, Match pure).
class ExprLexerMatch  // NAME 5: thunk order, binary.md 5.11; NAME 3 [ExprLexerMatch]: from expr_lexer_match_list.cpp
{
public:
	virtual ~ExprLexerMatch() {}
	virtual int Match(const char *text) = 0;		// +4  // NAME 3 [Match]
};

#endif
