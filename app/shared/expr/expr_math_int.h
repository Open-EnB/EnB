// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\expr\expr_math_int.h (name assumed from
**	expr_math_int.cpp, whose __FILE__ is 'e:\build\beta\G\app\shared\expr\expr_math_int.cpp')
**
**	Integer expressions: Expr_Math_Int builds an evaluation tree from a source with the operator
**	groups the flags select (as ExprMathParserFactoryClass::Create's), optionally listing the names
**	it uses. Only 's' is the client's name (the FAIL_IF '!s').
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __EXPR_MATH_INT_H	// NAME 3 [expr_math_int.h]: header name assumed from the client's expr_math_int.cpp
#define __EXPR_MATH_INT_H

#include <string>
#include <vector>
#include "expr_math_builder.h"

// The int operators: % of its own, then the groups the flags select. 8 bytes, vtable 0xAEBDE4.
class ExprMathIntFactoryClass : public ExprMathEvalFactoryClass<int>	// NAME 3 [ExprMathIntFactoryClass]
{
public:
	ExprMathIntFactoryClass(unsigned int flags);																	// 0x4CBCA0
	virtual PtrType Create_Binary(const std::string &name, const PtrType &left, const PtrType &right);			// 0x4CBCD0
	virtual PtrType Create_Unary(const std::string &name, const PtrType &child);								// 0x4CC120
	virtual PtrType Create_Constant(const std::string &text);													// 0x4CC500
	virtual PtrType Create_Variable(const std::string &name);													// 0x4CC580

protected:
	unsigned int m_flags;	// NAME 3 [m_flags]
};

RefCountPtr<ExprMathEvalClass<int> > Expr_Math_Int(unsigned int flags, const char *s);								// 0x4CBAF0  // NAME 3 [Expr_Math_Int]
RefCountPtr<ExprMathEvalClass<int> > Expr_Math_Int(unsigned int flags, const char *s, std::vector<std::string> *names);	// 0x4CBB30

#endif
