// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Header of the client's 0x529AE0-0x529FAF (no assert, so no __FILE__: file and header names provisional,
**	NAME 1, D137).
**
**	An item's description as a property (0x18 bytes, table 0xAEDD70): the visitor's slot +0x0C hands it to
**	the item examination window's description (0x537A10).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CLIENT_ITEM_PROPERTY_DESCRIPTION_H  // NAME 1 [client_item_property_description.h]: provisional (D137), after client_item_property_icon.cpp
#define CLIENT_ITEM_PROPERTY_DESCRIPTION_H

#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\ui_item_examination_property_router.h"
#include <string>

class ClientItemPropertyDescriptionClass : public ClientItemPropertyClass  // NAME 1: provisional (D137), class-from-file for the provisional client_item_property_description.cpp
{
public:
	static RefCountPtr<ClientItemPropertyDescriptionClass> Create(const std::string &description);	// 0x529AE0  // NAME 1 [Create, description]: as the icon's

	ClientItemPropertyDescriptionClass(const std::string &description);						// 0x529BB0
	virtual ~ClientItemPropertyDescriptionClass();											// 0x529DA0 (deleting 0x529D70)
	virtual int Unknown_08();																// +0x08 0x529E10  // NAME 1: neutral (D10)
	virtual std::string Get_Tooltip();														// +0x0C 0x529E30 ('description')  // NAME 2: what its callers do with it (ui_item_examination_attributes / _options / _specials.cpp make it the row's tooltip; P-L14-2)
	virtual void Accept(ClientItemPropertyVisitorClass *visitor);							// +0x10 0x529F40 (the visitor's +0x0C)

	std::string Description;																// +0x08  // NAME 2 [Description]: what it holds (the item's +0x24)
};

#endif
