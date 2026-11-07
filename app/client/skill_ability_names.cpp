// Earth & Beyond client (reconstruction): app\client\skill_ability_names.cpp, the abilities' ini tables
// (the client's file per its asserts, 'e:\build\beta\G\app\Client\skill_ability_names.cpp' at 0xBA7A18). Notes at the end.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <hash_map.h>
#include "skill_ability_names.h"
#include "point.h"
#include "ini.h"
#include "..\shared\g_assert.h"
#include "..\shared\kv_tripl.h"
#include "..\shared\split_string.h"

// skillnam.cpp's (not rebuilt yet)
class SkillNamesDatabase { public: static int Max_Skill();	// NAME 5: 0x6A4F40's assert
	static int Unknown_818170(const std::string &name);	// NAME 1: neutral (D10)
	static int Unknown_818340(int skill, unsigned int level);	// NAME 1: neutral (D10)
	static TextureClass *Unknown_818430(int skill); };	// NAME 1: neutral (D10)
// kv_util.cpp's (not rebuilt yet)
void Unknown_4BB3F0(KVTripleListClass &list, INIClass &ini, const char *section);	// NAME 1: neutral (D10)

std::vector<Unknown815320Struct *> SkillAbilityNamesDatabase::_AllAbilities;					// 0xBE8DA8  // NAME 3: its asserts
std::vector<Unknown8155B0Struct> SkillAbilityNamesDatabase::_SkillAbilities;					// 0xBE8DB8  // NAME 3: its asserts
std::hash_map<std::string, unsigned int> SkillAbilityNamesDatabase::_AbilityIndex;			// 0xBE8DC8  // NAME 2: describes what it does
int SkillAbilityNamesDatabase::_NumActivated;															// 0xBE8DF8  // NAME 2: describes what it does


// 0x814430
void SkillAbilityNamesDatabase::Unknown_814430()
{
	std::vector<Unknown815320Struct *>::iterator it;
	for (it = _AllAbilities.begin(); it != _AllAbilities.end(); ++it) {
		Unknown815320Struct *ability = *it;
		delete ability;
	}
	_AllAbilities.clear();
	_AbilityIndex.clear();
	_SkillAbilities.clear();
}

static bool Compare_Abilities(const Unknown815320Struct *a, const Unknown815320Struct *b)	// 0x8149C0  // NAME 2: describes what it does
{
	return(a->Unknown18 < b->Unknown18);
}




















// 0x814620
void SkillAbilityNamesDatabase::Unknown_814620(const char *ini_name, const char *ex_ini_name)
{
	INIClass ini;
	ini.Load(ini_name);
	ini.Load(ex_ini_name);


	FAIL_IF(!ini.Is_Loaded()) return;
	unsigned int count = SkillNamesDatabase::Max_Skill() + 1;
	Unknown8155B0Struct blank;
	_SkillAbilities.resize(count, blank);
	unsigned int index = 0;
	Unknown_814A30(ini, "Activated Abilities", index, true);
	_NumActivated = index;
	Unknown_814A30(ini, "Passive Abilities", index, false);
	for (std::vector<Unknown8155B0Struct>::iterator it = _SkillAbilities.begin(); it != _SkillAbilities.end(); ++it) {
		std::vector<Unknown815320Struct *> &abilities = it->Abilities;
		std::sort(abilities.begin(), abilities.end(), Compare_Abilities);
	}
}













// 0x814A30
void SkillAbilityNamesDatabase::Unknown_814A30(INIClass &ini, const char *section, unsigned int &index, bool activated)
{
	KVTripleListClass entries;
	Unknown_4BB3F0(entries, ini, section);
	int count = entries.Get_Count(); unsigned int total = index + count;
	_AllAbilities.resize(total);
	TPoint3D<float> white(1.0f, 1.0f, 1.0f);
	for (; index < total; index++) {
		KVTripleClass *item;
		{ char key[4]; sprintf(key, "%d", index); item = entries.Find(key); }



		FAIL_IF(NULL == item) continue;

		G_ASSERT(index == (unsigned int)atol(item->Key));
		std::string name(item->Value);
		char buffer[0x1000];
		ini.Get_String(name.c_str(), "Skill", "", buffer, sizeof(buffer) - 1);
		std::string skill_name(buffer);



		FAIL_IF(skill_name.empty()) continue;
		int level = ini.Get_Int(name.c_str(), "MinLevel", 0);

		FAIL_IF(level < 1) continue;
		char level_key[32];
		sprintf(level_key, "Level_%d", level);
		TPoint3D<float> color = ini.Get_Point("AbilityColors", level_key, white);
		color = ini.Get_Point(name.c_str(), "Color", color);
		unsigned int skill_index = SkillNamesDatabase::Unknown_818170(skill_name);




		FAIL_IF(skill_index >= _SkillAbilities.size()) continue;
		float cost = ini.Get_Float(name.c_str(), "Cost", 0.0f);
		ini.Get_String(name.c_str(), "Desc", "", buffer, sizeof(buffer) - 1);
		std::string description(buffer);
		Unknown815320Struct *ability = new Unknown815320Struct(name, index, skill_index, level, activated, (const Vector3 *)&color, (int)cost, description);
		_AllAbilities[index] = ability;
		_AbilityIndex[name] = index;
		Unknown8155B0Struct &skill = _SkillAbilities[skill_index];
		skill.Abilities.push_back(ability);
		if (activated) {
			skill.NumActivatable++;
		}
	}
}

