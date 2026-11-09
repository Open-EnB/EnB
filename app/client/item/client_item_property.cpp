#include "..\ui_item_examination_property_router.h"

ClientItemPropertyClass::ClientItemPropertyClass()
{
}

ClientItemPropertyClass::~ClientItemPropertyClass()
{
}

void ClientItemPropertyClass::Accept(ClientItemPropertyVisitorClass *visitor)
{
	visitor->Unknown_20(this);
}

/*
**	Reconstruction notes (RECONSTRUCTED): the client's 0x51F900-0x51F9AF, the item property files' common base
**	(table 0xAEDBF0: Delete_This, the deleting destructor 0x51F920, two pure slots, Accept 0x51F970): the
**	constructor 0x51F900, the destructors 0x51F950 / 0x51F920, Accept 0x51F970 (the visitor's slot +0x20), and
**	<string>'s guarded static 0x51F990 closing the object. No assert: the file's name is provisional (D137,
**	NAME 1), after the class's subclasses client_item_property_*.cpp; the class stays declared in
**	ui_item_examination_property_router.h (interim, D137).
*/
