// Earth & Beyond client (reconstruction): app\shared\px_box_list.cpp. Notes at the end of the file.
#include "px_box_list.h"
#include "px_wrapper.h"
#include "..\shared\unknown_debug.h"
#include <algorithm>

// 0x44D740
PxBoxList::PxBoxList(void *owner) :
	PxBox(owner)
{
}

// 0x44D7B0
PxBoxList::PxBoxList(void *owner, const PxBoxList &src) :
	PxBox(owner, src)
{
}

// 0x44D800
PxBoxList::~PxBoxList()
{
}













// 0x44D840
void PxBoxList::On_Enter(PxBox *box)
{
	std::vector<PxBox *>::iterator it = std::lower_bound(Boxes.begin(), Boxes.end(), box);

	FAIL_IF((it != Boxes.end()) && (*it == box)) return;
	Boxes.insert(it, box);
}





// 0x44DA90
void PxBoxList::On_Exit(PxBox *box)
{
	std::vector<PxBox *>::iterator it = std::lower_bound(Boxes.begin(), Boxes.end(), box);

	FAIL_IF((it == Boxes.end()) || (*it != box)) return;
	Boxes.erase(it);
}

// 0x44DB50
PxOwnerList::PxOwnerList(void *owner) :
	PxBox(owner)
{
}

// 0x44DBC0
PxOwnerList::PxOwnerList(void *owner, const PxOwnerList &src) :
	PxBox(owner, src)
{
}

// 0x44DC10
PxOwnerList::~PxOwnerList()
{
}

















// 0x44DC50
void PxOwnerList::On_Enter(PxBox *box)
{
	DynObj *owner = ((PxObject *)box->Owner)->Owner;
	std::vector<DynObj *>::iterator it = std::lower_bound(Owners.begin(), Owners.end(), owner);
	FAIL_IF((it != Owners.end()) && (*it == owner)) return;
	Owners.insert(it, owner);
}







// 0x44DEB0
void PxOwnerList::On_Exit(PxBox *box)
{
	DynObj *owner = ((PxObject *)box->Owner)->Owner;
	std::vector<DynObj *>::iterator it = std::lower_bound(Owners.begin(), Owners.end(), owner);
	FAIL_IF((it == Owners.end()) || (*it != owner)) return;
	Owners.erase(it);
}

/*
** Notes
**
** px_box_list.cpp is client 0x44D740-0x44E020, after px.cpp and before px_callback_accurate.cpp; its __FILE__ is
** pushed by the four FAIL_IFs (lines 41, 54, 96, 111). PxBoxList (vtable 0xAE7A4C) keeps the boxes that overlap it,
** PxOwnerList (vtable 0xAE7A5C, PxWrapper's callback box) the DynObjs that own them (the box's PxObject's +0x00);
** both vectors stay sorted by address: On_Enter inserts at lower_bound, On_Exit erases there. The copy constructors
** copy only the PxBox part. The names of the classes and of this header are provisional (the vectors' names are
** the asserts'). The vector<DynObj *> helpers 0x44DF70-0x44DFE0 are this file's instances (vector<PxBox *>'s are
** px.cpp's).
*/
