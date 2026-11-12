// Earth & Beyond client (reconstruction): app\common\effect_description.cpp
// Client 0x8320D0-0x834D30: an effect's texts and the values their tags show. Notes at the end of the file.
#include "effect_description.h"
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include "..\packets\pk.h"
#include "..\shared\g_assert.h"

void Unknown_4FC2F0(std::vector<std::string> &list, const std::string &string, const std::string &separator);	// 0x4FC2F0: splits string at separator  // NAME 1: neutral placeholder (D10)

// 0x8320D0
EffectDescriptionClass::EffectDescriptionClass() :
	Unknown00(""),
	Name(""),
	Description("")
{
}

// 0x832200
EffectDescriptionClass::EffectDescriptionClass(const std::string &unknown, const std::string &name, const std::string &description) :
	Unknown00(unknown),
	Name(name),
	Description(description)
{
}

// 0x832310
void EffectDescriptionClass::Add_Name_Value(float value)
{
	NameValues.push_back(value);
}

// 0x832530
void EffectDescriptionClass::Add_Description_Value(float value)
{
	DescriptionValues.push_back(value);
}

// 0x832750
void EffectDescriptionClass::Get_Name_Values(std::vector<float> &values) const
{
	values = NameValues;
}

// 0x832930
void EffectDescriptionClass::Get_Description_Values(std::vector<float> &values) const
{
	values = DescriptionValues;
}

// 0x832B10: the three strings, then each value list as its count and the values
int EffectDescriptionClass::Get_Size()
{
	int size = RawPacketClass::Get_Size(Unknown00);	// NAME 1 [size]: neutral
	size += RawPacketClass::Get_Size(Name);
	size += RawPacketClass::Get_Size(Description);
	size += sizeof(int) + NameValues.size() * sizeof(float);
	size += sizeof(int) + DescriptionValues.size() * sizeof(float);
	return(size);
}

// 0x832B90
void EffectDescriptionClass::Write(RawPacketClass *raw)
{
	raw->Add(Unknown00);
	raw->Add(Name);
	raw->Add(Description);
	int count = NameValues.size();
	raw->Add(count);
	std::vector<float>::iterator it;	// NAME 1 [it, count, value, i]: neutral
	for (it = NameValues.begin(); it != NameValues.end(); ++it) {
		float value = *it;
		raw->Add(value);
	}
	count = DescriptionValues.size();
	raw->Add(count);
	for (it = DescriptionValues.begin(); it != DescriptionValues.end(); ++it) {
		float value = *it;
		raw->Add(value);
	}
}

// 0x832CA0
void EffectDescriptionClass::Read(RawPacketClass *raw)
{
	raw->Get(&Unknown00);
	raw->Get(&Name);
	raw->Get(&Description);
	NameValues.clear();
	DescriptionValues.clear();
	int count = 0;
	raw->Get(count);
	float value = 0;
	int i;
	for (i = 0; i < count; i++) {
		raw->Get(value);
		NameValues.push_back(value);
	}
	raw->Get(count);
	for (i = 0; i < count; i++) {
		raw->Get(value);
		DescriptionValues.push_back(value);
	}
}

// 0x832E00: a text and its values (no caller in the client)
struct Unknown832E00Struct  // NAME 1: neutral (D10)
{
	Unknown832E00Struct(const std::string &text);
	std::string Text;	// NAME 1: what it holds
	std::vector<float> Values;	// NAME 1: what it holds
};
Unknown832E00Struct::Unknown832E00Struct(const std::string &text) :
	Text(text)
{
}

// 0x832F20
void EffectDescriptionClass::Set_Name(const std::string &name)
{
	Name = name;
}

// The first tag in text replaced by value_text, false when text has none (an inline layer: Replace_Value and Fill_Values
// call string::replace 0x834650 out of line through it)
inline bool Replace_Tag(std::string &text, const std::string &tag, const std::string &value_text)  // NAME 1: what it does
{
	int pos = text.find(tag);
	if (pos == std::string::npos) {
		return(false);
	}
	text.replace(pos, tag.length(), value_text);
	return(true);
}

static std::string ValueTag("%value");	// 0xBF32A8 (a value's tag: '%value', its printf format, '%')  // NAME 1: what it is





































// 0x8331E0: the first value tag of text replaced by value, false when text has none (no caller in the client)
bool Replace_Value(std::string &text, float value)  // NAME 1: what it does; NAME 1 [text, value, tag, format, buffer, value_text, pos]
{
	int match_start = text.find(ValueTag);
	FAIL_IF(match_start == std::string::npos) return(false);



	int match_end = text.find('%', match_start + 1);
	FAIL_IF(match_end == std::string::npos) return(false);
	static int tag_length = ValueTag.length();
	std::string tag = text.substr(match_start, match_end - match_start + 1);
	std::string format_end = tag.substr(tag_length);
	std::string format = std::string("%") + format_end.substr(0, format_end.length() - 1);
	char buffer[32];
	sprintf(buffer, format.c_str(), value);
	std::string value_text = buffer;
	bool replaced = Replace_Tag(text, tag, value_text);	// NAME 1 [replaced]: what it holds
	return(replaced);
}























