// Earth & Beyond client (reconstruction): app\shared\expr\expr_math_token.cpp (interim name, D137). Notes at the end of the file.
#include "expr_math_lexer.h"


const char *ExprMathTokenClass::Invalid_Text = "";


// @0x4C7620
ExprMathTokenClass::ExprMathTokenClass() :
	Type(-1),
	Text(Invalid_Text)
{
}


// @0x4C7740
ExprMathTokenClass::ExprMathTokenClass(int type, const char *text) :
	Type(type),
	Text(text)
{
}


// @0x4C7850
ExprMathTokenClass::ExprMathTokenClass(int type, const std::string &text) :
	Type(type),
	Text(text)
{
}


// @0x4C79E0
bool ExprMathTokenClass::Is_Valid() const
{
	return Type != -1 && Text != Invalid_Text;
}


// @0x4C7A60
bool ExprMathTokenClass::operator==(const ExprMathTokenClass &other) const
{
	return Type == other.Type && Text == other.Text;
}


// @0x4C7AE0
bool ExprMathTokenClass::operator!=(const ExprMathTokenClass &other) const
{
	return !(*this == other);
}


/*
**	Reconstruction notes
**
**	client.exe 0x4C7620-0x4C7B4F: no __FILE__, no assert; after expr_lexer_match_list.cpp's COMDATs and before
**	expr_math_float.cpp (0x4C7BE0, line 25), its one global 0xB7459C (a pointer to "") just before that file's
**	strings. The file name is interim (D137). The token's three constructors, Is_Valid and the two compares.
*/
