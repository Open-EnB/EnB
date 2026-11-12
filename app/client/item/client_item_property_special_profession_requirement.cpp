#include "client_item_property_special_profession_requirement.h"
#include "..\sclient.h"
#include "..\aux_roots.h"
#include "..\..\shared\auxdata\auxdata.h"

template<class C> inline C *Unknown_Find_Valid(DeltaCompoundClass *root, const NameClass &name) { C *t = Unknown_Find_Field<C>(root, name); if (t && t->Unknown70) return(t); return(NULL); }	// NAME 1: neutral (D10): as co.cpp's
























RefCountPtr<ClientItemPropertySpecialProfessionRequirementClass> ClientItemPropertySpecialProfessionRequirementClass::Create(SClient *space_client, int professions)
{
	std::string label;
	std::string tooltip;
	switch (professions & 7) {
		case 0:
			G_ASSERT(0);
			return(RefCountPtr<ClientItemPropertySpecialProfessionRequirementClass>());
		case 2:
			label = "Trader Restricted";
			tooltip = "May not be used by Tradesmen.";
			break;
		case 1:
			label = "Warriors Restricted";
			tooltip = "May not be used by Warriors.";
			break;
		case 4:
			label = "Explorers Restricted";
			tooltip = "May not be used by Explorers.";
			break;
		case 3:
			label = "Explorers Only";
			tooltip = "May only be used by Explorers.";
			break;
		case 6:
			label = "Warriors Only";
			tooltip = "May only be used by Warriors.";
			break;
		case 5:
			label = "Tradesmen Only";
			tooltip = "May only be used by Tradesmen.";
			break;
		default:
			label = "Profession Restricted";
			tooltip = "Players cannot use.";
			break;
	}
	return(Create_NEW(new ClientItemPropertySpecialProfessionRequirementClass(space_client, 4, professions, label, tooltip)));
}

ClientItemPropertySpecialProfessionRequirementClass::ClientItemPropertySpecialProfessionRequirementClass(SClient *space_client, int sort_order, int professions, const std::string &label, const std::string &tooltip) :
	ClientItemPropertySpecialClass(sort_order, tooltip, 0, 1),
	SpaceClient(space_client),
	Label(label),
	Professions(professions)
{
}

ClientItemPropertySpecialProfessionRequirementClass::~ClientItemPropertySpecialProfessionRequirementClass()
{
}

std::vector<ClientItemPropertySpecialLineStruct> ClientItemPropertySpecialProfessionRequirementClass::Get_Lines()
{
	std::vector<ClientItemPropertySpecialLineStruct> lines;
	lines.push_back(ClientItemPropertySpecialLineStruct(Label, false));
	return(lines);
}













// NOT EXACT: 0x52D900 (register-only: the client tests and casts the found field in eax, ours copies it to ecx first; the same code, asserts and lines)
bool ClientItemPropertySpecialProfessionRequirementClass::Is_Usable()
{
	DeltaCompoundClass *aux_data = (DeltaCompoundClass *)SpaceClient->AuxData;
	FAIL_IF(!aux_data) return(false);
	NameClass name("RPGInfo.Profession");
	DeltaTemplateClass<UnknownB03588Enum> *profession_aux_data = Unknown_Find_Valid<DeltaTemplateClass<UnknownB03588Enum> >(aux_data, name);
	FAIL_IF(!profession_aux_data) return(false);
	return((Professions & (1 << profession_aux_data->Value)) == 0);
}

/*
**	Reconstruction notes (RECONSTRUCTED): the client's 0x52CF90-0x52DB5F (__FILE__ 0xB780B4,
**	'e:\build\beta\G\app\Client\item\client_item_property_special_profession_requirement.cpp'), a profession
**	requirement as a special property (table 0xAEDFAC, 0x3C bytes): Create 0x52CF90 (the constructor inlined into
**	it; G_ASSERT(0) at line 37 for no profession), the constructor 0x52D490, the destructors 0x52D690 / 0x52D660,
**	the table's slots 0x52D700 (Get_Lines: the label alone, not bold) and 0x52D900 (Is_Usable: FAIL_IFs at lines
**	106 and 109; false when the player's RPGInfo.Profession is one of Professions' bits),
**	Create_NEW<ClientItemPropertySpecialProfessionRequirementClass> 0x52D9F0, then <string>'s and permstr.h's
**	guarded statics. The bits: 1 warriors, 2 tradesmen, 4 explorers (the labels); the cases sit in the client's
**	order.
*/
