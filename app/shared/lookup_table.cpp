// Earth & Beyond client (reconstruction): app\shared\lookup_table.cpp. Notes at the end of the file.
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include "..\shared\unknown_debug.h"
#include "lookup_table.h"
#include "..\..\wwlib\ini.h"
#include "..\..\wwlib\inisup.h"

void Unknown_4FC2F0(std::vector<std::string> &list, const std::string &string, const std::string &separator);	// 0x4FC2F0: splits string at separator  // NAME 1: neutral placeholder (D10)

static RefCountPtr<LookupTableSetClass> _LookupTables;	// 0xBE566C (file-static: its release has the client's register form)

// The self-test's samples (TestFloor, TestCeiling, TestClosest and TestInterpolated in the ini)
static float sample_indices[9] = { -1.0f, 0.0f, 1.0f, 2.0f, 10.0f, 11.0f, 17.0f, 23.0f, 40.0f };	// 0xB75240  // NAME 2: what it holds
static float floor_output[9] = { 0.0f, 0.0f, 0.0f, 0.0f, 10.0f, 10.0f, 10.0f, 20.0f, 30.0f };		// 0xB75264
static float ceiling_output[9] = { 0.0f, 0.0f, 10.0f, 10.0f, 10.0f, 20.0f, 20.0f, 30.0f, 30.0f };	// 0xB75288
static float closest_output[9] = { 0.0f, 0.0f, 0.0f, 0.0f, 10.0f, 10.0f, 20.0f, 20.0f, 30.0f };	// 0xB752AC
static float interpolated_output[9] = { 0.0f, 0.0f, 1.0f, 2.0f, 10.0f, 11.0f, 17.0f, 23.0f, 30.0f };	// 0xB752D0















void LookupTableManagerClass::Init(const std::string &filename)
{
	_LookupTables = LookupTableSetClass::Load(filename);
	FAIL_IF(!_LookupTables) return;

	RefCountPtr<LookupTableClass> floor = Get_Table("TestFloor");
	RefCountPtr<LookupTableClass> ceiling = Get_Table("TestCeiling");
	RefCountPtr<LookupTableClass> closest = Get_Table("TestClosest");
	RefCountPtr<LookupTableClass> interpolate = Get_Table("TestInterpolated");










	for (int sample_index = 0; sample_index < 9; sample_index++) {
		float index = sample_indices[sample_index];
		G_ASSERT(floor->Lookup(index) == floor_output[sample_index]);
		G_ASSERT(ceiling->Lookup(index) == ceiling_output[sample_index]);
		G_ASSERT(closest->Lookup(index) == closest_output[sample_index]);
		G_ASSERT(interpolate->Lookup(index) == interpolated_output[sample_index]);
	}
}

void LookupTableManagerClass::Shutdown()
{
	_LookupTables = 0;
}





float LookupTableManagerClass::Lookup(const std::string &name, float index)
{
	FAIL_IF(!_LookupTables) return 0;
	return _LookupTables->Lookup(name, index);
}


RefCountPtr<LookupTableClass> LookupTableManagerClass::Get_Table(const std::string &name)
{
	FAIL_IF(!_LookupTables) return 0;
	return _LookupTables->Get(name);
}


RefCountPtr<LookupTableSetClass> LookupTableSetClass::Load(const std::string &filename)
{
	INIClass ini;
	FAIL_IF(!ini.Load(filename.c_str())) return 0;

	RefCountPtr<LookupTableSetClass> tables = Create_NEW(new LookupTableSetClass);
	for (INISection *section = ini.Get_Section_List().First_Valid(); section != 0; section = section->Next_Valid()) {

		INIEntry *type_entry = section->Find_Entry("Type");
		FAIL_IF(!type_entry) continue;

		FAIL_IF(!type_entry->Entry) continue;

		RefCountPtr<LookupTableClass> table = LookupTableClass::Create(type_entry->Value, section);
		FAIL_IF(!table) continue;

		tables->Tables[std::string(section->Section)] = table;
	}
	return tables;
}

LookupTableSetClass::LookupTableSetClass()
{
}


LookupTableSetClass::~LookupTableSetClass()
{
	Tables.clear();
}


float LookupTableSetClass::Lookup(const std::string &name, float index)
{
	std::hash_map<std::string, RefCountPtr<LookupTableClass> >::iterator find = Tables.find(name.c_str());
	FAIL_IF(find == Tables.end()) return 0;
	return (*find).second->Lookup(index);
}





