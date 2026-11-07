// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\nav_list_builder.h (name assumed from nav_list_builder.cpp, the
**	client's __FILE__ 0xB9EE4C). The search's node type is the .cpp's own.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __NAV_LIST_BUILDER_H  // NAME 3 [nav_list_builder.h]: header name assumed from the client's nav_list_builder.cpp
#define __NAV_LIST_BUILDER_H

#include "..\common\unknown_navlist.h"

class SClient;
class ClientObjectClass;
class NavNodeStruct;

// nav_list_builder.cpp's list (0x20 bytes): a navigation list that the builder fills
class NavListBuilderClass : public NavigationListClass	// NAME 3: class-from-file (D24) for nav_list_builder.cpp
{
public:
	NavListBuilderClass(SClient *client);					// 0x7B7BF0  // NAME 2 [client]: what it stores
	virtual ~NavListBuilderClass();							// +0x00 0x7B7C30 (0x7B7C60)
	virtual void Clear();									// +0x04 0x7B81E0
	bool Unknown_7B7C80(ClientObjectClass *from, ClientObjectClass *to);	// 0x7B7C80  // NAME 1 [from, to]: FAIL_IF '!from'
	bool Unknown_7B7EF0(float range, ClientObjectClass *co_from, ClientObjectClass *to);	// 0x7B7EF0: the path by the search  // NAME 1 [range, to]: neutral; co_from: the FAIL_IF
	void Build_Nodes(std::vector<NavNodeStruct *> &nav_points, ClientObjectClass *from, float range, ClientObjectClass *dest);	// 0x7B8210  // NAME 2 [Build_Nodes, range]: what it does; the rest the FAIL_IFs'
	void Link_Nodes(std::vector<NavNodeStruct *> &nav_points, float range1, float range2);	// 0x7B8510 (ecx unused)  // NAME 2 [Link_Nodes, range1, range2]: what it does
	bool Find_Path(std::vector<NavNodeStruct *> &nav_points, std::vector<NavNodeStruct *> &result);	// 0x7B8880 (ecx unused)  // NAME 2 [Find_Path]: what it does
	void Add_Path(std::vector<NavNodeStruct *> &path);	// 0x7B8BF0 (inlined in 0x7B7EF0; no caller)  // NAME 2 [Add_Path, path]: what it does
	void Delete_Nodes(std::vector<NavNodeStruct *> &nav_points);	// 0x7B8E40 (inlined in 0x7B7EF0; no caller)  // NAME 2 [Delete_Nodes]: what it does

	SClient *Client;										// +0x18  // NAME 2: what the constructor stores
	int Unknown1C;											// +0x1C (0x7B81E0 counts in it)
};

#endif //__NAV_LIST_BUILDER_H
