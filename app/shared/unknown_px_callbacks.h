/*
**	Earth & Beyond client (reconstruction)
**	Original location: e:\build\beta\G\app\shared\ (file name unknown: the object 0x451C00-0x452770 has no
**	__FILE__ string; neutral name, D10). E&B's app\shared objects link in alphabetical order, so the original
**	sorts between px_callback_accurate.cpp and px_callback_poll.cpp.
**
**	PxBoxCallback: a box that reports each box entering or leaving it to its function (Enter / Exit), and the
**	callback factories 0x452140 / 0x452460 (px_wrapper.h declares them).
*/
#if defined(_MSC_VER)
#pragma once
#endif
#ifndef UNKNOWN_PX_CALLBACKS_H  // NAME 1 [unknown_px_callbacks.h]: neutral header name (D10)
#define UNKNOWN_PX_CALLBACKS_H

#include "vector3.h"
#include "px.h"
#include "px_callback.h"

/*
** 0x34 bytes; vtables 0xAE7BDC (box) and 0xAE7BC0 (callback, +0x20). The factories make it for the Liberal and
** Conservative types without Polling (the extent the radius, or the radius over sqrt(3)).
*/
class PxBoxCallback : public PxBox, public PxCallback  // NAME 2: what it does
{
public:
	PxBoxCallback(PxObject *object, PxCallbackFunction callback, const Vector3 &extent);	// 0x451C30  // NAME 2 [object, callback, extent]: what it stores
	PxBoxCallback(PxObject *object, const PxBoxCallback &src);						// 0x451DA0  // NAME 2 [object, src]: as PxBox's

	virtual PxCallback *Clone(PxObject *object);		// 0x451EB0
	virtual void Attach(PxList *list);					// 0x451FE0
	virtual void Detach(PxList *list);					// 0x452000
	virtual void Update();								// 0x452020 (the box to the extent around the position)
	virtual void Add_Filter(int mask, int test);		// 0x4520A0
	virtual void On_Enter(PxBox *box);					// 0x4520D0 (PX_EVENT_ENTER)
	virtual void On_Exit(PxBox *box);					// 0x452100 (PX_EVENT_EXIT)

	PxCallbackFunction Callback;						// +0x24  // NAME 2: as PxPollCallback's
	Vector3 Extent;										// +0x28  // NAME 2: as above
};

#endif
