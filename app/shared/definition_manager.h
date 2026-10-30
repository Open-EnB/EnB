// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\definition_manager.h (the client's __FILE__ of
**	its asserts, through ini_manager.cpp)
**
**	A reference-counting cache of named definitions: Acquire(name) creates a definition once
**	(the derived class's Create) and counts its users, Release(name) drops one; a definition no
**	longer used is destroyed (the derived class's Destroy) at once when Unknown08 is 1 or its own flag is
**	set, otherwise when the manager frees the unused ones (Free_Unused, the destructor). The
**	definitions are kept by name in an SGI hash_map (libs/sgistl, D87). The client names
**	Definitions, drc->RefCount and the iterators (ref_it, p) through its asserts.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __DEFINITION_MANAGER_H
#define __DEFINITION_MANAGER_H

#include <string>
#include <hash_map.h>
#include "..\..\wwlib\refcount.h"
#include "unknown_debug.h"

// One cached definition and its users (12 bytes)
template <class T>
struct DefinitionRefCount  // NAME 3: from the client's 'drc->RefCount'
{
	DefinitionRefCount(T *definition) : Definition(definition), RefCount(0), Unknown08(false) {}

	T *Definition;			// +0x00  // NAME 2: what it holds
	int RefCount;			// +0x04
	bool Unknown08;		// +0x08: destroy as soon as it is unused  // NAME 1: neutral placeholder (D10)
};

// The cache (0x24 bytes; vtable slots: Delete_This, the destructor, Create, Destroy)
template <class T>
class DefinitionManagerClass : public RefCountClass  // NAME 3: by convention (D24) from the client's definition_manager.h
{
public:
	typedef std::hash_map<std::string, DefinitionRefCount<T> *> DefinitionMap;

	DefinitionManagerClass() : Unknown08(0) {}






















































	virtual T *Create(const std::string &name) = 0;			// NAME 2: what it does
	virtual void Destroy(T *definition)								// NAME 2: what it does
	{
		G_ASSERT(false);
	}








	// NOT EXACT: shape (explain.py), two registers swapped in the inlined hashtable destructor and one EH state store more in ours (0x4EAFB0)
	virtual ~DefinitionManagerClass()
	{
		Free_Unused();
		G_ASSERT(!Definitions.size());
	}

	T *Acquire(const std::string &name)								// NAME 2: what it does
	{
		DefinitionMap::iterator find = Definitions.find(name);
		if (find == Definitions.end()) {
			T *definition = Create(name);
			if (definition == 0) {
				return 0;
			}
			DefinitionRefCount<T> *drc = new DefinitionRefCount<T>(definition);
			std::pair<DefinitionMap::iterator, bool> p;
			p = Definitions.insert(DefinitionMap::value_type(name, drc));
			G_ASSERT(p.second);
			find = p.first;
		}
		(*find).second->RefCount++;
		return (*find).second->Definition;
	}




	void Release(const std::string &name)								// NAME 2: what it does
	{
		DefinitionMap::iterator ref_it = Definitions.find(name);
		FAIL_IF(ref_it == Definitions.end()) return;
		DefinitionRefCount<T> *drc = (*ref_it).second;
		drc->RefCount--;
		if (drc->RefCount == 0 && (Unknown08 == 1 || drc->Unknown08)) {
			Remove(ref_it);
		}
	}

	void Free_Unused()														// NAME 2: what it does
	{
		for (DefinitionMap::iterator it = Definitions.begin(); it != Definitions.end(); ) {
			if ((*it).second->RefCount == 0) {
				DefinitionMap::iterator remove = it++;
				Remove(remove);
			} else {
				++it;
			}
		}
	}












































	void Remove(DefinitionMap::iterator &it)							// NAME 2: what it does
	{
		DefinitionRefCount<T> *drc = (*it).second;
		FAIL_IF(drc->RefCount) return;
		if (drc->Definition) {
			Destroy(drc->Definition);
			drc->Definition = 0;
		}
		delete drc;
		Definitions.erase(it);
	}

	int Unknown08;								// +0x08: 1 destroys a definition as soon as it is unused  // NAME 1: neutral placeholder (D10)
	DefinitionMap Definitions;					// +0x0C
};

#endif //__DEFINITION_MANAGER_H
