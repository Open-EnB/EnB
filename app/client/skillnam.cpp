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
	void Set_Max(int char_tmpl, int max);
	void Set_Trainer(int char_tmpl, const std::string &text);
	const char *Get_Description(unsigned int level) const;
	int Get_Max(int char_tmpl) const;
	const char *Get_Trainer(int char_tmpl) const;
	int Get_Range(unsigned int idx) const;
	void Add_Range(int range) { Ranges.push_back(range); }

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
	Add_Range(0);
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
	{ std::string blank("No Description Available"); Descriptions.resize(MAX_SKILL_LEVEL + 2, blank); }
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
inline void SkillNameClass::Set_Max(int char_tmpl, int max) {
	FAIL_IF((unsigned) char_tmpl >= NUM_CHAR_TEMPLATES) return;
	Max[char_tmpl] = max;
}
inline void SkillNameClass::Set_Trainer(int char_tmpl, const std::string &text)
{
	FAIL_IF((unsigned) char_tmpl >= NUM_CHAR_TEMPLATES) return;
	Trainers[char_tmpl] = text;
}

inline const char *SkillNameClass::Get_Description(unsigned int level) const
{







	FAIL_IF(level > MAX_SKILL_LEVEL+1) return("Invalid Skill Level");
	return(Descriptions[level].c_str());
}
inline int SkillNameClass::Get_Max(int char_tmpl) const {
	FAIL_IF((unsigned) char_tmpl >= NUM_CHAR_TEMPLATES) return(0);
	return(Max[char_tmpl]);
}
inline const char *SkillNameClass::Get_Trainer(int char_tmpl) const
{
	FAIL_IF((unsigned) char_tmpl >= NUM_CHAR_TEMPLATES) return("Bug");
	return(Trainers[char_tmpl].c_str());
}

inline int SkillNameClass::Get_Range(unsigned int idx) const
{

	FAIL_IF(idx >= Ranges.size()) return(0);
	return(Ranges[idx]);
}

std::vector<SkillNameClass *> SkillNamesDatabase::_AllSkills;				// 0xBE8E28  // NAME 5: the assert 'index >= _AllSkills.size()'
std::hash_map<std::string, int> SkillNamesDatabase::_SkillIndex;			// 0xBE8E38  // NAME 2: describes what it does

// 0x816E60
void SkillNamesDatabase::Unknown_816E60()
{
	for (std::vector<SkillNameClass *>::iterator it = _AllSkills.begin(); it != _AllSkills.end(); ++it) {
		delete *it;
		*it = NULL;
	}
	_AllSkills.erase(_AllSkills.begin(), _AllSkills.end());
	_SkillIndex.clear();
}

// 0x817070
void SkillNamesDatabase::Unknown_817070(const char *ini_name, const char *ex_ini_name)
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
				skill->Add_Range(atoi(p));
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
				trainer.append(temp);
				ini.Get_String(temp, name.c_str(), "Location", "Unknown");
				trainer.append(" at ");
				trainer.append(temp);
				ini.Get_String(temp, name.c_str(), "Sector", "Unknown");
				trainer.append(" in ");
				trainer.append(temp);
				ini.Get_String(temp, name.c_str(), "System", "Unknown");
				trainer.append(", ");
				trainer.append(temp);
				trainer.append(" System");
				if (ini.Get_Int(name.c_str(), "Learn Lvl", 0)) {
					ini.Get_String(temp, name.c_str(), "Learn Lvl", "");
					trainer += " upon reaching level ";
					trainer.append(temp);
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
int SkillNamesDatabase::Unknown_818170(const std::string &name)
{
	std::hash_map<std::string, int>::iterator it = _SkillIndex.find(name);
	if (it == _SkillIndex.end()) {
		return(-1);
	}
	return((*it).second);
}

// 0x818200
const char *SkillNamesDatabase::Unknown_818200(int index)
{















	FAIL_IF(index >= _AllSkills.size()) return("?Skill Error?");
	return(_AllSkills[index]->Name);
}

// 0x818250
const char *SkillNamesDatabase::Unknown_818250(int index)
{
	FAIL_IF(index >= _AllSkills.size()) return("?Skill Error?");
	return(_AllSkills[index]->Description);
}

// 0x8182A0
const char *SkillNamesDatabase::Unknown_8182A0(int index, int level)
{
	FAIL_IF(index >= _AllSkills.size()) return("?Skill Error?");
	return(_AllSkills[index]->Get_Description(level));
}
// 0x818340
int SkillNamesDatabase::Unknown_818340(int index, unsigned int idx)
{
	FAIL_IF(index >= _AllSkills.size()) return(0);
	return(_AllSkills[index]->Get_Range(idx));
}

// 0x8183E0
const char *SkillNamesDatabase::Unknown_8183E0(int index)
{

	FAIL_IF(index >= _AllSkills.size()) return("?Skill Error?");
	return(_AllSkills[index]->Icon);
}

// 0x818430
TextureClass *SkillNamesDatabase::Unknown_818430(int index)
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
const char *SkillNamesDatabase::Unknown_8184D0(int index)
{



	FAIL_IF(index >= _AllSkills.size()) return("?Skill Error?");
	return(_AllSkills[index]->Category);
}

// 0x818520
int SkillNamesDatabase::Unknown_818520(const char *name)
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
unsigned int SkillNamesDatabase::Unknown_8185C0(int index, int char_tmpl)
{
	FAIL_IF(index >= _AllSkills.size()) return(0);
	return(_AllSkills[index]->Get_Max(char_tmpl));
}
// 0x818640
const char *SkillNamesDatabase::Unknown_818640(int index, int char_tmpl)
{
	FAIL_IF(index >= _AllSkills.size()) return(0);
	return(_AllSkills[index]->Get_Trainer(char_tmpl));
}

/*
**	Reconstruction notes.
**
**	The object runs from 0x816D70 (_AllSkills' constructor stub; 0x816CE0 before it is skill_ability_names.cpp's template) to
**	<string>'s initialiser at 0x8190A0, with its statics at 0xBE8E20-0xBE8E4F. The next object (0x8190D0) is time_mgr.cpp.
**	Only SkillNamesDatabase::Max_Skill is named by the client (0x6A4F40's assert). The others are its static members too:
**	their asserts read _AllSkills unqualified, and only members with external linkage give both the statics' guarded
**	atexit stubs (0x816DB0, 0x816E30) and 0x816E60's and 0x817070's code: a file static gives other registers there, a
**	plain global loses the guards. Unknown_818430 calls Unknown_8183E0, inlined one level: the assert's line 346 is shared.
**	SkillNameClass's inline members carry this file's __FILE__ on lines 65-139, so the class is this file's, not a header's.
**
**	Forms 0x817070 needs: Add_Range, an inline taking the value, shares the atoi temporary's slot with the constructor's
**	(a direct push_back makes the frame 4 bigger); a named string for the descriptions' resize; append for the trainer's
**	pieces, since += spends enough inline budget to push ' upon reaching level ' out of line.
*/
