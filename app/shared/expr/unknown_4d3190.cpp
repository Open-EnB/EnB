// Earth & Beyond client (reconstruction): app\shared\expr\unknown_4d3190.cpp, PROVISIONAL file name (D137). Notes at the end of the file.
#include <stdio.h>
#include "expr_math_tree.h"


// @0x4D3190
void ExprMathInfixPrinterClass::Visit_Leaf(ExprMathTreeNode *node)
{
	printf("%s", node->Get_Token().Text.c_str());
}


// @0x4D31D0
void ExprMathInfixPrinterClass::Visit_Binary(ExprMathTreeBinary *node)
{
	printf("(");
	node->Get_Left()->Accept(this);
	printf("%s", node->Get_Token().Text.c_str());
	node->Get_Right()->Accept(this);
	printf(")");
}


// @0x4D3260
void ExprMathInfixPrinterClass::Visit_Unary(ExprMathTreeUnary *node)
{
	printf("(%s", node->Get_Token().Text.c_str());
	node->Get_Child()->Accept(this);
	printf(")");
}


// @0x4D32D0
void ExprMathPostfixPrinterClass::Visit_Leaf(ExprMathTreeNode *node)
{
	printf("%s ", node->Get_Token().Text.c_str());
}


// @0x4D3310
void ExprMathPostfixPrinterClass::Visit_Binary(ExprMathTreeBinary *node)
{
	node->Get_Left()->Accept(this);
	node->Get_Right()->Accept(this);
	printf("%s ", node->Get_Token().Text.c_str());
}


// @0x4D3380
void ExprMathPostfixPrinterClass::Visit_Unary(ExprMathTreeUnary *node)
{
	node->Get_Child()->Accept(this);
	printf("%s ", node->Get_Token().Text.c_str());
}


// @0x4D33D0
ExprMathTokenCollectorClass::ExprMathTokenCollectorClass(int type) :
	m_type(type)
{
}


// @0x4D3410
void ExprMathTokenCollectorClass::Visit_Leaf(ExprMathTreeNode *node)
{
	const ExprLexerToken &token = node->Get_Token();
	if (token.Type == m_type) {
		m_tokens.push_back(token);
	}
}


// @0x4D36C0
void ExprMathTokenCollectorClass::Visit_Binary(ExprMathTreeBinary *node)
{
	ExprMathTokenCollectorClass::Visit_Leaf(node);
	node->Get_Left()->Accept(this);
	node->Get_Right()->Accept(this);
}


// @0x4D3960
void ExprMathTokenCollectorClass::Visit_Unary(ExprMathTreeUnary *node)
{
	ExprMathTokenCollectorClass::Visit_Leaf(node);
	node->Get_Child()->Accept(this);
}

/*
**	Reconstruction notes
**
**	Client 0x4D3190-0x4D42B0, an object of its own by the CRT initialiser table (SG, run 2026-11-18):
**	expr_math_tree.cpp's object (the asserts) ends with <string>'s guard 0x4D3160 (entry 109), this one with
**	0x4D4290 (entry 110). The infix and postfix printers, the token collector and its vector<ExprLexerToken>
**	instances (expr_math_tree.cpp's notes). No assert or string names the file: PROVISIONAL (D137).
*/
