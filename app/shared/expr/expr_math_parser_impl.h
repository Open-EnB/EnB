// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\expr\expr_math_parser_impl.h (name assumed from
**	expr_math_parser_impl.cpp, whose __FILE__ is 'e:\build\beta\G\app\shared\expr\expr_math_parser_impl.cpp')
**
**	A precedence-climbing parser: m_groups holds one operator group per precedence level (Type 0
**	prefix operators, otherwise left-associative binary operators), lowest first; the level past
**	the last group parses a parenthesised expression or a primary. The asserts name m_groups and
**	the arguments ops, similar, lex and level; the rest are D10 / D24 names.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __EXPR_MATH_PARSER_IMPL_H	// NAME 3 [expr_math_parser_impl.h]: header name assumed from the client's expr_math_parser_impl.cpp
#define __EXPR_MATH_PARSER_IMPL_H

#include <string>
#include <vector>
#include "expr_math_parser.h"
#include "expr_math_lexer.h"

class ExprMathParserImplClass : public ExprMathParserClass	// NAME 3 [ExprMathParserImplClass]: from the file name
{
public:
	struct GroupStruct	// NAME 3 [GroupStruct]: 0x14 bytes, one precedence level
	{
		int Type;							// NAME 3 [Type]: 0 prefix (Parse_Unary), else binary (Parse_Binary)
		std::vector<std::string> Ops;		// NAME 3 [Ops]: from the argument 'ops'
	};

	void Add_Group(int type, const char *ops);							// 0x4D0AC0  // NAME 3 [Add_Group]
	void Add_Similar(int type, const char *ops, const char *similar);	// 0x4D0DF0  // NAME 3 [Add_Similar]

	virtual ExprMathTreeClass *Parse(ExprMathLexerClass *lex);			// 0x4D10A0
	virtual ExprMathTreeClass *Parse_Primary(ExprMathLexerClass *lex);	// 0x4D18A0  // NAME 3 [Parse_Primary]

protected:
	ExprMathTreeClass *Parse_Level(ExprMathLexerClass *lex, int level);	// 0x4D10E0  // NAME 3 [Parse_Level]
	ExprMathTreeClass *Parse_Unary(ExprMathLexerClass *lex, int level);	// 0x4D13E0  // NAME 3 [Parse_Unary]
	ExprMathTreeClass *Parse_Binary(ExprMathLexerClass *lex, int level);	// 0x4D1600  // NAME 3 [Parse_Binary]
	bool Is_Operator(const ExprMathTokenClass &token, int level);		// 0x4D19E0  // NAME 3 [Is_Operator]
	ExprMathTokenClass Peek(ExprMathLexerClass *lex);					// 0x4D1AC0  // NAME 3 [Peek]: skips type-0 tokens

	std::vector<GroupStruct> m_groups;
};

#endif
