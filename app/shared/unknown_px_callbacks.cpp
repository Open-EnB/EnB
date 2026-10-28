// Earth & Beyond client (reconstruction): app\shared\ (file name unknown, D10). Notes at the end of the file.
#include "unknown_px_callbacks.h"
#include "px_callback_type.h"
#include "px_wrapper.h"

// 0x451C30
PxBoxCallback::PxBoxCallback(PxObject *object, PxCallbackFunction callback, const Vector3 &extent) :
	PxBox(object),
	Callback(callback),
	Extent(extent)
{
	Update();
}

// 0x451DA0
PxBoxCallback::PxBoxCallback(PxObject *object, const PxBoxCallback &src) :
	PxBox(object),
	Callback(src.Callback),
	Extent(src.Extent)
{
	Update();
}

// 0x451EB0
PxCallback *PxBoxCallback::Clone(PxObject *object)
{
	return(new PxBoxCallback(object, *this));
}

// 0x451FE0
void PxBoxCallback::Attach(PxList *list)
{
	list->Insert(this);
}

// 0x452000
void PxBoxCallback::Detach(PxList *list)
{
	list->Remove(this);
}

// 0x452020
void PxBoxCallback::Update()
{
	const Vector3 &pos = ((PxObject *)Owner)->Pos;
	Set_Corners(pos - Extent, Extent + pos);
}

// 0x4520A0
void PxBoxCallback::Add_Filter(int mask, int test)
{
	Set_Test_Mask(Test | test, Mask | mask);
}

// 0x4520D0
void PxBoxCallback::On_Enter(PxBox *box)
{
	Callback(((PxObject *)Owner)->Owner, ((PxObject *)box->Owner)->Owner, PX_EVENT_ENTER);
}

// 0x452100
void PxBoxCallback::On_Exit(PxBox *box)
{
	Callback(((PxObject *)Owner)->Owner, ((PxObject *)box->Owner)->Owner, PX_EVENT_EXIT);
}

/*
** Notes
**
** The object 0x451C00-0x452770 sits between px_callback_accurate.cpp (its guarded <string> static at 0x451BD0) and
** px_callback_poll.cpp (from 0x452780); its own guard is at 0x452750. It has no __FILE__ string, so its name is
** unknown (D10). It holds this-adjusting thunks for px_callback_accurate.cpp's classes (0x451C00-0x451C20),
** PxBoxCallback (0x451C30-0x452130) and the two factories 0x452140 / 0x452460 (not rebuilt: they construct
** px_callback_accurate.cpp's classes). The copy constructor copies the PxBox's owner only (PxBox(object), not
** PxBox's copy), unlike the polling boxes.
*/
