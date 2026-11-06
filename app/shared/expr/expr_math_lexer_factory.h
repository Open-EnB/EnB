// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\expr\expr_math_lexer_factory.h (name assumed from
**	expr_math_lexer_factory.cpp, whose __FILE__ is 'e:\build\beta\G\app\shared\expr\expr_math_lexer_factory.cpp')
**
**	Builds an expression lexer with the operator tokens the flags select (callers 0x4C7BE0, 0x4CBB30).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __EXPR_MATH_LEXER_FACTORY_H	// NAME 3 [expr_math_lexer_factory.h]: header name assumed from the client's expr_math_lexer_factory.cpp
#define __EXPR_MATH_LEXER_FACTORY_H

class ExprMathLexerClass;
class ExprLexerImplClass;

class ExprMathLexerFactoryClass	// NAME 3 [ExprMathLexerFactoryClass]: class-from-file (D24)
{
public:
	// The flags as ExprMathParserFactoryClass::Create's; number_flags 2 allows a decimal point, name_flags 1
	// forbids dotted names.
	static ExprMathLexerClass *Create(unsigned int flags, int number_flags, int name_flags);	// 0x4D0360  // NAME 3 [Create]

protected:
	static void Add_Operators(unsigned int flags, ExprLexerImplClass *lexer);					// 0x4D0230  // NAME 3 [Add_Operators]: 'lexer' from the assert
};

#endif
