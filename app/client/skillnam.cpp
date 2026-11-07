// Earth & Beyond client (reconstruction): app\Client\skillnam.cpp, the skills' ini table
// (the client's file per its asserts, 'e:\build\beta\G\app\Client\skillnam.cpp' at 0xBA7E00). Notes at the end.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <string>
#include <vector>
#include <hash_map.h>
#include "skillnam.h"
#include "ini.h"
#include "inisup.h"
#include "assetmgr.h"
#include "wwstring.h"
#include "nstrdup.h"
#include "..\shared\g_assert.h"

const char *Unknown_82F3C0(int index);	// 0x82F3C0: a character template's name  // NAME 1: neutral (D10)







enum {
	MAX_SKILL_LEVEL = 9,		// NAME 5: the assert 'level > MAX_SKILL_LEVEL+1', 0x8182E0
	NUM_CHAR_TEMPLATES = 9		// NAME 5: the assert '(unsigned) char_tmpl >= NUM_CHAR_TEMPLATES', 0x8185FD
};

// One skill, 0xE8 bytes.
class SkillNameClass	// NAME 2: an entry of _AllSkills
{
public:
	SkillNameClass(const char *name, const char *icon, const char *category, const char *description, bool selectable);
	~SkillNameClass();

	void Set_Description(unsigned int level, const char *text);
	void Set_Max(unsigned int char_tmpl, int max);
	void Set_Trainer(unsigned int char_tmpl, const std::string &text);
	const char *Get_Description(unsigned int level) const;
	int Get_Max(unsigned int char_tmpl) const;
	const char *Get_Trainer(unsigned int char_tmpl) const;
	int Get_Range(unsigned int idx) const;

	char *Name;								// +0x00  // NAME 2: 'All Skills' entry's value
	char *Icon;								// +0x04  // NAME 2: its 'Icon'
	char *Category;						// +0x08  // NAME 2: its 'Category'
	char *Description;					// +0x0C  // NAME 2: its 'Description'
	bool Selectable;						// +0x10  // NAME 2: its 'Selectable'
	int Max[NUM_CHAR_TEMPLATES];		// +0x14  // NAME 5: the assert 'max < 0'
	std::string Trainers[NUM_CHAR_TEMPLATES];		// +0x38  // NAME 2: its section's 'Trainer'
	std::vector<int> Ranges;			// +0xC8  // NAME 5: the assert 'idx >= Ranges.size()'
	std::vector<std::string> Descriptions;			// +0xD8  // NAME 2: its 'Desc_N'
};

inline SkillNameClass::SkillNameClass(const char *name, const char *icon, const char *category, const char *description, bool selectable) :
	Name(NULL), Icon(NULL), Category(NULL), Description(NULL), Selectable(selectable)
{
	memset(Max, 0, NUM_CHAR_TEMPLATES);	// the client clears 9 bytes, not 9 ints
	Ranges.push_back(0);
	if (name && *name) {
		Name = nstrdup(name);
	} else {
		G_ASSERT(false);
		Name = nstrdup("");
	}
	if (icon && *icon) {
		Icon = nstrdup(icon);
	}
	if (category && *category) {
		Category = nstrdup(category);
	}
	if (description && *description) {
		Description = nstrdup(description);
	}
	Descriptions.resize(MAX_SKILL_LEVEL + 2, std::string("No Description Available"));
	Descriptions[0] = "You do not have this skill yet";
	Descriptions[MAX_SKILL_LEVEL + 1] = "You have achieved the highest level for this skill.";
}

inline SkillNameClass::~SkillNameClass()
{
	delete [] Name;
	Name = NULL;
	delete [] Icon;
	Icon = NULL;
	delete [] Category;
	Category = NULL;
	delete [] Description;
	Description = NULL;
}

inline void SkillNameClass::Set_Description(unsigned int level, const char *text)
{





	FAIL_IF(level > MAX_SKILL_LEVEL+1) return;
	Descriptions[level] = text;
}
inline void SkillNameClass::Set_Max(unsigned int char_tmpl, int max) {
	FAIL_IF((unsigned) char_tmpl >= NUM_CHAR_TEMPLATES) return;
	Max[char_tmpl] = max;
}
inline void SkillNameClass::Set_Trainer(unsigned int char_tmpl, const std::string &text)
{
	FAIL_IF((unsigned) char_tmpl >= NUM_CHAR_TEMPLATES) return;
	Trainers[char_tmpl] = text;
}

