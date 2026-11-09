// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Header of the client's 0x52AD00-0x52B38F (no assert, so no __FILE__: file and header names provisional,
**	NAME 1, D137, after the client's client_item_property_attribute_* / _special_* files).
**
**	A weapon slot's 'Auto Fire' option (0x3C bytes, table 0xAEDE64): reads the slot's flag 0x2000 and sends
**	ItemStatePacketClass to change it.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CLIENT_ITEM_PROPERTY_OPTION_AUTO_FIRE_H  // NAME 1 [client_item_property_option_auto_fire.h]: provisional (D137)
#define CLIENT_ITEM_PROPERTY_OPTION_AUTO_FIRE_H

#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "client_item_property_option.h"

class SClient;
class InvDataClass;

class ClientItemPropertyOptionAutoFireClass : public ClientItemPropertyOptionClass  // NAME 2: its label 'Auto Fire'
{
public:
	static RefCountPtr<ClientItemPropertyOptionAutoFireClass> Create(SClient *space_client, const RefCountPtr<InvDataClass> &data, int slot);	// 0x52AD00  // NAME 1 [Create]: as the icon's; NAME 1 [space_client, data, slot]: the builder's

	ClientItemPropertyOptionAutoFireClass(SClient *space_client, const RefCountPtr<InvDataClass> &data, int slot);	// 0x52AF10
	virtual ~ClientItemPropertyOptionAutoFireClass();										// 0x52B100 (deleting 0x52B0D0)
	virtual bool Unknown_14();																// +0x14 0x52B120 (the slot's flag 0x2000)
	virtual void Unknown_18(bool value);													// +0x18 0x52B160 (ItemStatePacketClass: inventory 2, the slot, state 0x2000)

	SClient *SpaceClient;																	// +0x30  // NAME 1: neutral
	const RefCountPtr<InvDataClass> &Data;													// +0x34 (a reference, as the builder's)  // NAME 1: neutral
	int Slot;																				// +0x38  // NAME 1: neutral
};

#endif
