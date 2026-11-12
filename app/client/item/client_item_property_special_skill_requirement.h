// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\item/client_item_property_special_skill_requirement.h (the header
**	of the client's client_item_property_special_skill_requirement.cpp, whose asserts name it).
**
**	A skill requirement of an item as a special property: 'Requires ', the skill in bold, ' level ', the level in
**	bold; unusable when the player's skill is below the level.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CLIENT_ITEM_PROPERTY_SPECIAL_SKILL_REQUIREMENT_H  // NAME 2 [client_item_property_special_skill_requirement.h]: the header of the client's .cpp
#define CLIENT_ITEM_PROPERTY_SPECIAL_SKILL_REQUIREMENT_H

#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "client_item_property_special.h"
#include <string>
#include <vector>

class SClient;

class ClientItemPropertySpecialSkillRequirementClass : public ClientItemPropertySpecialClass  // NAME 2: class-from-file (D24) for client_item_property_special_skill_requirement.cpp (__FILE__ 0xB78430); table 0xAEE104, 0x3C bytes
{
public:
	enum RequirementType {																	// NAME 1 [RequirementType, REQUIRED_TO_EQUIP, REQUIRED_TO_ACTIVATE]: the tooltips they select
		REQUIRED_TO_EQUIP = 0,																// 'You must have this skill level to equip this item.' (the item's attribute 11)
		REQUIRED_TO_ACTIVATE = 1															// 'You must have this skill level to activate this item.' (attributes 27, 28)
	};

	static RefCountPtr<ClientItemPropertySpecialClass> Create(SClient *space_client, const std::string &skill, int level, RequirementType type);	// 0x52E760 (the sort order is the type; G_ASSERT(0) on another, line 48)  // NAME 1 [Create]: as the effect's; NAME 1 [skill, level, type]: what they hold

	ClientItemPropertySpecialSkillRequirementClass(SClient *space_client, int sort_order, const std::string &tooltip, const std::string &skill, int level);	// 0x52E9E0  // NAME 1 [space_client, sort_order, tooltip, skill, level]: the members they fill
	virtual ~ClientItemPropertySpecialSkillRequirementClass();								// 0x52EBE0 (deleting 0x52EBB0)
	virtual std::vector<ClientItemPropertySpecialLineStruct> Get_Lines();					// +0x14 0x52EC50 ('Requires ', Skill in bold, ' level ', Level in bold)
	virtual bool Is_Usable();																// +0x18 0x52F010 (the player's RPGInfo.Skills entry has Level or more; FAIL_IFs '!aux_data', '!skills_aux_data', 'skill_index < 0', 'skill_index >= (int)skills_aux_data->Get_Size()')

	SClient *SpaceClient;																	// +0x24  // NAME 1 [SpaceClient]: as the builder's
	std::string Skill;																		// +0x28 (a skill's name, SkillNamesDatabase's)  // NAME 1 [Skill]: what Is_Usable does with it
	int Level;																				// +0x38  // NAME 1 [Level]: what Get_Lines and Is_Usable do with it
};

#endif
