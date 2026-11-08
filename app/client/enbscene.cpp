// Earth & Beyond client (reconstruction): app\client\enbscene.cpp (the client's __FILE__), so far 0x75ECF0-0x75F852 (notes at the end)
#include "enbscene.h"
#include <string.h>
#include "..\..\wwlib\wwstring.h"
#include "..\..\wwmath\vector3.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\part_emt.h"
#include "..\shared\land.h"
#include "..\shared\proj_man.h"
#include "..\..\ww3d2\texproject.h"
#include "..\..\ww3d2\camera.h"
#include "..\shared\rs_track.h"
#include "..\shared\g_assert.h"
#include "unknown_75b7b0.h"

static void Unknown_75F750(RenderObjClass *obj, const Vector3 &color);	// NAME 1 [obj, color]: neutral (D10)

// @0x75ECF0
MainViewSceneClass::MainViewSceneClass(void)
{
	Create_Projector_Manager();
	Create_Static_Culling_System();
	Vector3 min(-200000.0f, -200000.0f, -5000.0f);
	Vector3 max(200000.0f, 200000.0f, 5000.0f);
	Create_Dynamic_Culling_System(min, max, 10000.0f);
}

// @0x75EED0
MainViewSceneClass::~MainViewSceneClass(void)
{
	UnknownA8.Reset_List();
	UnknownC0.Reset_List();
	UnknownDC.Reset_List();
}

// @0x75F180
void MainViewSceneClass::Add_Render_Object(RenderObjClass * obj)
{
	ProjSceneClass::Add_Render_Object(obj);

	StringClass name(obj->Get_Name(), true);
	strlwr(name.Peek_Buffer());

	if (strncmp(name, "plt", 3) == 0) {
		UnknownA8.Add(obj);
	} else if (strncmp(name, "clouds", 6) == 0) {
		UnknownC0.Add(obj);
	} else if (obj->Class_ID() == RenderObjClass::CLASSID_LAND) {
		Land = Create_Peek((LandClass *)obj);
		UnknownDC.Add(obj);
	} else if ((obj->Class_ID() == RenderObjClass::CLASSID_MESH) || (obj->Class_ID() == RenderObjClass::CLASSID_HLOD) || (obj->Class_ID() == 0x10001)) {
		if (obj->Class_ID() == 0x10001) {
			RenderObjClass * robj = ((RsTrackClass *)obj)->Peek_Render_Object();
			if ((robj != NULL) && ((robj->Class_ID() == RenderObjClass::CLASSID_MESH) || (robj->Class_ID() == RenderObjClass::CLASSID_HLOD))) {
				UnknownDC.Add(obj);
			}
		} else {
			UnknownDC.Add(obj);
		}
	}
}


































// @0x75F440
void MainViewSceneClass::Remove_Render_Object(RenderObjClass * obj)
{
	ProjSceneClass::Remove_Render_Object(obj);

	UnknownA8.Remove(obj);
	UnknownC0.Remove(obj);
	UnknownDC.Remove(obj);

	if (obj->Class_ID() == RenderObjClass::CLASSID_LAND) {
		G_ASSERT(Land.Peek() == (LandClass*)obj);
		Land = NULL;
	}
}

// @0x75F560
void MainViewSceneClass::Remove_All_Render_Objects(void)
{
	UnknownA8.Reset_List();
	UnknownC0.Reset_List();
	UnknownDC.Reset_List();
	ProjSceneClass::Remove_All_Render_Objects();
	Land = NULL;
}

// @0x75F690
void MainViewSceneClass::Unknown_75F690(const Vector3 &sun, const Vector3 &camera)
{
	RefMultiListIterator<RenderObjClass> it(&UnknownA8);
	for (it.First(); !it.Is_Done(); it.Next()) {
		Unknown_75B8F0(it.Peek_Obj(), sun, camera);
	}
}

