// Earth & Beyond client (reconstruction): app\Client\control_keys_manager.cpp. Notes at the end of the file.
#include "control_keys_manager.h"
#include "..\shared\unknown_debug.h"

char *Unknown_8B44E0(__int64 value, char *buffer);	// 0x8B44E0: _i64toa(value, buffer, 10), as pkguildplayeronline.h  // NAME 1: neutral placeholder (D10)
std::string &Unknown_42B7E0(const ControlKeyComboStruct &key, std::string &out);	// NAME 1 [Unknown_42B7E0]: neutral (D10): 0x42B7E0, the key's text

static std::string _Empty;  // NAME 2: 0xBE7B80, what the accessors return without a definition
unsigned int ControlKeysManager::_NumKeyDefs;
std::hash_map<std::string, std::auto_ptr<ControlKeyDefStruct> > ControlKeysManager::_KeyDefinitions;
std::hash_map<std::string, KeyIdStruct> ControlKeysManager::_KeyIds;
std::vector<KeyMappingStruct> ControlKeysManager::_DefaultPSW;
std::vector<KeyMappingStruct> ControlKeysManager::_DefaultSIM;





















































































bool ControlKeysManager::Register_Key(const std::string &module_name, const std::string &key_name, int key_id)
{
	FAIL_IF(!ControlKeysManager::_Does_Key_Exist(key_name)) return false;
	FAIL_IF(key_id == -1) return false;

	KeyIdStruct &key_id_ref = _KeyIds[key_name];



	FAIL_IF(!key_id_ref.ModuleName.empty()) return false;
	key_id_ref.Id = key_id;
	key_id_ref.ModuleName = module_name;
	return true;
}








// NOT EXACT: inline budget (ours inlines _KeyIds.find, the client calls 0x71EBC0; ~56 rows)
int ControlKeysManager::Get_Key_Id(ControlKeysManager *manager, const std::string &module_name, const ControlKeyValueStruct &key)
{
	FAIL_IF(!manager) return -1;
	ControlKeyValueStruct k(key);
	const std::string &name = manager->Get_Key_Name(k);
	if (name.empty()) return -1;

	std::hash_map<std::string, KeyIdStruct>::iterator it = _KeyIds.find(name);
	if (it == _KeyIds.end()) return -1;

	if (it->second.ModuleName == module_name) return it->second.Id;
	return -1;
}





ControlKeysManager::ControlKeysManager() :
	Client(0),
	UserKeysMap(100)
{
}










void ControlKeysManager::Init(ControlKeysSClientClass *sclient)
{
	if (!_NumKeyDefs) {
		G_ASSERT(_DefaultPSW.empty());
		G_ASSERT(_DefaultSIM.empty());
		Load_Definitions();
	} else {
		G_ASSERT(!_DefaultSIM.empty());
		G_ASSERT(!_DefaultPSW.empty());
	}

	FAIL_IF(!sclient) return;
	Client = sclient;
	Unknown_8B44E0(sclient->Unknown12E8, PlayerID);
	Load_Keys(&UserKeys, PlayerID);
	Build_Keys_Map();
	Copy_Keys(&RestoreKeys, UserKeys);
}











void ControlKeysManager::Find_Mappings(std::vector<KeyMappingStruct *> *vector, int unknown30)
{
	FAIL_IF(!vector) return;
	vector->clear();
	for (std::vector<KeyMappingStruct>::iterator it = UserKeys.begin(); it != UserKeys.end(); ++it) {
		if (it->Get_Unknown30() == unknown30) vector->push_back(&*it);
	}
}





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


















// NOT EXACT: frame 0x2C vs the client 0x28, ~60 rows; the client keeps count in the dead name argument slot and builds the converted key once per set (attempts.csv)
int ControlKeysManager::Add_Keys(ControlKeyListClass *keylist, const std::string &name, void *handler, unsigned short modifiers)
{
	FAIL_IF(!keylist) return -1;

	std::hash_map<std::string, std::auto_ptr<ControlKeyDefStruct> >::iterator def_it = _KeyDefinitions.find(name);
	FAIL_IF(def_it == _KeyDefinitions.end()) return -1;

	ControlKeyDefStruct *def = def_it->second.get();
	FAIL_IF(!def) return -1;

	FAIL_IF(def->Get_Index() >= UserKeys.size()) return -1;

	int count = 0;
	KeyMappingStruct &mapping = UserKeys[def->Get_Index()];
	for (int set = 0; set < NUM_MAPPING_SETS; set++) {
		ControlKeyComboStruct key(mapping.Keys[set].Key, mapping.Keys[set].Modifiers);
		ControlKeyComboStruct combo = key;
		combo.Modifiers |= modifiers;
		if (combo.Key) {
			keylist->Add(combo, handler);
			count++;
		}
	}
	return count;
}

