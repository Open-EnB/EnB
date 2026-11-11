// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\item/client_item_property_special_maker.h (the header of the
**	client's client_item_property_special_maker.cpp, whose G_ASSERT names it; the header itself has no string).
**
**	Who manufactured an item, as a special property: the bold label 'Manufactured by: ' and the maker's name; the
**	tooltip says whether a player or a corporation made it.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CLIENT_ITEM_PROPERTY_SPECIAL_MAKER_H  // NAME 2 [client_item_property_special_maker.h]: the header of the client's client_item_property_special_maker.cpp
#define CLIENT_ITEM_PROPERTY_SPECIAL_MAKER_H

#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "client_item_property_special.h"
#include <string>
#include <vector>

class ClientItemPropertySpecialMakerClass : public ClientItemPropertySpecialClass  // NAME 2: class-from-file (D24) for client_item_property_special_maker.cpp (__FILE__ 0xB77EA8); table 0xAEDF00, 0x34 bytes
{
public:
	enum MakerType {																		// NAME 1 [MakerType, MAKER_PLAYER, MAKER_CORPORATION]: the tooltips they select
		MAKER_PLAYER = 0,																	// 'This item was manufactured by a player.'
		MAKER_CORPORATION = 1																// 'This item was manufactured by a corporation.'
	};

	static RefCountPtr<ClientItemPropertySpecialMakerClass> Create(const std::string &maker, MakerType type);	// 0x52C770 (G_ASSERT(0) on another type, line 35)  // NAME 1 [Create]: as the effect's; NAME 1 [maker, type]: what they hold

	ClientItemPropertySpecialMakerClass(const std::string &maker, const std::string &tooltip);	// 0x52C980 (sort order 0x6E)  // NAME 1 [maker, tooltip]: the members they fill
	virtual ~ClientItemPropertySpecialMakerClass();											// 0x52CB80 (deleting 0x52CB50)
	virtual std::vector<ClientItemPropertySpecialLineStruct> Get_Lines();					// +0x14 0x52CBF0 ('Manufactured by: ' in bold, then Maker)
	virtual bool Is_Usable();																// +0x18 0x52CEC0 (always)

	std::string Maker;																		// +0x24 (the second line)  // NAME 1 [Maker]: what Get_Lines does with it
};

#endif
