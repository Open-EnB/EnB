// Earth & Beyond client (reconstruction): app\Client\interface_static_sort_list.cpp. Notes at the end of the file.
#include "interface_static_sort_list.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\..\ww3d2\scene.h"
#include "..\..\ww3d2\camera.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\predlod.h"
#include "..\..\wwmath\wwmath.h"
#include "..\shared\unknown_debug.h"

static bool Unknown_BE67E0;  // 0xBE67E0 (.bss, never written: every render object to the front list)  // NAME 1: neutral

// the render objects of class 0x10002 (their +0x8C, called at +0x50 with their +0x90 after a PIP scene's render)
class PIPUnknownClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(); virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C();
	virtual void Unknown_50(void *unknown);  // NAME 1 [unknown]: neutral
};




















































































































































































InterfaceStaticSortListClass::~InterfaceStaticSortListClass()
{
	G_ASSERT(InterfaceVector.size() == 2);



	G_ASSERT(InterfaceMap.empty());
	G_ASSERT(RenderObjMap.empty());
	G_ASSERT(PIPScenes.empty());

	G_ASSERT(PIPPolyBudget <= WWMATH_EPSILON);
}


















void InterfaceStaticSortListClass::Add_To_List(RenderObjClass *robj, unsigned int sort_level)
{
	Find_Interface(robj);
	FAIL_IF(!LastInterface) return;
	LastInterface->Add_To_List(robj, sort_level);
}

void InterfaceStaticSortListClass::Render(const RefCountPtr<SceneClass> &scene, const RefCountPtr<CameraClass> &camera)
{
	for (std::vector<RefCountPtr<InterfaceRenderClass> >::iterator it = InterfaceVector.begin(); it != InterfaceVector.end(); ++it) {
		(*it)->Render(scene, camera);
	}
}



































// NOT EXACT: shape (erase(it) 0x6DD3D0 out of line in the client)
void InterfaceStaticSortListClass::Add_Interface(InterfaceClass *ui)
{
	FAIL_IF(!ui) return;


	G_ASSERT((*InterfaceVector.rbegin()) == FrontInterface);
	InterfaceVector.pop_back();


	FAIL_IF(InterfaceMap.find(ui) != InterfaceMap.end()) return;

	RefCountPtr<InterfaceRenderClass> ui_render = Create_NEW(new InterfaceRenderClass(ui, InterfaceVector.size()));
	InterfaceVector.push_back(ui_render);
	FrontInterface->Index = InterfaceVector.size();
	InterfaceVector.push_back(FrontInterface);
	InterfaceMap[ui] = ui_render;
}

















// NOT EXACT: shape (one call more: the pop_back)
void InterfaceStaticSortListClass::Remove_Interface(InterfaceClass *ui)
{
	FAIL_IF(!ui) return;

	if (LastInterface && LastInterface->Interface == ui) {
		LastInterface = NULL;
		LastRenderObj = NULL;
	}



	InterfaceMapType::iterator imt_it = InterfaceMap.find(ui);
	FAIL_IF(imt_it == InterfaceMap.end()) return;

	RefCountPtr<InterfaceRenderClass> ui_render = (*imt_it).second;
	FAIL_IF(!ui_render) return;

	int count = InterfaceVector.size() - 1;
	for (int i = ui_render->Index; i < count; i++) {
		InterfaceVector[i] = InterfaceVector[i + 1];
		InterfaceVector[i]->Index = i;
	}
	InterfaceVector.pop_back();
	InterfaceMap.erase(imt_it);
}

































// NOT EXACT: shape (erase(it) 0x6DD3D0 out of line in the client)
void InterfaceStaticSortListClass::Interface_To_Front(InterfaceClass *ui)
{
	FAIL_IF(!ui) return;


	InterfaceMapType::iterator imt_it = InterfaceMap.find(ui);
	FAIL_IF(imt_it == InterfaceMap.end()) return;

	RefCountPtr<InterfaceRenderClass> ui_render = (*imt_it).second;
	FAIL_IF(!ui_render) return;


	G_ASSERT((*InterfaceVector.rbegin()) == FrontInterface);
	InterfaceVector.pop_back();

	int i;
	for (i = ui_render->Index; i < (int)InterfaceVector.size() - 1; i++) {
		InterfaceVector[i] = InterfaceVector[i + 1];
		InterfaceVector[i]->Index = i;
	}
	InterfaceVector[i] = ui_render;
	ui_render->Index = i;
	FrontInterface->Index = InterfaceVector.size();
	InterfaceVector.push_back(FrontInterface);
}





























