#include "client_item_property_attribute.h"

ClientItemPropertyAttributeClass::ClientItemPropertyAttributeClass(int sort, const std::string &label, const std::string &units, const std::string &tooltip, int unknown, int level) :
	Unknown08(sort),
	Label(label),
	Unknown1C(units),
	Tooltip(tooltip),
	Unknown3C(unknown),
	Unknown40(level)
{
}

ClientItemPropertyAttributeClass::~ClientItemPropertyAttributeClass()
{
}

int ClientItemPropertyAttributeClass::Unknown_08()
{
	return(Unknown3C);
}

std::string ClientItemPropertyAttributeClass::Get_Tooltip()
{
	return(Tooltip);
}

void ClientItemPropertyAttributeClass::Accept(ClientItemPropertyVisitorClass *visitor)
{
	visitor->Unknown_08(this);
}

/*
**	Reconstruction notes (RECONSTRUCTED): the client's 0x51F9C0-0x51FE8F, the attribute properties' base
**	(table 0xAEDC08, 0x44 bytes; the subclasses client_item_property_attribute_distance.cpp and its siblings
**	follow): the constructor 0x51F9C0, the destructors 0x51FBD0 / 0x51FBA0, the table's slots 0x51FCB0 /
**	0x51FCD0 / 0x51FE50, and <string>'s guarded static 0x51FE70 closing the object. No assert: the file's
**	name is provisional (D137, NAME 2), after its header client_item_property_attribute.h (__FILE__ 0xB7883C).
*/
