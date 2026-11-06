// Earth & Beyond client (reconstruction): e:\build\beta\G\app\Client\nlp.cpp (the asserts' __FILE__)
// Notes at the end of the file.
#include "nlp.h"
#include "ini.h"
#include "nstrdup.h"
#include "random.h"
#include "..\shared\g_assert.h"
#include "..\packets\pkmessagechatstream.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

extern RandomClass Unknown_BDBB84;	// NAME 1 [Unknown_BDBB84]: neutral (D10), as eff_o.cpp's

// 0xB9D668: the talk animations' levels (the first one above the value; 5 past the last)
static const char Unknown_B9D668[5] = { 7, 15, 30, 45, 60 };	// NAME 1 [Unknown_B9D668]: neutral (D10)

// 0x78E190: the path of fname in dir, unless fname already holds dir
// NOT EXACT: functional for POC (D64), order-only (the same instructions in another order)
void Unknown_78E190(char *path, const char *dir, const char *fname, const char *ext)	// NAME 1: neutral (D10)
{
	if (dir != NULL) {
		char buffer[256];
		const char *d = dir;
		if (dir[strlen(dir) - 1] != '\\') {
			strcpy(buffer, dir);
			strcat(buffer, "\\");
			d = buffer;
		}
		if (fname != NULL && strstr(fname, d) != NULL) {
			_makepath(path, NULL, NULL, fname, ext);
			return;
		}
	}
	_makepath(path, NULL, dir, fname, ext);
}

// 0x78E2C0
void Unknown_78E2C0(char *path, const char *fname)	// NAME 1: neutral (D10)
{
	_makepath(path, NULL, NULL, fname, NULL);
}

// 0x78E2F0
void Unknown_78E2F0(const char *format, ...)
{
	char buffer[2048];
	va_list va;
	va_start(va, format);
	vsprintf(buffer, format, va);
	va_end(va);
}

// 0x78E870
static int __cdecl Unknown_78E870(const void *a, const void *b)	// NAME 1: neutral (D10)
{
	return(_stricmp((const char *)a, (const char *)b));
}

// 0x78E330
// NOT EXACT: functional for POC (D64), inline-budget (the client calls 0x78E190 and 0x78E8F0; ours inlines them)
NLPLexiconClass::NLPLexiconClass(const char *filename)
{
	Initialized = false;
	Phrases = NULL;
	PhraseCount = 0;
	char path[260];
	Unknown_78E190(path, "nlp\\", filename, NULL);
	INIClass ini(path);
	int lexicons = 0;
	for (;;) {
		char section[64];
		sprintf(section, "LEXICON-%d", lexicons);
		if (!ini.Section_Present(section)) {
			break;
		}
		for (int p = 0; ; p++) {
			char entry[64];
			sprintf(entry, "Phrase%d", p);
			if (!ini.Is_Present(section, entry)) {
				break;
			}
			PhraseCount++;
		}
		lexicons++;
	}
	Unknown_NLPPhraseStruct *phrase = new Unknown_NLPPhraseStruct[PhraseCount];
	Phrases = phrase;
	for (int l = 0; l < lexicons; l++) {
		char section[64];
		char entry[64];
		char buffer[256];
		sprintf(section, "LEXICON-%d", l);
		for (int p = 0; ; p++) {
			sprintf(entry, "Phrase%d", p);
			if (!ini.Is_Present(section, entry)) {
				break;
			}
			ini.Get_String(section, entry, "", buffer, 0xFF);
			strcpy(phrase->Phrase, buffer);
			ini.Get_String(section, "ContextBegin", "", buffer, 0xFF);
			strcpy(phrase->Context[0], buffer);
			ini.Get_String(section, "ContextMiddle", "", buffer, 0xFF);
			strcpy(phrase->Context[1], buffer);
			ini.Get_String(section, "ContextEnd", "", buffer, 0xFF);
			strcpy(phrase->Context[2], buffer);
			ini.Get_String(section, "ContextAlone", "", buffer, 0xFF);
			strcpy(phrase->Context[3], buffer);
			ini.Get_String(section, "ContextAlways", "", buffer, 0xFF);
			strcpy(phrase->Context[4], buffer);
			phrase++;
		}
	}
	Unknown_78E8F0();
	Initialized = true;
	Unknown_78E2F0("------------------------------------------------------\n");
	Unknown_78E2F0("INITNLP: lexicon loaded:\n");
}

