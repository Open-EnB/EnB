// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\item/client_item_property_option.h (its FAIL_IFs'
**	__FILE__, 0xB78A58).
**
**	Partial (interim, D137): only what ui_item_examination_options.cpp needs. An option property of an
**	item: reference counted, sorted by the int at +0x08 (operator< 0x536E20, the FAIL_IFs at lines 70
**	and 71). The rest of the header (17 functions and 18 asserts in the client) is not yet rebuilt.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CLIENT_ITEM_PROPERTY_OPTION_H  // NAME 3 [client_item_property_option.h]: the client's __FILE__ 0xB78A58
#define CLIENT_ITEM_PROPERTY_OPTION_H

#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\shared\g_assert.h"








































class ClientItemPropertyOptionClass : public RefCountClass  // NAME 2 [ClientItemPropertyOptionClass]: class from file (D24), client_item_property_option.h
{
public:
	int Unknown08;																			// +0x08 (operator< compares it)  // NAME 1 [Unknown08]: neutral (D10)
};

inline bool operator<(const RefCountPtr<ClientItemPropertyOptionClass> &lhs, const RefCountPtr<ClientItemPropertyOptionClass> &rhs)	// 0x536E20
{
	FAIL_IF(!lhs) return false;
	FAIL_IF(!rhs) return true;
	return lhs->Unknown08 < rhs->Unknown08;
}

#endif
