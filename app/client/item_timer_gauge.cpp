// Earth & Beyond client (reconstruction): app\client\item_timer_gauge.cpp. Notes at the end of the file.
#include "item_timer_gauge.h"
#include "rendobj.h"
#include "assetmgr.h"
bool Unknown_496110(RenderObjClass *robj, float a, float b, float c);
#include "matrix3d.h"
#include "..\shared\g_assert.h"

















































// 0x691170
ItemTimerGaugeClass::ItemTimerGaugeClass(RenderObjClass *attach) :
	RObj(NULL),
	TimerRObj(NULL),
	TimerBone(0)
{
	FAIL_IF(!attach) {
		return;
	}
	RObj = WW3DAssetManager::Get_Instance()->Create_Render_Obj("ItemTime");
	FAIL_IF(!RObj) {
		return;
	}
	TimerRObj = RObj->Get_Sub_Object_By_Name("timer");
	FAIL_IF(!TimerRObj) {
		return; } TimerBone = RObj->Get_Bone_Index("timer");
	RObj->Capture_Bone(TimerBone);
	RenderObjClass *container;
	if ((container = attach->Get_Container()) == NULL) {
		container = attach; }
	int bone = container->Get_Sub_Object_Bone_Index(attach);
	container->Add_Sub_Object_To_Bone(RObj, bone);
}

// 0x6912A0
ItemTimerGaugeClass::~ItemTimerGaugeClass()
{
	if (RObj) {
		RObj->Release_Bone(TimerBone);
		RObj->Remove();
		REF_PTR_RELEASE(RObj);
	}
	REF_PTR_RELEASE(TimerRObj);
}

// 0x691320
void ItemTimerGaugeClass::Update(unsigned int time)
{
	if (!TimerRObj) return;
	if (!time) {
		if (!RObj->Is_Hidden()) RObj->Set_Hidden(1);
		return;
	}
	if (RObj->Is_Hidden()) RObj->Set_Hidden(0);
	float turns;
	Vector3 color;
	if (time > 60000) {
		turns = time * (1.0f / 3600000.0f);
		color.Set(0.0f, 0.25f, 1.0f);
	} else {
		turns = time * (1.0f / 60000.0f);
		color.Set(0.12f, 0.924f, 0.012f);
	}
	float angle = -(turns * 6.2831855f);
	Matrix3D tm(true);
	tm.Rotate_Z(angle);
	RObj->Control_Bone(TimerBone, tm, false);
	if (Last != color) {
		Last = color;
		Unknown_496110(TimerRObj, Last.X, Last.Y, Last.Z);
	}
}

/*
**	Reconstruction notes (item_timer_gauge.cpp, client 0x691170-0x6914E8; __FILE__ 0xB907F0, FAIL_IF '!attach' / '!RObj' /
**	'!TimerRObj' on the client's lines 63 / 67 / 71 in 0x691170).
**
**	- No client function calls the constructor, the destructor or Update: the class is compiled but unused.
**	- The constructor makes 'ItemTime' (WW3DAssetManager 0xBFAEE0's Create_Render_Obj), finds its 'timer' sub object,
**	  captures RObj's 'timer' bone and adds the model to attach's container (or attach) on attach's own bone. Each
**	  FAIL_IF returns from a braced block (the client's lines are four apart). NOT EXACT: the client stores the three
**	  zeros as immediates and tests with test; ours keeps 0 in ebp (register class).
**	- Update hides the model at 0, else turns the 'timer' bone by -2 pi * the time (minutes up to 60000 ms, hours
**	  above) through Control_Bone (an identity Matrix3D and Rotate_Z: the client's 'cos + 0', '0 - sin' terms) and
**	  colours TimerRObj through 0x496110 when the colour changes ((0, 0.25, 1) for hours, (0.12, 0.924, 0.012) for
**	  minutes, kept in Last). NOT EXACT: the client stores the angle and reloads it before fcos, ours keeps it (fst).
*/
