// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\skill_data_peek.h (the client's __FILE__ string 0xB9146C, G_ASSERT 'false'
**	on line 138 in Peek_Value, inlined in 0x6A54B0, 0x6A5520, 0x6A5970 and 0x6A59E0)
**
**	A read-only view of the aux tree's RPGInfo.Skills: skill N's seven fields by the client's field names (table 0xB913DC).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __SKILL_DATA_PEEK_H
#define __SKILL_DATA_PEEK_H

#include <string>
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\auxdata\delta_string.h"
#include "..\shared\duration.h"
#include "..\shared\g_assert.h"

struct SkillLevelAvailabilityStruct;

class SkillDataPeekClass  // NAME 3: by convention (D24) from the client's file skill_data_peek.cpp
{
public:
	SkillDataPeekClass(void *owner);							// 0x6A5330

	~SkillDataPeekClass();							// 0x6A5380 (0x6A4710's unwind states; it clears, the name stays)
	void Attach(DeltaCompoundClass *root);					// 0x6A53A0  // NAME 2: describes what it does

	bool Unknown_6A5440();										// the list's Unknown70 > 1  // NAME 1: neutral (D10)
	bool Unknown_6A5470(int skill);							// skill's Unknown70 > 1  // NAME 1: neutral (D10)

	int Get_Level(int skill);									// 0x6A54B0  // NAME 2: the client's field name (0xB913DC)
	DurationClass Get_Recycle_Time(int skill);				// 0x6A5520  // NAME 2: the client's field name (0xB913DC)
	DurationClass Get_Last_Activation(int skill);			// 0x6A55B0  // NAME 2: the client's field name (0xB913DC)
	SkillLevelAvailabilityStruct &Get_Availability(int skill);	// 0x6A5640  // NAME 2: the client's field name (0xB913DC)
	std::string Get_Affiliations(int skill);				// 0x6A5730  // NAME 2: the client's field name (0xB913DC)
	int Get_Quest_Only_Levels(int skill);					// 0x6A5970  // NAME 2: the client's field name (0xB913DC)
	int Get_Max_Skill_Levels(int skill);					// 0x6A59E0  // NAME 2: the client's field name (0xB913DC)

protected:
	DeltaCompoundClass *Get_Skill(int skill);				// 0x6A5A50  // NAME 2: describes what it does
	DeltaClass *Get_Field(int skill, int field);			// 0x6A5B10  // NAME 2: describes what it does
	void Update_Fields();										// 0x6A5BD0  // NAME 2: describes what it does

	// VC6 can't take a member template's explicit argument: the unused pointer names T
	template<class T> T Peek_Value(int skill, int field, T *)  // NAME 2: describes what it does
	{
		DeltaClass *d = Get_Field(skill, field);
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
	NameClass Name;				// +0x04 'RPGInfo.Skills'  // NAME 2: describes what it does
	DeltaCompoundClass *List;	// +0x38 what Name finds under Attach's root  // NAME 2: describes what it does
	unsigned int Count;			// +0x3C List's children  // NAME 2: describes what it does
	int Fields[7];					// +0x40 each field's position in a skill, -1 if it has none  // NAME 2: describes what it does
	int Status;						// +0x5C 2 after a failed read, else the field's Unknown70 < 2  // NAME 2: describes what it does
};

#endif //__SKILL_DATA_PEEK_H
