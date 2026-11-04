// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\nlp.h (D24: by convention from nlp.cpp's __FILE__)
**
**	The avatar's natural-language processor (PlayerNLPClass, its lexicon).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef NLP_H	// NAME 3: by convention from nlp.cpp
#define NLP_H

#include <string.h>
#include "unknown_personality.h"

class Unknown_ChatStreamClass;

void Unknown_78E2F0(const char *format, ...);	// 0x78E2F0: the NLP's debug text (formatted, then dropped)  // NAME 1: neutral (D10)

// One lexicon phrase (0x120 bytes, 0x78E330 news them): the phrase and its section's gesture tags by
// the phrase's place in the text
struct Unknown_NLPPhraseStruct	// NAME 1: neutral (D10)
{
	Unknown_NLPPhraseStruct() { memset(Phrase, 0, sizeof(Phrase)); memset(Context, 0, sizeof(Context)); }
	char Phrase[0x80];			// +0x00 'Phrase%d'  // NAME 2: the INI key
	char Context[5][0x20];		// +0x80 'ContextBegin', 'ContextMiddle', 'ContextEnd', 'ContextAlone', 'ContextAlways'  // NAME 2: the INI keys
};

// nlp\lexiconNN.ini's phrases, sorted, indexed by first letter (0x80C bytes)
class NLPLexiconClass	// NAME 2: the client's text 'NLPLexicon not initialized!'
{
public:
	NLPLexiconClass(const char *filename);			// 0x78E330
	~NLPLexiconClass();								// 0x78E8A0
	void Unknown_78E8F0();							// 0x78E8F0: sort and index  // NAME 1: neutral (D10)
	Unknown_NLPPhraseStruct *Unknown_78E980(const char *text);	// 0x78E980: the phrase text starts with  // NAME 1: neutral (D10)

	bool Initialized;								// +0x00  // NAME 2: 'NLPLexicon not initialized!'
	Unknown_NLPPhraseStruct *Phrases;				// +0x04  // NAME 2: what it holds
	int PhraseCount;								// +0x08  // NAME 2: what it holds
	struct IndexStruct { int First; int Count; };	// NAME 2: what it holds
	IndexStruct Index[256];	// +0x0C by tolower(first letter)  // NAME 2: what it holds
};

// nlp.cpp's class (0x14 bytes, vtable 0xB04A2C): the avatar's (+0xF4)
class PlayerNLPClass	// NAME 2: the client's assert 'PlayerNLP != 0'
{
public:
	PlayerNLPClass() : Initialized(false), Unknown08(-1), Personality(NULL), Lexicon(NULL) {}	// 0x78EB50
	~PlayerNLPClass() { Unknown_78ECC0(); }			// 0x78EB80
	virtual bool Unknown_78EC00(int personality, int nlp);	// +0x00 0x78EC00: the personality's data, nlp\lexicon%02d.ini  // NAME 1: neutral (D10)
	virtual void Unknown_78F690(const char *text, int len, int *mood, Unknown_ChatStreamClass *stream);	// +0x04 0x78F690: parses text into the stream  // NAME 1: neutral (D10)
	virtual void Unknown_78ECC0();					// +0x08 0x78ECC0: deletes the lexicon  // NAME 1: neutral (D10)
	virtual const char *Unknown_78ED30(const char *text, const char *end);	// +0x0C 0x78ED30: the next word (NULL: none)  // NAME 1: neutral (D10)
	int Unknown_78EE00(int value, bool sitting);	// 0x78EE00: a talk animation's number  // NAME 1: neutral (D10)
	int Unknown_78EEF0(const char *text, int len);	// 0x78EEF0: the talk animation for a phrase  // NAME 1: neutral (D10)
	void Unknown_78F050(const char *text, int len, NLPLexiconClass *lexicon, int *mood, Unknown_ChatStreamClass *stream);	// 0x78F050  // NAME 1: neutral (D10)
	int Unknown_78F640(const char *tag);			// 0x78F640: the body animation number by tag  // NAME 1: neutral (D10)

	bool Initialized;								// +0x04  // NAME 2: 'NLP not intialized!'
	int Unknown08;									// +0x08 (-1)  // NAME 1: neutral (D10)
	Unknown_78F700Class *Personality;				// +0x0C  // NAME 2: 'Personality's body anim is missing'
	NLPLexiconClass *Lexicon;						// +0x10  // NAME 2: 'NLP error: null lexicon!'
};

#endif
