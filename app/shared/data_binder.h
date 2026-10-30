/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
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
#include "unknown_debug.h"

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
	// Remove 0x79F980 (0x1B4), each type 0x350 bytes further; Get_Value_Map 0x7A1050 (0xE6),
	// each 0x2C0 further; the value maps' find 0x7A0A10, find_or_insert 0x7A0EC0, erase
	// 0x7A0F80, resize 0x7A3B50 and constructor 0x7A2140 (libs/sgistl). The type map: the
	// default constructor 0x79DE00, the (n, hash, equal) constructor 0x79E6E0, its destructor
	// 0x79D2C0, find 0x7A1E60 / 0x7A2BE0, operator[] 0x7A1F30, erase 0x7A1C80. Has and Get
	// compare a const_iterator with end() (the client converts both iterators through
	// 0x7A0920, a 0x13-byte const_iterator(const iterator &), where they are inlined); Delete
	// passes Get and Remove fresh type objects (the client's two one-byte temporaries at
	// [esp+0xe] / [esp+0xf] in Remove_All 0x79D390, Has's in the dead argument's slot).
	// Delete is the client's: an inline function with the key by value (0x79D390 and the
	// Remove wrappers copy the key once, [esp+0x10]); the per-type Get (made on request) is
	// co_data.h's, since its T must be named (VC6 can't call a member template with an
	// explicit argument).

	// Unbinds and deletes the value bound to key; false when there's none
	template<class T>
	static bool Delete(KEY key, const DataBinderType<T> &type)  // NAME 2: what it does
	{
		DataBinderClass &binder = Instance();
		if (binder.Has(key, type)) {
			T *data = binder.Get(key, DataBinderType<T>());
			binder.Remove(key, DataBinderType<T>());
			delete data;
			return(true);
		}
		return(false);
	}
	// NOT EXACT: order-only (the client's 0x7A1050 takes the type name's length before the string's zero stores)
	template<class T>
	std::hash_map<KEY, T *> *Get_Value_Map(const DataBinderType<T> &) const  // NAME 2: what it does
	{
		TypeMapType::const_iterator it = Types.find(std::string(typeid(T *).name()));
		if (it != Types.end()) {
			return((std::hash_map<KEY, T *> *)(*it).second);
		}
		return(0);
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
	T *const &Get(const KEY &key, const DataBinderType<T> &type)  // NAME 2: what it does
	{
		std::hash_map<KEY, T *> *value_map = Get_Value_Map(type);

		G_ASSERT(value_map);
		std::hash_map<KEY, T *>::const_iterator it = value_map->find(key);
		G_ASSERT(it != value_map->end());
		return((*it).second);
	}

	template<class T>
	void Bind(const KEY &key, T *data)  // NAME 2: what it does
	{
		std::hash_map<KEY, T *> *value_map = (std::hash_map<KEY, T *> *)Types[std::string(typeid(T *).name())];
		if (value_map == 0) {
			value_map = new std::hash_map<KEY, T *>;
			Types[std::string(typeid(T *).name())] = value_map;
		}
		(*value_map)[key] = data;
	}

	// NOT EXACT: frame (0x28 vs the client's 0x2c: the client keeps value_map in esi and a stack slot, zero in ebx, and clears the erased name), 0x79F980 and the next four
	template<class T>
	void Remove(const KEY &key, const DataBinderType<T> &)  // NAME 2: what it does
	{
		TypeMapType::const_iterator it = Types.find(std::string(typeid(T *).name()));
		if (it != Types.end()) {
			std::hash_map<KEY, T *> *value_map = (std::hash_map<KEY, T *> *)(*it).second;
			if (value_map != 0) {
				value_map->erase(key);
				if (value_map->size() == 0) {
					Types.erase(std::string(typeid(T *).name()));
					delete value_map;
				}
			}
		}
	}

private:
	TypeMapType Types;  // +0x00  // NAME 2: what it holds
};

#endif //__DATA_BINDER_H
