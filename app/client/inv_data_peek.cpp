// Earth & Beyond client (reconstruction): app\client\inv_data_peek.cpp, a view of an inventory list
// (the client's file per its asserts, 'e:\build\beta\G\app\Client\inv_data_peek.cpp' at 0xB8FFCC). Notes at the end.
#define __DELTA_ARRAY_MEMBERS	// delta_array.h's Get_Value
#include <string.h>
#include "inv_data_peek.h"

static const char *ItemFieldNames[14] = {	// 0xB8FEB4  // NAME 2: describes what it does
	"ItemTemplateID", "StackCount", "Price", "AveCost", "Structure", "ReadyTime", "TargetRange", "ItemState",
	"Quality", "InstanceInfo", "InstanceActivatedEffectInfo", "InstanceEquipEffectInfo", "BuilderName", "Effects"
};

class Unknown831340Class; Unknown831340Class *Unknown_8318B0(int item_template_id, ClientClass *client);	// item_template.cpp's (client.h's item template)  // NAME 1: neutral (D10); NAME 3 [item_template_id]: ui_manu.cpp's

// 0x689A80
InvDataPeekClass::InvDataPeekClass(ClientClass *client, const char *name, bool mounts) :
	Owner(client),
	Name(name),
	List(0),
	HasMounts(mounts),
	Mounts(0),
	Count(0),
	Status(2)
{
	Update_Fields();
}

// 0x689AD0
InvDataPeekClass::~InvDataPeekClass()
{
	Owner = 0;
	List = 0;
	Count = 0;
	Update_Fields();
}

















































// 0x689AF0
void InvDataPeekClass::Attach(DeltaCompoundClass *root)
{
	List = 0;
	Count = 0;
	Mounts = 0;
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
		if (HasMounts) {
			NameClass mounts_name("Inventory.Mounts");  // NAME 1: neutral (D10)
			DeltaArrayClass<UnknownB016DCType> *t = Unknown_Find_Field<DeltaArrayClass<UnknownB016DCType> >(root, mounts_name);
			Mounts = (t && t->Unknown70) ? t : 0;
			G_ASSERT(Mounts);
		}
	}
	Update_Fields();
}

// 0x689C30
bool InvDataPeekClass::Changed()
{
	if (!List) {
		return(false);
	}
	return(List->Unknown70 > 1);
}

// 0x689C60
bool InvDataPeekClass::Slot_Changed(int slot)
{
	DeltaCompoundClass *d = Get_Slot(slot);
	if (!d) {
		return(false);
	}
	return(d->Unknown70 > 1);
}

// 0x689CA0
int InvDataPeekClass::Get_Item_Template_ID(int slot) { return(Peek_Value(slot, 0, -2)); }

// 0x689D20
int InvDataPeekClass::Get_Stack_Count(int slot) { return(Peek_Value(slot, 1, 0)); }

// 0x689DA0 (Peek_Object out of line at 0x689E20)
MoneyClass InvDataPeekClass::Get_Price(int slot) { return(Peek_Object(slot, 2, MoneyClass())); }

// 0x689F10
float InvDataPeekClass::Get_Ave_Cost(int slot) { return(Peek_Value(slot, 3, 0.0f)); }

// 0x689F90
float InvDataPeekClass::Get_Structure(int slot) { return(Peek_Value(slot, 4, 0.0f)); }

// 0x68A010
float InvDataPeekClass::Get_Quality(int slot) { return(Peek_Value(slot, 8, 0.0f)); }









