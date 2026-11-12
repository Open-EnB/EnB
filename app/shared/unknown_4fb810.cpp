// Earth & Beyond client (reconstruction): app\shared\unknown_4fb810.cpp. Notes at the end of the file.
#include "unknown_4fac20.h"
#include "..\..\wwlib\nstrdup.h"
#include <algorithm>
#include <ctype.h>
#include <string.h>

// NOT EXACT: the client's instructions with other registers (explain.py: shape, mostly registers; 5 rows remain
// once registers are masked): the inlined String_Get_Quoted keeps its cursors elsewhere.
// 0x4FB810: the next token of [first, last): a word, or a "quoted string" when the first character after the
// white space is '"'. next receives where to go on (after the token and the white space behind it)
bool String_Get_Token(const char *first, const char *last, std::string *token, const char **next)
{
	if (first == last) {
		return false;
	}
	first = String_Skip_Whitespace(first, last);
	if (*first == '"') {
		if (!String_Get_Quoted(first, last, token, &first)) {
			return false;
		}
	} else {
		const char *end = String_Find_Whitespace(first, last);
		if (first == end) {
			return false;
		}
		if (token != NULL) {
			token->assign(first, end);
		}
		first = end;
	}
	if (next != NULL) {
		*next = String_Skip_Whitespace(first, last);
	}
	return true;
}

// 0x4FBA70: how many tokens String_Get_Token finds in [first, last)
int String_Count_Tokens(const char *first, const char *last)
{
	int count = 0;
	const char *cursor = first;
	while (String_Get_Token(cursor, last, NULL, &cursor)) {
		count++;
	}
	return count;
}

// NOT EXACT: the client keeps first in its stack home and 0xFF in bl through the inlined replace(); here first
// sits in ebx (explain.py: shape, 35 masked rows). docs/attempts/0x4FBAE0.
// 0x4FBAE0: the next "quoted string" of [first, last), without its quotes; next receives the character after the
// closing '"'. False when there is no pair of quotes
bool String_Get_Quoted(const char *first, const char *last, std::string *token, const char **next)
{
	first = std::find(first, last, '"');
	if (first == last) {
		return false;
	}
	++first;
	const char *end = std::find(first, last, '"');
	if (end == last) {
		return false;
	}
	if (token != NULL) {
		token->assign(first, end);
	}
	if (next != NULL) {
		*next = end + 1;
	}
	return true;
}

// 0x4FBD50: strtok over a copy of text; every piece is appended to list
void String_Tokenize(const char *text, const char *delimiters, std::vector<std::string> &list)
{
	char *copy = nstrdup(text);
	for (char *token = strtok(copy, delimiters); token != NULL; token = strtok(NULL, delimiters)) {
		list.push_back(std::string(token));
	}
	delete [] copy;
}

// 0x4FBEC0: the same with at most count pieces: the last one is the rest of the text, delimiters included
void String_Tokenize(const char *text, const char *delimiters, unsigned int count, std::vector<std::string> &list)
{
	if (count == 0) {
		return;
	}
	if (count == 1) {
		list.push_back(std::string(text));
		return;
	}
	char *copy = nstrdup(text);
	char *token = strtok(copy, delimiters);
	while (token != NULL && count > 2) {
		list.push_back(std::string(token));
		token = strtok(NULL, delimiters);
		count--;
	}
	if (token != NULL) {
		list.push_back(std::string(token));
		token = strtok(NULL, "");
		if (token != NULL) {
			list.push_back(std::string(token));
		}
	}
	delete [] copy;
}

// 0x4FC270: the first character of [first, last) that is not white space
const char *String_Skip_Whitespace(const char *first, const char *last)
{
	while (first != last && isspace(*first)) {
		first++;
	}
	return first;
}

// 0x4FC2B0: the first character of [first, last) that is white space
const char *String_Find_Whitespace(const char *first, const char *last)
{
	while (first != last && !isspace(*first)) {
		first++;
	}
	return first;
}

// 0x4FC2F0: string cut at every separator into list (cleared first); two separators in a row give an empty
// string, an empty string gives an empty list
void String_Split(std::vector<std::string> &list, const std::string &string, const std::string &separator)
{
	list.clear();
	if (string.size() == 0) {
		return;
	}
	std::string::size_type pos = 0;
	do {
		std::string::size_type at = string.find(separator, pos);
		if (at == std::string::npos) {
			list.push_back(string.substr(pos));
			return;
		}
		if (at == pos) {
			list.push_back(std::string(""));
		} else {
			list.push_back(string.substr(pos, at - pos));
		}
		pos = at + separator.size();
	} while (pos != string.size());
}

/*
**	Reconstruction notes
**
**	client.exe 0x4FB810-0x4FCBC0: the object after unknown_4fac20.cpp's (closed by its guarded <string> static
**	initialiser 0x4FB7E0) and before symbol_substitution.cpp (0x4FCBD0). Its own functions end with String_Split
**	(0x4FC2F0-0x4FC4BA); behind them sit the template copies it emits, string::replace(iterator, iterator, const char *,
**	const char *) 0x4FC540, vector<string>::erase(iterator, iterator) 0x4FC7C0 and string::replace(size_type, size_type,
**	size_type, char) 0x4FC990, then its guarded <string> initialiser 0x4FCBA0. No __FILE__ and no string names the
**	file: a neutral name (D10), the placement provisional (D137).
**
**	Byte-exact: String_Count_Tokens, both String_Tokenize, String_Skip_Whitespace, String_Find_Whitespace and
**	String_Split. NOT EXACT: String_Get_Token and String_Get_Quoted (the same instructions, other registers).
**
**	What the bytes say about the source: the functions are defined in this order, the small ones after their
**	callers, and still inline into them (String_Get_Token holds both skips and String_Get_Quoted; neither skip has a
**	caller of its own). String_Get_Quoted is std::find twice and assign(first, end), which is replace(begin(), end(),
**	first, end): wholly inline in String_Get_Quoted, one layer out of line (0x4FC990) in String_Get_Token's word
**	branch and two layers (0x4FC540) in its quoted branch. String_Count_Tokens walks with a local cursor (the client
**	stores first into it before the first call). The counted String_Tokenize is one loop "token && count > 2", then
**	the rest of the text through strtok(NULL, ""). String_Split pushes std::string("") for an empty piece and
**	substr() for the others, and stops when the position reaches the string's size (so a trailing separator adds no
**	empty piece).
*/
