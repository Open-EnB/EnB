// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Header of the client's 0x529810-0x529A8F (no assert, so no __FILE__: file and header names provisional,
**	NAME 1, D137).
**
**	An int of an item as a property (12 bytes, table 0xAEDD58): the visitor's slot +0x14 takes it (the item
**	examination router's 0x5379B0 does nothing with it).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CLIENT_ITEM_PROPERTY_UNKNOWN_529810_H  // NAME 1 [client_item_property_unknown_529810.h]: provisional (D137), neutral
#define CLIENT_ITEM_PROPERTY_UNKNOWN_529810_H

#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\ui_item_examination_property_router.h"
#include <string>

class ClientItemPropertyUnknown529810Class : public ClientItemPropertyClass  // NAME 1: neutral (D10)
{
public:
	static RefCountPtr<ClientItemPropertyUnknown529810Class> Create(int id);				// 0x529810  // NAME 1 [Create, id]: as the icon's; neutral

	ClientItemPropertyUnknown529810Class(int id);											// 0x5298B0
	virtual ~ClientItemPropertyUnknown529810Class();										// 0x529910 (deleting 0x5298E0)
	virtual int Unknown_08();																// +0x08 0x529930  // NAME 1: neutral (D10)
	virtual std::string Get_Tooltip();														// +0x0C 0x529950 ('category tooltip')  // NAME 2: what its callers do with it (ui_item_examination_attributes / _options / _specials.cpp make it the row's tooltip; P-L14-2)
	virtual void Accept(ClientItemPropertyVisitorClass *visitor);							// +0x10 0x529A60 (the visitor's +0x14)

	int Unknown08;																			// +0x08 (the item's +0x08)  // NAME 1: neutral
};

#endif
