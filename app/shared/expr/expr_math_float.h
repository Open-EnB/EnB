// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\expr\expr_math_float.h (name assumed from
**	expr_math_float.cpp, whose __FILE__ is 'e:\build\beta\G\app\shared\expr\expr_math_float.cpp')
**
**	Float expressions: Expr_Math_Float builds an evaluation tree from a source with the operator
**	groups the flags select (as ExprMathParserFactoryClass::Create's), optionally listing the names
**	it uses. Only 's' is the client's name (the FAIL_IF '!s').
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __EXPR_MATH_FLOAT_H	// NAME 3 [expr_math_float.h]: header name assumed from the client's expr_math_float.cpp
#define __EXPR_MATH_FLOAT_H

#include <string>
#include <vector>
#include "expr_math_builder.h"

// The float operators: ^ and % of its own, then the groups the flags select. 8 bytes, vtable 0xAEBBE0.
class ExprMathFloatFactoryClass : public ExprMathEvalFactoryClass<float>	// NAME 3 [ExprMathFloatFactoryClass]
{
public:
	ExprMathFloatFactoryClass(unsigned int flags);																	// 0x4C7D50
	virtual PtrType Create_Binary(const std::string &name, const PtrType &left, const PtrType &right);			// 0x4C7D80
	virtual PtrType Create_Unary(const std::string &name, const PtrType &child);								// 0x4C8370
	virtual PtrType Create_Constant(const std::string &text);													// 0x4C8690
	virtual PtrType Create_Variable(const std::string &name);													// 0x4C8710

protected:
	unsigned int m_flags;	// NAME 3 [m_flags]
};

RefCountPtr<ExprMathEvalClass<float> > Expr_Math_Float(unsigned int flags, const char *s);								// 0x4C7BA0  // NAME 3 [Expr_Math_Float]
RefCountPtr<ExprMathEvalClass<float> > Expr_Math_Float(unsigned int flags, const char *s, std::vector<std::string> *names);	// 0x4C7BE0

#endif
