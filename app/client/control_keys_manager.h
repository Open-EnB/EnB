/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\control_keys_manager.h (name assumed from
**	control_keys_manager.cpp, whose __FILE__ is 'e:\build\beta\G\app\Client\control_keys_manager.cpp')
**
**	The control keys: control_keys.ini's key definitions (a static table keyed by the key name,
**	'_KeyDefinitions' in the client's asserts) and keymap.ini's user and default mappings, one
**	ControlKeysManager (RTTI) per SClient ('_Instances'). Declared as far as the rebuilt functions need.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __CONTROL_KEYS_MANAGER_H  // NAME 3 [control_keys_manager.h]: header name assumed from the client's control_keys_manager.cpp
#define __CONTROL_KEYS_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <hash_map.h>

#define NUM_MAPPING_SETS	2	// the client's asserts compare '(unsigned int)set > NUM_MAPPING_SETS' with 2

struct ControlKeyDefStruct  // NAME 1: neutral (D10): what a key definition's DefPtr points at (0x38 bytes or more)
{
	std::string Name;																		// +0x00  // NAME 2: 0x71CC60 returns it; the table's key
	std::string Unknown10;																// +0x10  // NAME 1: neutral (D10)
	std::string Unknown20;																// +0x20  // NAME 1: neutral (D10)
	int Get_Index() const { return Index; }				// the client's 'def->Get_Index()', inlined

	int Unknown30;								// +0x30  // NAME 1: neutral (D10)
	int Index;									// +0x34: def->Get_Index() in the asserts
};

class ControlKeyDefClass  // NAME 1: neutral (D10): the holder whose member the asserts call DefPtr
{
public:
	const std::string &Get_Unknown10() const;										// 0x71CC20  // NAME 1: neutral (D10)
	const std::string &Get_Name() const;											// 0x71CC60  // NAME 2: returns the definition's first string
	const std::string &Get_Unknown20() const;										// 0x71CCA0  // NAME 1: neutral (D10)
	int Get_Unknown30() const;						// 0x71CCE0  // NAME 1: neutral (D10)
	int Get_Index() const;							// 0x71CD20: the definition's +0x34, as 'def->Get_Index()'

	ControlKeyDefStruct *DefPtr;														// +0x00: the client's '!DefPtr'
};

struct ControlKeyComboStruct  // NAME 1: neutral (D10): 0x10 bytes, what 0x42B7E0 turns into text
{
	ControlKeyComboStruct(unsigned short key, unsigned short modifiers) : Key(key), Modifiers(modifiers), Unknown04(0), Unknown08(0), Unknown0C(0) {}

	unsigned short Key;																	// +0x00  // NAME 2: the key's code
	unsigned short Modifiers;															// +0x02  // NAME 2: the modifier bits
	int Unknown04;																			// +0x04  // NAME 1: neutral (D10)
	int Unknown08;																			// +0x08  // NAME 1: neutral (D10)
	int Unknown0C;																			// +0x0C  // NAME 1: neutral (D10)
};

struct ControlKeyStruct  // NAME 1: neutral (D10): one key of a mapping (4 bytes)
{
	ControlKeyStruct() : Key(0), Modifiers(0) {}

	unsigned short Key;																	// +0x00  // NAME 2: the key's code
	unsigned short Modifiers;															// +0x02  // NAME 2: the modifier bits
};

struct ControlKeyValueStruct  // NAME 1: neutral (D10): the 4-byte key value 0x5CB000's options screen copies
{
	ControlKeyValueStruct(const ControlKeyValueStruct &src);					// 0x71C9C0
	ControlKeyValueStruct(unsigned int value);										// 0x71CA80 (no caller)
	ControlKeyValueStruct &operator=(const ControlKeyValueStruct &src);	// 0x71C9F0 (no caller)
	operator ControlKeyComboStruct() const;										// 0x71CA20
	operator unsigned int() const;													// 0x71CA50 (no caller)

	unsigned short Key;																	// +0x00  // NAME 2: the key's code
	unsigned short Modifiers;															// +0x02  // NAME 2: the modifier bits
};

