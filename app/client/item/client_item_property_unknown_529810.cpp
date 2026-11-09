#include "client_item_property_unknown_529810.h"

RefCountPtr<ClientItemPropertyUnknown529810Class> ClientItemPropertyUnknown529810Class::Create(int id)
{
	return(Create_NEW(new ClientItemPropertyUnknown529810Class(id)));
}

ClientItemPropertyUnknown529810Class::ClientItemPropertyUnknown529810Class(int id) :
	Unknown08(id)
{
}

ClientItemPropertyUnknown529810Class::~ClientItemPropertyUnknown529810Class()
{
}

int ClientItemPropertyUnknown529810Class::Unknown_08()
{
	return(0);
}

std::string ClientItemPropertyUnknown529810Class::Unknown_0C()
{
	return("category tooltip");
}

void ClientItemPropertyUnknown529810Class::Accept(ClientItemPropertyVisitorClass *visitor)
{
	visitor->Unknown_14(this);
}

/*
**	Reconstruction notes (RECONSTRUCTED): the client's 0x529810-0x529A8F (Create 0x529810, the constructor
**	0x5298B0, the destructors 0x529910 / 0x5298E0, the table 0xAEDD58's slots 0x529930 / 0x529950 / 0x529A60,
**	Create_NEW<ClientItemPropertyUnknown529810Class> 0x529A80; no <string> guarded static at its end, as the icon's
**	file). No assert: the file's name is provisional (D137), neutral (what the int is, is not known: the builder
**	passes the item's +0x08).
*/
