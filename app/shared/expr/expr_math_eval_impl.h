// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\expr\expr_math_eval_impl.h (the client's __FILE__ of
**	the FAIL_IFs 'right == 0', lines 175 and 190).
**
**	The evaluation tree, a template over the value type (float in expr_math_float.cpp, int in
**	expr_math_int.cpp): ref-counted nodes evaluated against a context that supplies the variables,
**	and the operator groups that create them by name (the groups as ExprMathParserFactoryClass's
**	flags). Only 'right' is the client's; the other names are D10 / D24 names.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __EXPR_MATH_EVAL_IMPL_H
#define __EXPR_MATH_EVAL_IMPL_H

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "..\..\..\wwlib\refcount.h"
#include "..\..\..\wwlib\ref_ptr.h"
#include "..\unknown_debug.h"

// Supplies a variable's value by name (slot +4).
template <class T>
class ExprMathContextClass	// NAME 3 [ExprMathContextClass]
{
public:
	virtual ~ExprMathContextClass() {}
	virtual bool Get_Value(const char *name, T &value) = 0;	// NAME 3 [Get_Value]
};

// A node: 8 bytes (RefCountClass), vtable 0xAEBC58 (float).
template <class T>
class ExprMathEvalClass : public RefCountClass	// NAME 3 [ExprMathEvalClass]
{
public:
	virtual T Evaluate(ExprMathContextClass<T> *context) = 0;	// +8  // NAME 3 [Evaluate]
	virtual bool Is_Constant() = 0;								// +0xC  // NAME 3 [Is_Constant]
};

// A number; vtable 0xAEBC1C (float).
template <class T>
class ExprMathConstantClass : public ExprMathEvalClass<T>	// NAME 3 [ExprMathConstantClass]
{
public:
	ExprMathConstantClass(T value) : m_value(value) {}
	virtual T Evaluate(ExprMathContextClass<T> *context) { return m_value; }
	virtual bool Is_Constant() { return true; }

protected:
	T m_value;	// NAME 3 [m_value]
};

// A variable, looked up in the context; 0 when the context has none. Vtable 0xAEBC30 (float).
template <class T>
class ExprMathVariableClass : public ExprMathEvalClass<T>	// NAME 3 [ExprMathVariableClass]
{
public:
	ExprMathVariableClass(const char *name) : m_name(NULL) { m_name = strdup(name); }
	virtual ~ExprMathVariableClass() { free(m_name); }
	virtual T Evaluate(ExprMathContextClass<T> *context)
	{
		T value;
		const char *name = m_name;
		if (!context->Get_Value(name, value)) {
			return 0;
		}
		return value;
	}
	virtual bool Is_Constant() { return false; }

protected:
	char *m_name;	// NAME 3 [m_name]
};

// A binary operator's base: constant when both operands are. Vtable 0xAEBC44 (float).
template <class T>
class ExprMathEvalBinaryClass : public ExprMathEvalClass<T>	// NAME 3 [ExprMathEvalBinaryClass]
{
public:
	ExprMathEvalBinaryClass(const RefCountPtr<ExprMathEvalClass<T> > &left, const RefCountPtr<ExprMathEvalClass<T> > &right);
	virtual bool Is_Constant() { return m_left->Is_Constant() && m_right->Is_Constant(); }

protected:
	RefCountPtr<ExprMathEvalClass<T> > m_left;		// NAME 3 [m_left]
	RefCountPtr<ExprMathEvalClass<T> > m_right;		// NAME 3 [m_right]
};

template <class T>
ExprMathEvalBinaryClass<T>::ExprMathEvalBinaryClass(const RefCountPtr<ExprMathEvalClass<T> > &left, const RefCountPtr<ExprMathEvalClass<T> > &right) :
	m_left(left),
	m_right(right)
{
}

