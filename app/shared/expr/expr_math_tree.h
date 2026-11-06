// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\expr\expr_math_tree.h (name assumed from
**	expr_math_tree.cpp, whose __FILE__ is 'e:\build\beta\G\app\shared\expr\expr_math_tree.cpp')
**
**	The expression tree: every node holds a token (an operand, or the operator of a unary or binary
**	node) and is walked by a visitor (Accept, vtable slot 1). The root class has no polymorphic base:
**	its constructor and destructor store no other vtable. The asserts name m_left, m_right, m_child
**	and the arguments child and n; the rest are D10 / D24 names.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __EXPR_MATH_TREE_H	// NAME 3 [expr_math_tree.h]: header name assumed from the client's expr_math_tree.cpp
#define __EXPR_MATH_TREE_H

#include <vector>
#include "expr_math_lexer.h"

class ExprMathTreeVisitorClass;

// A node and its token; alone, a leaf (a number or a name: token types 1, 2 and 5). 0x18 bytes, vtable 0xAEC0FC.
class ExprMathTreeClass	// NAME 3 [ExprMathTreeClass]: class-from-file (D24)
{
public:
	ExprMathTreeClass(const ExprMathTokenClass &token);				// 0x4D2580
	virtual ~ExprMathTreeClass();									// 0x4D2760, deleting 0x4D2730
	const ExprMathTokenClass &Get_Token() const;					// 0x4D27D0  // NAME 3 [Get_Token]
	void Set_Token(const ExprMathTokenClass &token);				// 0x4D27F0  // NAME 3 [Set_Token]
	virtual void Accept(ExprMathTreeVisitorClass *visitor);			// 0x4D2980: the visitor's slot 2  // NAME 3 [Accept]

protected:
	ExprMathTokenClass m_token;		// NAME 3 [m_token]: m_ as the asserts' m_left / m_child
};

// A binary operator and its operands; 0x20 bytes, vtable 0xAEC108.
class ExprMathBinaryClass : public ExprMathTreeClass	// NAME 3 [ExprMathBinaryClass]
{
public:
	ExprMathBinaryClass(const ExprMathTokenClass &token, ExprMathTreeClass *left, ExprMathTreeClass *right);	// 0x4D29A0
	virtual ~ExprMathBinaryClass();									// 0x4D2BE0, deleting 0x4D2BB0
	ExprMathTreeClass *Get_Left() const;							// 0x4D2CF0  // NAME 3 [Get_Left]
	ExprMathTreeClass *Get_Right() const;							// 0x4D2D10  // NAME 3 [Get_Right]
	void Set_Left(ExprMathTreeClass *n);							// 0x4D2D30  // NAME 3 [Set_Left]
	void Set_Right(ExprMathTreeClass *n);							// 0x4D2D80  // NAME 3 [Set_Right]
	virtual void Accept(ExprMathTreeVisitorClass *visitor);			// 0x4D2DD0: the visitor's slot 1

protected:
	ExprMathTreeClass *m_left;
	ExprMathTreeClass *m_right;
};

// A prefix operator and its operand; 0x1C bytes, vtable 0xAEC114.
class ExprMathUnaryClass : public ExprMathTreeClass	// NAME 3 [ExprMathUnaryClass]
{
public:
	ExprMathUnaryClass(const ExprMathTokenClass &token, ExprMathTreeClass *child);	// 0x4D2DF0
	virtual ~ExprMathUnaryClass();									// 0x4D3000, deleting 0x4D2FD0
	ExprMathTreeClass *Get_Child() const;							// 0x4D30D0  // NAME 3 [Get_Child]
	void Set_Child(ExprMathTreeClass *n);							// 0x4D30F0  // NAME 3 [Set_Child]
	virtual void Accept(ExprMathTreeVisitorClass *visitor);			// 0x4D3140: the visitor's slot 0

protected:
	ExprMathTreeClass *m_child;
};

// The visitor interface (no destructor slot).
class ExprMathTreeVisitorClass	// NAME 3 [ExprMathTreeVisitorClass]
{
public:
	virtual void Visit_Unary(ExprMathUnaryClass *node) = 0;		// NAME 3 [Visit_Unary]
	virtual void Visit_Binary(ExprMathBinaryClass *node) = 0;		// NAME 3 [Visit_Binary]
	virtual void Visit_Leaf(ExprMathTreeClass *node) = 0;			// NAME 3 [Visit_Leaf]
};

// Prints a tree in infix form, parenthesised; no vtable in the client (never constructed).
class ExprMathInfixPrinterClass : public ExprMathTreeVisitorClass	// NAME 3 [ExprMathInfixPrinterClass]
{
public:
	virtual void Visit_Unary(ExprMathUnaryClass *node);			// 0x4D3260
	virtual void Visit_Binary(ExprMathBinaryClass *node);			// 0x4D31D0
	virtual void Visit_Leaf(ExprMathTreeClass *node);				// 0x4D3190
};

// Prints a tree in postfix form; no vtable in the client (never constructed).
class ExprMathPostfixPrinterClass : public ExprMathTreeVisitorClass	// NAME 3 [ExprMathPostfixPrinterClass]
{
public:
	virtual void Visit_Unary(ExprMathUnaryClass *node);			// 0x4D3380
	virtual void Visit_Binary(ExprMathBinaryClass *node);			// 0x4D3310
	virtual void Visit_Leaf(ExprMathTreeClass *node);				// 0x4D32D0
};

// Collects, in prefix order, the tokens of the nodes whose token type is m_type; 0x18 bytes, vtable
// 0xAEC120. Its visits (0x4D3410-) are not rebuilt yet.
class ExprMathTokenCollectorClass : public ExprMathTreeVisitorClass	// NAME 3 [ExprMathTokenCollectorClass]
{
public:
	ExprMathTokenCollectorClass(int type);							// 0x4D33D0
	virtual void Visit_Unary(ExprMathUnaryClass *node);			// 0x4D3960
	virtual void Visit_Binary(ExprMathBinaryClass *node);			// 0x4D36C0
	virtual void Visit_Leaf(ExprMathTreeClass *node);				// 0x4D3410

	std::vector<ExprMathTokenClass> m_tokens;	// NAME 3 [m_tokens]
	int m_type;									// NAME 3 [m_type]
};

#endif
