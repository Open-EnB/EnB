// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\cfaction_database.h (name assumed from
**	cfaction_database.cpp, whose __FILE__ is 'e:\build\beta\G\app\Client\cfaction_database.cpp')
**
**	The factions' names and descriptions, read once from cfaction_t.ini (one section per faction)
**	into a static SGI hash_map keyed by the section name (libs/sgistl, D87). The member _Factions
**	is the client's name (the assert 'it == _Factions.end()'); the rest are guesses (D10, D24).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __CFACTION_DATABASE_H  // NAME 3 [cfaction_database.h]: header name assumed from the client's cfaction_database.cpp
#define __CFACTION_DATABASE_H

#include <string>
#include <hash_map.h>

class CFactionDatabaseClass  // NAME 3: by convention (D24) from the client's file cfaction_database.cpp
{
public:
	static void Init();																		// 0x7E4BE0: loads cfaction_t.ini on the first call (a use count)  // NAME 2: describes what it does
	static void Shutdown();																	// 0x7E4F00: empties the table on the last call  // NAME 2: describes what it does
	static const std::string &Get_Name(const std::string &faction);					// 0x7E5040  // NAME 2: describes what it does
	static const std::string &Get_Description(const std::string &faction);		// 0x7E50D0 (line 98)  // NAME 2: describes what it does

	struct FactionStruct  // NAME 2: describes what it holds
	{
		std::string Name;																	// +0x00, the ini's Name  // NAME 2: the ini key
		std::string Description;														// +0x10, the ini's Description  // NAME 2: the ini key
	};

private:
	static std::hash_map<std::string, FactionStruct> _Factions;					// 0xBE8B08
	static int UseCount;																		// 0xBE8B24  // NAME 2: describes what it does
};

#endif //__CFACTION_DATABASE_H
