// Earth & Beyond client (reconstruction): app\Client\gadget_list_manager.cpp. Notes at the end of the file.
#include <string.h>
#include "gadget_list_manager.h"
#include "gadget.h"
#include "..\shared\unknown_debug.h"

GadgetListManagerClass::GadgetListManagerClass() :
	Unknown0C(100)
{
	GroupLists.push_back(&Unknown24);
	GroupLists.push_back(&Unknown30);
}






































GadgetListManagerClass::~GadgetListManagerClass()
{
	GroupLists.remove(&Unknown24);
	GroupLists.remove(&Unknown30);
	G_ASSERT(GroupLists.empty());
}

bool GadgetListManagerClass::Unknown_661610(InterfaceClass *iface)
{
	GadgetListType *list = &Unknown0C[iface];
	if (list->size() == 0) {
		return(false);
	}
	std::list<GadgetListType *>::iterator it = GroupLists.begin();
	if (*(++it) == list) {
		return(false);
	}
	GroupLists.remove(list);
	Unknown_661B40(list);
	return(true);
}









void GadgetListManagerClass::Unknown_661700(GadgetClass *gadget)
{
	GadgetListType *list = Unknown_661BB0(gadget, true);
	FAIL_IF(!list) return;
	GadgetListType::iterator it;
	for (it = list->begin(); it != list->end(); ++it) {
		if (gadget->Unknown0C < (*it)->Unknown0C) {
			break;
		}
		FAIL_IF((*it) == gadget) return;
	}
	list->insert(it, gadget);
}


void GadgetListManagerClass::Unknown_6617E0(GadgetClass *gadget)
{
	GadgetListType *list = Unknown_661BB0(gadget, false);
	FAIL_IF(!list) return;
	Unknown3C.clear();
	int size = list->size();
	list->remove(gadget);



	G_ASSERT(size != list->size());
	if (list->size() == 0 && list != &Unknown24 && list != &Unknown30) {
		GroupLists.remove(list);
	}
}





GadgetClass *GadgetListManagerClass::Unknown_661940(const char *name)
{
	FAIL_IF(!name) return(NULL);
	GadgetListIteratorClass it(this);
	GadgetClass *gadget;
	while ((gadget = it.Get()) != NULL) {
		if (gadget->Unknown24 && strcmp(gadget->Unknown24, name) == 0) {
			return(gadget);
		}
		++it;
	}
	return(NULL);
}





GadgetClass *GadgetListManagerClass::Unknown_661AA0(bool shown, bool flag)
{
	for (std::vector<GadgetClass *>::iterator it = Unknown3C.begin(); it != Unknown3C.end(); ++it) {
		GadgetClass *gadget = *it;
		FAIL_IF(!gadget) continue;
		if ((!shown || gadget->Unknown_60()) && gadget->Is_Unknown2C_02() && gadget->Is_Unknown2C_04() == flag) {
			return(gadget);
		}
	}
	return(NULL);
}

void GadgetListManagerClass::Unknown_661B40(GadgetListType *list)
{
	GroupLists.insert(++GroupLists.begin(), list);
}

















GadgetListType *GadgetListManagerClass::Unknown_661BB0(GadgetClass *gadget, bool add)
{
	FAIL_IF(!gadget) return(NULL);
	GadgetListType *list;
	if (gadget->Is_Last_Sort_Group()) {
		list = &Unknown30;
	} else if (gadget->Is_First_Sort_Group()) {
		list = &Unknown24;
	} else {
		list = &Unknown0C[gadget->Get_Interface()];
		if (list->size() == 0) {
			if (add) {
				Unknown_661B40(list);
			} else {







				G_ASSERT(add);
			}
		}
	}
	return(list);
}




















GadgetListIteratorClass::GadgetListIteratorClass(GadgetListManagerClass *manager) :
	Manager(manager)
{
	FAIL_IF(!Manager) return;
	Unknown_661DA0(true);
}

bool GadgetListIteratorClass::Unknown_661DA0(bool first)
{
	if (first) {
		ListIt = Manager->GroupLists.begin();
	} else {
		++ListIt;
	}
	for (; ListIt != Manager->GroupLists.end(); ++ListIt) {
		GadgetIt = (*ListIt)->begin();
		if (GadgetIt != (*ListIt)->end()) {
			return(true);
		}
	}
	return(false);
}

















GadgetListIteratorClass &GadgetListIteratorClass::operator ++()
{
	FAIL_IF(!Manager) return(*this);
	if (ListIt != Manager->GroupLists.end()) {
		++GadgetIt;
		if (GadgetIt == (*ListIt)->end()) {
			Unknown_661DA0(false);
		}
	}
	return(*this);
}



GadgetClass *GadgetListIteratorClass::Get()
{
	FAIL_IF(!Manager) return(NULL);
	if (ListIt == Manager->GroupLists.end()) {
		return(NULL);
	}
	return(*GadgetIt);
}


/*
**	Reconstruction notes
**
**	The client's window (app-functions.json, __FILE__ 0xB8CB7C 'e:\build\beta\G\app\Client\gadget_list_manager.cpp'):
**	the constructor 0x6611B0 (no assert), the destructor 0x661430 (line 55), 0x661610, the put in / taken out
**	0x661700 / 0x6617E0 (84-106), the name search 0x661940 (118; it inlines the iterator's 217 / 269), 0x661AA0
**	(138), 0x661B40, the gadget's list 0x661BB0 (169 / 188), the iterator 0x661D20-0x661E80 (217, 255, 269).
**	Compiled with them: the hash_map's operator[] 0x661F60, its hashtable's clear 0x661360 and the STL helpers
**	up to 0x662C50. The client's 0x661CE0 / 0x661D00 (a list's begin / end out of line) and 0x661D70 (the
**	iterator's copy) are not emitted by these sources.
*/
