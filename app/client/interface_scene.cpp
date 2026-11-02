// Earth & Beyond client (reconstruction): app\Client\interface_scene.cpp. Notes at the end of the file.
#include "interface_scene.h"
#include "interface_static_sort_list.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\..\ww3d2\camera.h"
#include "..\..\ww3d2\predlod.h"
#include "..\shared\unknown_debug.h"
#pragma inline_depth(0)	// the client calls the list's inline constructor 0x6DA930 out of line (a code-generation lever, as fontdatabase.cpp's auto_inline: no inlining in this constructor)
// 0x6DA4D0
InterfaceSceneClass::InterfaceSceneClass() :
	InsideSortListRender(false),
	InsideRenderWithPIPScenes(false),
	SSL(NULL)
{
	SSL = new InterfaceStaticSortListClass;
}
#pragma inline_depth()
// 0x6DA5C0 (scalar deleting 0x6DA590)
InterfaceSceneClass::~InterfaceSceneClass()
{
	delete SSL;
	SSL = NULL;
}

// 0x6DA650
void InterfaceSceneClass::Add_Render_Object(RenderObjClass *obj)
{
	SimpleSceneClass::Add_Render_Object(obj);
	SSL->Add_Render_Object(obj);
}

// 0x6DA680
void InterfaceSceneClass::Remove_Render_Object(RenderObjClass *obj)
{
	SimpleSceneClass::Remove_Render_Object(obj);
	SSL->Remove_Render_Object(obj);
}

// 0x6DA6B0
void InterfaceSceneClass::Visibility_Check(CameraClass *camera)
{
	SimpleSceneClass::Visibility_Check(camera);
	PredictiveLODOptimizerClass::Add_Cost(SSL->PIPPolyBudget);
}

// 0x6DA6F0
bool InterfaceSceneClass::Add_PIP_Scene(const RefCountPtr<SimpleSceneClass> &scene, const RefCountPtr<CameraClass> &camera, float poly_budget, unsigned int sort_level)
{
	return(SSL->Add_PIP_Scene(scene, camera, poly_budget, sort_level));
}

// 0x6DA730
bool InterfaceSceneClass::Remove_PIP_Scene(const RefCountPtr<SimpleSceneClass> &scene)
{
	return(SSL->Remove_PIP_Scene(scene));
}

// 0x6DA760
void InterfaceSceneClass::Render_With_PIP_Scenes(const RefCountPtr<CameraClass> &camera)
{
	InsideRenderWithPIPScenes = true;
	WW3D::Override_Current_Static_Sort_Lists(SSL);
	InsideSortListRender = false;
	WW3D::Render(this, camera.Peek(), false, false, Vector3(0.0f, 0.0f, 0.0f));
	InsideSortListRender = true;
	RefCountPtr<SceneClass> scene = Create_Peek((SceneClass *)this);
	SSL->Render(scene, camera);
	WW3D::Reset_Current_Static_Sort_Lists_To_Default();
	InsideRenderWithPIPScenes = false;
}









































































// 0x6DA880
void InterfaceSceneClass::Customized_Render(RenderInfoClass &rinfo)
{
	FAIL_IF(!InsideRenderWithPIPScenes) return;
	if (!InsideSortListRender) {
		SimpleSceneClass::Customized_Render(rinfo);
	}
}

/*
**	Reconstruction notes
**
**	The client's interface_scene.obj runs 0x6DA4D0-0x6DADDF: the class's functions to 0x6DA8BB, then
**	Create_Peek<SceneClass> 0x6DA8D0, <string>'s guarded static 0x6DA900 (0xC416FC, atexit 0x42B320), and the
**	inline functions of interface_static_sort_list.h it instantiates by building the list: the constructor
**	0x6DA930, StaticSortListClass's destructors 0x6DACA0 / 0x6DACC0, Render_And_Clear 0x6DACF0,
**	InterfaceRenderClass's destructors 0x6DAD10 / 0x6DAD40 and the list's scalar deleting destructor 0x6DADB0.
**	The one assert (FAIL_IF '!InsideRenderWithPIPScenes') sits on the client's line 147 (D25).
**	0xC0754C is PredictiveLODOptimizerClass::TotalCost (its Clear 0x96F060 zeroes it): Visibility_Check adds
**	the PIP scenes' budget through Add_Cost. 0x919070 / 0x919080 are WW3D::Override_Current_Static_Sort_Lists
**	and Reset_Current_Static_Sort_Lists_To_Default (0xBFAE6C the current list, 0xBFAE68 the default).
**	The three hash_map destructors 0x6DADE0 / 0x6DAEB0 / 0x6DAF80 after the list's scalar deleting destructor
**	are this object's too (the constructor's unwind reaches them; their order is ours): byte-identical to other
**	instances, they are named in tools/re/symbols_named.json from the unwind funclets (+0x20 / +0x38 / +0x58).
**	Get_Scene_ID's 0x10000 is SCENE_ID_LAST + 1, the first id after ww3d2's (the sort list's render objects of
**	class 0x10002 are the app's too).
*/