// NOT EXACT: register (the previous interface in esi in the client)
InterfaceClass *InterfaceStaticSortListClass::Set_Current_Interface(InterfaceClass *ui)
{
	InterfaceClass *old = CurrentInterface ? CurrentInterface->Interface : NULL;
	if (old != ui) {
		CurrentInterface = NULL;
		if (ui != NULL) {
			InterfaceMapType::iterator imt_it = InterfaceMap.find(ui);
			FAIL_IF(imt_it == InterfaceMap.end()) return old;
			CurrentInterface = (*imt_it).second;
		} else {
			CurrentInterface = NULL;
		}
	}
	return old;
}

InterfaceClass *InterfaceStaticSortListClass::Set_Current_Front()
{
	InterfaceClass *old = CurrentInterface ? CurrentInterface->Interface : NULL;
	CurrentInterface = FrontInterface;
	return old;
}

InterfaceClass *InterfaceStaticSortListClass::Set_Current_Back()
{
	InterfaceClass *old = CurrentInterface ? CurrentInterface->Interface : NULL;
	CurrentInterface = BackInterface;
	return old;
}








































void InterfaceStaticSortListClass::Add_Render_Object(RenderObjClass *robj)
{
	G_ASSERT(CurrentInterface);
	FAIL_IF(!robj) return;
	FAIL_IF(robj->Get_Container()) return;
	RenderObjMap[robj] = CurrentInterface ? CurrentInterface : FrontInterface;
}














void InterfaceStaticSortListClass::Remove_Render_Object(RenderObjClass *robj)
{
	FAIL_IF(!robj) return;
	FAIL_IF(robj->Get_Container()) return;

	RenderObjMapType::iterator robj_it = RenderObjMap.find(robj);
	FAIL_IF(robj_it == RenderObjMap.end()) return;
	RenderObjMap.erase(robj_it);
}














// NOT EXACT: inline-budget (the client calls insert(P, n, x) 0x6DDCA0 and the table resize 0x6DF5E0)
bool InterfaceStaticSortListClass::Add_PIP_Scene(const RefCountPtr<SimpleSceneClass> &scene, const RefCountPtr<CameraClass> &camera, float poly_budget, unsigned int sort_level)
{
	FAIL_IF(!CurrentInterface) return false;
	PIPSceneStruct *ps = new PIPSceneStruct(CurrentInterface, scene, camera, poly_budget, sort_level);
	PIPScenes[scene.Peek()] = ps;
	CurrentInterface->Add_PIP(ps);
	PIPPolyBudget += poly_budget;
	return true;
}




















// NOT EXACT: shape (3 masked rows)
bool InterfaceStaticSortListClass::Remove_PIP_Scene(const RefCountPtr<SimpleSceneClass> &scene)
{
	PIPSceneMapType::iterator pip_it = PIPScenes.find(scene.Peek());
	FAIL_IF(pip_it == PIPScenes.end()) return false;

	PIPSceneStruct *pip_scene = (*pip_it).second;
	PIPScenes.erase(pip_it);


	FAIL_IF(!pip_scene) return false;

	RefCountPtr<InterfaceRenderClass> ui_render = pip_scene->Owner;
	PIPPolyBudget -= pip_scene->PolyBudget;





	FAIL_IF(!ui_render) return false;
	ui_render->Remove_PIP(pip_scene);
	delete pip_scene;
	return true;
}





















// NOT EXACT: shape (the client stores robj back before the loop)
InterfaceClass *InterfaceStaticSortListClass::Find_Interface(RenderObjClass *robj)
{
	if (Unknown_BE67E0) {
		LastInterface = FrontInterface;
		return LastInterface->Interface;
	}

	FAIL_IF(!robj) return LastInterface->Interface;

	RenderObjClass *container = robj->Get_Container();
	while (container != NULL) {
		robj = container;
		container = robj->Get_Container();
	}

	if (robj != LastRenderObj) {
		RenderObjMapType::iterator robj_it = RenderObjMap.find(robj);
		if (robj_it != RenderObjMap.end()) {
			LastRenderObj = robj;
			LastInterface = (*robj_it).second;
			return LastInterface->Interface;
		}


		G_ASSERT(false);
		LastInterface = FrontInterface;
	}
	return LastInterface->Interface;
}


