// Earth & Beyond client (reconstruction): the avatar picture-in-picture (notes at the end)
#include <math.h>
#include "avatar_pip.h"
#include "sclient.h"
#include "option.h"
#include "avatar.h"
#include "co.h"
#include "..\shared\unknown_debug.h"
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\auxdata\delta_template.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\scene.h"
#include "..\..\ww3d2\camera.h"
#include "..\..\ww3d2\light.h"
#include "matrix3d.h"
#include "sphere.h"
#include "aabox.h"
#include "wwmath.h"

RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj);	// 0x497300 (cdecl, ro_funcs.cpp's neighbourhood)  // NAME 1 [robj]: as ui_avatr.cpp's
template<class C> inline C *Unknown_Find_Valid(DeltaCompoundClass *root, const NameClass &name) { C *t = Unknown_Find_Field<C>(root, name); if (t && t->Unknown70) return(t); return(NULL); }	// NAME 1: neutral (D10): as co.cpp's

// @0x7E31B0
AvatarPipClass::AvatarPipClass() :
	Client(NULL),
	Unknown038(false)
{
	Unknown034 = Create_NEW(new Unknown47FA70Class(false));
	Unknown028 = Create_NEW(new LightClass(LightClass::POINT));
	Unknown028->Set_Intensity(1.0f);
	Unknown028->Set_Force_Visible(true);
	Unknown028->Set_Far_Attenuation_Range(1000000.0, 1000000.0);
	Unknown028->Set_Ambient(Vector3(0.5f, 0.5f, 0.5f));
	Unknown028->Set_Specular(Vector3(0.0f, 0.0f, 0.0f));
	Unknown028->Set_Diffuse(Vector3(1.0f, 1.0f, 1.0f));
	Unknown028->Set_Transform(Matrix3D::Identity);
}

// @0x7E3410
AvatarPipClass::~AvatarPipClass()
{
	Unknown_7E4350();
}

// @0x7E35D0
void AvatarPipClass::Unknown_7E35D0()
{
	Unknown_7E4350();
	Client = NULL;
	Unknown010 = NULL;
	Scene = NULL;
	Camera = NULL;
	Unknown014.X = 0.0f;
	Unknown014.Y = 0.0f;
}






























































































// @0x7E36B0
bool AvatarPipClass::Unknown_7E36B0(SClient *sclient, const RefCountPtr<RenderObjClass> &vp, bool arg)
{
	Unknown_7E35D0();
	FAIL_IF(!vp) return(false);
	FAIL_IF(!sclient) return(false);
	Client = sclient;



	FAIL_IF(!Client->CurrentView) return(false);
	PIPView = Client->CurrentView->Unknown_20();
	FAIL_IF(!PIPView) return(false);
	Scene = Create_NEW(new SimpleSceneClass);
	Scene->Set_Fog_Enable(false);
	Scene->Set_Ambient_Light(Vector3(0.0f, 0.0f, 0.0f));
	Unknown010 = vp;
	Camera = Create_NEW(new CameraClass);
	Unknown_7E4740(true);
	Unknown038 = arg;
	return(true);
}





















// @0x7E3A20
// NOT EXACT: inline budget (the client calls 0x7E4350, RefCountPtr's operator = 0x7E4980, its conversion 0x551D40 and
// destructor 0x4740B0 out of line and copies +0x330 through a temporary; ours expands them; 0.47)
bool AvatarPipClass::Unknown_7E3A20(const char *name, unsigned id, CharacterViewAvatarClass *avatar)
{
	bool changed = false;
	if (!Unknown010) return(false);
	if (!Unknown02C) {
		if (avatar) {
			RefCountPtr<CharacterViewAvatarClass> peek = Create_Peek(avatar);
			Unknown_7E4350();
			Unknown02C = peek;
			Unknown_7E4520(RefCountPtr<RenderObjClass>(peek));
		} else {
			FAIL_IF(!Client) return(false);
			DeltaPlayerClass *aux = Client->AuxData;

			FAIL_IF(!aux) return(false);
			if (!id) {
				DeltaTemplateClass<int> *npc_id = Unknown_Find_Valid<DeltaTemplateClass<int> >((DeltaCompoundClass *)aux, "PIPAvatarID");







				FAIL_IF(!npc_id) return(false);
				id = npc_id->Value;
			}
			RefCountPtr<CharacterViewAvatarClass> object = Create_NEW((CharacterViewAvatarClass *)Client->Unknown_74C7B0(id));
			if (object) {
				Unknown_7E4350();
				Unknown02C = object;
				Unknown_7E4520(RefCountPtr<RenderObjClass>(object));
				changed = true;
			} else if (!Unknown030) {
				ClientObjectClass *co = Client->Find_Object(id);
				if (co) {
					Unknown_7E4520(Unknown_497300(co->RenderObject));
				}
			}
		}
	}
	if (Unknown02C) {
		if (!Unknown02C->Unknown204) Unknown02C->Unknown_77E3D0(1);
		if (name) Unknown02C->Unknown_784A50(name, true, false);
		Unknown02C->Unknown_1C4();
	}
	if (Unknown02C || Unknown030) {
		Unknown_7E4740(false);
		Matrix3D cam(true);
		Matrix3D tm(true);
		Vector3 target(0.0f, 0.0f, 0.0f);
		if (Unknown02C) {
			tm = Unknown02C->Unknown330;
			target = tm.Get_Translation();
			Unknown034->Set_Transform(tm);
			Unknown02C->Set_Transform(tm);
			Unknown02C->Unknown_786410(&cam, Unknown038);
		} else if (Unknown030) {
			Unknown034->Set_Transform(tm);
			const SphereClass &sphere = Unknown030->Get_Bounding_Sphere();
			cam = tm;
			cam.Rotate_X(DEG_TO_RADF(90.0f));
			cam.Rotate_Y(DEG_TO_RADF(90.0f));
			cam.Translate_Z(sphere.Radius * 1.8f);
		}
		Camera->Set_Transform(cam);
		float dist = (cam.Get_Translation() - target).Length();
		Camera->Set_Clip_Planes(0.1f, dist + dist);
		float d = dist * 1.5f;
		float x = 0.0f;
		float y = 0.0f;
		Unknown028->Set_Transform(Matrix3D(Vector3(x + y, d + x, d + y) + target));
	}
	return(changed);
}

