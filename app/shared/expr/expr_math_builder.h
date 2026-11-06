// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\expr\expr_math_builder.h (the client's __FILE__ of
**	the G_ASSERT 'factory', line 51).
**
**	Builds an evaluation tree (expr_math_eval_impl.h) from an expression: the lexer and parser make
**	the syntax tree, a visitor turns each node into an evaluation node through a factory (the
**	value type's operators), folding constant subtrees. Only 'factory' is the client's name.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __EXPR_MATH_BUILDER_H
#define __EXPR_MATH_BUILDER_H

#include <string>
#include <vector>
#include "expr_math_eval_impl.h"
#include "expr_math_lexer.h"
#include "expr_math_parser.h"
#include "expr_math_tree.h"

// Creates the evaluation nodes (vtable 0xAEBBE0 for float's factory).
template <class T>
class ExprMathEvalFactoryClass	// NAME 3 [ExprMathEvalFactoryClass]
{
public:
	typedef RefCountPtr<ExprMathEvalClass<T> > PtrType;
	virtual PtrType Create_Binary(const std::string &name, const PtrType &left, const PtrType &right) = 0;	// NAME 3 [Create_Binary]
	virtual PtrType Create_Unary(const std::string &name, const PtrType &child) = 0;						// NAME 3 [Create_Unary]
	virtual PtrType Create_Constant(const std::string &text) = 0;											// NAME 3 [Create_Constant]
	virtual PtrType Create_Variable(const std::string &name) = 0;											// NAME 3 [Create_Variable]
};

// Turns a syntax tree into an evaluation tree; vtable 0xAEBDD4 (float).
template <class T>
class ExprMathBuildVisitorClass : public ExprMathTreeVisitorClass	// NAME 3 [ExprMathBuildVisitorClass]
{
public:
	typedef RefCountPtr<ExprMathEvalClass<T> > PtrType;

	ExprMathBuildVisitorClass(ExprMathEvalFactoryClass<T> *factory) : m_factory(factory)
	{




		G_ASSERT(factory);
	}

	// NOT EXACT: 0x4CAF50: from the pseudocode; not matched yet
	virtual void Visit_Unary(ExprMathUnaryClass *node)
	{
		node->Get_Child()->Accept(this);
		if (m_result == NULL) {
			return;
		}
		PtrType child = m_result;
		m_result = m_factory->Create_Unary(std::string(node->Get_Token().Text.c_str()), child);
		if (m_result == NULL) {
			return;
		}
		if (m_result->Is_Constant()) {
			m_result = Create_NEW((ExprMathEvalClass<T> *)new ExprMathConstantClass<T>(m_result->Evaluate(NULL)));
		}
	}

	// NOT EXACT: 0x4CB260: from the pseudocode; not matched yet
	virtual void Visit_Binary(ExprMathBinaryClass *node)
	{
		node->Get_Left()->Accept(this);
		if (m_result == NULL) {
			return;
		}
		PtrType left = m_result;
		node->Get_Right()->Accept(this);
		if (m_result == NULL) {
			return;
		}
		PtrType right = m_result;
		m_result = m_factory->Create_Binary(std::string(node->Get_Token().Text.c_str()), left, right);
		if (m_result == NULL) {
			return;
		}
		if (m_result->Is_Constant()) {
			m_result = Create_NEW((ExprMathEvalClass<T> *)new ExprMathConstantClass<T>(m_result->Evaluate(NULL)));
		}
	}

	// NOT EXACT: 0x4CB620: from the pseudocode; not matched yet
	virtual void Visit_Leaf(ExprMathTreeClass *node)
	{
		const ExprMathTokenClass &token = node->Get_Token();
		if (token.Type == 1) {
			m_result = m_factory->Create_Variable(token.Text);
		} else if (token.Type == 2) {
			m_result = m_factory->Create_Constant(token.Text);
		} else if (token.Type == 5) {
			m_result = m_factory->Create_Variable(token.Text.substr(1, token.Text.size() - 2));
		}
	}

	const PtrType &Get_Result() const { return m_result; }	// NAME 3 [Get_Result]

protected:
	ExprMathEvalFactoryClass<T> *m_factory;	// NAME 3 [m_factory]
	PtrType m_result;						// NAME 3 [m_result]
};

// The lexer, parser and factory an expression is built with.
template <class T>
class ExprMathBuilderClass	// NAME 3 [ExprMathBuilderClass]
{
public:
	typedef RefCountPtr<ExprMathEvalClass<T> > PtrType;

	ExprMathBuilderClass(ExprMathLexerClass *lexer, ExprMathParserClass *parser, ExprMathEvalFactoryClass<T> *factory) :
		m_lexer(lexer), m_parser(parser), m_factory(factory) {}

	// NOT EXACT: 0x4CACE0: from the pseudocode; not matched yet
	PtrType Build(const char *source, std::vector<std::string> *names)
	{
		m_lexer->Set_Source(source);
		ExprMathTreeClass *tree = m_parser->Parse(m_lexer);
		if (tree == NULL) {
			return NULL;
		}
		if (m_lexer->Has_Error() || m_lexer->Has_Token()) {
			delete tree;
			return NULL;
		}
		ExprMathBuildVisitorClass<T> visitor(m_factory);
		tree->Accept(&visitor);
		if (names != NULL) {
			ExprMathTokenCollectorClass collector(1);
			tree->Accept(&collector);
			for (int i = 0; i < (int)collector.m_tokens.size(); i++) {
				names->push_back(collector.m_tokens[i].Text);
			}
		}
		delete tree;
		return visitor.Get_Result();
	}

protected:
	ExprMathLexerClass *m_lexer;				// NAME 3 [m_lexer]
	ExprMathParserClass *m_parser;				// NAME 3 [m_parser]
	ExprMathEvalFactoryClass<T> *m_factory;		// NAME 3 [m_factory]
};

#endif
