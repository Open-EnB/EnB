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

struct ControlKeyDefStruct  // NAME 1: neutral (D10): what a key definition's DefPtr points at (0x38 bytes or more)
{
	std::string Name;																		// +0x00  // NAME 2: 0x71CC60 returns it; the table's key
	std::string Unknown10;																// +0x10  // NAME 1: neutral (D10)
	std::string Unknown20;																// +0x20  // NAME 1: neutral (D10)
	int Index;																				// +0x30: def->Get_Index() in the asserts  // NAME 4: Get_Index (the client's assert)
	int Unknown34;																			// +0x34  // NAME 1: neutral (D10)
};

class ControlKeyDefClass  // NAME 1: neutral (D10): the holder whose member the asserts call DefPtr
{
public:
	const std::string &Get_Unknown10() const;										// 0x71CC20  // NAME 1: neutral (D10)
	const std::string &Get_Name() const;											// 0x71CC60  // NAME 2: returns the definition's first string
	const std::string &Get_Unknown20() const;										// 0x71CCA0  // NAME 1: neutral (D10)
	int Get_Index() const;																// 0x71CCE0  // NAME 5: 'def->Get_Index() >= UserKeys.size()'
	int Get_Unknown34() const;															// 0x71CD20  // NAME 1: neutral (D10)

	ControlKeyDefStruct *DefPtr;														// +0x00: the client's '!DefPtr'
};

#endif //__CONTROL_KEYS_MANAGER_H
