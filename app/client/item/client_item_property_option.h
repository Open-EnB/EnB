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
#include "..\ui_item_examination_property_router.h"
#include <string>



























class ClientItemPropertyOptionClass : public ClientItemPropertyClass  // NAME 2 [ClientItemPropertyOptionClass]: class from file (D24), client_item_property_option.h; its base the router.h one (client_item_property_builder.cpp converts its RefCountPtrs to it for Route)
{
public:
	ClientItemPropertyOptionClass(int sort, const std::string &label, const std::string &tooltip, int unknown);	// 0x52A840 (table 0xAEDDB8)  // NAME 1 [sort, label, tooltip, unknown]: what the auto fire option passes (1, 'Auto Fire', its tooltip, 0)
	virtual ~ClientItemPropertyOptionClass();												// 0x52AA00 (deleting 0x52A9D0)
	virtual int Unknown_08();																// +0x08 0x52AAC0 (Unknown2C)  // NAME 1: neutral (D10)
	virtual std::string Unknown_0C();														// +0x0C 0x52AAE0 (Tooltip)  // NAME 1: neutral (D10)
	virtual void Accept(ClientItemPropertyVisitorClass *visitor);							// +0x10 0x52AC60 (the visitor's +0x00)
	virtual bool Unknown_14() = 0;															// +0x14 (pure; the auto fire option's 0x52B120: the slot's flag 0x2000)  // NAME 1: neutral (D10)
	virtual void Unknown_18(bool value) = 0;												// +0x18 (pure; the auto fire option's 0x52B160 sends it)  // NAME 1: neutral (D10); NAME 1 [value]

	int Unknown08;																			// +0x08 (operator< compares it)  // NAME 1 [Unknown08]: neutral (D10)
	std::string Label;																		// +0x0C  // NAME 1: 'Auto Fire'
	std::string Tooltip;																	// +0x1C  // NAME 1: the auto fire option's tooltip text
	int Unknown2C;																			// +0x2C  // NAME 1: neutral
};

inline bool operator<(const RefCountPtr<ClientItemPropertyOptionClass> &lhs, const RefCountPtr<ClientItemPropertyOptionClass> &rhs)	// 0x536E20
{
	FAIL_IF(!lhs) return false;
	FAIL_IF(!rhs) return true;
	return lhs->Unknown08 < rhs->Unknown08;
}

#endif
