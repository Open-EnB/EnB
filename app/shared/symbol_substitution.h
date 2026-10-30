// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\symbol_substitution.h (name assumed from
**	symbol_substitution.cpp, whose __FILE__ is 'e:\build\beta\G\app\shared\symbol_substitution.cpp')
**
**	Named symbols (name -> text, an SGI hash_map, libs/sgistl, D87) substituted into a string where it
**	says $name$; '\$' is a literal dollar sign. The client names neither the class nor its methods (D10,
**	D24); its report category is 'SymbolSubstitution'.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __SYMBOL_SUBSTITUTION_H  // NAME 3 [symbol_substitution.h]: header name assumed from the client's symbol_substitution.cpp
#define __SYMBOL_SUBSTITUTION_H

#include <string>
#include <hash_map.h>

class SymbolSubstitutionClass  // NAME 3: by convention (D24) from the client's file symbol_substitution.cpp
{
public:
	SymbolSubstitutionClass();														// 0x4FCBD0
	~SymbolSubstitutionClass();														// 0x4FCE00

	void Add_Symbol(const std::string &name, const std::string &value);			// 0x4FD000  // NAME 2: describes what it does
	const std::string &Get_Symbol(const std::string &name) const;					// 0x4FD190  // NAME 2: describes what it does
	void Clear();																		// 0x4FD330 (no caller)  // NAME 2: describes what it does

private:
	std::hash_map<std::string, std::string> Symbols;								// +0x00, 0x18 bytes  // NAME 2: describes what it holds
};

bool Substitute_Symbols(std::string &str, const SymbolSubstitutionClass &symbols);	// 0x4FD3B0  // NAME 2: describes what it does
void Escape_Symbols(std::string &str);													// 0x4FD7A0 (no caller)  // NAME 2: describes what it does

#endif
