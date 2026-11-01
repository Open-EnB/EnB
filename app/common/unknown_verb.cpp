// Earth & Beyond client (reconstruction): the verbs' names (VerbEnum), client 0x84C450-0x84C5A2.
// PROVISIONAL file name (D10): its own object between vendor_defs.cpp and weapontype.cpp (its data
// 0xBB1218-0xBB1350 lies between theirs), no assert names it. Notes at the end of the file.
#include "unknown_verb.h"
#include <string.h>

struct VerbNameStruct  // NAME 1: what it holds
{
	int Code;	// NAME 1: what it holds
	char *Name;	// NAME 1: what it holds
};

// The verbs (the high 16 bits of a VerbEnum) and their names
static VerbNameStruct VerbNames[] = {	// 0xBB1218  // NAME 1: what it holds
	{ 0x00000, "NA" },
	{ 0x10000, "Scan" },
	{ 0x20000, "Land" },
	{ 0x40000, "Group" },
	{ 0x50000, "Message" },
	{ 0x60000, "Trade" },
	{ 0x70000, "Tractor" },
	{ 0x80000, "Dock" },
	{ 0x30000, "Loot" },
	{ 0x90000, "Prospect" },
	{ 0xA0000, "Gate" },
	{ 0xB0000, "Register" },
	{ 0xC0000, "Jumpstart" },
	{ 0xD0000, "Follow" }
};

// The low 16 bits' texts (why a verb is unavailable)
static VerbNameStruct VerbReasons[] = {	// 0xBB1288  // NAME 1: what it holds
	{ 0x0001, ": Player already in a group" },
	{ 0x0002, ": Too Far" },
	{ 0xFFFF, ": Unavailable" }
};

// 0x84C450
const char *Type_Name(VerbEnum *)
{
	return("VerbEnum");
}

// 0x84C470
// NOT EXACT: 0.65, prologue: the client saves ebx / edi only in the found path (their pushes after the search loop)
char *Value_To_String(char *buf, int size, VerbEnum verb)
{
	for (int i = 0; i < (int)(sizeof(VerbNames) / sizeof(VerbNames[0])); i++) {
		if (VerbNames[i].Code == (verb & 0xFFFF0000)) {
			strncpy(buf, VerbNames[i].Name, size);
			size -= strlen(buf);
			if ((verb & 0xFFFF) != 0 && size > 0) {
				for (int j = 0; j < (int)(sizeof(VerbReasons) / sizeof(VerbReasons[0])); j++) {
					if (VerbReasons[j].Code == (verb & 0xFFFF)) {
						strncpy(buf + strlen(buf), VerbReasons[j].Name, size);
						return(buf);
					}
				}
			}
			return(buf);
		}
	}
	strncpy(buf, "unknown", size);
	return(buf);
}

// 0x84C560
bool String_To_Value(const char *text, VerbEnum *value)
{
	for (int i = 0; i < (int)(sizeof(VerbNames) / sizeof(VerbNames[0])); i++) {
		if (stricmp(VerbNames[i].Name, text) == 0) {
			*value = (VerbEnum)VerbNames[i].Code;
			return(true);
		}
	}
	return(false);
}

/*
**	Notes (reconstruction)
**
**	The object: Type_Name 0x84C450, Value_To_String 0x84C470 (called by the verb packets' Print and
**	the verb interface), String_To_Value 0x84C560. Its data follows vendor_defs.cpp's and precedes
**	weapontype.cpp's, so it is its own object; the link order of app\common is alphabetical, so the
**	original name lies between vendor_defs.cpp and weapontype.cpp (verb*.cpp, perhaps).
*/
