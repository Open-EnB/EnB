// Earth & Beyond client (reconstruction): app\common\item_effect.cpp
// Client 0x834D30 on: an item's effect (an effect description with its usage and flags). Notes at the end of the file.
#include "effect_description.h"
#include "..\packets\pk.h"
#include <string.h>

// aux_roots.h's, for these three (that header is the generated roots', far too much for this object)
enum EffectGeneratorUsageBF { UNKNOWN_B01844_0 };	// NAME 5: the client's Get_Type_Name string 'EffectGeneratorUsageBF'

void Unknown_4FC2F0(std::vector<std::string> &list, const std::string &string, const std::string &separator);	// 0x4FC2F0: splits string at separator  // NAME 1: neutral
char *Unknown_4A1590(char *buffer, int size, const char *string);	// 0x4A1590: a bounded strcpy  // NAME 1: neutral placeholder (D10)

// 0x834D30
ItemEffectClass::ItemEffectClass() :
	Usage(0)
{
	*(int *)&Flags = 0;
}

// 0x834D60
ItemEffectClass::ItemEffectClass(const EffectDescriptionClass &description, int usage, ItemEffectFlagsStruct flags) :
	EffectDescriptionClass(description),
	Usage(usage),
	Flags(flags)
{
}

// 0x834F20
int ItemEffectClass::Get_Size()
{
	return(EffectDescriptionClass::Get_Size() + sizeof(Usage) + sizeof(Flags));
}

// 0x834F40
void ItemEffectClass::Write(RawPacketClass *raw)
{
	EffectDescriptionClass::Write(raw);
	raw->Add(&Usage, sizeof(Usage));
	raw->Add(&Flags, sizeof(Flags));
}

// 0x834F80
void ItemEffectClass::Read(RawPacketClass *raw)
{
	EffectDescriptionClass::Read(raw);
	raw->Get(&Usage, sizeof(Usage));
	raw->Get(&Flags, sizeof(Flags));
}

// 0x834FC0
std::string ItemEffectClass::Get_Summary() const
{
	return(EffectDescriptionClass::Get_Summary());
}

// 0x834FF0
void ItemEffectClass::Set_Summary(const std::string &summary)
{
	EffectDescriptionClass::Set_Summary(summary);
}

static std::string UsageSeparator("|");	// 0xBF32F8 (between the names of Value_To_String 0x835590)  // NAME 1: what it is

// A value's name
struct EffectGeneratorUsageNameStruct  // NAME 1: what it is
{
	EffectGeneratorUsageNameStruct(int value, const char *name) : Value(value), Name(name) {}	// 0x8351E0

	int Value;	// NAME 1: what it holds
	std::string Name;	// NAME 1: what it holds
};

static EffectGeneratorUsageNameStruct EffectGeneratorUsageNames[4] = {	// 0xBF3308 (the converters' table)  // NAME 1: what it is
	EffectGeneratorUsageNameStruct(0, "none"),
	EffectGeneratorUsageNameStruct(1, "activated"),
	EffectGeneratorUsageNameStruct(2, "permanent"),
	EffectGeneratorUsageNameStruct(4, "toggled")
};

// The bits' or (an inline layer the client spends budget on: its String_To_Value copies 0x4F58A0 / 0x835390 / 0x83D290)
inline EffectGeneratorUsageBF &operator|=(EffectGeneratorUsageBF &value, unsigned int bits)
{
	value = (EffectGeneratorUsageBF)(value | bits);
	return(value);
}

// 0x835370
const char *Type_Name(EffectGeneratorUsageBF)
{
	return("EffectGeneratorUsageBF");
}

// 0x835390: the '|'-separated names' values or'ed, false when none is known
bool String_To_Value(const char *text, EffectGeneratorUsageBF *value)
{
	*value = (EffectGeneratorUsageBF)0;
	std::vector<std::string> list;	// NAME 1 [list, found, it, i, entry]: neutral
	Unknown_4FC2F0(list, std::string(text), UsageSeparator);
	bool found = false;
	for (std::vector<std::string>::iterator it = list.begin(); it != list.end(); ++it) {
		for (int i = 0; i < sizeof(EffectGeneratorUsageNames) / sizeof(EffectGeneratorUsageNames[0]); i++) {
			const EffectGeneratorUsageNameStruct &entry = EffectGeneratorUsageNames[i];
			if (_stricmp(entry.Name.c_str(), it->c_str()) == 0) {
				*value |= entry.Value;
				found = true;
				break;
			}
		}
	}
	return(found);
}

// 0x835590: the names of value's bits, each followed by '|'
char *Value_To_String(char *buf, int size, EffectGeneratorUsageBF value)
{
	std::string text;	// NAME 1 [text, entry]: neutral
	for (const EffectGeneratorUsageNameStruct *entry = EffectGeneratorUsageNames; entry < EffectGeneratorUsageNames + 4; entry++) {
		if (entry->Value != 0 && (value & entry->Value) == entry->Value) {
			text += entry->Name + UsageSeparator;
		}
	}
	if (text.size() == 0) {
		return(NULL);
	}
	return(Unknown_4A1590(buf, size, text.c_str()));
}

/*
**	Reconstruction notes
**
**	A separate object from effect_description.cpp: ItemEffectClass's constructor 0x834D30 and Get_Size 0x834F20 call
**	EffectDescriptionClass's constructor and Get_Size, which that object inlines into its own callers, and this object
**	repeats <xlocale>'s guarded static initialiser (0x835010, as effect_description.cpp's 0x834CB0). Then the usage
**	names' separator '|' and table (their initialisers 0x835040 / 0x835170, the entry's constructor 0x8351E0) and
**	EffectGeneratorUsageBF's converters Type_Name 0x835370, String_To_Value 0x835390 and Value_To_String 0x835590,
**	in target_generator_summary.cpp's pattern (its LV2 form, operator|= with i < sizeof / sizeof, is the client's
**	String_To_Value here). The file name is assumed: no assert names it.
*/
