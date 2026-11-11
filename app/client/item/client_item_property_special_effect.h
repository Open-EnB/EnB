// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown; the name is the family's (client_item_property_special_maker.cpp and its
**	siblings are the client's own __FILE__s; this class has no assert).
**
**	An effect of an item as a special property: the bold label 'Effect: ' and the effect's text.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CLIENT_ITEM_PROPERTY_SPECIAL_EFFECT_H  // NAME 2 [client_item_property_special_effect.h]: as its siblings, which the client names; 'Effect: ' is its label
#define CLIENT_ITEM_PROPERTY_SPECIAL_EFFECT_H

#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "client_item_property_special.h"
#include <string>
#include <vector>

class ItemEffectClass;

class ClientItemPropertySpecialEffectClass : public ClientItemPropertySpecialClass  // NAME 2: class-from-file (D24) for client_item_property_special_effect.cpp, itself a NAME 2; table 0xAEDEDC, 0x34 bytes
{
public:
	static RefCountPtr<ClientItemPropertySpecialEffectClass> Create(const ItemEffectClass &effect);	// 0x52B840  // NAME 1 [Create]: as the icon's; NAME 1 [effect]: what the builder passes

	ClientItemPropertySpecialEffectClass(const std::string &text, const std::string &tooltip);	// 0x52B9C0 (sort order 100)  // NAME 1 [text, tooltip]: the members they fill
	virtual ~ClientItemPropertySpecialEffectClass();										// 0x52BBC0 (deleting 0x52BB90)
	virtual std::vector<ClientItemPropertySpecialLineStruct> Get_Lines();					// +0x14 0x52BC30 ('Effect: ' in bold, then Text)
	virtual bool Is_Usable();																// +0x18 0x52BF50 (always)

	std::string Text;																		// +0x24 (the effect's Unknown_833D00; the second line)  // NAME 1 [Text]: what Get_Lines does with it
};

#endif