// 0x78E8A0
NLPLexiconClass::~NLPLexiconClass()
{
	if (Initialized) {
		if (Phrases) {
			delete [] Phrases;
			Phrases = NULL;
		}
		PhraseCount = 0;
		memset(Index, 0, sizeof(Index));
		Initialized = false;
	}
}

// 0x78E8F0
void NLPLexiconClass::Unknown_78E8F0()
{
	qsort(Phrases, PhraseCount, sizeof(Unknown_NLPPhraseStruct), Unknown_78E870);
	memset(Index, 0, sizeof(Index));
	Unknown_NLPPhraseStruct *phrase = Phrases;
	for (int i = 0; i < PhraseCount; i++, phrase++) {
		IndexStruct *index = &Index[tolower(phrase->Phrase[0])];
		if (index->Count == 0) {
			index->First = i;
		}
		index->Count++;
	}
}

// 0x78E980
Unknown_NLPPhraseStruct *NLPLexiconClass::Unknown_78E980(const char *text)
{
	if (!Initialized) {
		Unknown_78E2F0("NLPLexicon not initialized!\n");
		return(NULL);
	}
	if (text == NULL || *text == '\0') {
		Unknown_78E2F0("NLP error: null phrase!\n");
		return(NULL);
	}
	if (Phrases == NULL || PhraseCount == 0) {
		Unknown_78E2F0("Null or empty lexicon!\n");
		return(NULL);
	}
	int letter = (char)tolower(*text);
	if (letter >= 0 && letter < 256) {
		IndexStruct *index = &Index[letter];
		if (index->Count == 0) {
			return(NULL);
		}
		Unknown_NLPPhraseStruct *phrase = &Phrases[index->First + index->Count - 1];
		for (int i = 0; i < index->Count; i++, phrase--) {
			int len = strlen(phrase->Phrase);
			if (_strnicmp(text, phrase->Phrase, len) == 0) {
				char next = text[len];
				tolower(next);	// the client drops the result
				if (next < 'a' || next > 'z') {
					Unknown_78E2F0("NLP: phrase found: phrase=%s!\n", phrase->Phrase);
					return(phrase);
				}
				Unknown_78E2F0("NLP: ignore half word comparison!\n");
			}
		}
	}
	return(NULL);
}

// 0x78EB50
PlayerNLPClass::PlayerNLPClass()
{
	Initialized = false;
	Unknown08 = -1;
	Personality = NULL;
	Lexicon = NULL;
}

// 0x78EB80
PlayerNLPClass::~PlayerNLPClass()
{
	Unknown_78ECC0();
}

// 0x78EC00
bool PlayerNLPClass::Unknown_78EC00(int personality, int nlp)
{
	Unknown_78ECC0();
	Personality = Unknown_BE856C->Unknown_790D40(personality);
	char name[64];
	sprintf(name, "lexicon%02d.ini", nlp);
	Lexicon = new NLPLexiconClass(name);
	Initialized = true;
	return(true);
}

// 0x78ECC0
void PlayerNLPClass::Unknown_78ECC0()
{
	if (Lexicon) {
		delete Lexicon;
		Lexicon = NULL;
	}
	Initialized = false;
}

// 0x78ED30
// NOT EXACT: functional for POC (D64), shape (the loops' guards: ours merges the second guard into its loop)
const char *PlayerNLPClass::Unknown_78ED30(const char *text, const char *end)
{
	if (!Initialized) {
		Unknown_78E2F0("NLP not intialized!\n");
		return(NULL);
	}
	if (*text == '\0') {
		return(NULL);
	}
	const char *p = text;
	while (p < end) {
		char c = tolower(*p);
		if (c == '\0' || c == ' ' || c == '.' || c == '!' || c == '?' || c == ',') {
			break;
		}
		p++;
	}
	if (*p != '\0' && p < end) {
		while (*p != '\0') {
			char c = tolower(*p);
			if (c != '\0' && c != ' ') {
				break;
			}
			if (++p >= end) {
				break;
			}
		}
	}
	if (p != text && *p != '\0') {
		return(p);
	}
	return(NULL);
}



































































































































