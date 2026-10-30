// Earth & Beyond client (reconstruction)
/*
**	Original location: e:\build\beta\G\app\shared\. px_callback_poll.cpp is the client's (its __FILE__, client
**	0x452780-0x453740); this header's name is provisional: no string names it.
**
**	The polling callbacks: a box (a PxBoxList) that, while anything overlaps it, sits in the current PxProcessClass
**	and on each Process reports every overlapping box's object to its function with the event Update.
*/
#if defined(_MSC_VER)
#pragma once
#endif
#ifndef PX_CALLBACK_POLL_H  // NAME 1 [px_callback_poll.h]: provisional header name (px_callback_poll.cpp's)
#define PX_CALLBACK_POLL_H

#include "vector3.h"
#include "px_box_list.h"
#include "px_process.h"
#include "px_callback.h"

/*
** The polling box (0x50 bytes; vtables 0xAE7C9C (box), 0xAE7C90 (node, +0x30), 0xAE7C74 (callback, +0x3C)): the box
** is the extent around its object's position. The factories 0x452140 / 0x452460 make it for the Liberal and
** Conservative types with Polling; PxWrapper's loose box is one (extent 0, _Collision_Callback).
*/
class PxPollCallback : public PxBoxList, public PxProcessNode, public PxCallback  // NAME 2: px_callback_poll.cpp's base class
{
public:
	PxPollCallback(PxObject *object, PxCallbackFunction callback, const Vector3 &extent);	// 0x452780  // NAME 2 [object, callback, extent]: what it stores
	PxPollCallback(PxObject *object, const PxPollCallback &src);						// 0x452960  // NAME 2 [object, src]: as PxBoxList's

	virtual void Process();								// 0x452AA0 (node slot +0x04: every overlap's objects, Update)
	virtual void On_Enter(PxBox *box);					// 0x452AF0 (the first overlap adds it to the current PxProcessClass)
	virtual void On_Exit(PxBox *box);					// 0x452B40 (the last one removes it)
	virtual PxCallback *Clone(PxObject *object);		// 0x452BB0
	virtual void Attach(PxList *list);					// 0x452D10
	virtual void Detach(PxList *list);					// 0x452D30
	virtual void Update();								// 0x452D50 (the box to the extent around the position)
	virtual void Add_Filter(int mask, int test);		// 0x452DE0

	PxCallbackFunction Callback;						// +0x40  // NAME 2: what it holds
	Vector3 Extent;										// +0x44  // NAME 2: the half size of the box
};

/*
** The accurate polling box (0x54 bytes; vtables 0xAE7CD4, 0xAE7CC8, 0xAE7CAC): reports only the overlapping objects
** inside its sphere (the extent's x as radius) or its volume (the box, grown by the other object's radius). The
** factory 0x452140 makes the sphere for Accurate with Polling.
*/
class PxPollAccurateCallback : public PxPollCallback  // NAME 2: what it does (the Accurate type)
{
public:
	enum {
		SPHERE = 0,  // the client's name, the FAIL_IF 'Type != SPHERE'
		VOLUME = 1  // the client's name, the FAIL_IF 'Type != VOLUME'
	};

	PxPollAccurateCallback(PxObject *object, PxCallbackFunction callback, float radius);			// 0x452E10 (SPHERE)  // NAME 2 [object, callback, radius]: what it stores
	PxPollAccurateCallback(PxObject *object, PxCallbackFunction callback, const Vector3 &extent);	// 0x453030 (VOLUME)  // NAME 2 [extent]: as above
	PxPollAccurateCallback(PxObject *object, const PxPollAccurateCallback &src);					// 0x4531A0  // NAME 2 [object, src]: as above

	void Unknown_453300();								// 0x453300 (Set_Test_Mask(5, 5); no caller)
	virtual PxCallback *Clone(PxObject *object);		// 0x453320
	virtual void Process();								// 0x4534A0
	void Process_Volume();								// VOLUME's test (inlined into 0x4534A0: the client's this is the object, not the node)  // NAME 1: a helper for the form
	void Set_Radius(float radius);						// 0x453660 (FAIL_IF l.216)  // NAME 2 [radius]: what it sets
	void Set_Extent(const Vector3 &extent);				// 0x4536B0 (FAIL_IF l.223)  // NAME 2 [extent]: as above

	int Type;											// +0x50 SPHERE / VOLUME (the client's name, the FAIL_IFs)
};

#endif
