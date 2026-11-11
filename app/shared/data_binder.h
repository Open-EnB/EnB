// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\data_binder.h (the client's asserts
**	'..\shared/data_binder.h', lines 99 and 101, in co_data.cpp's instances)
**
**	A registry that binds data objects of any type to a key: one SGI hash_map (libs/sgistl, D87)
**	per data type, keyed by the type's typeid name, each holding the type's objects by key. The
**	client's only user is co_data.cpp (the client objects' radar, gutter, starmap, navigation and
**	emote data, keyed by the object). The asserts' names are the client's (value_map, it); the
**	rest is named from what it does (D10, D24); the asserts sit on the client's lines.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __DATA_BINDER_H  // NAME 5 [data_binder.h]: the client's assert path
#define __DATA_BINDER_H

#include <string>
#include <typeinfo.h>
#include <hash_map.h>
#include "g_assert.h"

// The binder's member templates take their type as an empty object passed by reference
template<class T>
struct DataBinderType  // NAME 1: neutral (D10), an empty tag type
{
};

template<class KEY>
class DataBinderClass  // NAME 3: by convention (D24) from the client's data_binder.h
{
public:
	typedef std::hash_map<std::string, void *> TypeMapType;  // NAME 2: what it holds

	static DataBinderClass &Instance();  // the client's 0x79D250 (co_data.cpp): a function-local static  // NAME 2: what it does

	// The client's instances (co_data.cpp, KEY = ClientObjectClass *), per type in the order
	// radar, gutter, starmap, navigation, emote: Has 0x79F860 (0x55 bytes), Get 0x79F8E0 (0x77),
	// Remove 0x79F980 (0x1B4), each type 0x350 bytes further (byte-exact); the const Get_Value_Map 0x7A1050
	// (0xE6), each 0x2C0 further; the value maps' find 0x7A0A10, find_or_insert 0x7A0EC0, erase
	// 0x7A0F80, resize 0x7A3B50 and constructor 0x7A2140 (libs/sgistl). The type map: the
	// default constructor 0x79DE00, the (n, hash, equal) constructor 0x79E6E0, its destructor
	// 0x79D2C0, the const find 0x7A1E60 (Has and Get, through the const Get_Value_Map), the
	// other find 0x7A2BE0 (Remove only: its Get_Value_Map is the non-const overload, inlined, which
	// is why Remove's first type name string calls _Tidy and assign where its second, a level
	// higher, expands them), operator[] 0x7A1F30, erase 0x7A1C80. Has and Get compare a
	// const_iterator with end() (the client converts both iterators through 0x7A0920, a
	// 0x13-byte const_iterator(const iterator &), where they are inlined). The client emits
	// Has, Get and Remove of a type together and before the value map's members: they are first
	// named in one function body, co_data.h's Remove (so Remove is compiled before the bucket
	// vector's _Destroy, which keeps the erased name's dead stores and its EH state store).
	// The per-type access (made on request, deleted on removal) is co_data.h's, since its T
	// must be named (VC6 can't call a member template with an explicit argument).










	// The type's map, 0 when no value of the type is bound. 0x7A1050 (radar) and the next four: byte-exact
	// (the name in a local and its implicit conversion: the client takes its length before the string's zero stores)
	template<class T>
	std::hash_map<KEY, T *> *Get_Value_Map(const DataBinderType<T> &) const  // NAME 2: what it does
	{
		const char *name = typeid(T *).name();
		TypeMapType::const_iterator it = Types.find(name);
		if (it == Types.end()) {
			return(0);
		}
		return((std::hash_map<KEY, T *> *)(*it).second);
	}

	// Has 0x79F860 (radar) and the next four: byte-exact
	template<class T>
	bool Has(const KEY &key, const DataBinderType<T> &type) const  // NAME 2: what it does
	{
		std::hash_map<KEY, T *> *value_map = Get_Value_Map(type);
		if (value_map == 0) {
			return(false);
		}
		std::hash_map<KEY, T *>::const_iterator it = value_map->find(key);
		return(it != value_map->end());
	}

	template<class T>
	T *const &Get(const KEY &key, const DataBinderType<T> &type) const  // NAME 2: what it does
	{
		std::hash_map<KEY, T *> *value_map = Get_Value_Map(type);

		G_ASSERT(value_map);
		std::hash_map<KEY, T *>::const_iterator it = value_map->find(key);
		G_ASSERT(it != value_map->end());
		return((*it).second);
	}

	// Binds data to key. The dead size() read is not the client's (D130): it keeps the value map's pair
	// constructor 0x7A0A80 out of line in co_data.h's Get, as the client has it
	template<class T>
	void Bind(const KEY &key, T *data)  // NAME 2: what it does
	{
		std::hash_map<KEY, T *> *value_map = Make_Value_Map(DataBinderType<T>());
		(*value_map)[key] = data;
		Types.size();
	}

	// The type's map, made when the type has none (inline in co_data.h's Get: the client takes the name once)
	template<class T>
	std::hash_map<KEY, T *> *Make_Value_Map(const DataBinderType<T> &)  // NAME 2: what it does
	{
		const char *name = typeid(T *).name();
		std::hash_map<KEY, T *> *value_map = (std::hash_map<KEY, T *> *)Types[name];
		if (value_map == 0) {
			value_map = new std::hash_map<KEY, T *>;
			Types[name] = value_map;
		}
		return(value_map);
	}

	// Unbinds the value bound to key (the caller deletes it); the type's map goes with its last value
	template<class T>
	void Remove(const KEY &key, const DataBinderType<T> &type)  // NAME 2: what it does
	{
		std::hash_map<KEY, T *> *value_map = Get_Value_Map(type);
		if (value_map != 0) {
			value_map->erase(key);
			if (value_map->size() == 0) {
				Types.erase(std::string(typeid(T *).name()));
				delete value_map;
			}
		}
	}

	template<class T>
	std::hash_map<KEY, T *> *Get_Value_Map(const DataBinderType<T> &)  // NAME 2: what it does
	{
		const char *name = typeid(T *).name();
		TypeMapType::iterator it = Types.find(name);
		if (it == Types.end()) {
			return(0);
		}
		return((std::hash_map<KEY, T *> *)(*it).second);
	}

private:
	TypeMapType Types;  // +0x00  // NAME 2: what it holds
};

#endif //__DATA_BINDER_H
