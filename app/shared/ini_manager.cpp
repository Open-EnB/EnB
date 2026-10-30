// Earth & Beyond client (reconstruction): app\shared\ini_manager.cpp. Notes at the end of the file.
#include "unknown_debug.h"
#include "ini_manager.h"

// libs/sgistl's out-of-class insert_unique_noresize returns pair<typename hashtable<...>::iterator, bool>,
// which VC6 refuses when it instantiates it (C2899): the definitions table's copy, SGI's body unchanged
typedef std::hashtable<std::pair<const std::string, DefinitionRefCount<INIClass> *>, std::string, std::hash<std::string>,
	std::_Select1st<std::pair<const std::string, DefinitionRefCount<INIClass> *> >, std::equal_to<std::string>,
	std::allocator<DefinitionRefCount<INIClass> *> > INIDefinitionTable;

template <>
std::pair<INIDefinitionTable::iterator, bool> INIDefinitionTable::insert_unique_noresize(const value_type& __obj)
{
  const size_type __n = _M_bkt_num(__obj);
  _Node* __first = _M_buckets[__n];

  for (_Node* __cur = __first; __cur; __cur = __cur->_M_next) 
    if (_M_equals(_M_get_key(__cur->_M_val), _M_get_key(__obj)))
      return pair<iterator, bool>(iterator(__cur, this), false);

  _Node* __tmp = _M_new_node(__obj);
  __tmp->_M_next = __first;
  _M_buckets[__n] = __tmp;
  ++_M_num_elements;
  return pair<iterator, bool>(iterator(__tmp, this), true);
}

INIManagerClass::INIManagerClass()
{
}

INIClass *INIManagerClass::Create(const std::string &name)
{
	return new INIClass(name.c_str());
}

void INIManagerClass::Destroy(INIClass *ini)
{
	delete ini;
}

struct INIManagerHolder
{
	INIManagerHolder() { Manager = Create_NEW(new INIManagerClass); }
	~INIManagerHolder() {}
	RefCountPtr<DefinitionManagerClass<INIClass> > Manager;
};

// NOT EXACT: shape (explain.py: 53 masked rows), the holder's new and the Create_Peek temporary take other registers and frame slots; objmatch would also reject the atexit handler's name $E18 (symbols.json gives it to other objects)
DefinitionAutoPtr<INIClass> INIManagerClass::Get_INI(const std::string &name)
{
	DefinitionAutoPtr<INIClass> ini;
	static INIManagerHolder holder;
	ini.Set(Create_Peek(holder.Manager.Peek()), name);
	return ini;
}
bool INIManagerClass::Split_Name(const std::string &name, std::string *ini_name, std::string *ini_section)
{
	FAIL_IF(!ini_name) return false;
	FAIL_IF(!ini_section) return false;
	std::string::size_type pos = name.find(":");
	if (pos == std::string::npos) {
		*ini_section = name;
		return false;
	}
	*ini_name = name.substr(0, pos - 1);
	*ini_name += ".ini";
	*ini_section = name.substr(pos + 1);
	return ini_name->size() != 0;
}


/*
**	Reconstruction notes
**
**	client.exe 0x4E9D40-0x4EC260 (unknown_new.cpp's delete 0x4E9D10 before it, lookup_table.cpp's
**	_LookupTables initializer after it): INIManagerClass's Create 0x4E9D40 and Destroy 0x4E9DD0 (its
**	vtable 0xAEC6CC; the base's 0xAEC6E0 has a pure Create and a Destroy asserting false, line 105 of
**	definition_manager.h), Get_INI 0x4E9E00 (a function-static holder 0xC416D8, guard 0xC416CC,
**	default-constructed and then assigned: the client's EH states and its atexit handler 0x4EA9E0
**	calling the destructor 0x4EAB00 out of line), the constructor 0x4EA080 (defined here: the client
**	calls it), the destructors 0x4EA2B0 / 0x4EAFB0, Split_Name 0x4EA5A0 (the FAIL_IFs on lines 59 /
**	60, D25; 'file:section', the name cut one character before the colon as the client does), then
**	the template instances of definition_manager.h (Acquire 0x4EB570, line 133; Remove 0x4EB730,
**	line 212; the table's find, erase, resize, insert_unique_noresize, iterator ++ and begin) and
**	definition_auto_ptr.h (the destructor 0x4EA850 and Release 0x4EB1B0, lines 119 / 120). Forms:
**	the auto pointer's erase(0, -1) (the client pushes -1, not npos from memory); Acquire's pair p
**	declared before the insert (the client's frame holds the returned pair and p).
*/