// 0x78EE00
int PlayerNLPClass::Unknown_78EE00(int value, bool sitting)
{
	if (Personality == NULL) {
		return(0);
	}
	int level = 5;
	for (int i = 0; i < 5; i++) {
		if (value < Unknown_B9D668[i]) {
			level = i + 1;
			break;
		}
	}
	char name[64];
	if (sitting) {
		sprintf(name, "sit_talk%d", level);
	} else if (Unknown_BDBB84(0, 10) < 3) {
		sprintf(name, "talk%d", level);
	} else {
		sprintf(name, "idle_talk%d", level);
	}
	Unknown_78F700Class::BAnimStruct *body_anim = Personality->Find_BAnim(name);
	G_ASSERT(body_anim);
	return(body_anim->AnimNumber);
}











// 0x78EEF0
int PlayerNLPClass::Unknown_78EEF0(const char *text, int len)
{
	if (Personality == NULL) {
		return(0);
	}
	char *copy = new char[len + 1];
	strncpy(copy, text, len);
	copy[len] = '\0';
	int delay = Personality->Get_Speech_Delay();
	int phonemes[256];
	int value = (Unknown_BE856C->Phonemes.Unknown_791400(copy, phonemes, 0xFF) + 3) * delay;
	delete [] copy;
	int level = 5;
	for (int i = 0; i < 5; i++) {
		if (value < Unknown_B9D668[i]) {
			level = i + 1;
			break;
		}
	}
	char name[64];
	if (Unknown_BDBB84(0, 10) < 3) {
		_snprintf(name, sizeof(name), "talk%d", level);
	} else {
		_snprintf(name, sizeof(name), "idle_talk%d", level);
	}
	Unknown_78F700Class::BAnimStruct *body_anim = Personality->Find_BAnim(name);
	G_ASSERT(body_anim);
	return(body_anim->AnimNumber);
}

