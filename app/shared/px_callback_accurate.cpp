// Earth & Beyond client (reconstruction): app\shared\px_callback_accurate.cpp. Notes at the end of the file.
#include "px_callback_accurate.h"
#include "px_callback_type.h"
#include "px_collision.h"
#include "..\shared\unknown_debug.h"

// 0x44E020
// NOT EXACT: inline budget (the client calls __stl_next_prime, the buckets' reserve 0x451420 and insert 0x451520 out of line; ours inlines them; attempts.py show 0x44E020)
PxAccurateCallback::PxAccurateCallback(PxObject *object, PxCallbackFunction callback) :
	Object(object),
	Callback(callback),
	Overlaps(100),
	Active(0)
{
}

// 0x44E260
// NOT EXACT: inline budget (as 0x44E020; attempts.py show 0x44E260)
PxAccurateCallback::PxAccurateCallback(PxObject *object, const PxAccurateCallback &src) :
	Object(object),
	Callback(src.Callback),
	Overlaps(100),
	Active(0)
{
}

// 0x44E390
PxAccurateCallback::~PxAccurateCallback()
{
}

// 0x44F2F0
void PxAccurateCallback::Process()
{
	for (std::vector<PxAccurateActiveStruct>::iterator it = Active.begin(), end = Active.end(); it != end; it++) {
		PxObject *object = it->Object;
		bool inside = Test(object);
		if (inside != it->Inside) {
			if (inside) {
				Enter(object);
			} else {
				Exit(object);
			}
			it->Inside = inside;
		}
	}
}

// 0x44F370
void PxAccurateCallback::Enter(PxObject *object)
{
	Callback(Object->Owner, object->Owner, PX_EVENT_ENTER);
}

// 0x44F3A0
void PxAccurateCallback::Exit(PxObject *object)
{
	Callback(Object->Owner, object->Owner, PX_EVENT_EXIT);
}

/*
** Notes
**
** px_callback_accurate.cpp is client 0x44E020-0x451BF0 (its guarded <string> static at 0x451BD0), after
** px_box_list.cpp; its __FILE__ is pushed by the FAIL_IFs 'it == Overlaps.end()' (l.133) and
** 'it == Active.end() || it->Object != object' (l.170) in 0x44E470-0x44F6E0. Only the base class's constructors,
** destructor, Process, Enter and Exit are rebuilt (0x44E020-0x44E390, 0x44F2F0-0x44F3A0); the overlap handling
** 0x44E470-0x44F200, the two derived classes (vtables 0xAE7B44 / 0xAE7B54 / 0xAE7B70: a box, 0x44F3D0-0x44FF40;
** 0xAE7B8C / 0xAE7BA8: 0x450030-0x450AC0) and the hash_map / vector instances 0x450B40-0x451B60 are not. They sit
** in the proximity system's dormant path: PxAccurateCallback::Process runs only from PxProcessClass::Process.
*/
