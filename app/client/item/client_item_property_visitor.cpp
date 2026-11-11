#include "..\ui_item_examination_property_router.h"

void ClientItemPropertyVisitorClass::Unknown_20(void *property)
{
}

void ClientItemPropertyVisitorClass::Unknown_1C(ClientItemPropertyNameClass *property)
{
	Unknown_20(property);
}

void ClientItemPropertyVisitorClass::Unknown_18(ClientItemPropertyIconClass *property)
{
	Unknown_20(property);
}

void ClientItemPropertyVisitorClass::Unknown_14(ClientItemPropertyUnknown529810Class *property)
{
	Unknown_20(property);
}

void ClientItemPropertyVisitorClass::Unknown_10(ClientItemPropertyTypeClass *property)
{
	Unknown_20(property);
}

void ClientItemPropertyVisitorClass::Unknown_0C(ClientItemPropertyDescriptionClass *property)
{
	Unknown_20(property);
}

void ClientItemPropertyVisitorClass::Unknown_08(ClientItemPropertyAttributeClass *property)
{
	Unknown_20(property);
}

void ClientItemPropertyVisitorClass::Unknown_04(ClientItemPropertySpecialClass *property)
{
	Unknown_20(property);
}

void ClientItemPropertyVisitorClass::Unknown_00(ClientItemPropertyOptionClass *property)
{
	Unknown_20(property);
}

/*
**	Reconstruction notes (RECONSTRUCTED): the client's 0x530390-0x5304CF, the item property visitor's default
**	slots (table 0xAEE6DC; ui_item_examination_property_router.h declares the class): Unknown_20 0x530390 does
**	nothing, the other eight 0x5303B0-0x530490 (in reverse slot order) hand their property to it; <string>'s guarded
**	static 0x5304B0 closes the object. No assert: the file's name is provisional (D137, NAME 2), after the property
**	files around it (client_item_property_type.cpp before, client_item_property_special_level_requirement.cpp after).
*/
