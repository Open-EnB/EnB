// Earth & Beyond client (reconstruction): app\shared\expr\expr_math_lexer_factory.cpp. Notes at the end of the file.
#include "expr_math_lexer_factory.h"
#include "expr_lexer_impl.h"
#include "expr_lexer_match_list.h"
#include "expr_lexer_matchers.h"
#include "..\g_assert.h"


// @0x4D0230
void ExprMathLexerFactoryClass::Add_Operators(unsigned int flags, ExprLexerImplClass *lexer)
{
	FAIL_IF(!lexer) return;

	ExprLexerMatchListClass *ops = new ExprLexerMatchListClass;	// NAME 3 [ops]
	if (flags & 8) {
		ops->Add("! || &&");
	}
	if (flags & 2) {
		ops->Add("< > <= >= == !=");
	}
	if (flags & 0x10) {
		ops->Add("min max abs sqrt");
	}
	if (flags & 4) {
		ops->Add("| ^ & ~ >> <<");
	}
	if (flags & 1) {
		ops->Add("+ - * / % div");
	}
	lexer->Add_Matcher(4, ops);
}


// @0x4D0360
ExprMathLexerClass *ExprMathLexerFactoryClass::Create(unsigned int flags, int number_flags, int name_flags)
{
	ExprLexerImplClass *lexer = new ExprLexerImplClass;
	Add_Operators(flags, lexer);
	lexer->Add_Matcher(3, new ExprLexerMatchListClass("( )"));
	lexer->Add_Matcher(1, new ExprLexerNameMatchClass(name_flags));
	lexer->Add_Matcher(2, new ExprLexerNumberMatchClass(number_flags));
	lexer->Add_Matcher(0, new ExprLexerSpaceMatchClass);
	return lexer;
}


/*
**	Reconstruction notes
**
**	client.exe 0x4D0230-0x4D052F by the FAIL_IF (line 12) and the vtable 0xAEC0D4: Add_Operators 0x4D0230,
**	Create 0x4D0360, and the white-space matcher's COMDAT destructors 0x4D04E0 / 0x4D0510. Callers: 0x4C7BE0
**	(expr_math_float.cpp: flags, 2, 0) and 0x4CBB30.
*/
