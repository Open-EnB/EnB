// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\ini_manager.h (name assumed from ini_manager.cpp,
**	whose __FILE__ is 'e:\build\beta\G\app\shared\ini_manager.cpp')
**
**	The ini files as shared definitions (definition_manager.h): one INIClass per file name, loaded on
**	first use and kept while it is used. Names are 'file:section'.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __INI_MANAGER_H  // NAME 3 [ini_manager.h]: header name assumed from the client's ini_manager.cpp
#define __INI_MANAGER_H

#include "definition_manager.h"
#include "definition_auto_ptr.h"
#include "..\..\wwlib\ini.h"

// vtable 0xAEC6CC (0x24 bytes)
class INIManager : public DefinitionManagerClass<INIClass>  // NAME 5: thunk order, binary.md 5.11
{
public:
	INIManager();															// 0x4EA080, in ini_manager.cpp: Get_INI calls it
	// NOT EXACT: register-only (explain.py), eax / ecx / edx rotated in the inlined Free_Unused (0x4EA2B0)
	virtual ~INIManager()
	{
		Free_Unused();
	}

	virtual INIClass *Create(const std::string &name);			// 0x4E9D40
	virtual void Destroy(INIClass *ini);								// 0x4E9DD0

	static DefinitionAutoPtr<INIClass> Get_INI(const std::string &name);	// 0x4E9E00  // NAME 2: what it does
	static bool Split_Name(const std::string &name, std::string *ini_name, std::string *ini_section);	// 0x4EA5A0  // NAME 2: what it does
};

#endif //__INI_MANAGER_H
