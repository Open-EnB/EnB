// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Header of client_item_property_attribute_float.cpp (__FILE__ 0xB770F4); its own name is assumed from it
**	(NAME 3, D137).
**
**	A float value as an attribute property (0x4C bytes, table 0xAEDC44): its value is shown as '%0.1f'.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CLIENT_ITEM_PROPERTY_ATTRIBUTE_FLOAT_H  // NAME 3 [client_item_property_attribute_float.h]: assumed from the client's client_item_property_attribute_float.cpp
#define CLIENT_ITEM_PROPERTY_ATTRIBUTE_FLOAT_H

#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "client_item_property_attribute.h"
#include <string>

class ClientItemPropertyAttributeFloatClass : public ClientItemPropertyAttributeClass  // NAME 3: class-from-file (D24) for client_item_property_attribute_float.cpp
{
public:
	static RefCountPtr<ClientItemPropertyAttributeFloatClass> Create(int sort, const std::string &label, const std::string &units, const std::string &tooltip, int unknown, float base, float actual, int level);	// 0x5203D0 (FAIL_IFs 'label.empty()', 'units.empty()', 'tooltip.empty()', lines 30, 31 and 32)  // NAME 1 [Create]: as the icon's; NAME 3 [label, tooltip]: the FAIL_IFs'; NAME 1 [sort, units, unknown, base, actual, level]: the builder's

	ClientItemPropertyAttributeFloatClass(int sort, const std::string &label, const std::string &units, const std::string &tooltip, int unknown, float base, float actual, int level);	// 0x520530
	virtual ~ClientItemPropertyAttributeFloatClass();	// 0x5205D0 (deleting 0x5205A0)
	virtual std::string Unknown_14();	// +0x14 0x5205F0 ('%0.1f')  // NAME 1: neutral (D10)

	float Base;	// +0x44  // NAME 1: the builder's base
	float Actual;	// +0x48 (the one shown)  // NAME 1: the builder's actual
};

#endif
