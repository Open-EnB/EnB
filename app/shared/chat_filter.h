// Earth & Beyond client (reconstruction): app\shared\chat_filter.h (the name provisional: see chat_filter.cpp's notes)
/*
**	The chat filters (client 0x4E0840-0x4E42B0, no RTTI, no assert): an abstract filter (vtable
**	0xAEC4A8), a word-list filter (vtable 0xAEC4B4, built by chat_filter_util.cpp's 0x4E42C0) and a
**	character-tree filter (vtable 0xAEC49C). A match is overwritten with the symbols '~@#$%^&*+='
**	in a random order. No caller in the client reaches any of them.
*/

#ifndef CHAT_FILTER_H
#define CHAT_FILTER_H

#include <string>
#include <vector>
#include <map>
#include <utility>

// PLACEMENT: provisional (D137): the object 0x4E0840-0x4E42B0 has no __FILE__; its classes are the ones chat_filter_util.cpp builds

class ChatFilterClass	// NAME 2: the base of chat_filter_util.cpp's filter
{
public:
	virtual ~ChatFilterClass() {}	// 0x4E2250, 0x4E2270
	virtual bool Filter(const std::string &text, std::string &filtered) = 0;
	std::string Filter(const std::string &text);	// 0x4E0840  // NAME 2: calls slot 1
};

// a source of the symbols '~@#$%^&*+=' in a random order (std::generate's generator, 0x4E3F90)
class ChatFilterSymbolsClass	// NAME 2: what it holds
{
public:
	ChatFilterSymbolsClass();	// 0x4E0930
	char operator () ();	// 0x4E0B30
	void Shuffle();	// 0x4E0CF0  // NAME 2: random_shuffle
protected:
	std::string Symbols;	// +0x00  // NAME 2: '~@#$%^&*+='
	std::string::iterator Next;	// +0x10  // NAME 2: the next symbol handed out
};

// the word list (vtable 0xAEC4B4; chat_filter_util.cpp's 0x4E42C0 fills it)
class ChatWordFilterClass : public ChatFilterClass	// NAME 2: a sorted list of words
{
public:
	virtual bool Filter(const std::string &text, std::string &filtered);	// 0x4E1300
	bool Add_Word(const std::string &word);	// 0x4E0EB0  // NAME 2: inserts in order
	bool Remove_Word(const std::string &word);	// 0x4E10B0  // NAME 2: erases
protected:
	std::vector<std::string> Words;	// +0x04 (longest first, then by text)  // NAME 2: what it holds
};

// a node of the character tree: 0x14 bytes
class ChatFilterNodeClass	// NAME 2: one character of the tree
{
public:
	ChatFilterNodeClass();	// 0x4E1A40
	~ChatFilterNodeClass();	// 0x4E1B20
	ChatFilterNodeClass *Find_Child(char c);	// 0x4E1E20  // NAME 2: map find
	const ChatFilterNodeClass *Find_Child(char c) const;	// 0x4E1EA0  // NAME 2: map find
	ChatFilterNodeClass *Get_Child(char c, bool bad, bool good);	// 0x4E1F20  // NAME 2: finds or adds

	bool Bad;	// +0x00 (a word to filter ends here)  // NAME 2: Add_Word(word, true) sets it
	bool Good;	// +0x01 (a word to let through ends here)  // NAME 2: Add_Word(word, false) sets it
	std::map<char, ChatFilterNodeClass *> Children;	// +0x04  // NAME 2: what it holds
};

// the character tree (vtable 0xAEC49C)
class ChatTreeFilterClass : public ChatFilterClass	// NAME 2: a tree of characters
{
public:
	ChatTreeFilterClass();	// 0x4E2040
	virtual ~ChatTreeFilterClass();	// 0x4E22D0, 0x4E22A0
	virtual bool Filter(const std::string &text, std::string &filtered);	// 0x4E2490
	bool Add_Word(const std::string &word, bool bad);	// 0x4E2870  // NAME 2: as ChatWordFilterClass's
	bool Remove_Word(const std::string &word, bool bad);	// 0x4E2910  // NAME 2: as ChatWordFilterClass's
protected:
	std::pair<int, bool> Match(std::string::iterator it) const;	// 0x4E26F0  // NAME 2: the match's length and whether to filter it
	ChatFilterNodeClass *Root;	// +0x04  // NAME 2: what it holds
};

#endif // CHAT_FILTER_H
