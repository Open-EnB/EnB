#include "client_item_property_special_skill_requirement.h"
#include <stdlib.h>
#include "..\sclient.h"
#include "..\aux_roots.h"
#include "..\skillnam.h"
#include "..\..\shared\auxdata\auxdata.h"
#include "..\..\shared\auxdata\delta_item_array.h"

template<class C> inline C *Unknown_Find_Valid(DeltaCompoundClass *root, const NameClass &name) { C *t = Unknown_Find_Field<C>(root, name); if (t && t->Unknown70) return(t); return(NULL); }	// NAME 1: neutral (D10): as co.cpp's
























RefCountPtr<ClientItemPropertySpecialClass> ClientItemPropertySpecialSkillRequirementClass::Create(SClient *space_client, const std::string &skill, int level, RequirementType type)
{
	std::string tooltip;
	int sort_order;
	switch (type) {
		case REQUIRED_TO_EQUIP:
			sort_order = 0;
			tooltip = "You must have this skill level to equip this item.";
			break;
		case REQUIRED_TO_ACTIVATE:
			sort_order = 1;
			tooltip = "You must have this skill level to activate this item.";
			break;
		default:
			G_ASSERT(0);
			return(RefCountPtr<ClientItemPropertySpecialClass>());
	}
	RefCountPtr<ClientItemPropertySpecialSkillRequirementClass> requirement = Create_NEW(new ClientItemPropertySpecialSkillRequirementClass(space_client, sort_order, tooltip, skill, level));
	return(requirement);
}

ClientItemPropertySpecialSkillRequirementClass::ClientItemPropertySpecialSkillRequirementClass(SClient *space_client, int sort_order, const std::string &tooltip, const std::string &skill, int level) :
	ClientItemPropertySpecialClass(sort_order, tooltip, 0, 1),
	SpaceClient(space_client),
	Skill(skill),
	Level(level)
{
}

ClientItemPropertySpecialSkillRequirementClass::~ClientItemPropertySpecialSkillRequirementClass()
{
}

std::vector<ClientItemPropertySpecialLineStruct> ClientItemPropertySpecialSkillRequirementClass::Get_Lines()
{
	std::vector<ClientItemPropertySpecialLineStruct> lines;
	lines.push_back(ClientItemPropertySpecialLineStruct("Requires ", false));
	lines.push_back(ClientItemPropertySpecialLineStruct(Skill, true));
	lines.push_back(ClientItemPropertySpecialLineStruct(" level ", false));
	char buffer[32];
	lines.push_back(ClientItemPropertySpecialLineStruct(itoa(Level, buffer, 10), true));
	return(lines);
}











// NOT EXACT: 0x52F010 (inline-budget: the client calls UnknownRTTIClass<DeltaItemArrayClass<DeltaSkillClass> >::Cast out of line, 0x52F990 (emitted in this object); ours inlines it; the rest, asserts and lines match)
bool ClientItemPropertySpecialSkillRequirementClass::Is_Usable()
{
	DeltaCompoundClass *aux_data = (DeltaCompoundClass *)SpaceClient->AuxData;
	FAIL_IF(!aux_data) return(false);
	NameClass name("RPGInfo.Skills");
	DeltaItemArrayClass<DeltaSkillClass> *skills_aux_data = Unknown_Find_Valid<DeltaItemArrayClass<DeltaSkillClass> >(aux_data, name);
	FAIL_IF(!skills_aux_data) return(false);

	int skill_index = SkillNamesDatabase::Unknown_818520(Skill.c_str());
	FAIL_IF(skill_index < 0) return(false);
	FAIL_IF(skill_index >= (int)skills_aux_data->Children.size()) return(false);
	DeltaSkillClass *skill = (DeltaSkillClass *)skills_aux_data->Children[skill_index];
	return(Level <= skill->Level.Value);
}

/*
**	Reconstruction notes (RECONSTRUCTED): the client's 0x52E760-0x52F9DF (__FILE__ 0xB78430,
**	'e:\build\beta\G\app\Client\item\client_item_property_special_skill_requirement.cpp'), a skill requirement as a
**	special property (table 0xAEE104, 0x3C bytes): Create 0x52E760 (the constructor inlined into it; G_ASSERT(0)
**	at line 48 for another type; sort order 0 to equip, 1 to activate), the constructor 0x52E9E0, the destructors
**	0x52EBE0 / 0x52EBB0, the table's slots 0x52EC50 (Get_Lines: 'Requires ', the skill in bold, ' level ', the level
**	in bold) and 0x52F010 (Is_Usable: FAIL_IFs at lines 92, 95, 98 and 99; true when the player's RPGInfo.Skills
**	entry has Level or more; the client's FAIL_IF reads 'skill_index >= (int)skills_aux_data->Get_Size()': a
**	Get_Size member in delta_item_array.h shifts the symbol pad of sclient.cpp / aux_roots.cpp, so the test spells
**	Children.size() out), and what the compiler emits for them: Create_NEW 0x52F190, the vector's
**	insert(iterator, const T &) 0x52F1F0 and its helpers 0x52F450-0x52F710, and
**	UnknownRTTIClass<DeltaItemArrayClass<DeltaSkillClass> >::Cast 0x52F990.
*/
