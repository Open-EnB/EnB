// Earth & Beyond client (reconstruction): app\shared\expr\expr_lexer_impl.cpp. Notes at the end of the file.
#include "expr_lexer_impl.h"
#include "..\g_assert.h"


typedef std::vector<std::pair<int, ExprLexerMatchClass *> > ExprLexerMatchVector;	// NAME 3 [ExprLexerMatchVector]


// @0x4C4980
ExprLexerImplClass::ExprLexerImplClass() :
	m_source(""),
	m_cursor(NULL)
{
	m_cursor = m_source.c_str();
}


// @0x4C4B40
ExprLexerImplClass::~ExprLexerImplClass()
{
	for (ExprLexerMatchVector::iterator it = m_matchers.begin(); it != m_matchers.end(); it++) {
		delete it->second;
	}
	m_matchers.clear();
}




// @0x4C4CF0
void ExprLexerImplClass::Add_Matcher(int type, ExprLexerMatchClass *m)
{

	FAIL_IF(!m) return;
	std::pair<int, ExprLexerMatchClass *> entry;
	entry.first = type;
	entry.second = m;
	m_matchers.push_back(entry);
}


// @0x4C4F60
ExprMathTokenClass ExprLexerImplClass::Get_Token()
{
	return m_token;
}


// @0x4C50F0
bool ExprLexerImplClass::Has_Token()
{
	return m_token.Is_Valid();
}


// @0x4C5110
bool ExprLexerImplClass::Has_Error()
{
	return !m_token.Is_Valid() && *m_cursor != '\0';
}


// @0x4C5150
void ExprLexerImplClass::Next()
{
	if (m_cursor == NULL || *m_cursor == '\0') {
		m_token = ExprMathTokenClass();
		return;
	}

	int best = 0;
	int type = 0;
	for (ExprLexerMatchVector::iterator it = m_matchers.begin(); it != m_matchers.end(); it++) {
		int len = it->second->Match(m_cursor);
		if (len > best) {
			type = it->first;
			best = len;
		}
	}

	if (best != 0) {
		m_token = ExprMathTokenClass(type, std::string(m_cursor, best));
	} else {
		m_token = ExprMathTokenClass();
	}
	m_cursor += best;
}










// @0x4C5410
void ExprLexerImplClass::Set_Source(const char *source)
{
	FAIL_IF(!source) return;
	m_source = source;
	m_cursor = m_source.c_str();
	Next();
}


// @0x4C5530
void ExprLexerImplClass::Reset()
{
	m_cursor = m_source.c_str();
	Next();
}


/*
**	Reconstruction notes
**
**	client.exe 0x4C4980-0x4C569F by the FAIL_IFs (lines 34, 101) and the vtable 0xAEBADC: the constructor 0x4C4980,
**	the destructors 0x4C4B10 / 0x4C4B40, Add_Matcher 0x4C4CF0, Get_Token 0x4C4F60, Has_Token 0x4C50F0, Has_Error
**	0x4C5110, Next 0x4C5150, Set_Source 0x4C5410, Reset 0x4C5530; the COMDATs this object places:
**	~ExprMathLexerClass 0x4C4AC0 / deleting 0x4C4AE0, ~ExprMathTokenClass 0x4C4CA0 and the
**	vector<pair<int, ExprLexerMatchClass *> > instances 0x4C5560-0x4C5650. Add_Matcher's pair is built by
**	default and assigned (the client inlines all of insert's size() reads; a pair constructed from the two
**	arguments, make_pair or a 3-argument insert leave the last one out of line). Next's non-empty match comes
**	first (`best != 0`; `best == 0` first, or `best > 0`, swap the blocks).
*/
