// Earth & Beyond client (reconstruction): app\shared\expr\expr_math_tree.cpp. Notes at the end of the file.
#include <stdio.h>
#include "expr_math_tree.h"
#include "..\g_assert.h"


// @0x4D2580
ExprMathTreeNode::ExprMathTreeNode(const ExprLexerToken &token) :
	m_token(token)
{
}


// @0x4D2760
ExprMathTreeNode::~ExprMathTreeNode()
{
}


// @0x4D27D0
const ExprLexerToken &ExprMathTreeNode::Get_Token() const
{
	return m_token;
}


// @0x4D27F0
void ExprMathTreeNode::Set_Token(const ExprLexerToken &token)
{
	m_token = token;
}


// @0x4D2980
void ExprMathTreeNode::Accept(ExprMathTreeVisitorClass *visitor)
{
	visitor->Visit_Leaf(this);
}







// @0x4D29A0
ExprMathTreeBinary::ExprMathTreeBinary(const ExprLexerToken &token, ExprMathTreeNode *left, ExprMathTreeNode *right) :
	ExprMathTreeNode(token),
	m_left(left),
	m_right(right)
{
	G_ASSERT(m_left);
	G_ASSERT(m_right);
}

// @0x4D2BE0
ExprMathTreeBinary::~ExprMathTreeBinary()
{
	G_ASSERT(m_left);
	delete m_left;
	m_left = NULL;

	G_ASSERT(m_right);
	delete m_right;
	m_right = NULL;
}


// @0x4D2CF0
ExprMathTreeNode *ExprMathTreeBinary::Get_Left() const
{
	return m_left;
}


// @0x4D2D10
ExprMathTreeNode *ExprMathTreeBinary::Get_Right() const
{
	return m_right;
}


// @0x4D2D30
void ExprMathTreeBinary::Set_Left(ExprMathTreeNode *n)
{
	G_ASSERT(n);
	delete m_left;
	m_left = n;
}



// @0x4D2D80
void ExprMathTreeBinary::Set_Right(ExprMathTreeNode *n)
{
	G_ASSERT(n);
	delete m_right;
	m_right = n;
}


// @0x4D2DD0
void ExprMathTreeBinary::Accept(ExprMathTreeVisitorClass *visitor)
{
	visitor->Visit_Binary(this);
}








// @0x4D2DF0
ExprMathTreeUnary::ExprMathTreeUnary(const ExprLexerToken &token, ExprMathTreeNode *child) :
	ExprMathTreeNode(token),
	m_child(child)
{
	G_ASSERT(child);
}

// @0x4D3000
ExprMathTreeUnary::~ExprMathTreeUnary()
{
	G_ASSERT(m_child);
	delete m_child;
	m_child = NULL;
}


// @0x4D30D0
ExprMathTreeNode *ExprMathTreeUnary::Get_Child() const
{
	return m_child;
}


// @0x4D30F0
void ExprMathTreeUnary::Set_Child(ExprMathTreeNode *n)
{
	G_ASSERT(n);
	delete m_child;
	m_child = n;
}


// @0x4D3140
void ExprMathTreeUnary::Accept(ExprMathTreeVisitorClass *visitor)
{
	visitor->Visit_Unary(this);
}


/*
**	Reconstruction notes
**
**	client.exe 0x4D2580-0x4D42C0 by the asserts (G_ASSERT lines 52, 53, 59, 63, 86, 96, 120, 126, 142) and
**	the vtables: the node 0xAEC0FC (constructor 0x4D2580, destructor 0x4D2760 / deleting 0x4D2730,
**	Get_Token 0x4D27D0, Set_Token 0x4D27F0, Accept 0x4D2980), the binary node 0xAEC108 (0x4D29A0-0x4D2DD0)
**	and the unary node 0xAEC114 (0x4D2DF0-0x4D3140; <string>'s static initialiser sits in Accept's padding at
**	0x4D3160). The node constructor and destructor store no base vtable, so the node class is the root.
**	SG (run 2026-11-18): this object ends with <string>'s guard 0x4D3160 (CRT entry 109); the visitors below
**	are unknown_4d3190.cpp, closed by their own guard 0x4D4290 (entry 110).
**	Then two visitors with no vtable in the client (never constructed): an infix printer (0x4D3190-0x4D3260)
**	and a postfix printer (0x4D32D0-0x4D3380), both printf, and the token collector's constructor 0x4D33D0
**	(vtable 0xAEC120) and its visits Visit_Leaf 0x4D3410, Visit_Binary 0x4D36C0, Visit_Unary 0x4D3960, then
**	this object's vector<ExprLexerToken> instances (_Destroy 0x4D3BE0, size 0x4D3C20, _Ucopy 0x4D3C60,
**	_Ufill 0x4D3CB0, fill 0x4D3CF0, copy_backward 0x4D3EC0, _Construct 0x4D40C0) up to attrib_dir's
**	neighbour 0x4D42C0. Visit_Binary and Visit_Unary call Visit_Leaf (qualified, so inlined): the extra
**	inline level is what leaves the insert's size() 0x4D3C20 out of line, as in the client; written
**	out in each, it inlines (a dead m_tokens.size() before the push_back also matched).
**
**	The printers call Get_Token, Get_Left, Get_Right and Get_Child out of line, as the other files do;
**	/Ob2 inlines them here unless their definitions sit in #pragma auto_inline(off) (in place of blank
**	lines, so the asserts keep their lines).
*/
