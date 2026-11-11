#include "client_item_property_special_flag.h"

RefCountPtr<ClientItemPropertySpecialFlagClass> ClientItemPropertySpecialFlagClass::Create(int sort_order, const std::string &text, const std::string &tooltip, int unknown_1c, int unknown_20)
{
	return(Create_NEW(new ClientItemPropertySpecialFlagClass(sort_order, text, tooltip, unknown_1c, unknown_20)));
}

ClientItemPropertySpecialFlagClass::ClientItemPropertySpecialFlagClass(int sort_order, const std::string &text, const std::string &tooltip, int unknown_1c, int unknown_20) :
	ClientItemPropertySpecialClass(sort_order, tooltip, unknown_1c, unknown_20),
	Text(text)
{
}

ClientItemPropertySpecialFlagClass::~ClientItemPropertySpecialFlagClass()
{
}

std::vector<ClientItemPropertySpecialLineStruct> ClientItemPropertySpecialFlagClass::Get_Lines()
{
	std::vector<ClientItemPropertySpecialLineStruct> lines;
	lines.push_back(ClientItemPropertySpecialLineStruct(Text, false));
	return(lines);
}

bool ClientItemPropertySpecialFlagClass::Is_Usable()
{
	return(true);
}

/*
**	Reconstruction notes (RECONSTRUCTED): the client's 0x52F9E0-0x52FFAF, a flag of an item as a special property
**	(table 0xAEE128, 0x34 bytes; the builder's 'Not Tradable', 'Temporary', 'Unique', 'Not Storable', 'Not
**	Destroyable', 'Not Manufacturable'): Create 0x52F9E0 (the constructor inlined into it), the constructor
**	0x52FAC0, the destructors 0x52FCC0 / 0x52FC90, the table's slots 0x52FD30 (Get_Lines: Text, not bold) and
**	0x52FF30 (Is_Usable: always), and Create_NEW<ClientItemPropertySpecialFlagClass> 0x52FF50. No assert: the
**	file's name is provisional (D137, NAME 2), after its siblings, which the client names.
*/