struct KeyMappingStruct  // NAME 1: neutral (D10): one key definition's mapping (0xC bytes)
{
	std::string &Get_Key_String(int set, std::string &out);					// 0x71CAB0  // NAME 2: describes what it does

	int Index;																				// +0x00  // NAME 2: the definition's index
	ControlKeyStruct Keys[NUM_MAPPING_SETS];										// +0x04: the client's 'set'
};

struct KeyIdStruct  // NAME 1: neutral (D10): the value of _KeyIds (0x14 bytes)
{
	int Id;																	// +0x00: the client's 'key_id'  // NAME 2: what it holds
	std::string ModuleName;												// +0x04: the client's 'key_id_ref.ModuleName'
};

class ControlKeyListClass  // NAME 1: neutral (D10): the screen's key list Add_Keys / Remove_Keys fill ('keylist')
{
public:
	void Add(const ControlKeyComboStruct &combo, void *handler);			// 0x721E60  // NAME 2: describes what it does
	void Remove(const ControlKeyComboStruct &combo);						// 0x721EF0  // NAME 2: describes what it does
};

class ControlKeysManager  // NAME 5: the client's RTTI .?AVControlKeysManager@@
{
public:
	virtual ~ControlKeysManager();													// not rebuilt yet

	KeyMappingStruct *Set_Key(KeyMappingStruct *entry, int set, const ControlKeyStruct &key, bool force);	// 0x71A860: returns the mapping already using key unless force  // NAME 2: describes what it does
	int Add_Keys(ControlKeyListClass *keylist, const std::string &name, void *handler, unsigned short modifiers);	// 0x71A960  // NAME 2: describes what it does
	int Remove_Keys(ControlKeyListClass *keylist, const std::string &name, unsigned short modifiers);	// 0x71AB40  // NAME 2: describes what it does
	void Restore_Keys();												// 0x71ADC0: RestoreKeys back into UserKeys  // NAME 2: describes what it does
	void Set_Default_PSW();											// 0x71ADF0  // NAME 2: describes what it does
	void Set_Default_SIM();											// 0x71AE20  // NAME 2: describes what it does
	const std::string &Get_Key_Name(const ControlKeyValueStruct &key);	// 0x71AE50: the definition's name for a key  // NAME 2: describes what it does
	void Build_Keys_Map();											// 0x71AEF0: UserKeysMap from UserKeys (not rebuilt yet)  // NAME 2: describes what it does
	static bool Register_Key(const std::string &module_name, const std::string &key_name, int key_id);	// 0x719ED0  // NAME 2: describes what it does
	static bool _Does_Key_Exist(const std::string &key_name);	// 0x71B160 (no caller; inlined at 0x719ED0): the client's assert
	static void Copy_Keys(std::vector<KeyMappingStruct> *to, const std::vector<KeyMappingStruct> &from);	// 0x71C4C0  // NAME 2: describes what it does

	std::hash_map<unsigned int, ControlKeyDefStruct *> UserKeysMap;		// +0x04: the client's asserts, keyed by the key value
	static std::hash_map<std::string, std::auto_ptr<ControlKeyDefStruct> > _KeyDefinitions;	// 0xBE7BC8: the client's asserts
	static std::hash_map<std::string, KeyIdStruct> _KeyIds;				// 0xBE7B98 (ctor 0x71D8F0, dtor 0x719C80)  // NAME 1: neutral (D10)
	static std::vector<KeyMappingStruct> _DefaultPSW;					// 0xBE7BB8: the client's asserts
	static std::vector<KeyMappingStruct> _DefaultSIM;					// 0xBE7C30: the client's asserts
	static unsigned int _NumKeyDefs;								// 0xBE7C44: the client's asserts

	std::vector<KeyMappingStruct> UserKeys;				// +0x1C: the client's asserts
	std::vector<KeyMappingStruct> RestoreKeys;				// +0x2C: the client's asserts
};

#endif //__CONTROL_KEYS_MANAGER_H
