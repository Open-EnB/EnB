// Earth & Beyond client (reconstruction): app\Client\control_keys_manager.cpp. Notes at the end of the file.
#include "control_keys_manager.h"
#include "..\shared\unknown_debug.h"

std::string &Unknown_42B7E0(const ControlKeyComboStruct &key, std::string &out);	// NAME 1 [Unknown_42B7E0]: neutral (D10): 0x42B7E0, the key's text

static std::string _Empty;  // NAME 2: 0xBE7B80, what the accessors return without a definition





























































































































































































KeyMappingStruct *ControlKeysManager::Set_Key(KeyMappingStruct *entry, int set, const ControlKeyStruct &key, bool force)
{
	FAIL_IF((unsigned int)set > NUM_MAPPING_SETS) return NULL;
	if (!key.Key) {
		entry->Keys[set].Key = 0;
		entry->Keys[set].Modifiers = 0;
		return NULL;
	}
	for (std::vector<KeyMappingStruct>::iterator it = UserKeys.begin(); it != UserKeys.end(); ++it) {
		for (int i = 0; i < NUM_MAPPING_SETS; i++) {
			ControlKeyStruct &k = it->Keys[i];
			if (k.Key == key.Key && k.Modifiers == key.Modifiers) {
				if (!force && it != entry) return it;
				k.Key = 0;
				k.Modifiers = 0;
			}
		}
	}
	entry->Keys[set] = key;
	return NULL;
}






























































































































































































































































































































































































ControlKeyValueStruct::ControlKeyValueStruct(const ControlKeyValueStruct &src)
{
	Key = src.Key;
	Modifiers = src.Modifiers;
}

ControlKeyValueStruct &ControlKeyValueStruct::operator=(const ControlKeyValueStruct &src)
{
	Key = src.Key;
	Modifiers = src.Modifiers;
	return *this;
}

ControlKeyValueStruct::operator ControlKeyComboStruct() const
{
	return ControlKeyComboStruct(Key, Modifiers);
}

ControlKeyValueStruct::operator unsigned int() const
{
	return (Modifiers << 16) | Key;
}

ControlKeyValueStruct::ControlKeyValueStruct(unsigned int value)
{
	Key = (unsigned short)value;
	Modifiers = (unsigned short)(value >> 16);
}











// NOT EXACT: register-only in the failure path (out in ebp, npos in ebx; the client swaps them); erase(0, npos) and resize(0) the same
std::string &KeyMappingStruct::Get_Key_String(int set, std::string &out)
{
	FAIL_IF((unsigned int)set > NUM_MAPPING_SETS) {
		out.erase();
		return out;
	}
	return Unknown_42B7E0(ControlKeyComboStruct(Keys[set].Key, Keys[set].Modifiers), out);
}

const std::string &ControlKeyDefClass::Get_Unknown10() const
{
	FAIL_IF(!DefPtr) return _Empty;
	return DefPtr->Unknown10;
}

const std::string &ControlKeyDefClass::Get_Name() const
{
	FAIL_IF(!DefPtr) return _Empty;
	return DefPtr->Name;
}

const std::string &ControlKeyDefClass::Get_Unknown20() const
{
	FAIL_IF(!DefPtr) return _Empty;
	return DefPtr->Unknown20;
}


int ControlKeyDefClass::Get_Index() const
{
	FAIL_IF(!DefPtr) return -1;
	return DefPtr->Index;
}

int ControlKeyDefClass::Get_Unknown34() const
{
	FAIL_IF(!DefPtr) return -1;
	return DefPtr->Unknown34;
}
/*
**	Reconstruction notes
**
**	client.exe: the object opens with the static empty string 0xBE7B80 (initializer 0x7197A0,
**	destructor 0x7197E0), then _Instances (0xBE7BE0, ctor 0x71D2F0 out of line), three vectors
**	(0xBE7BB8, 0xBE7C30, 0xBE7BF8: _DefaultPSW / _DefaultSIM and a third) and _KeyDefinitions
**	(0xBE7BC8, ctor 0x71D0E0, destructor 0x719B50), the manager's functions 0x719D50-0x71CAB0
**	(asserts on lines 64-642) and the definition accessors 0x71CC20-0x71CD20 (lines 651-676), then
**	the containers' instances to 0x720F80. Rebuilt so far: the static string, Set_Key 0x71A860, the
**	key value's helpers 0x71C9C0-0x71CA80 (called from the options screen 0x5CB000), Get_Key_String
**	0x71CAB0 and the definition accessors.
*/
