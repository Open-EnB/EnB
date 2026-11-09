#include "client_item_property_icon.h"
#include "..\..\shared\g_assert.h"























RefCountPtr<ClientItemPropertyIconClass> ClientItemPropertyIconClass::Create(const RefCountPtr<TextureClass> &icon_texture)
{
	FAIL_IF(!icon_texture) return(RefCountPtr<ClientItemPropertyIconClass>());
	return(Create_NEW(new ClientItemPropertyIconClass(icon_texture)));
}

ClientItemPropertyIconClass::ClientItemPropertyIconClass(const RefCountPtr<TextureClass> &icon_texture) :
	IconTexture(icon_texture)
{
}

ClientItemPropertyIconClass::~ClientItemPropertyIconClass()
{
}

int ClientItemPropertyIconClass::Unknown_08()
{
	return(0);
}

std::string ClientItemPropertyIconClass::Unknown_0C()
{
	return("category tooltip");
}

void ClientItemPropertyIconClass::Accept(ClientItemPropertyVisitorClass *visitor)
{
	visitor->Unknown_18(this);
}

/*
**	Reconstruction notes (RECONSTRUCTED): the client's 0x529FC0-0x52A30F (Create 0x529FC0, its FAIL_IF
**	'!icon_texture' at line 28; the constructor 0x52A0B0, the destructors 0x52A120 / 0x52A0F0, the table
**	0xAEDD88's slots 0x52A1B0 / 0x52A1D0 / 0x52A2E0, and Create_NEW<ClientItemPropertyIconClass> 0x52A300).
*/
