// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\expr\expr_lexer_impl.h (name assumed from
**	expr_lexer_impl.cpp, whose __FILE__ is 'e:\build\beta\G\app\shared\expr\expr_lexer_impl.cpp')
**
**	The lexer: each matcher measures the text at the cursor; the longest match makes the token, typed
**	with its matcher's type.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __EXPR_LEXER_IMPL_H	// NAME 3 [expr_lexer_impl.h]: header name assumed from the client's expr_lexer_impl.cpp
#define __EXPR_LEXER_IMPL_H

#include <string>
#include <vector>
#include <utility>
#include "expr_math_lexer.h"

// 0x3C bytes, vtable 0xAEBADC.
class ExprLexerImplClass : public ExprMathLexerClass	// NAME 3 [ExprLexerImplClass]: class-from-file (D24)
{
public:
	ExprLexerImplClass();										// 0x4C4980
	virtual ~ExprLexerImplClass();								// 0x4C4B40

	virtual ExprMathTokenClass Get_Token();						// 0x4C4F60
	virtual bool Has_Token();									// 0x4C50F0
	virtual bool Has_Error();									// 0x4C5110
	virtual void Next();										// 0x4C5150
	virtual void Set_Source(const char *source);				// 0x4C5410
	virtual void Reset();										// 0x4C5530

	void Add_Matcher(int type, ExprLexerMatchClass *m);		// 0x4C4CF0  // NAME 3 [Add_Matcher]: 'm' from the assert; the lexer deletes it

protected:
	std::vector<std::pair<int, ExprLexerMatchClass *> > m_matchers;	// +4  // NAME 3 [m_matchers]: m_ as the asserts' m
	std::string m_source;											// +0x14  // NAME 3 [m_source]
	const char *m_cursor;											// +0x24  // NAME 3 [m_cursor]
	ExprMathTokenClass m_token;										// +0x28  // NAME 3 [m_token]: as ExprMathTreeClass's
};

#endif
