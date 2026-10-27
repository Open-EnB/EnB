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

	// The value bound to key, or (create) a new T made for the key
	template<class T>
	static T *Get(KEY key, bool create, const DataBinderType<T> &type)  // NAME 2: what it does
	{
		DataBinderClass &binder = Instance();
		if (binder.Has(key, type)) {
			return(binder.Get(key, type));
		}
		if (!create) {
			return(0);
		}
		T *data = new T(key);
		binder.Bind(key, data);
		return(data);
	}

	// Unbinds and deletes the value bound to key; false when there's none
	template<class T>
	static bool Delete(KEY key, const DataBinderType<T> &type)  // NAME 2: what it does
	{
		DataBinderClass &binder = Instance();
		if (binder.Has(key, type)) {
			T *data = binder.Get(key, type);
			binder.Remove(key, type);
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

	// NOT EXACT: inline budget in emote's 0x7A05A0 only (the client calls the bucket count out of line); radar's 0x79F860 and the next three are exact
	template<class T>
	bool Has(const KEY &key, const DataBinderType<T> &type) const  // NAME 2: what it does
	{
		std::hash_map<KEY, T *> *value_map = Get_Value_Map(type);
		if (value_map == 0) {
			return(false);
		}
		return(value_map->find(key) != value_map->end());
	}

	template<class T>
	T *&Get(const KEY &key, const DataBinderType<T> &type)  // NAME 2: what it does
	{
		std::hash_map<KEY, T *> *value_map = Get_Value_Map(type);

		G_ASSERT(value_map);
		std::hash_map<KEY, T *>::iterator it = value_map->find(key);
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

	// NOT EXACT: frame (0x28 vs the client's 0x2c; ours inlines the first type name's assign), 0x79F980 and the next four
	template<class T>
	void Remove(const KEY &key, const DataBinderType<T> &)  // NAME 2: what it does
	{
		TypeMapType::iterator it = Types.find(std::string(typeid(T *).name()));
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
