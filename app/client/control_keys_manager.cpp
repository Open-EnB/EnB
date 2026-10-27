// Earth & Beyond client (reconstruction): app\Client\control_keys_manager.cpp. Notes at the end of the file.
#include "control_keys_manager.h"
#include "..\shared\unknown_debug.h"
#include "..\..\wwlib\ini.h"
#include "..\..\wwlib\inisup.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *Unknown_8B44E0(__int64 value, char *buffer);	// 0x8B44E0: _i64toa(value, buffer, 10), as pkguildplayeronline.h  // NAME 1: neutral placeholder (D10)
ControlKeyComboStruct Unknown_42A1A0(INIClass &ini, const char *section, const char *entry);	// NAME 1 [Unknown_42A1A0]: neutral (D10): 0x42A1A0 (app\input), a key read from an ini entry
void Unknown_42A410(INIClass &ini, const char *section, const char *entry, const ControlKeyComboStruct &key);	// NAME 1 [Unknown_42A410]: neutral (D10): 0x42A410 (app\input), a key written to an ini entry
std::string &Unknown_42B7E0(const ControlKeyComboStruct &key, std::string &out);	// NAME 1 [Unknown_42B7E0]: neutral (D10): 0x42B7E0, the key's text

struct ContextEqualStruct  // NAME 2: describes what it does (the SGI documentation's eqstr; strcmp inlined in 0x71ECD0)
{
	bool operator()(const char *a, const char *b) const { return strcmp(a, b) == 0; }
};

static std::string _Empty;  // NAME 2: 0xBE7B80, what the accessors return without a definition
static std::hash_map<unsigned int, ControlKeysManager> _Instances;  // 0xBE7BE0: the client's asserts, one manager per SClient
unsigned int ControlKeysManager::_NumKeyDefs;
std::hash_map<std::string, std::auto_ptr<ControlKeyDefStruct> > ControlKeysManager::_KeyDefinitions;
std::hash_map<std::string, KeyIdStruct> ControlKeysManager::_KeyIds;
std::vector<KeyMappingStruct> ControlKeysManager::_DefaultPSW;
std::vector<KeyMappingStruct> ControlKeysManager::_DefaultSIM;
std::vector<KeyMappingStruct> ControlKeysManager::_DefaultUser;


































bool ControlKeysManager::Create(ControlKeysSClientClass *client)
{
	FAIL_IF(!client) return false;

	std::hash_map<unsigned int, ControlKeysManager>::iterator manager_it = _Instances.find((unsigned int)client);
	FAIL_IF(manager_it != _Instances.end()) return false;
	ControlKeysManager &manager = _Instances[(unsigned int)client]; manager.Init(client);
	return true;
}





bool ControlKeysManager::Destroy(ControlKeysSClientClass *client)
{
	std::hash_map<unsigned int, ControlKeysManager>::iterator manager_it = _Instances.find((unsigned int)client);
	FAIL_IF(manager_it == _Instances.end()) return false;
	return true;
}

ControlKeysManager *ControlKeysManager::Get(ControlKeysSClientClass *client)
{
	std::hash_map<unsigned int, ControlKeysManager>::iterator manager_it = _Instances.find((unsigned int)client);
	if (manager_it == _Instances.end()) return NULL;
	return &manager_it->second;
}










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


