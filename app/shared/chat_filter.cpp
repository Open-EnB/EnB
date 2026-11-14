// Earth & Beyond client (reconstruction): app\shared\chat_filter.cpp (the name provisional). Notes at the end of the file.
#include "chat_filter.h"
#include <algorithm>
#include <ctype.h>

void Chat_Filter_Strip_Spaces(std::string &text);	// 0x4E17D0  // NAME 2: erases the spaces
void Chat_Filter_To_Lower(std::string &text);	// 0x4E16E0  // NAME 2: tolower over the text
bool Chat_Filter_Word_Less(const std::string &a, const std::string &b);	// 0x4E19A0  // NAME 2: the longer first, then by text

// 0x4E0840
std::string ChatFilterClass::Filter(const std::string &text)
{
	std::string filtered;
	Filter(text, filtered);
	return(filtered);
}

// 0x4E0930
ChatFilterSymbolsClass::ChatFilterSymbolsClass() :
	Symbols("~@#$%^&*+=")
{
	Shuffle();
}

// 0x4E0B30
char ChatFilterSymbolsClass::operator () ()
{
	if (Next == Symbols.end()) {
		Shuffle();
	}
	return(*Next++);
}

// 0x4E0CF0
void ChatFilterSymbolsClass::Shuffle()
{
	std::random_shuffle(Symbols.begin(), Symbols.end());
	Next = Symbols.begin();
}

// 0x4E0EB0
bool ChatWordFilterClass::Add_Word(const std::string &word)
{
	std::string lower(word);
	Chat_Filter_To_Lower(lower);
	std::vector<std::string>::iterator it = std::lower_bound(Words.begin(), Words.end(), lower, Chat_Filter_Word_Less);
	if (it != Words.end() && *it == lower) {
		return(false);
	}
	Words.insert(it, lower);
	return(true);
}

// 0x4E10B0
bool ChatWordFilterClass::Remove_Word(const std::string &word)
{
	std::string lower(word);
	Chat_Filter_To_Lower(lower);
	std::vector<std::string>::iterator it = std::lower_bound(Words.begin(), Words.end(), lower, Chat_Filter_Word_Less);
	if (it != Words.end() && *it == lower) {
		Words.erase(it);
		return(true);
	}
	return(false);
}

// 0x4E1300
// NOT EXACT: frame (the client keeps one more dword local: a dead store of find's pos, the word iterator held twice; attempts.py show 0x4E1300)
bool ChatWordFilterClass::Filter(const std::string &text, std::string &filtered)
{
	bool changed = false;
	std::string stripped(text);
	Chat_Filter_Strip_Spaces(stripped);
	Chat_Filter_To_Lower(stripped);
	for (std::vector<std::string>::iterator word = Words.begin(); word != Words.end(); ++word) {
		int pos;
		while ((pos = stripped.find(*word)) != std::string::npos) {
			std::string::iterator first = stripped.begin() + pos;
			std::string::iterator last = first + word->size();
			std::generate(first, last, ChatFilterSymbolsClass());
			changed = true;
		}
	}
	filtered = text;
	if (changed) {
		std::string::iterator f = filtered.begin();
		std::string::iterator s = stripped.begin();
		while (f != filtered.end() && s != stripped.end()) {
			if (isspace(*f)) {
				++f;
			} else {
				char c = *s;
				if (c != tolower(*f)) {
					*f = c;
				}
				++f;
				++s;
			}
		}
	}
	return(changed);
}

// 0x4E16E0
void Chat_Filter_To_Lower(std::string &text)
{
	std::transform(text.begin(), text.end(), text.begin(), tolower);
}

// 0x4E17D0
void Chat_Filter_Strip_Spaces(std::string &text)
{
	std::string::iterator it = text.begin();
	while (it != text.end()) {
		if (isspace(*it)) {
			it = text.erase(it);
		} else {
			++it;
		}
	}
}

// 0x4E19A0
bool Chat_Filter_Word_Less(const std::string &a, const std::string &b)
{
	if (a.size() < b.size()) {
		return(false);
	}
	if (b.size() < a.size()) {
		return(true);
	}
	return(a < b);
}

// 0x4E1A40
ChatFilterNodeClass::ChatFilterNodeClass() :
	Bad(false),
	Good(false)
{
}

// 0x4E1B20
ChatFilterNodeClass::~ChatFilterNodeClass()
{
	for (std::map<char, ChatFilterNodeClass *>::iterator it = Children.begin(); it != Children.end(); ++it) {
		delete (*it).second;
	}
	Children.clear();
}

// 0x4E1E20
ChatFilterNodeClass *ChatFilterNodeClass::Find_Child(char c)
{
	std::map<char, ChatFilterNodeClass *>::iterator it = Children.find(c);
	if (it != Children.end()) {
		return((*it).second);
	}
	return(NULL);
}