// A prefix operator's base: constant when its operand is (no vtable of its own in the client).
template <class T>
class ExprMathEvalUnaryClass : public ExprMathEvalClass<T>	// NAME 3 [ExprMathEvalUnaryClass]
{
public:
	ExprMathEvalUnaryClass(const RefCountPtr<ExprMathEvalClass<T> > &child) : m_child(child) {}
	virtual bool Is_Constant() { return m_child->Is_Constant(); }

protected:
	RefCountPtr<ExprMathEvalClass<T> > m_child;		// NAME 3 [m_child]
};

// The arithmetic operators (flag 1).





























template <class T>
class ExprMathEvalAddClass : public ExprMathEvalBinaryClass<T>	// NAME 3 [ExprMathEvalAddClass]
{
public:
	ExprMathEvalAddClass(const RefCountPtr<ExprMathEvalClass<T> > &left, const RefCountPtr<ExprMathEvalClass<T> > &right) : ExprMathEvalBinaryClass<T>(left, right) {}
	virtual T Evaluate(ExprMathContextClass<T> *context) { return m_left->Evaluate(context) + m_right->Evaluate(context); }
};

template <class T>
class ExprMathEvalSubtractClass : public ExprMathEvalBinaryClass<T>	// NAME 3 [ExprMathEvalSubtractClass]
{
public:
	ExprMathEvalSubtractClass(const RefCountPtr<ExprMathEvalClass<T> > &left, const RefCountPtr<ExprMathEvalClass<T> > &right) : ExprMathEvalBinaryClass<T>(left, right) {}
	virtual T Evaluate(ExprMathContextClass<T> *context) { return m_left->Evaluate(context) - m_right->Evaluate(context); }
};

template <class T>
class ExprMathEvalMultiplyClass : public ExprMathEvalBinaryClass<T>	// NAME 3 [ExprMathEvalMultiplyClass]
{
public:
	ExprMathEvalMultiplyClass(const RefCountPtr<ExprMathEvalClass<T> > &left, const RefCountPtr<ExprMathEvalClass<T> > &right) : ExprMathEvalBinaryClass<T>(left, right) {}
	virtual T Evaluate(ExprMathContextClass<T> *context) { return m_left->Evaluate(context) * m_right->Evaluate(context); }
};

template <class T>
class ExprMathEvalDivideClass : public ExprMathEvalBinaryClass<T>	// NAME 3 [ExprMathEvalDivideClass]
{
public:
	ExprMathEvalDivideClass(const RefCountPtr<ExprMathEvalClass<T> > &left, const RefCountPtr<ExprMathEvalClass<T> > &right) : ExprMathEvalBinaryClass<T>(left, right) {}
	virtual T Evaluate(ExprMathContextClass<T> *context)
	{
		T right = m_right->Evaluate(context);
		FAIL_IF(right == 0) return 0;
		return m_left->Evaluate(context) / right;
	}
};


// div: the integer quotient.
template <class T>
class ExprMathEvalIntDivideClass : public ExprMathEvalBinaryClass<T>	// NAME 3 [ExprMathEvalIntDivideClass]
{
public:
	ExprMathEvalIntDivideClass(const RefCountPtr<ExprMathEvalClass<T> > &left, const RefCountPtr<ExprMathEvalClass<T> > &right) : ExprMathEvalBinaryClass<T>(left, right) {}
	virtual T Evaluate(ExprMathContextClass<T> *context)
	{
		T right = m_right->Evaluate(context);
		FAIL_IF(right == 0) return 0;
		return (T)((int)m_left->Evaluate(context) / (int)right);
	}
};

// A binary function (min, max) through a pointer.
template <class T>
class ExprMathEvalFunction2Class : public ExprMathEvalBinaryClass<T>	// NAME 3 [ExprMathEvalFunction2Class]
{
public:
	typedef T (*FunctionType)(const T &, const T &);	// NAME 3 [FunctionType]
	ExprMathEvalFunction2Class(const RefCountPtr<ExprMathEvalClass<T> > &left, const RefCountPtr<ExprMathEvalClass<T> > &right, FunctionType function) :
		ExprMathEvalBinaryClass<T>(left, right), m_function(function) {}
	virtual T Evaluate(ExprMathContextClass<T> *context)
	{
		T right = m_right->Evaluate(context);
		T left = m_left->Evaluate(context);
		return m_function(left, right);
	}

protected:
	FunctionType m_function;	// NAME 3 [m_function]
};

