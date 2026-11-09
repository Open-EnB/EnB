// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Header of client_item_property_attribute_seconds.cpp (__FILE__ 0xB77224); its own name is assumed from it
**	(NAME 3, D137).
**
**	A seconds value as an attribute property (0x4C bytes, table 0xAEDC98): its value is shown as '%2.1f'.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CLIENT_ITEM_PROPERTY_ATTRIBUTE_SECONDS_H  // NAME 3 [client_item_property_attribute_seconds.h]: assumed from the client's client_item_property_attribute_seconds.cpp
#define CLIENT_ITEM_PROPERTY_ATTRIBUTE_SECONDS_H

#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "client_item_property_attribute.h"
#include <string>

class ClientItemPropertyAttributeSecondsClass : public ClientItemPropertyAttributeClass  // NAME 3: class-from-file (D24) for client_item_property_attribute_seconds.cpp
{
public:
	static RefCountPtr<ClientItemPropertyAttributeSecondsClass> Create(int sort, const std::string &label, const std::string &tooltip, int unknown, float base, float actual, int level);	// 0x521050 (FAIL_IFs 'label.empty()', 'tooltip.empty()', lines 30 and 31)  // NAME 1 [Create]: as the icon's; NAME 3 [label, tooltip]: the FAIL_IFs'; NAME 1 [sort, unknown, base, actual, level]: the builder's

	ClientItemPropertyAttributeSecondsClass(int sort, const std::string &label, const std::string &tooltip, int unknown, float base, float actual, int level);	// 0x521230
	virtual ~ClientItemPropertyAttributeSecondsClass();	// 0x5213A0 (deleting 0x521370)
	virtual std::string Unknown_14();	// +0x14 0x5213C0 ('%2.1f')  // NAME 1: neutral (D10)

	float Base;	// +0x44  // NAME 1: the builder's base
	float Actual;	// +0x48 (the one shown)  // NAME 1: the builder's actual
};

#endif
