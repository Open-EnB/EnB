/*
**	Earth & Beyond client (reconstruction)
**	Original location: e:\build\beta\G\app\shared\. px_callback_accurate.cpp is the client's (its __FILE__, client
**	0x44E020-0x451BF0); this header's name is provisional: no string names it.
**
**	The accurate callbacks' base: a PxProcessNode and a PxCallback that keeps the objects overlapping it (Overlaps,
**	an SGI hash_map) and the ones it reports on (Active, sorted by address), and on each Process tests each active
**	object again (slot +0x08) and reports the changes as Enter / Exit. Only the base is declared so far.
*/
#if defined(_MSC_VER)
#pragma once
#endif
#ifndef PX_CALLBACK_ACCURATE_H  // NAME 1 [px_callback_accurate.h]: provisional header name (px_callback_accurate.cpp's)
#define PX_CALLBACK_ACCURATE_H

#include "px_process.h"
#include "px_callback.h"
#include "px.h"
#include "vector3.h"
#include <vector>
#include <hash_map.h>

class PxObject;

__STL_BEGIN_NAMESPACE
__STL_TEMPLATE_NULL struct hash<PxObject *> {  // the key's address (as co_data.h's)
	size_t operator()(PxObject *object) const { return((size_t)object); }
};
__STL_END_NAMESPACE

struct PxAccurateOverlapStruct  // NAME 1: Overlaps' value (0x44E470 reads a state 1 / 2 and a flag); layout not confirmed
{
	int State;							// NAME 1: what it holds
	bool Flag;							// NAME 1: as above
};

struct PxAccurateActiveStruct  // NAME 2: Active's element (8 bytes; the FAIL_IF 'it->Object != object')
{
	PxObject *Object;					// +0x00 (the client's name, the FAIL_IF)
	bool Inside;						// +0x04 (Process's last test result)  // NAME 2: what it holds
};

/*
** 0x40 bytes; vtables 0xAE7B10 (node: the deleting destructor 0x44E1A0, Process, the pure Test, Enter, Exit) and
** 0xAE7AF4 (callback, +0x0C: all pure).
*/
class PxAccurateCallback : public PxProcessNode, public PxCallback  // NAME 2: px_callback_accurate.cpp's base class
{
public:
	PxAccurateCallback(PxObject *object, PxCallbackFunction callback);		// 0x44E020  // NAME 2 [object, callback]: what it stores
	PxAccurateCallback(PxObject *object, const PxAccurateCallback &src);	// 0x44E260 (only the function is copied)  // NAME 2 [object, src]: as the boxes'
	virtual ~PxAccurateCallback();											// 0x44E390 (deleting 0x44E1A0)

	virtual void Process();								// 0x44F2F0 (node slot +0x04)
	virtual bool Test(PxObject *object) = 0;			// node slot +0x08  // NAME 2: what Process asks
	virtual void Enter(PxObject *object);				// 0x44F370 (PX_EVENT_ENTER)  // NAME 2: what it reports
	virtual void Exit(PxObject *object);				// 0x44F3A0 (PX_EVENT_EXIT)  // NAME 2: as above

	PxObject *Object;															// +0x10  // NAME 2: the constructor's argument
	PxCallbackFunction Callback;												// +0x14  // NAME 2: as PxPollCallback's
	std::hash_map<PxObject *, PxAccurateOverlapStruct> Overlaps;				// +0x18 (the client's name, the FAIL_IF 'it == Overlaps.end()'; 100 buckets)
	std::vector<PxAccurateActiveStruct> Active;									// +0x30 (the client's name, the FAIL_IF 'it == Active.end()')
};


/*
** The box of an accurate callback (0x24 bytes, vtable 0xAE7B44: the deleting destructor 0x44F870, On_Enter
** 0x44F5D0, On_Exit 0x44F6E0, not rebuilt): it hands its overlaps to its callback's Overlaps / Active.
*/
class PxAccurateBox : public PxBox  // NAME 2: what it is
{
public:
	PxAccurateBox(PxObject *object, PxAccurateCallback *callback) : PxBox(object), Callback(callback) {}	// NAME 2 [object, callback]: what it stores
	PxAccurateBox(PxObject *object, const PxBox &src, PxAccurateCallback *callback) : PxBox(object, src), Callback(callback) {}	// NAME 2 [src]: the copy's

