/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\option.h (name assumed from option.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\Client\option.cpp')
**
**	The player's options ('Client Options' in its reports): five SGI hash_maps (libs/sgistl, D87)
**	keyed by the option's name, one per value type (bool, int, float, Vector3, std::string), each
**	value a new'd option holding its name, its value and its default. Saved per character as
**	player<id>_options.ini, sections BOOL-000, INT-000, FLOAT-000, VECTOR3-000, STRING-000 with
**	OptionName / OptionValue. The object is WinMain's 'options manager' at 0xBE8B38
**	(docs/kernel-map-step9.md); tsk_game.h, sclient.cpp, fontdatabase.cpp and soundlib.cpp still
**	declare their own views of it (Unknown_BE8B38Class).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __OPTION_H  // NAME 3 [option.h]: header name assumed from the client's option.cpp
#define __OPTION_H

#include <string>
#include <hash_map.h>
#include "vector3.h"

class SClient;

struct BoolOptionStruct  // NAME 2: what it holds (0x14 bytes)
{
	BoolOptionStruct(const char *name, bool value) : Name(name), Value(value), Default(value) {}	// 0x7F7E10
	const char *Get_Name() { return(Name.c_str()); }		// 0x7F7F30  // NAME 2: what it does
	bool Get_Value() { return(Value); }		// inlined into Save  // NAME 2: what it does
	void Set_Value(bool value) { Value = value; }		// inlined into Load  // NAME 2: what it does

	std::string Name;										// +0x00  // NAME 2: the ini's OptionName
	bool Value;												// +0x10  // NAME 2: the ini's OptionValue
	bool Default;											// +0x11  // NAME 2: what it holds
};

struct IntOptionStruct  // NAME 2: what it holds (0x18 bytes)
{
	IntOptionStruct(const char *name, int value) : Name(name), Value(value), Default(value) {}		// 0x7F7F50
	const char *Get_Name() { return(Name.c_str()); }		// 0x7F8070  // NAME 2: what it does
	int Get_Value() { return(Value); }		// inlined into Save  // NAME 2: what it does
	void Set_Value(int value) { Value = value; }		// inlined into Load  // NAME 2: what it does

	std::string Name;										// +0x00  // NAME 2: the ini's OptionName
	int Value;												// +0x10  // NAME 2: the ini's OptionValue
	int Default;											// +0x14  // NAME 2: what it holds
};

struct FloatOptionStruct  // NAME 2: what it holds (0x18 bytes)
{
	FloatOptionStruct(const char *name, float value) : Name(name), Value(value), Default(value) {}	// 0x7F8090
	const char *Get_Name() { return(Name.c_str()); }		// 0x7F8180  // NAME 2: what it does
	void Set_Value(float value) { Value = value; }		// inlined into Load  // NAME 2: what it does

	std::string Name;										// +0x00  // NAME 2: the ini's OptionName
	float Value;											// +0x10  // NAME 2: the ini's OptionValue
	float Default;											// +0x14  // NAME 2: what it holds
};

struct Vector3OptionStruct  // NAME 2: what it holds (0x28 bytes)
{
	Vector3OptionStruct(const char *name, Vector3 value) : Name(name), Value(value), Default(value) {}	// 0x7F81A0
	const char *Get_Name() { return(Name.c_str()); }		// 0x7F8240  // NAME 2: what it does
	Vector3 Get_Value() { return(Value); }		// inlined into Save  // NAME 2: what it does
	void Set_Value(Vector3 value) { Value = value; }		// 0x765BB0 (the linker's one copy)  // NAME 2: what it does

	std::string Name;										// +0x00  // NAME 2: the ini's OptionName
	Vector3 Value;											// +0x10  // NAME 2: the ini's OptionValue
	Vector3 Default;										// +0x1C  // NAME 2: what it holds
};

struct StringOptionStruct  // NAME 2: what it holds (0x30 bytes)
{
	StringOptionStruct(const char *name, const char *value) : Name(name), Value(value), Default(value) {}	// 0x7F8260
	const char *Get_Name() { return(Name.c_str()); }		// 0x7F83F0  // NAME 2: what it does
	const char * Get_Value() { return(Value.c_str()); }		// inlined into Save  // NAME 2: what it does
	void Set_Value(const char *value) { Value = value; }		// inlined into Load  // NAME 2: what it does

	std::string Name;										// +0x00  // NAME 2: the ini's OptionName
	std::string Value;										// +0x10  // NAME 2: the ini's OptionValue
	std::string Default;									// +0x20  // NAME 2: what it holds
};

class OptionClass  // NAME 3: by convention (D24) from the client's file option.cpp (WinMain's 'options manager')
{
public:
	OptionClass();											// 0x7F0F60
	~OptionClass();											// 0x7F1470

	static void Set_Instance(OptionClass *options);			// 0x7F1840 (0xBE8B38)  // NAME 2: what it does

	void Free_Options();									// 0x7F1860 (inlined into the destructor and Set_Defaults)  // NAME 2: what it does
	void Set_Defaults(SClient *client);						// 0x7F1AD0  // NAME 2: what it does
	void Load(__int64 character_id, SClient *client);		// 0x7F84F0 (ClientTaskClass's 0x766350)  // NAME 2: what it does
	void Save();											// 0x7F8EF0  // NAME 2: what it does

	BoolOptionStruct *Find_Bool(const char *name);			// 0x7F9520  // NAME 2: what it does
	IntOptionStruct *Find_Int(const char *name);			// 0x7F9550  // NAME 2: what it does
	FloatOptionStruct *Find_Float(const char *name);		// 0x7F9580  // NAME 2: what it does
	Vector3OptionStruct *Find_Vector3(const char *name);	// 0x7F95B0  // NAME 2: what it does
	StringOptionStruct *Find_String(const char *name);		// 0x7F95E0  // NAME 2: what it does

	bool Get_Bool(const char *name);						// 0x7F9610  // NAME 2: what it does
	int Get_Int(const char *name);							// 0x7F9640  // NAME 2: what it does
	float Get_Float(const char *name);						// 0x7F9670  // NAME 2: what it does
	Vector3 Get_Vector3(const char *name);					// 0x7F96B0  // NAME 2: what it does
	std::string Get_String(const char *name);				// 0x7F9700  // NAME 2: what it does

	bool Get_Default_Bool(const char *name);				// 0x7F9880  // NAME 2: what it does
	int Get_Default_Int(const char *name);					// 0x7F98B0  // NAME 2: what it does
	float Get_Default_Float(const char *name);				// 0x7F98E0  // NAME 2: what it does
	Vector3 Get_Default_Vector3(const char *name);			// 0x7F9920  // NAME 2: what it does
	std::string Get_Default_String(const char *name);		// 0x7F9970  // NAME 2: what it does

	static OptionClass *Instance;							// 0xBE8B38  // NAME 2: what it holds

private:
	std::hash_map<std::string, BoolOptionStruct *> Bools;			// +0x00  // NAME 2: what it holds
	std::hash_map<std::string, IntOptionStruct *> Ints;				// +0x18  // NAME 2: what it holds
	std::hash_map<std::string, FloatOptionStruct *> Floats;			// +0x30  // NAME 2: what it holds
	std::hash_map<std::string, Vector3OptionStruct *> Vector3s;		// +0x48  // NAME 2: what it holds
	std::hash_map<std::string, StringOptionStruct *> Strings;		// +0x60  // NAME 2: what it holds
	int CharacterID;										// +0x78: the character's id modulo 1000000000 (the file name)  // NAME 2: what it holds
};

#endif //__OPTION_H
