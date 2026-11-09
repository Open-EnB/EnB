// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Header of e:\build\beta\G\app\Client\item\client_item_property_icon.cpp (its FAIL_IF's __FILE__,
**	0xB77D50; the header's name is assumed from it, NAME 3).
**
**	An item's icon as a property (12 bytes, table 0xAEDD88): the visitor's slot +0x18 hands it to the
**	item examination title (0x5379D0 -> Set_Icon 0x53A2C0, which passes &IconTexture to the icon window's
**	0x519FE0; that function's 0x51A190 reads it as a TextureClass reference: Request_Foreground_Loading,
**	DynamicMeshClass::Set_Texture).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CLIENT_ITEM_PROPERTY_ICON_H  // NAME 3 [client_item_property_icon.h]: header name assumed from the client's client_item_property_icon.cpp
#define CLIENT_ITEM_PROPERTY_ICON_H

#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\ww3d2\texture.h"
#include "..\ui_item_examination_property_router.h"
#include <string>

class ClientItemPropertyIconClass : public ClientItemPropertyClass  // NAME 3: class-from-file (D24) for client_item_property_icon.cpp
{
public:
	static RefCountPtr<ClientItemPropertyIconClass> Create(const RefCountPtr<TextureClass> &icon_texture);	// 0x529FC0 (FAIL_IF '!icon_texture', line 28)  // NAME 3 [icon_texture]: the FAIL_IF's; NAME 1 [Create]: what it does

	ClientItemPropertyIconClass(const RefCountPtr<TextureClass> &icon_texture);				// 0x52A0B0
	virtual ~ClientItemPropertyIconClass();													// 0x52A120 (deleting 0x52A0F0)
	virtual int Unknown_08();																// +0x08 0x52A1B0 (0)  // NAME 1: neutral (D10)
	virtual std::string Unknown_0C();														// +0x0C 0x52A1D0 ('category tooltip')  // NAME 1: neutral (D10)
	virtual void Accept(ClientItemPropertyVisitorClass *visitor);							// +0x10 0x52A2E0 (the visitor's +0x18)

	RefCountPtr<TextureClass> IconTexture;													// +0x08  // NAME 2 [IconTexture]: the FAIL_IF's icon_texture
};

#endif
