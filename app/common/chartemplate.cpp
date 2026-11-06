// Earth & Beyond client (reconstruction): app\Common\chartemplate.cpp (0x82F210-0x8303B0). Notes at the end of the file.
#include <stdio.h>
#include <string.h>
#include "chartemplate.h"
#include "..\shared\g_assert.h"
#include "..\..\wwlib\ini.h"
#include "..\..\wwlib\inisup.h"
#include "..\..\wwlib\wwstring.h"

#define NUMBER_PLAYER_RACES		3	// the FAIL_IF's (Terran, Jenquai, Progen)
#define MAX_PLAYER_PROFESSIONS	3	// the FAIL_IF's (warrior, trader, explorer)
#define NUM_CHAR_TEMPLATES		9	// the FAIL_IF's: race * 3 + profession

// 0xBA8A08: the templates' names, 0x82F3C0's
static const char *Unknown_BA8A08[NUM_CHAR_TEMPLATES] = {	// NAME 1: neutral (D10)
	"Enforcer", "Tradesman", "Scout", "Defender", "Seeker", "Explorer", "Warrior", "Privateer", "Sentinel"
};

// 0xBF3070 (0x82F210 constructs it, 0x82F250 destroys it)
std::vector<CharTemplateClass *> CharTemplateClass::Templates;
int CharTemplateClass::_References = 0;	// 0xBF3084

// @0x82F2A0
const char *Type_Name(CharTemplateEnum)
{
	return("CharTemplateEnum");
}

// @0x82F2C0
bool String_To_Value(const char *text, CharTemplateEnum *value)
{
	for (int i = 0; i < NUM_CHAR_TEMPLATES; i++) {
		if (stricmp(text, Unknown_BA8A08[i]) == 0) {
			*value = (CharTemplateEnum)i;
			return(true);
		}
	}
	return(false);
}

// @0x82F310
char *Value_To_String(char *buf, int size, CharTemplateEnum value)
{
	strncpy(buf, Unknown_BA8A08[value], size);
	return(buf);
}



































// @0x82F350
int Unknown_82F350(int race, int prof)	// NAME 2 [race, prof]: the FAIL_IFs'; NAME 1 [Unknown_82F350]: neutral (D10)
{
	FAIL_IF((unsigned int)race >= NUMBER_PLAYER_RACES) return(9);
	FAIL_IF((unsigned int)prof >= MAX_PLAYER_PROFESSIONS) return(9);
	return(race * 3 + prof);
}
// @0x82F3C0
const char *Unknown_82F3C0(int template_idx)	// NAME 2 [template_idx]: the FAIL_IF's; NAME 1 [Unknown_82F3C0]: as ui_class.cpp's
{
	FAIL_IF((unsigned int)template_idx >= NUM_CHAR_TEMPLATES) return(0);
	return(Unknown_BA8A08[template_idx]);
}

// @0x82F400
void CharTemplateClass::Init()
{
	if (_References == 0) {
		INIClass ini("chartemplate.ini");
		for (int i = 0; i < NUM_CHAR_TEMPLATES; i++) {
			Templates.push_back(new CharTemplateClass(i, ini));
		}
	}
	_References++;
}

// @0x82F740
void CharTemplateClass::Shutdown()
{
	_References--;
	if (_References == 0) {
		for (std::vector<CharTemplateClass *>::iterator i = Templates.begin(); i != Templates.end(); ++i) {
			delete *i;
		}
		Templates.clear();
	}
}



// @0x82F7D0
CharTemplateClass::CharTemplateClass(int index, INIClass &ini) :
	Name(Unknown_BA8A08[index]),
	Index(index),
	Reputation(100)
{
	char buf[256];
	char desc[128];
	ini.Get_String(Name, "Race", "", buf, 255);
	bool ok = String_To_Value(buf, &Race);
	G_ASSERT(ok);

	ini.Get_String(Name, "Profession", "", buf, 255);
	ok = String_To_Value(buf, &Profession);
	G_ASSERT(ok);

	sprintf(desc, "%s\\Desc", Name);
	std::string section = std::string(Name) + "\\Reputation";
	INISection *found = ini.Find_Section(section.c_str());
	if (found) {
		for (INIEntry *entry = found->EntryList.First_Valid(); entry; entry = entry->Next_Valid()) {
			std::string key(entry->Entry);
			unsigned int value = ini.Get_Int(section.c_str(), key.c_str(), 0);
			Reputation[key] = value;
		}
	}
	StringClass affiliation;



	StartingAffiliation = (const char *)ini.Get_String(affiliation, Name, "Affiliation", "");
	G_ASSERT(!StartingAffiliation.empty());
}

// @0x82FC30
CharTemplateClass::~CharTemplateClass()
{
}

// @0x82FD40
void CharTemplateClass::Get_Reputation(std::hash_map<std::string, unsigned int> &reputation) const
{
	reputation = Reputation;
}

// @0x82FF90
CharTemplateClass *CharTemplateClass::Get_Template(int index)
{

	FAIL_IF(!_References) return(NULL);
	FAIL_IF((unsigned int)index >= NUM_CHAR_TEMPLATES);
	return(Templates[index]);
}

// @0x830000
CharTemplateClass *CharTemplateClass::Get_First()
{
	return(Get_Template(0));
}

// @0x830040
CharTemplateClass *CharTemplateClass::Get_Next() const
{
	unsigned int next = Index + 1;
	if (next >= Templates.size()) {
		return(NULL);
	}
	return(Get_Template(next));
}


/*
**	Notes.
**	The client's chartemplate.cpp, whole (0x82F210 .. 0x8303B0): the nine templates of 'chartemplate.ini'. The FAIL_IFs
**	and asserts sit on the client's lines 85, 86, 92, 132, 136, 153, 171 and 172 (D25). Nothing in the client calls
**	CharTemplateClass, Type_Name, String_To_Value or Value_To_String: only 0x82F350 and 0x82F3C0 have callers.
**	The templates copies 0x8300D0-0x8303B0 are the hash_map's and the vector's, as the compiler emits them.
*/
