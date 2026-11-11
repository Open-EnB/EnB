// Earth & Beyond client (reconstruction): app\client\user_click_indicator.cpp. Notes at the end of the file.
#include "sclient.h"
#include "user_click_indicator.h"
#include "rendobj.h"
#include "assetmgr.h"
#include "matinfo.h"
#include "ww3d.h"
#include "scene.h"
#include "..\..\wwmath\sphere.h"
#include "..\..\wwmath\matrix3d.h"
#include "..\shared\g_assert.h"

void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp's: effect.ini's ResetMappers)  // NAME 1 [robj, make_geometry_unique]: as eff_o.cpp's

static const char *_BoneNames[4] = { "T", "R", "B", "L" };	// 0xB91C7C  // NAME 1: what it holds










































// 0x6B3A90
UserClickIndicatorClass::UserClickIndicatorClass() :
	Indicator(NULL),
	Unknown38(true)
{
	Indicator = WW3DAssetManager::Get_Instance()->Create_Render_Obj("hud04a");
	FAIL_IF(!Indicator) return;
	for (int i = 0; i < 4; i++) {
		Bones[i].Bone = Indicator->Get_Bone_Index(_BoneNames[i]);
		Indicator->Capture_Bone(Bones[i].Bone);
		RenderObjClass *sub = Indicator->Get_Sub_Object_By_Name(_BoneNames[i]);
		Unknown_496AE0(sub, false);
		Bones[i].MaterialInfo = sub->Get_Material_Info();
		Bones[i].VertexMaterial = Bones[i].MaterialInfo->Get_Vertex_Material(0);
		sub->Release_Ref();
	}
	Indicator->Set_Transform(Matrix3D(true));
}

// 0x6B3C20
UserClickIndicatorClass::~UserClickIndicatorClass()
{
	Unknown_6B4400();
	for (int i = 0; i < 4; i++) {
		Indicator->Release_Bone(Bones[i].Bone);
		REF_PTR_RELEASE(Bones[i].MaterialInfo);
		REF_PTR_RELEASE(Bones[i].VertexMaterial);
	}
	REF_PTR_RELEASE(Indicator);
}













// 0x6B3D00
bool UserClickIndicatorClass::Unknown_6B3D00(UIViewClass *view)
{
	FAIL_IF(!Indicator) return(false);

	if (Indicator->Peek_Scene()) {
		Indicator->Remove();
	}
	FAIL_IF(!view) return(false);
	Vector2 min;
	Vector2 max;
	view->Unknown_65B610(min, max);
	Size = ((max.X > max.Y) ? max.X : max.Y) * 1.5f;
	Unknown38 = true;
	RefCountPtr<SceneClass> scene = Create_Peek((SceneClass *)view->Unknown_50());
	return(Unknown_6B3ED0(scene));
}




// 0x6B3ED0
bool UserClickIndicatorClass::Unknown_6B3ED0(const RefCountPtr<SceneClass> &scene)
{
	FAIL_IF(!scene) return(false);
	if (Indicator->Peek_Scene()) {
		Indicator->Remove();
	}
	Indicator->Add(scene.Peek());
	Indicator->Set_Hidden(true);
	return(true);
}

// 0x6B3F50
void UserClickIndicatorClass::Unknown_6B3F50(const SphereClass &sphere)
{
	FAIL_IF(!Indicator) return;
	unsigned int frame_time = WW3D::Get_Frame_Time();
	float target = sphere.Radius * 0.8f;
	float time = frame_time;
	float step = time * 0.5f;
	float spin = time * 0.15f;
	Matrix3D tm = Indicator->Get_Transform();
	tm.Set_Translation(sphere.Center);
	if (Indicator->Is_Hidden()) {
		Indicator->Set_Hidden(false);
		tm.Rotate_Z(DEG_TO_RADF(45.0f));
	}

	if (Size != target) {
		if (Size > target) {
			Size = WWMath::Clamp(Size - step, target, Size);
		} else {
			Size = WWMath::Clamp(Size + step, Size, target);
		}
		Matrix3D bone(true);
		bone.Set_Translation(Vector3(0.0f, Size, 0.0f));
		Indicator->Control_Bone(Bones[0].Bone, bone);
		bone.Set_Translation(Vector3(Size, 0.0f, 0.0f));
		Indicator->Control_Bone(Bones[1].Bone, bone);
		bone.Set_Translation(Vector3(-Size, 0.0f, 0.0f));
		Indicator->Control_Bone(Bones[3].Bone, bone);
		bone.Set_Translation(Vector3(0.0f, -Size, 0.0f));
		Indicator->Control_Bone(Bones[2].Bone, bone);
	} else {
		Unknown38 = false;
	}

	if (!Unknown38) {
		tm.Rotate_Z(DEG_TO_RADF(spin));
	}
	Indicator->Set_Transform(tm);
}
















// 0x6B4400
void UserClickIndicatorClass::Unknown_6B4400()
{
	FAIL_IF(!Indicator) return;
	if (Indicator->Peek_Scene()) {
		Indicator->Remove();
	}
}



/*
**	Notes (reconstruction).
**
**	The object is 0x6B3A90-0x6B4470: the constructor 0x6B3A90, the destructor 0x6B3C20 (0x6B4400 inlined), 0x6B3D00
**	(0x6B3ED0 inlined), 0x6B3ED0, the frame update 0x6B3F50, 0x6B4400, then the guarded initialiser of
**	std::ctype<unsigned short>::id 0x6B4450 (every <string> includer's). 0x6B3A70 / 0x6B3A80 before it are the
**	adjustor thunks of the object before (0x6AF220's); view.cpp's 0x6B4480 follows. The FAIL_IFs sit on the
**	client's lines (64, 104, 109, 125, 137, 193). _BoneNames 0xB91C7C points at the shared literals "T", "R", "B",
**	"L"; the constructor's loop runs a pointer over it to 0xB91C8C.
*/
