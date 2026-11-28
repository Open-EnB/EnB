// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	The expression parser's interface (vtable 0xAEC0F0: the destructor 0x4D0A40 and Parse, whose
**	default returns NULL). Header name assumed from expr_math_parser_impl.cpp and
**	expr_math_parser_factory.cpp.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __EXPR_MATH_PARSER_H	// NAME 3 [expr_math_parser.h]: header name assumed from expr_math_parser_impl.cpp
#define __EXPR_MATH_PARSER_H

#include <stddef.h>

class ExprLexer;
class ExprMathTreeNode;

class ExprMathParser  // NAME 5: thunk order A, binary.md 5.11; NAME 3 [ExprMathParser]: base of ExprMathParserImplClass
{
public:
	virtual ~ExprMathParser() {}											// 0x4D0A20, deleting 0x4D0A40
	virtual ExprMathTreeNode *Parse(ExprLexer *lex) { return NULL; }	// NAME 3 [Parse]: lex from the assert '!lex'
};

#endif
