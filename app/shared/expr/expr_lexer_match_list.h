// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\expr\expr_lexer_match_list.h (name assumed from
**	expr_lexer_match_list.cpp, whose __FILE__ is 'e:\build\beta\G\app\shared\expr\expr_lexer_match_list.cpp')
**
**	A matcher for a list of literal tokens ("( )", "+ - * / % div"): the longest listed token at the text.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __EXPR_LEXER_MATCH_LIST_H	// NAME 3 [expr_lexer_match_list.h]: header name assumed from the client's expr_lexer_match_list.cpp
#define __EXPR_LEXER_MATCH_LIST_H

#include "expr_math_lexer.h"

struct ExprLexerMatchNodeStruct;	// NAME 3 [ExprLexerMatchNodeStruct]: the token tree's node (not rebuilt yet)

// 8 bytes, vtable 0xAEBBD0.
class ExprLexerMatchListClass : public ExprLexerMatchClass	// NAME 3 [ExprLexerMatchListClass]: class-from-file (D24)
{
public:
	ExprLexerMatchListClass();									// 0x4C5930
	ExprLexerMatchListClass(const char *tokens);				// 0x4C5C20: 'tokens' from Add's assert
	virtual ~ExprLexerMatchListClass();							// 0x4C5EC0
	virtual int Match(const char *text);						// 0x4C6530

	void Add(const char *tokens);								// 0x4C6160  // NAME 3 [Add]: the tokens, separated by spaces

protected:
	ExprLexerMatchNodeStruct *Begin;							// +4  // NAME 3 [Begin]: from the constructors' asserts
};

#endif
