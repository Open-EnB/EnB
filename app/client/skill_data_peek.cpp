// Earth & Beyond client (reconstruction): app\client\skill_data_peek.cpp, a view of RPGInfo.Skills
// (the client's file per its asserts, 'e:\build\beta\G\app\Client\skill_data_peek.cpp' at 0xB914A4). Notes at the end.
#include <string.h>
#include "skill_data_peek.h"
#include "aux_roots.h"

static const char *SkillFieldNames[7] = {	// 0xB913DC  // NAME 2: describes what it does
	"Level", "RecycleTime", "LastActivation", "Availability", "Affiliations", "QuestOnlyLevels", "MaxSkillLevels"
};

// A value with a destructor (0x6A5640's atexit 0x6A5710 is an empty one; SkillLevelAvailabilityStruct itself has none)
struct UnknownEmptyAvailability { SkillLevelAvailabilityStruct Value; ~UnknownEmptyAvailability() {} };	// NAME 2: describes what it does

// 0x6A5330
SkillDataPeekClass::SkillDataPeekClass(void *owner) :
	Owner(owner),
	Name("RPGInfo.Skills"),
	List(0),
	Count(0),
	Status(2)
{
	Update_Fields();
}

// 0x6A5380
SkillDataPeekClass::~SkillDataPeekClass()
{
	Owner = 0;
	List = 0;
	Count = 0;
	Update_Fields();
}

// 0x6A53A0
void SkillDataPeekClass::Attach(DeltaCompoundClass *root)
{
	List = 0;
	Count = 0;
	if (root && Name.Get_String()[0]) {
		const NameClass &name = Name;
		LookupClass *lookup = root->Get_Lookup();
		DeltaClass *d = lookup->Find(name);
		if (d) {
			List = d->As_Compound();
			if (List) {
				Count = List->Children.size();
			}
		}
	}
	Update_Fields();
}

// 0x6A5440
bool SkillDataPeekClass::Unknown_6A5440()
{
	if (!List) {
		return(false);
	}
	return(List->Unknown70 > 1);
}

// 0x6A5470
bool SkillDataPeekClass::Unknown_6A5470(int skill)
{
	DeltaCompoundClass *d = Get_Skill(skill);
	if (!d) {
		return(false);
	}
	return(d->Unknown70 > 1);
}

// 0x6A54B0
int SkillDataPeekClass::Get_Level(int skill)
{
	return(Peek_Value(skill, 0, (int *)0));
}

// 0x6A5520
DurationClass SkillDataPeekClass::Get_Recycle_Time(int skill)
{
	return(Peek_Value(skill, 1, (int *)0));
}








































// 0x6A55B0
DurationClass SkillDataPeekClass::Get_Last_Activation(int skill)
{
	DeltaClass *d = Get_Field(skill, 2);
	if (d) {
		if (d->Unknown04 == DeltaTemplateClass<DurationClass>::UnknownGlobal)
			return(((DeltaTemplateClass<DurationClass> *)d)->Value);
		G_ASSERT(false);
	}
	Status = 2;
	return(DurationClass(0));
}

// 0x6A5640
SkillLevelAvailabilityStruct &SkillDataPeekClass::Get_Availability(int skill)
{
	static UnknownEmptyAvailability empty;
	DeltaClass *d = Get_Field(skill, 3);
	if (d) {
		if (d->Unknown04 == DeltaTemplateClass<SkillLevelAvailabilityStruct>::UnknownGlobal)
			return(((DeltaTemplateClass<SkillLevelAvailabilityStruct> *)d)->Value);
		G_ASSERT(false);
	}
	Status = 2;
	return(empty.Value);
}
// NOT EXACT: 0x6A57B5 has mov byte ptr [ebx + ecx], 0 where this has [ecx + ebx] (register-only, three forms of the return)
// 0x6A5730
std::string SkillDataPeekClass::Get_Affiliations(int skill)
{
	DeltaClass *d = Get_Field(skill, 4);
	if (d) {
		if (d->Unknown04 == DeltaStringClass::UnknownGlobal) return(std::string(((DeltaStringClass *)d)->Get()));
		G_ASSERT(false);
	}
	Status = 2;
	static std::string empty("");
	return(empty);
}

// 0x6A5970
int SkillDataPeekClass::Get_Quest_Only_Levels(int skill)
{
	return(Peek_Value(skill, 5, (int *)0));
}

// 0x6A59E0
int SkillDataPeekClass::Get_Max_Skill_Levels(int skill)
{
	return(Peek_Value(skill, 6, (int *)0));
}

#pragma auto_inline(off)
// 0x6A5A50
DeltaCompoundClass *SkillDataPeekClass::Get_Skill(int skill)
{
	Status = 2;
	if (!List) return(0);
	if ((unsigned int)skill >= Count) return(0);
	DeltaClass *dskill = List->Children[skill];



	FAIL_IF(!dskill) return(0);
	DeltaCompoundClass *dcont = dskill->As_Compound();

	FAIL_IF(!dcont) return(0);
	if (dcont->Unknown70) {
		Status = 1 >= dcont->Unknown70;
	}
	return(dcont);
}

// 0x6A5B10
DeltaClass *SkillDataPeekClass::Get_Field(int skill, int field)
{
	Status = 2;
	int index = Fields[field];
	if (index == -1) {
		return(0);
	}
	DeltaCompoundClass *dcont = Get_Skill(skill);
	if (!dcont) {
		return(0);
	}
	DeltaClass *dskill = dcont->Children[index];


	FAIL_IF(!dskill) {
		Status = 2;
		return(0);
	}
	if (dskill->Unknown70) {
		Status = 1 >= dskill->Unknown70;
		return(dskill);
	}
	Status = 2;
	return(dskill);
}






// 0x6A5BD0
void SkillDataPeekClass::Update_Fields()
{
	for (int f = 0; f < 7; f++) Fields[f] = -1;
	if (List) {
		DeltaClass *dskill = List->Children[0];


		FAIL_IF(!dskill) return;
		DeltaCompoundClass *dcont = dskill->As_Compound();

		FAIL_IF(!dcont) return;
		int count = dcont->Children.size();
		for (int i = 0; i < count; i++) {
			dskill = dcont->Children[i];


			FAIL_IF(!dskill) continue;
			const char *field_name = dskill->Name.Get_String();



			FAIL_IF(!field_name) continue;
			for (int j = 0; j < 7; j++) {
				if (strcmp(field_name, SkillFieldNames[j]) == 0) {
					Fields[j] = i;
					break;
				}
			}
		}
	}
}

/*
**	Notes.
**	Object 0x6A52E0-0x6A5E9F: opened by permstr.h's PermStringInit (0x6A52E0, the instance 0xBE6658, its atexit 0x6A5310),
**	closed by <string>'s initialiser 0x6A5E00. The thunk 0x6A52D0 before it is skill_data.cpp's.
*/
