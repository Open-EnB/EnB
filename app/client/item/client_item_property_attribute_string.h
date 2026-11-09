// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Header of client_item_property_attribute_string.cpp (__FILE__ 0xB77288); its own name is assumed from it
**	(NAME 3, D137).
**
**	A text as an attribute property (0x54 bytes, table 0xAEDCB4): its value is the text itself.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CLIENT_ITEM_PROPERTY_ATTRIBUTE_STRING_H  // NAME 3 [client_item_property_attribute_string.h]: assumed from the client's client_item_property_attribute_string.cpp
#define CLIENT_ITEM_PROPERTY_ATTRIBUTE_STRING_H

#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "client_item_property_attribute.h"
#include <string>

class ClientItemPropertyAttributeStringClass : public ClientItemPropertyAttributeClass  // NAME 3: class-from-file (D24) for client_item_property_attribute_string.cpp
{
public:
	static RefCountPtr<ClientItemPropertyAttributeStringClass> Create(int sort, const std::string &label, const std::string &units, const std::string &tooltip, int unknown, const std::string &value);	// 0x521560 (FAIL_IFs 'label.empty()', 'tooltip.empty()', lines 29 and 30)  // NAME 1 [Create]: as the icon's; NAME 3 [label, tooltip]: the FAIL_IFs'; NAME 1 [sort, units, unknown, value]: the builder's

	ClientItemPropertyAttributeStringClass(int sort, const std::string &label, const std::string &units, const std::string &tooltip, int unknown, const std::string &value);	// 0x5216D0 (the base's level 1)
	virtual ~ClientItemPropertyAttributeStringClass();	// 0x5218E0 (deleting 0x5218B0)
	virtual std::string Unknown_14();	// +0x14 0x521950 (Value)  // NAME 1: neutral (D10)

	std::string Value;	// +0x44  // NAME 1: the builder's value
};

#endif
