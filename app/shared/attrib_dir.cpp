// Earth & Beyond client (reconstruction): app\shared\attrib_dir.cpp. Notes at the end of the file.
#define ENABLE_WWPROFILE
#include <ctype.h>
#include <algorithm>
#include "..\shared\unknown_debug.h"
#include "attrib_dir.h"
#include "wwprofile.h"

// libs/sgistl's out-of-class insert_unique_noresize returns pair<typename hashtable<...>::iterator, bool>,
// which VC6 refuses when it instantiates it (C2899): the attribute table's copy, SGI's body unchanged
typedef std::hashtable<std::pair<const std::string, AttributeInfo *>, std::string, std::hash<std::string>,
	std::_Select1st<std::pair<const std::string, AttributeInfo *> >, std::equal_to<std::string>,
	std::allocator<AttributeInfo *> > AttributeTable;

template <>
std::pair<AttributeTable::iterator, bool> AttributeTable::insert_unique_noresize(const value_type& __obj)
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








































AttribDir::AttribDir()
{
}








































// NOT EXACT: frame (explain.py: 8 vs 0x10), the client inlines each AttributeInfo's destructor into the delete loop; ours calls it
AttribDir::~AttribDir()
{
	for (AttributeMap::iterator it = Attributes.begin(); it != Attributes.end(); ++it) {
		delete (*it).second;
	}
	Attributes.clear();
}










































































// NOT EXACT: shape (explain.py: 9 vs 8 calls), as AttributeInfo::Attach_Mod: the client calls the list's insert 0x4D8F00 out of line and the value's construction
bool AttribDir::Attach_Mod(const char *name, AttribModClass *mod)
{
	WWPROFILE("AttribDir::Attach_Mod");
	FAIL_IF(!name || !mod) return false;
	return Get_Info(name)->Attach_Mod(mod);
}







bool AttribDir::Detach_Mod(const char *name, AttribModClass *mod)
{
	WWPROFILE("AttribDir::Detach_Mod");
	FAIL_IF(!name || !mod) return false;
	AttributeInfo *info = Find_Info(name);
	if (info == 0) {
		return false;
	}
	return info->Detach_Mod(mod);
}







// NOT EXACT: shape (explain.py: 24 vs 25 calls), the client inlines the handle's Is_Done / Get_Name and Find_Info with Make_Key called (frame 0x28; ours 0x3C)
AttribModIterator AttribDir::Detach_Mod(AttribModIterator &iter)
{
	WWPROFILE("AttribDir::Detach_Mod");
	FAIL_IF(iter.Is_Done()) return iter;
	AttributeInfo *info = Find_Info(iter.Get_Name());
	if (info == 0) {
		return iter;
	}
	AttribModClass *mod = iter.Get_Mod();
	AttribModIterator next(iter);
	next.Iterator->Lock_Next();
	AttributeInfo::ModList::iterator it = std::find(info->Mods.begin(), info->Mods.end(), mod);
	if (it != info->Mods.end()) {
		info->ModIterators.Advance(it);
		info->Mods.erase(it);
		info->Recalculate();
	}
	next.Iterator->Unlock();
	return next;
}















bool AttribDir::Attach_Callback(const char *name, AttribCallbackClass *cb)
{
	WWPROFILE("AttribDir::Attach_Callback");
	FAIL_IF(!name || !cb) return false;
	return Get_Info(name)->Attach_Callback(cb);
}




// NOT EXACT: inline budget (explain.py: shape, 10 vs 9 calls), the client inlines the iterator list's Advance, ours calls it
bool AttribDir::Detach_Callback(const char *name, AttribCallbackClass *cb)
{
	WWPROFILE("AttribDir::Detach_Callback");
	FAIL_IF(!name || !cb) return false;
	AttributeInfo *info = Find_Info(name);
	if (info == 0) {
		return false;
	}
	return info->Detach_Callback(cb);
}





bool AttribDir::Attach_Update(const char *src, const char *dst)
{
	WWPROFILE("AttribDir::Attach_Update");
	FAIL_IF(!src|| !dst) return false;
	AttributeInfo *src_info = Get_Info(src);
	AttributeInfo *dst_info = Get_Info(dst);
	return src_info->Attach_Update(dst_info);
}







bool AttribDir::Detach_Update(const char *src, const char *dst)
{
	WWPROFILE("AttribDir::Detach_Update");
	FAIL_IF(!src || !dst) return false;
	AttributeInfo *src_info = Find_Info(src);
	AttributeInfo *dst_info = Find_Info(dst);
	if (src_info == 0 || dst_info == 0) {
		return false;
	}
	return src_info->Detach_Update(dst_info);
}







