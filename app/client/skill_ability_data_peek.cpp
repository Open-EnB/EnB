// Earth & Beyond client (reconstruction): app\client\skill_ability_data_peek.cpp, a view of RPGInfo.SkillAbilities
// (the client's file per its asserts, 'e:\build\beta\G\app\Client\skill_ability_data_peek.cpp' at 0xBA7990). Notes at the end.
#include <string.h>
#include "aux_roots.h"
#include "skill_ability_data_peek.h"

static const char *SkillAbilityFieldNames[7] = {	// 0xBA7910  // NAME 2: describes what it does
	"HasAbility", "Usage", "Targets", "Range", "Radius", "Validity", "CanBeUsedWhileIncapacitated"
};

// 0x8139F0
SkillAbilityDataPeekClass::SkillAbilityDataPeekClass(void *owner) :
	Owner(owner),
	Name("RPGInfo.SkillAbilities"),
	List(0),
	Count(0),
	Status(2)
{
	Update_Fields();
}

// 0x813A40
void SkillAbilityDataPeekClass::Detach()
{
	Owner = 0;
	List = 0;
	Count = 0;
	Update_Fields();
}

// 0x813A60
void SkillAbilityDataPeekClass::Attach(DeltaCompoundClass *root)
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

// 0x813B00
bool SkillAbilityDataPeekClass::Unknown_813B00()
{
	if (!List) {
		return(false);
	}
	return(List->Unknown70 > 1);
}

// 0x813B30
bool SkillAbilityDataPeekClass::Unknown_813B30(int ability)
{
	DeltaCompoundClass *d = Get_Ability(ability);
	if (!d) {
		return(false);
	}
	return(d->Unknown70 > 1);
}

// 0x813B70
bool SkillAbilityDataPeekClass::Get_Has_Ability(int ability)
{
	return(Peek_Value(ability, 0, (bool *)0));
}

// 0x813BE0
EffectGeneratorUsageBF SkillAbilityDataPeekClass::Get_Usage(int ability)
{
	return(Peek_Value(ability, 1, (EffectGeneratorUsageBF *)0));
}

// 0x813C50
TargetGeneratorSummaryBF SkillAbilityDataPeekClass::Get_Targets(int ability)
{
	return(Peek_Value(ability, 2, (TargetGeneratorSummaryBF *)0));
}

// 0x813CC0
int SkillAbilityDataPeekClass::Get_Range(int ability)
{
	return(Peek_Value(ability, 3, (int *)0));
}

// 0x813D30
int SkillAbilityDataPeekClass::Get_Radius(int ability)
{
	return(Peek_Value(ability, 4, (int *)0));
}

// 0x813DA0
GameActionValidityBitFlag SkillAbilityDataPeekClass::Get_Validity(int ability)
{
	return(Peek_Value(ability, 5, (GameActionValidityBitFlag *)0));
}

// 0x813E10
bool SkillAbilityDataPeekClass::Get_Can_Be_Used_While_Incapacitated(int ability)
{
	return(Peek_Value(ability, 6, (bool *)0));
}






































#pragma auto_inline(off)
// 0x813E80
DeltaCompoundClass *SkillAbilityDataPeekClass::Get_Ability(int ability)
{
	Status = 2;
	if (!List) return(0);
	if ((unsigned int)ability >= Count) return(0);
	DeltaClass *dskill = List->Children[ability];



	FAIL_IF(!dskill) return(0);
	DeltaCompoundClass *dcont = dskill->As_Compound();

	FAIL_IF(!dcont) return(0);
	if (dcont->Unknown70) {
		Status = 1 >= dcont->Unknown70;
	}
	return(dcont);
}

// 0x813F40
DeltaClass *SkillAbilityDataPeekClass::Get_Field(int ability, int field)
{
	Status = 2;
	int index = Fields[field];
	if (index == -1) {
		return(0);
	}
	DeltaCompoundClass *dcont = Get_Ability(ability);
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









// 0x814000
void SkillAbilityDataPeekClass::Update_Fields()
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
				if (strcmp(field_name, SkillAbilityFieldNames[j]) == 0) {
					Fields[j] = i;
					break;
				}
			}
		}
	}
}

/*
**	Notes.
**	skill_data_peek.cpp's twin. Object from 0x8139A0: permstr.h's PermStringInit (the instance 0xBE8D58, its atexit 0x8139D0)
**	opens it. Its end: see importer.py's RECONSTRUCTED entry.
*/
