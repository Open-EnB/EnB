#include "sclient.h"
#include "uiview.h"
#include "..\shared\unknown_debug.h"
#include "..\..\ww3d2\camera.h"
#include "..\..\ww3d2\layer.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\..\wwdebug\wwprofile.h"

// 0x65A6C0 (the destructor 0x65A760, deleting 0x65A730)
UIViewClass::UIViewClass(ClientTaskClass *task, int arg) :
	Unknown14(task),
	Unknown18(arg),
	Unknown1C(NULL),
	Unknown24(NULL),
	Unknown28(NULL),
	Unknown2C(NULL),
	Unknown30(NULL),
	Unknown34(NULL),
	CameraEffects(NULL)
{
}

UIViewClass::~UIViewClass()
{
	delete Unknown2C;
	Unknown2C = NULL;
}

// 0x65A820: slot +0x04: the gadget list, the scenes and cameras, the cameras set up (+0x40), the +0x30 object and the
// interface object over a layer of the 2-D scene and camera
void UIViewClass::Unknown_04()
{
	Unknown2C = new GadgetListManagerClass;
	Unknown24 = new SimpleSceneClass;
	Unknown24->Set_Fog_Enable(false);
	Unknown1C = new ViewUnknown1CClass;
	Unknown1C->Set_Fog_Enable(false);
	Unknown28 = new CameraClass;
	Unknown20 = Create_NEW(new CameraClass);
	Unknown_40();
	Unknown30 = new UIViewUnknown30Class(this);
	Unknown34 = new ViewUnknown34Class(this, LayerClass(Unknown1C, Unknown20.Peek()));
	Unknown34->Unknown_54();
	Unknown_6B4940(Unknown34);
}

// 0x65AB10: slot +0x40: the cameras by the view's kind (+0x18): +0x28 the 3-D one, +0x20 the 2-D one (its z
// scales the view plane, 0x65B570)
void UIViewClass::Unknown_40()
{
	switch (Unknown18) {
		case 0:
			Unknown28->Set_View_Plane(Vector2(-1.0f, -1.0f), Vector2(1.0f, 1.0f));
			Unknown28->Set_Position(Vector3(0.0f, 0.0f, 1.0f));
			Unknown28->Set_Clip_Planes(0.1f, 10.0f);
			Unknown20->Set_View_Plane(Vector2(-1.0f, -0.75f), Vector2(1.0f, 0.75f));
			Unknown20->Set_Position(Vector3(0.0f, 0.0f, 320.0f));
			Unknown20->Set_Clip_Planes(300.0f, 1000.0f);
			break;
		case 5:
			Unknown28->Set_View_Plane(Vector2(-1.0f, -1.0f), Vector2(1.0f, 1.0f));
			Unknown28->Set_Position(Vector3(0.0f, 0.0f, 1.0f));
			Unknown28->Set_Clip_Planes(1.0f, 3000.0f);
			Unknown20->Set_View_Plane(Vector2(-1.0f, -0.75f), Vector2(1.0f, 0.75f));
			Unknown20->Set_Position(Vector3(0.0f, 0.0f, 320.0f));
			Unknown20->Set_Clip_Planes(1.0f, 800.0f);
			break;
		case 3:
		case 4:
			Unknown28->Set_View_Plane(Vector2(-1.0f, -1.0f), Vector2(1.0f, 1.0f));
			Unknown28->Set_Position(Vector3(0.0f, 0.0f, 1.0f));
			Unknown28->Set_Clip_Planes(0.1f, 10.0f);
			Unknown20->Set_View_Plane(Vector2(-1.0f, -0.75f), Vector2(1.0f, 0.75f));
			Unknown20->Set_Position(Vector3(0.0f, 0.0f, 320.0f));
			Unknown20->Set_Clip_Planes(1.0f, 800.0f);
			break;
		case 1:
			Unknown28->Set_View_Plane(Vector2(-1.0f, -1.0f), Vector2(1.0f, 1.0f));
			Unknown28->Set_Position(Vector3(0.0f, 0.0f, 1.0f));
			Unknown28->Set_Clip_Planes(0.1f, 10.0f);
			Unknown20->Set_View_Plane(Vector2(-1.0f, -1.0f), Vector2(1.0f, 1.0f));
			Unknown20->Set_Position(Vector3(0.0f, 0.0f, 1.0f));
			Unknown20->Set_Clip_Planes(0.1f, 10.0f);
			break;
		case 2:
			Unknown28->Set_View_Plane(Vector2(-1.0f, -1.0f), Vector2(1.0f, 1.0f));
			Unknown28->Set_Position(Vector3(0.0f, 0.0f, 1.0f));
			Unknown28->Set_Clip_Planes(0.1f, 10.0f);
			Unknown20->Set_View_Plane(Vector2(-0.5f, -0.375f), Vector2(0.5f, 0.375f));
			Unknown20->Set_Position(Vector3(0.0f, 0.0f, 640.0f));
			Unknown20->Set_Clip_Planes(100.0f, 2000.0f);
			Unknown20->Set_Depth_Range(0.0f, 0.5f);
			break;
		case 6:
			Unknown28->Set_View_Plane(Vector2(-1.0f, -1.0f), Vector2(1.0f, 1.0f));
			Unknown28->Set_Position(Vector3(0.0f, 0.0f, 1.0f));
			Unknown28->Set_Clip_Planes(0.1f, 10.0f);
			Unknown20->Set_View_Plane(Vector2(-1.0f, -0.75f), Vector2(1.0f, 0.75f));
			Unknown20->Set_Position(Vector3(0.0f, 0.0f, 320.0f));
			Unknown20->Set_Clip_Planes(1.0f, 800.0f);
			break;
	}
	Unknown_65B570();
}

