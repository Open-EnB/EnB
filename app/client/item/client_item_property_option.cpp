#include "client_item_property_option.h"

ClientItemPropertyOptionClass::ClientItemPropertyOptionClass(int sort, const std::string &label, const std::string &tooltip, int unknown) :
	Unknown08(sort),
	Label(label),
	Tooltip(tooltip)
{
}

ClientItemPropertyOptionClass::~ClientItemPropertyOptionClass()
{
}

int ClientItemPropertyOptionClass::Unknown_08()
{
	return(Unknown2C);
}

std::string ClientItemPropertyOptionClass::Unknown_0C()
{
	return(Tooltip);
}

void ClientItemPropertyOptionClass::Accept(ClientItemPropertyVisitorClass *visitor)
{
	visitor->Unknown_00(this);
}

/*
**	Reconstruction notes (RECONSTRUCTED): the client's 0x52A840-0x52AC9F, the option property's own code (table
**	0xAEDDB8, two pure slots +0x14 / +0x18): the constructor 0x52A840, the destructors 0x52AA00 / 0x52A9D0, the slots
**	0x52AAC0 / 0x52AAE0 / 0x52AC60, and <string>'s guarded static 0x52AC80. No assert of its own: the file's name is
**	its header's (client_item_property_option.h's __FILE__ 0xB78A58, NAME 3). The constructor never writes +0x2C
**	(Unknown2C, which slot +0x08 returns): its fourth argument (0 from the auto fire option) goes unused, as in the client.
*/
