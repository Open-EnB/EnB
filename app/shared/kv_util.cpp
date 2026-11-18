// Earth & Beyond client (reconstruction): app\shared\kv_util.cpp, KV lists to and from ini sections. Notes at the end.
#include <string.h>
#include "kv_tripl.h"
#include "auxdata\permstr.h"	// (its PermStringInitClass static: 0x4BB090, CRT entry 95)
#include "g_assert.h"
#include "ini.h"
#include "inisup.h"
static char KeyBuffer[0x200];		// 0xBE509C  // NAME 2: describes what it does








// (inlined: 0x4BB0E0's '.' -> '\', 0x4BB4C0's '\' -> '.')
inline static void Replace_Char(char *string, char from, char to)	// NAME 2: describes what it does
{

	G_ASSERT(string);
	for (char *p = strchr(string, from); p != NULL; p = strchr(p, from)) {
		*p = to;
	}
}






















































// (inlined in 0x4BB4C0)
inline static const char *Make_Key(const char *section_base, const char *section, const char *entry)	// NAME 2: describes what it does
{
	G_ASSERT(section_base);
	G_ASSERT(section);
	G_ASSERT(entry);
	strcpy(KeyBuffer, section);
	strcat(KeyBuffer, "\\");
	strcat(KeyBuffer, entry);






	unsigned int base_len = strlen(section_base);
	G_ASSERT(strlen(KeyBuffer) > base_len);
	G_ASSERT(strnicmp(section_base, KeyBuffer, base_len) == 0);
	G_ASSERT(KeyBuffer[base_len] == '\\');
	Replace_Char(KeyBuffer, '\\', '.');
	return(KeyBuffer + base_len + 1);
}





// 0x4BB4C0
static bool Add_Section(KVTripleListClass &list, INISection *section, const char *section_base)	// NAME 2: describes what it does
{

	G_ASSERT(section);
	bool ok = true;
	for (INIEntry *entry = section->EntryList.First_Valid(); entry != NULL; entry = entry->Next_Valid()) {
		bool error = false;
		const char *key = Make_Key(section_base, section->Section, entry->Entry);
		char value[0x400];
		strcpy(value, entry->Value);
		const char *type;
		const char *data;
		char *token = strtok(value, "{}");
		if (token != NULL) {
			type = token;
		} else {
			type = NULL;
			error = true;
		}
		token = strtok(NULL, ":{}");
		if (token != NULL) {
			data = token;
		} else {
			data = type;
			type = "unknown";
		}
		if (!error) {
			list.Add(new KVTripleClass(key, data, type));
		} else {
			ok = false;
		}
	}
	return(ok);
}




























































// 0x4BB3F0
bool Unknown_4BB3F0(KVTripleListClass &list, INIClass &ini, const char *section_base)	// NAME 1: neutral (D10)
{

	G_ASSERT(section_base);
	bool found = false;
	unsigned int base_len = strlen(section_base);
	for (INISection *section = ini.Get_Section_List().First_Valid(); section != NULL; section = section->Next_Valid()) {
		if (strncmp(section->Section, section_base, base_len) == 0 && (section->Section[base_len] == '\\' || section->Section[base_len] == '\0')) {
			Add_Section(list, section, section_base);
			found = true;
		}
	}
	return(found);
}

/*
**	Notes (X2, 2026-11-14).
**	The client's file per its asserts ('e:\build\beta\G\app\shared\kv_util.cpp', 0xB73A9C). Rebuilt here: the reader
**	0x4BB3F0 (line 206) and its helper 0x4BB4C0 (line 111, with the key builder of lines 83-98 and the '\' -> '.' helper of
**	line 21 inlined). The reader takes every section named section_base or section_base\..., and each entry
**	'name={type}:{value}' (or '{value}') of it becomes a KVTripleClass(key, value, type) whose key is the section's
**	rest and the entry's name joined with '.'; skill_ability_names.cpp's 0x814A30 fills the abilities from it.
**	Not rebuilt yet: the writer 0x4BB0E0 (line 188; its section-name builder of lines 45-47 inlined, static buffer
**	0xBE4E98, '{%s}:{%s}' through INIClass::Put_String) over kv_list.h's list. The lines left blank are theirs.
*/
