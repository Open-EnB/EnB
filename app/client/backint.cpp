// Earth & Beyond client (reconstruction): backint.cpp, the backdrop's interface (notes at the end)
#include "backint.h"
#include "sclient.h"
#include "..\shared\g_assert.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\nullrobj.h"
#include "..\..\ww3d2\camera.h"
#include "..\..\ww3d2\scene.h"

// 0x53AA30
BackInterfaceClass::BackInterfaceClass(UIViewClass *view, SimpleSceneClass *scene) :
	InterfaceClass(view, (int)scene),
	Backdrop(NULL),
	Override(NULL)
{
}

// 0x53AAB0
BackInterfaceClass::~BackInterfaceClass()
{
	REF_PTR_RELEASE(Backdrop);
	REF_PTR_RELEASE(Override);
}

// 0x53AB80
void BackInterfaceClass::Unknown_54()
{
	Backdrop = new Null3DObjClass("NULL");
}

// 0x53AC00
void BackInterfaceClass::Unknown_58()
{
	InterfaceClass::Unknown_58();
	RenderObjClass *robj = Override;
	if (robj == NULL) {
		robj = Backdrop;
	}
	robj->Add(Peek_Scene());
}

// 0x53AC40
void BackInterfaceClass::Unknown_5C()
{
	if (Override) {
		Override->Remove();
	} else {
		Backdrop->Remove();
	}
	InterfaceClass::Unknown_5C();
}


























// 0x53AC80
void BackInterfaceClass::Unknown_88(RenderObjClass *robj)
{
	G_ASSERT(robj);
	if (Backdrop) {
		if (Active && Override == NULL) {
			Backdrop->Remove();
		}
		Backdrop->Release_Ref(); Backdrop = NULL;
	}
	Backdrop = robj;
	robj->Add_Ref();
	if (Active && Override == NULL) {
		Backdrop->Add(Peek_Scene());
	}
}

// 0x53AD40
void BackInterfaceClass::Unknown_8C(RenderObjClass *robj)
{
	if (robj) {
		if (Override) {
			if (Active) {
				Override->Remove();
			}
			Override->Release_Ref(); Override = NULL;
		} else if (Active) {
			Backdrop->Remove();
		}
		Override = robj;
		robj->Add_Ref();
		if (Active) {
			Override->Add(Peek_Scene());
		}
	} else if (Override) {
		if (Active) {
			Override->Remove();
			Backdrop->Add(Peek_Scene());
		}
		Override->Release_Ref(); Override = NULL;
	}
}


// 0x53AE30
void BackInterfaceClass::Unknown_60()
{
	FAIL_IF(!Get_View()) return;
	CameraClass *camera = Get_View()->Unknown_58();
	FAIL_IF(!camera) return;
	if (Override) {
		Override->Set_Position(camera->Get_Position());
	} else {
		Backdrop->Set_Position(camera->Get_Position());
	}
}

/*
**	Reconstruction notes (from the retail client.exe, SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\backint.cpp (its __FILE__ 0xB78C80: G_ASSERT 'robj' on line 81,
**	FAIL_IFs '!Get_View()' / '!camera' on lines 125 / 127). Client 0x53AA30-0x53AF30: the constructor, the deleting
**	destructor 0x53AA80 and the destructor, slots +0x54 / +0x58 / +0x5C, the backdrop and override setters +0x88 / +0x8C,
**	the frame +0x60, <string>'s guarded initialiser 0x53AF00 (guard 0xC416FC) and the second table's thunk 0x53AF30.
**	The class name follows the scripted movie's member 'BackInterface' (its asserts, 0x804800) and the file (WE2).
**	The main view's +0x1DC (0x50C690, over the +0x110 scene: SERVER_PARAMETERS' backdrop model through 0x513540) and
**	the scripted movie's skybox (Create_Skybox 0x804800) are its two users. With no backdrop model in the scene the
**	main view's space mode (Begin_Render without a colour clear unless SwapBackdrop) leaves the last frame drawn under
**	the cockpit: the POC kept the login's starmap splash (tsk_login.cpp's Splash(1)) on screen.
**	The setters release without a null test (inside the 'if'), not REF_PTR_RELEASE.
*/
