// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Interim placement (D137): the expression tree's nodes as far as expr_math_parser_impl.cpp
**	builds them. expr_math_tree.cpp (0x4D27D0-, asserts from 0x4D29A0) is not rebuilt yet.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __EXPR_MATH_TREE_H	// NAME 3 [expr_math_tree.h]: header name assumed from the client's expr_math_tree.cpp
#define __EXPR_MATH_TREE_H

#include "expr_math_lexer.h"

class ExprMathTreeClass	// NAME 3 [ExprMathTreeClass]: the nodes' base (slot 0 the deleting destructor)
{
public:
	virtual ~ExprMathTreeClass() {}
	virtual float Evaluate() = 0;	// NAME 3 [Evaluate]: slot 1 (leaf 0x4D2980)
};

// A leaf: a number or a name (token types 1 and 2); 0x18 bytes, vtable 0xAEC0FC.
class ExprMathLeafClass : public ExprMathTreeClass	// NAME 3 [ExprMathLeafClass]
{
public:
	ExprMathLeafClass(const ExprMathTokenClass &token);	// 0x4D2580 (expr_math_tree.cpp's window)
	virtual ~ExprMathLeafClass();										// 0x4D2760, deleting 0x4D2730
	virtual float Evaluate();												// 0x4D2980

	ExprMathTokenClass Token;		// NAME 3 [Token]
};

// A prefix operator and its operand; 0x1C bytes.
class ExprMathUnaryClass : public ExprMathTreeClass	// NAME 3 [ExprMathUnaryClass]
{
public:
	ExprMathUnaryClass(const ExprMathTokenClass &op, ExprMathTreeClass *operand);	// 0x4D2DF0
	virtual ~ExprMathUnaryClass();
	virtual float Evaluate();

	ExprMathTokenClass Op;			// NAME 3 [Op]
	ExprMathTreeClass *Operand;		// NAME 3 [Operand]
};

// A binary operator and its operands; 0x20 bytes.
class ExprMathBinaryClass : public ExprMathTreeClass	// NAME 3 [ExprMathBinaryClass]
{
public:
	ExprMathBinaryClass(const ExprMathTokenClass &op, ExprMathTreeClass *left, ExprMathTreeClass *right);	// 0x4D29A0
	virtual ~ExprMathBinaryClass();
	virtual float Evaluate();

	ExprMathTokenClass Op;			// NAME 3 [Op]
	ExprMathTreeClass *Left;		// NAME 3 [Left]
	ExprMathTreeClass *Right;		// NAME 3 [Right]
};

#endif
