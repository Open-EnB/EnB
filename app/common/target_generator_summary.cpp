// Earth & Beyond client (reconstruction): the object at client 0x84B360 (INTERIM file name, see the notes at the end).
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>

struct TargetSummaryText	// NAME 2: what it holds
{
	std::string Scope;		// +0x00 ("to group", "to targets", ...)  // NAME 2: what it holds
	std::string Radius;		// +0x10 ("Effect Radius : %d")  // NAME 2: what it holds
	int Unknown20;			// +0x20 (only ever copied here, never initialised)
	int Unknown24;			// +0x24 (likewise)
};

// 0x84B360 (the implicit destructor is 0x84B8A0, the implicit copy constructor 0x84B930)
TargetSummaryText Target_Summary_Text(unsigned char flags, int radius)	// NAME 2: what it does; NAME 1 [flags, radius]: neutral
{
	TargetSummaryText text;
	char buffer[128];
	sprintf(buffer, "Effect Radius : %d", radius);
	if (flags & 2) {
		text.Scope = "to group";
		text.Radius.assign(buffer);
		return(text);
	}
	if (flags & 0x10) {
		if (flags & 4) {
			text.Scope = "to targets";
			text.Radius.assign(buffer);
			return(text);
		}
		if (flags & 8) {
			text.Scope = "to targets";
			return(text);
		}
		text.Scope = "to target";
		return(text);
	}
	if (flags & 0x20) {
		if (flags & 4) {
			text.Scope = "to enemies";
			text.Radius.assign(buffer);
			return(text);
		}
		if (flags & 8) {
			text.Scope = "to enemies";
			text.Radius.assign(buffer);
			return(text);
		}
		text.Scope = "to enemy";
		return(text);
	}
	return(text);
}

// aux_roots.h's, for these three (that header is the generated roots', far too much for this object)
enum TargetGeneratorSummaryBF { UNKNOWN_B0189C_0 };	// NAME 5: the client's Get_Type_Name string 'TargetGeneratorSummaryBF'

void Unknown_4FC2F0(std::vector<std::string> &list, const std::string &string, const std::string &separator);	// 0x4FC2F0: splits string at separator  // NAME 1: neutral
char *Unknown_4A1590(char *buffer, int size, const char *string);	// 0x4A1590: a bounded strcpy  // NAME 1: neutral placeholder (D10)

static std::string Unknown_BF3F80("|");	// 0xBF3F80 (the separator of the bits' names)  // NAME 1: neutral (D10)

// One bit's name (0x14 bytes)
struct TargetGeneratorFlagStruct	// NAME 1: neutral (D10)
{
	// 0x84BCB0 (the implicit destructor is 0x84BDF0, the array's atexit 0x84BDC0)
	TargetGeneratorFlagStruct(unsigned int flag, const char *name) : Flag(flag), Name(name) {}	// NAME 2 [flag, name]: what they hold

	unsigned int Flag;		// +0x00  // NAME 2: what it holds
	std::string Name;		// +0x04  // NAME 2: what it holds
};

// 0xBF3F90 (the client gives 'enemy' 8, as 'pointblank', where 0x84B360 reads 0x20)
static TargetGeneratorFlagStruct Unknown_BF3F90[7] = {	// NAME 1: neutral (D10)
	TargetGeneratorFlagStruct(0, "none"),
	TargetGeneratorFlagStruct(1, "self"),
	TargetGeneratorFlagStruct(2, "group"),
	TargetGeneratorFlagStruct(4, "aoe"),
	TargetGeneratorFlagStruct(8, "pointblank"),
	TargetGeneratorFlagStruct(0x10, "friend"),
	TargetGeneratorFlagStruct(8, "enemy"),
};

// @0x84BE40
const char *Type_Name(TargetGeneratorSummaryBF)
{
	return("TargetGeneratorSummaryBF");
}

// @0x84BE60
// NOT EXACT: inline-budget (the temporary string's _Eos stays out of line in the client, and value lives in ebx)
bool String_To_Value(const char *text, TargetGeneratorSummaryBF *value)
{
	*value = (TargetGeneratorSummaryBF)0;
	std::vector<std::string> list;
	Unknown_4FC2F0(list, std::string(text), Unknown_BF3F80);
	bool found = false;
	for (std::vector<std::string>::iterator it = list.begin(); it != list.end(); ++it) {
		for (int i = 0; i < 7; i++) {
			const TargetGeneratorFlagStruct &entry = Unknown_BF3F90[i];
			if (_stricmp(entry.Name.c_str(), it->c_str()) == 0) {
				*value = (TargetGeneratorSummaryBF)(*value | entry.Flag);
				found = true;
				break;
			}
		}
	}
	return(found);
}

// @0x84C060
char *Value_To_String(char *buf, int size, TargetGeneratorSummaryBF value)
{
	std::string text;
	for (const TargetGeneratorFlagStruct *entry = Unknown_BF3F90; entry < Unknown_BF3F90 + 7; entry++) {
		if (entry->Flag != 0 && (value & entry->Flag) == entry->Flag) {
			text += entry->Name + Unknown_BF3F80;
		}
	}
	if (text.size() == 0) {
		return(NULL);
	}
	return(Unknown_4A1590(buf, size, text.c_str()));
}

/*
**	Reconstruction notes. The client holds no __FILE__ for this object: it starts after stredit.cpp's <string>
**	initialiser (0x84B330) and sits before vendor_defs.cpp (0x84C270), and it has no caller. Its neighbours 0x84BE40-0x84C060
**	are the converters of TargetGeneratorSummaryBF (aux_roots.h), whose 'self / group / aoe /
**	pointblank / friend / enemy' names are the bits read here (2 group, 4 aoe, 8 pointblank, 0x10 friend, 0x20 enemy): hence the
**	interim file name, D137, NAME 3. `text.Radius.assign(buffer)` against `= buffer`: the operator's extra inline level
**	put the seventh return's destructor out of line and merged it with the last return (0.82); assign on Scope instead of
**	Radius is exact as well, so which of the two the original used is not decided by the bytes.
*/
