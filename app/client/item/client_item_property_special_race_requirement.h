// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\item/client_item_property_special_race_requirement.h (the
**	header of the client's client_item_property_special_race_requirement.cpp, whose asserts name it).
**
**	A race requirement of an item as a special property: one line ('Terran Only', 'Progen Restricted' ...),
**	unusable when the player's race is one the item excludes.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CLIENT_ITEM_PROPERTY_SPECIAL_RACE_REQUIREMENT_H  // NAME 2 [client_item_property_special_race_requirement.h]: the header of the client's .cpp
#define CLIENT_ITEM_PROPERTY_SPECIAL_RACE_REQUIREMENT_H

#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "client_item_property_special.h"
#include <string>
#include <vector>

class SClient;

class ClientItemPropertySpecialRaceRequirementClass : public ClientItemPropertySpecialClass  // NAME 2: class-from-file (D24) for client_item_property_special_race_requirement.cpp (__FILE__ 0xB78324); table 0xAEE058, 0x3C bytes
{
public:
	static RefCountPtr<ClientItemPropertySpecialRaceRequirementClass> Create(SClient *space_client, int races, int race);	// 0x52DB60 (sort order 4; G_ASSERT(0) for none, line 43)  // NAME 1 [Create]: as the effect's; NAME 1 [races]: the bits it tests; NAME 1 [race]: the builder's attribute 17, which adds a bit

	ClientItemPropertySpecialRaceRequirementClass(SClient *space_client, int sort_order, int races, const std::string &label, const std::string &tooltip);	// 0x52E090  // NAME 1 [space_client, sort_order, races, label, tooltip]: the members they fill
	virtual ~ClientItemPropertySpecialRaceRequirementClass();							// 0x52E290 (deleting 0x52E260)
	virtual std::vector<ClientItemPropertySpecialLineStruct> Get_Lines();					// +0x14 0x52E300 (Label, not bold)
	virtual bool Is_Usable();																// +0x18 0x52E500 (false when the player's RPGInfo.Race is one of Races' bits; FAIL_IFs '!aux_data', '!race_aux_data')

	SClient *SpaceClient;																	// +0x24  // NAME 1 [SpaceClient]: as the builder's
	std::string Label;																		// +0x28 ('Terran Only' ...)  // NAME 1 [Label]: what Get_Lines does with it
	int Races;																		// +0x38 (the excluded races: 1 Terran, 2 Jenquai, 4 Progen)  // NAME 1 [Races]: what Is_Usable does with it
};

#endif
