// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\combo_ffactory.h (name assumed from
**	combo_ffactory.cpp, whose __FILE__ is 'e:\build\beta\G\app\shared\combo_ffactory.cpp')
**
**	A file factory over a directory and every .MIX file in it: a file is taken from the global factory
**	when it is there, else from the mix file that holds it (MixMap, keyed by the lower-case file name in
**	an SGI hash_map, libs/sgistl, D87). The class name is the file's (D24); MixMap is the client's own
**	(its assert 'MixMap.find(temp_string) != MixMap.end()').
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __COMBO_FFACTORY_H  // NAME 3 [combo_ffactory.h]: header name assumed from the client's combo_ffactory.cpp
#define __COMBO_FFACTORY_H

#include <string>
#include <list>
#include <hash_map.h>
#include "..\..\wwlib\ffactory.h"

class MixFileFactoryClass;

class ComboFileFactoryClass : public FileFactoryClass  // NAME 3: by convention (D24) from the client's file combo_ffactory.cpp
{
public:
	ComboFileFactoryClass(const char *path);											// 0x4E4D10: the directory and its *.MIX files
	virtual ~ComboFileFactoryClass();													// 0x4E5230 (vtable 0xAEC5D0)

	virtual FileClass *Get_File(char const *filename);								// 0x4E57A0
	virtual void Return_File(FileClass *file);										// 0x4E5A00

	void Add_Mix(MixFileFactoryClass *mix);											// 0x4E5440  // NAME 2: describes what it does

private:
	SimpleFileFactoryClass *Simple;														// +0x04  // NAME 2: describes what it holds
	std::hash_map<std::string, MixFileFactoryClass *> MixMap;					// +0x08, the client's name (its assert)
	std::list<MixFileFactoryClass *> Mixes;											// +0x20  // NAME 2: describes what it holds
};

#endif //__COMBO_FFACTORY_H
