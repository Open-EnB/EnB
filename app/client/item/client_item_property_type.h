// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown; the name is the family's (client_item_property_special_maker.cpp and its
**	siblings are the client's own __FILE__s; this class has no assert).
**
**	An item's level, damage type and type as a property: the item examination title's subtitles ('Level %d ', the
**	damage type's name, the type's name).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CLIENT_ITEM_PROPERTY_TYPE_H  // NAME 2 [client_item_property_type.h]: as its siblings, which the client names; 'type tooltip' is its key
#define CLIENT_ITEM_PROPERTY_TYPE_H

#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\ui_item_examination_property_router.h"
#include <string>

class ClientItemPropertyTypeClass : public ClientItemPropertyClass  // NAME 2: class-from-file (D24) for client_item_property_type.cpp, itself a NAME 2; table 0xAEE14C, 0x14 bytes
{
public:
	enum { DAMAGE_TYPE_NONE = 9 };															// (0x530310; Unknown_49E210 names the others)  // NAME 1 [DAMAGE_TYPE_NONE]: what Has_Damage_Type tests

	static RefCountPtr<ClientItemPropertyTypeClass> Create(int level, int damage_type, int type);	// 0x52FFB0 (the item has a damage type, attribute 6)  // NAME 1 [Create]: as the name's; NAME 1 [level, damage_type, type]: the members they fill
	static RefCountPtr<ClientItemPropertyTypeClass> Create(int level, int type);			// 0x530070 (DAMAGE_TYPE_NONE)

	ClientItemPropertyTypeClass(int level, int damage_type, int type);						// 0x530130 (inlined into both Creates)  // NAME 1 [level, damage_type, type]: the members they fill
	virtual ~ClientItemPropertyTypeClass();													// 0x5301A0 (deleting 0x530170)
	virtual int Unknown_08();																// +0x08 0x5301C0 (0)  // NAME 1: neutral (D10)
	virtual std::string Unknown_0C();														// +0x0C 0x5301E0 ('type tooltip')  // NAME 1: neutral (D10)
	virtual void Accept(ClientItemPropertyVisitorClass *visitor);							// +0x10 0x5302F0 (the visitor's +0x10)

	bool Has_Damage_Type() const;															// 0x530310 (DamageType != DAMAGE_TYPE_NONE; the title's Set_Subtitles asks)  // NAME 1 [Has_Damage_Type]: what it tests
	int Get_Level() const { return(Level); }												// (inline: Set_Subtitles reads through it)  // NAME 1: the member it returns
	int Get_Damage_Type() const { return(DamageType); }										// (inline)  // NAME 1: the member it returns
	int Get_Type() const { return(Type); }													// (inline)  // NAME 1: the member it returns

	int Level;																				// +0x08 ('Level %d '; the item's +0x48)  // NAME 1: what it shows
	int DamageType;																			// +0x0C (Unknown_49E210's name; DAMAGE_TYPE_NONE none)  // NAME 1: what it shows ('SUBTITLE_DAMAGE_TYPE_IEI00')
	int Type;																				// +0x10 (Unknown_7EB4D0's name; the item's +0x08)  // NAME 1: what it shows ('SUBTITLE_TYPE_IEI00')
};

#endif
