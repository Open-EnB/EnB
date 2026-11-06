// Earth & Beyond client (reconstruction): app\shared\expr\expr_math_int.cpp. Notes at the end of the file.
#include <stdlib.h>
#include "expr_math_int.h"
#include "expr_math_builder.h"
#include "expr_math_lexer_factory.h"
#include "expr_math_parser_factory.h"
#include "..\unknown_debug.h"


// @0x4CBAF0
RefCountPtr<ExprMathEvalClass<int> > Expr_Math_Int(unsigned int flags, const char *s)
{
	return Expr_Math_Int(flags, s, NULL);
}




// @0x4CBB30
RefCountPtr<ExprMathEvalClass<int> > Expr_Math_Int(unsigned int flags, const char *s, std::vector<std::string> *names)
{
	FAIL_IF(!s) return NULL;

	ExprMathLexerClass *lexer = ExprMathLexerFactoryClass::Create(flags, 1, 0);
	ExprMathParserClass *parser = ExprMathParserFactoryClass::Create(flags);
	ExprMathIntFactoryClass factory(flags);
	ExprMathBuilderClass<int> builder(lexer, parser, &factory);
	RefCountPtr<ExprMathEvalClass<int> > result = builder.Build(s, names);
	delete lexer;
	delete parser;
	return result;
}


// x % y, the remainder.
class ExprMathIntModuloClass : public ExprMathEvalBinaryClass<int>	// NAME 3 [ExprMathIntModuloClass]
{
public:
	ExprMathIntModuloClass(const RefCountPtr<ExprMathEvalClass<int> > &left, const RefCountPtr<ExprMathEvalClass<int> > &right) : ExprMathEvalBinaryClass<int>(left, right) {}
	virtual int Evaluate(ExprMathContextClass<int> *context) { return m_left->Evaluate(context) % m_right->Evaluate(context); }
};


// @0x4CBCA0
ExprMathIntFactoryClass::ExprMathIntFactoryClass(unsigned int flags) :
	m_flags(flags)
{
}


// NOT EXACT: 0x4CBCD0 similarity 0.77: float's register choice, and the client inlines more of the groups' assignments
// @0x4CBCD0
ExprMathIntFactoryClass::PtrType ExprMathIntFactoryClass::Create_Binary(const std::string &name, const PtrType &left, const PtrType &right)
{
	if (name == "%") {
		return Create_NEW((ExprMathEvalClass<int> *)new ExprMathIntModuloClass(left, right));
	}
	PtrType result;
	if (m_flags & 1) {
		if (result = ExprMathArithmeticOpsClass<int>::Create_Binary(name, left, right)) {
			return result;
		}

	}
	if (m_flags & 4) {
		if (result = ExprMathBitwiseOpsClass<int>::Create_Binary(name, left, right)) {
			return result;
		}

	}
	if (m_flags & 0x10) {
		if (result = ExprMathFunctionOpsClass<int>::Create_Binary(name, left, right)) {
			return result;
		}

	}
	if (m_flags & 2) {
		if (result = ExprMathCompareOpsClass<int>::Create_Binary(name, left, right)) {
			return result;
		}

	}
	if (m_flags & 8) {
		if (result = ExprMathLogicOpsClass<int>::Create_Binary(name, left, right)) {
			return result;
		}

	}
	return NULL;
}


// NOT EXACT: 0x4CC120 similarity 0.78: the client inlines more of the five groups' assignments (float's form, 0x4C8370, is exact)
// @0x4CC120
ExprMathIntFactoryClass::PtrType ExprMathIntFactoryClass::Create_Unary(const std::string &name, const PtrType &child)
{
	PtrType result;
	if (m_flags & 1) {
		if (result = ExprMathArithmeticOpsClass<int>::Create_Unary(name, child)) {
			return result;
		}

	}
	if (m_flags & 4) {
		if (result = ExprMathBitwiseOpsClass<int>::Create_Unary(name, child)) {
			return result;
		}

	}
	if (m_flags & 0x10) {
		if (result = ExprMathFunctionOpsClass<int>::Create_Unary(name, child)) {
			return result;
		}

	}
	if (m_flags & 2) {
		if (result = ExprMathCompareOpsClass<int>::Create_Unary(name, child)) {
			return result;
		}

	}
	if (m_flags & 8) {
		if (result = ExprMathLogicOpsClass<int>::Create_Unary(name, child)) {
			return result;
		}

	}
	return NULL;
}


// @0x4CC500
ExprMathIntFactoryClass::PtrType ExprMathIntFactoryClass::Create_Constant(const std::string &text)
{
	return Create_NEW((ExprMathEvalClass<int> *)new ExprMathConstantClass<int>((int)atol(text.c_str())));
}


// @0x4CC580
ExprMathIntFactoryClass::PtrType ExprMathIntFactoryClass::Create_Variable(const std::string &name)
{
	return Create_NEW((ExprMathEvalClass<int> *)new ExprMathVariableClass<int>(name.c_str()));
}


/*
**	Reconstruction notes
**
**	client.exe 0x4CBAF0-0x4D022F: the FAIL_IF '!s' (line 22) and the int factory's vtable 0xAEBDE4; the
**	templates of expr_math_eval_impl.h and expr_math_builder.h instantiated for int follow (Abs 0x4D0180,
**	Sqrt 0x4D01A0 and the building visitor's destructor 0x4D01D0 last). % is int's own operator, before
**	the groups; ^ is the bitwise group's. The float counterpart is expr_math_float.cpp.
*/
