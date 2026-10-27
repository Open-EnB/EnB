/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\definition_auto_ptr.h (the client's __FILE__ of
**	its asserts, through ini_manager.cpp)
**
**	A definition acquired from a DefinitionManagerClass, released when the pointer is released or
**	destroyed. The client names DefinitionName and Pointer through its asserts.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __DEFINITION_AUTO_PTR_H
#define __DEFINITION_AUTO_PTR_H

#include "definition_manager.h"
#include "..\..\wwlib\ref_ptr.h"

// 0x18 bytes
template <class T>
class DefinitionAutoPtr  // NAME 3: from the client's file definition_auto_ptr.h
{
public:
	DefinitionAutoPtr() : Pointer(0) {}
	DefinitionAutoPtr(const DefinitionAutoPtr<T> &src) : Pointer(0)
	{
		Set(src.Manager, src.DefinitionName);
	}
	~DefinitionAutoPtr()
	{
		Release();
	}

	const DefinitionAutoPtr<T> &operator=(const DefinitionAutoPtr<T> &src)
	{
		Set(src.Manager, src.DefinitionName);
		return *this;
	}

	T *operator->() const { return Pointer; }
	T *Peek() const { return Pointer; }																// NAME 2: as ref_ptr.h's

	void Set(const RefCountPtr<DefinitionManagerClass<T> > &manager, const std::string &name)	// NAME 2: what it does
	{
		Release();
		if (manager) {
			Pointer = manager->Acquire(name);
			if (Pointer) {
				Manager = manager;
				DefinitionName = name;
			}
		}
	}




















































	// NOT EXACT: register-only (explain.py), the inlined erase's terminator store addresses [edi+eax] in the client, [eax+edi] in ours (0x4EB1B0)
	void Release()																									// NAME 2: what it does
	{
		if (Manager) {
			Manager->Release(DefinitionName);
			Manager = 0;
			Pointer = 0;
			DefinitionName.erase(0, -1);
		} else {
			FAIL_IF(Pointer) Pointer = 0;
			FAIL_IF(DefinitionName.size()) DefinitionName.erase(0, -1);
		}
	}

private:
	std::string DefinitionName;															// +0x00
	T *Pointer;																					// +0x10
	RefCountPtr<DefinitionManagerClass<T> > Manager;								// +0x14  // NAME 2: what it holds
};

#endif //__DEFINITION_AUTO_PTR_H
