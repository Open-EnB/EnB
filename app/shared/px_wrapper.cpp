// Earth & Beyond client (reconstruction): app\shared\px_wrapper.cpp. Notes at the end of the file.
#define ENABLE_WWPROFILE
#include "px_wrapper.h"
#include "px.h"
#include "..\shared\unknown_debug.h"
#include "wwprofile.h"
#include "wwmath.h"
#include <algorithm>

static void _Collision_Callback(PxBox *a, PxBox *b);

// 0x4560B0
PxWrapper::PxWrapper(DynObj *owner) :
	Owner(owner),
	Object(NULL),
	List(NULL),
	Unknown0C(false),
	Unknown0D(false),
	Unknown0E(false),
	TightBox(NULL),
	LooseBox(NULL),
	CallbackBox(NULL)
{
	Create_Boxes();
	Unknown_4567D0(true);
}

// 0x456190
PxWrapper::PxWrapper(DynObj *owner, const PxWrapper &src) :
	Owner(owner),
	Object(NULL),
	List(NULL),
	Unknown0C(false),
	Unknown0D(false),
	Unknown0E(false),
	TightBox(NULL),
	LooseBox(NULL),
	CallbackBox(NULL)
{
	Create_Boxes();
	for (std::vector<PxCallback *>::const_iterator it = src.Callbacks.begin(); it != src.Callbacks.end(); it++) {
		PxCallback *callback = (*it)->Clone(Object);
		if (callback) {
			Callbacks.push_back(callback);
		}
	}
	Unknown_456840(src.Unknown0C);
	Unknown_456890(src.Unknown0D);
	Unknown_4567D0(src.Unknown0E);
}

// 0x456350
void PxWrapper::Create_Boxes()
{
	Object = new PxObject(Owner);
	TightBox = new PxBox(Object);
	TightBox->Set_Self(3);
	LooseBox = new PxCollisionBox(Object, _Collision_Callback, Vector3(0.0f, 0.0f, 0.0f));
	LooseBox->Set_Self(5);
	LooseBox->Set_Test(0x1D);
	LooseBox->Set_Mask(0x15);
	CallbackBox = new PxOwnerList(Object);
	CallbackBox->Set_Test(5);
	CallbackBox->Set_Mask(5);
}







































// 0x456500
PxWrapper::~PxWrapper()
{
	FAIL_IF(List) {
		Unknown_456730();
	}
	for (std::vector<PxCallback *>::iterator it = Callbacks.begin(); it != Callbacks.end(); it++) {
		delete *it;
	}
	Callbacks.clear();
	delete CallbackBox;
	CallbackBox = NULL;
	delete LooseBox;
	LooseBox = NULL;
	delete TightBox;
	TightBox = NULL;
	delete Object;
	Object = NULL;
}

// 0x456660
void PxWrapper::Add_To_List(PxList *list)
{
	FAIL_IF(!list) return;


	FAIL_IF(List) return;
	List = list;
	list->Insert(LooseBox);
	list->Insert(TightBox);
	if (Unknown0D) {
		list->Insert(CallbackBox);
	}
	for (std::vector<PxCallback *>::iterator it = Callbacks.begin(); it != Callbacks.end(); it++) {
		(*it)->Attach(list);
	}
}









// 0x456730
void PxWrapper::Unknown_456730()
{
	FAIL_IF(!List) return;
	List->Remove(TightBox);
	List->Remove(LooseBox);
	if (Unknown0D) {
		List->Remove(CallbackBox);
	}
	for (std::vector<PxCallback *>::iterator it = Callbacks.begin(); it != Callbacks.end(); it++) {
		(*it)->Detach(List);
	}
	List = NULL;
}

// 0x4567D0
void PxWrapper::Unknown_4567D0(bool value)
{
	if (Unknown0E != value) {
		Unknown0E = value;
		LooseBox->Set_Self(LooseBox->Self ^ 0x10);
		LooseBox->Set_Test_Mask(LooseBox->Test ^ 0x10, LooseBox->Mask ^ 0x10);
		TightBox->Set_Self(TightBox->Self ^ 0x10);
	}
}

// 0x456840
void PxWrapper::Unknown_456840(bool value)
{
	if (Unknown0C != value) {
		Unknown0C = value;
		LooseBox->Set_Self(LooseBox->Self ^ 8);
		LooseBox->Set_Mask(LooseBox->Mask ^ 8);
	}
}

// 0x456890
void PxWrapper::Unknown_456890(bool value)
{
	if (Unknown0D != value) {
		if (List) {
			if (value) {
				List->Insert(CallbackBox);
			} else {
				List->Remove(CallbackBox);
			}
		}
		Unknown0D = value;
	}
}

// 0x4568E0
std::vector<DynObj *> &PxWrapper::Unknown_4568E0()
{
	return(CallbackBox->Owners);
}

