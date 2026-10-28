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

// 0x44F3D0
// NOT EXACT: inline budget (the base's hash_map constructor is called out of line, 0x450DB0)
PxAccurateBoxCallback::PxAccurateBoxCallback(PxObject *object, PxCallbackFunction callback, const Vector3 &extent) :
	PxAccurateCallback(object, callback),
	Box(NULL),
	Extent(extent)
{
	Box = new PxAccurateBox(object, this);
	Box->Set_Test(1);
	Box->Set_Mask(1);
	Update();
}

// 0x44F8F0
// NOT EXACT: inline budget (as 0x44F3D0)
PxAccurateBoxCallback::PxAccurateBoxCallback(PxObject *object, const PxAccurateBoxCallback &src) :
	PxAccurateCallback(object, src),
	Box(NULL),
	Extent(src.Extent)
{
	Box = new PxAccurateBox(object, *src.Box, this);
	Update();
}

// 0x44FAD0
// NOT EXACT: inline budget (it inlines the copy constructor 0x44F8F0)
PxCallback *PxAccurateBoxCallback::Clone(PxObject *object)
{
	return(new PxAccurateBoxCallback(object, *this));
}

// 0x44FCF0
PxAccurateBoxCallback::~PxAccurateBoxCallback()
{
	delete Box;
	Box = NULL;
}

// 0x44FE50
void PxAccurateBoxCallback::Attach(PxList *list)
{
	list->Insert(Box);
}

// 0x44FE70
void PxAccurateBoxCallback::Detach(PxList *list)
{
	list->Remove(Box);
}

// 0x44FE90
void PxAccurateBoxCallback::Update()
{
	const Vector3 &pos = Object->Pos;
	Box->Set_Corners(pos - Extent, Extent + pos);
}

// 0x44FF10
void PxAccurateBoxCallback::Add_Filter(int mask, int test)
{
	Box->Set_Test_Mask(Box->Test | test, Box->Mask | mask);
}

// 0x44FF40
bool PxAccurateBoxCallback::Test(PxObject *object)
{
	float x = object->Pos.X;
	float y = object->Pos.Y;
	float z = object->Pos.Z;
	float r = object->Radius;
	Vector3 low;
	Vector3 high;
	Box->Get_Corners(&low, &high);
	return(x < high.X + r && low.X - r < x && y < high.Y + r && low.Y - r < y && z < high.Z + r && low.Z - r < z);
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