// 0x815230
int SkillAbilityNamesDatabase::Unknown_815230()
{
	return(_AllAbilities.size());
}

// 0x815260
int SkillAbilityNamesDatabase::Unknown_815260(const std::string &name)
{
	if (_AbilityIndex.find(name) == _AbilityIndex.end()) {
		return(-1);
	}
	return(_AbilityIndex[name]);
}

// 0x815300
unsigned int SkillAbilityNamesDatabase::Unknown_815300(const std::string &name)
{
	return(SkillNamesDatabase::Unknown_818170(name));
}








// 0x815320
Unknown815320Struct *SkillAbilityNamesDatabase::Unknown_815320(int ability_idx)
{
	static Unknown815320Struct null_ability("null", -1, -1, 0, false, NULL, 0, "");
	if ((unsigned int)ability_idx < _AllAbilities.size()) {
		Unknown815320Struct *ability = _AllAbilities[ability_idx];









		FAIL_IF(!ability) return(&null_ability);
		return(ability);
	}
	return(&null_ability);
}

// 0x8155B0
const Unknown8155B0Struct &SkillAbilityNamesDatabase::Unknown_8155B0(int skill)
{
	static Unknown8155B0Struct empty;
	if ((unsigned int)skill >= _SkillAbilities.size()) {
		return(empty);
	}
	return(_SkillAbilities[skill]);
}

// 0x815690
TextureClass *SkillAbilityNamesDatabase::Unknown_815690(int ability_idx, Vector3 *color)
{
	FAIL_IF((unsigned int)ability_idx >= _AllAbilities.size()) return(NULL);
	Unknown815320Struct *ability = Unknown_815320(ability_idx);
	if (color) {
		*color = ability->Unknown20;
	}
	return(SkillNamesDatabase::Unknown_818430(ability->Unknown14));
}

// NOT EXACT: inline-budget, the client calls _Eos and _Tidy out of line and inlines _Grow in the last append
// 0x815720
void SkillAbilityNamesDatabase::Unknown_815720(std::string *string, int ability_idx, bool below, unsigned int level)
{



	FAIL_IF((unsigned int)ability_idx >= _AllAbilities.size()) {
		*string = "Error: Invalid skill index.";
		return;
	}
	Unknown815320Struct *ability = Unknown_815320(ability_idx);
	int range = SkillNamesDatabase::Unknown_818340(ability->Unknown14, level > ability->Unknown18 ? level : ability->Unknown18);
	char cost[64];
	if (ability->EnergyCost >= 0) {
		itoa(ability->EnergyCost, cost, 10);
	} else {
		strcpy(cost, "Variable");
	}
	char buffer[128];
	_snprintf(buffer, sizeof(buffer), "\n\nEnergy cost: %s\nRange: %0.2fk units\n\n", cost, range * 0.001f);
	*string += ability->Name + buffer;
	SplitStringClass split(ability->Description.c_str(), 40.0f, 0);
	for (unsigned int i = 0; i < split.Count(); i++) {
		*string += split.Get(i);
		*string += "\n";
	}
	if (below) {
		_snprintf(buffer, sizeof(buffer), "\nRequires Level %d Skill Upgrade", ability->Unknown18);
		string->append(buffer);
	}
}

/*
**	Notes.
**	Object 0x814280-0x816xxx, after skill_ability_data_peek.cpp's <string> initialiser 0x814230: _AllAbilities 0x814280 (atexit
**	0x8142C0), _SkillAbilities 0x814310 (0x814350), the hash_map 0x8143D0 (0x814400). After 0x815720 come the templates:
**	0x815BF0 size, 0x815C20 size, 0x815C60 hash_map operator[], 0x815E70 insert, 0x816110 erase, 0x816160 / 0x816520 the 0x14-byte
**	elements' insert / erase, and on to std::sort's helpers.
*/