// 0x4E1EA0
const ChatFilterNodeClass *ChatFilterNodeClass::Find_Child(char c) const
{
	std::map<char, ChatFilterNodeClass *>::const_iterator it = Children.find(c);
	if (it != Children.end()) {
		return((*it).second);
	}
	return(NULL);
}

// 0x4E1F20
ChatFilterNodeClass *ChatFilterNodeClass::Get_Child(char c, bool bad, bool good)
{
	ChatFilterNodeClass *child = Find_Child(c);
	if (child == NULL) {
		child = new ChatFilterNodeClass;
		(*Children.insert(std::map<char, ChatFilterNodeClass *>::value_type(c, NULL)).first).second = child;
	}
	if (bad) {
		child->Bad = bad;
	}
	if (good) {
		child->Good = good;
	}
	return(child);
}

// 0x4E2040
// NOT EXACT: inline budget (the client inlines the first node whole and calls the second's _Tree constructor 0x4E2B20 and an insert 0x4E2D60 out of line; Root->Get_Child(' ', false, true) scores 0.85; attempts.py show 0x4E2040)
ChatTreeFilterClass::ChatTreeFilterClass() :
	Root(NULL)
{
	Root = new ChatFilterNodeClass;
	ChatFilterNodeClass *space = Root->Find_Child(' ');
	if (space == NULL) {
		space = new ChatFilterNodeClass;
		Root->Children[' '] = space;
	}
	space->Good = true;
}

// 0x4E22D0
ChatTreeFilterClass::~ChatTreeFilterClass()
{
	delete Root;
	Root = NULL;
}

// 0x4E2490
bool ChatTreeFilterClass::Filter(const std::string &text, std::string &filtered)
{
	bool changed = false;
	filtered = text;
	std::string::iterator it = filtered.begin();
	while (it != filtered.end()) {
		std::pair<int, bool> match = Match(it);
		if (match.second) {
			std::generate(it, it + match.first, ChatFilterSymbolsClass());
			changed = true;
		}
		it += match.first;
	}
	return(changed);
}

// 0x4E26F0
std::pair<int, bool> ChatTreeFilterClass::Match(std::string::iterator it) const
{
	std::pair<int, bool> match(1, false);
	int length = 0;
	const ChatFilterNodeClass *node = Root;
	while (node != NULL) {
		if (node->Bad || node->Good) {
			match.first = length;
			match.second = !node->Good;
		}
		const ChatFilterNodeClass *next = node->Find_Child(tolower(*it));
		while (next == NULL && (isspace(*it) || ispunct(*it))) {
			++length;
			++it;
			next = node->Find_Child(tolower(*it));
		}
		++length;
		++it;
		node = next;
	}
	return(match);
}

// 0x4E2870
bool ChatTreeFilterClass::Add_Word(const std::string &word, bool bad)
{
	ChatFilterNodeClass *node = Root;
	for (std::string::const_iterator it = word.begin(); it != word.end(); ++it) {
		node = node->Get_Child(tolower(*it), false, false);
	}
	if (bad) {
		if (!node->Bad) {
			node->Bad = true;
			return(true);
		}
	} else {
		if (node->Good) {
			return(false);
		}
		node->Good = true;
	}
	return(true);
}

// 0x4E2910
// NOT EXACT: inline budget (the client calls map::find 0x4E2F90 out of line inside the inlined Find_Child, as Get_Child does; attempts.py show 0x4E2910)
bool ChatTreeFilterClass::Remove_Word(const std::string &word, bool bad)
{
	ChatFilterNodeClass *node = Root;
	std::string::const_iterator it = word.begin();
	if (node != NULL) {
		while (it != word.end()) {
			node = node->Find_Child(tolower(*it));
			++it;
			if (node == NULL) {
				return(false);
			}
		}
		if (node != NULL) {
			if (bad) {
				if (!node->Bad) {
					return(false);
				}
				node->Bad = false;
			} else {
				if (!node->Good) {
					return(false);
				}
				node->Good = false;
			}
			return(true);
		}
	}
	return(false);
}

/*
**	Reconstruction notes (chat_filter.cpp)
**
**	* Client 0x4E0840-0x4E42B0: one object (<string>'s guarded ctype initialiser at 0x4E0810 ends the
**	  one before, 0x4E4290 ends this one). No __FILE__, RTTI or string names it: the name is
**	  provisional (D137), taken from chat_filter_util.cpp (0x4E42C0-0x4E4AF0), the next object, whose
**	  0x4E42C0 builds ChatWordFilterClass and whose class's Filter (0x4E1300) sits here.
**	* No function of either object has a caller outside them: 0x4E42C0 itself is never called.
*/
