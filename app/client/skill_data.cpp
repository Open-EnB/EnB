// Earth & Beyond client (reconstruction): app\client\skill_data.cpp, a slot's data for a skill
// (the client's file per its asserts, 'e:\build\beta\G\app\Client\skill_data.cpp' at 0xB91300). Notes at the end.
#include "skill_data.h"
#include "aux_roots.h"

// (0x818590's assert names it; skillnam.cpp is not rebuilt yet)
class SkillNamesDatabase { public: static int Max_Skill(); };	// NAME 5: 0x6A4F40's assert
// as co.cpp's
template<class C> inline C *Unknown_Find_Valid(DeltaCompoundClass *root, const NameClass &name) { C *t = Unknown_Find_Field<C>(root, name); if (t && t->Unknown70) return(t); return(NULL); }	// NAME 1: neutral (D10)
// (+0x10BC of the client: the time; sclient.h does not hold it, skill_render.cpp's stand-in does)
struct UnknownClientTime { char Unknown00[0x10BC]; unsigned int Time; };	// NAME 2: describes what it does

// 0x6A45D0
SkillDataClass::SkillDataClass(SClient *client) :
	SlotDataClass(client),
	Skills(client),
	Abilities(client)
{
	Skills.Attach((DeltaCompoundClass *)Client->AuxData);
	Abilities.Attach((DeltaCompoundClass *)Client->AuxData);
}

// 0x6A4710
SkillDataClass::~SkillDataClass()
{
	Unknown_24();
}






























// 0x6A47E0
int SkillDataClass::Get_Skill_Points()
{

	FAIL_IF(!Peek_Aux_Data()) return(0);
	NameClass name("RPGInfo.SkillPoints");
	DeltaTemplateClass<int> *int_ptr = Unknown_Find_Valid<DeltaTemplateClass<int> >(Peek_Aux_Data(), name);
	FAIL_IF(!int_ptr) return(0);
	return(int_ptr->Value);
}

// 0x6A48C0
int SkillDataClass::Get_Skill_Powerup_Start()
{
	FAIL_IF(!Peek_Aux_Data()) return(0);
	NameClass name("RPGInfo.SkillPowerupStart"); DeltaTemplateClass<DurationClass> *time_ptr = Unknown_Find_Valid<DeltaTemplateClass<DurationClass> >(Peek_Aux_Data(), name);
	FAIL_IF(!time_ptr) return(0);
	return(time_ptr->Value.As_Int());
}
// 0x6A4A00
int SkillDataClass::Get_Skill_Powerup_End()
{
	FAIL_IF(!Peek_Aux_Data()) return(0);
	NameClass name("RPGInfo.SkillPowerupEnd"); DeltaTemplateClass<DurationClass> *time_ptr = Unknown_Find_Valid<DeltaTemplateClass<DurationClass> >(Peek_Aux_Data(), name);
	FAIL_IF(!time_ptr) return(0);
	return(time_ptr->Value.As_Int());
}

// 0x6A4B40
int SkillDataClass::Get_Skill_Powerup_Ability_Number()
{
	FAIL_IF(!Peek_Aux_Data()) return(0);
	NameClass name("RPGInfo.SkillPowerupAbilityNumber");
	DeltaTemplateClass<int> *int_ptr = Unknown_Find_Valid<DeltaTemplateClass<int> >(Peek_Aux_Data(), name);
	FAIL_IF(!int_ptr) return(0);
	return(int_ptr->Value);
}
// NOT EXACT: inline-budget, the client calls 0x610290 (Unknown_Find_Field) out of line in its two inlined reads
// 0x6A4C20
float SkillDataClass::Unknown_6A4C20()
{
	FAIL_IF(!Get_Client()) return(0.0f);
	unsigned int start = Get_Skill_Powerup_Start();
	unsigned int end = Get_Skill_Powerup_End();
	unsigned int now = ((UnknownClientTime *)Client)->Time;
	if (now < end && start < now) {
		return((float)(now - start) / (int)(end - start));
	}
	return(0.0f);
}

// 0x6A4EA0
void SkillDataClass::Unknown_24()
{
	Skills.Attach(Unknown_1C());
	Abilities.Attach(Unknown_1C());
	SlotDataClass::Unknown_24();
}

#pragma auto_inline(off)
// 0x6A4EE0
DeltaCompoundClass *SkillDataClass::Unknown_1C()
{
	return((DeltaCompoundClass *)Client->AuxData);
}
#pragma auto_inline(on)

// 0x6A4F00
bool SkillDataClass::Unknown_18()
{
	if (!Skills.Unknown_6A5440() && !Abilities.Unknown_813B00()) {
		return(false);
	}
	return(true);
}











// NOT EXACT: inline-budget, the client inlines only the -1 push_back and calls 0x4A0880 for the other two
// 0x6A4F40
bool SkillDataClass::Unknown_20()
{
	if (!Unknown40.empty()) {
		return(false);
	}
	SlotDataClass::Unknown_24();
	int num_skills;


	G_ASSERT(num_skills = SkillNamesDatabase::Max_Skill());
	for (int skill = 0; skill < num_skills; skill++) {
		SkillLevelAvailabilityStruct &available = Skills.Get_Availability(skill);
		int index;
		if (available.I00 == 0 || available.I00 == 1) {
			index = -1;
			Unknown30.push_back(index);
		} else {
			index = Unknown40.size();
			Unknown30.push_back(index);
			Unknown40.push_back(skill);
		}
	}
	return(true);
}

/*
**	Notes.
**	Object 0x6A4580-0x6A52D8: permstr.h's PermStringInit (0x6A4580, the instance 0xBE663C, its atexit 0x6A45B0) opens it; the
**	adjustor thunk 0x6A52D0 (RefCountClass's slot +0x04, table 0xAFD518) closes it. 0x6A46A0 is SlotDataClass's Unknown_10.
*/
