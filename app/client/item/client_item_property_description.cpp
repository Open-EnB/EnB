#include "client_item_property_description.h"

RefCountPtr<ClientItemPropertyDescriptionClass> ClientItemPropertyDescriptionClass::Create(const std::string &description)
{
	return(Create_NEW(new ClientItemPropertyDescriptionClass(description)));
}

ClientItemPropertyDescriptionClass::ClientItemPropertyDescriptionClass(const std::string &description) :
	Description(description)
{
}

ClientItemPropertyDescriptionClass::~ClientItemPropertyDescriptionClass()
{
}

int ClientItemPropertyDescriptionClass::Unknown_08()
{
	return(0);
}

std::string ClientItemPropertyDescriptionClass::Unknown_0C()
{
	return("description");
}

void ClientItemPropertyDescriptionClass::Accept(ClientItemPropertyVisitorClass *visitor)
{
	visitor->Unknown_0C(this);
}

/*
**	Reconstruction notes (RECONSTRUCTED): the client's 0x529AE0-0x529FAF (Create 0x529AE0, the constructor
**	0x529BB0, the destructors 0x529DA0 / 0x529D70, the table 0xAEDD70's slots 0x529E10 / 0x529E30 / 0x529F40,
**	Create_NEW<ClientItemPropertyDescriptionClass> 0x529F60, and <string>'s guarded static 0x529F90). No assert:
**	the file's name is provisional (D137), after its neighbour client_item_property_icon.cpp.
*/