// @0x75F700
void MainViewSceneClass::Unknown_75F700(const Vector3 &color)
{
	RefMultiListIterator<RenderObjClass> it(&UnknownC0);
	for (it.First(); !it.Is_Done(); it.Next()) {
		Unknown_75F750(it.Peek_Obj(), color);
	}
}

// @0x75F750
static void Unknown_75F750(RenderObjClass *obj, const Vector3 &color)
{
	if (obj->Class_ID() == RenderObjClass::CLASSID_PARTICLEEMITTER) {
		ParticleEmitterClass *emitter = (ParticleEmitterClass *)obj;
		ParticlePropertyStruct<Vector3> colors;
		emitter->Get_Color_Key_Frames(colors);
		for (unsigned int i = 0; i < colors.NumKeyFrames - 1; i++) {
			colors.Values[i] = color;
		}
		colors.Values[colors.NumKeyFrames - 1].Set(0.0f, 0.0f, 0.0f);
		emitter->Reset_Colors(colors);
		if (colors.KeyTimes) {
			delete [] colors.KeyTimes;
		}
		if (colors.Values) {
			delete [] colors.Values;
		}
	} else {
		int count = obj->Get_Num_Sub_Objects();
		for (int i = 0; i < count; i++) {
			RenderObjClass *sub = obj->Get_Sub_Object(i);
			Unknown_75F750(sub, color);
			if (sub) {
				sub->Release_Ref();
			}
		}
	}
}

// @0x75F8B0
// NOT EXACT: field-offset (explain.py; 0.78): the client addresses the projector lists' heads from the manager (+0x60 / +0x64, +0x48 / +0x4C), ours from the list (add 0x5C, +4 / +8), and copies the camera's transform in another order
void MainViewSceneClass::Apply_Projectors(CameraClass * camera)
{
	if (ProjectorManager != NULL) {
		ProjSceneClass::Apply_Projectors(camera);
		if (Land != NULL) {
			Land->Clear_Unknown1218();
			Matrix3D tm = camera->Get_Transform();
			TexProjListIterator dynamic_it(&ProjectorManager->DynamicProjectorList);
			for (; !dynamic_it.Is_Done(); dynamic_it.Next()) {
				TexProjectClass *projector = dynamic_it.Peek_Obj();
				projector->Pre_Render_Update(tm);
				RefCountPtr<TexProjectClass> ptr = Create_Peek(projector);
				Land->Add_Unknown1218(ptr);
			}
			TexProjListIterator static_it(&ProjectorManager->StaticProjectorList);
			for (; !static_it.Is_Done(); static_it.Next()) {
				TexProjectClass *projector = static_it.Peek_Obj();
				projector->Pre_Render_Update(tm);
				RefCountPtr<TexProjectClass> ptr = Create_Peek(projector);
				Land->Add_Unknown1218(ptr);
			}
		}
	}
}

#include "..\..\ww3d2\coltest.h"
#include "..\..\ww3d2\inttest.h"
#include "..\..\wwmath\wwmath.h"

