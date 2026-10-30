// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\unique_design_id_lookup.h (name assumed from
**	unique_design_id_lookup.cpp, whose __FILE__ is 'e:\build\beta\G\app\shared\unique_design_id_lookup.cpp')
**
**	Numeric ids of named designs, read from one section of an ini (each entry 'id=name', the names
**	lower-cased) into an SGI hash_map (libs/sgistl, D87); new names are added to the ini, which is
**	saved when the table is destroyed. INI and Map are the client's names (its asserts); the class
**	name is the file's (D24), the rest are guesses (D10). Nothing in the retail client constructs it.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __UNIQUE_DESIGN_ID_LOOKUP_H  // NAME 3 [unique_design_id_lookup.h]: header name assumed from the client's unique_design_id_lookup.cpp
#define __UNIQUE_DESIGN_ID_LOOKUP_H

#include <string>
#include <hash_map.h>
#include "..\..\wwlib\ini.h"

class UniqueDesignIDLookupClass  // NAME 3: by convention (D24) from the client's file unique_design_id_lookup.cpp (vtable 0xAECCF4)
{
public:
	UniqueDesignIDLookupClass(const std::string &filename, const std::string &section_name);	// 0x4FFC70
	virtual ~UniqueDesignIDLookupClass();														// 0x5002C0
	virtual void Add_Identifier(int id, const std::string &name);							// 0x500760  // NAME 2: describes what it does

	int Get_ID(const std::string &name);														// 0x500470  // NAME 2: describes what it does
	std::string Get_Identifier(int id);														// 0x5005F0  // NAME 2: describes what it does

	static const int INVALID_ID;													// 0xAECC44 (0; also read by 0x838970-0x838B80)  // NAME 2: describes what it is

protected:
	std::string Filename;																		// +0x04  // NAME 2: describes what it holds
	INIClass INI;																				// +0x14, the client's name (its assert)
	std::string Section;																		// +0x24  // NAME 2: describes what it holds
	std::hash_map<std::string, int> Map;														// +0x34, the client's name (its assert)
};

#endif
