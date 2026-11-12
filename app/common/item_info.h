// Earth & Beyond client (reconstruction): app\common\item_info.h
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Common\item_info.h (name assumed from item_info.cpp,
**	whose asserts name the .cpp; no assert names the header)
**
**	An item's info: a sparse list of typed fields (an int, a float or a string each, keyed by an
**	ItemInfoEnum value) and the item's two effect descriptions. ITEM_BASE (opcode 37, pkitemba.h)
**	carries one per item template; an item instance's delta string is applied to a copy of it
**	(client_item_property_builder.cpp). item_info.cpp is client 0x83E7C0-0x842930.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef ITEM_INFO_H  // NAME 1 [item_info.h]: header name assumed from the client's item_info.cpp
#define ITEM_INFO_H

#include "refcount.h"
#include "item_effect_description.h"
#include <string>
#include <vector>

class RawPacketClass;

// Which field of an item's info (the client's asserts 'item_info <= ITEM_INFO_NONE' and
// 'item_info >= NUM_ITEM_INFO'; the values between them have no name in the client: the
// functions take them as int)
enum ItemInfoEnum  // NAME 2: the asserts' ITEM_INFO_NONE / NUM_ITEM_INFO and the parameter 'iteminfo'
{
	ITEM_INFO_NONE = -1,
	NUM_ITEM_INFO = 38
};

// One field: which one, and its value by the field's type (_ItemField's table in item_info.cpp)
struct ItemFieldStruct  // NAME 2: the asserts' 'field->ItemInfo' and '_ItemField'
{
	int ItemInfo;																						// +0x00 (the client's 'field->ItemInfo')
	union {
		int Int;																						// +0x04  // NAME 2: its type
		float Float;																					// +0x04  // NAME 2: its type
		char *String;																					// +0x04 (new[]: the info owns it)  // NAME 2: its type
	};
};

// What ITEM_BASE's Info and an item's template info point at: 0x90 bytes, vtable 0xB08C34 over RefCountClass
// (Delete_This, the destructor 0x83F550).
class ItemBaseInfoClass : public RefCountClass  // NAME 2: what it is (ITEM_BASE's FAIL_IF '!Info'; D219 merged ItemTemplateInfoClass into it)
{
public:
	ItemBaseInfoClass();																				// 0x83F490
	ItemBaseInfoClass(const ItemBaseInfoClass &that);													// 0x83F710
	virtual ~ItemBaseInfoClass();																		// 0x83F580 (0x83F550)
	ItemBaseInfoClass &operator=(const ItemBaseInfoClass &that);										// 0x83F7E0

	void Reset();																						// 0x83F8A0 (no fields, no effects)  // NAME 2: what it does
	void Reset_Fields();																				// 0x83F940 (frees the strings)  // NAME 2: what it does
	void Reset_Effects();																				// 0x83F9E0  // NAME 2: what it does
	void Merge(const ItemBaseInfoClass &that);															// 0x83FA10 (sets that's non-empty fields, copies its effects)  // NAME 2: what it does
	bool Has_Field(int iteminfo);																		// 0x83FD60  // NAME 2: what it returns
	int Get_Enum_Mapping(int iteminfo);																	// 0x83FDE0 (the field's index, 0xFF: none)  // NAME 2: what it returns
	unsigned int Get_Field_Count();																		// 0x83FE00  // NAME 2: what it returns
	int Get_Size();																						// 0x83FE30 (its bytes in a message)  // NAME 2: what it returns
	void Write(RawPacketClass *raw);																	// 0x83FEF0  // NAME 2: what it does
	void Read(RawPacketClass *raw);																		// 0x840030  // NAME 2: what it does
	void Set_Int(int iteminfo, int val, bool force);													// 0x840420 (a zero only when forced)  // NAME 2: what it does; NAME 1 [force]
	void Set_Float(int iteminfo, float val, bool force);												// 0x840540  // NAME 2: what it does; NAME 1 [force]
	void Set_String(int iteminfo, const char *val);														// 0x840670  // NAME 2: what it does
	void Set_String(int iteminfo, const std::string &val);												// 0x8407D0  // NAME 2: what it does
	int Get_Field_Info(unsigned int index);																// 0x840940 (the ItemInfoEnum of the field at index, ITEM_INFO_NONE: none)  // NAME 2: what it returns; NAME 1 [index]
	bool Get_String(std::string &val, unsigned int index);												// 0x840980 (the field at index as text)  // NAME 2: what it does
	bool Get_String(int iteminfo, std::string &val);													// 0x8409D0 (any field as text)  // NAME 2: what it does
	bool Get_Int(int iteminfo, int *val);																// 0x840A20  // NAME 2: what it does
	bool Get_Float(int iteminfo, float *val);															// 0x840AC0  // NAME 2: what it does
	void Set_Activated_Effects(const ItemEffectDescriptionClass &effects);								// 0x840B60  // NAME 2: what it does; NAME 1 [effects]
	void Set_Equip_Effects(const ItemEffectDescriptionClass &effects);									// 0x840DB0  // NAME 2: what it does
	ItemFieldStruct *Add_Field(int iteminfo);															// 0x841000 (the field, added empty when missing)  // NAME 2: what it does
	ItemFieldStruct *Find_Field(int iteminfo);															// 0x8410D0 (NULL: none)  // NAME 2: what it does
	bool Field_To_String(const ItemFieldStruct *field, std::string &val);								// 0x841100 (does not use the info: its callers leave ecx as it is)  // NAME 2: what it does
	std::string Get_Delta();																			// 0x841410 ('id:value' per field, the text Apply_Delta reads)  // NAME 2: what it returns
	void Apply_Delta(const std::string &delta);															// 0x8417B0 (applies an encoded delta)  // NAME 2: what it does; NAME 1 [delta]

	const ItemEffectDescriptionClass &Get_Activated_Effects() const { return(ActivatedEffects); }	// NAME 2: its member; an inline layer the client spends budget on
	const ItemEffectDescriptionClass &Get_Equip_Effects() const { return(EquipEffects); }			// NAME 2: its member; an inline layer the client spends budget on

	std::vector<ItemFieldStruct> Fields;																// +0x08 (the client's 'Fields.size()')
	unsigned char EnumMapping[NUM_ITEM_INFO];															// +0x18 (the client's 'EnumMapping[iteminfo]': the field's index in Fields, 0xFF: none)
	ItemEffectDescriptionClass ActivatedEffects;														// +0x40  // NAME 2: the instance's Get_Instance_Activated_Effect_Info delta is applied to it
	ItemEffectDescriptionClass EquipEffects;															// +0x68  // NAME 2: the instance's Get_Instance_Equip_Effect_Info delta is applied to it
};

#endif
