// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\item/client_item_property_special.h (its FAIL_IFs'
**	__FILE__, 0xB78B68).
**
**	Partial (interim, D137): only what ui_item_examination_specials.cpp needs. A special property of an
**	item: reference counted, sorted by the int at +0x08 (its operator< is inlined into the sort 0x539790, the FAIL_IFs at lines 84
**	and 85). The rest of the header is not yet rebuilt.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CLIENT_ITEM_PROPERTY_SPECIAL_H  // NAME 3 [client_item_property_special.h]: the client's __FILE__ 0xB78B68
#define CLIENT_ITEM_PROPERTY_SPECIAL_H

#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\shared\unknown_debug.h"






















































class ClientItemPropertySpecialClass : public RefCountClass  // NAME 2 [ClientItemPropertySpecialClass]: class from file (D24), client_item_property_special.h
{
public:
	int Unknown08;																			// +0x08 (operator< compares it)  // NAME 1 [Unknown08]: neutral (D10)
};

inline bool operator<(const RefCountPtr<ClientItemPropertySpecialClass> &lhs, const RefCountPtr<ClientItemPropertySpecialClass> &rhs)	// (inline: the sort 0x539790)
{
	FAIL_IF(!lhs) return false;
	FAIL_IF(!rhs) return true;
	return lhs->Unknown08 < rhs->Unknown08;
}

#endif
