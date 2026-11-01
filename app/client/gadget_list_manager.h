// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\gadget_list_manager.h (name assumed from gadget_list_manager.cpp,
**	whose __FILE__ is 'e:\build\beta\G\app\Client\gadget_list_manager.cpp'; the client holds no string of this header)
**
**	The gadgets of a view (UIViewClass +0x2C; SClient +0x1278's for the packets): one list per interface, kept
**	sorted by the gadgets' +0x0C, in GroupLists between the first and the last sort group's (constructor 0x6611B0,
**	destructor 0x661430, 0x4C bytes), and the iterator over them (0x0C bytes, 0x661D20-0x661E80).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __GADGET_LIST_MANAGER_H  // NAME 3 [gadget_list_manager.h]: header name assumed from the client's gadget_list_manager.cpp
#define __GADGET_LIST_MANAGER_H

#include <list>
#include <vector>
#include <hash_map.h>

class GadgetClass;
class InterfaceClass;

typedef std::list<GadgetClass *> GadgetListType;  // NAME 1: neutral (D10)

__STL_BEGIN_NAMESPACE
__STL_TEMPLATE_NULL struct hash<InterfaceClass *> {  // the key's address (as co_data.h's)
	size_t operator()(InterfaceClass *iface) const { return((size_t)iface); }
};
__STL_END_NAMESPACE

class GadgetListManagerClass  // NAME 3: class-from-file (D24) for app\client\gadget_list_manager.cpp
{
public:
	GadgetListManagerClass();											// 0x6611B0
	~GadgetListManagerClass();											// 0x661430 (G_ASSERT 'GroupLists.empty()')

	bool Unknown_661610(InterfaceClass *iface);							// 0x661610 (gadget.cpp's input: the interface's list to the front of the groups)  // NAME 1 [iface]: neutral
	void Unknown_661700(GadgetClass *gadget);							// 0x661700 (FAIL_IF '!list', '(*it) == gadget': put in, by +0x0C)  // NAME 1 [gadget]: the FAIL_IF's
	void Unknown_6617E0(GadgetClass *gadget);							// 0x6617E0 (FAIL_IF '!list', G_ASSERT 'size != list->size()': taken out)  // NAME 1 [gadget]: neutral
	GadgetClass *Unknown_661940(const char *name);						// 0x661940 (FAIL_IF '!name': by the gadget's +0x24; cpacket.cpp)  // NAME 2 [name]: the FAIL_IF's
	GadgetClass *Unknown_661AA0(bool shown, bool flag);					// 0x661AA0 (FAIL_IF '!gadget': the first of +0x3C under the pointer)  // NAME 1 [shown, flag]: neutral
	void Unknown_661B40(GadgetListType *list);							// 0x661B40 (a list into GroupLists, after the first)  // NAME 1 [list]: neutral
	GadgetListType *Unknown_661BB0(GadgetClass *gadget, bool add);		// 0x661BB0 (FAIL_IF '!gadget', G_ASSERT 'add': the gadget's list)  // NAME 2 [gadget, add]: the asserts'

	std::list<GadgetListType *> GroupLists;								// +0x00 (G_ASSERT 'GroupLists.empty()')  // NAME 3: the assert's
	std::hash_map<InterfaceClass *, GadgetListType> Unknown0C;				// +0x0C (by the gadgets' interface, +0x08; 100 buckets)
	GadgetListType Unknown24;											// +0x24 (the first sort group's)
	GadgetListType Unknown30;											// +0x30 (the last sort group's)
	std::vector<GadgetClass *> Unknown3C;								// +0x3C (gadget.cpp's input: the gadgets under the pointer)
};

// What walks a manager's gadgets, list by list (0x0C bytes)
class GadgetListIteratorClass  // NAME 1: neutral (D10)
{
public:
	GadgetListIteratorClass(GadgetListManagerClass *manager);			// 0x661D20 (FAIL_IF '!Manager')  // NAME 1 [manager]: the member's
	~GadgetListIteratorClass() {}										// (empty: 0x661940 keeps an unwind state for it)
	bool Unknown_661DA0(bool first);									// 0x661DA0 (to the next list with gadgets)  // NAME 1 [first]: neutral
	GadgetListIteratorClass &operator ++();								// 0x661E10 (FAIL_IF '!Manager')
	GadgetClass *Get();													// 0x661E80 (FAIL_IF '!Manager')  // NAME 1: neutral (D10)

	GadgetListManagerClass *Manager;									// +0x00 (the FAIL_IFs' '!Manager')  // NAME 3: the FAIL_IFs'
	std::list<GadgetListType *>::iterator ListIt;						// +0x04  // NAME 1: neutral (D10)
	GadgetListType::iterator GadgetIt;									// +0x08  // NAME 1: neutral (D10)
};

#endif // __GADGET_LIST_MANAGER_H