// NOT EXACT: inline budget (ours inlines Get_Key_Id 0x71A190, which the client calls)
int ControlKeysManager::Get_Key_Id(ControlKeysSClientClass *client, const std::string &module_name, const ControlKeyValueStruct &key)
{
	ControlKeysManager *keys_manager = Get(client); FAIL_IF(!keys_manager) return -1;
	return Get_Key_Id(keys_manager, module_name, key);
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

void ControlKeysManager::Save()
{
	Save_Keys(&UserKeys, PlayerID);
	Build_Keys_Map();
	Get_Signal_Map().emit(this, KeysSavedEvent());
	Copy_Keys(&RestoreKeys, UserKeys);
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

SignalMap &ControlKeysManager::Get_Signal_Map()
{
	static SignalMap signal_map; return signal_map;
}


bool ControlKeysManager::_Does_Key_Exist(const std::string &key_name)
{
	return _KeyDefinitions.find(key_name) != _KeyDefinitions.end();
}



// NOT EXACT: inline budget, ~90 rows: the client calls hash_map() 0x71DCE0, First_Valid 0x71BCC0 and two pair ctors 0x71ED90 out of line, ours inlines them (attempts.csv)
void ControlKeysManager::Load_Definitions()
{
	FAIL_IF(_NumKeyDefs) return;

	std::hash_map<const char *, int, std::hash<const char *>, ContextEqualStruct> contexts;
	contexts["Targeting"] = 0;
	contexts["Navigation"] = 1;
	contexts["Camera"] = 2;
	contexts["Activation"] = 3;
	contexts["Chat"] = 4;
	contexts["General"] = 5;

	INIClass ini("control_keys.ini");
	List<INISection *> &list = ini.Get_Section_List();
	INISection *section = list.First_Valid();
	int count = list.Get_Valid_Count();
	_DefaultPSW.resize(count);
	_DefaultSIM.resize(count);

	INISection *next_section;
	for (; section; section = next_section) {




		FAIL_IF(_NumKeyDefs >= _DefaultPSW.size()) break;
		FAIL_IF(_NumKeyDefs >= _DefaultSIM.size()) break;

		const char *section_name = section->Section;
		next_section = section->Next_Valid();



		FAIL_IF(!section_name) break;
		if (*section_name == '-') continue;

		char text_str[256];
		ini.Get_String(section_name, "Text", "", text_str, sizeof(text_str));



		FAIL_IF(!text_str[0]) continue;

		char context_str[256];
		ini.Get_String(section_name, "Context", "", context_str, sizeof(context_str));
		int context = 5;
		std::hash_map<const char *, int, std::hash<const char *>, ContextEqualStruct>::iterator it = contexts.find(context_str);
		if (it != contexts.end()) {
			context = it->second;
		} else {


			G_ASSERT(false && "Context is not valid");
		}

		char key_str[256];
		ini.Get_String(section_name, "KEY", "", key_str, sizeof(key_str));
		std::auto_ptr<ControlKeyDefStruct> def_ptr(new ControlKeyDefStruct(section_name, text_str, key_str, context, _NumKeyDefs));
		ControlKeyDefStruct *def = def_ptr.get();
		_KeyDefinitions[def->Name] = def_ptr;
		KeyMappingStruct &psw = _DefaultPSW[_NumKeyDefs]; KeyMappingStruct &sim = _DefaultSIM[_NumKeyDefs]; psw.DefPtr = sim.DefPtr = def;
		if (!key_str[0]) {
			psw.Keys[0] = Unknown_42A1A0(ini, section_name, "PSW_1");
			psw.Keys[1] = Unknown_42A1A0(ini, section_name, "PSW_2");
			sim.Keys[0] = Unknown_42A1A0(ini, section_name, "SIM_1");
			sim.Keys[1] = Unknown_42A1A0(ini, section_name, "SIM_2");
		}
		_NumKeyDefs++;
	}

	_DefaultPSW.resize(_NumKeyDefs);
	_DefaultSIM.resize(_NumKeyDefs);
	Load_Keys(&_DefaultUser, NULL);
}















void ControlKeysManager::Load_Keys(std::vector<KeyMappingStruct> *keys, const char *section)
{
	FAIL_IF(!keys) return;

	INIClass ini("keymap.ini");
	char user[0x18];
	if (!section) {
		ini.Get_String("Default", "User", "", user, sizeof(user));
		section = user;
	}

	if (!ini.Is_Present(section)) {
		if (keys == &_DefaultUser) {
			Copy_Keys(keys, _DefaultPSW);
		} else {
			Copy_Keys(keys, _DefaultUser);
		}
		return;
	}

	Reset_Keys(keys);
	int count = ini.Entry_Count(section);
	for (int index = 0; index < count; index++) {
		const char *entry = ini.Get_Entry(section, index);





		FAIL_IF(!entry) continue;

		const char *key_ptr = entry;
		while (*key_ptr != '_') {

			FAIL_IF(!*key_ptr) break;
			key_ptr++;
		}


		FAIL_IF(!*key_ptr) continue;
		key_ptr++;
		FAIL_IF(!*key_ptr) continue;

		std::hash_map<std::string, std::auto_ptr<ControlKeyDefStruct> >::const_iterator it = _KeyDefinitions.find(key_ptr);
		if (it == _KeyDefinitions.end()) continue;

		ControlKeyDefStruct *def = it->second.get();

		FAIL_IF(!def) continue;

		FAIL_IF(def->Get_Index() >= keys->size()) continue;

		int set = atoi(entry) - 1;

		FAIL_IF(set < 0) continue;
		(*keys)[def->Get_Index()].Keys[set] = Unknown_42A1A0(ini, section, entry);
	}
}

// NOT EXACT: shape, 12 rows: the client keeps the 1-based set count in a register (inc at the loop top), ours in a frame slot (attempts.csv)
void ControlKeysManager::Save_Keys(std::vector<KeyMappingStruct> *keys, const char *section)
{
	FAIL_IF(!keys) return;
	FAIL_IF(!section) return;

	INIClass ini("keymap.ini");
	ini.Clear(section);
	ini.Put_String("Default", "User", section);
	for (std::vector<KeyMappingStruct>::iterator it = keys->begin(); it != keys->end(); ++it) {
		for (int set = 1; set <= NUM_MAPPING_SETS; set++) {
			char buffer[32];
			sprintf(buffer, "%d_", set);
			std::string entry(buffer);
			entry += it->Get_Name();
			Unknown_42A410(ini, section, entry.c_str(), ControlKeyComboStruct(it->Keys[set - 1].Key, it->Keys[set - 1].Modifiers));
		}
	}
	ini.Save("keymap.ini");
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
**
**	keymap.ini (Load_Keys 0x71BD90, Save_Keys 0x71C170): one section per player id ("1_Name" /
**	"2_Name" entries, 0x42A1A0 / 0x42A410 read and write a key), [Default] User names the section
**	Load_Definitions loads into _DefaultUser (0xBE7BF8). control_keys.ini (Load_Definitions
**	0x71B1F0): one section per key definition (Text, Context, KEY, PSW_1/2, SIM_1/2; a name starting
**	with '-' is skipped). Save 0x71AD20 emits KeysSavedEvent on the SignalMap 0x71B0F0 returns.
**	Left unbuilt: the _Instances destructor 0x7198B0 and 0x71BCE0 (P-H11-2).
*/
