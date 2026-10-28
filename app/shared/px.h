/*
**	Earth & Beyond client (reconstruction)
**	Original location: e:\build\beta\G\app\shared\. px.cpp is the client's (its __FILE__, client 0x44B760-0x44D740);
**	this header's name is provisional: no string names it.
**
**	The proximity system: a sweep-and-prune list of axis-aligned boxes. PxList keeps, per axis, one sorted doubly
**	linked list of the boxes' low and high ends; moving a box's corners walks its ends past their neighbours and
**	calls PxBox::On_Enter / On_Exit on the boxes whose overlap begins or ends. A box reports another only when the
**	other's Self bits, masked by the box's Mask, equal its Test.
*/
#if defined(_MSC_VER)
#pragma once
#endif
#ifndef PX_H  // NAME 2 [px.h]: provisional header name (px.cpp's)
#define PX_H

#include "vector3.h"

class PxBox;
class PxList;

/*
** One end of a box on all three axes (0x2C bytes; PxList is one too, the lists' sentinel).
*/
class PxEnd  // NAME 2: a box's low or high end in the sorted lists
{
public:
	enum EndType {  // NAME 3: PxList's dump prints "LOW" / "HIGH" for it
		LOW = 0,  // NAME 3: as above
		HIGH = 1  // NAME 3: as above
	};

	PxEnd(PxBox *box, int type);	// 0x44B760  // NAME 2 [box, type]: what it stores
	~PxEnd();						// 0x44B7B0 (empty)

	PxBox *Box;						// +0x00  // NAME 3: the dump's "Box = %p"
	PxEnd *Next[3];					// +0x04 per axis  // NAME 2: the following end in the axis's list
	PxEnd *Prev[3];					// +0x10 per axis  // NAME 2: the preceding end
	Vector3 Pos;					// +0x1C  // NAME 3: the dump's "Pos = %f"
	int Type;						// +0x28 LOW / HIGH  // NAME 3: the dump's "Type = %s"
};

/*
** A box in the proximity system (0x20 bytes, vtable 0xAE7A3C: the deleting destructor, On_Enter, On_Exit).
*/
class PxBox
{
public:
	PxBox(void *owner);										// 0x44B7D0  // NAME 2 [owner]: stored at +0x08
	PxBox(void *owner, const PxBox &src);					// 0x44B930 (copies Self / Test / Mask)  // NAME 2 [owner, src]: as above
	virtual ~PxBox();										// 0x44BA20 (deleting 0x44B8C0)

	virtual void On_Enter(PxBox *b);						// 0x44BAF0 'PxBox::On_Enter'
	virtual void On_Exit(PxBox *b);							// 0x44BB20 'PxBox::On_Exit'

	void Get_Corners(Vector3 *low, Vector3 *high);			// 0x44BA70 (FAIL_IF '!(low && high)' l.180)  // NAME 2: what it does
	void Remove();											// 0x44BB50 (FAIL_IF '!List' l.200)  // NAME 2: what it does
	void Set_Self(int self);								// 0x44BB90  // NAME 3: the dump's "Self"
	void Set_Test(int test);								// 0x44BBD0  // NAME 3: the dump's "Test"
	void Set_Mask(int mask);								// 0x44BC10  // NAME 3: the dump's "Mask"
	void Set_Test_Mask(int test, int mask);					// 0x44BC50  // NAME 3: as above
	void Set_Corners(const Vector3 &low, const Vector3 &high);	// 0x44BC90 (FAIL_IFs l.244 / 245 / 252)  // NAME 3: PxList::Set_Corners

	bool Wants(const PxBox *b) const { return((b->Self & Mask) == Test); }	// NAME 2: the filter every notification tests

	PxList *List;											// +0x04 (the client's name, the FAIL_IFs 'box->List' / '!List')
	void *Owner;											// +0x08  // NAME 2: the constructor's argument
	PxEnd *High;											// +0x0C (Type HIGH)  // NAME 2: 'new_high'
	PxEnd *Low;												// +0x10 (Type LOW)  // NAME 2: 'new_low'
	int Self;												// +0x14  // NAME 3: the dump's "Self"
	int Test;												// +0x18  // NAME 3: the dump's "Test"
	int Mask;												// +0x1C  // NAME 3: the dump's "Mask"
};

/*
** The list of boxes (0x34 bytes): the sentinel end of the three axis lists.
*/
class PxList : public PxEnd
{
public:
	PxList();												// 0x44BE90  // NAME 3: PxList is the client's (PxList::Insert)
	~PxList();												// 0x44BF00 (FAIL_IF '!Is_Empty()' l.292)

	bool Is_Empty() const { return(Next[0] == (const PxEnd *)this); }		// the client's name, the FAIL_IF '!Is_Empty()'
	void Insert(PxBox *box);								// 0x44BFC0 'PxList::Insert' (FAIL_IFs l.306 / 309)
	void Remove(PxBox *box);								// 0x44C2E0 (FAIL_IFs l.410 / 413)  // NAME 2: Insert's opposite
	void Remove_All();										// 0x44C4E0  // NAME 2: what it does
	void Set_Self(PxBox *box, int self);					// 0x44C520 (FAIL_IF '!box' l.490)  // NAME 3: PxBox::Set_Self's
	void Set_Test_Mask(PxBox *box, int test, int mask);		// 0x44C690 (FAIL_IF '!box' l.564)  // NAME 3: as above
	void Set_Corners(PxBox *box, const Vector3 &low, const Vector3 &high);	// 0x44C900 'PxList::Set_Corners'
	void Dump();											// 0x44D210 (prints the three axes)  // NAME 2: what it does
	bool Is_Sorted();										// 0x44D320  // NAME 2: what it tests

	int Unknown2C;											// +0x2C 0  // NAME 1: only the constructor sets it
	bool Updating;											// +0x30 set while a change walks the lists  // NAME 2: what it marks
};

#endif
