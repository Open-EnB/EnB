// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\item/client_item_property_special_level_requirement.h (the header
**	of the client's client_item_property_special_level_requirement.cpp, whose asserts name it).
**
**	A level requirement of an item as a special property: 'Requires level 12' (the total level) or 'Requires Combat
**	level 12'; unusable when the player's level of that kind is below it.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CLIENT_ITEM_PROPERTY_SPECIAL_LEVEL_REQUIREMENT_H  // NAME 2 [client_item_property_special_level_requirement.h]: the header of the client's .cpp
#define CLIENT_ITEM_PROPERTY_SPECIAL_LEVEL_REQUIREMENT_H

#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "client_item_property_special.h"
#include <string>
#include <vector>

class SClient;

class ClientItemPropertySpecialLevelRequirementClass : public ClientItemPropertySpecialClass  // NAME 2: class-from-file (D24) for client_item_property_special_level_requirement.cpp (__FILE__ 0xB785C0); table 0xAEE1EC, 0x30 bytes
{
public:
	enum {																					// NAME 1 [KIND_COMBAT, KIND_EXPLORE, KIND_TRADE, KIND_TOTAL]: the names Kind_Name gives them ('RPGInfo.CombatLevel' ...)
		KIND_COMBAT = 0,
		KIND_EXPLORE = 1,
		KIND_TRADE = 2,
		KIND_TOTAL = 3
	};

	static RefCountPtr<ClientItemPropertySpecialLevelRequirementClass> Create(SClient *space_client, int kind, int level);	// 0x530530  // NAME 1 [Create]: as the effect's; NAME 1 [kind, level]: the members they fill

	ClientItemPropertySpecialLevelRequirementClass(SClient *space_client, const std::string &tooltip, int kind, int level);	// 0x530A00 (sort order 4)  // NAME 1 [space_client, tooltip, kind, level]: the members they fill
	virtual ~ClientItemPropertySpecialLevelRequirementClass();								// 0x530A80 (deleting 0x530A50)
	virtual std::vector<ClientItemPropertySpecialLineStruct> Get_Lines();					// +0x14 0x530AA0 ('Requires level ' or 'Requires ', the kind in bold, ' level '; then Level in bold)
	virtual bool Is_Usable();																// +0x18 0x531080 (RPGInfo.<Kind>Level is Level or more; FAIL_IFs '!aux_data', '!rpg_aux_data', '!level_aux_data')

	SClient *SpaceClient;																	// +0x24  // NAME 1 [SpaceClient]: as the builder's
	int Kind;																				// +0x28 (KIND_COMBAT ...)  // NAME 1 [Kind]: what Kind_Name names
	int Level;																				// +0x2C  // NAME 1 [Level]: what Get_Lines and Is_Usable do with it
};

#endif
