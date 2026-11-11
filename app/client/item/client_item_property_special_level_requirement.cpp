#include "client_item_property_special_level_requirement.h"
#include <stdlib.h>
#include "..\sclient.h"
#include "..\aux_roots.h"
#include "..\..\shared\auxdata\auxdata.h"

template<class C> inline C *Unknown_Find_Valid(DeltaCompoundClass *root, const NameClass &name) { C *t = Unknown_Find_Field<C>(root, name); if (t && t->Unknown70) return(t); return(NULL); }	// NAME 1: neutral (D10): as co.cpp's























static std::string Kind_Name(int kind)
{
	switch (kind) {
		case ClientItemPropertySpecialLevelRequirementClass::KIND_COMBAT:
			return("Combat");
		case ClientItemPropertySpecialLevelRequirementClass::KIND_EXPLORE:
			return("Explore");
		case ClientItemPropertySpecialLevelRequirementClass::KIND_TRADE:
			return("Trade");
		case ClientItemPropertySpecialLevelRequirementClass::KIND_TOTAL:
			return("");
		default:
			G_ASSERT(0);
			return("Unknown");
	}
}

RefCountPtr<ClientItemPropertySpecialLevelRequirementClass> ClientItemPropertySpecialLevelRequirementClass::Create(SClient *space_client, int kind, int level)
{
	std::string tooltip;
	if (kind == KIND_TOTAL) {
		tooltip = "You must be this level to use.";
	} else {
		tooltip = std::string("You must have this level of ") + Kind_Name(kind) + " to use this item.";
	}
	return(Create_NEW(new ClientItemPropertySpecialLevelRequirementClass(space_client, tooltip, kind, level)));
}

ClientItemPropertySpecialLevelRequirementClass::ClientItemPropertySpecialLevelRequirementClass(SClient *space_client, const std::string &tooltip, int kind, int level) :
	ClientItemPropertySpecialClass(4, tooltip, 0, 1),
	SpaceClient(space_client),
	Kind(kind),
	Level(level)
{
}

ClientItemPropertySpecialLevelRequirementClass::~ClientItemPropertySpecialLevelRequirementClass()
{
}

// NOT EXACT: 0x530AA0 (frame: the same calls and strings in order, but the client's frame is 0x14 smaller and its temporaries sit in other slots; one vector per branch, each returned)
std::vector<ClientItemPropertySpecialLineStruct> ClientItemPropertySpecialLevelRequirementClass::Get_Lines()
{
	if (Kind == KIND_TOTAL) {
		std::vector<ClientItemPropertySpecialLineStruct> lines;
		lines.push_back(ClientItemPropertySpecialLineStruct("Requires level ", false));
		char buffer[32];
		lines.push_back(ClientItemPropertySpecialLineStruct(itoa(Level, buffer, 10), true));
		return(lines);
	} else {
		std::vector<ClientItemPropertySpecialLineStruct> lines;
		lines.push_back(ClientItemPropertySpecialLineStruct("Requires ", false));
		lines.push_back(ClientItemPropertySpecialLineStruct(Kind_Name(Kind), true));
		lines.push_back(ClientItemPropertySpecialLineStruct(" level ", false));
		char buffer[32];
		lines.push_back(ClientItemPropertySpecialLineStruct(itoa(Level, buffer, 10), true));
		return(lines);
	}
}

// NOT EXACT: 0x531080 (inline-budget: the client calls UnknownRTTIClass<DeltaTemplateClass<int> >::Cast out of line, 0x531590, and compares Value < Level after reloading this; the rest, asserts and lines match)
bool ClientItemPropertySpecialLevelRequirementClass::Is_Usable()
{
	DeltaCompoundClass *aux_data = (DeltaCompoundClass *)SpaceClient->AuxData;
	FAIL_IF(!aux_data) return(false);

	if (Kind == KIND_TOTAL) {
		NameClass name("RPGInfo");
		DeltaCompoundClass *rpg_aux_data = Unknown_Find_Valid<DeltaCompoundClass>(aux_data, name);

		FAIL_IF(!rpg_aux_data) return(false);
	} else {
		std::string field = std::string("RPGInfo.") + Kind_Name(Kind) + "Level";
		NameClass name(field.c_str());
		DeltaTemplateClass<int> *level_aux_data = Unknown_Find_Valid<DeltaTemplateClass<int> >(aux_data, name);







		FAIL_IF(!level_aux_data) return(false);
		if (level_aux_data->Value < Level) {
			return(false);
		}
	}
	return(true);
}

/*
**	Reconstruction notes (RECONSTRUCTED): the client's 0x530530-0x53164F (__FILE__ 0xB785C0,
**	'e:\build\beta\G\app\Client\item\client_item_property_special_level_requirement.cpp'), a level requirement as a
**	special property (table 0xAEE1EC, 0x30 bytes): Create 0x530530 (the constructor inlined into it), Kind_Name
**	0x530820 (static, called directly; G_ASSERT(0) at line 43 for another kind), the constructor 0x530A00, the
**	destructors 0x530A80 / 0x530A50, the table's slots 0x530AA0 (Get_Lines) and 0x531080 (Is_Usable: FAIL_IFs at
**	lines 95, 101 and 113; for the total level only RPGInfo is looked for), and what the compiler emits for them:
**	Create_NEW<ClientItemPropertySpecialLevelRequirementClass> 0x531430, auxdata.h's Unknown_Find_Field 0x5314D0 and
**	UnknownRTTIClass<DeltaTemplateClass<int> >::Cast 0x531590 out of line. NAME 1 [Kind_Name]: what it returns.
*/