static bool Is_Close(const Vector3 &a, const Vector3 &b)  // NAME 2: what it tests
{
	return(WWMath::Fabs(a.X - b.X) < 0.1f && WWMath::Fabs(a.Y - b.Y) < 0.1f && WWMath::Fabs(a.Z - b.Z) < 0.1f);
}

static bool Is_Close(float a, float b)  // NAME 2: as above
{
	return(WWMath::Fabs(a - b) < 0.1f);
}



























// 0x456900
void PxWrapper::Unknown_456900(const Vector3 &low, const Vector3 &high)
{
	FAIL_IF(!low.Is_Valid()) return;
	FAIL_IF(!high.Is_Valid()) return;
	if (Is_Close(low, CallbackBox->Low->Pos) && Is_Close(high, CallbackBox->High->Pos)) {
		return;
	}
	CallbackBox->Set_Corners(low, high);
}














// 0x456B60
// NOT EXACT: shape (the client loads Object->Radius before radius: fld; fsub st(1); attempts.py show 0x456B60)
void PxWrapper::Update(const Vector3 &pos, float radius)
{
	FAIL_IF(!pos.Is_Valid()) return;
	FAIL_IF(!WWMath::Is_Valid_Float(radius)) return;
	if (Is_Close(pos, Object->Pos) && Is_Close(radius, Object->Radius)) {
		return;
	}
	Object->Pos = pos;
	Object->Radius = radius;
	{
		WWPROFILE("PxWrapper::Update(LooseBox");	// the client's string, its parenthesis unclosed
		Vector3 extent(radius, radius, radius);
		LooseBox->Set_Corners(pos - extent, pos + extent);
	}
	{
		Vector3 extent(radius * 0.57735026f, radius * 0.57735026f, radius * 0.57735026f);
		WWPROFILE("PxWrapper::Update(TightBox)");
		TightBox->Set_Corners(pos - extent, pos + extent);
	}
	{
		WWPROFILE("PxWrapper::Update(Callbacks)");
		for (std::vector<PxCallback *>::iterator it = Callbacks.begin(); it != Callbacks.end(); it++) {
			(*it)->Update();
		}
	}
}

// 0x456F60
PxCallback *PxWrapper::Unknown_456F60(int a, int b, int c)
{
	PxCallback *callback = Unknown_452140(a, Object, b, c);
	Add_Callback(callback);
	return(callback);
}

// 0x457190
PxCallback *PxWrapper::Unknown_457190(int a, int b, int c)
{
	PxCallback *callback = Unknown_452460(a, Object, b, c);
	Add_Callback(callback);
	return(callback);
}

// 0x4573C0
void PxWrapper::Add_Callback(PxCallback *callback)
{
	Callbacks.push_back(callback);
	if (List) {
		callback->Attach(List);
	}
}

// 0x4575E0
bool PxWrapper::Unknown_4575E0(void *callback)
{
	std::vector<PxCallback *>::iterator it = std::find(Callbacks.begin(), Callbacks.end(), callback);
	if (it == Callbacks.end()) {
		return(false);
	}
	if (List) {
		((PxCallback *)callback)->Detach(List);
	}
	Callbacks.erase(it);
	return(true);
}

// 0x457670
static void _Collision_Callback(PxBox *a, PxBox *b)
{
	WWPROFILE("_Collision_Callback");
	if (a > b) return;
	PxCollision collision(a, b);
	collision.Process();
}

/*
** Notes
**
** px_wrapper.cpp is client 0x4560B0-0x457740 (its __FILE__; FAIL_IFs lines 108-273), after px_process.cpp. The class
** name PxWrapper and _Collision_Callback are the client's (their profile strings); the member and function names
** other than List, TightBox, LooseBox and Callbacks are provisional, as are the declarations of the other px_*.cpp
** classes in px_wrapper.h (PxObject, PxCallback, PxCollisionBox, PxCollision; px_box_list.h has PxBoxList / PxOwnerList).
**
** The constructors build the boxes (Create_Boxes: the tight box Self 3, the loose box Self 5 / Test 0x1D / Mask 0x15
** reporting to _Collision_Callback, the callback box Test 5 / Mask 5); the copy clones the source's callbacks and
** copies its three flags through their setters (which flip bits 8 and 0x10 of the boxes' filters). Update sets the
** loose box to the radius around pos and the tight box to radius / sqrt(3) (0.57735026), skipping changes under 0.1,
** then updates every callback; Unknown_456900 sets the callback box's corners. _Collision_Callback hands each pair
** of loose boxes (once, the lower address first) to px_collision.cpp's PxCollision::Process.
**
** The vector<PxCallback *> members 0x457780-0x457B30 are this file's instances; 0x456080 (the vector's construct
** helper) precedes the constructor.
*/
