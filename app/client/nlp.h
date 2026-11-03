// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\nlp.h (D24: by convention from nlp.cpp's __FILE__)
**
**	The avatar's natural-language processor (PlayerNLPClass, its lexicon) and the personality
**	manager with its entries and their data (interim placement, D137: see nlp.cpp's notes).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef NLP_H	// NAME 3: by convention from nlp.cpp
#define NLP_H

#include <string.h>
#include <vector>

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
	struct { int First; int Count; } Index[256];	// +0x0C by tolower(first letter)  // NAME 2: what it holds
};

// A personality's data (0x1030 bytes, vtable 0xB04A40), loaded from personalities\PersonalityNN.ini
class Unknown_78F700Class	// NAME 1: neutral (D10)
{
public:
	struct MoodStruct		// 8 bytes, destructor 0x78F970  // NAME 2: the INI section 'MOOD-%d'
	{
		~MoodStruct() { if (MoodName) delete [] MoodName; }
		char *MoodName;		// +0x00  // NAME 2: the INI key
		int MoodNumber;		// +0x04  // NAME 2: the INI key
	};
	struct BAnimStruct		// 8 bytes, destructor 0x78F990  // NAME 2: the INI section 'BANIM-%d'
	{
		~BAnimStruct() { if (Tag) delete [] Tag; }
		char *Tag;			// +0x00  // NAME 2: the INI key
		int AnimNumber;		// +0x04 (at most 255)  // NAME 2: the INI key
	};

	Unknown_78F700Class() { Moods = NULL; Name = NULL; BAnims = NULL; }	// 0x78F700
	~Unknown_78F700Class();							// 0x78F850
	virtual void Unknown_78F9F0();					// +0x00 0x78F9F0: frees the name, moods and body animations  // NAME 1: neutral (D10)
	virtual bool Load(const char *filename);		// +0x04 0x78FCB0  // NAME 2: what it does
	virtual int Get_Gender() { return(Gender); }				// +0x08 0x78F730  // NAME 2: what it returns
	virtual const char *Get_Name() { return(Name); }			// +0x0C 0x78F750  // NAME 2: what it returns
	virtual const char *Get_Label() { return(Label); }			// +0x10 0x78F770  // NAME 2: what it returns
	virtual const char *Get_Desc() { return(Desc); }			// +0x14 0x78F790  // NAME 2: what it returns
	virtual int Get_Speech_Delay() { return(SpeechDelay); }	// +0x18 0x78F7B0  // NAME 2: what it returns
	virtual int Get_Blink_Delay() { return(BlinkDelay); }		// +0x1C 0x78F7D0  // NAME 2: what it returns
	virtual int Get_Fidget_Delay() { return(FidgetDelay); }	// +0x20 0x78F7F0  // NAME 2: what it returns
	virtual int Get_Mood_Count() { return(4); }				// +0x24 0x78F810  // NAME 2: what it returns
	virtual int Get_Mood_Number(const char *name);				// +0x28 0x78FBB0 (-1: none)  // NAME 2: what it returns
	virtual int Get_BAnim_Count() { return(BAnimCount); }		// +0x2C 0x78F830  // NAME 2: what it returns
	virtual BAnimStruct *Find_BAnim(const char *tag);			// +0x30 0x78FC20  // NAME 2: what it does
	void Unknown_7901C0();							// 0x7901C0: sort and index the body animations  // NAME 1: neutral (D10)

	char Unknown004[0x800];							// +0x004 (not used by these functions)  // NAME 1: neutral (D10)
	struct { int First; int Count; } Index[256];	// +0x804 the body animations by tolower(first letter)  // NAME 2: what it holds
	char *Name;										// +0x1004 the file's name  // NAME 2: what it holds
	char *Label;									// +0x1008  // NAME 2: the INI key
	char *Desc;										// +0x100C  // NAME 2: the INI key
	int Gender;										// +0x1010  // NAME 2: the INI key
	int FidgetDelay;								// +0x1014  // NAME 2: the INI key
	int SpeechDelay;								// +0x1018  // NAME 2: the INI key
	int BlinkDelay;									// +0x101C  // NAME 2: the INI key
	int Unknown1020;								// +0x1020  // NAME 1: neutral (D10)
	int BAnimCount;									// +0x1024  // NAME 2: what it holds
	MoodStruct *Moods;								// +0x1028 (4)  // NAME 2: what it holds
	BAnimStruct *BAnims;							// +0x102C  // NAME 2: what it holds
};

