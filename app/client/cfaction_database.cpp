// Earth & Beyond client (reconstruction): app\Client\cfaction_database.cpp. Notes at the end of the file.
#include <stdlib.h>
#include "cfaction_database.h"
#include "..\shared\unknown_debug.h"
#include "..\..\wwlib\ini.h"
#include "..\..\wwlib\inisup.h"

std::hash_map<std::string, CFactionDatabaseClass::FactionStruct> CFactionDatabaseClass::_Factions;
int CFactionDatabaseClass::UseCount;
// NOT EXACT: register-only in the first string assignment (the client moves the length to ecx before the _Eos store); explain.py says shape (195 vs 197 instructions)
void CFactionDatabaseClass::Init()
{
	if (UseCount == 0) {
		INIClass ini("cfaction_t.ini");
		INISection *next;
		for (INISection *section = ini.Get_Section_List().First_Valid(); section != 0; section = next) {
			char *section_name = section->Section;
			next = section->Next_Valid();
			FactionStruct &faction = _Factions[std::string(section_name)];
			char *name = ini.Get_Alloc_String(section_name, "Name", section_name);
			faction.Name = name;
			free(name);
			char *description = ini.Get_Alloc_String(section_name, "Description", section_name);
			faction.Description = description;
			free(description);
		}
	}
	UseCount++;
}

void CFactionDatabaseClass::Shutdown()
{
	UseCount--;
	if (UseCount == 0) {
		_Factions.clear();
	}
}

static std::string Empty;  // NAME 2: what Get_Name and Get_Description return for an unknown faction

const std::string &CFactionDatabaseClass::Get_Name(const std::string &faction)
{
	std::hash_map<std::string, FactionStruct>::iterator it = _Factions.find(faction);
	if (it == _Factions.end()) return Empty;
	return (*it).second.Name;
}
















































const std::string &CFactionDatabaseClass::Get_Description(const std::string &faction)
{
	std::hash_map<std::string, FactionStruct>::iterator it = _Factions.find(faction);
	FAIL_IF(it == _Factions.end()) return Empty;
	return (*it).second.Description;
}

/*
**	Reconstruction notes
**
**	client.exe 0x7E4A30-0x7E50D0 (then the table's instances to 0x7E5E50): the object opens with
**	<string>'s guarded statics (0x7E4A30); _Factions's initializer 0x7E4AB0 calls the table's
**	constructor 0x7E5170 out of line and registers 0x7E4AE0, which calls the destructor 0x7E4B10.
**	Init 0x7E4BE0 loads cfaction_t.ini once (the use count 0xBE8B24): per section,
**	`_Factions[std::string(section)]` (operator[]'s value_type temporary: the struct's two strings from
**	the out-of-line 0x603BB0), then the Name and Description strings (Get_Alloc_String with the section
**	name as default, freed after the copy). Shutdown 0x7E4F00 empties the table on the last call.
**	The static empty string (0xBE8AF8; its initializer 0x7E4F90 and destructor 0x7E4FD0 sit between
**	Shutdown and Get_Name, so it is defined there) is what Get_Name 0x7E5040 and Get_Description
**	0x7E50D0 return for an unknown faction; only Get_Description FAIL_IFs (line 98).
**	Not reproduced: the table constructor's copy 0x7E5170 (370 bytes, __stl_prime_list's lower_bound
**	inlined; ours, 358 bytes, matches the identical copies at 0x429630 / 0x4D42C0 / 0x79DE00).
**	Callers: 0x58D080 (Init), 0x58D1E0 (Shutdown), 0x58E740 / 0x58EFC0 / 0x60F1A0 (Get_Name),
**	0x58EFC0 (Get_Description).
*/
