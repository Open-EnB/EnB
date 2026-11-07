// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\nav_list_builder.h (name assumed from nav_list_builder.cpp, the
**	client's __FILE__ 0xB9EE4C). Declared as far as radar_system.cpp uses it; the object 0x7B7BF0 is not rebuilt.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __NAV_LIST_BUILDER_H  // NAME 3 [nav_list_builder.h]: header name assumed from the client's nav_list_builder.cpp
#define __NAV_LIST_BUILDER_H

#include "..\common\unknown_navlist.h"

class SClient;
class ClientObjectClass;

// nav_list_builder.cpp's list (0x20 bytes), not rebuilt: a navigation list that the builder fills
class NavListBuilderClass : public NavigationListClass	// NAME 3: class-from-file (D24) for nav_list_builder.cpp
{
public:
	NavListBuilderClass(SClient *client);					// 0x7B7BF0  // NAME 2 [client]: what it stores
	virtual ~NavListBuilderClass();							// +0x00 0x7B7C30 (0x7B7C60)
	virtual void Clear();									// +0x04 0x7B81E0
	bool Unknown_7B7C80(ClientObjectClass *from, ClientObjectClass *to);	// 0x7B7C80  // NAME 1 [from, to]: FAIL_IF '!from'
	bool Unknown_7B7EF0(float range, ClientObjectClass *from, ClientObjectClass *to);	// 0x7B7EF0  // NAME 1 [range, from, to]: FAIL_IF '!co_from'

	SClient *Client;										// +0x18  // NAME 2: what the constructor stores
	int Unknown1C;											// +0x1C (0x7B81E0 counts in it)
};

#endif //__NAV_LIST_BUILDER_H