void PIPSceneStruct::Render(const RefCountPtr<SceneClass> &scene, const RefCountPtr<CameraClass> &camera)
{
	PredictiveLODOptimizerClass::Clear();
	Scene->Visibility_Check(Camera.Peek());
	PredictiveLODOptimizerClass::Optimize_LODs(PolyBudget);
	WW3D::Render(Scene.Peek(), Camera.Peek(), false, true);

	SceneIterator *it = Scene->Create_Iterator();
	for (; !it->Is_Done(); it->Next()) {
		RenderObjClass *robj = it->Current_Item();
		if (robj->Class_ID() == 0x10002) {
			PIPUnknownClass *unknown = *(PIPUnknownClass **)((char *)robj + 0x8C);
			if (unknown != NULL) {
				unknown->Unknown_50((char *)robj + 0x90);
			}
		}
	}
	Scene->Destroy_Iterator(it);
}

void InterfaceRenderClass::Add_To_List(RenderObjClass *robj, unsigned int sort_level)
{
	if (sort_level < MinSortLevel) {
		MinSortLevel = sort_level;
	}
	if (sort_level > MaxSortLevel) {
		MaxSortLevel = sort_level;
	}
	DefaultStaticSortListClass::Add_To_List(robj, sort_level);
}

// NOT EXACT: shape (the loop layout)
void InterfaceRenderClass::Render(const RefCountPtr<SceneClass> &scene, const RefCountPtr<CameraClass> &camera)
{
	if (PIPScenes.size() != 0) {
		int max_sort = MaxSortLevel;
		std::vector<PIPSceneStruct *>::iterator it = PIPScenes.begin();
		PIPSceneStruct *ps = *it;
		int min_sort = ps->SortLevel;
		for (;;) {
			if (min_sort < max_sort) {
				WW3D::Override_Current_Static_Sort_Lists(this);
				Set_Max_Sort(max_sort);
				Set_Min_Sort(min_sort);
				WW3D::Render(scene.Peek(), camera.Peek());
			}
			if (ps == NULL) {
				break;
			}
			max_sort = min_sort - 1;
			WW3D::Reset_Current_Static_Sort_Lists_To_Default();
			ps->Render(scene, camera);
			++it;
			if (it != PIPScenes.end()) {
				ps = *it;
				min_sort = ps->SortLevel;
			} else {
				ps = NULL;
				min_sort = MinSortLevel;
			}
		}
	} else {
		if (MinSortLevel > MaxSortLevel) {
			return;
		}
		WW3D::Override_Current_Static_Sort_Lists(this);
		Set_Min_Sort(MinSortLevel);
		Set_Max_Sort(MaxSortLevel);
		WW3D::Render(scene.Peek(), camera.Peek());
	}
	MinSortLevel = MAX_SORT_LEVEL;
	MaxSortLevel = 0;
}






























































// NOT EXACT: inline-budget (the client keeps _Ucopy / _Ufill out of line)
void InterfaceRenderClass::Add_PIP(PIPSceneStruct *ps)
{
	FAIL_IF(!ps) return;
	std::vector<PIPSceneStruct *>::iterator it;
	for (it = PIPScenes.begin(); it != PIPScenes.end(); ++it) {
		if ((*it)->SortLevel <= ps->SortLevel) {
			break;
		}
	}
	PIPScenes.insert(it, 1, ps);
}



void InterfaceRenderClass::Remove_PIP(PIPSceneStruct *ps)
{
	FAIL_IF(!ps) return;
	std::vector<PIPSceneStruct *>::iterator find = std::find(PIPScenes.begin(), PIPScenes.end(), ps);

	FAIL_IF(find == PIPScenes.end()) return;
	PIPScenes.erase(find);
}

/*
**	Reconstruction notes
**
**	The client's interface_static_sort_list.obj runs 0x6DADE0-0x6DCE80 (and its template instances to about
**	0x6DF880); the asserts sit on the client's lines (D25): the destructor 206-214, Add_To_List 237,
**	Add_Interface 285-292, Remove_Interface 320-333, Interface_To_Front 379-389, Set_Current_Interface 439,
**	Add_Render_Object 503-505, Remove_Render_Object 524-528, Add_PIP_Scene 548, Remove_PIP_Scene 579-594,
**	Find_Interface 628-645, InterfaceRenderClass::Add_PIP 790 and Remove_PIP 804-807 (both inlined into
**	Add_PIP_Scene / Remove_PIP_Scene, /Ob2).
**	The constructor 0x6DA930 and InterfaceRenderClass's destructors 0x6DAD10 / 0x6DAD40 are inline (header):
**	the client's copies sit in interface_scene.cpp's object, which builds the list.
**	Unknown_BE67E0 is a zero .bss byte the client reads once (0x6DC5F1) and never writes: a debug switch.
*/
