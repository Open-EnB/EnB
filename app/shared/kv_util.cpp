// Earth & Beyond client (reconstruction): app\shared\kv_util.cpp, KV lists to and from ini sections. Notes at the end.
#include <string.h>
#include "kv_tripl.h"
#include "auxdata\permstr.h"	// (its PermStringInitClass static: 0x4BB090, CRT entry 95)
#include "g_assert.h"
#include "ini.h"
#include "inisup.h"
static char KeyBuffer[0x200];		// 0xBE509C  // NAME 2: describes what it does
#include "kv_list.h"
#include "kv_pair.h"
#include <stdio.h>
#include "kv_pair_value.h"




// (inlined: 0x4BB0E0's '.' -> '\', 0x4BB4C0's '\' -> '.')
inline static void Replace_Char(char *string, char from, char to)	// NAME 2: describes what it does
{

	G_ASSERT(string);
	for (char *p = strchr(string, from); p != NULL; p = strchr(p, from)) {
		*p = to;
	}
}
















inline const char *Make_Section(const char *base, const char *append, char *section)  // NAME 2: describes what it does
{
	static char SectionBuffer[0x200];  // NAME 2: holds the converted section
	G_ASSERT(base);
	G_ASSERT(append);
	G_ASSERT(section);
	strcpy(section, base);
	strcpy(SectionBuffer, append);
	Replace_Char(SectionBuffer, '.', '\\');
	char *split = strrchr(SectionBuffer, '\\'); const char *entry;
	if (split != NULL) {
		*split = '\0'; entry = split + 1;
		strcat(section, "\\");
		strcat(section, SectionBuffer);
	} else {
		entry = SectionBuffer;
	}
	return entry;
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






































bool Unknown_4BB0E0(KVListClass &list, INIClass &ini, const char *base)  // NAME 1: neutral address
{
	unsigned int count = list.Get_Count();
	for (unsigned int i = 0; i < count; i++) {
		KVPairClass *pair = Get_KV_Pair(list.Pairs, i);



		G_ASSERT(pair);
		char section[0x200];
		const char *entry = Make_Section(base, pair->Key.Get_String(), section);
		char value[0x300];
		sprintf(value, "{%s}:{%s}", pair->Get_Type_Name(), Pair_Value_String(pair));
		if (!ini.Put_String(section, entry, value)) return false;
	}
	return true;
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
**	The writer 0x4BB0E0 (610 bytes) checks each list item (kv_list.h:73, here:188), builds the section
**	and entry through the inlined helper (asserts 45-47), formats '{%s}:{%s}', and stops on Put_String failure.
**	Its shared value conversion buffer is the private helper's 0x1000 bytes (0xBE3B60), also used by the list
**	getter 0x4B6B60. The converted-key buffer at 0xBE4E98 is distinct from the reader's KeyBuffer 0xBE509C.
**	Helper spelling/linkage and the converted-key capacity are inferred: the external inline helper gives
**	independent common storage, compatible with retail's buffer before this file's initializer 0xBE5098.
**	The object audit checks relocation roles and section offsets; full-image linker placement is not proven.
**	The checked free vector helper lives in kv_list.h at line 73; its ownership is inferred. The class
**	declarations and existing unchecked accessor remain in kv_list_core.h, used by the encoders, so the new
**	helper body is included only where needed. Neither accessor's original source spelling is recovered.
**	The lines left blank preserve the client's assertion locations.
*/
