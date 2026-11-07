// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location assumed: e:\build\beta\G\app\Client\skill_data.h, from skill_data.cpp's __FILE__ 0xB91300; the client
**	holds no string of this header.
**
**	A slot's data for a skill: the views of the player's RPGInfo.Skills and SkillAbilities. Tables 0xAFD524 and, for its
**	RefCountClass at +0x18, 0xAFD518; constructor 0x6A45D0, destructor 0x6A4710 (deleting 0x6A46E0, its thunk 0x6A52D0).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __SKILL_DATA_H
#define __SKILL_DATA_H

#include "slot_data.h"
#include "skill_data_peek.h"
#include "skill_ability_data_peek.h"

class SkillDataClass : public SlotDataClass	// NAME 3: by convention (D24) from the client's file skill_data.cpp
{
public:
	SkillDataClass(SClient *client);								// 0x6A45D0
	virtual ~SkillDataClass();										// +0x00 0x6A4710
	virtual SkillDataClass *Unknown_14() { return(this); }	// +0x14 0x6A46C0
	virtual bool Unknown_18();										// +0x18 0x6A4F00
	virtual DeltaCompoundClass *Unknown_1C();					// +0x1C 0x6A4EE0
	virtual bool Unknown_20();										// +0x20 0x6A4F40
	virtual void Unknown_24();										// +0x24 0x6A4EA0

	int Get_Skill_Points();											// 0x6A47E0  // NAME 2: the client's field name 'RPGInfo.SkillPoints'
	int Get_Skill_Powerup_Start();								// 0x6A48C0  // NAME 2: the client's field name 'RPGInfo.SkillPowerupStart'
	int Get_Skill_Powerup_End();									// 0x6A4A00  // NAME 2: the client's field name 'RPGInfo.SkillPowerupEnd'
	int Get_Skill_Powerup_Ability_Number();					// 0x6A4B40  // NAME 2: the client's field name 'RPGInfo.SkillPowerupAbilityNumber'
	float Unknown_6A4C20();											// how far the powerup has run, 0 to 1  // NAME 1: neutral (D10)

	SkillDataPeekClass Skills;										// +0x50  // NAME 2: what it views
	SkillAbilityDataPeekClass Abilities;						// +0xB0  // NAME 2: what it views
};

#endif //__SKILL_DATA_H
