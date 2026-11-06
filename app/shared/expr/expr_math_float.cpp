// Earth & Beyond client (reconstruction): app\shared\expr\expr_math_float.cpp. Notes at the end of the file.
#include <math.h>
#include <stdlib.h>
#include "expr_math_float.h"
#include "expr_math_builder.h"
#include "expr_math_lexer_factory.h"
#include "expr_math_parser_factory.h"
#include "..\unknown_debug.h"


// @0x4C7BA0
RefCountPtr<ExprMathEvalClass<float> > Expr_Math_Float(unsigned int flags, const char *s)
{
	return Expr_Math_Float(flags, s, NULL);
}






// @0x4C7BE0
RefCountPtr<ExprMathEvalClass<float> > Expr_Math_Float(unsigned int flags, const char *s, std::vector<std::string> *names)
{
	FAIL_IF(!s) return NULL;

	ExprMathLexerClass *lexer = ExprMathLexerFactoryClass::Create(flags, 2, 0);
	ExprMathParserClass *parser = ExprMathParserFactoryClass::Create(flags);
	ExprMathFloatFactoryClass factory(flags);
	ExprMathBuilderClass<float> builder(lexer, parser, &factory);
	RefCountPtr<ExprMathEvalClass<float> > result = builder.Build(s, names);
	delete lexer;
	delete parser;
	return result;
}


// x ^ y, a power.
class ExprMathFloatPowerClass : public ExprMathEvalBinaryClass<float>	// NAME 3 [ExprMathFloatPowerClass]
{
public:
	ExprMathFloatPowerClass(const RefCountPtr<ExprMathEvalClass<float> > &left, const RefCountPtr<ExprMathEvalClass<float> > &right) : ExprMathEvalBinaryClass<float>(left, right) {}
	virtual float Evaluate(ExprMathContextClass<float> *context) { return pow(m_left->Evaluate(context), m_right->Evaluate(context)); }
};

// x % y, the remainder.
class ExprMathFloatModuloClass : public ExprMathEvalBinaryClass<float>	// NAME 3 [ExprMathFloatModuloClass]
{
public:
	ExprMathFloatModuloClass(const RefCountPtr<ExprMathEvalClass<float> > &left, const RefCountPtr<ExprMathEvalClass<float> > &right) : ExprMathEvalBinaryClass<float>(left, right) {}
	virtual float Evaluate(ExprMathContextClass<float> *context) { return fmod(m_left->Evaluate(context), m_right->Evaluate(context)); }
};


// @0x4C7D50
ExprMathFloatFactoryClass::ExprMathFloatFactoryClass(unsigned int flags) :
	m_flags(flags)
{
}


// NOT EXACT: 0x4C7D80 similarity 0.56: this and name in swapped registers (esi / edi), ebx not kept as 0 past ^ and %
// @0x4C7D80
ExprMathFloatFactoryClass::PtrType ExprMathFloatFactoryClass::Create_Binary(const std::string &name, const PtrType &left, const PtrType &right)
{
	if (name == "^") {
		return Create_NEW((ExprMathEvalClass<float> *)new ExprMathFloatPowerClass(left, right));
	}
	if (name == "%") {
		return Create_NEW((ExprMathEvalClass<float> *)new ExprMathFloatModuloClass(left, right));
	}
	PtrType result;
	if (m_flags & 1) {
		if (result = ExprMathArithmeticOpsClass<float>::Create_Binary(name, left, right)) {
			return result;
		}

	}
	if (m_flags & 0x10) {
		if (result = ExprMathFunctionOpsClass<float>::Create_Binary(name, left, right)) {
			return result;
		}

	}
	if (m_flags & 2) {
		if (result = ExprMathCompareOpsClass<float>::Create_Binary(name, left, right)) {
			return result;
		}

	}
	if (m_flags & 8) {
		if (result = ExprMathLogicOpsClass<float>::Create_Binary(name, left, right)) {
			return result;
		}

	}
	return NULL;
}


// @0x4C8370
ExprMathFloatFactoryClass::PtrType ExprMathFloatFactoryClass::Create_Unary(const std::string &name, const PtrType &child)
{
	PtrType result;
	if (m_flags & 1) {
		if (result = ExprMathArithmeticOpsClass<float>::Create_Unary(name, child)) {
			return result;
		}

	}
	if (m_flags & 0x10) {
		if (result = ExprMathFunctionOpsClass<float>::Create_Unary(name, child)) {
			return result;
		}

	}
	if (m_flags & 2) {
		if (result = ExprMathCompareOpsClass<float>::Create_Unary(name, child)) {
			return result;
		}

	}
	if (m_flags & 8) {
		if (result = ExprMathLogicOpsClass<float>::Create_Unary(name, child)) {
			return result;
		}

	}
	return NULL;
}


// @0x4C8690
ExprMathFloatFactoryClass::PtrType ExprMathFloatFactoryClass::Create_Constant(const std::string &text)
{
	return Create_NEW((ExprMathEvalClass<float> *)new ExprMathConstantClass<float>((float)atof(text.c_str())));
}


// @0x4C8710
ExprMathFloatFactoryClass::PtrType ExprMathFloatFactoryClass::Create_Variable(const std::string &name)
{
	return Create_NEW((ExprMathEvalClass<float> *)new ExprMathVariableClass<float>(name.c_str()));
}


/*
**	Reconstruction notes
**
**	client.exe 0x4C7B70-0x4CBAEF: the FAIL_IF '!s' (line 25) and the float factory's vtable 0xAEBBE0; the
**	templates of expr_math_eval_impl.h (FAIL_IFs at its lines 175, 190) and expr_math_builder.h
**	(G_ASSERT line 51) instantiated for float follow, with RefCountPtr's and the strings' COMDATs.
**	^ (pow) and % (fmod) are float's own operators, before the groups. The int counterpart is
**	expr_math_int.cpp (0x4CBAF0 on).
*/