// NOT EXACT: the failure path's inlined string constructor has mov byte ptr [ecx + ebx], 0 where the client has [ebx + ecx] (register-only, as skill_data_peek's 0x6A5730)
// 0x68A090
std::string InvDataPeekClass::Get_Instance_Info(int slot)
{
	DeltaStringClass *encoded_info_delta_string = UnknownRTTIClass<DeltaStringClass>::Cast(Get_Field(slot, 9));
	FAIL_IF(!encoded_info_delta_string) return(NULL);
	return(encoded_info_delta_string->Get());
}
// NOT EXACT: the failure path's inlined string constructor has mov byte ptr [ecx + ebx], 0 where the client has [ebx + ecx] (register-only, as skill_data_peek's 0x6A5730)
// 0x68A270
std::string InvDataPeekClass::Get_Instance_Activated_Effect_Info(int slot)
{
	DeltaStringClass *encoded_effect_values_string = UnknownRTTIClass<DeltaStringClass>::Cast(Get_Field(slot, 10));
	FAIL_IF(!encoded_effect_values_string) return(NULL);
	return(encoded_effect_values_string->Get());
}
// NOT EXACT: the failure path's inlined string constructor has mov byte ptr [ecx + ebx], 0 where the client has [ebx + ecx] (register-only, as skill_data_peek's 0x6A5730)
// 0x68A450
std::string InvDataPeekClass::Get_Instance_Equip_Effect_Info(int slot)
{
	DeltaStringClass *encoded_effect_values_string = UnknownRTTIClass<DeltaStringClass>::Cast(Get_Field(slot, 11));
	FAIL_IF(!encoded_effect_values_string) return(NULL);
	return(encoded_effect_values_string->Get());
}

// 0x68A630
float InvDataPeekClass::Get_Effect_Range(int slot)
{
	DeltaEffectClass *di = (DeltaEffectClass *)Get_Field(slot, 13);
	FAIL_IF(!di) return(0);
	return(di->Range.Value);
}

// 0x68A680
EffectGeneratorUsageBF InvDataPeekClass::Get_Effect_Usage(int slot)
{
	DeltaEffectClass *di = (DeltaEffectClass *)Get_Field(slot, 13);
	if (!di) return((EffectGeneratorUsageBF)0);
	return(di->Usage.Value);
}

// 0x68A6B0
TargetGeneratorSummaryBF InvDataPeekClass::Get_Effect_Targets(int slot)
{
	DeltaEffectClass *di = (DeltaEffectClass *)Get_Field(slot, 13);
	if (!di) return((TargetGeneratorSummaryBF)0);
	return(di->Targets.Value);
}

// 0x68A6E0
GameActionValidityBitFlag InvDataPeekClass::Get_Effect_Validity(int slot)
{
	DeltaEffectClass *di = (DeltaEffectClass *)Get_Field(slot, 13);
	if (!di) return((GameActionValidityBitFlag)256);
	return(di->Validity.Value);
}





























// 0x68A720
DurationClass InvDataPeekClass::Get_Ready_Time(int slot)
{
	DeltaClass *d = Get_Field(slot, 5);
	if (d) {
		if (d->Unknown04 == DeltaTemplateClass<DurationClass>::UnknownGlobal)
			return(((DeltaTemplateClass<DurationClass> *)d)->Value);
		G_ASSERT(false);
	}
	Status = 2;
	return(DurationClass(0));
}

// 0x68A7C0
float InvDataPeekClass::Get_Target_Range(int slot) { return(Peek_Value(slot, 6, 0.0f)); }

// 0x68A840
ItemStateEnum InvDataPeekClass::Get_Item_State(int slot) { return(Peek_Value(slot, 7, (ItemStateEnum)0)); }



// NOT EXACT: the failure path's inlined string constructor has mov byte ptr [ecx + ebx], 0 where the client has [ebx + ecx] (register-only, as skill_data_peek's 0x6A5730)
// 0x68A8C0
std::string InvDataPeekClass::Get_Builder_Name(int slot)
{
	DeltaStringClass *encoded_info_delta_string = UnknownRTTIClass<DeltaStringClass>::Cast(Get_Field(slot, 12));
	FAIL_IF(!encoded_info_delta_string) return(NULL);
	return(encoded_info_delta_string->Get());
}












