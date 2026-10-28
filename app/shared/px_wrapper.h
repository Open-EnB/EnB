/*
**	Earth & Beyond client (reconstruction)
**	Original location: e:\build\beta\G\app\shared\. px_wrapper.cpp is the client's (its asserts, client
**	0x4560B0-0x457740); this header's name is provisional: no string names it.
**
**	PxWrapper, the client's name ('PxWrapper::Update(LooseBox)' in 0x456B60): a dynamic object's entry in the
**	proximity system (DynObj's ProximityObject, dynamic_obj.cpp), 0x2C bytes. It owns the object's PxObject (its
**	position and radius) and three boxes in px.cpp's PxList: the tight box (Self 3, the radius over sqrt(3)), the
**	loose box (the radius; it reports collisions to _Collision_Callback) and the callback box (in the list only
**	while Unknown0D is set), plus the callbacks added through Unknown_456F60 / Unknown_457190 / Add_Callback.
**
**	PxObject (px_collision.h) and the callback factories below are other files' classes and functions.
*/
#if defined(_MSC_VER)
#pragma once
#endif
#ifndef PX_WRAPPER_H  // NAME 1 [px_wrapper.h]: provisional header name (px_wrapper.cpp's)
#define PX_WRAPPER_H

#include "vector3.h"
#include "px.h"
#include "px_box_list.h"
#include "px_callback.h"
#include "px_callback_poll.h"
#include "px_collision.h"
#include <vector>

class DynObj;
class PxList;

PxCallback *Unknown_452140(int a, PxObject *object, int b, int c);	// 0x452140 (a factory switching on a's type bits)  // NAME 1 [a, b, c]: neutral
PxCallback *Unknown_452460(int a, PxObject *object, int b, int c);	// 0x452460 (another factory)  // NAME 1 [a, b, c]: neutral

class PxWrapper
{
public:
	PxWrapper(DynObj *owner);											// 0x4560B0  // NAME 1 [owner]: it stores it at +0x00
	PxWrapper(DynObj *owner, const PxWrapper &src);					// 0x456190 (DynObj's copy)  // NAME 1 [owner, src]: as above
	~PxWrapper();														// 0x456500 (FAIL_IF 'List' l.108)

	bool Is_In_List() const { return(List != NULL); }					// dynamic_obj.cpp's FAIL_IF 'ProximityObject->Is_In_List()'
	void Create_Boxes();												// 0x456350 (both constructors)  // NAME 2: what it does
	void Add_To_List(PxList *list);										// 0x456660 (FAIL_IFs '!list' l.128, 'List' l.131)  // NAME 2: what it does
	void Unknown_456730();												// 0x456730 (FAIL_IF '!List' l.154: leaves the list)
	void Unknown_4567D0(bool value);									// 0x4567D0 (sets +0x0E)  // NAME 1 [value]: what it stores
	void Unknown_456840(bool value);									// 0x456840 (sets +0x0C)  // NAME 1 [value]: what it stores
	void Unknown_456890(bool value);									// 0x456890 (sets +0x0D)  // NAME 1 [value]: what it stores
	std::vector<DynObj *> &Unknown_4568E0();								// 0x4568E0 (the callback box's Owners)
	void Unknown_456900(const Vector3 &low, const Vector3 &high);		// 0x456900 (FAIL_IF '!low.Is_Valid()' l.247, '!high.Is_Valid()' l.248)
	void Update(const Vector3 &pos, float radius);						// 0x456B60 'PxWrapper::Update' (FAIL_IF '!pos.Is_Valid()' l.272, '!WWMath::Is_Valid_Float(radius)')
	PxCallback *Unknown_456F60(int a, int b, int c);					// 0x456F60 (adds 0x452140's callback)  // NAME 1 [a, b, c]: neutral
	PxCallback *Unknown_457190(int a, int b, int c);					// 0x457190 (adds 0x452460's callback)  // NAME 1 [a, b, c]: neutral
	void Add_Callback(PxCallback *callback);							// 0x4573C0  // NAME 2: what it does
	bool Unknown_4575E0(void *callback);								// 0x4575E0 (removes it from Callbacks)  // NAME 1 [callback]: what it searches for

	DynObj *Owner;														// +0x00  // NAME 1: the constructor's argument
	PxObject *Object;													// +0x04  // NAME 2: the boxes' owner
	PxList *List;														// +0x08 (the client's name, the FAIL_IFs 'List' / '!List')
	bool Unknown0C;														// +0x0C false (0x456840: bit 8 of the loose box's Self and Mask)
	bool Unknown0D;														// +0x0D false (0x456890: the callback box is in the list)
	bool Unknown0E;														// +0x0E (the constructor sets it: bit 0x10 of the loose and tight boxes)
	PxBox *TightBox;													// +0x10  // NAME 3: 'PxWrapper::Update(TightBox)'
	PxPollCallback *LooseBox;											// +0x14  // NAME 3: 'PxWrapper::Update(LooseBox)'
	PxOwnerList *CallbackBox;												// +0x18  // NAME 2: in the list with Unknown0D
	std::vector<PxCallback *> Callbacks;								// +0x1C  // NAME 3: 'PxWrapper::Update(Callbacks)'
};

#endif