// @0x7E4350
void AvatarPipClass::Unknown_7E4350()
{
	if (Unknown030) {
		Unknown028->Remove();
		Unknown034->Remove();
		Unknown034->Unknown_47FF70(NULL);
		PIPView->Unknown_65B7E0(Scene);
		Unknown02C = NULL;
		Unknown030 = NULL;
	}
}

// @0x7E43E0
void AvatarPipClass::Unknown_7E43E0(const RefCountPtr<CharacterViewAvatarClass> &avatar)
{
	Unknown_7E4350();
	Unknown02C = avatar;
	Unknown_7E4520(RefCountPtr<RenderObjClass>(avatar));
}

// @0x7E4520
// NOT EXACT: scheduling (the client converts budget * 0.1f to int before the vp's Get_Sort_Level call; ours after; 0.79)
void AvatarPipClass::Unknown_7E4520(const RefCountPtr<RenderObjClass> &model)
{
	Unknown_7E4350();
	if (model) {
		Unknown030 = model;
		Unknown034->Unknown_47FF70(Unknown030.Peek());
		Unknown034->Set_Transform(Matrix3D(true));
		Unknown034->Add(Scene.Peek());
		float budget = OptionClass::Instance->Get_Bool("Graphics_Default") ? (float)((SClient *)Client->Unknown_18())->PolyBudget : (float)(int)(8000.0f + (OptionClass::Instance->Get_Float("Graphics_Space_Poly") * 0.01f) * (40000.0f - 8000.0f));
		PIPView->Unknown_65B7A0(Scene, Camera, (int)(budget * 0.1f), Unknown010->Get_Sort_Level() + 1);
		Unknown028->Add(Scene.Peek());
	}
}

// @0x7E4740
void AvatarPipClass::Unknown_7E4740(bool force)
{
	Vector3 pos = Unknown010->Get_Position();
	const AABoxClass &box = Unknown010->Get_Bounding_Box();
	Vector2 size(box.Extent.X * 2.0f, box.Extent.Y * 2.0f);
	if (!force && pos == Unknown01C && size == Unknown014) return;
	Unknown014 = size;
	Unknown01C = pos;
	Vector2 p(pos.X - Unknown014.X * 0.5f, pos.Y + Unknown014.Y * 0.5f);
	p.X = p.X + 320.0f;
	p.Y = 240.0f - p.Y;
	Camera->Set_Viewport(Vector2(p.X * (1.0f / 640.0f), p.Y * (1.0f / 480.0f)), Vector2((p.X + Unknown014.X) * (1.0f / 640.0f), (p.Y + Unknown014.Y) * (1.0f / 480.0f)));
	float a = 0.5 / Unknown014.Y * Unknown014.X;
	Camera->Set_View_Plane(Vector2(-a, -0.5f), Vector2(a, 0.5f));
}

/*
**	Reconstruction notes (AP1, 2026-11-06)
**
**	The window 0x7E31B0-0x7E4980: the constructor, the destructor, 0x7E35D0, 0x7E36B0, 0x7E3A20, 0x7E4350, 0x7E43E0,
**	0x7E4520, 0x7E4740, then RefCountPtr<SimpleSceneClass>'s operator =(DummyPtrType *) 0x7E4940 and
**	RefCountPtr<CharacterViewAvatarClass>'s operator =(const RefCountPtr &) 0x7E4980 out of line; cfaction_database.cpp
**	follows at 0x7E4AE0. The FAIL_IFs sit on the client's lines: '!vp' 155, '!sclient' 156, '!Client->CurrentView'
**	161, '!PIPView' 163 (0x7E36B0); '!Client' 208, '!aux' 211, '!npc_id' 221 (0x7E3A20).
**
**	0x7E4350 is defined after 0x7E3A20 as in the client and still expands into the constructor's neighbours
**	(VC6 inlines members defined later in the file): the destructor, 0x7E35D0, 0x7E43E0 and 0x7E4520 expand it,
**	0x7E36B0 expands 0x7E35D0 and calls 0x7E4350 and the RefCountPtr assignments of NULL out of line.
*/
