// Earth & Beyond client (reconstruction): app\shared\unknown_4f5570.cpp (PROVISIONAL file name, D137). Notes at the end.
#include <string.h>
#include <string>
#include <vector>

#include "unknown_4fac20.h"
#include "unknown_4a1590.h"

enum ValidityRequirementBitFlag {	// NAME 5: client string 'ValidityRequirementBitFlag' (Type_Name 0x4F5880)
	VALIDITY_REQUIREMENT_IRRELEVANT = 0,	// NAME 2: from the client's table at 0xBE5678
	VALIDITY_REQUIREMENT_NECESSARY = 1,		// NAME 2: from the client's table at 0xBE5678
	VALIDITY_REQUIREMENT_SUFFICIENT = 2,	// NAME 2: from the client's table at 0xBE5678
};

static std::string ValidityRequirementSeparator("|");	// 0xBE56B8 (between the names of Value_To_String 0x4F5AA0)  // NAME 1: what it is

// One bit's name (0x14 bytes)
struct ValidityRequirementNameStruct	// NAME 1: what it is
{
	// 0x4F56F0 (implicit destructor 0x4F5830, array atexit 0x4F5800)
	ValidityRequirementNameStruct(unsigned int flag, const char *name) : Flag(flag), Name(name) {}	// NAME 2 [flag, name]: what they hold

	unsigned int Flag;		// +0x00  // NAME 2: what it holds
	std::string Name;		// +0x04  // NAME 2: what it holds
};

// 0xBE5678 (initialised by 0x4F56A0)
static ValidityRequirementNameStruct ValidityRequirementNames[3] = {	// NAME 1: what it is
	ValidityRequirementNameStruct(1, "necessary"),
	ValidityRequirementNameStruct(2, "sufficient"),
	ValidityRequirementNameStruct(0, "irrelevant"),
};

// The bits' or (an inline layer the client spends budget on, as game_action_validity.cpp's)
inline ValidityRequirementBitFlag &operator|=(ValidityRequirementBitFlag &value, unsigned int bits)
{
	value = (ValidityRequirementBitFlag)(value | bits);
	return(value);
}

// @0x4F5880
const char *Type_Name(ValidityRequirementBitFlag)
{
	return("ValidityRequirementBitFlag");
}

// @0x4F58A0
bool String_To_Value(const char *text, ValidityRequirementBitFlag *value)
{
	*value = (ValidityRequirementBitFlag)0;
	std::vector<std::string> list;	// NAME 1 [list, found, it, i, entry]: neutral
	String_Split(list, std::string(text), ValidityRequirementSeparator);
	bool found = false;
	for (std::vector<std::string>::iterator it = list.begin(); it != list.end(); ++it) {
		for (int i = 0; i < sizeof(ValidityRequirementNames) / sizeof(ValidityRequirementNames[0]); i++) {
			const ValidityRequirementNameStruct &entry = ValidityRequirementNames[i];
			if (_stricmp(entry.Name.c_str(), it->c_str()) == 0) {
				*value |= entry.Flag;
				found = true;
				break;
			}
		}
	}
	return(found);
}

// @0x4F5AA0
char *Value_To_String(char *buf, int size, ValidityRequirementBitFlag value)
{
	std::string text;	// NAME 1 [text, entry, seen]: neutral
	ValidityRequirementBitFlag seen = (ValidityRequirementBitFlag)0;
	for (const ValidityRequirementNameStruct *entry = ValidityRequirementNames; entry < ValidityRequirementNames + 3; entry++) {
		if (entry->Flag != 0 && (entry->Flag & seen) != entry->Flag && (entry->Flag & value) == entry->Flag) {
			text += entry->Name + ValidityRequirementSeparator;
			seen |= entry->Flag;
		}
	}
	if (text.empty()) {
		return(NULL);
	}
	return(Bounded_Strcpy(buf, size, text.c_str()));
}

/*
**	Reconstruction notes. The client holds no __FILE__ or assert for this object: it starts after
**	orbitalmanager.cpp's <string> initialiser (0x4F5540) and ends with its own <string> initialiser
**	(0x4F5DE0), before safe_ptr.cpp (0x4F5E10). It holds the separator '|' (initialiser 0x4F5570,
**	atexit 0x4F5630), the three-entry ValidityRequirementNames table (0xBE5678, initialiser 0x4F56A0,
**	element ctor 0x4F56F0, element dtor 0x4F5830, array atexit 0x4F5800), and the three converters of
**	ValidityRequirementBitFlag (Type_Name 0x4F5880, String_To_Value 0x4F58A0, Value_To_String 0x4F5AA0).
**	Hence the interim file name (D137, PROVISIONAL) between orbitalmanager and safe_ptr.
*/