// NOT EXACT: shape (explain.py: 4 rows), the client keeps the inlined Recalculate's result in bl as a bool; ours in eax, converted after (int Recalculate keeps 0x4D7060 exact)
bool AttribDir::Recalculate(const char *name)
{
	WWPROFILE("AttribDir::Recalculate");
	FAIL_IF(!name) return false;
	AttributeInfo *info = Find_Info(name);
	if (info == 0) {
		return false;
	}
	return info->Recalculate();
}







float AttribDir::Get_Value(const char *name) const
{
	WWPROFILE("AttribDir::Get_Value");
	FAIL_IF(!name) return 0;
	AttributeInfo *info = Find_Info(name);
	if (info == 0) {
		return 0;
	}
	return info->Value;
}




float AttribDir::Get_Base(const char *name) const
{
	WWPROFILE("AttribDir::Get_Base");
	FAIL_IF(!name) return 0;
	AttributeInfo *info = Find_Info(name);
	if (info == 0) {
		return 0;
	}
	return info->Get_Base();
}




AttribShadowClass AttribDir::Get_Shadow(const char *name)
{
	WWPROFILE("AttribDir::Get_Shadow");
	FAIL_IF(!name) return AttribShadowClass();
	return AttribShadowClass(Get_Info(name));
}








































AttributeInfo *AttribDir::Get_Info(const char *name)
{
	std::string key = Make_Key(name);
	AttributeMap::iterator find = Attributes.find(key);
	if (find == Attributes.end()) {
		AttributeInfo *info = new AttributeInfo(name);
		find = Attributes.insert(std::make_pair(std::string(key), info)).first;
	}
	return (*find).second;
}








































AttributeInfo *AttribDir::Find_Info(const char *name)
{
	std::string key = Make_Key(name);
	AttributeMap::iterator find = Attributes.find(key);
	if (find == Attributes.end()) {
		return 0;
	}
	return (*find).second;
}














AttributeInfo *AttribDir::Find_Info(const char *name) const
{
	std::string key = Make_Key(name);
	AttributeMap::const_iterator find = Attributes.find(key);
	if (find == Attributes.end()) {
		return 0;
	}
	return (*find).second;
}

std::string AttribDir::Make_Key(const char *name) const
{
	std::string key(name);
	for (std::string::iterator it = key.begin(); it != key.end(); ++it) {
		*it = toupper(*it);
	}
	return key;
}

AttributeInfo::AttributeInfo(const char *name) : Name(name), Updates(100), Value(0)
{
}

// NOT EXACT: inline budget (explain.py), the client inlines the update table's bucket vector destructor and keeps &ModIterators in edi (frame 0x14 vs 0x10)
AttributeInfo::~AttributeInfo()
{
	ModIterators.Invalidate();
	CallbackIterators.Invalidate();
}

// NOT EXACT: inline budget (explain.py), the client calls the list node value's construction 0x4D9FB0 out of line; ours inlines it
bool AttributeInfo::Attach_Mod(AttribModClass *mod)
{
	Mods.push_front(mod);
	Apply_Mod(mod);
	Update_Value();
	return true;
}

bool AttributeInfo::Detach_Mod(AttribModClass *mod)
{
	ModList::iterator it = std::find(Mods.begin(), Mods.end(), mod);
	if (it == Mods.end()) {
		return false;
	}
	ModIterators.Advance(it);
	Mods.erase(it);
	Recalculate();
	return true;
}

bool AttributeInfo::Attach_Callback(AttribCallbackClass *cb)
{
	Callbacks.push_front(cb);
	return true;
}

bool AttributeInfo::Detach_Callback(AttribCallbackClass *cb)
{
	CallbackList::iterator it = std::find(Callbacks.begin(), Callbacks.end(), cb);
	if (it == Callbacks.end()) {
		return false;
	}
	CallbackIterators.Advance(it);
	Callbacks.erase(it);
	return true;
}

bool AttributeInfo::Attach_Update(AttributeInfo *dst)
{
	Updates[dst]++;
	return true;
}

bool AttributeInfo::Detach_Update(AttributeInfo *dst)
{
	std::hash_map<AttributeInfo *, int, AttributeInfoHash>::iterator it = Updates.find(dst);
	if (it == Updates.end()) {
		return false;
	}
	(*it).second--;
	if ((*it).second == 0) {
		Updates.erase(it);
	}
	return true;
}








