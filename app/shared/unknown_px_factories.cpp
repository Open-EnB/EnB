// Earth & Beyond client (reconstruction): app\shared\ (file name unknown, D10). Notes at the end of the file.
// PLACEMENT: provisional (D137): a string or a link map naming the object 0x452140-0x452770; the inline evidence
// below puts it outside PxBoxCallback's file only.
#include "unknown_px_callbacks.h"
#include "px_callback_type.h"
#include "px_wrapper.h"
#include "px_callback_accurate.h"
#include "px_callback_poll.h"

// 0x452140
PxCallback *Unknown_452140(int type, PxObject *object, PxCallbackFunction callback, float radius)
{
	switch (type & ~PX_CALLBACK_POLLING) {
		case PX_CALLBACK_LIBERAL:
		{
			Vector3 extent(radius, radius, radius);
			PxBox *box;
			PxCallback *result;
			if (type & PX_CALLBACK_POLLING) {
				PxPollCallback *poll = new PxPollCallback(object, callback, extent);
				result = poll;
				box = poll;
			} else {
				PxBoxCallback *box_callback = new PxBoxCallback(object, callback, extent);
				result = box_callback;
				box = box_callback;
			}
			box->Set_Test(5);
			box->Set_Mask(5);
			return(result);
		}
		case PX_CALLBACK_CONSERVATIVE:
		{
			Vector3 extent(radius, radius, radius);
			extent *= 0.57735026f;
			PxBox *box;
			PxCallback *result;
			if (type & PX_CALLBACK_POLLING) {
				PxPollCallback *poll = new PxPollCallback(object, callback, extent);
				result = poll;
				box = poll;
			} else {
				PxBoxCallback *box_callback = new PxBoxCallback(object, callback, extent);
				result = box_callback;
				box = box_callback;
			}
			box->Set_Test(3);
			box->Set_Mask(3);
			return(result);
		}
		case PX_CALLBACK_ACCURATE:
			if (type & PX_CALLBACK_POLLING) {
				return(new PxPollAccurateCallback(object, callback, radius));
			}
			return(new PxAccurateSphereCallback(object, callback, radius));
	}
	return(NULL);
}

// 0x452460
PxCallback *Unknown_452460(int type, PxObject *object, PxCallbackFunction callback, const Vector3 &extent)
{
	switch (type & ~PX_CALLBACK_POLLING) {
		case PX_CALLBACK_LIBERAL:
		{
			PxBox *box;
			PxCallback *result;
			if (type & PX_CALLBACK_POLLING) {
				PxPollCallback *poll = new PxPollCallback(object, callback, extent);
				result = poll;
				box = poll;
			} else {
				PxBoxCallback *box_callback = new PxBoxCallback(object, callback, extent);
				result = box_callback;
				box = box_callback;
			}
			box->Set_Test(5);
			box->Set_Mask(5);
			return(result);
		}
		case PX_CALLBACK_CONSERVATIVE:
		{
			PxBox *box;
			PxCallback *result;
			if (type & PX_CALLBACK_POLLING) {
				PxPollCallback *poll = new PxPollCallback(object, callback, extent);
				result = poll;
				box = poll;
			} else {
				PxBoxCallback *box_callback = new PxBoxCallback(object, callback, extent);
				result = box_callback;
				box = box_callback;
			}
			box->Set_Test(3);
			box->Set_Mask(3);
			return(result);
		}
		case PX_CALLBACK_ACCURATE:
			if (type & PX_CALLBACK_POLLING) {
				return(new PxPollAccurateCallback(object, callback, extent));
			}
			return(new PxAccurateBoxCallback(object, callback, extent));
	}
	return(NULL);
}

/*
** Notes
**
** The callback factories 0x452140 / 0x452460 (PX3, 2026-10-29), after unknown_px_callbacks.cpp's PxBoxCallback
** (0x451C30-0x452130) and before px_callback_poll.cpp (from 0x452780); the guard 0x452750 ends the object. In
** PxBoxCallback's file VC6 inlines its constructor 0x451C30 into them (0.31); the client calls it, so the
** factories were compiled in an object of their own. One switch on the type without Polling; the EH states 0-5
** order the cases Liberal, Conservative, Accurate, with Polling first in each. PxWrapper::Unknown_456F60 /
** Unknown_457190 are their only callers; nothing calls those (packet-flow.md section 2.6).
*/
