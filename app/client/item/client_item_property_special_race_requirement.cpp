#include "client_item_property_special_race_requirement.h"
#include "..\sclient.h"
#include "..\aux_roots.h"
#include "..\..\shared\auxdata\auxdata.h"

template<class C> inline C *Unknown_Find_Valid(DeltaCompoundClass *root, const NameClass &name) { C *t = Unknown_Find_Field<C>(root, name); if (t && t->Unknown70) return(t); return(NULL); }	// NAME 1: neutral (D10): as co.cpp's






















RefCountPtr<ClientItemPropertySpecialRaceRequirementClass> ClientItemPropertySpecialRaceRequirementClass::Create(SClient *space_client, int races, int race)
{
	std::string label;
	std::string tooltip;
	switch (race) {
		case 1:
			races |= 4;
			break;
		case 2:
			races |= 2;
			break;
	}
	switch (races & 7) {
		case 0:
			G_ASSERT(0);
			return(RefCountPtr<ClientItemPropertySpecialRaceRequirementClass>());
		case 1:
			label = "Terran Restricted";
			tooltip = "May not be equipped on Terran ships.";
			break;
		case 4:
			label = "Progen Restricted";
			tooltip = "May not be equipped on Progen ships.";
			break;
		case 2:
			label = "Jenquai Restricted";
			tooltip = "May not be equipped on Jenquai ships.";
			break;
		case 5:
			label = "Jenquai Only";
			tooltip = "May only be equipped on Jenquai ships.";
			break;
		case 3:
			label = "Progen Only";
			tooltip = "May only be equipped on Progen ships.";
			break;
		case 6:
			label = "Terran Only";
			tooltip = "May only be equipped on Terran ships.";
			break;
		default:
			label = "Race Restricted";
			tooltip = "Players cannot equip.";
			break;
	}
	return(Create_NEW(new ClientItemPropertySpecialRaceRequirementClass(space_client, 4, races, label, tooltip)));
}

ClientItemPropertySpecialRaceRequirementClass::ClientItemPropertySpecialRaceRequirementClass(SClient *space_client, int sort_order, int races, const std::string &label, const std::string &tooltip) :
	ClientItemPropertySpecialClass(sort_order, tooltip, 0, 1),
	SpaceClient(space_client),
	Label(label),
	Races(races)
{
}

ClientItemPropertySpecialRaceRequirementClass::~ClientItemPropertySpecialRaceRequirementClass()
{
}

std::vector<ClientItemPropertySpecialLineStruct> ClientItemPropertySpecialRaceRequirementClass::Get_Lines()
{
	std::vector<ClientItemPropertySpecialLineStruct> lines;
	lines.push_back(ClientItemPropertySpecialLineStruct(Label, false));
	return(lines);
}













// NOT EXACT: 0x52E500 (register-only: the client tests and casts the found field in eax, ours copies it to ecx first; the same code, asserts and lines)
bool ClientItemPropertySpecialRaceRequirementClass::Is_Usable()
{
	DeltaCompoundClass *aux_data = (DeltaCompoundClass *)SpaceClient->AuxData;
	FAIL_IF(!aux_data) return(false);
	NameClass name("RPGInfo.Race");
	DeltaTemplateClass<UnknownB035E0Enum> *race_aux_data = Unknown_Find_Valid<DeltaTemplateClass<UnknownB035E0Enum> >(aux_data, name);
	FAIL_IF(!race_aux_data) return(false);
	return((Races & (1 << race_aux_data->Value)) == 0);
}

/*
**	Reconstruction notes (RECONSTRUCTED): the client's 0x52DB60-0x52E75F (__FILE__ 0xB78324,
**	'e:\build\beta\G\app\Client\item\client_item_property_special_race_requirement.cpp'), a race requirement as a
**	special property (table 0xAEE058, 0x3C bytes): Create 0x52DB60 (the constructor inlined into it; G_ASSERT(0)
**	at line 43 for no race), the constructor 0x52E090, the destructors 0x52E290 / 0x52E260, the table's slots
**	0x52E300 (Get_Lines: the label alone, not bold) and 0x52E500 (Is_Usable: FAIL_IFs at lines 112 and 115; false
**	when the player's RPGInfo.Race is one of Races' bits), Create_NEW<ClientItemPropertySpecialRaceRequirementClass>
**	0x52E5F0, then <string>'s and permstr.h's guarded statics. The bits: 1 Terran, 2 Jenquai, 4 Progen (the
**	labels); race (the item's attribute 17) adds a bit: 1 excludes the Progen, 2 the Jenquai. The cases sit in the
**	client's order.
*/
