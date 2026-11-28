// Earth & Beyond client (reconstruction)
/*
**	Original location: e:\build\beta\G\app\shared\. px_box_list.cpp is the client's (its __FILE__, client
**	0x44D740-0x44E020); this header's name is provisional: no string names it.
**
**	Two boxes that remember what overlaps them: PxBoxList keeps the overlapping boxes, PxBoxOwnerList their owners'
**	objects, each as a vector sorted by address (On_Enter inserts, On_Exit erases).
*/
#if defined(_MSC_VER)
#pragma once
#endif
#ifndef PX_BOX_LIST_H  // NAME 1 [px_box_list.h]: provisional header name (px_box_list.cpp's)
#define PX_BOX_LIST_H

#include "px.h"
#include <vector>

class DynamicObjectClass;

/*
** A box keeping the boxes that overlap it (0x30 bytes, vtable 0xAE7A4C; the base of the loose box 0x452780 and of
** px_callback_accurate.cpp's boxes 0x452E10 / 0x453030).
*/
class PxBoxList : public PxBox  // NAME 2: px_box_list.cpp's class with the 'Boxes' asserts
{
public:
	PxBoxList(void *owner);								// 0x44D740  // NAME 2 [owner]: the PxBox owner
	PxBoxList(void *owner, const PxBoxList &src);		// 0x44D7B0 (the vector starts empty)  // NAME 2 [owner, src]: as PxBox's
	virtual ~PxBoxList();								// 0x44D800 (deleting 0x44D780)

	protected: virtual void On_Enter(PxBox *box);					// 0x44D840 (FAIL_IF l.41)
	virtual void On_Exit(PxBox *box);					// 0x44DA90 (FAIL_IF l.54)

	public: std::vector<PxBox *> Boxes;							// +0x20 (the client's name, the FAIL_IFs 'Boxes.end()')
};

/*
** A box keeping the objects whose boxes overlap it (0x30 bytes, vtable 0xAE7A5C; PxWrapper's callback box): the
** owner of each box b is its PxObject's DynamicObjectClass (b->Owner's +0x00).
*/
class PxBoxOwnerList : public PxBox  // NAME 5: thunk order, binary.md 5.11
{
public:
	PxBoxOwnerList(void *owner);							// 0x44DB50  // NAME 2 [owner]: the PxBox owner
	PxBoxOwnerList(void *owner, const PxBoxOwnerList &src);	// 0x44DBC0 (the vector starts empty)  // NAME 2 [owner, src]: as PxBox's
	virtual ~PxBoxOwnerList();								// 0x44DC10 (deleting 0x44DB90)

	virtual void On_Enter(PxBox *box);					// 0x44DC50 (FAIL_IF l.96)
	virtual void On_Exit(PxBox *box);					// 0x44DEB0 (FAIL_IF l.111)

	std::vector<DynamicObjectClass *> Owners;						// +0x20 (the client's name, the FAIL_IFs 'Owners.end()')
};

#endif