RefCountPtr<LookupTableClass> LookupTableSetClass::Get(const std::string &name)
{
	std::hash_map<std::string, RefCountPtr<LookupTableClass> >::iterator find = Tables.find(name.c_str());
	FAIL_IF(find == Tables.end()) return 0;
	return (*find).second;
}







// NOT EXACT: inline budget (explain.py: shape, 19 vs 17 calls), the client inlines the values vector's constructor and resize's first size(); ours calls them (frame 0x2C, the client 0x28)
RefCountPtr<LookupTableClass> ContinuousLookupTableClass::Create(const std::string &values)
{
	std::vector<std::string> strings;
	Unknown_4FC2F0(strings, values, std::string(","));
	FAIL_IF(strings.empty()) return 0;

	RefCountPtr<ContinuousLookupTableClass> table = Create_NEW(new ContinuousLookupTableClass);
	table->Values.resize(strings.size());
	int index = 0;
	for (std::vector<std::string>::iterator it = strings.begin(); it != strings.end(); ++it) {
		table->Values[index++] = (float)atof((*it).c_str());
	}
	return table;
}











float ContinuousLookupTableClass::Lookup(float index)
{
	FAIL_IF(index < 0) return Values[0];
	if (index >= (int)Values.size()) {
		return Values.back();
	}
	return Values[(int)index];
}























static bool Compare_Samples(const LookupSampleStruct &a, const LookupSampleStruct &b)	// NAME 2: what it does
{
	return a.Index < b.Index;
}

// NOT EXACT: order-only, the client stores the sample's Value (atof) after pushing push_back's arguments, ours before; everything else equal
SampledLookupTableClass::SampledLookupTableClass(INISection *section)
{
	for (INIEntry *entry = section->EntryList.First_Valid(); entry != 0; entry = entry->Next_Valid()) {
		if (stricmp(entry->Entry, "Type") != 0) {
			LookupSampleStruct sample;
			sample.Index = (float)atoi(entry->Entry);
			sample.Value = (float)atof(entry->Value);
			Samples.push_back(sample);
		}
	}
	std::sort(Samples.begin(), Samples.end(), Compare_Samples);






	std::vector<LookupSampleStruct>::iterator it = Samples.begin();
	while (it != Samples.end()) {
		float previous_index = (*it).Index;
		float previous_value = (*it).Value;
		++it;
		while (it != Samples.end() && (*it).Index == previous_index) {
			float current_value = (*it).Value;
			G_ASSERT(current_value == previous_value);
			Samples.erase(it);
		}
	}
}

inline void SampledLookupTableClass::Find_Samples(float index, LookupSampleStruct &lower, LookupSampleStruct &upper)
{
	if (Samples.size() == 0) {
		LookupSampleStruct zero;
		zero.Index = 0;
		zero.Value = 0;
		lower = upper = zero;
		return;
	}
	std::vector<LookupSampleStruct>::iterator prev = Samples.begin();
	if ((*prev).Index > index) {
		lower = upper = *prev;
		return;
	}
	std::vector<LookupSampleStruct>::iterator next = prev + 1;
	while (next != Samples.end() && (*next).Index <= index) {
		prev = next;
		++next;
	}
	lower = *prev;
	if (next == Samples.end()) {
		upper = *prev;
	} else if ((*prev).Index == index) {
		upper = *prev;
	} else {
		upper = *next;
	}
}

RefCountPtr<LookupTableClass> FloorLookupTableClass::Create(INISection *section)
{
	RefCountPtr<FloorLookupTableClass> table = Create_NEW(new FloorLookupTableClass(section));
	return table;
}

float FloorLookupTableClass::Lookup(float index)
{
	LookupSampleStruct lower;
	LookupSampleStruct upper;
	Find_Samples(index, lower, upper);
	return lower.Value;
}

RefCountPtr<LookupTableClass> CeilingLookupTableClass::Create(INISection *section)
{
	RefCountPtr<CeilingLookupTableClass> table = Create_NEW(new CeilingLookupTableClass(section));
	return table;
}

float CeilingLookupTableClass::Lookup(float index)
{
	LookupSampleStruct lower;
	LookupSampleStruct upper;
	Find_Samples(index, lower, upper);
	return upper.Value;
}

