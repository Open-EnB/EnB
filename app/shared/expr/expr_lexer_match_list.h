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

#include <hash_map.h>
#include "expr_math_lexer.h"

// The token tree's node: 0x1C bytes, a flag and the children by character (an SGI hash_map of 100 buckets).
struct ExprLexerMatchNodeStruct	// NAME 3 [ExprLexerMatchNodeStruct]
{
	ExprLexerMatchNodeStruct() :
		Terminal(false)
	{
	}

	~ExprLexerMatchNodeStruct()		// 0x4C65D0
	{
		for (std::hash_map<char, ExprLexerMatchNodeStruct *>::iterator it = Children.begin(); it != Children.end(); ++it) {
			delete (*it).second;
		}
		Children.clear();
	}

	bool Terminal;														// +0  // NAME 3 [Terminal]: a listed token ends here
	std::hash_map<char, ExprLexerMatchNodeStruct *> Children;			// +4  // NAME 3 [Children]
};

// 8 bytes, vtable 0xAEBBD0.
class ExprLexerMatchListClass : public ExprLexerMatchClass	// NAME 3 [ExprLexerMatchListClass]: class-from-file (D24)
{
public:
	ExprLexerMatchListClass();									// 0x4C5930
	ExprLexerMatchListClass(const char *tokens);				// 0x4C5C20: 'tokens' from Add's assert
	virtual ~ExprLexerMatchListClass();							// 0x4C5EC0
	virtual int Match(const char *text);						// 0x4C6530

	void Add(const char *tokens);								// 0x4C6160  // NAME 3 [Add]: the tokens, separated by spaces
	void Add_Token(const char *token);							// 0x4C62C0  // NAME 3 [Add_Token]: 'token' from the assert

protected:
	ExprLexerMatchNodeStruct *Begin;							// +4  // NAME 3 [Begin]: from the constructors' asserts
};

#endif
