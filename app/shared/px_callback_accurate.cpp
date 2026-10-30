// Earth & Beyond client (reconstruction): app\shared\px_callback_accurate.cpp. Notes at the end of the file.
#include "px_callback_accurate.h"
#include "px_callback_type.h"
#include "px_collision.h"
#include <algorithm>
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

// 0x44E470
// NOT EXACT: inline budget (the client inlines Activate / Deactivate in the exit half only and calls the hash_map's find_or_insert 0x4517C0 out of line; ours inlines Deactivate in Tight_Enter; attempts.py show 0x44E470)
void PxAccurateCallback::Overlap(PxBox *box, bool enter)
{
	if (enter) {
		if (box->Self & 2) {
			Tight_Enter((PxObject *)box->Owner);
		} else {
			Loose_Enter((PxObject *)box->Owner);
		}
	} else {
		if (box->Self & 2) {
			Tight_Exit((PxObject *)box->Owner);
		} else {
			Loose_Exit((PxObject *)box->Owner);
		}
	}
}

// 0x44E780
void PxAccurateCallback::Tight_Enter(PxObject *object)
{
	PxAccurateOverlapStruct &overlap = Overlaps[object];
	if (overlap.State == 1) {
		Deactivate(object, overlap.Flag);
	}
	if (!overlap.Flag) {
		Enter(object);
	}
	overlap.State = 2;
	overlap.Flag = true;
}

// 0x44E960
void PxAccurateCallback::Loose_Enter(PxObject *object)
{
	PxAccurateOverlapStruct &overlap = Overlaps[object];
	if (overlap.State != 2) {
		overlap.State = 1;
		Activate(object, overlap.Flag);
	}
}

// 0x44EAE0
void PxAccurateCallback::Tight_Exit(PxObject *object)
{
	std::hash_map<PxObject *, PxAccurateOverlapStruct>::iterator it = Overlaps.find(object);
	if (it != Overlaps.end()) {
		PxAccurateOverlapStruct &overlap = (*it).second;
		bool inside = overlap.Flag;
		overlap.State = 1;
		Activate(object, inside);
	}
}










































// 0x44EDB0
void PxAccurateCallback::Loose_Exit(PxObject *object)
{
	std::hash_map<PxObject *, PxAccurateOverlapStruct>::iterator it = Overlaps.find(object);
	FAIL_IF(it == Overlaps.end()) return;
	PxAccurateOverlapStruct &overlap = (*it).second;
	if (overlap.State == 1) {
		Deactivate(object, overlap.Flag);
	}
	if (overlap.Flag) {
		Exit(object);
	}
	Overlaps.erase(it);
}

// 0x44EF80
void PxAccurateCallback::Activate(PxObject *object, bool inside)
{
	if (Active.empty()) {
		PxProcessClass::Get_Current()->Add(this);
	}
	PxAccurateActiveStruct active;
	active.Object = object;
	active.Inside = inside;
	std::vector<PxAccurateActiveStruct>::iterator it = std::lower_bound(Active.begin(), Active.end(), active);
	Active.insert(it, active);
}