// 0x68AAA0
InvDataPeekItemStruct *InvDataPeekClass::Get_Item(int slot, bool flag)
{
	int id = Peek_Value(slot, 0, -2);
	if (Status == 2) return(0);
	if (id == -2) return(0);
	if (id == -1 && !flag) return(0);
	InvDataPeekItemStruct *item = (InvDataPeekItemStruct *)Unknown_8318B0(id, Owner);
	if (item) return(item);
	G_ASSERT(false);
	return((InvDataPeekItemStruct *)Unknown_8318B0(-1, Owner));
}

// 0x68AB80
UnknownB016DCType *InvDataPeekClass::Get_Mount(int slot)
{
	Status = 2;
	if ((unsigned int)slot < Count) {
		if (Mounts) {
			UnknownB016DCType *m = &Mounts->Get_Value(slot);
			Status = 1 >= Mounts->Children[slot]->Unknown70;
			return(m);
		}
		Status = 1;
	}
	return(0);
}

// 0x68AC50
int InvDataPeekClass::Get_Mount_I00(int slot)
{
	UnknownB016DCType *m = Get_Mount(slot);
	if (m) return(m->I00);
	if (Peek_Value(slot, 0, -2) == -2) return(0);
	return(-1);
}

// 0x68AD80
int InvDataPeekClass::Get_Mount_I08(int slot)
{
	UnknownB016DCType *m = Get_Mount(slot);
	if (m) return(m->I08);
	return(-1);
}

// 0x68AE50
int InvDataPeekClass::Get_Mount_I04(int slot)
{
	UnknownB016DCType *m = Get_Mount(slot);
	if (m) return(m->I04);
	return(-1);
}

#pragma auto_inline(off)





// 0x68AF20
DeltaCompoundClass *InvDataPeekClass::Get_Slot(int slot)
{
	Status = 2;
	if (!List) return(0);
	if ((unsigned int)slot >= Count) return(0);
	DeltaClass *ditem = List->Children[slot];
	FAIL_IF(!ditem) return(0);
	DeltaCompoundClass *dcont = ditem->As_Compound();

	FAIL_IF(!dcont) return(0);
	if (dcont->Unknown70) {
		Status = 1 >= dcont->Unknown70;
	}
	return(dcont);
}



// 0x68AFF0
DeltaClass *InvDataPeekClass::Get_Field(int slot, int field)
{
	Status = 2;
	int index = Fields[field];
	if (index == -1) {
		return(0);
	}
	DeltaCompoundClass *dcont = Get_Slot(slot);
	if (!dcont) {
		return(0);
	}
	DeltaClass *ditem = dcont->Children[index];
	FAIL_IF(!ditem) {
		Status = 2;
		return(0);
	}
	if (ditem->Unknown70) {
		Status = 1 >= ditem->Unknown70;
		return(ditem);
	}
	Status = 2;
	return(ditem);
}








// 0x68B0C0
void InvDataPeekClass::Update_Fields()
{
	for (int f = 0; f < 14; f++) Fields[f] = -1;
	if (List) {
		DeltaClass *ditem = List->Children[0];
		FAIL_IF(!ditem) return;
		DeltaCompoundClass *dcont = ditem->As_Compound();

		FAIL_IF(!dcont) return;
		int count = dcont->Children.size();
		for (int i = 0; i < count; i++) {
			ditem = dcont->Children[i];


			FAIL_IF(!ditem) continue;
			const char *field_name = ditem->Name.Get_String();



			FAIL_IF(!field_name) continue;
			for (int j = 0; j < 14; j++) {
				if (strcmp(field_name, ItemFieldNames[j]) == 0) {
					Fields[j] = i;
					break;
				}
			}
		}
	}
}

/*
**	Notes.
**	Object 0x689A20-0x68B35F: opened by permstr.h's PermStringInit (0x689A20), closed by <string>'s initialiser after
**	Update_Fields. The fourteen field getters read the slot's DeltaItemClass / DeltaEquipItemClass (aux_roots.h) by
**	the field's position, found once per Attach by name (Update_Fields); the four string getters are one pattern written
**	four times (their FAIL_IF texts differ: Get_Builder_Name repeats Get_Instance_Info's), not a template.
*/
