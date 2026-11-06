// Earth & Beyond client (reconstruction): app\shared\expr\expr_math_parser_impl.cpp. Notes at the end of the file.
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include "expr_math_parser_impl.h"
#include "expr_math_tree.h"
#include "..\unknown_debug.h"



// @0x4D0AC0
// Appends a precedence level holding the space-separated operators ops.
void ExprMathParserImplClass::Add_Group(int type, const char *ops)
{
	FAIL_IF(!ops) return;

	m_groups.resize(m_groups.size() + 1);
	GroupStruct &group = m_groups.back();
	group.Type = type;

	char *buf = strdup(ops);
	char *token = strtok(buf, " ");
	while (token) {
		group.Ops.push_back(std::string(token));
		token = strtok(NULL, " ");
	}
	free(buf);
}


// @0x4D0DF0
// Adds the operators ops to the first group of the given type that holds similar.
// NOT EXACT: the loop latch reloads it / &it->Ops in the other order (2 instructions swapped, same length)


void ExprMathParserImplClass::Add_Similar(int type, const char *ops, const char *similar)
{
	FAIL_IF(!ops) return;
	FAIL_IF(!similar) return;

	for (std::vector<GroupStruct>::iterator it = m_groups.begin(); it != m_groups.end(); ++it) {
		if (it->Type == type) {
			std::vector<std::string> &list = it->Ops;
			std::vector<std::string>::iterator op = std::find(list.begin(), list.end(), similar);
			if (op != list.end()) {
				char *buf = strdup(ops);
				char *token = strtok(buf, " ");
				while (token) {
					list.push_back(std::string(token));
					token = strtok(NULL, " ");
				}
				free(buf);
				return;
			}
		}
	}
}













// @0x4D10A0
ExprMathTreeClass *ExprMathParserImplClass::Parse(ExprMathLexerClass *lex)
{
	FAIL_IF(!lex) return NULL;
	return Parse_Level(lex, 0);
}


// @0x4D10E0
// Parses one level; the level past the last group: a parenthesised expression or a primary.
// NOT EXACT: the level > size guard keeps its own return NULL, and the token's destructor is inline / called on the other two exits
ExprMathTreeClass *ExprMathParserImplClass::Parse_Level(ExprMathLexerClass *lex, int level)
{
	G_ASSERT(level <= (int)m_groups.size());
	if (level > (int)m_groups.size()) {
		return NULL;
	}
	if (level == (int)m_groups.size()) {
		ExprMathTokenClass token = Peek(lex);
		if (token.Is_Valid()) {
			if (token.Text == "(") {
				lex->Next();
				ExprMathTreeClass *tree = Parse_Level(lex, 0);
				if (tree) {
					token = Peek(lex);
					if (token.Is_Valid() && token.Text == ")") {
						lex->Next();
						return tree;
					}
					delete tree;
				}
			} else {
				return Parse_Primary(lex);
			}
		}
		return NULL;
	}
	if (m_groups[level].Type == 0) {
		return Parse_Unary(lex, level);
	}
	return Parse_Binary(lex, level);
}


// @0x4D13E0
ExprMathTreeClass *ExprMathParserImplClass::Parse_Unary(ExprMathLexerClass *lex, int level)
{
	if (Is_Operator(Peek(lex), level)) {
		ExprMathTokenClass op = lex->Get_Token();
		lex->Next();
		ExprMathTreeClass *operand = Parse_Level(lex, level);
		if (!operand) {
			return NULL;
		}
		return new ExprMathUnaryClass(op, operand);
	}
	return Parse_Level(lex, level + 1);
}


// @0x4D1600
ExprMathTreeClass *ExprMathParserImplClass::Parse_Binary(ExprMathLexerClass *lex, int level)
{
	ExprMathTreeClass *left = Parse_Level(lex, level + 1);
	if (!left) {
		return NULL;
	}
	while (Is_Operator(Peek(lex), level)) {
		ExprMathTokenClass op = lex->Get_Token();
		lex->Next();
		ExprMathTreeClass *right = Parse_Level(lex, level + 1);
		if (!right) {
			delete left;
			return NULL;
		}
		left = new ExprMathBinaryClass(op, left, right);
	}
	return left;
}


// @0x4D18A0
ExprMathTreeClass *ExprMathParserImplClass::Parse_Primary(ExprMathLexerClass *lex)
{
	ExprMathTokenClass token = Peek(lex);
	if (token.Type == 2 || token.Type == 1) {
		ExprMathTreeClass *leaf = new ExprMathTreeClass(token);
		lex->Next();
		return leaf;
	}
	return NULL;
}


// @0x4D19E0
bool ExprMathParserImplClass::Is_Operator(const ExprMathTokenClass &token, int level)
{
	if (!token.Is_Valid()) {
		return false;
	}
	if (token.Type != 4) {
		return false;
	}
	std::vector<std::string> &ops = m_groups[level].Ops;
	return std::find(ops.begin(), ops.end(), token.Text) != ops.end();
}


// @0x4D1AC0
ExprMathTokenClass ExprMathParserImplClass::Peek(ExprMathLexerClass *lex)
{
	while (lex->Get_Token().Type == 0) {
		lex->Next();
	}
	return lex->Get_Token();
}



/*
**	Reconstruction notes
**
**	client.exe 0x4D0A20-0x4D2050 by the asserts (FAIL_IF lines 15, 38, 39, 74; G_ASSERT line 84) and
**	the calls: Add_Group 0x4D0AC0, Add_Similar 0x4D0DF0, Parse 0x4D10A0 (vtable slot 1), Parse_Level
**	0x4D10E0, Parse_Unary 0x4D13E0, Parse_Binary 0x4D1600, Parse_Primary 0x4D18A0, Is_Operator
**	0x4D19E0 (uncalled: inlined into Parse_Unary / Parse_Binary), Peek 0x4D1AC0; then this object's
**	template instances (vector<GroupStruct>::size 0x4D1B60, insert 0x4D1BA0, erase 0x4D1F90, the
**	GroupStruct _Construct 0x4D2050 and the vector<string> helpers after it). The parser built by
**	expr_math_parser_factory.cpp (0x4D0790) is a class of its own (vtable 0xAEC0E0: its destructor
**	0x4D0920, Parse, a Parse_Primary 0x4D07E0 that also takes token type 5), so ExprMathParserImplClass's
**	own vtable and Parse_Primary are unreferenced. The leaf node's constructor 0x4D2580 is called, not
**	inlined, so it is taken to open expr_math_tree.cpp's window.
*/
