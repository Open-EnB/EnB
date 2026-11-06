// Earth & Beyond client (reconstruction): app\shared\expr\expr_lexer_matchers.cpp (interim name, D137). Notes at the end of the file.
#include <ctype.h>
#include "expr_lexer_matchers.h"


// @0x4C56B0
ExprLexerNumberMatchClass::ExprLexerNumberMatchClass(int flags) :
	m_flags(flags)
{
	if (flags == 0) {
		m_flags = 1;
	}
}


// @0x4C5760
int ExprLexerNumberMatchClass::Match(const char *text)
{
	bool dot = (m_flags & 2) != 0;
	int len = 0;
	while (*text != '\0') {
		if (!isdigit(*text)) {
			if (!dot || *text != '.') {
				return len;
			}
			dot = false;
		}
		len++;
		text++;
	}
	return len;
}


// @0x4C57D0
ExprLexerNameMatchClass::ExprLexerNameMatchClass(int flags) :
	m_flags(flags)
{
}


// @0x4C5850
int ExprLexerNameMatchClass::Match(const char *text)
{
	int len = 0;
	bool dots = !(m_flags & 1);
	bool alpha = false;
	while (text[len] != '\0') {
		if (isalpha(text[len])) {
			alpha = true;
		} else if (!alpha || !isdigit(text[len])) {
			if (!dots || text[len] != '.' || text[len + 1] == '.') {
				return len;
			}
			alpha = false;
		}
		len++;
	}
	return len;
}


// @0x4C58F0
int ExprLexerSpaceMatchClass::Match(const char *text)
{
	int len = 0;
	while (*text != '\0' && isspace(*text)) {
		len++;
		text++;
	}
	return len;
}


/*
**	Reconstruction notes
**
**	client.exe 0x4C56B0-0x4C592F: no __FILE__, no assert; between expr_lexer_impl.cpp's COMDATs and
**	expr_lexer_match_list.cpp's first function (line 13 at 0x4C5930), with the vtables 0xAEBB24 / 0xAEBB3C
**	between theirs. The file name is interim (D137). The number matcher 0x4C56B0 / Match 0x4C5760 (with its
**	COMDAT destructors 0x4C56E0 / 0x4C5710 and ExprLexerMatchClass's deleting destructor 0x4C5730), the name
**	matcher 0x4C57D0 / Match 0x4C5850 (0x4C5800 / 0x4C5830) and the white-space matcher's Match 0x4C58F0 (its
**	vtable and destructors are emitted by expr_math_lexer_factory.cpp, which constructs it inline). The client
**	counts before it advances (len++ before text++), and reads the name matcher's flag into a bool first.
*/