void AttributeInfo::Apply_Mod(AttribModClass *mod)
{
	switch (mod->Get_Type()) {
		case 0:
			Values.BaseMin = std::_cpp_max(Values.BaseMin, mod->Get_Value());
			break;
		case 1:
			Values.BaseMax = std::_cpp_min(Values.BaseMax, mod->Get_Value());
			break;
		case 2:
			Values.Add += mod->Get_Value();
			break;
		case 3:
			Values.Percent += mod->Get_Value();
			break;
		case 4:
			Values.Min = std::_cpp_max(Values.Min, mod->Get_Value());
			break;
		case 5:
			Values.Max = std::_cpp_min(Values.Max, mod->Get_Value());
			break;
		default:
			G_ASSERT(!"AttributeInfo::Apply_Mod - Bad mod type!");
			break;
	}
}








































float AttributeInfo::Get_Base()
{
	float base = std::_cpp_max(Values.BaseMin, 0.0f);
	base = std::_cpp_min(Values.BaseMax, base);
	return base;
}






float AttributeInfo::Calculate()
{
	float value = std::_cpp_max(Values.BaseMin, 0.0f);
	value = std::_cpp_min(Values.BaseMax, value);
	value = value + (value * Values.Percent * 0.01f + Values.Add);
	value = std::_cpp_max(Values.Min, value);
	value = std::_cpp_min(Values.Max, value);
	return value;
}

int AttributeInfo::Recalculate()
{
	Values = AttribModValuesStruct();
	for (ModList::iterator it = Mods.begin(); it != Mods.end(); ++it) {
		Apply_Mod(*it);
	}
	return Update_Value();
}

inline int AttributeInfo::Update_Value()
{
	float old_value = Value;
	float value = Calculate();
	if (old_value != value) {
		Value = value;
		Notify();
		return 1;
	}
	return false;
}

// NOT EXACT: inline budget (explain.py: shape), the client inlines the update table's begin() (ours calls it; frame 0x2C vs 0x28); the callback loop is the client's
void AttributeInfo::Notify()
{
	float value = Value;
	for (CallbackList::iterator it = Callbacks.begin(); it != Callbacks.end(); ) {
		AttribCallbackClass *cb = *it;
		++it;
		CallbackIterators.push_back(&it);
		cb->Value_Changed(Name.c_str(), value);
		bool ok = CallbackIterators.Pop();
		G_ASSERT(ok);
	}
	for (std::hash_map<AttributeInfo *, int, AttributeInfoHash>::iterator u = Updates.begin(); u != Updates.end(); ++u) {
		(*u).first->Recalculate();
	}
}



































AttribModValuesStruct::AttribModValuesStruct() : BaseMin(ATTRIB_MIN_VALUE), BaseMax(ATTRIB_MAX_VALUE), Add(0), Percent(0), Min(ATTRIB_MIN_VALUE), Max(ATTRIB_MAX_VALUE)
{
}

AttribShadowClass::AttribShadowClass() : Info(0)
{
}

AttribShadowClass::AttribShadowClass(AttributeInfo *info) : Info(info)
{
}

float AttribShadowClass::Get_Value()
{
	FAIL_IF(!Info) return 0;
	return Info->Value;
}

float AttribShadowClass::Get_Base()
{
	FAIL_IF(!Info) return 0;
	return Info->Get_Base();
}


























AttribModValueClass::AttribModValueClass(int type, float value, void *owner) : Type(type), Value(value), Owner(owner)
{
}

























AttribModIterator AttribDir::Get_All_Mods()
{
	return AttribModIterator(new AttribDirModIteratorClass(this));
}


























































AttribModIterator AttribDir::Get_Mods(const char *name)
{
	FAIL_IF(!name) return AttribModIterator();
	AttributeInfo *info = Find_Info(name);
	if (info == 0) {
		return AttribModIterator();
	}
	return AttribModIterator(new AttribInfoModIteratorClass(info));
}

























AttribModIterator::AttribModIterator() : Iterator(0)
{
}


















AttribModIterator::AttribModIterator(AttribModIteratorClass *iterator) : Iterator(iterator)
{
}

AttribModIterator::AttribModIterator(const AttribModIterator &src) : Iterator(0)
{
	*this = src;
}

AttribModIterator::~AttribModIterator()
{
	delete Iterator;
	Iterator = 0;
}

AttribModIterator &AttribModIterator::operator=(const AttribModIterator &src)
{
	if (&src != this) {
		delete Iterator;
		Iterator = src.Iterator ? src.Iterator->Clone() : 0;
	}
	return *this;
}