// 0x65B330: slot +0x0C: both scenes, the 2-D one over the 3-D one
int UIViewClass::Unknown_0C(bool on)
{
	WWPROFILE("UIViewClass::Render");
	WW3D::Begin_Render(on);
	Unknown1C->Visibility_Check(Unknown20.Peek());
	Unknown24->Visibility_Check(Unknown28);
	((ViewUnknown1CClass *)Unknown1C)->Unknown_6DA760(Unknown20);
	WW3D::Render(Unknown24, Unknown28);
	WW3D::End_Render();
	return(0);
}

// 0x65B470: slot +0x14: the interface object, the +0x30 object, then the base
void UIViewClass::Unknown_14()
{
	Unknown34->Unknown_588870();
	Unknown30->Unknown_6AB5B0();
	ViewClass::Unknown_14();
}

// 0x65B4A0: slot +0x10: Shutdown, Init's counterpart
void UIViewClass::Unknown_10()
{
	ViewClass::Unknown_10();
	Unknown34->Unknown_5C();
	REF_PTR_RELEASE(Unknown34);
	delete Unknown30;
	Unknown30 = NULL;
	if (Unknown24) {
		Unknown24->Release_Ref();
		Unknown24 = NULL;
	}
	if (Unknown1C) {
		Unknown1C->Release_Ref();
		Unknown1C = NULL;
	}
	if (Unknown28) {
		Unknown28->Release_Ref();
		Unknown28 = NULL;
	}
}


















































































// 0x65B570: the view plane from the 2-D camera (slot +0x54), scaled by the camera's z
bool UIViewClass::Unknown_65B570()
{
	CameraClass *camera = (CameraClass *)Unknown_54();
	FAIL_IF(!camera) {
		Unknown38.Set(-1.0f, -1.0f);
		Unknown40.Set(1.0f, 1.0f);
		return(false);
	}
	camera->Get_View_Plane(Unknown38, Unknown40);
	float z = camera->Get_Frustum().CameraTransform.Get_Z_Translation();
	Unknown38 *= z;
	Unknown40 *= z;
	return(true);
}

// 0x65B610: the view plane
void UIViewClass::Unknown_65B610(Vector2 &min, Vector2 &max)
{
	min = Unknown38;
	max = Unknown40;
}


// 0x65B650: a point in 0..1 to the view plane (y down)
void UIViewClass::Unknown_65B650(const Vector2 &norm, Vector2 *coords)
{
	FAIL_IF(!coords) {return;}
	coords->X = (Unknown40.X - Unknown38.X) * norm.X;
	coords->X += Unknown38.X;
	coords->Y = (Unknown38.Y - Unknown40.Y) * norm.Y;
	coords->Y += Unknown40.Y;
}


// 0x65B6C0: a point of the view plane to 0..1, 0x65B650's inverse
void UIViewClass::Unknown_65B6C0(const Vector2 &coords, Vector2 *norm)
{
	FAIL_IF(!norm) {return;}

	float width = Unknown40.X - Unknown38.X;
	FAIL_IF(width == 0.0f) {return;}

	float height = Unknown40.Y - Unknown38.Y;
	FAIL_IF(height == 0.0f) {return;}
	norm->X = (coords.X - Unknown38.X) / width;
	norm->Y = (coords.Y - Unknown40.Y) / height;
}

// 0x65B7A0
bool UIViewClass::Unknown_65B7A0(int a, int b, int c, int d)
{
	if (Unknown1C) {
		return(((ViewUnknown1CClass *)Unknown1C)->Unknown_6DA6F0(a, b, c, d));
	}
	return(false);
}

// 0x65B7E0
bool UIViewClass::Unknown_65B7E0(int a)
{
	if (Unknown1C) {
		return(((ViewUnknown1CClass *)Unknown1C)->Unknown_6DA730(a));
	}
	return(false);
}

// 0x65B810 (deleting destructor 0x65B880; the implicit destructor's out-of-line copy is mainview.cpp's 0x50B930)
Unknown65B810ViewClass::Unknown65B810ViewClass(ClientTaskClass *task, int arg) :
	UIViewClass(task, 0),
	Client(task->Client)
{
}

// 0x65B8B0: slot +0x10
void Unknown65B810ViewClass::Unknown_10()
{
	Client = NULL;
	UIViewClass::Unknown_10();
}

// 0x65B8D0: slot +0x44: the client's software mouse
int Unknown65B810ViewClass::Unknown_44()
{
	if (Client) {
		return((int)Client->Unknown1274);
	}
	return(0);
}

/*
**	Notes (LD4, 2026-10-29)
**
**	uiview.cpp is UIViewClass, the views' base (sclient.h; its own base is view.cpp's ViewClass, 0x6B4480 /
**	0x6B45D0), and the second view Unknown65B810ViewClass (0x65B810, the base of the main, loading and 3-D movie
**	views): client 0x65A6C0-0x65B8E0 (appfile.py show app\client\uiview.cpp). The class is named by the profile
**	label 'UIViewClass::Render' (0x65B330) and the FAIL_IFs' __FILE__ 'e:\build\beta\G\app\Client\uiview.cpp'
**	(lines 234, 257, 268, 271, 274). +0x20 is a RefCountPtr<CameraClass>: Init assigns it through Create_NEW
**	(0x5C4030, the CameraClass instance, out of line) and the destructor releases it as a member.
*/
