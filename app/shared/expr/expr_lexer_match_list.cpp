// Earth & Beyond client (reconstruction): app\shared\expr\expr_lexer_match_list.cpp. Notes at the end of the file.
#include <string.h>
#include <stdlib.h>
#include <vector>
#include "expr_lexer_match_list.h"
#include "..\unknown_debug.h"

// @0x4C5930
ExprLexerMatchListClass::ExprLexerMatchListClass() :
	Begin(NULL)
{
	Begin = new ExprLexerMatchNodeStruct;
	G_ASSERT(Begin);
}


// @0x4C5C20
ExprLexerMatchListClass::ExprLexerMatchListClass(const char *tokens) :
	Begin(NULL)
{
	Begin = new ExprLexerMatchNodeStruct;
	G_ASSERT(Begin);
	Add(tokens);
}


// @0x4C5EC0
ExprLexerMatchListClass::~ExprLexerMatchListClass()
{
	delete Begin;
	Begin = NULL;
}


// @0x4C6160
void ExprLexerMatchListClass::Add(const char *tokens)
{
	FAIL_IF(!tokens) return;

	char *copy = strdup(tokens);
	std::vector<char *> list;
	char *token = strtok(copy, " ");
	while (token != NULL) {
		list.push_back(token);
		token = strtok(NULL, " ");
	}
	int count = list.size();
	for (int i = 0; i < count; i++) {
		Add_Token(list[i]);
	}
	free(copy);
}











// @0x4C62C0
void ExprLexerMatchListClass::Add_Token(const char *token)
{
	FAIL_IF(!token) return;

	ExprLexerMatchNodeStruct *node = Begin;
	for (char c = *token; c != '\0'; c = *++token) {
		if (node->Children[c] == NULL) {
			ExprLexerMatchNodeStruct *child = new ExprLexerMatchNodeStruct;
			node->Children[c] = child;
			node = child;
		} else {
			node = node->Children[c];
		}
	}
	node->Terminal = true;
}












// @0x4C6530
int ExprLexerMatchListClass::Match(const char *text)
{
	int best = 0;
	int len = 0;
	ExprLexerMatchNodeStruct *node = Begin;
	for (;;) {
		if (node->Terminal) {
			best = len;
		}
		std::hash_map<char, ExprLexerMatchNodeStruct *>::iterator it = node->Children.find(*text);
		if (it == node->Children.end()) {
			return best;
		}
		node = (*it).second;
		FAIL_IF(!node) return best;
		text++;
		len++;
	}
}


/*
**	Reconstruction notes
**
**	client.exe 0x4C5930-0x4C761F by the asserts (G_ASSERT lines 13 22, FAIL_IF 38 67 108) and the vtable
**	0xAEBBD0: the two constructors 0x4C5930 / 0x4C5C20 (the node's hash_map of 100 buckets inlined), the
**	destructors 0x4C5BF0 / 0x4C5EC0, Add 0x4C6160, Add_Token 0x4C62C0, Match 0x4C6530; then the COMDATs this
**	object places: the node's destructors 0x4C6130 / 0x4C65D0 and the SGI hashtable and vector instances up to
**	0x4C75F0 (lower_bound's distance / advance). Add collects strtok's pieces with push_back (an explicit
**	insert(end(), 1, x) inlines the insert the client calls); Add_Token keys on a char local advanced in the
**	for header (`*token` re-read as the key caches 0 in ebx instead of the character in bl).
*/