// A function of one argument (abs, sqrt) through a pointer.
template <class T>
class ExprMathEvalFunction1Class : public ExprMathEvalUnaryClass<T>	// NAME 3 [ExprMathEvalFunction1Class]
{
public:
	typedef T (*FunctionType)(const T &);	// NAME 3 [FunctionType]
	ExprMathEvalFunction1Class(const RefCountPtr<ExprMathEvalClass<T> > &child, FunctionType function) :
		ExprMathEvalUnaryClass<T>(child), m_function(function) {}
	virtual T Evaluate(ExprMathContextClass<T> *context)
	{
		T value = m_child->Evaluate(context);
		return m_function(value);
	}

protected:
	FunctionType m_function;	// NAME 3 [m_function]
};

template <class T> T Expr_Math_Min(const T &a, const T &b) { return a < b ? a : b; }		// NAME 3 [Expr_Math_Min]
template <class T> T Expr_Math_Max(const T &a, const T &b) { return a > b ? a : b; }		// NAME 3 [Expr_Math_Max]
template <class T> T Expr_Math_Abs(const T &a) { return a < 0 ? -a : a; }				// NAME 3 [Expr_Math_Abs]
template <class T> T Expr_Math_Sqrt(const T &a) { return a <= 0 ? 0 : (T)sqrt(a); }		// NAME 3 [Expr_Math_Sqrt]

template <class T>
class ExprMathEvalNegateClass : public ExprMathEvalUnaryClass<T>	// NAME 3 [ExprMathEvalNegateClass]
{
public:
	ExprMathEvalNegateClass(const RefCountPtr<ExprMathEvalClass<T> > &child) : ExprMathEvalUnaryClass<T>(child) {}
	virtual T Evaluate(ExprMathContextClass<T> *context) { return -m_child->Evaluate(context); }
};

template <class T>
class ExprMathEvalPlusClass : public ExprMathEvalUnaryClass<T>	// NAME 3 [ExprMathEvalPlusClass]
{
public:
	ExprMathEvalPlusClass(const RefCountPtr<ExprMathEvalClass<T> > &child) : ExprMathEvalUnaryClass<T>(child) {}
	virtual T Evaluate(ExprMathContextClass<T> *context) { return m_child->Evaluate(context); }
};

// The comparisons (flag 2): 1 or 0.
#define EXPR_MATH_COMPARE_CLASS(NAME, OP) \
template <class T> \
class NAME : public ExprMathEvalBinaryClass<T> \
{ \
public: \
	NAME(const RefCountPtr<ExprMathEvalClass<T> > &left, const RefCountPtr<ExprMathEvalClass<T> > &right) : ExprMathEvalBinaryClass<T>(left, right) {} \
	virtual T Evaluate(ExprMathContextClass<T> *context) { return m_left->Evaluate(context) OP m_right->Evaluate(context); } \
};

EXPR_MATH_COMPARE_CLASS(ExprMathEvalLessEqualClass, <=)		// NAME 3 [ExprMathEvalLessEqualClass]
EXPR_MATH_COMPARE_CLASS(ExprMathEvalLessClass, <)			// NAME 3 [ExprMathEvalLessClass]
EXPR_MATH_COMPARE_CLASS(ExprMathEvalGreaterEqualClass, >=)	// NAME 3 [ExprMathEvalGreaterEqualClass]
EXPR_MATH_COMPARE_CLASS(ExprMathEvalGreaterClass, >)		// NAME 3 [ExprMathEvalGreaterClass]
EXPR_MATH_COMPARE_CLASS(ExprMathEvalEqualClass, ==)			// NAME 3 [ExprMathEvalEqualClass]
EXPR_MATH_COMPARE_CLASS(ExprMathEvalNotEqualClass, !=)		// NAME 3 [ExprMathEvalNotEqualClass]