// @0x75FB00
// NOT EXACT: field-offset (explain.py; 0.84): FP operand order only: the client evaluates the inlined Matrix3::Rotate_Vector rows (Compute_Point) and Matrix3D::Inverse_Transform_Vector in another order than the tree's wwmath forms
void MainViewSceneClass::Unknown_75FB00(const OBBoxClass &to, const OBBoxClass &from, const Vector3 &move, CastResultStruct *result, RenderObjClass *ignore, bool ship)
{
	Vector3 points[16];
	int count = 0;
	for (int i = -1; i <= 1; i += 2) {
		for (int j = -1; j <= 1; j += 2) {
			for (int k = -1; k <= 1; k += 2) {
				float params[3];
				params[0] = (float)i;
				params[1] = (float)j;
				params[2] = (float)k;
				to.Compute_Point(params, &points[count]);
				count++;
				from.Compute_Point(params, &points[count]);
				count++;
			}
		}
	}
	AABoxClass box(points, 16);
	AABoxIntersectionTestClass aabox_test(box, COLLISION_TYPE_3);
	OBBoxCollisionTestClass obbox_test(to, move, result, COLLISION_TYPE_3);

	RefMultiListIterator<RenderObjClass> it(&UnknownDC);
	if (ship) {
		for (it.First(); !it.Is_Done(); it.Next()) {
			RenderObjClass *obj = it.Peek_Obj();
			if ((obj != ignore && obj->Intersect_AABox(aabox_test)) || obj->Class_ID() == RenderObjClass::CLASSID_LAND) {
				obj->Cast_OBBox(obbox_test);
			}
		}
	} else {
		for (it.First(); !it.Is_Done(); it.Next()) {
			RenderObjClass *obj = it.Peek_Obj();
			if (obj != ignore && obj->Intersect_AABox(aabox_test)) {
				Vector3 center;
				Matrix3D::Inverse_Transform_Vector(obj->Get_Transform(), from.Center, &center);
				float radius = from.Extent.Length();
				AABoxClass obj_box;
				obj->Get_Obj_Space_Bounding_Box(obj_box);
				obj_box.Extent.X += radius;
				obj_box.Extent.Y += radius;
				obj_box.Extent.Z += radius;
				if (WWMath::Fabs(center.X - obj_box.Center.X) <= obj_box.Extent.X &&
					WWMath::Fabs(center.Y - obj_box.Center.Y) <= obj_box.Extent.Y &&
					WWMath::Fabs(center.Z - obj_box.Center.Z) <= obj_box.Extent.Z) {
					obj->Cast_OBBox(obbox_test);
				}
			} else if (obj->Class_ID() == RenderObjClass::CLASSID_LAND) {
				obj->Cast_OBBox(obbox_test);
			}
		}
	}
}

/*
** Reconstruction notes (WE5, WE7, 2026-11-07)
**
** The scene (enbscene.h) derives from projscen.cpp's ProjSceneClass: the constructor calls its constructor and its
** three Create_ methods (0x495280, 0x495560, 0x495680 with a +-200000 x +-200000 x +-5000 box and 10000), the virtuals
** call its Add / Remove_Render_Object and Remove_All_Render_Objects. Add_Render_Object sorts by name and class:
** 'plt' models into +0xA8, 'clouds' into +0xC0, the land (Class_ID 0x23) into Land and +0xDC, meshes, HLODs and the
** render-state tracks (0x10001) over one into +0xDC. Create_Peek is RefCountPtr's out-of-line friend 0x760290.
** Remove_Render_Object holds the file's one assert (line 106). Apply_Projectors 0x75F8B0 (the +0x68 slot, RH1) runs
** ProjSceneClass's, then hands every dynamic and static texture projector, updated for the camera, to the land
** (Clear_Unknown1218 / Add_Unknown1218; Create_Peek<TexProjectClass> is 0x7602F0). 0x75FB00 (RH2;
** camera_manager.cpp's 0x7998B0 and camctrl_takeoff.cpp's 0x796920) sweeps an oriented box from 'from' to 'to': the
** 16 corners of both boxes (OBBoxClass::Compute_Point) bound an AABoxClass (its points constructor, which spends the
** inline budget so that the box's implicit copy constructor stays out of line, 0x7601B0, as in the client), then every
** +0xDC object but 'ignore' whose Intersect_AABox passes, and the land always, gets Cast_OBBox; without 'ship' the
** object must also hold from's centre within its object-space box grown by from's extent length.
** 0x75F690 gives each 'plt' model in the +0xA8 list its day / night phase (unknown_75b7b0.cpp's 0x75B8F0) from the
** sun direction and the camera position; 0x75F700 sets every colour key but the last of each particle emitter in the
** +0xC0 list to the given colour, the last to black.
*/
