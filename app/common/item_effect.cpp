// Earth & Beyond client (reconstruction): app\common\item_effect.cpp
// Client 0x834D30 on: an item's effect (an effect description with its usage and flags). Notes at the end of the file.
#include "effect_description.h"
#include "..\packets\pk.h"
#include <string.h>

#include "..\shared\unknown_4fac20.h"
#include "..\shared\unknown_4a1590.h"

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
void ItemEffectClass::Write(PacketClass *raw)
{
	EffectDescriptionClass::Write(raw);
	raw->Add(&Usage, sizeof(Usage));
	raw->Add(&Flags, sizeof(Flags));
}

// 0x834F80
void ItemEffectClass::Read(PacketClass *raw)
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

/*
**	Reconstruction notes
**
**	SG (run 2026-11-18): two objects by the CRT initialiser table: ItemEffectClass's members end with <string>'s guard
**	0x835010 (entry 761); the usage names, converters and bit field below are unknown_835040.cpp (entries 762-764).
**	A separate object from effect_description.cpp: ItemEffectClass's constructor 0x834D30 and Get_Size 0x834F20 call
**	EffectDescriptionClass's constructor and Get_Size, which that object inlines into its own callers, and this object
**	repeats <xlocale>'s guarded static initialiser (0x835010, as effect_description.cpp's 0x834CB0). Then the usage
**	names' separator '|' and table (their initialisers 0x835040 / 0x835170, the entry's constructor 0x8351E0) and
**	EffectGeneratorUsageBF's converters Type_Name 0x835370, String_To_Value 0x835390 and Value_To_String 0x835590,
**	in target_generator_summary.cpp's pattern (its LV2 form, operator|= with i < sizeof / sizeof, is the client's
**	String_To_Value here). The file name is assumed: no assert names it.
**	Last, an 8-byte bit field's four functions 0x835770 (cleared), 0x835790 (both words), 0x8357C0 (|=) and
**	0x8357F0 (|), uncalled: they lie between Value_To_String 0x835590 and the <string> initialiser 0x835830
**	that closes this object (faction_and_reputation.cpp's notes), so they are this file's; which bits they
**	hold is not shown. The forms of |= and | are the ones that match (plain 'Low |= other.Low' schedules the
**	second load earlier; a plain | reads a's words first).
*/