inline const char *SkillNameClass::Get_Description(unsigned int level) const
{







	FAIL_IF(level > MAX_SKILL_LEVEL+1) return("Invalid Skill Level");
	return(Descriptions[level].c_str());
}
inline int SkillNameClass::Get_Max(unsigned int char_tmpl) const {
	FAIL_IF((unsigned) char_tmpl >= NUM_CHAR_TEMPLATES) return(0);
	return(Max[char_tmpl]);
}
inline const char *SkillNameClass::Get_Trainer(unsigned int char_tmpl) const
{
	FAIL_IF((unsigned) char_tmpl >= NUM_CHAR_TEMPLATES) return("Bug");
	return(Trainers[char_tmpl].c_str());
}

inline int SkillNameClass::Get_Range(unsigned int idx) const
{

	FAIL_IF(idx >= Ranges.size()) return(0);
	return(Ranges[idx]);
}

static std::vector<SkillNameClass *> _AllSkills;				// 0xBE8E28  // NAME 5: the assert 'index >= _AllSkills.size()'
static std::hash_map<std::string, int> _SkillIndex;			// 0xBE8E38  // NAME 2: describes what it does

// NOT EXACT: register, the client's loop in esi and a dead store of end() before the erase's last store
// 0x816E60
void Unknown_816E60()
{
	for (std::vector<SkillNameClass *>::iterator it = _AllSkills.begin(); it != _AllSkills.end(); ++it) {
		delete *it;
		*it = NULL;
	}
	_AllSkills.erase(_AllSkills.begin(), _AllSkills.end());
	_SkillIndex.clear();
}

// NOT EXACT: frame, 0x1C4 against the client's 0x1C0, and the string locals' slots; every call in the client's order
// 0x817070
void Unknown_817070(const char *ini_name, const char *ex_ini_name)
{
	std::string bug("Bug: Report Me");
	INIClass ini;
	ini.Load(ini_name);
	ini.Load(ex_ini_name);






















	FAIL_IF(!ini.Is_Loaded()) return;
	INISection *section = ini.Find_Section("All Skills");

	FAIL_IF(0 == section) return;
	unsigned int entrycount = section->EntryIndex.Count();

	FAIL_IF(entrycount == 0) return;
	_AllSkills.resize(entrycount);
	int skill_index = 0;
	INIEntry *entry = section->EntryList.First_Valid();
	for (std::vector<SkillNameClass *>::iterator it = _AllSkills.begin(); it != _AllSkills.end(); ++it) {
		int skill_index_2 = atol(entry->Entry);

		// each skill's index is its entry's key
		G_ASSERT(skill_index == skill_index_2);
		char icon[256];
		ini.Get_String(entry->Value, "Icon", "skill_.tga", icon, sizeof(icon) - 1);
		StringClass category;
		ini.Get_String(category, entry->Value, "Category", "");
		StringClass description;
		ini.Get_String(description, entry->Value, "Description", "");
		bool selectable = ini.Get_Bool(entry->Value, "Selectable", false);
		SkillNameClass *skill = new SkillNameClass(entry->Value, icon, category, description, selectable);
		for (unsigned int level = 1; level <= MAX_SKILL_LEVEL; level++) {
			char key[16];
			sprintf(key, "Desc_%d", level);
			StringClass text;
			ini.Get_String(text, entry->Value, key, "Skill level description not available.");
			skill->Set_Description(level, text);
		}
		StringClass range;
		ini.Get_String(range, entry->Value, "Range", "");
		const char *p = range;
		if (p) {
			while (*p) {
				skill->Ranges.push_back(atoi(p));
				while (*p && *p != ',') {
					p++;
				}
				while (*p && !isdigit(*p)) {
					p++;
				}
			}
		}
		for (int char_tmpl = 0; char_tmpl < NUM_CHAR_TEMPLATES; char_tmpl++) {
			const char *tmpl_name = Unknown_82F3C0(char_tmpl);
			int max = ini.Get_Int(entry->Value, tmpl_name, 0);
















			FAIL_IF(max < 0) max = 0;
			skill->Set_Max(char_tmpl, max);
			std::string name(entry->Value);
			name += " ";
			name += tmpl_name;
			if (ini.Find_Section(name.c_str())) {
				std::string trainer;
				StringClass temp;
				ini.Get_String(temp, name.c_str(), "Trainer", "Unknown");
				trainer = "See ";
				trainer += temp;
				ini.Get_String(temp, name.c_str(), "Location", "Unknown");
				trainer += " at ";
				trainer += temp;
				ini.Get_String(temp, name.c_str(), "Sector", "Unknown");
				trainer += " in ";
				trainer += temp;
				ini.Get_String(temp, name.c_str(), "System", "Unknown");
				trainer += ", ";
				trainer += temp;
				trainer += " System";
				if (ini.Get_Int(name.c_str(), "Learn Lvl", 0)) {
					ini.Get_String(temp, name.c_str(), "Learn Lvl", "");
					trainer += " upon reaching level ";
					trainer += temp;
				}
				trainer += ".";
				skill->Set_Trainer(char_tmpl, trainer);
			} else {
				skill->Set_Trainer(char_tmpl, bug);
			}
		}
		*it = skill;
		_SkillIndex[std::string(entry->Value)] = skill_index;
		entry = entry->Next_Valid();
		skill_index++;
	}
}

