// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown; the name is the family's (client_item_property_special_maker.cpp and its
**	siblings are the client's own __FILE__s; this class has no assert).
**
**	A flag of an item as a special property: one line of text ('Not Tradable', 'Unique' ...), always usable.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CLIENT_ITEM_PROPERTY_SPECIAL_FLAG_H  // NAME 2 [client_item_property_special_flag.h]: as its siblings, which the client names; the builder's flags use it
#define CLIENT_ITEM_PROPERTY_SPECIAL_FLAG_H

#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "client_item_property_special.h"
#include <string>
#include <vector>

class ClientItemPropertySpecialFlagClass : public ClientItemPropertySpecialClass  // NAME 2: class-from-file (D24) for client_item_property_special_flag.cpp, itself a NAME 2; table 0xAEE128, 0x34 bytes
{
public:
	static RefCountPtr<ClientItemPropertySpecialFlagClass> Create(int sort_order, const std::string &text, const std::string &tooltip, int unknown_1c, int unknown_20);	// 0x52F9E0  // NAME 1 [Create]: as the effect's; NAME 1 [sort_order, text, tooltip, unknown_1c, unknown_20]: the members they fill

	ClientItemPropertySpecialFlagClass(int sort_order, const std::string &text, const std::string &tooltip, int unknown_1c, int unknown_20);	// 0x52FAC0  // NAME 1 [sort_order, text, tooltip, unknown_1c, unknown_20]: the members they fill
	virtual ~ClientItemPropertySpecialFlagClass();											// 0x52FCC0 (deleting 0x52FC90)
	virtual std::vector<ClientItemPropertySpecialLineStruct> Get_Lines();					// +0x14 0x52FD30 (Text, not bold)
	virtual bool Is_Usable();																// +0x18 0x52FF30 (always)

	std::string Text;																		// +0x24 ('Not Tradable' ...)  // NAME 1 [Text]: what Get_Lines does with it
};

#endif