// 0x78F050: the text into the stream's blocks: punctuation ends a phrase (1, its talk animation),
// '/name' is a body animation (2) or a mood (0, *mood set), a lexicon phrase gets its context's gesture
// NOT EXACT: functional for POC (D64), shape (a merged or split tail)
void PlayerNLPClass::Unknown_78F050(const char *text, int len, NLPLexiconClass *lexicon, int *mood, Unknown_ChatStreamClass *stream)
{
	if (text == NULL) {
		Unknown_78E2F0("NLP error: null input!\n");
		return;
	}
	if ((int)strlen(text) <= len) {
		len = strlen(text);
	}
	if (!Initialized || Lexicon == NULL) {
		stream->Unknown_4C45B0(1, 0, text, len);
		if (!Initialized) {
			Unknown_78E2F0("NLP error: not initialized!\n");
		}
		if (Lexicon == NULL) {
			Unknown_78E2F0("NLP error: null lexicon!\n");
		}
		return;
	}
	int count = 0;
	const char *end = text + len;
	const char *start = text;
	const char *p = text;
	if (len <= 0) {
		return;
	}
	do {
		char c = *p;
		if (c == '.' || c == '!' || c == '?' || c == ',') {
			char *phrase = new char[count + 2];
			strncpy(phrase, start, count);
			phrase[count] = '\0';
			strncat(phrase, p, 1);
			if (count == 0) {
				stream->Unknown_4C45B0(1, 0, phrase, 1);
			} else {
				stream->Unknown_4C45B0(1, (unsigned char)Unknown_78EEF0(start, count), phrase, count + 1);
			}
			delete [] phrase;
			p++;
			count = 0;
			start = p;
		} else if (c == '/') {
			const char *space = strchr(p, ' ');
			int cmdlen = space ? (space - p) + 1 : strlen(p);
			int n = cmdlen - 1;
			if (n >= 0x3F) {
				n = 0x3F;
			}
			char name[64];
			strncpy(name, p + 1, n);
			name[n] = '\0';
			if (name[n - 1] == ' ') {
				name[n - 1] = '\0';
			}
			Unknown_78F700Class::BAnimStruct *gesture = Personality->Find_BAnim(name);
			if (gesture == NULL) {
				int number = Personality->Get_Mood_Number(name);
				if (number < 0) {
					p += cmdlen;
					count += cmdlen;
					continue;
				}
				for (int i = 0; i < count; i++) {
					if (start[i] != '\0' && start[i] != ' ') {
						stream->Unknown_4C45B0(1, (unsigned char)Unknown_78EEF0(start, count), start, count);
						break;
					}
				}
				*mood = number;
				Unknown_78E2F0("NLP: Changing Mood to ( %d ) \n", number);
				Unknown_78E2F0("------------------------------------------------------\n");
				stream->Unknown_4C45B0(0, (unsigned char)*mood, NULL, 0);
			} else {
				for (int i = 0; i < count; i++) {
					if (start[i] != '\0' && start[i] != ' ') {
						stream->Unknown_4C45B0(1, (unsigned char)Unknown_78EEF0(start, count), start, count);
						break;
					}
				}
				Unknown_78E2F0("NLP: Manual Gesture for action %s is ( %s ) \n", name, gesture->Tag);
				Unknown_78E2F0("------------------------------------------------------\n");
				stream->Unknown_4C45B0(2, (unsigned char)gesture->AnimNumber, NULL, 0);
			}
			p += cmdlen;
			count = 0;
			start = p;
		} else {
			Unknown_NLPPhraseStruct *phrase = lexicon->Unknown_78E980(p);
			if (phrase == NULL) {
				const char *next = Unknown_78ED30(p, end);
				int n = next ? next - p : strlen(p);
				if (n != 0) {
					count += n;
					p += n;
				}
			} else {
				int n = strlen(phrase->Phrase);
				int context;
				if (p == text) {
					context = Unknown_78ED30(p + n, end) ? 0 : 3;
				} else {
					context = Unknown_78ED30(p + n, end) ? 1 : 2;
				}
				const char *tag = phrase->Context[context];
				if (_stricmp(tag, "none") == 0 || Personality->Find_BAnim(tag) == NULL) {
					Unknown_78E2F0("NLP: No Gesture for phrase in the context or Personality's body anim is missing.\n");
					const char *next = Unknown_78ED30(p, end);
					int m = next ? next - p : strlen(p);
					if (m != 0) {
						count += m;
						p += m;
					}
				} else {
					Unknown_78F700Class::BAnimStruct *gesture = Personality->Find_BAnim(tag);
					if (count + n != 0) {
						stream->Unknown_4C45B0(1, (unsigned char)gesture->AnimNumber, start, count + n);
					}
					count = 0;
					p += n;
					start = p;
				}
				Unknown_78E2F0("------------------------------------------------------\n");
			}
		}
	} while (p - text < len);
	if (count != 0) {
		stream->Unknown_4C45B0(1, (unsigned char)Unknown_78EEF0(start, count), start, count);
	}
}

















































































































// 0x78F640
int PlayerNLPClass::Unknown_78F640(const char *tag)
{
	Unknown_78F700Class::BAnimStruct *body_anim = Personality->Find_BAnim(tag);
	FAIL_IF(!body_anim) {
		return(0);
	}
	return(body_anim->AnimNumber);
}

// 0x78F690
void PlayerNLPClass::Unknown_78F690(const char *text, int len, int *mood, Unknown_ChatStreamClass *stream)
{
	Unknown_78E2F0("Parsing Text '%s'\n", text);
	Unknown_78E2F0("------------------------------------------------------\n");
	Unknown_78F050(text, len, Lexicon, mood, stream);
}

/*
**	Reconstruction notes
**
**	The client's nlp.cpp: 0x78E190 to 0x78F690 (the asserts on lines 413, 454 and 708), its data
**	0xB9D668 and strings 0xB9D670-0xB9D9C0. The personality code from 0x78F700 is another file's
**	(unknown_personality.cpp): 0x78EC00 calls 0x790D40 (20 instructions) where the same compiler
**	inlines it when both share a file.
*/
