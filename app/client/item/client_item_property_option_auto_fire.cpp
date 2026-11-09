#include "client_item_property_option_auto_fire.h"
#include "client_item_property_builder.h"
#include "..\sclient.h"
#include "..\..\packets\pkitemstate.h"

static inline unsigned Get_Ship_ID(SClient *client) { return(client->ShipID); }	// an inline layer the client spends (as a ClientClass member in client.h it cost sclient.cpp's 0x74D990 its registers)  // NAME 1: as Get_Avatar_ID

RefCountPtr<ClientItemPropertyOptionAutoFireClass> ClientItemPropertyOptionAutoFireClass::Create(SClient *space_client, const RefCountPtr<InvDataClass> &data, int slot)
{
	return(Create_NEW(new ClientItemPropertyOptionAutoFireClass(space_client, data, slot)));
}

ClientItemPropertyOptionAutoFireClass::ClientItemPropertyOptionAutoFireClass(SClient *space_client, const RefCountPtr<InvDataClass> &data, int slot) :
	ClientItemPropertyOptionClass(1, "Auto Fire", "When selected, weapon will fire automatically.", 0),
	SpaceClient(space_client),
	Data(data),
	Slot(slot)
{
}

ClientItemPropertyOptionAutoFireClass::~ClientItemPropertyOptionAutoFireClass()
{
}

bool ClientItemPropertyOptionAutoFireClass::Unknown_14()
{
	InvDataPeekClass *peek = Data->Unknown_688830();	// NAME 1 [peek]: neutral
	return((peek->Unknown_68A840(Slot) & 0x2000) == 0x2000);
}

void ClientItemPropertyOptionAutoFireClass::Unknown_18(bool value)
{
	ItemStatePacketClass packet(Get_Ship_ID(SpaceClient), 2, Slot, 0x2000, value);
	SpaceClient->Send_Packet(&packet);
}

/*
**	Reconstruction notes (RECONSTRUCTED): the client's 0x52AD00-0x52B38F (Create 0x52AD00, the constructor
**	0x52AF10, the destructors 0x52B100 / 0x52B0D0, the table 0xAEDE64's slots 0x52B120 / 0x52B160, pk.h's
**	PacketClass inlines 0x52B210-0x52B350 that ItemStatePacketClass's temporary needs, and
**	Create_NEW<ClientItemPropertyOptionAutoFireClass> 0x52B380). No assert: the file's name is provisional (D137).
*/
