// Earth & Beyond client (reconstruction): app\shared\expr\expr_math_parser_factory.cpp. Notes at the end of the file.
#include <string.h>
#include "expr_math_parser_factory.h"
#include "expr_math_parser_impl.h"
#include "expr_math_tree.h"
#include "..\g_assert.h"


// The parser the factory builds: a primary may also be a token of type 5. 0x14 bytes, vtable 0xAEC0E0.
class ExprMathFactoryParserClass : public ExprMathParserImplClass	// NAME 3 [ExprMathFactoryParserClass]
{
public:
	virtual ExprMathTreeClass *Parse_Primary(ExprMathLexerClass *lex);	// 0x4D07E0
};














// @0x4D0560
void ExprMathParserFactoryClass::Add_Operators(unsigned int flags, ExprMathParserImplClass *parser)
{
	FAIL_IF(!parser) return;

	char prefix[32];	// NAME 3 [prefix]: the prefix operators, one group at level 0
	prefix[0] = '\0';
	if (flags & 8) {
		parser->Add_Group(1, "||");
		parser->Add_Group(1, "&&");
		strcat(prefix, "! ");
	}
	if (flags & 2) {
		parser->Add_Group(1, "< > <= >= == !=");
	}
	if (flags & 0x10) {
		parser->Add_Group(1, "min max");
		strcat(prefix, "abs sqrt ");
	}
	if (flags & 4) {
		parser->Add_Group(1, "|");
		parser->Add_Group(1, "^");
		parser->Add_Group(1, "&");
		parser->Add_Group(1, "<< >>");
		strcat(prefix, "~ ");
	}
	if (flags & 1) {
		parser->Add_Group(1, "+ -");
		parser->Add_Group(1, "* / % div");
		strcat(prefix, "+ - ");
	}
	if (strlen(prefix) != 0) {
		parser->Add_Group(0, prefix);
	}
}


// @0x4D0790
ExprMathParserClass *ExprMathParserFactoryClass::Create(unsigned int flags)
{
	ExprMathFactoryParserClass *parser = new ExprMathFactoryParserClass;
	Add_Operators(flags, parser);
	return parser;
}


// @0x4D07E0
ExprMathTreeClass *ExprMathFactoryParserClass::Parse_Primary(ExprMathLexerClass *lex)
{
	ExprMathTokenClass token = Peek(lex);
	if (token.Type == 2 || token.Type == 1 || token.Type == 5) {
		ExprMathTreeClass *leaf = new ExprMathTreeClass(token);
		lex->Next();
		return leaf;
	}
	return NULL;
}


/*
**	Reconstruction notes
**
**	client.exe 0x4D0560-0x4D0A7F by the FAIL_IF (line 32) and the vtable 0xAEC0E0: Add_Operators 0x4D0560,
**	Create 0x4D0790 (the parser's constructor inlined), the parser's Parse_Primary 0x4D07E0 (as
**	ExprMathParserImplClass's 0x4D18A0, plus token type 5), its destructors 0x4D0920 / 0x4D0950, then the
**	COMDATs this object places: ~ExprMathParserClass 0x4D0A20 / deleting 0x4D0A40 and
**	allocator<string>::deallocate 0x4D0A70. The parser class is declared here only: no other file
**	references its vtable.
*/
