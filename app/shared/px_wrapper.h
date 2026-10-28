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
**	The classes below PxWrapper are other px_*.cpp files' (not rebuilt), declared only as far as px_wrapper.cpp
**	uses them; their names are provisional.
*/
#if defined(_MSC_VER)
#pragma once
#endif
#ifndef PX_WRAPPER_H  // NAME 1 [px_wrapper.h]: provisional header name (px_wrapper.cpp's)
#define PX_WRAPPER_H

#include "vector3.h"
#include "px.h"
#include <vector>

class DynObj;
class PxList;

/*
** What a dynamic object's boxes belong to (0x14 bytes; constructor 0x4557F0, destructor 0x455860, the file after
** px_collision.cpp): the owner and the position and radius PxWrapper::Update last set.
*/
class PxObject  // NAME 2: the boxes' owner (PxBox::Owner)
{
public:
	PxObject(DynObj *owner);			// 0x4557F0  // NAME 2 [owner]: stored at +0x00
	~PxObject();						// 0x455860 (empty, not inline)

	DynObj *Owner;						// +0x00  // NAME 2: the constructor's argument
	Vector3 Pos;						// +0x04  // NAME 2: Update's pos
	float Radius;						// +0x10  // NAME 2: Update's radius
};

/*
** A proximity callback (another file's classes; vtable slots used here).
*/
class PxCallback  // NAME 2: PxWrapper's 'Update(Callbacks)' calls them
{
public:
	virtual ~PxCallback();									// +0x00
	virtual PxCallback *Clone(PxObject *object) = 0;		// +0x04 (PxWrapper's copy)  // NAME 2: what the copy uses it for
	virtual void Attach(PxList *list) = 0;					// +0x08 (Unknown_456660)  // NAME 2: called on entering a list
	virtual void Detach(PxList *list) = 0;					// +0x0C (Unknown_456730)  // NAME 2: called on leaving it
	virtual void Update() = 0;								// +0x10 ('PxWrapper::Update(Callbacks)')  // NAME 3: the profile string
};

PxCallback *Unknown_452140(int a, PxObject *object, int b, int c);	// 0x452140 (a factory switching on a's type bits)  // NAME 1 [a, b, c]: neutral
PxCallback *Unknown_452460(int a, PxObject *object, int b, int c);	// 0x452460 (another factory)  // NAME 1 [a, b, c]: neutral

typedef void (*PxCollisionCallback)(PxBox *a, PxBox *b);  // NAME 2: _Collision_Callback's type

/*
** px_box_list.cpp's two box classes (vtables 0xAE7A4C / 0xAE7A5C, 0x30 bytes: a PxBox and a vector).
*/
class PxBoxList : public PxBox  // NAME 2: px_box_list.cpp's (0x44DB50)
{
public:
	PxBoxList(PxObject *object);							// 0x44DB50  // NAME 2 [object]: the PxBox owner
	std::vector<PxBox *> Boxes;								// +0x20  // NAME 3: px_box_list.cpp's asserts 'it != Boxes.end()'
};

/*
** The loose box (0x50 bytes, constructor 0x452780: a box list with a collision callback and an extent; the box is
** the extent around its object's position).
*/
class PxCollisionBox : public PxBox  // NAME 2: what it reports
{
public:
	PxCollisionBox(PxObject *object, PxCollisionCallback callback, const Vector3 &extent);	// 0x452780  // NAME 2 [object, callback, extent]: what it stores
	char Unknown20[0x50 - 0x20];							// +0x20  // NAME 1: not rebuilt
};

/*
** px_collision.cpp's PxCollision (the client's name, 'PxCollision::Process'): a pair of boxes to test.
*/
class PxCollision
{
public:
	PxCollision(PxBox *a, PxBox *b) :						// NAME 2 [a, b]: _Collision_Callback's arguments
		A(a), B(b), Unknown08(1.0f), Unknown0C(0.0f), Unknown10(0.0f), Unknown14(0.0f), Unknown18(0.0f), Unknown1C(0.0f),
		Unknown20(false), Unknown24(0.0f) {}
	void Process();											// 0x453A40 'PxCollision::Process'

	PxBox *A;												// +0x00  // NAME 2: the constructor's argument
	PxBox *B;												// +0x04  // NAME 2: as above
	float Unknown08;										// +0x08 1.0
	float Unknown0C;											// +0x0C 0
	float Unknown10;											// +0x10 0
	float Unknown14;											// +0x14 0
	float Unknown18;											// +0x18 0
	float Unknown1C;											// +0x1C 0
	bool Unknown20;											// +0x20 false
	float Unknown24;											// +0x24 0
};

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
	std::vector<PxBox *> &Unknown_4568E0();								// 0x4568E0 (the callback box's Boxes)
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
	PxCollisionBox *LooseBox;											// +0x14  // NAME 3: 'PxWrapper::Update(LooseBox)'
	PxBoxList *CallbackBox;												// +0x18  // NAME 2: in the list with Unknown0D
	std::vector<PxCallback *> Callbacks;								// +0x1C  // NAME 3: 'PxWrapper::Update(Callbacks)'
};

#endif
