// Earth & Beyond client (reconstruction)
// PROVISIONAL file name (D10): a personality's data and the personality files, client 0x78F700-0x790560
// (unknown_personality.h). Notes at the end of the file.
#include "unknown_personality.h"
#include "ini.h"
#include "nstrdup.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// 0x78F850
// NOT EXACT: functional for POC (D64), frame (the client's has an EH frame: a member or base with a destructor not known yet)
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
	if (Label) {
		delete [] Label;
	}
	if (Desc) {
		delete [] Desc;
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

// 0x78FCB0
// NOT EXACT: functional for POC (D64), inline-budget (ours inlines 0x7901C0 differently)
bool Unknown_78F700Class::Load(const char *filename)
{
	INIClass ini(filename);
	char header[64];
	char buffer[256];
	sprintf(header, "HEADER");
	ini.Get_String(header, "Label", "", buffer, 0x3F);
	Label = nstrdup(buffer);
	ini.Get_String(header, "Desc", "", buffer, 0xFF);
	Desc = nstrdup(buffer);
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
	for (;;) {
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
	Unknown_7901C0();
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
// NOT EXACT: functional for POC (D64), field-offset (the client inlines the delete of a failed load: the destructor)
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
**	The client reads HEADER's 'FidgetDelay' (default 45); the shipped PersonalityNN.ini files spell it
**	'FigetDelay', so the client's avatars use 45. avatar.cpp's stand-in read 'FigetDelay' (50).
*/