// One personality file (0x188 bytes): its path, its data loaded on first use
class Unknown_790260Class	// NAME 1: neutral (D10)
{
public:
	Unknown_790260Class(const char *filename);		// 0x790260
	~Unknown_790260Class();							// 0x790320
	Unknown_78F700Class *Unknown_790360();			// 0x790360: the data, loaded (Personality00.ini when the file fails)  // NAME 1: neutral (D10)

	char Filename[0x100];							// +0x000 (lower case)  // NAME 2: what it holds
	char Name[0x40];								// +0x100 the file's name  // NAME 2: what it holds
	char Unknown140[0x44];							// +0x140 (0x790E20 returns it; nothing here writes it)  // NAME 1: neutral (D10)
	Unknown_78F700Class *Data;						// +0x184  // NAME 2: what it holds
};

// The lip-sync phoneme converter (0x791310-0x792040, not rebuilt: another file's), the manager's +0x10
class Unknown_791310Class	// NAME 1: neutral (D10)
{
public:
	Unknown_791310Class();							// 0x791310: sorts and indexes the table 0xB9DAB8
	int Unknown_791400(const char *text, int *phonemes, int max);	// 0x791400  // NAME 1: neutral (D10)
};

// The personality manager (0x14 bytes, 0xBE856C; WinMain makes it): personalities\PersonalityNN.ini from 00
class Unknown_790560Class	// NAME 1: neutral (D10)
{
public:
	Unknown_790560Class();							// 0x790560
	~Unknown_790560Class();							// 0x790870
	void Unknown_790960();							// 0x790960: loads the files again (appends)  // NAME 1: neutral (D10)
	void Unknown_790C70();							// 0x790C70: deletes them  // NAME 1: neutral (D10)
	Unknown_790260Class *Unknown_790D00(int index);	// 0x790D00  // NAME 1: neutral (D10)
	Unknown_78F700Class *Unknown_790D40(int index);	// 0x790D40: the data  // NAME 1: neutral (D10)
	const char *Unknown_790D90(int index);			// 0x790D90: the name  // NAME 1: neutral (D10)
	Unknown_790260Class *Unknown_790DE0(int index);	// 0x790DE0  // NAME 1: neutral (D10)
	const char *Unknown_790E20(int index);			// 0x790E20: +0x140  // NAME 1: neutral (D10)
	int Unknown_790E70(const char *filename);		// 0x790E70: the index by path (-1)  // NAME 1: neutral (D10)
	bool Unknown_790EE0(const char *filename);		// 0x790EE0: adds one  // NAME 1: neutral (D10)

	std::vector<Unknown_790260Class *> Personalities;	// +0x00 (allocator, +0x04 first, +0x08 last, +0x0C end)  // NAME 2: what it holds
	Unknown_791310Class Phonemes;					// +0x10  // NAME 2: what it does
};

void Unknown_790940(Unknown_790560Class *manager);	// 0x790940: sets 0xBE856C  // NAME 1: neutral (D10)
extern Unknown_790560Class *Unknown_BE856C;			// 0xBE856C  // NAME 1: neutral (D10)

// nlp.cpp's class (0x14 bytes, vtable 0xB04A2C): the avatar's (+0xF4)
class PlayerNLPClass	// NAME 2: the client's assert 'PlayerNLP != 0'
{
public:
	PlayerNLPClass() : Initialized(false), Unknown08(-1), Personality(NULL), Lexicon(NULL) {}	// 0x78EB50
	~PlayerNLPClass() { Unknown_78ECC0(); }			// 0x78EB80
	virtual void Unknown_78EC00(int personality, int nlp);	// +0x00 0x78EC00: the personality's data, nlp\lexicon%02d.ini  // NAME 1: neutral (D10)
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
