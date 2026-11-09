// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\inv_data_peek.h (the client's __FILE__ string 0xB90004: G_ASSERT 'false'
**	on line 195 in Peek_Value, inlined in 0x689CA0, 0x689D20, 0x689F10, 0x689F90, 0x68A010, 0x68A7C0, 0x68A840, 0x68AAA0
**	and 0x68AC50; G_ASSERT 'find' on line 209 in Peek_Object, out of line at 0x689E20)
**
**	A read-only view of an inventory list in the aux tree (a DeltaItemClass / DeltaEquipItemClass per slot): slot N's
**	fourteen fields by the client's field names (table 0xB8FEB4), and with the mounts flag the parallel Inventory.Mounts.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __INV_DATA_PEEK_H
#define __INV_DATA_PEEK_H

#include <string>
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\auxdata\delta_string.h"
#include "..\shared\auxdata\delta_array.h"
#include "..\shared\duration.h"
#include "..\shared\money.h"
#include "..\shared\g_assert.h"
#include "aux_roots.h"

class ClientClass;
struct InvDataPeekItemStruct;

class InvDataPeekClass  // NAME 3: by convention (D24) from the client's file inv_data_peek.cpp
{
public:
	InvDataPeekClass(ClientClass *client, const char *name, bool mounts);	// 0x689A80  // NAME 2 [client, name, mounts]: what it does with them
	~InvDataPeekClass();														// 0x689AD0
	void Attach(DeltaCompoundClass *root);								// 0x689AF0  // NAME 2: describes what it does

	bool Changed();															// 0x689C30  // NAME 2: what it returns
	bool Slot_Changed(int slot);											// 0x689C60  // NAME 2: what it returns

	int Get_Item_Template_ID(int slot);								// 0x689CA0 (-2: none)  // NAME 2: the client's field name (0xB8FEB4)
	int Get_Stack_Count(int slot);										// 0x689D20  // NAME 2: the client's field name (0xB8FEB4)
	MoneyClass Get_Price(int slot);										// 0x689DA0  // NAME 2: the client's field name (0xB8FEB4)
	float Get_Ave_Cost(int slot);											// 0x689F10  // NAME 2: the client's field name (0xB8FEB4)
	float Get_Structure(int slot);										// 0x689F90  // NAME 2: the client's field name (0xB8FEB4)
	float Get_Quality(int slot);											// 0x68A010  // NAME 2: the client's field name (0xB8FEB4)
	std::string Get_Instance_Info(int slot);							// 0x68A090  // NAME 2: the client's field name (0xB8FEB4)
	std::string Get_Instance_Activated_Effect_Info(int slot);		// 0x68A270  // NAME 2: the client's field name (0xB8FEB4)
	std::string Get_Instance_Equip_Effect_Info(int slot);			// 0x68A450  // NAME 2: the client's field name (0xB8FEB4)
	float Get_Effect_Range(int slot);									// 0x68A630  // NAME 2: the client's field names (Effects.Range)
	EffectGeneratorUsageBF Get_Effect_Usage(int slot);				// 0x68A680  // NAME 2: the client's field names (Effects.Usage)
	TargetGeneratorSummaryBF Get_Effect_Targets(int slot);			// 0x68A6B0  // NAME 2: the client's field names (Effects.Targets)
	GameActionValidityBitFlag Get_Effect_Validity(int slot);		// 0x68A6E0  // NAME 2: the client's field names (Effects.Validity)
	DurationClass Get_Ready_Time(int slot);								// 0x68A720  // NAME 2: the client's field name (0xB8FEB4)
	float Get_Target_Range(int slot);									// 0x68A7C0  // NAME 2: the client's field name (0xB8FEB4)
	ItemStateEnum Get_Item_State(int slot);								// 0x68A840  // NAME 2: the client's field name (0xB8FEB4)
	std::string Get_Builder_Name(int slot);								// 0x68A8C0  // NAME 2: the client's field name (0xB8FEB4)
	InvDataPeekItemStruct *Get_Item(int slot, bool flag);			// 0x68AAA0 (the item template, 0x8318B0's)  // NAME 2: what it returns

	UnknownB016DCType *Get_Mount(int slot);								// 0x68AB80  // NAME 2: the field Inventory.Mounts
	int Get_Mount_I00(int slot);											// 0x68AC50  // NAME 1: neutral (D10), as UnknownB016DCType's member
	int Get_Mount_I08(int slot);											// 0x68AD80  // NAME 1: neutral (D10), as UnknownB016DCType's member
	int Get_Mount_I04(int slot);											// 0x68AE50  // NAME 1: neutral (D10), as UnknownB016DCType's member

protected:
	DeltaCompoundClass *Get_Slot(int slot);								// 0x68AF20  // NAME 2: describes what it does
	DeltaClass *Get_Field(int slot, int field);						// 0x68AFF0  // NAME 2: describes what it does
	void Update_Fields();													// 0x68B0C0  // NAME 2: describes what it does






















































































































	// VC6 can't take a member template's explicit argument: the default names T
	template<class T> T Peek_Value(int slot, int field, T def)  // NAME 2: describes what it does
	{
		DeltaClass *d = Get_Field(slot, field);
		if (d) {
			if (d->Unknown04 == DeltaTemplateClass<T>::UnknownGlobal) {
				return(((DeltaTemplateClass<T> *)d)->Value);
			}
			G_ASSERT(false);
		}
		Status = 2;
		return(def);
	}
	// the money field's (DeltaTemplateClass<MoneyClass>: its value at +0x88), out of line
	template<class T> T Peek_Object(int slot, int field, T def)  // NAME 2: describes what it does
	{
		DeltaClass *d = Get_Field(slot, field);
		if (d) {
			if (d->Unknown04 == DeltaTemplateClass<T>::UnknownGlobal) {
				return(((DeltaTemplateClass<T> *)d)->Value);
			}
			const bool find = false;  // NAME 1: neutral (D10): the assert's text
			G_ASSERT(find);
		}
		Status = 2;
		return(def);
	}

public:	// ui_mfd_extended_context_harvestable.cpp reads Count
	ClientClass *Owner;									// +0x00 the constructor's argument  // NAME 2: describes what it does
	NameClass Name;										// +0x04 the list's name  // NAME 2: describes what it does
	DeltaCompoundClass *List;							// +0x38 what Name finds under Attach's root  // NAME 2: describes what it does
	bool HasMounts;										// +0x3C Attach looks up Inventory.Mounts  // NAME 2: describes what it does
	DeltaArrayClass<UnknownB016DCType> *Mounts;	// +0x40 'Inventory.Mounts'  // NAME 2: the field's name
	unsigned int Count;									// +0x44 List's children  // NAME 2: describes what it does
	int Fields[14];										// +0x48 each field's position in a slot, -1 if it has none  // NAME 2: describes what it does
	int Status;												// +0x80 2 after a failed read, else the field's Unknown70 < 2  // NAME 2: describes what it does
};

#endif //__INV_DATA_PEEK_H
