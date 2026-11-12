// Earth & Beyond client (reconstruction): app\common\item_effect_description.cpp
// Client 0x83DB00-0x83E7C0: an item's effects (the effects, a duration and their range and radius). Notes at the end of the file.
#include "item_effect_description.h"
#include "..\packets\pk.h"
#include "..\shared\g_assert.h"

#include "..\shared\unknown_4fac20.h"

// 0x83DB00
ItemEffectDescriptionClass::ItemEffectDescriptionClass() :
	Duration(0),
	Unknown18(0),
	Range(0),
	Radius(0)
{
}

// 0x83DB40: the duration if an effect has one, the parameters' range and radius if an effect uses them
ItemEffectDescriptionClass::ItemEffectDescriptionClass(const std::vector<ItemEffectClass> &effects, DurationClass duration, int unused, ItemEffectParameterClass *parameters) :
	Effects(effects),
	Duration(0),
	Unknown18(0),
	Range(0),
	Radius(0)
{
	bool has_range = false;	// NAME 1 [has_range, has_radius, has_duration, range, radius, it]: what they hold
	bool has_radius = false;
	bool has_duration = false;
	float range = 0;
	float radius = 0;
	parameters->Get("Range", &range);
	parameters->Get("Radius", &radius);
	for (std::vector<ItemEffectClass>::const_iterator it = effects.begin(); it != effects.end(); ++it) {
		switch (it->Usage) {	// a switch: the client loads the value and tests it with dec, not cmp
			case 1:	// 'activated' (item_effect.cpp's usage names)
				has_duration = true;
				break;
		}
		if (it->Is_Unknown1()) {
			has_radius = true;
		}
		if (it->Is_Unknown2()) {
			has_range = true;
			has_radius = true;
		}
		if (it->Is_Unknown3()) {
			has_radius = true;
		}
		if (it->Is_Unknown4()) {
			has_range = true;
		}
	}
	if (has_duration) {
		Duration = duration;
	}
	if (has_range) {
		Range = range + 0.5f;
	}
	if (has_radius) {
		Radius = radius + 0.5f;
	}
}

// 0x83DD40
void ItemEffectDescriptionClass::Clear_Effects()
{
	Effects.erase(Effects.begin(), Effects.end());
}

// 0x83DE30
bool ItemEffectDescriptionClass::Has_Effects() const
{
	return(Effects.size() != 0);
}

// 0x83DE70: the count; with effects, each effect, the duration (an int) and the three numbers
int ItemEffectDescriptionClass::Get_Size()
{
	int count = Effects.size();
	int size = sizeof(count);
	if (count == 0) {
		return(size);
	}
	for (std::vector<ItemEffectClass>::iterator it = Effects.begin(); it != Effects.end(); ++it) {
		size += it->Get_Size();
	}
	int time = Duration.As_Int();
	size += sizeof(time);
	size += sizeof(Unknown18);
	size += sizeof(Range);
	size += sizeof(Radius);
	return(size);
}

// 0x83DF10
void ItemEffectDescriptionClass::Write(RawPacketClass *raw)
{
	int count = Effects.size();
	raw->Add(count);
	if (count == 0) {
		return;
	}
	for (std::vector<ItemEffectClass>::iterator it = Effects.begin(); it != Effects.end(); ++it) {
		it->Write(raw);
	}
	int time = Duration.As_Int();
	raw->Add(time);
	raw->Add(Unknown18);
	raw->Add(Range);
	raw->Add(Radius);
}

// 0x83E040
void ItemEffectDescriptionClass::Read(RawPacketClass *raw)
{
	int count = 0;
	raw->Get(count);
	if (count == 0) {
		return;
	}
	Clear_Effects();
	for (int i = 0; i < count; i++) {
		ItemEffectClass effect;
		effect.Read(raw);
		Effects.push_back(effect);
	}
	int time = 0;
	raw->Get(time);
	Duration = time;
	raw->Get(Unknown18);
	raw->Get(Range);
	raw->Get(Radius);
}

static std::string SummarySeparator("#");	// 0xBF34C8 (its initialiser 0x83E270, its destructor 0x83E330)  // NAME 1: what it is

// 0x83E3A0: each effect's summary followed by the separator
std::string ItemEffectDescriptionClass::Get_Summary() const
{
	std::string summary;
	for (std::vector<ItemEffectClass>::const_iterator it = Effects.begin(); it != Effects.end(); ++it) {
		std::string effect = it->Get_Summary();
		summary += effect + SummarySeparator;
	}
	return(summary);
}




















































// 0x83E5F0: the summaries Get_Summary joined, one per effect
void ItemEffectDescriptionClass::Set_Summary(const std::string &summary)
{
	std::vector<std::string> summary_strings;
	String_Split(summary_strings, summary, SummarySeparator);
	G_ASSERT(summary_strings.size() == Effects.size());
	std::vector<std::string>::iterator s = summary_strings.begin();	// NAME 1 [s, it]: neutral
	for (std::vector<ItemEffectClass>::iterator it = Effects.begin(); it != Effects.end() && s != summary_strings.end(); ++it, ++s) {
		it->Set_Summary(*s);
	}
}

/*
**	Reconstruction notes
**
**	The object: ItemEffectDescriptionClass in source order 0x83DB00-0x83E5F0 with SummarySeparator's initialiser and
**	destructor (0x83E270 / 0x83E330) after Read, then std::vector<ItemEffectClass>'s destructor 0x83E770. Its other
**	vector<ItemEffectClass> instances (0x83E7C0 on) are item_info.cpp's. The names Effects and summary_strings are the
**	G_ASSERT's; Range and Radius the strings 0x83DB40 asks its parameters for.
*/
