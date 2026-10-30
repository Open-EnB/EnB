// Earth & Beyond client (reconstruction)
/*
**	Original location: e:\build\beta\G\app\shared\ (file name unknown: no string names this header).
**
**	PxCallback, the interface of the proximity callbacks (vtable 0xAE7B28: the deleting destructor 0x44E170 and five
**	pure slots), and the function type the callbacks report to.
*/
#if defined(_MSC_VER)
#pragma once
#endif
#ifndef PX_CALLBACK_H  // NAME 1 [px_callback.h]: provisional header name (no string)
#define PX_CALLBACK_H

class DynObj;
class PxList;
class PxObject;

typedef void (*PxCallbackFunction)(DynObj *a, DynObj *b, int event);  // NAME 2: the callbacks' Process passes the two boxes' objects and a PxCallbackEvent

class PxCallback  // NAME 2: PxWrapper's 'Update(Callbacks)' calls them
{
public:
	virtual ~PxCallback() {}								// +0x00 (0x44E150; deleting 0x44E170)
	virtual PxCallback *Clone(PxObject *object) = 0;		// +0x04 (PxWrapper's copy)  // NAME 2: what the copy uses it for
	virtual void Attach(PxList *list) = 0;					// +0x08 (PxWrapper::Add_To_List)  // NAME 2: inserts its box in the list
	virtual void Detach(PxList *list) = 0;					// +0x0C (PxWrapper::Unknown_456730)  // NAME 2: removes it
	virtual void Update() = 0;								// +0x10 ('PxWrapper::Update(Callbacks)')  // NAME 3: the profile string
	virtual void Add_Filter(int mask, int test) = 0;		// +0x14 (ORs the bits into its box's Mask / Test)  // NAME 2 [mask, test]: what it does
};

#endif
