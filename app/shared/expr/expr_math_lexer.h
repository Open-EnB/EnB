// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Interim placement (D137): the token, the lexer interface (vtable 0xAEBB00) and the interface of the
**	lexer's matchers (vtable 0xAEBB30). The client names no header for them; the lexer's files are
**	expr_lexer_impl.cpp (ExprLexerImplClass), expr_lexer_match_list.cpp and expr_math_lexer_factory.cpp.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __EXPR_MATH_LEXER_H	// NAME 3 [expr_math_lexer.h]: interim header, named after expr_math_lexer_factory.cpp
#define __EXPR_MATH_LEXER_H

#include <string>

class ExprMathTokenClass	// NAME 3 [ExprMathTokenClass]: 0x14 bytes, a type and its text
{
public:
	ExprMathTokenClass();										// 0x4C7620: type -1, the text from a global
	ExprMathTokenClass(int type, const char *text);				// 0x4C7740
	ExprMathTokenClass(int type, const std::string &text);		// 0x4C7850

	bool Is_Valid() const;		// 0x4C79E0  // NAME 3 [Is_Valid]: type != -1 and the text differs from a global

	int Type;					// NAME 3 [Type]: 0 white space (skipped by the parser), 1 names, 2 numbers, 3 parentheses, 4 operators
	std::string Text;			// NAME 3 [Text]
};

// The lexer's interface: 4 bytes, vtable 0xAEBB00 (destructor 0x4C4AE0, six pure slots).
class ExprMathLexerClass	// NAME 3 [ExprMathLexerClass]
{
public:
	virtual ~ExprMathLexerClass() {}
	virtual ExprMathTokenClass Get_Token() = 0;	// +4  // NAME 3 [Get_Token]
	virtual bool Has_Token() = 0;					// +8  // NAME 3 [Has_Token]: the current token is valid
	virtual bool Has_Error() = 0;					// +0xC  // NAME 3 [Has_Error]: no valid token, text left
	virtual void Next() = 0;						// +0x10  // NAME 3 [Next]
	virtual void Set_Source(const char *source) = 0;	// +0x14  // NAME 3 [Set_Source]: 'source' from the assert
	virtual void Reset() = 0;						// +0x18  // NAME 3 [Reset]: back to the source's start
};

// A matcher: how many characters at text form one token of its kind (0: none). 4 bytes, vtable 0xAEBB30
// (destructor 0x4C5730, Match pure).
class ExprLexerMatchClass	// NAME 3 [ExprLexerMatchClass]: from expr_lexer_match_list.cpp
{
public:
	virtual ~ExprLexerMatchClass() {}
	virtual int Match(const char *text) = 0;		// +4  // NAME 3 [Match]
};

#endif
