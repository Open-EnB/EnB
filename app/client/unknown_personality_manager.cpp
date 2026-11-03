// Earth & Beyond client (reconstruction)
// PROVISIONAL file name (D10): the personality manager (0xBE856C), client 0x790560-0x7912E0 with its
// std::vector instances (unknown_personality.h). The phoneme converter from 0x791310 is another file's.
#include "unknown_personality.h"
#include "ini.h"
#include "nstrdup.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

Unknown_790560Class *Unknown_BE856C;

// 0x790560
// NOT EXACT: functional for POC (D64), shape (push_back's inlined insert, a merged or split tail)
Unknown_790560Class::Unknown_790560Class()
{
	for (int i = 0; ; i++) {
		char name[256];
		_snprintf(name, sizeof(name), "%sPersonality%02d.ini", "personalities\\", i);
		INIClass ini(name);
		if (!ini.Is_Loaded()) {
			break;
		}
		Unknown_790260Class *personality = new Unknown_790260Class(name);
		if (personality != NULL) {
			Personalities.push_back(personality);
		}
	}
}

// 0x790870
Unknown_790560Class::~Unknown_790560Class()
{
	for (std::vector<Unknown_790260Class *>::iterator it = Personalities.begin(); it != Personalities.end(); it++) {
		if (*it) {
			delete *it;
		}
	}
	Personalities.erase(Personalities.begin(), Personalities.end());
}

// 0x790940
void Unknown_790940(Unknown_790560Class *manager)
{
	Unknown_BE856C = manager;
}

// 0x790960
// NOT EXACT: functional for POC (D64), inline-budget (push_back's inlined insert)
void Unknown_790560Class::Unknown_790960()
{
	for (int i = 0; ; i++) {
		char name[256];
		_snprintf(name, sizeof(name), "%sPersonality%02d.ini", "personalities\\", i);
		INIClass ini(name);
		if (!ini.Is_Loaded()) {
			break;
		}
		Unknown_790260Class *personality = new Unknown_790260Class(name);
		if (personality != NULL) {
			Personalities.push_back(personality);
		}
	}
}

// 0x790C70
void Unknown_790560Class::Unknown_790C70()
{
	for (std::vector<Unknown_790260Class *>::iterator it = Personalities.begin(); it != Personalities.end(); it++) {
		if (*it) {
			delete *it;
		}
	}
	Personalities.erase(Personalities.begin(), Personalities.end());
}

// 0x790D00
Unknown_790260Class *Unknown_790560Class::Unknown_790D00(int index)
{
	if (index < (int)Personalities.size() && index >= 0) {
		return(Personalities[index]);
	}
	return(NULL);
}

// 0x790D40
// NOT EXACT: functional for POC (D64), shape (5 rows: the null test's return layout)
Unknown_78F700Class *Unknown_790560Class::Unknown_790D40(int index)
{
	if (index < (int)Personalities.size() && index >= 0 && Personalities[index] != NULL) {
		return(Personalities[index]->Unknown_790360());
	}
	return(NULL);
}

// 0x790D90
// NOT EXACT: functional for POC (D64), shape (5 rows: the null test's return layout)
const char *Unknown_790560Class::Unknown_790D90(int index)
{
	if (index < (int)Personalities.size() && index >= 0 && Personalities[index] != NULL) {
		return(Personalities[index]->Name);
	}
	return(NULL);
}

// 0x790DE0
Unknown_790260Class *Unknown_790560Class::Unknown_790DE0(int index)
{
	if (index >= (int)Personalities.size() || index < 0 || Personalities[index] == NULL) {
		return(NULL);
	}
	return(Personalities[index]);
}

// 0x790E20
// NOT EXACT: functional for POC (D64), shape (5 rows: the null test's return layout)
const char *Unknown_790560Class::Unknown_790E20(int index)
{
	if (index < (int)Personalities.size() && index >= 0 && Personalities[index] != NULL) {
		return(Personalities[index]->Unknown140);
	}
	return(NULL);
}

// 0x790E70
int Unknown_790560Class::Unknown_790E70(const char *filename)
{
	for (int i = 0; i < (int)Personalities.size(); i++) {
		if (_stricmp(Personalities[i]->Filename, filename) == 0) {
			return(i);
		}
	}
	return(-1);
}

// 0x790EE0
// NOT EXACT: functional for POC (D64), shape (push_back's inlined insert)
bool Unknown_790560Class::Unknown_790EE0(const char *filename)
{
	Unknown_790260Class *personality = new Unknown_790260Class(filename);
	if (personality == NULL) {
		return(false);
	}
	Personalities.push_back(personality);
	return(true);
}