RefCountPtr<LookupTableClass> ClosestLookupTableClass::Create(INISection *section)
{
	RefCountPtr<ClosestLookupTableClass> table = Create_NEW(new ClosestLookupTableClass(section));
	return table;
}

float ClosestLookupTableClass::Lookup(float index)
{
	LookupSampleStruct lower;
	LookupSampleStruct upper;
	Find_Samples(index, lower, upper);
	if (index - lower.Index > upper.Index - index) {
		return upper.Value;
	}
	return lower.Value;
}

RefCountPtr<LookupTableClass> InterpolatedLookupTableClass::Create(INISection *section)
{
	RefCountPtr<InterpolatedLookupTableClass> table = Create_NEW(new InterpolatedLookupTableClass(section));
	return table;
}

float InterpolatedLookupTableClass::Lookup(float index)
{
	LookupSampleStruct lower;
	LookupSampleStruct upper;
	Find_Samples(index, lower, upper);
	if (lower.Index == upper.Index) {
		return lower.Value;
	}
	return ((index - lower.Index) / (upper.Index - lower.Index)) * (upper.Value - lower.Value) + lower.Value;
}



















































































RefCountPtr<LookupTableClass> LookupTableClass::Create(const std::string &type, INISection *section)
{
	if (stricmp(type.c_str(), "continuous") == 0) {
		INIEntry *value_strings_entry = section->Find_Entry("Values");
		FAIL_IF(!value_strings_entry) return 0;
		return ContinuousLookupTableClass::Create(std::string(value_strings_entry->Value));
	} else if (stricmp(type.c_str(), "floor") == 0) {
		return FloorLookupTableClass::Create(section);
	} else if (stricmp(type.c_str(), "ceiling") == 0) {
		return CeilingLookupTableClass::Create(section);
	} else if (stricmp(type.c_str(), "closest") == 0) {
		return ClosestLookupTableClass::Create(section);
	} else if (stricmp(type.c_str(), "interpolated") == 0) {
		return InterpolatedLookupTableClass::Create(section);
	}










	G_ASSERT(0);
	return 0;
}


/*
**	Reconstruction notes
**
**	client.exe 0x4EC1F0-0x4F0080 (ini_manager.cpp's object ends with its _Construct 0x4EC040; money.cpp
**	follows): _LookupTables's initializer 0x4EC1F0 and destructor 0x4EC220 (a file-static RefCountPtr:
**	as a class static or an extern global its release compiles to the shorter form, file-static gives the
**	client's), Init 0x4EC260 with its self-test (the four Test* tables of the ini against the arrays
**	0xB75240-0xB752F0), Shutdown 0x4EC6B0, the two lookups 0x4EC710 / 0x4EC760, Load 0x4EC7D0, the set's
**	constructor 0x4ECC10 (inlined into Load, which calls the table's constructor 0x4EE810 out of line),
**	destructor 0x4ECE50 (exact since 51782f6's member destroy in libs/sgistl), Lookup 0x4ED0B0 (find inlined) and Get 0x4ED220 (find 0x4EEC20 called), the
**	factory 0x4ED390 (line 412), the continuous table 0x4ED660-0x4EDAD0, the sampled tables
**	0x4EDB00-0x4EE7D0 (the sort's comparison 0x4EDE30 sits inside the base constructor's window), then
**	the instances: RefCountPtr (Create_NEW 0x4EE9F0 / 0x4EEA50 / 0x4EEAE0, the destructors), vector<float>,
**	vector<LookupSampleStruct> (insert 0x4EF160), std::sort (0x4EFCD0, 0x4EFEE0, 0x4EFF50) and the SGI
**	table (_M_bkt_num_key 0x4EF450 with the string hash 0x42DB00, find_or_insert 0x4EF8A0, resize
**	0x4EFA00). The asserts sit on the client's lines 38, 56-59, 74, 81, 89, 95, 97, 100, 121, 132, 147,
**	170, 229, 412 and 433 (D25). Init, Shutdown and the static Lookup have no caller in the client.
**	Forms: each Create through a named RefCountPtr of the derived class (Create_NEW's friend for the
**	base class would be found otherwise, and the client converts after); the sampled lookups share an
**	inline Find_Samples (the lower and upper samples, zero ones for an empty table: Floor, Ceiling,
**	Closest and Interpolated are its dead-code-eliminated copies); Closest compares
**	`index - lower.Index > upper.Index - index` (the client evaluates the lower distance first).
*/
