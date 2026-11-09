// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Header of client_item_property_attribute_integer.cpp (__FILE__ 0xB77154); its own name is assumed from it
**	(NAME 3, D137).
**
**	A integer value as an attribute property (0x4C bytes, table 0xAEDC60): its value is shown as '%d'.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CLIENT_ITEM_PROPERTY_ATTRIBUTE_INTEGER_H  // NAME 3 [client_item_property_attribute_integer.h]: assumed from the client's client_item_property_attribute_integer.cpp
#define CLIENT_ITEM_PROPERTY_ATTRIBUTE_INTEGER_H

#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "client_item_property_attribute.h"
#include <string>

class ClientItemPropertyAttributeIntegerClass : public ClientItemPropertyAttributeClass  // NAME 3: class-from-file (D24) for client_item_property_attribute_integer.cpp
{
public:
	static RefCountPtr<ClientItemPropertyAttributeIntegerClass> Create(int sort, const std::string &label, const std::string &units, const std::string &tooltip, int unknown, int base, int actual, int level);	// 0x520790 (FAIL_IFs 'label.empty()', 'tooltip.empty()', lines 31 and 32)  // NAME 1 [Create]: as the icon's; NAME 3 [label, tooltip]: the FAIL_IFs'; NAME 1 [sort, units, unknown, base, actual, level]: the builder's

	ClientItemPropertyAttributeIntegerClass(int sort, const std::string &label, const std::string &units, const std::string &tooltip, int unknown, int base, int actual, int level);	// 0x5208C0
	virtual ~ClientItemPropertyAttributeIntegerClass();	// 0x520960 (deleting 0x520930)
	virtual std::string Unknown_14();	// +0x14 0x520980 ('%d')  // NAME 1: neutral (D10)

	int Base;	// +0x44  // NAME 1: the builder's base
	int Actual;	// +0x48 (the one shown)  // NAME 1: the builder's actual
};

#endif
