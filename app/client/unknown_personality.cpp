// Earth & Beyond client (reconstruction)
// PROVISIONAL file name (D10): a personality's data and the personality files, client 0x78F700-0x790560
// (unknown_personality.h). Notes at the end of the file.
#include "unknown_personality.h"
#include "ini.h"
#include "nstrdup.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

Unknown_78F700Class::Unknown_78F700Class() { Moods = NULL; Name = NULL; BAnims = NULL; }

// 0x78F850
// The Text member is destroyed after this body (the client inlines its destructor).
Unknown_78F700Class::~Unknown_78F700Class()
{
	if (Name) {
		delete [] Name;
		Name = NULL;
	}
	if (Moods) {
		delete [] Moods;
		Moods = NULL;
	}
	if (BAnims) {
		delete [] BAnims;
		BAnims = NULL;
	}
}

// 0x78F9F0
void Unknown_78F700Class::Unknown_78F9F0()
{
	if (Name) {
		delete [] Name;
		Name = NULL;
	}
	if (Moods) {
		delete [] Moods;
		Moods = NULL;
	}
	if (BAnims) {
		delete [] BAnims;
		BAnims = NULL;
	}
}

// 0x78FBB0
int Unknown_78F700Class::Get_Mood_Number(const char *name)
{
	for (int i = 0; i < 4; i++) {
		if (_stricmp(name, Moods[i].MoodName) == 0) {
			return(Moods[i].MoodNumber);
		}
	}
	return(-1);
}

// 0x78FC20
Unknown_78F700Class::BAnimStruct *Unknown_78F700Class::Find_BAnim(const char *tag)
{
	IndexStruct *index = &Index[(char)tolower(*tag)];
	BAnimStruct *banim = &BAnims[index->First + index->Count - 1];
	for (int i = 0; i < index->Count; i++, banim--) {
		if (_stricmp(tag, banim->Tag) == 0) {
			return(banim);
		}
	}
	return(NULL);
}

static int __cdecl Unknown_790190(const void *a, const void *b);
// 0x78FCB0 (the index loop is Load's own: its count kept in edx and &Count in ecx across the First store, 0x78FFD2, where 0x7901C0 indexes through the entry, 0x79020E)
bool Unknown_78F700Class::Load(const char *filename)
{
	INIClass ini(filename);
	char header[64];
	char buffer[256];
	sprintf(header, "HEADER");
	ini.Get_String(header, "Label", "", buffer, 0x3F);
	Text.Label = nstrdup(buffer);
	ini.Get_String(header, "Desc", "", buffer, 0xFF);
	Text.Desc = nstrdup(buffer);
	Gender = ini.Get_Bool(header, "Gender", false);
	FidgetDelay = ini.Get_Int(header, "FidgetDelay", 45);
	SpeechDelay = ini.Get_Int(header, "SpeechDelay", 3);
	BlinkDelay = ini.Get_Int(header, "BlinkDelay", 60);
	Moods = new MoodStruct[4];
	int i;
	for (i = 0; i < 4; i++) {
		char section[64];
		char name[64];
		sprintf(section, "MOOD-%d", i);
		ini.Get_String(section, "MoodName", "", name, 0x3F);
		Moods[i].MoodName = nstrdup(name);
		Moods[i].MoodNumber = ini.Get_Int(section, "MoodNumber", 0);
	}
	int count = 0;
	for (; true;) {
		char section[64];
		sprintf(section, "BANIM-%d", count);
		if (!ini.Section_Present(section)) {
			break;
		}
		count++;
	}
	BAnimCount = count;
	BAnims = new BAnimStruct[BAnimCount];
	for (i = 0; i < BAnimCount; i++) {
		char section[64];
		char tag[64];
		sprintf(section, "BANIM-%d", i);
		ini.Get_String(section, "Tag", "", tag, 0x3F);
		BAnims[i].Tag = nstrdup(tag);
		BAnims[i].AnimNumber = (ini.Get_Int(section, "AnimNumber", 0) < 0xFF) ? ini.Get_Int(section, "AnimNumber", 0) : 0xFF;
	}
	qsort(BAnims, Get_BAnim_Count(), sizeof(BAnimStruct), Unknown_790190);
	memset(Index, 0, sizeof(Index));
	BAnimStruct *banim = BAnims;
	for (int j = 0; j < Get_BAnim_Count(); j++, banim++) {
		int c = tolower(*banim->Tag); IndexStruct *index = &Index[c]; int n = index->Count;
		if (n == 0) {
			Index[c].First = j;
		}
		index->Count = n + 1;
	}
	char fname[256];
	_splitpath(filename, NULL, NULL, fname, NULL);
	Name = new char[strlen(fname) + 1];
	strcpy(Name, fname);
	return(true);
}

// 0x790190
static int __cdecl Unknown_790190(const void *a, const void *b)	// NAME 1: neutral (D10)
{
	return(_stricmp(*(const char * const *)a, *(const char * const *)b));
}

// 0x7901C0
void Unknown_78F700Class::Unknown_7901C0()
{
	qsort(BAnims, Get_BAnim_Count(), sizeof(BAnimStruct), Unknown_790190);
	memset(Index, 0, sizeof(Index));
	BAnimStruct *banim = BAnims;
	for (int i = 0; i < Get_BAnim_Count(); i++, banim++) {
		IndexStruct *index = &Index[tolower(*banim->Tag)];
		if (index->Count == 0) {
			index->First = i;
		}
		index->Count++;
	}
}

// 0x790260
Unknown_790260Class::Unknown_790260Class(const char *filename)
{
	strcpy(Filename, filename);
	_strlwr(Filename);
	_splitpath(Filename, NULL, NULL, Name, NULL);
	_strlwr(Name);
	char fname[256];
	_splitpath(Filename, NULL, NULL, fname, NULL);
	strchr(fname, '_');	// the client drops the result
	Data = NULL;
}

// 0x790320
Unknown_790260Class::~Unknown_790260Class()
{
	if (Data) {
		delete Data;
		Data = NULL;
	}
}

// 0x790360
// NOT EXACT: shape (the client materializes a boolean and repeats the Data null check before cleanup)
Unknown_78F700Class *Unknown_790260Class::Unknown_790360()
{
	if (Data == NULL) {
		Data = new Unknown_78F700Class;
		if (!Data->Load(Filename) && !Data->Load("personalities\\Personality00.ini")) {
			delete Data;
			Data = NULL;
			return(NULL);
		}
	}
	return(Data);
}

/*
**	Reconstruction notes
**
**	0x78F850 and 0x790360 unwind through 0xACF1A0 and 0xACF1E0 respectively:
**	both pass this + 0x1008 through thunk 0x41269D to the two-pointer destructor 0x78F9B0.
**	Those pointers are Label and Desc, so they form the destructible Text member.
**	0x78F700 is defined out of line before its caller so VC6 emits its exact standalone
**	constructor and still inlines its initialization into 0x790360.
**	The client reads HEADER's 'FidgetDelay' (default 45); the shipped PersonalityNN.ini files spell it
**	'FigetDelay', so the client's avatars use 45. avatar.cpp's stand-in read 'FigetDelay' (50).
*/
