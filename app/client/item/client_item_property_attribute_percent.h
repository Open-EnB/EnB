// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Header of client_item_property_attribute_percent.cpp (__FILE__ 0xB771B0); its own name is assumed from it
**	(NAME 3, D137).
**
**	A percent value as an attribute property (0x4C bytes, table 0xAEDC7C): its value is shown as '%0.0f%%' of Actual * 100.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CLIENT_ITEM_PROPERTY_ATTRIBUTE_PERCENT_H  // NAME 3 [client_item_property_attribute_percent.h]: assumed from the client's client_item_property_attribute_percent.cpp
#define CLIENT_ITEM_PROPERTY_ATTRIBUTE_PERCENT_H

#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "client_item_property_attribute.h"
#include <string>

class ClientItemPropertyAttributePercentClass : public ClientItemPropertyAttributeClass  // NAME 3: class-from-file (D24) for client_item_property_attribute_percent.cpp
{
public:
	static RefCountPtr<ClientItemPropertyAttributePercentClass> Create(int sort, const std::string &label, const std::string &tooltip, int unknown, float base, float actual, int level);	// 0x520B20 (FAIL_IFs 'label.empty()', 'tooltip.empty()', lines 30 and 31)  // NAME 1 [Create]: as the icon's; NAME 3 [label, tooltip]: the FAIL_IFs'; NAME 1 [sort, unknown, base, actual, level]: the builder's

	ClientItemPropertyAttributePercentClass(int sort, const std::string &label, const std::string &tooltip, int unknown, float base, float actual, int level);	// 0x520D00
	virtual ~ClientItemPropertyAttributePercentClass();	// 0x520E70 (deleting 0x520E40)
	virtual std::string Unknown_14();	// +0x14 0x520E90 ('%0.0f%%' of Actual * 100)  // NAME 1: neutral (D10)

	float Base;	// +0x44  // NAME 1: the builder's base
	float Actual;	// +0x48 (the one shown)  // NAME 1: the builder's actual
};

#endif