// The logical operators (flag 8): 1 or 0.
template <class T>
class ExprMathEvalAndClass : public ExprMathEvalBinaryClass<T>	// NAME 3 [ExprMathEvalAndClass]
{
public:
	ExprMathEvalAndClass(const RefCountPtr<ExprMathEvalClass<T> > &left, const RefCountPtr<ExprMathEvalClass<T> > &right) : ExprMathEvalBinaryClass<T>(left, right) {}
	virtual T Evaluate(ExprMathContextClass<T> *context) { return m_left->Evaluate(context) != 0 && m_right->Evaluate(context) != 0; }
};

template <class T>
class ExprMathEvalOrClass : public ExprMathEvalBinaryClass<T>	// NAME 3 [ExprMathEvalOrClass]
{
public:
	ExprMathEvalOrClass(const RefCountPtr<ExprMathEvalClass<T> > &left, const RefCountPtr<ExprMathEvalClass<T> > &right) : ExprMathEvalBinaryClass<T>(left, right) {}
	virtual T Evaluate(ExprMathContextClass<T> *context) { return m_left->Evaluate(context) != 0 || m_right->Evaluate(context) != 0; }
};

template <class T>
class ExprMathEvalNotClass : public ExprMathEvalUnaryClass<T>	// NAME 3 [ExprMathEvalNotClass]
{
public:
	ExprMathEvalNotClass(const RefCountPtr<ExprMathEvalClass<T> > &child) : ExprMathEvalUnaryClass<T>(child) {}
	virtual T Evaluate(ExprMathContextClass<T> *context) { return m_child->Evaluate(context) == 0; }
};

// The operator groups: each creates a node by its operator's name, NULL for a name not in it.
template <class T>
class ExprMathArithmeticOpsClass	// NAME 3 [ExprMathArithmeticOpsClass]: flag 1
{
public:
	typedef RefCountPtr<ExprMathEvalClass<T> > PtrType;
	// NOT EXACT: the client calls the binary base constructor in every branch (ours inlines it in later ones); 0x4C8920, 0x4C8CE0, 0x4C9010, 0x4C9300 for float
	static PtrType Create_Binary(const std::string &name, const PtrType &left, const PtrType &right) throw()
	{
		ExprMathEvalClass<T> *node;
		if (name == "/") {
			node = new ExprMathEvalDivideClass<T>(left, right);
		} else if (name == "*") {
			node = new ExprMathEvalMultiplyClass<T>(left, right);
		} else if (name == "+") {
			node = new ExprMathEvalAddClass<T>(left, right);
		} else if (name == "-") {
			node = new ExprMathEvalSubtractClass<T>(left, right);
		} else if (name == "div") {
			node = new ExprMathEvalIntDivideClass<T>(left, right);
		} else {
			return NULL;
		}
		return Create_NEW(node);
	}
	// NOT EXACT: not matched yet (0x4C8B80, 0x4C8EB0, 0x4C92D0, 0x4C94C0 for float)
	static PtrType Create_Unary(const std::string &name, const PtrType &child) throw()
	{
		ExprMathEvalClass<T> *node;
		if (name == "-") {
			node = new ExprMathEvalNegateClass<T>(child);
		} else if (name == "+") {
			node = new ExprMathEvalPlusClass<T>(child);
		} else {
			return NULL;
		}
		return Create_NEW(node);
	}
};

