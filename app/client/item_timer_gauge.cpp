// Earth & Beyond client (reconstruction): app\client\item_timer_gauge.cpp. Notes at the end of the file.
#include "item_timer_gauge.h"
#include "rendobj.h"
#include "assetmgr.h"
#include "..\shared\g_assert.h"


















































// NOT EXACT (0.63, attempts.py 0x691170): register: the client stores the three zeros as immediates, ours keeps 0 in ebp
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
		return;
	}
	TimerBone = RObj->Get_Bone_Index("timer");
	RObj->Capture_Bone(TimerBone);
	RenderObjClass *container = attach->Get_Container();
	if (!container) container = attach;
	container->Add_Sub_Object_To_Bone(RObj, container->Get_Sub_Object_Bone_Index(attach));
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

/*
**	Reconstruction notes (item_timer_gauge.cpp, client 0x691170-0x6914E8; __FILE__ 0xB907F0, FAIL_IF '!attach' / '!RObj' /
**	'!TimerRObj' on the client's lines 63 / 67 / 71 in 0x691170).
**
**	- No client function calls the constructor, the destructor or 0x691320: the class is compiled but unused.
**	- The constructor makes 'ItemTime' (WW3DAssetManager 0xBFAEE0's Create_Render_Obj), finds its 'timer' sub object,
**	  captures RObj's 'timer' bone and adds the model to attach's container (or attach) on attach's own bone. Each
**	  FAIL_IF returns from a braced block (the client's lines are four apart). NOT EXACT: the client stores the three
**	  zeros as immediates and tests with test; ours keeps 0 in ebp (register class).
**	- 0x691320 (Update, not rebuilt yet): hides the model at 0, else turns the 'timer' bone by -2 pi * the time
**	  (minutes up to 60000 ms, hours above) through Control_Bone (+0xC8) and colours TimerRObj through 0x496110 when the
**	  colour changes ((0, 0.25, 1) for hours, (0.12, 0.924, 0.012) for minutes, kept in Last).
*/
