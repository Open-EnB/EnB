// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\expr\expr_math_parser_factory.h (name assumed from
**	expr_math_parser_factory.cpp, whose __FILE__ is 'e:\build\beta\G\app\shared\expr\expr_math_parser_factory.cpp')
**
**	Builds an expression parser with the operator groups the flags select (callers 0x4C7BE0, 0x4CBB30).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __EXPR_MATH_PARSER_FACTORY_H	// NAME 3 [expr_math_parser_factory.h]: header name assumed from the client's expr_math_parser_factory.cpp
#define __EXPR_MATH_PARSER_FACTORY_H

class ExprMathParserClass;
class ExprMathParserImplClass;

class ExprMathParserFactoryClass	// NAME 3 [ExprMathParserFactoryClass]: class-from-file (D24)
{
public:
	// The flags: 1 + - * / % div (and the prefix + -), 2 comparisons, 4 | ^ & << >> (and ~),
	// 8 || && (and !), 0x10 min max (and the prefix abs sqrt).
	static ExprMathParserClass *Create(unsigned int flags);								// 0x4D0790  // NAME 3 [Create]

protected:
	static void Add_Operators(unsigned int flags, ExprMathParserImplClass *parser);		// 0x4D0560  // NAME 3 [Add_Operators]: 'parser' from the assert
};

#endif
