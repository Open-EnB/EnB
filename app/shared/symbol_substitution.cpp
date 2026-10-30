// Earth & Beyond client (reconstruction): app\shared\symbol_substitution.cpp. Notes at the end of the file.
#include "symbol_substitution.h"
#include "unknown_debug.h"

void Unknown_4BC260(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// NAME 1 [Unknown_4BC260]: 0x4BC260, 0x4BC000's sibling

SymbolSubstitutionClass::SymbolSubstitutionClass()
{
}

SymbolSubstitutionClass::~SymbolSubstitutionClass()
{
	Symbols.clear();
}

void SymbolSubstitutionClass::Add_Symbol(const std::string &name, const std::string &value)
{
	Symbols[name] = value;
}

const std::string &SymbolSubstitutionClass::Get_Symbol(const std::string &name) const
{
	std::hash_map<std::string, std::string>::const_iterator it = Symbols.find(name);
	if (it == Symbols.end()) {
		static std::string empty("");
		return empty;
	}
	return it->second;
}

void SymbolSubstitutionClass::Clear()
{
	Symbols.clear();
}
























































static size_t Find_Mark(const std::string &str, size_t pos)	// NAME 2: describes what it does (not in the client: inlined into both searches)
{
	size_t index = str.find('$', pos);
	while (index != std::string::npos && index != 0 && str.at(index - 1) == '\\') {
		index = str.find('$', index + 1);
	}
	return index;
}
// NOT EXACT: inline budget, the client calls string::erase(iterator) 0x4E2A00 out of line in the last loop
bool Substitute_Symbols(std::string &str, const SymbolSubstitutionClass &symbols)
{
	std::string original(str);
	size_t start;
	size_t end;

	while (true) {
		start = Find_Mark(str, 0);
		if (start == std::string::npos) break;

		end = Find_Mark(str, start + 1);
		if (end == std::string::npos) {
			Unknown_4BC260("", "SymbolSubstitution", __FILE__, __LINE__, "Unmatched '%c' pair! Cannot perform string substitution on string \"%s\"", '$', original.c_str());
			str = original;
			return false;
		}

		size_t len = end - start + 1;
		std::string name = str.substr(start + 1, len - 2);
		const std::string &value = symbols.Get_Symbol(name);
		if (value.length() == 0) {
			str.erase(start, len);
		} else {
			str.reserve(str.length() - len - 2 + value.length());
			str.replace(start, len, value);
		}
	}

	for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
		if (*it == '\\' && it + 1 != str.end() && it[1] == '$') {
			it = str.erase(it);
		}
	}
	return true;
}

void Escape_Symbols(std::string &str)
{
	for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
		if (*it == '$') {
			it = str.insert(it, '\\');
			++it;
		}
	}
}
/*
**	Reconstruction notes
**
**	client.exe 0x4FCBD0-0x4FD7A0, then the string's and the table's instances to 0x4FE770: the
**	constructor 0x4FCBD0 (the table's 100 buckets inlined), the destructor 0x4FCE00 (an explicit
**	clear() before the table's own: two clear loops), Add_Symbol 0x4FD000, Get_Symbol 0x4FD190 (its
**	function-static empty string 0xBE5830, guard 0xBE5840), Clear 0x4FD330 (no caller),
**	Substitute_Symbols 0x4FD3B0 (the report on the client's line 112, D25) and Escape_Symbols
**	0x4FD7A0 (no caller). The only user is GuildMessageSectorPacket::Process_Packet 0x73E9E0
**	(cpacket.cpp): a local table, two Add_Symbol calls, then Substitute_Symbols on the message.
**	Find_Mark is not in the client: it is how the two searches compile (const string::at 0x4FD950
**	out of line, the same temporaries for the '$' in both). NOT EXACT: Substitute_Symbols, the
**	client calls string::erase(iterator) 0x4E2A00 out of line in the unescape loop (inline budget).
*/