AttribModClass *AttribModIterator::Get_Mod()
{
	return Iterator ? Iterator->Get_Mod() : 0;
}

const char *AttribModIterator::Get_Name()
{
	return Iterator ? Iterator->Get_Name() : 0;
}

bool AttribModIterator::Is_Done()
{
	return Iterator ? Iterator->Is_Done() : true;
}

void AttribModIterator::Next()
{
	if (Iterator) {
		Iterator->Next();
	}
}

void AttribInfoModIteratorClass::Lock_Next()
{
	++Cur;
	Info->ModIterators.push_back(&Cur);
}

void AttribInfoModIteratorClass::Unlock()
{
	bool ok = Info->ModIterators.Pop_Back();
	G_ASSERT(ok);
}






















AttribDirModIteratorClass::AttribDirModIteratorClass(AttribDir *dir) : MapCur(dir->Attributes.begin()), MapEnd(dir->Attributes.end())
{
	Skip_Empty();
}

void AttribDirModIteratorClass::Skip_Empty()
{
	while (MapCur != MapEnd) {
		ModCur = (*MapCur).second->Mods.begin();
		ModEnd = (*MapCur).second->Mods.end();
		if (ModCur != ModEnd) {
			return;
		}
		++MapCur;
	}
}

// NOT EXACT: shape (explain.py: 8 masked rows), registers swapped after ++ModCur and the client tests MapCur != MapEnd twice before the inlined Skip_Empty
void AttribDirModIteratorClass::Next()
{
	++ModCur;
	if (ModCur == ModEnd) {
		++MapCur;
		Skip_Empty();
	}
}


void AttribDirModIteratorClass::Lock_Next()
{
	FAIL_IF(MapCur == MapEnd) return;
	++ModCur;
	(*MapCur).second->ModIterators.push_back(&ModCur);
}










// NOT EXACT: inline budget (explain.py: 4 vs 5 calls), the client calls the bucket function out of line in the inlined ++MapCur
void AttribDirModIteratorClass::Unlock()
{
	FAIL_IF(MapCur == MapEnd) return;


	bool ok = (*MapCur).second->ModIterators.Pop_Back();

	G_ASSERT(ok);
	if (ModCur == ModEnd) {
		++MapCur;
		Skip_Empty();
	}
}


/*
**	Reconstruction notes
**
**	client.exe 0x4D42C0-0x4D7730 rebuilt here (the object runs to 0x4DAB40, attrib_dir_collective.cpp
**	follows): AttribDir's constructor and destructor, its API on the client's lines 200-371 (D25; the
**	profile labels 'AttribDir::Attach_Mod' ... are its WWPROFILEs), Get_Info / Find_Info (two copies,
**	0x4D5F10 and 0x4D6070) / Make_Key, AttributeInfo's constructor, destructor and methods (Apply_Mod
**	on line 603, Notify's G_ASSERT(ok) on 700), AttribShadowClass (755 / 761), and the list, vector and
**	hashtable instances; the out-of-line constructors 0x4D74D0 / 0x4D7510 / 0x4D7530 / 0x4D7620 (no
**	caller: defined in this file); the mod iterators: the interface (vtable 0xAEC20C, every slot pure),
**	one attribute's (0xAEC230, Unlock's G_ASSERT(ok) on 990), the directory's (0xAEC1E8, the FAIL_IFs
**	'MapCur == MapEnd' on 1044 / 1061 and G_ASSERT(ok) on 1066), the handle AttribModIterator
**	(0x4D7C90-0x4D7E00), Get_All_Mods 0x4D77D0, Get_Mods 0x4D7A20 (882) and Detach_Mod by iterator
**	0x4D4BD0 (231). Unlock pops with pop_back, Notify with erase(end() - 1) (the client's two forms).
**	Forms: -FLT_MAX / FLT_MAX are named consts (the client loads them from 0xAEC1A0 / 0xAEC1A4); min and
**	max are VC6's reference-returning std::_cpp_min / _cpp_max with the bound first; the value is
**	value + (value * Percent * 0.01f + Add) (other spellings fold to value * (Percent * 0.01f + 1));
**	Update_Value keeps the old value before calculating (the client loads it first); Make_Key is a
**	member (the client passes this) and Find_Info keeps the key in a named local; the iterator lists'
**	Pop erases end() - 1 (VC6's pop_back has no copy loop); Recalculate returns int.
**	libs/sgistl's insert_unique_noresize needs the explicit specialization at the top (C2899).
*/
