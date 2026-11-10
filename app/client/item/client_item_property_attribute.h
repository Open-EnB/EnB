// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\item/client_item_property_attribute.h (its FAIL_IFs'
**	__FILE__, 0xB7883C).
**
**	Partial (interim, D137): the class (its own code in client_item_property_attribute.cpp, 0x51F9C0-0x51FE9F)
**	and operator<. An attribute property of an item: reference counted, sorted by the int at +0x08
**	(operator< 0x5343A0, the FAIL_IFs at lines 75 and 76). The rest of the header's inline code (13
**	functions and 18 asserts in the client) is not yet rebuilt.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CLIENT_ITEM_PROPERTY_ATTRIBUTE_H  // NAME 3 [client_item_property_attribute.h]: the client's __FILE__ 0xB7883C
#define CLIENT_ITEM_PROPERTY_ATTRIBUTE_H

#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\shared\g_assert.h"
#include "..\ui_item_examination_property_router.h"





























#include <string>

class ClientItemPropertyAttributeClass : public ClientItemPropertyClass  // NAME 2 [ClientItemPropertyAttributeClass]: class from file (D24), client_item_property_attribute.h; its base the router.h one (client_item_property_builder.cpp converts its RefCountPtrs to it for Route); table 0xAEDC08, 0x44 bytes
{
public:
	ClientItemPropertyAttributeClass(int sort, const std::string &label, const std::string &units, const std::string &tooltip, int unknown, int level);	// 0x51F9C0 (FAIL_IFs 'label.empty()', 'tooltip.empty()' in the subclasses' Create)  // NAME 3 [label, tooltip]: the subclasses' FAIL_IFs; NAME 1 [sort, units, unknown, level]: as client_item_property_builder.h names its subclasses' Create
	virtual ~ClientItemPropertyAttributeClass();											// 0x51FBD0 (deleting 0x51FBA0)
	virtual int Unknown_08();																// +0x08 0x51FCB0 (Unknown3C)
	virtual std::string Unknown_0C();														// +0x0C 0x51FCD0 (Tooltip)
	virtual void Accept(ClientItemPropertyVisitorClass *visitor);							// +0x10 0x51FE50 (the visitor's slot +0x08)
	virtual std::string Unknown_14() = 0;													// +0x14 (pure; distance 0x520210 formats its value)  // NAME 1: neutral (D10)

	int Unknown08;																			// +0x08 (operator< compares it)  // NAME 1 [Unknown08]: neutral (D10)
	std::string Label;																		// +0x0C  // NAME 3: the subclasses' FAIL_IF 'label.empty()'
	std::string Unknown1C;																	// +0x1C (distance passes "units")  // NAME 1: neutral (D10)
	std::string Tooltip;																	// +0x2C  // NAME 3: the subclasses' FAIL_IF 'tooltip.empty()'
	int Unknown3C;																			// +0x3C  // NAME 1: neutral (D10)
	int Unknown40;																			// +0x40  // NAME 1: neutral (D10)
};

inline bool operator<(const RefCountPtr<ClientItemPropertyAttributeClass> &lhs, const RefCountPtr<ClientItemPropertyAttributeClass> &rhs)	// 0x5343A0
{
	FAIL_IF(!lhs) return false;
	FAIL_IF(!rhs) return true;
	int lhs_sort = lhs->Unknown08;
	int rhs_sort = rhs->Unknown08;
	return lhs_sort < rhs_sort;
}

#endif
