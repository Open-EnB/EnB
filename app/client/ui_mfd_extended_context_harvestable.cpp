// Earth & Beyond client (reconstruction): app\client\ui_mfd_extended_context_harvestable.cpp (per the client's asserts)
#include <stdio.h>
#include <string>
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "fontdatabase.h"
#include "aux_roots.h"
#include "..\shared\g_assert.h"
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\auxdata\delta_item_array.h"
#include "ui_mfd_extended_context_harvestable.h"

template<class C> inline C *Unknown_Find_Valid(DeltaCompoundClass *root, const NameClass &name) { C *t = Unknown_Find_Field<C>(root, name); if (t == NULL || t->Unknown70 == 0) return(NULL); return(t); }	// (cameraeffects.cpp's)  // NAME 1: neutral (D10)
class SkillNamesDatabase { public: static int Unknown_818520(const char *name); };	// 0x818520 (skillnam.cpp's: a skill's index by its name)  // NAME 1: neutral (D10)

static const char *HarvestableName = "HarvestableResources";	// 0xB82294  // NAME 2: what it holds

// 0x5BF730
void Unknown_5BF730()
{
	Unknown_BE67BC->Load("fa_harvesting.ini");
}

// 0x5BF760
void Unknown_5BF760()
{
	Unknown_BE67BC->Unload("fa_harvesting.ini");
}










// 0x5BF790
RefCountPtr<UIMFDExtendedContextClass> Unknown_5BF790(ClientObjectClass *self, ClientObjectClass *co, UIMFDClass *owner)
{


	FAIL_IF(0 == co)
		return(NULL);
	if (!(char)co->Unknown_6F5E20())
		return(NULL);
	static int prospect_skill_index = SkillNamesDatabase::Unknown_818520("Prospect");


	FAIL_IF(-1 == prospect_skill_index)
		return(NULL);
	SClient *player = (SClient *)self->Client->Unknown_18();
	FAIL_IF(0 == player)
		return(NULL);
	DeltaPlayerClass *avatar_aux_data = player->AuxData;
	FAIL_IF(0 == avatar_aux_data)
		return(NULL);
	{ NameClass name("RPGInfo.Skills"); DeltaItemArrayClass<DeltaSkillClass> *skills = Unknown_Find_Valid<DeltaItemArrayClass<DeltaSkillClass> >(avatar_aux_data, name);


	FAIL_IF(0 == skills)
		return(NULL);
	DeltaSkillClass *skill = (DeltaSkillClass *)skills->Children[prospect_skill_index];
	if (skill->Level.Value == 0)
		return(NULL);
	}



	FAIL_IF(0 == co->AuxData)
		return(NULL);
	DeltaItemArrayClass<DeltaItemClass> *harvestable_aux_data;
	{ NameClass name(HarvestableName); harvestable_aux_data = Unknown_Find_Field<DeltaItemArrayClass<DeltaItemClass> >((DeltaCompoundClass *)co->AuxData, name); }



	FAIL_IF(0 == harvestable_aux_data)
		return(NULL);
	if (harvestable_aux_data->Unknown70 != 0) {
		return(Create_NEW((UIMFDExtendedContextClass *)new UIMFDExtendedContextHarvestableClass(co, owner)));
	}
	return(UIMFDExtendedContextWatchClass::Create(harvestable_aux_data, self, co, owner));
}

static inline void Unknown_Clear(std::list<int> &v) { v.clear(); }
static inline ClientClass *Unknown_Client(ClientObjectClass *co) { return(co->Client); }
// 0x5BFB00
UIMFDExtendedContextHarvestableClass::UIMFDExtendedContextHarvestableClass(ClientObjectClass *co, UIMFDClass *owner) :
	UIMFDExtendedContextStringsClass(co, owner),
	Peek(Unknown_Client(co), "HarvestableResources", false),
	UnknownB4(false)
{
	Peek.Attach(co->AuxData);
	Create_Fonts();
	Unknown_Clear(Slots);
	unsigned int count = Peek.Count;
	for (unsigned int slot = 0; slot < count; slot++) {
		if (Has_Contents(slot)) {
			Slots.push_back(slot);
		}
	}
	Update_Index_Map();
	PreviousIndexMap = CurrentIndexMap;
	Set_Texts();
}

