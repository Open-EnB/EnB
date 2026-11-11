// Earth & Beyond client (reconstruction): app\common\item_effect.cpp
// Client 0x834D30 on: an item's effect (an effect description with its usage and flags). Notes at the end of the file.
#include "effect_description.h"
#include "..\packets\pk.h"

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

static std::string UsageSeparator("|");	// 0xBF32F8 (between the names of Value_To_String(EffectGeneratorUsageBF) 0x835590, not rebuilt)  // NAME 1: what it is

// A value's name
struct EffectGeneratorUsageNameStruct  // NAME 1: what it is
{
	EffectGeneratorUsageNameStruct(int value, const char *name) : Value(value), Name(name) {}	// 0x8351E0

	int Value;	// NAME 1: what it holds
	std::string Name;	// NAME 1: what it holds
};

static EffectGeneratorUsageNameStruct EffectGeneratorUsageNames[4] = {	// 0xBF3308 (Value_To_String 0x835590's table)  // NAME 1: what it is
	EffectGeneratorUsageNameStruct(0, "none"),
	EffectGeneratorUsageNameStruct(1, "activated"),
	EffectGeneratorUsageNameStruct(2, "permanent"),
	EffectGeneratorUsageNameStruct(4, "toggled")
};


/*
**	Reconstruction notes
**
**	A separate object from effect_description.cpp: ItemEffectClass's constructor 0x834D30 and Get_Size 0x834F20 call
**	EffectDescriptionClass's constructor and Get_Size, which that object inlines into its own callers, and this object
**	repeats <xlocale>'s guarded static initialiser (0x835010, as effect_description.cpp's 0x834CB0). Then the usage
**	names' separator '|' and table (their initialisers 0x835040 / 0x835170, the entry's constructor 0x8351E0) that
**	Value_To_String(EffectGeneratorUsageBF) 0x835590, not rebuilt, reads; Type_Name 0x835370 and String_To_Value
**	0x835390 (aux_roots.h) sit in this object too. The file name is assumed: no assert names it.
*/
