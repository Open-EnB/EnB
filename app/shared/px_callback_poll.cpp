// Earth & Beyond client (reconstruction): app\shared\px_callback_poll.cpp. Notes at the end of the file.
#include "px_callback_poll.h"
#include "px_callback_type.h"
#include "px_wrapper.h"
#include "..\shared\unknown_debug.h"

// 0x452780
PxPollCallback::PxPollCallback(PxObject *object, PxCallbackFunction callback, const Vector3 &extent) :
	PxBoxList(object),
	Callback(callback),
	Extent(extent)
{
	Update();
}

// 0x452960
PxPollCallback::PxPollCallback(PxObject *object, const PxPollCallback &src) :
	PxBoxList(object, src),
	Callback(src.Callback),
	Extent(src.Extent)
{
	Update();
}

// 0x452AA0
void PxPollCallback::Process()
{
	for (std::vector<PxBox *>::iterator it = Boxes.begin(); it != Boxes.end(); it++) {
		Callback(((PxObject *)Owner)->Owner, ((PxObject *)(*it)->Owner)->Owner, PX_EVENT_UPDATE);
	}
}

// 0x452AF0
void PxPollCallback::On_Enter(PxBox *box)
{
	if (Boxes.empty()) {
		PxProcessClass::Get_Current()->Add(this);
	}
	PxBoxList::On_Enter(box);
}

// 0x452B40
void PxPollCallback::On_Exit(PxBox *box)
{
	PxBoxList::On_Exit(box);
	if (Boxes.empty()) {
		PxProcessClass::Get_Current()->Remove(this);
	}
}

// 0x452BB0
PxCallback *PxPollCallback::Clone(PxObject *object)
{
	return(new PxPollCallback(object, *this));
}

// 0x452D10
void PxPollCallback::Attach(PxList *list)
{
	list->Insert(this);
}

// 0x452D30
void PxPollCallback::Detach(PxList *list)
{
	list->Remove(this);
}

// 0x452D50
void PxPollCallback::Update()
{
	PxObject *object = (PxObject *)Owner; Vector3 pos = object->Pos;
	Set_Corners(pos - Extent, pos + Extent);
}

// 0x452DE0
void PxPollCallback::Add_Filter(int mask, int test)
{
	Set_Test_Mask(Test | test, Mask | mask);
}

// 0x452E10
PxPollAccurateCallback::PxPollAccurateCallback(PxObject *object, PxCallbackFunction callback, float radius) :
	PxPollCallback(object, callback, Vector3(radius, radius, radius)),
	Type(SPHERE)
{
	Set_Test_Mask(5, 5);
}

// 0x453030
PxPollAccurateCallback::PxPollAccurateCallback(PxObject *object, PxCallbackFunction callback, const Vector3 &extent) :
	PxPollCallback(object, callback, extent),
	Type(VOLUME)
{
	Set_Test_Mask(5, 5);
}

// 0x4531A0
PxPollAccurateCallback::PxPollAccurateCallback(PxObject *object, const PxPollAccurateCallback &src) :
	PxPollCallback(object, src),
	Type(src.Type)
{
}

// 0x453300
void PxPollAccurateCallback::Unknown_453300()
{
	Set_Test_Mask(5, 5);
}

// 0x453320
PxCallback *PxPollAccurateCallback::Clone(PxObject *object)
{
	return(new PxPollAccurateCallback(object, *this));
}

// (inlined into Process 0x4534A0)
inline void PxPollAccurateCallback::Process_Volume()
{
	Vector3 low; Vector3 high;
	Get_Corners(&low, &high);
	for (std::vector<PxBox *>::iterator it = Boxes.begin(); it != Boxes.end(); it++) {
		PxObject *other = (PxObject *)(*it)->Owner;
		float r = other->Radius;
		if (other->Pos.X < high.X + r && low.X - r < other->Pos.X &&
			other->Pos.Y < high.Y + r && low.Y - r < other->Pos.Y &&
			other->Pos.Z < high.Z + r && low.Z - r < other->Pos.Z) {
			Callback(((PxObject *)Owner)->Owner, other->Owner, PX_EVENT_UPDATE);
		}
	}
}

// 0x4534A0
void PxPollAccurateCallback::Process()
{
	switch (Type) {
	case SPHERE: {
		const Vector3 &pos = ((PxObject *)Owner)->Pos;
		float radius = Extent.X;
		for (std::vector<PxBox *>::iterator it = Boxes.begin(); it != Boxes.end(); it++) {
			PxObject *other = (PxObject *)(*it)->Owner;
			Vector3 delta = other->Pos - pos;
			float dist = other->Radius; dist += radius;
			if (delta.Length2() < dist * dist) {
				Callback(((PxObject *)Owner)->Owner, other->Owner, PX_EVENT_UPDATE);
			}
		}
		break; }
	case VOLUME:
		Process_Volume();
		break;
	}
}



























































// 0x453660
void PxPollAccurateCallback::Set_Radius(float radius)
{
	FAIL_IF(Type != SPHERE) return;
	Extent.X = radius; Extent.Y = radius; Extent.Z = radius;
}

// 0x4536B0
void PxPollAccurateCallback::Set_Extent(const Vector3 &extent)
{
	FAIL_IF(Type != VOLUME) return;
	Extent = extent;
}

/*
** Notes
**
** px_callback_poll.cpp is client 0x452780-0x453740 (its __FILE__ in the two FAIL_IFs, lines 216 / 223), after the
** object 0x451C00-0x452770 that holds the callback factories 0x452140 / 0x452460 (no string names it) and before
** px_callback_type.cpp. 0x453700-0x453730 are the compiler's this-adjusting thunks for the two classes' node and
** callback vtables. The class and function names are provisional; Type, SPHERE and VOLUME are the FAIL_IFs'.
**
** PxPollCallback is a PxBoxList, a PxProcessNode (+0x30) and a PxCallback (+0x3C): its first overlap puts it in
** PxProcessClass::Get_Current(), its last takes it out, and Process reports every overlapping box's object with
** PX_EVENT_UPDATE. PxPollAccurateCallback filters those reports by a sphere (Extent.X as radius plus the other's)
** or by its volume grown by the other's radius. Nothing calls PxProcessClass::Process (px_process.cpp's notes), so in
** the retail client these Process functions never run.
*/
