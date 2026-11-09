// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Header of client_item_property_attribute_distance.cpp (__FILE__ 0xB77080); its own name is assumed from it
**	(NAME 3, D137).
**
**	A distance as an attribute property (0x4C bytes, table 0xAEDC24): its value is shown in thousands ('%1.1fK').
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CLIENT_ITEM_PROPERTY_ATTRIBUTE_DISTANCE_H  // NAME 3 [client_item_property_attribute_distance.h]: assumed from the client's client_item_property_attribute_distance.cpp
#define CLIENT_ITEM_PROPERTY_ATTRIBUTE_DISTANCE_H

#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "client_item_property_attribute.h"
#include <string>

class ClientItemPropertyAttributeDistanceClass : public ClientItemPropertyAttributeClass  // NAME 3: class-from-file (D24) for client_item_property_attribute_distance.cpp
{
public:
	static RefCountPtr<ClientItemPropertyAttributeDistanceClass> Create(int sort, const std::string &label, const std::string &tooltip, int unknown, int base, int actual, int level);	// 0x51FEA0 (FAIL_IFs 'label.empty()', 'tooltip.empty()', lines 30 and 31)  // NAME 1 [Create]: as the icon's; NAME 3 [label, tooltip]: the FAIL_IFs'; NAME 1 [sort, unknown, base, actual, level]: the builder's

	ClientItemPropertyAttributeDistanceClass(int sort, const std::string &label, const std::string &tooltip, int unknown, int base, int actual, int level);	// 0x520080
	virtual ~ClientItemPropertyAttributeDistanceClass();									// 0x5201F0 (deleting 0x5201C0)
	virtual std::string Unknown_14();														// +0x14 0x520210 ('%1.1fK' of Actual / 1000)  // NAME 1: neutral (D10)

	int Base;																				// +0x44  // NAME 1: the builder's base
	int Actual;																				// +0x48 (the one shown)  // NAME 1: the builder's actual
};

#endif
