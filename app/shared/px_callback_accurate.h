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

#endif