template <class T>
class ExprMathFunctionOpsClass	// NAME 3 [ExprMathFunctionOpsClass]: flag 0x10
{
public:
	typedef RefCountPtr<ExprMathEvalClass<T> > PtrType;
	// NOT EXACT: the client calls the binary base constructor in every branch (ours inlines it in later ones); 0x4C8920, 0x4C8CE0, 0x4C9010, 0x4C9300 for float
	static PtrType Create_Binary(const std::string &name, const PtrType &left, const PtrType &right) throw()
	{
		ExprMathEvalClass<T> *node;
		if (name == "min") {
			node = new ExprMathEvalFunction2Class<T>(left, right, Expr_Math_Min);
		} else if (name == "max") {
			node = new ExprMathEvalFunction2Class<T>(left, right, Expr_Math_Max);
		} else {
			return NULL;
		}
		return Create_NEW(node);
	}
	// NOT EXACT: not matched yet (0x4C8B80, 0x4C8EB0, 0x4C92D0, 0x4C94C0 for float)
	static PtrType Create_Unary(const std::string &name, const PtrType &child) throw()
	{
		ExprMathEvalClass<T> *node;
		if (name == "abs") {
			node = new ExprMathEvalFunction1Class<T>(child, Expr_Math_Abs);
		} else if (name == "sqrt") {
			node = new ExprMathEvalFunction1Class<T>(child, Expr_Math_Sqrt);
		} else {
			return NULL;
		}
		return Create_NEW(node);
	}
};

template <class T>
class ExprMathCompareOpsClass	// NAME 3 [ExprMathCompareOpsClass]: flag 2
{
public:
	typedef RefCountPtr<ExprMathEvalClass<T> > PtrType;
	// NOT EXACT: the client calls the binary base constructor in every branch (ours inlines it in later ones); 0x4C8920, 0x4C8CE0, 0x4C9010, 0x4C9300 for float
	static PtrType Create_Binary(const std::string &name, const PtrType &left, const PtrType &right) throw()
	{
		ExprMathEvalClass<T> *node;
		if (name == "<=") {
			node = new ExprMathEvalLessEqualClass<T>(left, right);
		} else if (name == "<") {
			node = new ExprMathEvalLessClass<T>(left, right);
		} else if (name == ">=") {
			node = new ExprMathEvalGreaterEqualClass<T>(left, right);
		} else if (name == ">") {
			node = new ExprMathEvalGreaterClass<T>(left, right);
		} else if (name == "==") {
			node = new ExprMathEvalEqualClass<T>(left, right);
		} else if (name == "!=") {
			node = new ExprMathEvalNotEqualClass<T>(left, right);
		} else {
			return NULL;
		}
		return Create_NEW(node);
	}
	// NOT EXACT: not matched yet (0x4C8B80, 0x4C8EB0, 0x4C92D0, 0x4C94C0 for float)
	static PtrType Create_Unary(const std::string &name, const PtrType &child) throw()
	{
		return NULL;
	}
};

template <class T>
class ExprMathLogicOpsClass	// NAME 3 [ExprMathLogicOpsClass]: flag 8
{
public:
	typedef RefCountPtr<ExprMathEvalClass<T> > PtrType;
	// NOT EXACT: the client calls the binary base constructor in every branch (ours inlines it in later ones); 0x4C8920, 0x4C8CE0, 0x4C9010, 0x4C9300 for float
	static PtrType Create_Binary(const std::string &name, const PtrType &left, const PtrType &right) throw()
	{
		ExprMathEvalClass<T> *node;
		if (name == "&&") {
			node = new ExprMathEvalAndClass<T>(left, right);
		} else if (name == "||") {
			node = new ExprMathEvalOrClass<T>(left, right);
		} else {
			return NULL;
		}
		return Create_NEW(node);
	}
	// NOT EXACT: not matched yet (0x4C8B80, 0x4C8EB0, 0x4C92D0, 0x4C94C0 for float)
	static PtrType Create_Unary(const std::string &name, const PtrType &child) throw()
	{
		if (name == "!") {
			return Create_NEW((ExprMathEvalClass<T> *)new ExprMathEvalNotClass<T>(child));
		}
		return NULL;
	}
};

#endif
