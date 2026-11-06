// Earth & Beyond client (reconstruction): a 3-D scene drawn into a rectangle of a view (notes at the end)
#include "pip_scene.h"
#include "sclient.h"
#include "..\shared\g_assert.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\scene.h"
#include "..\..\ww3d2\camera.h"
#include "aabox.h"
#include "matrix3d.h"


PipSceneClass::PipSceneClass(SClient *client) :
	Unknown04(client),
	View(NULL),
	Rect(0.0f, 0.0f, 0.0f, 0.0f),
	ViewportSize(0.0f, 0.0f),
	Unknown2C(0.0f, 0.0f)
{
	Scene = Create_NEW(new SimpleSceneClass);
	Scene->Set_Fog_Enable(false);
	Scene->Set_Ambient_Light(Vector3(0.0f, 0.0f, 0.0f));
	Camera = Create_NEW(new CameraClass);
}


PipSceneClass::~PipSceneClass()
{
	if (View) {
		View->Unknown_65B7E0(Scene);
		View = NULL;
	}
}


bool PipSceneClass::Unknown_7BACD0(const RefCountPtr<RenderObjClass> &vp, bool arg)
{











































































	FAIL_IF(!vp) return(false);

	if (arg) {
		Vector3 tl = vp->Get_Bone_Transform("tl").Get_Translation();
		Vector3 br = vp->Get_Bone_Transform("br").Get_Translation();
		Rect.Set(tl.X, br.Y, br.X, tl.Y);
		float width = Rect.Right - Rect.Left;
		ViewportSize.X = width;







		FAIL_IF(!ViewportSize.X) return(false);
		float height = Rect.Bottom - Rect.Top;
		ViewportSize.Y = height;
		FAIL_IF(!ViewportSize.Y) return(false);
		Unknown2C.X = width * 0.5f + Rect.Left;
		Unknown2C.Y = height * 0.5f + Rect.Top;
	} else {
		const AABoxClass &box = vp->Get_Bounding_Box();
		Vector2 half(box.Extent.X, box.Extent.Y);
		ViewportSize.X = half.X + half.X;




		FAIL_IF(!ViewportSize.X) return(false);
		ViewportSize.Y = half.Y + half.Y;

		FAIL_IF(!ViewportSize.Y) return(false);
		Unknown2C.Set(box.Center.X, box.Center.Y);
		Rect_Min() = Unknown2C - half;
		Rect_Max() = half + Unknown2C;
	}

	float x = (Rect.Left + 320.0f) / 640.0f;
	float y = (240.0f - Rect.Bottom) / 480.0f;
	Camera->Set_Viewport(Vector2(x, y), Vector2(ViewportSize.X / 640.0f + x, ViewportSize.Y / 480.0f + y));

	if (ViewportSize.X > ViewportSize.Y) {
		Vector2 plane_min;
		Vector2 plane_max;
		plane_max.X = 0.5f;
		plane_min.X = -0.5f;
		float h = 0.5 / ViewportSize.X * ViewportSize.Y;
		plane_max.Y = h;
		plane_min.Y = -h;
		Camera->Set_View_Plane(plane_min, plane_max);
		return(true);
	} else {
		Vector2 plane_max;
		Vector2 plane_min;
		plane_max.Y = 0.5f;
		plane_min.Y = -0.5f;
		float w = 0.5 / ViewportSize.Y * ViewportSize.X;
		plane_max.X = w;
		plane_min.X = -w;
		Camera->Set_View_Plane(plane_min, plane_max);
		return(true);
	}
}


bool PipSceneClass::Unknown_7BB080(UIViewClass *view, int arg1, int arg2)
{
	if (View) {
		View->Unknown_65B7E0(Scene);
		View = NULL;
	}










	FAIL_IF(!view) return(false);
	View = view;
	View->Unknown_65B7A0(Scene, Camera, (float)arg1, arg2);
	return(true);
}


bool PipSceneClass::Unknown_7BB110()
{
	if (View) {
		View->Unknown_65B7E0(Scene);
		View = NULL;
		return(true);
	}
	return(false);
}

/*
**	Reconstruction notes
**
**	pip_scene.cpp's window runs 0x7BA9C0 to 0x7BB131: __FILE__ 0xB9F038 on the FAIL_IFs at lines 113-195 of
**	0x7BACD0 and 0x7BB080.
*/
