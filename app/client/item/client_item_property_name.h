// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Header of the client's 0x52A360-0x52A82F (no assert, so no __FILE__: file and header names provisional,
**	NAME 1, D137).
**
**	An item's name as a property (0x18 bytes, table 0xAEDDA0): the visitor's slot +0x1C hands it to the
**	item examination title (0x537990 -> Set_Name 0x53A0A0, which calls Get_Name's out-of-line copy 0x53A140).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CLIENT_ITEM_PROPERTY_NAME_H  // NAME 1 [client_item_property_name.h]: provisional (D137), after client_item_property_icon.cpp
#define CLIENT_ITEM_PROPERTY_NAME_H

#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\ui_item_examination_property_router.h"
#include <string>

class ClientItemPropertyNameClass : public ClientItemPropertyClass  // NAME 1: provisional (D137), class-from-file for the provisional client_item_property_name.cpp
{
public:
	static RefCountPtr<ClientItemPropertyNameClass> Create(const std::string &name);		// 0x52A360  // NAME 1 [Create, name]: as the icon's

	ClientItemPropertyNameClass(const std::string &name);									// 0x52A430
	virtual ~ClientItemPropertyNameClass();													// 0x52A620 (deleting 0x52A5F0)
	virtual int Unknown_08();																// +0x08 0x52A690  // NAME 1: neutral (D10)
	virtual std::string Get_Tooltip();														// +0x0C 0x52A6B0 ('name tooltip')  // NAME 2: what its callers do with it (ui_item_examination_attributes / _options / _specials.cpp make it the row's tooltip; P-L14-2)
	virtual void Accept(ClientItemPropertyVisitorClass *visitor);							// +0x10 0x52A7C0 (the visitor's +0x1C)

	std::string Get_Name() const { return(Name); }											// 0x53A140 (out of line in ui_item_examination_title.cpp)  // NAME 1: what it returns

	std::string Name;																		// +0x08  // NAME 2 [Name]: what it holds (the item's Name, +0x14)
};

#endif
