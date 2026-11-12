#include "client_item_property_special_effect.h"
#include "client_item_property_builder.h"	// ItemEffectClass

RefCountPtr<ClientItemPropertySpecialEffectClass> ClientItemPropertySpecialEffectClass::Create(const ItemEffectClass &effect)
{
	std::string text = effect.Unknown_833D00();
	std::string tooltip = effect.Unknown_833D40();
	return(Create_NEW(new ClientItemPropertySpecialEffectClass(text, tooltip)));
}

ClientItemPropertySpecialEffectClass::ClientItemPropertySpecialEffectClass(const std::string &text, const std::string &tooltip) :
	ClientItemPropertySpecialClass(100, tooltip, 0, 1),
	Text(text)
{
}

ClientItemPropertySpecialEffectClass::~ClientItemPropertySpecialEffectClass()
{
}

std::vector<ClientItemPropertySpecialLineStruct> ClientItemPropertySpecialEffectClass::Get_Lines()
{
	std::vector<ClientItemPropertySpecialLineStruct> lines;
	lines.push_back(ClientItemPropertySpecialLineStruct("Effect: ", true));
	lines.push_back(ClientItemPropertySpecialLineStruct(Text, false));
	return(lines);
}

bool ClientItemPropertySpecialEffectClass::Is_Usable()
{
	return(true);
}

/*
**	Reconstruction notes (RECONSTRUCTED): the client's 0x52B840-0x52C75F, the first of the eight special
**	properties (table 0xAEDEDC, 0x34 bytes): Create 0x52B840 (the constructor inlined into it), the constructor
**	0x52B9C0, the destructors 0x52BBC0 / 0x52BB90, the table's slots 0x52BC30 (Get_Lines) and 0x52BF50
**	(Is_Usable), and what the compiler emits for them: ClientItemPropertySpecialLineStruct's destructors
**	0x52BF00 / 0x52C710 and copy 0x52C580, Create_NEW<ClientItemPropertySpecialEffectClass> 0x52BF70,
**	std::vector<ClientItemPropertySpecialLineStruct>'s destructor 0x52BFA0 and insert 0x52BFF0, std::_Construct
**	0x52C3D0, and <string>'s guarded static 0x52C740 closing the object. No assert: the file's name is
**	provisional (D137, NAME 2), after its siblings, which the client names.
*/
