// Earth & Beyond client (reconstruction): app\Client\local_interface_sort.cpp. Notes at the end of the file.
#define __INTRFACE_MEMBERS
#include "intrface.h"
#include "local_interface_sort.h"
#include "interface_static_sort_list.h"
#include "..\shared\unknown_debug.h"
#include "sclient.h"	// UIViewClass (slot +0x50: its scene)
#include "interface_scene.h"	// InterfaceSceneClass (+0x9C the static sort list)
#include "rendobj.h"

enum { SCENE_ID_INTERFACE = SceneClass::SCENE_ID_LAST + 1 };	// InterfaceSceneClass's Get_Scene_ID (0x10000)  // NAME 2: the FAIL_IFs'












































// NOT EXACT: inline-budget (0.48; the client's case 2 calls RefCountPtr's assignment out of line, 0x6B55D0, EH state 5)
// 0x6E0E80
InterfaceSortClass::InterfaceSortClass(InterfaceClass *ui) :
	SortList(NULL)
{
	FAIL_IF(!ui) return;
	ui = ui->Unknown78;
	FAIL_IF(!ui) return;
	SortList = ui->Unknown_541800();
	if (!SortList) return;
	switch (ui->Unknown74) {




		default:
			G_ASSERT(false);
		case 0:
			Previous = Create_Peek(SortList->Set_Current_Interface(ui));
			break;
		case 1:
			Previous = Create_Peek(SortList->Set_Current_Front());
			break;
		case 2:
			Previous = Create_Peek(SortList->Set_Current_Back());
			break;
	}
}



// 0x6E1090
InterfaceSortClass::InterfaceSortClass(InterfaceStaticSortListClass *sort_list, bool front) :
	SortList(sort_list)
{
	FAIL_IF(!SortList) return;
	if (front) Previous = Create_Peek(SortList->Set_Current_Front());
	else Previous = Create_Peek(SortList->Set_Current_Back());
}

// 0x6E1200
InterfaceSortClass::InterfaceSortClass(InterfaceSceneClass *ui_scene, bool front) :
	SortList(NULL)
{
	FAIL_IF(!ui_scene) return;
	SortList = ui_scene->SSL;

	FAIL_IF(!SortList) return;
	if (front) Previous = Create_Peek(SortList->Set_Current_Front());
	else Previous = Create_Peek(SortList->Set_Current_Back());
}

// 0x6E1390
InterfaceSortClass::InterfaceSortClass(SceneClass *castable_ui_scene, bool front) :
	SortList(NULL)
{

	FAIL_IF(!castable_ui_scene) return;
	FAIL_IF(castable_ui_scene->Get_Scene_ID() != SCENE_ID_INTERFACE) return;
	InterfaceSceneClass *ui_scene = (InterfaceSceneClass *)castable_ui_scene;

	SortList = ui_scene->SSL;

	FAIL_IF(!SortList) return;
	if (front) Previous = Create_Peek(SortList->Set_Current_Front());
	else Previous = Create_Peek(SortList->Set_Current_Back());
}

// 0x6E1560
InterfaceSortClass::InterfaceSortClass(UIViewClass *uiview, bool front) :
	SortList(NULL)
{


	FAIL_IF(!uiview) return;
	InterfaceSceneClass *ui_scene = (InterfaceSceneClass *)uiview->Unknown_50();

	FAIL_IF(!ui_scene) return;
	SortList = ui_scene->SSL;

	FAIL_IF(!SortList) return;
	if (front) Previous = Create_Peek(SortList->Set_Current_Front());
	else Previous = Create_Peek(SortList->Set_Current_Back());
}

// 0x6E1730
InterfaceSortClass::InterfaceSortClass(const RefCountPtr<RenderObjClass> &robj) :
	SortList(NULL)
{
	SceneClass *scene = robj->Peek_Scene();

	FAIL_IF(!scene) return;
	FAIL_IF(scene->Get_Scene_ID() != SCENE_ID_INTERFACE) return;
	InterfaceSceneClass *ui_scene = (InterfaceSceneClass *)scene;

	SortList = ui_scene->SSL;

	FAIL_IF(!SortList) return;
	Previous = Create_Peek(SortList->Set_Current_Interface(SortList->Find_Interface(robj.Peek())));
}

// 0x6E18B0
InterfaceSortClass::~InterfaceSortClass()
{
	if (SortList) {
		SortList->Set_Current_Interface(Previous.Peek());
		SortList = NULL;
	}
}

// 0x6E1940 (exact in verify; appfile match says PREFIX: the client's size 0x160 counts the int3 padding to 0x6E1AA0)
void InterfaceSortClass::Unknown_6E1940(bool front)
{
	if (front) Previous = Create_Peek(SortList->Set_Current_Front());
	else Previous = Create_Peek(SortList->Set_Current_Back());
}

/*
**	Reconstruction notes
**
**	The client's window 0x6E0E80-0x6E2280 (the asserts' lines 61-153): the constructors over an interface
**	(0x6E0E80: its top interface's (+0x78) sort mode +0x74 picks the current interface, the front or the back
**	list), 0x6E1090 ('!SortList' 91), 0x6E1200 ('!ui_scene' 100), 0x6E1390 ('!castable_ui_scene' 113),
**	0x6E1560 ('!uiview' 130) and over a render object 0x6E1730 ('!scene' 147), the destructor 0x6E18B0 and
**	0x6E1940-0x6E2280. Rebuilt here: the six constructors and 0x6E18B0 (the windows' scope, 0x541980, is
**	intrface.cpp's); 0x6E1940-0x6E2280 are not yet. SCENE_ID_INTERFACE is InterfaceSceneClass's scene id
**	(0x10000, SceneClass::SCENE_ID_LAST + 1), named from the FAIL_IFs' text.
**
**	0x6E0E80: the client's case 2 calls RefCountPtr<InterfaceClass>'s assignment out of line (0x6B55D0) where
**	cases 0 and 1 inline it.
*/