	virtual void On_Enter(PxBox *box);					// 0x44F5D0 (not rebuilt)
	virtual void On_Exit(PxBox *box);					// 0x44F6E0 (not rebuilt)

	PxAccurateCallback *Callback;						// +0x20 (both accurate classes' boxes)  // NAME 2: its owner
};

/*
** The accurate box callback (0x50 bytes; vtables 0xAE7B70 (node), 0xAE7B54 (callback, +0x0C)): its own box (Test 1,
** Mask 1) is the extent around its object; Test asks whether an object's sphere touches that box.
*/
class PxAccurateBoxCallback : public PxAccurateCallback  // NAME 2: the accurate callback with a box
{
public:
	PxAccurateBoxCallback(PxObject *object, PxCallbackFunction callback, const Vector3 &extent);	// 0x44F3D0  // NAME 2 [object, callback, extent]: what it stores
	PxAccurateBoxCallback(PxObject *object, const PxAccurateBoxCallback &src);					// 0x44F8F0  // NAME 2 [object, src]: as the boxes'
	virtual ~PxAccurateBoxCallback();									// 0x44FCF0 (deleting 0x44F8C0)

	virtual PxCallback *Clone(PxObject *object);		// 0x44FAD0
	virtual void Attach(PxList *list);					// 0x44FE50
	virtual void Detach(PxList *list);					// 0x44FE70
	virtual void Update();								// 0x44FE90
	virtual void Add_Filter(int mask, int test);		// 0x44FF10
	virtual bool Test(PxObject *object);				// 0x44FF40 (the object's sphere against the box)

	PxAccurateBox *Box;									// +0x40  // NAME 2: what it holds
	Vector3 Extent;										// +0x44  // NAME 2: as PxPollCallback's
};

/*
** The accurate sphere callback (0x4C bytes; vtables 0xAE7BA8 (node), 0xAE7B8C (callback, +0x0C)): the factory 0x452140
** makes it for Accurate without Polling. Its inner box (Test / Mask 3, as PxWrapper's tight box) is the radius / sqrt(3)
** cube, its outer box (Test / Mask 5, as the loose box) the radius cube; Test asks whether an object's sphere touches its sphere.
*/
class PxAccurateSphereCallback : public PxAccurateCallback  // NAME 2: the accurate callback with a sphere
{
public:
	PxAccurateSphereCallback(PxObject *object, PxCallbackFunction callback, float radius);		// 0x450030  // NAME 2 [object, callback, radius]: what it stores
	PxAccurateSphereCallback(PxObject *object, const PxAccurateSphereCallback &src);			// 0x450350  // NAME 2 [object, src]: as the boxes'
	virtual ~PxAccurateSphereCallback();								// 0x4506B0 (deleting 0x450320)

	virtual PxCallback *Clone(PxObject *object);		// 0x450620
	virtual void Attach(PxList *list);					// 0x450820
	virtual void Detach(PxList *list);					// 0x450860
	virtual void Update();								// 0x4508A0
	virtual void Add_Filter(int mask, int test);		// 0x4509E0
	void Set_Filter(int mask, int test);				// 0x450A30 (bit 2 into the inner box's, bit 4 into the outer's; no caller)  // NAME 2 [mask, test]: what it does
	void Set_Self(int self);							// 0x450A80 (as above for Self; no caller)  // NAME 2 [self]: as PxBox's
	virtual bool Test(PxObject *object);				// 0x450AC0 (the spheres touch)

	PxAccurateBox *InnerBox;							// +0x40 (Test / Mask 3)  // NAME 2: the radius / sqrt(3) cube
	PxAccurateBox *OuterBox;							// +0x44 (Test / Mask 5)  // NAME 2: the radius cube
	float Radius;										// +0x48  // NAME 2: what it holds
};

#endif