// 0x44F200
void PxAccurateCallback::Deactivate(PxObject *object, bool &inside)
{
	PxAccurateActiveStruct key = { object, false };
	std::vector<PxAccurateActiveStruct>::iterator it = std::lower_bound(Active.begin(), Active.end(), key);
	FAIL_IF(it == Active.end() || it->Object != object) return;
	inside = it->Inside;
	Active.erase(it);
	if (Active.empty()) {
		PxProcessClass::Get_Current()->Remove(this);
	}
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

// 0x44F5D0
// NOT EXACT: inline budget (the client calls Activate 0x44EF80 out of line, ours inlines it; attempts.py show 0x44F5D0)
void PxAccurateBox::On_Enter(PxBox *box)
{
	Callback->Overlap(box, true);
}

// 0x44F6E0
// NOT EXACT: inline budget (as 0x44F5D0; attempts.py show 0x44F6E0)
void PxAccurateBox::On_Exit(PxBox *box)
{
	Callback->Overlap(box, false);
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

// 0x450030
// NOT EXACT: inline budget (as 0x44F3D0)
PxAccurateSphereCallback::PxAccurateSphereCallback(PxObject *object, PxCallbackFunction callback, float radius) :
	PxAccurateCallback(object, callback),
	InnerBox(NULL),
	OuterBox(NULL),
	Radius(radius)
{
	InnerBox = new PxAccurateBox(object, this);
	InnerBox->Set_Test(3);
	InnerBox->Set_Mask(3);
	OuterBox = new PxAccurateBox(object, this);
	OuterBox->Set_Test(5);
	OuterBox->Set_Mask(5);
	Update();
}

// 0x450350
// NOT EXACT: inline budget (as 0x44F3D0)
PxAccurateSphereCallback::PxAccurateSphereCallback(PxObject *object, const PxAccurateSphereCallback &src) :
	PxAccurateCallback(object, src),
	InnerBox(NULL),
	OuterBox(NULL),
	Radius(src.Radius)
{
	InnerBox = new PxAccurateBox(object, *src.InnerBox, this);
	OuterBox = new PxAccurateBox(object, *src.OuterBox, this);
	Update();
}

// 0x450620
// NOT EXACT: inline budget (it inlines the copy constructor 0x450350)
PxCallback *PxAccurateSphereCallback::Clone(PxObject *object)
{
	return(new PxAccurateSphereCallback(object, *this));
}

// 0x4506B0
PxAccurateSphereCallback::~PxAccurateSphereCallback()
{
	delete InnerBox;
	InnerBox = NULL;
	delete OuterBox;
	OuterBox = NULL;
}

// 0x450820
void PxAccurateSphereCallback::Attach(PxList *list)
{
	list->Insert(OuterBox);
	list->Insert(InnerBox);
}

// 0x450860
void PxAccurateSphereCallback::Detach(PxList *list)
{
	list->Remove(InnerBox);
	list->Remove(OuterBox);
}

// 0x4508A0
void PxAccurateSphereCallback::Update()
{
	const Vector3 &pos = Object->Pos;
	Vector3 extent(Radius, Radius, Radius);
	OuterBox->Set_Corners(pos - extent, extent + pos);
	extent *= 0.57735026f;
	InnerBox->Set_Corners(pos - extent, extent + pos);
}

// 0x4509E0
void PxAccurateSphereCallback::Add_Filter(int mask, int test)
{
	InnerBox->Set_Test_Mask(InnerBox->Test | test, InnerBox->Mask | mask);
	OuterBox->Set_Test_Mask(OuterBox->Test | test, OuterBox->Mask | mask);
}

// 0x450A30
void PxAccurateSphereCallback::Set_Filter(int mask, int test)
{
	InnerBox->Set_Test_Mask(test | 2, mask | 2);
	OuterBox->Set_Test_Mask(test | 4, mask | 4);
}

// 0x450A80
void PxAccurateSphereCallback::Set_Self(int self)
{
	InnerBox->Set_Self(self | 2);
	OuterBox->Set_Self(self | 4);
}

// 0x450AC0
// NOT EXACT: order (the client sums the two radii and squares dz between the other products; attempts.py show 0x450AC0)
bool PxAccurateSphereCallback::Test(PxObject *object)
{
	Vector3 delta = object->Pos - Object->Pos;
	float dist = object->Radius; dist += Radius;
	if (delta.Length2() < dist * dist) return(true); return(false);
}

/*
** Notes
**
** px_callback_accurate.cpp is client 0x44E020-0x451BF0 (its guarded <string> static at 0x451BD0), after
** px_box_list.cpp; its __FILE__ is pushed by the FAIL_IFs 'it == Overlaps.end()' (l.133) and
** 'it == Active.end() || it->Object != object' (l.170) in 0x44E470-0x44F6E0 (the blank lines before Loose_Exit and
** Deactivate keep them there). Every function from 0x44E020 to 0x451B60 is in source.
**
** The overlap handling (PX3, 2026-10-29). A box b reaching an accurate callback's box is either a tight one (Self
** bit 2: the other object's sphere certainly touches) or a loose one (it may). Overlaps keeps, per object, State 1
** (only loose: the object is in Active and Process tests it each frame) or 2 (tight: no test needed) and Flag (Enter
** reported). Tight_Enter takes it out of Active and reports Enter once; Loose_Enter puts it in Active unless tight;
** Tight_Exit puts it back in Active; Loose_Exit takes it out, reports Exit if it was in and forgets it. Activate /
** Deactivate keep Active sorted and add / remove the node to / from PxProcessClass::Get_Current() as it fills and
** empties. Overlap 0x44E470, Tight_Enter .. Deactivate and the hash_map's Overlaps have no caller (client.py refs):
** only PxAccurateBox::On_Enter / On_Exit (vtable 0xAE7B44) reach this code, through Overlap inlined. The template
** instances 0x450B40-0x451B60 come from these functions (vector insert / erase / _Insert_n / _Ufill / _Ucopy,
** the hashtable's constructor / find / find_or_insert / erase / resize / bucket vector, _Lower_bound).
** They sit in the proximity system's dormant path: PxAccurateCallback::Process runs only from PxProcessClass::Process.
*/
