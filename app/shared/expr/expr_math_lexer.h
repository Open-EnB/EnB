// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Interim placement (D137): the token and the lexer interface the expression parser reads,
**	declared only as far as expr_math_parser_impl.cpp uses them. The lexer's own files
**	(expr_math_lexer_factory.cpp, 0x4D0230) are not rebuilt yet.
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
	bool Is_Valid() const;		// 0x4C79E0  // NAME 3 [Is_Valid]: type != -1 and the text differs from a global

	int Type;					// NAME 3 [Type]: 0 skipped by the parser, 1 / 2 leaves, 4 operators
	std::string Text;			// NAME 3 [Text]
};

class ExprMathLexerClass	// NAME 3 [ExprMathLexerClass]: the parser calls slots +4 and +0x10
{
public:
	virtual ~ExprMathLexerClass() {}
	virtual ExprMathTokenClass Get_Token() = 0;	// +4  // NAME 3 [Get_Token]
	virtual void Unknown08() = 0;					// NAME 3 [Unknown08]
	virtual void Unknown0C() = 0;					// NAME 3 [Unknown0C]
	virtual void Next() = 0;						// +0x10  // NAME 3 [Next]
};

#endif
