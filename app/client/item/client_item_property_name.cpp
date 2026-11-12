#include "client_item_property_name.h"

RefCountPtr<ClientItemPropertyNameClass> ClientItemPropertyNameClass::Create(const std::string &name)
{
	return(Create_NEW(new ClientItemPropertyNameClass(name)));
}

ClientItemPropertyNameClass::ClientItemPropertyNameClass(const std::string &name) :
	Name(name)
{
}

ClientItemPropertyNameClass::~ClientItemPropertyNameClass()
{
}

int ClientItemPropertyNameClass::Unknown_08()
{
	return(0);
}

std::string ClientItemPropertyNameClass::Get_Tooltip()
{
	return("name tooltip");
}

void ClientItemPropertyNameClass::Accept(ClientItemPropertyVisitorClass *visitor)
{
	visitor->Unknown_1C(this);
}

/*
**	Reconstruction notes (RECONSTRUCTED): the client's 0x52A360-0x52A82F (Create 0x52A360, the constructor
**	0x52A430, the destructors 0x52A620 / 0x52A5F0, the table 0xAEDDA0's slots 0x52A690 / 0x52A6B0 / 0x52A7C0,
**	Create_NEW<ClientItemPropertyNameClass> 0x52A7E0, and <string>'s guarded static 0x52A810). No assert:
**	the file's name is provisional (D137), after its neighbour client_item_property_icon.cpp.
*/