// NOT EXACT: frame 0x2C vs the client 0x28, ~60 rows; the client keeps count in the dead name argument slot and builds the converted key once per set (attempts.csv)
int ControlKeysManager::Remove_Keys(ControlKeyListClass *keylist, const std::string &name, unsigned short modifiers)
{
	FAIL_IF(!keylist) return -1;

	std::hash_map<std::string, std::auto_ptr<ControlKeyDefStruct> >::iterator def_it = _KeyDefinitions.find(name);
	FAIL_IF(def_it == _KeyDefinitions.end()) return -1;

	ControlKeyDefStruct *def = def_it->second.get();
	FAIL_IF(!def) return -1;



	FAIL_IF(def->Get_Index() >= RestoreKeys.size()) return -1;

	int count = 0;
	KeyMappingStruct &mapping = RestoreKeys[def->Get_Index()];
	for (int set = 0; set < NUM_MAPPING_SETS; set++) {
		ControlKeyComboStruct key(mapping.Keys[set].Key, mapping.Keys[set].Modifiers);
		ControlKeyComboStruct combo = key;
		combo.Modifiers |= modifiers;
		if (combo.Key) {
			keylist->Remove(combo);
			count++;
		}
	}
	return count;
}

















void ControlKeysManager::Restore_Keys()
{
	Copy_Keys(&UserKeys, RestoreKeys);
}

void ControlKeysManager::Set_Default_PSW()
{
	Copy_Keys(&UserKeys, _DefaultPSW);
	Build_Keys_Map();
}

void ControlKeysManager::Set_Default_SIM()
{
	Copy_Keys(&UserKeys, _DefaultSIM);
	Build_Keys_Map();
}







const std::string &ControlKeysManager::Get_Key_Name(const ControlKeyValueStruct &key)
{
	std::hash_map<unsigned int, ControlKeyDefStruct *>::iterator it = UserKeysMap.find(key);
	if (it == UserKeysMap.end()) return _Empty;
	FAIL_IF(!it->second) return _Empty;
	return it->second->Name;
}








// NOT EXACT: registers and the loop bounds (begin/end swapped, ~28 rows); four loop forms tried (attempts.csv)
void ControlKeysManager::Build_Keys_Map()
{
	UserKeysMap.clear();
	for (std::vector<KeyMappingStruct>::iterator it = UserKeys.begin(); it != UserKeys.end(); ++it) {
		for (int set = 0; set < NUM_MAPPING_SETS; set++) {
			unsigned int event = *(unsigned int *)&it->Keys[set];
			if (event) {

				G_ASSERT(UserKeysMap.find(event) == UserKeysMap.end());
				UserKeysMap[event] = it->DefPtr;
			}
		}
	}
}







bool ControlKeysManager::_Does_Key_Exist(const std::string &key_name)
{
	return _KeyDefinitions.find(key_name) != _KeyDefinitions.end();
}




















































































































































































void ControlKeysManager::Copy_Keys(std::vector<KeyMappingStruct> *to, const std::vector<KeyMappingStruct> &from)
{
	FAIL_IF(!to) return;
	FAIL_IF(from.size() != _NumKeyDefs) return;

	if (to->size() != _NumKeyDefs) {
		G_ASSERT(to->empty());
		to->resize(_NumKeyDefs);
	}
	std::vector<KeyMappingStruct>::iterator d = to->begin();
	std::vector<KeyMappingStruct>::const_iterator s = from.begin();
	for (unsigned int i = 0; i < _NumKeyDefs; i++) *d++ = *s++;
}





// NOT EXACT: shape, ~56 rows: the client calls 0x71E850 on the whole vector where our erase inlines a copy loop (attempts.csv)
void ControlKeysManager::Reset_Keys(std::vector<KeyMappingStruct> *keys)
{
	FAIL_IF(!_NumKeyDefs) return;



	if (keys->size() != _NumKeyDefs) {
		FAIL_IF(!keys->empty()) keys->erase(keys->begin(), keys->end());
		keys->resize(_NumKeyDefs);
	} else {
		for (std::vector<KeyMappingStruct>::iterator it = keys->begin(); it != keys->end(); ++it) { *(unsigned int *)&it->Keys[0] = 0; *(unsigned int *)&it->Keys[1] = 0; }
	}
	for (std::hash_map<std::string, std::auto_ptr<ControlKeyDefStruct> >::iterator it = _KeyDefinitions.begin(); it != _KeyDefinitions.end(); ++it) { ControlKeyDefStruct *def_ptr = it->second.get();
		FAIL_IF(!def_ptr) continue;
		FAIL_IF(def_ptr->Get_Index() >= _NumKeyDefs) continue;
		(*keys)[def_ptr->Get_Index()].DefPtr = def_ptr;
	}
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


int ControlKeyDefClass::Get_Unknown30() const
{
	FAIL_IF(!DefPtr) return -1;
	return DefPtr->Unknown30;
}

int ControlKeyDefClass::Get_Index() const
{
	FAIL_IF(!DefPtr) return -1;
	return DefPtr->Index;
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
