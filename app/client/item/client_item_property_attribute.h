// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\item/client_item_property_attribute.h (its FAIL_IFs'
**	__FILE__, 0xB7883C).
**
**	Partial (interim, D137): only what ui_item_examination_attributes.cpp needs. An attribute property of an
**	item: reference counted, sorted by the int at +0x08 (operator< 0x5343A0, the FAIL_IFs at lines 75
**	and 76). The rest of the header (13 functions and 18 asserts in the client) is not yet rebuilt.
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












































class ClientItemPropertyAttributeClass : public ClientItemPropertyClass  // NAME 2 [ClientItemPropertyAttributeClass]: class from file (D24), client_item_property_attribute.h; its base the router.h one (client_item_property_builder.cpp converts its RefCountPtrs to it for Route)
{
public:
	int Unknown08;																			// +0x08 (operator< compares it)  // NAME 1 [Unknown08]: neutral (D10)
};

inline bool operator<(const RefCountPtr<ClientItemPropertyAttributeClass> &lhs, const RefCountPtr<ClientItemPropertyAttributeClass> &rhs)	// 0x5343A0
{
	FAIL_IF(!lhs) return false;
	FAIL_IF(!rhs) return true;
	return lhs->Unknown08 < rhs->Unknown08;
}

#endif
