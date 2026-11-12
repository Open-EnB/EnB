// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\item/client_item_property_special.h (its FAIL_IFs'
**	__FILE__, 0xB78B68).
**
**	The specials' base class (its own code in client_item_property_special.cpp, 0x52B3E0-0x52B80F) and
**	operator<. A special property of an item (an effect, the maker, a requirement): reference counted, sorted by
**	SortOrder (operator< is inlined into the sort 0x539790, the FAIL_IFs at lines 84 and 85).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CLIENT_ITEM_PROPERTY_SPECIAL_H  // NAME 3 [client_item_property_special.h]: the client's __FILE__ 0xB78B68
#define CLIENT_ITEM_PROPERTY_SPECIAL_H

#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\shared\g_assert.h"
#include "..\ui_item_examination_property_router.h"
#include <string>
#include <vector>

// One line of a special's text (the row 0x538350 makes one text per line; 0x14 bytes)
struct ClientItemPropertySpecialLineStruct  // NAME 1: what it holds
{
	ClientItemPropertySpecialLineStruct(const std::string &text, bool bold) : Text(text), Bold(bold) {}	// (inline in the subclasses' Get_Lines)  // NAME 1 [text, bold]: the members'

	std::string Text;																		// +0x00  // NAME 1: what it holds
	bool Bold;																				// +0x10 (the font gets '_BOLD')  // NAME 1: what it selects
};































class ClientItemPropertySpecialClass : public ClientItemPropertyClass  // NAME 2 [ClientItemPropertySpecialClass]: class from file (D24), client_item_property_special.h; its base the router.h one (client_item_property_builder.cpp converts its RefCountPtrs to it for Route); table 0xAEDEB8, 0x24 bytes
{
public:
	ClientItemPropertySpecialClass(int sort_order, const std::string &tooltip, int unknown_1c, int unknown_20);	// 0x52B3E0  // NAME 1 [sort_order, tooltip]: the members they fill; NAME 1 [unknown_1c, unknown_20]: neutral (D10)
	virtual ~ClientItemPropertySpecialClass();												// 0x52B5E0 (deleting 0x52B5B0)
	virtual int Unknown_08();																// +0x08 0x52B650 (Unknown1C)
	virtual std::string Unknown_0C();														// +0x0C 0x52B670 (Tooltip)
	virtual void Accept(ClientItemPropertyVisitorClass *visitor);							// +0x10 0x52B7F0 (the visitor's slot +0x04)
	virtual std::vector<ClientItemPropertySpecialLineStruct> Get_Lines() = 0;				// +0x14 (pure; the row's lines, a bold label and its text)  // NAME 1 [Get_Lines]: what the row 0x538350 does with them
	virtual bool Is_Usable() = 0;															// +0x18 (pure; false: the font gets '_UNUSABLE', a requirement the player does not meet)  // NAME 1 [Is_Usable]: the font's suffix

	int SortOrder;																			// +0x08 (operator< compares it; 100 for an effect)  // NAME 1 [SortOrder]: what operator< does with it
	std::string Tooltip;																	// +0x0C (the slot +0x0C answers it, as the attributes' Tooltip)  // NAME 1 [Tooltip]: as ClientItemPropertyAttributeClass's, the same slot
	int Unknown1C;																			// +0x1C (the slot +0x08 answers it; 0 for an effect)  // NAME 1: neutral (D10)
	int Unknown20;																			// +0x20 (1 for an effect)  // NAME 1: neutral (D10)
};

inline bool operator<(const RefCountPtr<ClientItemPropertySpecialClass> &lhs, const RefCountPtr<ClientItemPropertySpecialClass> &rhs)	// (inline: the sort 0x539790)
{
	FAIL_IF(!lhs) return false;
	FAIL_IF(!rhs) return true;
	int lhs_sort = lhs->SortOrder;
	int rhs_sort = rhs->SortOrder;
	return lhs_sort < rhs_sort;
}

#endif
