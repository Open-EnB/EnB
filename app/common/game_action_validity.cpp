// Earth & Beyond client (reconstruction): the object at client 0x83C870 (INTERIM file name, see the notes at the end).
#include "game_action_validity.h"
#include <string.h>
#include <vector>

// aux_roots.h's, for these three (that header is the generated roots', far too much for this object)
enum GameActionValidityBitFlag { UNKNOWN_B018F4_0 };	// NAME 5: the client's Get_Type_Name string 'GameActionValidityBitFlag'

#include "..\shared\unknown_4fac20.h"
#include "..\shared\unknown_4a1590.h"

// @0x83C870
GameActionValidityClass::GameActionValidityClass() : Flags(0)
{
}

// @0x83C8A0
GameActionValidityClass::GameActionValidityClass(unsigned int flags, const std::string &reason) : Flags(flags), Reason(reason)
{
}

// @0x83CA30
// NOT EXACT: inline-budget (the client's second assign leaves _Tidy and erase(0, 0) out of line; ours inlines _Tidy)
GameActionValidityClass &GameActionValidityClass::operator|=(const GameActionValidityClass &other)
{
	bool valid = Is_Valid();
	if (valid && !other.Is_Valid()) {
		Reason = other.Reason;
	} else if (!valid && !other.Is_Valid() && Reason.empty()) {
		Reason = other.Reason;
	}
	Flags |= other.Flags;
	return(*this);
}

// @0x83CC00
GameActionValidityClass operator|(const GameActionValidityClass &a, const GameActionValidityClass &b)
{
	if (b.Flags == 0) {
		return(a);
	}
	if (a.Flags == 0) {
		return(b);
	}
	GameActionValidityClass result(a);	// NAME 1: neutral
	result |= b;
	return(result);
}

// @0x83CE30
GameActionValidityClass::~GameActionValidityClass()
{
}

static std::string GameActionValiditySeparator("|");	// 0xBF3480 (between the names of Value_To_String 0x83D490)  // NAME 1: what it is

// One bit's name (0x14 bytes)
struct GameActionValidityNameStruct	// NAME 1: what it is
{
	// 0x83D0E0 (the implicit destructor is 0x83D220, the array's atexit 0x83D1F0)
	GameActionValidityNameStruct(unsigned int flag, const char *name) : Flag(flag), Name(name) {}	// NAME 2 [flag, name]: what they hold

	unsigned int Flag;		// +0x00  // NAME 2: what it holds
	std::string Name;		// +0x04  // NAME 2: what it holds
};

// 0xBF3390 (initialised by 0x83CFB0)
static GameActionValidityNameStruct GameActionValidityNames[12] = {	// NAME 1: what it is
	GameActionValidityNameStruct(0, "ok"),
	GameActionValidityNameStruct(1, "range"),
	GameActionValidityNameStruct(2, "energy"),
	GameActionValidityNameStruct(4, "recycle"),
	GameActionValidityNameStruct(8, "target"),
	GameActionValidityNameStruct(0x10, "enemy"),
	GameActionValidityNameStruct(0x20, "friend"),
	GameActionValidityNameStruct(0x40, "group"),
	GameActionValidityNameStruct(0x80, "active"),
	GameActionValidityNameStruct(0x100, "other"),
	GameActionValidityNameStruct(0x200, "error"),
	GameActionValidityNameStruct(0x400, "resist"),
};

// The bits' or (an inline layer the client spends budget on, as item_effect.cpp's)
inline GameActionValidityBitFlag &operator|=(GameActionValidityBitFlag &value, unsigned int bits)
{
	value = (GameActionValidityBitFlag)(value | bits);
	return(value);
}

// @0x83D270
const char *Type_Name(GameActionValidityBitFlag)
{
	return("GameActionValidityBitFlag");
}

// @0x83D290
bool String_To_Value(const char *text, GameActionValidityBitFlag *value)
{
	*value = (GameActionValidityBitFlag)0;
	std::vector<std::string> list;	// NAME 1 [list, found, it, i, entry]: neutral
	String_Split(list, std::string(text), GameActionValiditySeparator);
	bool found = false;
	for (std::vector<std::string>::iterator it = list.begin(); it != list.end(); ++it) {
		for (int i = 0; i < sizeof(GameActionValidityNames) / sizeof(GameActionValidityNames[0]); i++) {
			const GameActionValidityNameStruct &entry = GameActionValidityNames[i];
			if (_stricmp(entry.Name.c_str(), it->c_str()) == 0) {
				*value |= entry.Flag;
				found = true;
				break;
			}
		}
	}
	return(found);
}

// @0x83D490
char *Value_To_String(char *buf, int size, GameActionValidityBitFlag value)
{
	std::string text;	// NAME 1 [text, entry]: neutral
	for (const GameActionValidityNameStruct *entry = GameActionValidityNames; entry < GameActionValidityNames + 12; entry++) {
		if (entry->Flag != 0 && (value & entry->Flag) == entry->Flag) {
			text += entry->Name + GameActionValiditySeparator;
		}
	}
	if (text.size() == 0) {
		return(NULL);
	}
	return(Bounded_Strcpy(buf, size, text.c_str()));
}

/*
**	Reconstruction notes. The client holds no __FILE__ or assert for this object: it starts after the run of
**	LoginGalaxyClass's wire functions (0x83C730-0x83C86F, unknown_galaxy.h) and ends before 0x83D6A0. Its last three
**	functions are the converters of GameActionValidityBitFlag (aux_roots.h; called by its KVPair's Get_Type_Name
**	0x707E80, Set_Value_String 0x709720 and Get_Value_String 0x707EA0), in item_effect.cpp's and
**	target_generator_summary.cpp's pattern: the separator '|' (initialiser 0x83CE80, its atexit 0x83CF40) and the
**	twelve bits' names. Before them sit five out-of-line members no code calls: a 0x14-byte class of the same
**	unsigned + std::string shape whose operator| ors the bits and keeps the first reason, read here as the validity
**	result those bits describe. Hence the interim file name (D137, NAME 3), which keeps the object order between
**	galaxy_map and group_common.
*/
