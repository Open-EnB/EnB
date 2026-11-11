#include "client_item_property_special_maker.h"
#include "..\..\shared\auxdata\permstr.h"	// (its PermStringInitClass static: 0x52CF40, its atexit 0x52CF70)





















RefCountPtr<ClientItemPropertySpecialMakerClass> ClientItemPropertySpecialMakerClass::Create(const std::string &maker, MakerType type)
{
	std::string tooltip;
	switch (type) {
		case MAKER_PLAYER:
			tooltip = "This item was manufactured by a player.";
			break;
		case MAKER_CORPORATION:
			tooltip = "This item was manufactured by a corporation.";
			break;
		default:
			G_ASSERT(0);
			return(RefCountPtr<ClientItemPropertySpecialMakerClass>());
	}
	return(Create_NEW(new ClientItemPropertySpecialMakerClass(maker, tooltip)));
}

ClientItemPropertySpecialMakerClass::ClientItemPropertySpecialMakerClass(const std::string &maker, const std::string &tooltip) :
	ClientItemPropertySpecialClass(0x6E, tooltip, 0, 1),
	Maker(maker)
{
}

ClientItemPropertySpecialMakerClass::~ClientItemPropertySpecialMakerClass()
{
}

std::vector<ClientItemPropertySpecialLineStruct> ClientItemPropertySpecialMakerClass::Get_Lines()
{
	std::vector<ClientItemPropertySpecialLineStruct> lines;
	lines.push_back(ClientItemPropertySpecialLineStruct("Manufactured by: ", true));
	lines.push_back(ClientItemPropertySpecialLineStruct(Maker, false));
	return(lines);
}

bool ClientItemPropertySpecialMakerClass::Is_Usable()
{
	return(true);
}

/*
**	Reconstruction notes (RECONSTRUCTED): the client's 0x52C770-0x52CF8F (__FILE__ 0xB77EA8,
**	'e:\build\beta\G\app\Client\item\client_item_property_special_maker.cpp'), the maker as a special property
**	(table 0xAEDF00, 0x34 bytes): Create 0x52C770 (the constructor inlined into it; G_ASSERT(0) at line 35), the
**	constructor 0x52C980, the destructors 0x52CB80 / 0x52CB50, the table's slots 0x52CBF0 (Get_Lines) and
**	0x52CEC0 (Is_Usable), and Create_NEW<ClientItemPropertySpecialMakerClass> 0x52CEE0. The line struct's and the
**	vector's code the compiler emits here is the effect's (client_item_property_special_effect.cpp, linked first).
*/
