// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\skill_ability_data_peek.h (the client's __FILE__ string 0xBA7950, G_ASSERT 'false'
**	on line 134 in Peek_Value, inlined in 0x813B70-0x813E10)
**
**	A read-only view of the aux tree's RPGInfo.SkillAbilities: ability N's seven fields by the client's field names (table 0xBA7910).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __SKILL_ABILITY_DATA_PEEK_H
#define __SKILL_ABILITY_DATA_PEEK_H

#include "..\shared\auxdata\auxdata.h"
#include "..\shared\g_assert.h"


enum EffectGeneratorUsageBF;
enum GameActionValidityBitFlag;
enum TargetGeneratorSummaryBF;
class SkillAbilityDataPeekClass  // NAME 3: by convention (D24) from the client's file skill_ability_data_peek.cpp
{
public:
	SkillAbilityDataPeekClass(void *owner);							// 0x8139F0

	~SkillAbilityDataPeekClass();							// 0x813A40 (0x6A4710's unwind states; it clears, the name stays)
	void Attach(DeltaCompoundClass *root);					// 0x813A60  // NAME 2: describes what it does

	bool Unknown_813B00();										// the list's Unknown70 > 1  // NAME 1: neutral (D10)
	bool Unknown_813B30(int ability);						// ability's Unknown70 > 1  // NAME 1: neutral (D10)

	bool Get_Has_Ability(int ability);						// 0x813B70  // NAME 2: the client's field name (0xBA7910)
	EffectGeneratorUsageBF Get_Usage(int ability);		// 0x813BE0  // NAME 2: the client's field name (0xBA7910)
	TargetGeneratorSummaryBF Get_Targets(int ability);	// 0x813C50  // NAME 2: the client's field name (0xBA7910)
	int Get_Range(int ability);								// 0x813CC0  // NAME 2: the client's field name (0xBA7910)
	int Get_Radius(int ability);								// 0x813D30  // NAME 2: the client's field name (0xBA7910)
	GameActionValidityBitFlag Get_Validity(int ability);	// 0x813DA0  // NAME 2: the client's field name (0xBA7910)
	bool Get_Can_Be_Used_While_Incapacitated(int ability);	// 0x813E10  // NAME 2: the client's field name (0xBA7910)

protected:
	DeltaCompoundClass *Get_Ability(int ability);			// 0x813E80  // NAME 2: describes what it does
	DeltaClass *Get_Field(int ability, int field);		// 0x813F40  // NAME 2: describes what it does
	void Update_Fields();										// 0x814000  // NAME 2: describes what it does

	// VC6 can't take a member template's explicit argument: the unused pointer names T
	template<class T> T Peek_Value(int ability, int field, T *)  // NAME 2: describes what it does
	{
		DeltaClass *d = Get_Field(ability, field);
		if (d) {
			if (d->Unknown04 == DeltaTemplateClass<T>::UnknownGlobal) {
				return(((DeltaTemplateClass<T> *)d)->Value);
			}














































































			G_ASSERT(false);
		}
		Status = 2;
		return(T());
	}

	void *Owner;					// +0x00 the constructor's argument  // NAME 2: describes what it does
	NameClass Name;				// +0x04 'RPGInfo.SkillAbilities'  // NAME 2: describes what it does
	DeltaCompoundClass *List;	// +0x38 what Name finds under Attach's root  // NAME 2: describes what it does
	unsigned int Count;			// +0x3C List's children  // NAME 2: describes what it does
	int Fields[7];					// +0x40 each field's position in an ability, -1 if it has none  // NAME 2: describes what it does
	int Status;						// +0x5C 2 after a failed read, else the field's Unknown70 < 2  // NAME 2: describes what it does
};

#endif //__SKILL_ABILITY_DATA_PEEK_H
