// Earth & Beyond client (reconstruction): app\shared\unique_design_id_lookup.cpp. Notes at the end of the file.
#include <ctype.h>
#include <sstream>
#include <algorithm>
#include "unique_design_id_lookup.h"
#include "unknown_debug.h"
#include "..\..\wwlib\inisup.h"



















UniqueDesignIDLookupClass::UniqueDesignIDLookupClass(const std::string &filename, const std::string &section_name) :
	Filename(filename),
	Section(section_name),
	Map(100)
{
	FAIL_IF(!INI.Load(filename.c_str())) return;
	INISection *section = INI.Find_Section(section_name.c_str());

	FAIL_IF(!section) return;
	for (INIEntry *entry = section->EntryList.First_Valid(); entry != NULL; entry = entry->Next_Valid()) {
		int id = INVALID_ID;
		std::stringstream stream(std::string(entry->Entry));
		stream >> id;
		if (id == INVALID_ID) {
			if (entry->Value != NULL) {




				G_ASSERT(*entry->Value == '~');
			}
			continue;
		}
		std::string identifier(entry->Value);
		std::transform(identifier.begin(), identifier.end(), identifier.begin(), tolower);
		Map[std::string(identifier)] = id;
	}
}


UniqueDesignIDLookupClass::~UniqueDesignIDLookupClass()
{
	INI.Save(Filename.c_str());
}







int UniqueDesignIDLookupClass::Get_ID(const std::string &name)
{
	std::string identifier(name);
	std::transform(identifier.begin(), identifier.end(), identifier.begin(), tolower);
	std::hash_map<std::string, int>::iterator find_result = Map.find(identifier);
	FAIL_IF(find_result == Map.end()) return INVALID_ID;
	return find_result->second;
}


std::string UniqueDesignIDLookupClass::Get_Identifier(int id)
{
	for (std::hash_map<std::string, int>::iterator it = Map.begin(); it != Map.end(); ++it) {
		if (it->second == id) {
			return it->first;
		}
	}
	return std::string("");
}





// NOT EXACT: inline budget, the client calls the pair constructor 0x5024A0 out of line in Map[identifier] (frame 0xC0, ours 0xBC)
void UniqueDesignIDLookupClass::Add_Identifier(int id, const std::string &identifier)
{
	std::string key(identifier);
	std::transform(key.begin(), key.end(), key.begin(), tolower);
	std::stringstream stream;
	stream << id;
	std::hash_map<std::string, int>::iterator find_result = Map.find(key);
	if (find_result != Map.end()) {
		G_ASSERT(identifier == find_result->first);
	} else {
		INI.Put_String(Section.c_str(), stream.str().c_str(), key.c_str());
		Map[identifier] = id;
	}
}

const int UniqueDesignIDLookupClass::INVALID_ID = 0;

/*
**	Reconstruction notes
**
**	client.exe 0x4FFC70-0x500760, then the iostream, string and table instances to about 0x509C40
**	(dbgmgr.cpp follows): the constructor 0x4FFC70 (the reports on the client's lines 32 / 35, the
**	assert on 46: each entry 'id=name' of the section read through a std::stringstream, names
**	lower-cased with std::transform), the destructor 0x5002C0 (the ini saved back), Get_ID 0x500470
**	(line 73), Get_Identifier 0x5005F0 and the virtual Add_Identifier 0x500760 (line 101).
**	INVALID_ID is the client's 0xAECC44 (0, read from memory: not folded; 0x838970-0x838B80 read it
**	too). Nothing in the client constructs the class (no caller of the constructor). The VC6
**	iostream instances compile from the toolchain's own headers; operator<<(int) 0x500AB0,
**	operator>>(int &), stringstream::str() and stringbuf::str() differ from the client's (shape,
**	library code). NOT EXACT: Add_Identifier 0x500760 (inline budget).
*/