// 0x5BFDD0
UIMFDExtendedContextHarvestableClass::~UIMFDExtendedContextHarvestableClass()
{
}

// 0x5BFF10
void UIMFDExtendedContextHarvestableClass::Unknown_08()
{
	UIMFDExtendedContextStringsClass::Unknown_08();
}

// 0x5BFF30
void UIMFDExtendedContextHarvestableClass::Unknown_0C()
{
	UIMFDExtendedContextStringsClass::Unknown_0C();
}

// 0x5BFF50
void UIMFDExtendedContextHarvestableClass::Unknown_10()
{
	if (!Peek.Changed()) {
		return;
	}
	unsigned int count = Peek.Count;
	for (unsigned int slot = 0; slot < count; slot++) {
		if (Peek.Slot_Changed(slot)) {
			std::list<int>::iterator it = std::find(Slots.begin(), Slots.end(), (int)slot);
			if (!Has_Contents(slot)) {
				if (it != Slots.end()) {
					Slots.erase(it);
				}
			} else if (it == Slots.end()) {
				Slots.push_back(slot);
			}
		}
	}
	Update_Index_Map();
	UIMFDExtendedContextStringsClass::Unknown_10();
	PreviousIndexMap = CurrentIndexMap;
}

// 0x5C0110
std::string UIMFDExtendedContextHarvestableClass::Get_Font_Name(int index)
{
	char name[32];
	sprintf(name, "TEXT%02d_HARV", index);
	return(std::string(name));
}

// 0x5C0240
bool UIMFDExtendedContextHarvestableClass::Line_Changed(unsigned int line_index)
{












	FAIL_IF(line_index >= CurrentIndexMap.size())
		return(false);
	G_ASSERT(CurrentIndexMap.size() == PreviousIndexMap.size());
	int index = CurrentIndexMap[line_index];
	if (index != PreviousIndexMap[line_index]) {
		return(true);
	}
	if (index == -1) {
		return(false);
	}
	return(Peek.Slot_Changed(index));
}




// NOT EXACT: 0x5C0350: VC6 merges the five return("") tails; the client repeats each in place and keeps 0 in ebp
// 0x5C0350
std::string UIMFDExtendedContextHarvestableClass::Get_Line_Text(unsigned int line_index)
{
	FAIL_IF(line_index >= CurrentIndexMap.size())
		return("");
	int slot = CurrentIndexMap[line_index];
	if (slot == -1) {
		return("");
	}
	int id = Peek.Get_ID(slot);
	if (id == -2) {
		return("");
	}
	if (id == -1) {
		return("");
	}
	InvDataPeekItemStruct *item = Peek.Get_Item(slot, true);


	FAIL_IF(0 == item)
		return("");
	int quantity = Peek.Get_Quantity(slot);
	if (quantity == 0) {
		return("");
	}
	char text[128];
	sprintf(text, "%s (%d)", item->Name.c_str(), quantity);
	return(std::string(text));
}



// 0x5C0750
bool UIMFDExtendedContextHarvestableClass::Has_Contents(unsigned int slot)
{
	int id = Peek.Get_ID(slot);
	if (id == -2) {
		return(false);
	}
	if (id == -1) {
		return(false);
	}
	InvDataPeekItemStruct *item = Peek.Get_Item(slot, true);


	FAIL_IF(0 == item)
		return(false);
	return(Peek.Get_Quantity(slot) > 0);
}

static inline void Unknown_Clear(std::vector<int> &v) { v.erase(v.begin(), v.end()); }	// NAME 1: neutral (D10): keeps std::copy out of line, as the client has it
// 0x5C07E0
void UIMFDExtendedContextHarvestableClass::Update_Index_Map()
{
	Unknown_Clear(CurrentIndexMap);
	unsigned int lines = Get_Line_Count();
	CurrentIndexMap.resize(lines, -1);
	if (Slots.size() >= lines) {
		std::copy(CurrentIndexMap.begin(), CurrentIndexMap.end(), Slots.begin());
	} else {
		std::vector<int>::iterator to = CurrentIndexMap.begin();
		int skip = (int)(lines - Slots.size()) / 2;
		while (skip--) {
			to++;
		}
		for (std::list<int>::iterator it = Slots.begin(); it != Slots.end(); it++) {
			*to = *it;
			to++;
		}
	}
}
