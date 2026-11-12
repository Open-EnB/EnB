// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\item/client_item_property_special_profession_requirement.h (the
**	header of the client's client_item_property_special_profession_requirement.cpp, whose asserts name it).
**
**	A profession requirement of an item as a special property: one line ('Warriors Only', 'Trader Restricted' ...),
**	unusable when the player's profession is one the item excludes.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CLIENT_ITEM_PROPERTY_SPECIAL_PROFESSION_REQUIREMENT_H  // NAME 2 [client_item_property_special_profession_requirement.h]: the header of the client's .cpp
#define CLIENT_ITEM_PROPERTY_SPECIAL_PROFESSION_REQUIREMENT_H

#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "client_item_property_special.h"
#include <string>
#include <vector>

class SClient;

class ClientItemPropertySpecialProfessionRequirementClass : public ClientItemPropertySpecialClass  // NAME 2: class-from-file (D24) for client_item_property_special_profession_requirement.cpp (__FILE__ 0xB780B4); table 0xAEDFAC, 0x3C bytes
{
public:
	static RefCountPtr<ClientItemPropertySpecialProfessionRequirementClass> Create(SClient *space_client, int professions);	// 0x52CF90 (sort order 4; G_ASSERT(0) for none, line 37)  // NAME 1 [Create]: as the effect's; NAME 1 [professions]: the bits it tests

	ClientItemPropertySpecialProfessionRequirementClass(SClient *space_client, int sort_order, int professions, const std::string &label, const std::string &tooltip);	// 0x52D490  // NAME 1 [space_client, sort_order, professions, label, tooltip]: the members they fill
	virtual ~ClientItemPropertySpecialProfessionRequirementClass();							// 0x52D690 (deleting 0x52D660)
	virtual std::vector<ClientItemPropertySpecialLineStruct> Get_Lines();					// +0x14 0x52D700 (Label, not bold)
	virtual bool Is_Usable();																// +0x18 0x52D900 (false when the player's RPGInfo.Profession is one of Professions' bits; FAIL_IFs '!aux_data', '!profession_aux_data')

	SClient *SpaceClient;																	// +0x24  // NAME 1 [SpaceClient]: as the builder's
	std::string Label;																		// +0x28 ('Warriors Only' ...)  // NAME 1 [Label]: what Get_Lines does with it
	int Professions;																		// +0x38 (the excluded professions: 1 warriors, 2 tradesmen, 4 explorers)  // NAME 1 [Professions]: what Is_Usable does with it
};

#endif
