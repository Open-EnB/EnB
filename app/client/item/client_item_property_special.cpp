#include "client_item_property_special.h"

ClientItemPropertySpecialClass::ClientItemPropertySpecialClass(int sort_order, const std::string &tooltip, int unknown_1c, int unknown_20) :
	SortOrder(sort_order),
	Tooltip(tooltip),
	Unknown1C(unknown_1c),
	Unknown20(unknown_20)
{
}

ClientItemPropertySpecialClass::~ClientItemPropertySpecialClass()
{
}

int ClientItemPropertySpecialClass::Unknown_08()
{
	return(Unknown1C);
}

std::string ClientItemPropertySpecialClass::Get_Tooltip()
{
	return(Tooltip);
}

void ClientItemPropertySpecialClass::Accept(ClientItemPropertyVisitorClass *visitor)
{
	visitor->Unknown_04(this);
}

/*
**	Reconstruction notes (RECONSTRUCTED): the client's 0x52B3E0-0x52B80F, the special properties' base (table
**	0xAEDEB8, 0x24 bytes; the subclasses client_item_property_special_effect.cpp and its seven siblings follow, to
**	0x530B5F): the constructor 0x52B3E0 (14 callers: the subclasses' constructors and their Creates, which
**	inline them), the destructors 0x52B5E0 / 0x52B5B0, and the table's slots 0x52B650 / 0x52B670 / 0x52B7F0.
**	Get_Lines and Is_Usable are pure here (0x9CCD10 in the table). No assert: the file's name is provisional
**	(D137, NAME 2), after its header client_item_property_special.h (__FILE__ 0xB78B68).
*/
