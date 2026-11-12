#include "client_item_property_type.h"

RefCountPtr<ClientItemPropertyTypeClass> ClientItemPropertyTypeClass::Create(int level, int damage_type, int type)
{
	return(Create_NEW(new ClientItemPropertyTypeClass(level, damage_type, type)));
}

RefCountPtr<ClientItemPropertyTypeClass> ClientItemPropertyTypeClass::Create(int level, int type)
{
	return(Create_NEW(new ClientItemPropertyTypeClass(level, DAMAGE_TYPE_NONE, type)));
}

ClientItemPropertyTypeClass::ClientItemPropertyTypeClass(int level, int damage_type, int type) :
	Level(level),
	DamageType(damage_type),
	Type(type)
{
}

ClientItemPropertyTypeClass::~ClientItemPropertyTypeClass()
{
}

int ClientItemPropertyTypeClass::Unknown_08()
{
	return(0);
}

std::string ClientItemPropertyTypeClass::Get_Tooltip()
{
	return("type tooltip");
}

void ClientItemPropertyTypeClass::Accept(ClientItemPropertyVisitorClass *visitor)
{
	visitor->Unknown_10(this);
}

bool ClientItemPropertyTypeClass::Has_Damage_Type() const
{
	return(DamageType != DAMAGE_TYPE_NONE);
}

/*
**	Reconstruction notes (RECONSTRUCTED): the client's 0x52FFB0-0x53037F, an item's level, damage type and type as
**	a property (table 0xAEE14C, 0x14 bytes; a ClientItemPropertyClass, not a special): the Creates 0x52FFB0 and
**	0x530070 (the constructor inlined into both), the constructor 0x530130, the destructors 0x5301A0 / 0x530170, the
**	table's slots 0x5301C0 / 0x5301E0 / 0x5302F0, Has_Damage_Type 0x530310, Create_NEW<ClientItemPropertyTypeClass>
**	0x530330 and <string>'s guarded static 0x530360. The visitor's default slots 0x530390-0x5304A0 and their
**	<string> static 0x5304B0 that follow are another object. No assert: the file's name is provisional (D137,
**	NAME 2), after its siblings, which the client names; the item examination title shows the class
**	(UIItemExaminationTitleClass::Set_Subtitles).
*/
