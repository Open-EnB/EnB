// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Common\chartemplate.h (name assumed from chartemplate.cpp;
**	no assert names it)
**
**	The nine character templates read from 'chartemplate.ini' (chartemplate.cpp, 0x82F210-0x8303B0).
**	Nothing in the client calls this class: only the two name functions 0x82F350 / 0x82F3C0 are used.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CHARTEMPLATE_H  // NAME 3 [chartemplate.h]: header name assumed from the client's chartemplate.cpp
#define CHARTEMPLATE_H

#include <string>
#include <vector>
#include <hash_map.h>
#include "race.h"
#include "prof.h"

class INIClass;

enum CharTemplateEnum {};	// NAME 5: the client's Type_Name 'CharTemplateEnum'

const char *Type_Name(CharTemplateEnum value);								// 0x82F2A0
bool String_To_Value(const char *text, CharTemplateEnum *value);			// 0x82F2C0
char *Value_To_String(char *buf, int size, CharTemplateEnum value);		// 0x82F310

class CharTemplateClass {	// NAME 3: built by convention from chartemplate.cpp (D24)
public:
	CharTemplateClass(int index, INIClass &ini);	// 0x82F7D0
	~CharTemplateClass();							// 0x82FC30

	static void Init();								// 0x82F400  // NAME 2: reads 'chartemplate.ini' on the first reference
	static void Shutdown();							// 0x82F740  // NAME 2: frees the templates with the last reference
	static CharTemplateClass *Get_Template(int index);	// 0x82FF90  // NAME 2: what it returns
	static CharTemplateClass *Get_First();			// 0x830000  // NAME 2: Get_Template(0)
	CharTemplateClass *Get_Next() const;			// 0x830040  // NAME 2: the template after this one, or NULL
	void Get_Reputation(std::hash_map<std::string, unsigned int> &reputation) const;	// 0x82FD40  // NAME 2: copies Reputation

	const char *Name;								// +0x00: 0xBA8A08's entry  // NAME 2: what it holds
	int Index;										// +0x04  // NAME 2: what it holds
	RaceType Race;									// +0x08: the ini's 'Race'  // NAME 2: the key it reads
	ProfessionType Profession;						// +0x0C: the ini's 'Profession'  // NAME 2: the key it reads
	std::hash_map<std::string, unsigned int> Reputation;		// +0x10: the ini's '<Name>\Reputation' section  // NAME 2: the section it reads
	std::string StartingAffiliation;				// +0x28: the client's assert

	static std::vector<CharTemplateClass *> Templates;	// 0xBF3070  // NAME 2: what it holds
	static int _References;							// 0xBF3084: the client's assert
};

#endif
