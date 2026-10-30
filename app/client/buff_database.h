/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\buff_database.h (name assumed from
**	buff_database.cpp, whose __FILE__ is 'e:\build\beta\G\app\Client\buff_database.cpp')
**
**	The buff definitions read from buffdef.ini (sections BUFF-0, BUFF-1, ...) into an SGI hash_map
**	keyed by the buff type (libs/sgistl, D87). The client names neither the classes nor the members
**	(D10, D24); the ini keys name the fields.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __BUFF_DATABASE_H  // NAME 3 [buff_database.h]: header name assumed from the client's buff_database.cpp
#define __BUFF_DATABASE_H

#include <string>
#include <hash_map.h>

struct BuffDefStruct  // NAME 2: what it holds (one BUFF-n section, 0x14 bytes)
{
	BuffDefStruct(const char *type, const char *asset, const char *tool_tip, const char *alt_tool_tip, bool is_good);	// 0x676CD0
	~BuffDefStruct();																		// 0x676D40

	char *Type;																				// +0x00, BuffType  // NAME 2: the ini key
	char *Asset;																			// +0x04, BuffAsset  // NAME 2: the ini key
	char *ToolTip;																			// +0x08, BuffToolTip  // NAME 2: the ini key
	char *AltToolTip;																		// +0x0C, BuffAltToolTip  // NAME 2: the ini key
	bool IsGoodBuff;																		// +0x10, IsGoodBuff  // NAME 2: the ini key
};

class BuffDatabaseClass  // NAME 3: by convention (D24) from the client's file buff_database.cpp ('Buff Database' in its messages)
{
public:
	BuffDatabaseClass();																	// 0x676DB0: loads buffdef.ini
	~BuffDatabaseClass();																// 0x677410

	static void Set_Instance(BuffDatabaseClass *database);					// 0x677640 (0xBE6014)  // NAME 2: describes what it does
	BuffDefStruct *Find(const char *type);											// 0x677660  // NAME 2: describes what it does

	static BuffDatabaseClass *Instance;												// 0xBE6014  // NAME 2: describes what it holds

private:
	std::hash_map<std::string, BuffDefStruct *> Buffs;							// +0x00  // NAME 2: describes what it holds
};

#endif //__BUFF_DATABASE_H