// 0x833880 (match a pair: its implicit constructor inlined leaves the strings' 0x603BB0 out of line)
// Text's first value tag and its printf format ("%0.1f" when the tag has no end), empty strings when text has none;
// static, so the client emits it after Fill_Values, its caller
static std::pair<std::string, std::string> Find_Value_Tag(const std::string &text)  // NAME 1: what it does
{
	static int tag_length = ValueTag.length();
	std::pair<std::string, std::string> match;	// NAME 1 [match]: the tag and its format

	int match_start = text.find(ValueTag);
	if (match_start == std::string::npos) {
		return(match);
	}
	int match_end = text.find('%', match_start + 1);
	FAIL_IF(match_end == std::string::npos) {
		match.first.substr(match_start);
		match.second = "0.1f";
		return(match);
	}
	match.first = text.substr(match_start, match_end - match_start + 1);
	std::string format_end = match.first.substr(tag_length); match.second = std::string("%") + format_end.substr(0, format_end.length() - 1);	// NAME 1 [format_end]: as Replace_Value's
	return(match);
}

















// NOT EXACT: 0x8335D0 (0.57, inline budget: the client keeps ebx / edi for the loop and does not rotate it; Replace_Tag gives its out-of-line find and replace)
// Text with each value tag replaced by the next of values
static std::string Fill_Values(const std::string &text, const std::vector<float> &values)  // NAME 1: what it does
{
	std::string result = text;
	std::vector<float>::const_iterator value_iterator = values.begin();
	for (;;) {
		std::pair<std::string, std::string> match = Find_Value_Tag(result);
		if (match.first.empty()) {
			G_ASSERT(value_iterator == values.end());
			return(result);
		}


		G_ASSERT(value_iterator != values.end());
		float value = (value_iterator != values.end()) ? *value_iterator++ : 0.0f;
		char buffer[32];
		sprintf(buffer, match.second.c_str(), value);
		std::string value_text = buffer;
		Replace_Tag(result, match.first, value_text);
	}
}

// 0x833D00
std::string EffectDescriptionClass::Get_Name_Text() const
{
	return(Fill_Values(Name, NameValues));
}

// 0x833D40
std::string EffectDescriptionClass::Get_Description_Text() const
{
	return(Fill_Values(Description, DescriptionValues));
}
static std::string NameDescriptionSeparator("^");	// 0xBF32D8 (between a summary's name and description values)  // NAME 1: what it is
static std::string ValueSeparator(",");				// 0xBF32C0 (after each value of a summary)  // NAME 1: what it is
// 0x833FE0: the name values, then the description values, each followed by ValueSeparator
std::string EffectDescriptionClass::Get_Summary() const
{
	std::stringstream summary;	// NAME 1 [summary, it, value, out]: neutral
	std::vector<float>::const_iterator it;
	for (it = NameValues.begin(); it != NameValues.end(); ++it) {
		float value = *it;
		std::ostream &out = summary << value;
		out << ValueSeparator.c_str();
	}
	summary << NameDescriptionSeparator.c_str();
	for (it = DescriptionValues.begin(); it != DescriptionValues.end(); ++it) {
		float value = *it;
		std::ostream &out = summary << value;
		out << ValueSeparator.c_str();
	}
	return(summary.rdbuf()->str());
}




// Each value from its string
inline void Set_Values(const std::vector<std::string> &strings, std::vector<float> &values)  // NAME 1: what it does; NAME 1 [value, it]
{
	G_ASSERT(strings.size() == values.size());
	std::vector<float>::iterator value = values.begin();
	for (std::vector<std::string>::const_iterator it = strings.begin(); it != strings.end() && value != values.end(); ++it, ++value) {
		*value = atof(it->c_str());
	}
}







// 0x8342A0: the values from a Get_Summary string
void EffectDescriptionClass::Set_Summary(const std::string &summary)
{
	std::vector<std::string> name_and_description;
	Unknown_4FC2F0(name_and_description, summary, NameDescriptionSeparator);
	FAIL_IF(name_and_description.size() != 2) return;
	const std::string &name_values = name_and_description[0];	// NAME 1 [name_values, description_values]: what they hold
	const std::string &description_values = name_and_description[1];
	std::vector<std::string> name_value_strings;
	std::vector<std::string> description_value_strings;
	Unknown_4FC2F0(name_value_strings, name_values, ValueSeparator);
	Unknown_4FC2F0(description_value_strings, description_values, ValueSeparator);




	G_ASSERT(name_value_strings.size() == NameValues.size());
	G_ASSERT(description_value_strings.size() == DescriptionValues.size());
	Set_Values(name_value_strings, NameValues);
	Set_Values(description_value_strings, DescriptionValues);
}

/*
**	Reconstruction notes
**
**	The object: EffectDescriptionClass in source order 0x8320D0-0x8342A0 with the statics' initialisers after their
**	definitions, then its template and <sstream> instances and <xlocale>'s guarded statics 0x834CB0. ItemEffectClass
**	(0x834D30 on) is another object, item_effect.cpp: it calls the constructor and Get_Size this object inlines.
**	The names NameValues, DescriptionValues, match_start, match_end, value_iterator, values, strings, name_and_description, name_value_strings, description_value_strings and summary_strings are
**	the asserts'. 0x832E00 (between Read and Set_Name, no caller and no reference in the client) is the constructor
**	of a text with its values, a struct this file defines; Find_Value_Tag 0x833880 builds its result in a pair local
**	(the client calls the strings' allocator constructor 0x603BB0 from the pair's inlined default constructor).
*/