// 0x818170
int Unknown_818170(const std::string &name)
{
	std::hash_map<std::string, int>::iterator it = _SkillIndex.find(name);
	if (it == _SkillIndex.end()) {
		return(-1);
	}
	return((*it).second);
}

// 0x818200
const char *Unknown_818200(int index)
{















	FAIL_IF(index >= _AllSkills.size()) return("?Skill Error?");
	return(_AllSkills[index]->Name);
}

// 0x818250
const char *Unknown_818250(int index)
{
	FAIL_IF(index >= _AllSkills.size()) return("?Skill Error?");
	return(_AllSkills[index]->Description);
}

// 0x8182A0
const char *Unknown_8182A0(int index, int level)
{
	FAIL_IF(index >= _AllSkills.size()) return("?Skill Error?");
	return(_AllSkills[index]->Get_Description(level));
}
// 0x818340
int Unknown_818340(int index, unsigned int idx)
{
	FAIL_IF(index >= _AllSkills.size()) return(0);
	return(_AllSkills[index]->Get_Range(idx));
}

// 0x8183E0
const char *Unknown_8183E0(int index)
{

	FAIL_IF(index >= _AllSkills.size()) return("?Skill Error?");
	return(_AllSkills[index]->Icon);
}

// 0x818430
TextureClass *Unknown_818430(int index)
{
	const char *icon = Unknown_8183E0(index);
	if (icon && *icon) {
		TextureClass *texture = WW3DAssetManager::Get_Instance()->Get_Texture(icon, MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true);
		if (texture) {
			return(texture);
		}
	}
	return(WW3DAssetManager::Get_Instance()->Get_Texture("skill_.tga", MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true));
}

// 0x8184D0
const char *Unknown_8184D0(int index)
{



	FAIL_IF(index >= _AllSkills.size()) return("?Skill Error?");
	return(_AllSkills[index]->Category);
}

// 0x818520
int Unknown_818520(const char *name)
{
	for (unsigned int i = 0; i < _AllSkills.size(); i++) {
		if (!stricmp(name, _AllSkills[i]->Name)) {
			return(i);
		}
	}
	return(-1);
}

// 0x818590
int SkillNamesDatabase::Max_Skill()
{
	return(_AllSkills.size());
}

// 0x8185C0
unsigned int Unknown_8185C0(int index, int char_tmpl)
{
	FAIL_IF(index >= _AllSkills.size()) return(0);
	return(_AllSkills[index]->Get_Max(char_tmpl));
}
// 0x818640
const char *Unknown_818640(int index, int char_tmpl)
{
	FAIL_IF(index >= _AllSkills.size()) return(0);
	return(_AllSkills[index]->Get_Trainer(char_tmpl));
}

/*
**	Reconstruction notes.
**
**	The object runs from 0x816D70 (_AllSkills' constructor stub; 0x816CE0 before it is skill_ability_names.cpp's template) to
**	<string>'s initialiser at 0x8190A0, with its statics at 0xBE8E20-0xBE8E4F. The next object (0x8190D0) is time_mgr.cpp.
**	Only SkillNamesDatabase::Max_Skill is named by the client (0x6A4F40's assert); the other functions take no this and
**	keep their callers' neutral names. Unknown_818430 calls Unknown_8183E0, inlined one level: the assert's line 346 is
**	shared. The line numbers come from the asserts; the class in this file holds lines 65-139 as in the client, where
**	the same __FILE__ is on its inline members' asserts, so the class is this file's and not a header's.
*/
