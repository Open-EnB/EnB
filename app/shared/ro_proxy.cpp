// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\ro_proxy.cpp (its assert, line 56); client
**	0x47FA70-0x480AEF. Notes at the end.
*/
#include <stdio.h>
#include "ro_proxy.h"
#include "ww3d.h"
#include "rinfo.h"
#include "part_emt.h"
#include "part_buf.h"
#include "sphere.h"
#include "aabox.h"
#include "wwprofile.h"
#include "..\..\wwlib\nstrdup.h"
#include "g_assert.h"

static void Unknown_480A40(RenderObjClass *robj, RenderInfoClass &rinfo);	// NAME 1: neutral

// 0x47FA70
ProxyRObj::ProxyRObj(bool arg) :
	Proxied(NULL),
	Unknown090(true),
	SortLevel(0)
{
	Unknown0C1 = arg;
}

// 0x47FB60
ProxyRObj::ProxyRObj(const ProxyRObj &src) :
	Proxied(src.Proxied),
	Unknown090(src.Unknown090),
	SortLevel(src.SortLevel),
	Unknown0C1(src.Unknown0C1)
{
	if (Proxied != NULL) {
		Proxied->Add_Ref();
	}
}










// 0x47FC90
const ProxyRObj &ProxyRObj::operator=(const ProxyRObj &src)
{
	RenderObjClass::operator=(src);
	if (this != &src) {
		G_ASSERT(0);
	}
	return(*this);
}

// 0x47FCD0
ProxyRObj::~ProxyRObj(void)
{
	if (Proxied != NULL) {
		Proxied->Release_Ref();
		Proxied = NULL;
	}
}

// 0x47FDE0
RenderObjClass *ProxyRObj::Clone(void) const
{
	return(new ProxyRObj(*this));
}

// 0x47FF70
bool ProxyRObj::Unknown_47FF70(RenderObjClass *obj)
{
	if (Proxied == obj) {
		return(false);
	}
	if (Proxied != NULL) {
		Proxied->Release_Ref();
		Proxied = NULL;
	}
	if (obj != NULL) {
		Proxied = obj;
		obj->Add_Ref();
	} else {
		Proxied = NULL;
	}
	return(true);
}

// 0x47FFF0
const char *ProxyRObj::Get_Name(void) const
{
	char name[100];

	if (Proxied != NULL) {
		sprintf(name, "Proxy of %s", Proxied->Get_Name());
	} else {
		sprintf(name, "Proxy");
	}
	return(nstrdup(name));
}

// 0x480070
int ProxyRObj::Get_Num_Polys(void) const
{
	if (Proxied != NULL) {
		return(Proxied->Get_Num_Polys());
	}
	return(0);
}

// 0x4800A0
void ProxyRObj::Render(RenderInfoClass &rinfo)
{
	WWPROFILE("ProxyRObj::Render");

	if (Proxied == NULL || !Is_Not_Hidden_At_All()) {
		return;
	}
	if (WW3D::Are_Static_Sort_Lists_Enabled() && SortLevel != 0) {
		WW3D::Add_To_Static_Sort_List(this, SortLevel);
	} else {
		Unknown090 = Proxied->Get_Transform();
		Proxied->Set_Transform(Get_Transform());
		Proxied->Render(rinfo);
		if (Unknown0C1) {
			Unknown_480A40(Proxied, rinfo);
		}
	}
}

// 0x4802F0
bool ProxyRObj::Intersect(IntersectionClass *Intersection, IntersectionResultClass *Final_Result)
{
	if (Proxied != NULL) {
		return(Proxied->Intersect(Intersection, Final_Result));
	}
	return(false);
}

// 0x480330 (the client calls this object's own slot, not the proxied object's; as 0x480370 and 0x4803B0-0x480470)
bool ProxyRObj::Intersect_Sphere(IntersectionClass *Intersection, IntersectionResultClass *Final_Result)
{
	if (Proxied != NULL) {
		return(Intersect_Sphere(Intersection, Final_Result));
	}
	return(false);
}

// 0x480370
bool ProxyRObj::Intersect_Sphere_Quick(IntersectionClass *Intersection, IntersectionResultClass *Final_Result)
{
	if (Proxied != NULL) {
		return(Intersect_Sphere_Quick(Intersection, Final_Result));
	}
	return(false);
}

// 0x4803B0
bool ProxyRObj::Cast_Ray(RayCollisionTestClass &raytest)
{
	if (Proxied != NULL) {
		return(Cast_Ray(raytest));
	}
	return(false);
}

// 0x4803E0
bool ProxyRObj::Cast_AABox(AABoxCollisionTestClass &boxtest)
{
	if (Proxied != NULL) {
		return(Cast_AABox(boxtest));
	}
	return(false);
}

// 0x480410
bool ProxyRObj::Cast_OBBox(OBBoxCollisionTestClass &boxtest)
{
	if (Proxied != NULL) {
		return(Cast_OBBox(boxtest));
	}
	return(false);
}

// 0x480440
bool ProxyRObj::Intersect_AABox(AABoxIntersectionTestClass &boxtest)
{
	if (Proxied != NULL) {
		return(Intersect_AABox(boxtest));
	}
	return(false);
}

// 0x480470
bool ProxyRObj::Intersect_OBBox(OBBoxIntersectionTestClass &boxtest)
{
	if (Proxied != NULL) {
		return(Intersect_OBBox(boxtest));
	}
	return(false);
}

// 0x4804A0
void ProxyRObj::Get_Obj_Space_Bounding_Sphere(SphereClass &sphere) const
{
	if (Proxied != NULL) {
		Proxied->Get_Obj_Space_Bounding_Sphere(sphere);
	} else {
		sphere.Init(Vector3(0.0f, 0.0f, 0.0f), 1.0f);
	}
}

// 0x4804F0
void ProxyRObj::Get_Obj_Space_Bounding_Box(AABoxClass &box) const
{
	if (Proxied != NULL) {
		Proxied->Get_Obj_Space_Bounding_Box(box);
	} else {
		box.Init(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f));
	}
}

// 0x480540
void ProxyRObj::Prepare_LOD(CameraClass &camera)
{
	if (Proxied != NULL) {
		Proxied->Prepare_LOD(camera);
	}
}

// 0x480570
void ProxyRObj::Increment_LOD(void)
{
	if (Proxied != NULL) {
		Proxied->Increment_LOD();
	}
}

// 0x4805A0
void ProxyRObj::Decrement_LOD(void)
{
	if (Proxied != NULL) {
		Proxied->Decrement_LOD();
	}
}

// 0x4805D0
float ProxyRObj::Get_Cost(void) const
{
	return((Proxied != NULL) ? Proxied->Get_Cost() : 0.000001f);
}

// 0x480600
float ProxyRObj::Get_Value(void) const
{
	return((Proxied != NULL) ? Proxied->Get_Value() : AT_MIN_LOD);
}

// 0x480630
float ProxyRObj::Get_Post_Increment_Value(void) const
{
	return((Proxied != NULL) ? Proxied->Get_Post_Increment_Value() : AT_MAX_LOD);
}

// 0x480660
void ProxyRObj::Set_LOD_Level(int lod)
{
	if (Proxied != NULL) {
		Proxied->Set_LOD_Level(lod);
	}
}

// 0x480690
int ProxyRObj::Get_LOD_Level(void) const
{
	if (Proxied != NULL) {
		return(Proxied->Get_LOD_Level());
	}
	return(0);
}

// 0x4806C0
int ProxyRObj::Get_LOD_Count(void) const
{
	if (Proxied != NULL) {
		return(Proxied->Get_LOD_Count());
	}
	return(1);
}

// 0x4806F0
void ProxyRObj::Set_LOD_Bias(float bias)
{
	if (Proxied != NULL) {
		Proxied->Set_LOD_Bias(bias);
	}
}

// 0x480720
RenderObjClass *ProxyRObj::Get_Current_LOD(void)
{
	if (Proxied != NULL) {
		return(Proxied->Get_Current_LOD());
	}
	Add_Ref();
	return(this);
}

// 0x480750
MaterialInfoClass *ProxyRObj::Get_Material_Info(void)
{
	if (Proxied != NULL) {
		return(Proxied->Get_Material_Info());
	}
	return(NULL);
}

// 0x480780
int ProxyRObj::Is_Translucent(void) const
{
	if (Proxied != NULL) {
		return(Proxied->Is_Translucent());
	}
	return(0);
}

// 0x4807B0
void ProxyRObj::Set_Translucent(int onoff)
{
	if (Proxied != NULL) {
		Proxied->Set_Translucent(onoff);
	}
}

// 0x4807E0
bool ProxyRObj::Is_Complete(void)
{
	if (Proxied != NULL) {
		return(Proxied->Is_Complete());
	}
	return(false);
}

// 0x480810
void ProxyRObj::Unknown_480810(float scale)
{
	if (Proxied != NULL) {
		Proxied->Scale(scale);
	}
}

// 0x480840
int ProxyRObj::Get_Num_Sub_Objects(void) const
{
	if (Proxied != NULL) {
		return(Proxied->Get_Num_Sub_Objects());
	}
	return(0);
}

// 0x480870
RenderObjClass *ProxyRObj::Get_Sub_Object(int index) const
{
	if (Proxied != NULL) {
		return(Proxied->Get_Sub_Object(index));
	}
	return(NULL);
}

// 0x4808A0
int ProxyRObj::Add_Sub_Object(RenderObjClass *subobj)
{
	if (Proxied != NULL) {
		return(Proxied->Add_Sub_Object(subobj));
	}
	return(0);
}

// 0x4808D0
int ProxyRObj::Remove_Sub_Object(RenderObjClass *robj)
{
	if (Proxied != NULL) {
		return(Proxied->Remove_Sub_Object(robj));
	}
	return(0);
}

// 0x480900
RenderObjClass *ProxyRObj::Get_Sub_Object_By_Name(const char *name) const
{
	if (Proxied != NULL) {
		return(Proxied->Get_Sub_Object_By_Name(name));
	}
	return(NULL);
}

// 0x480930
int ProxyRObj::Get_Num_Sub_Objects_On_Bone(int boneindex) const
{
	if (Proxied != NULL) {
		return(Proxied->Get_Num_Sub_Objects_On_Bone(boneindex));
	}
	return(0);
}

// 0x480960
RenderObjClass *ProxyRObj::Get_Sub_Object_On_Bone(int index, int boneindex) const
{
	if (Proxied != NULL) {
		return(Proxied->Get_Sub_Object_On_Bone(index, boneindex));
	}
	return(NULL);
}

// 0x4809A0
int ProxyRObj::Get_Sub_Object_Bone_Index(RenderObjClass *subobj) const
{
	if (Proxied != NULL) {
		return(Proxied->Get_Sub_Object_Bone_Index(subobj));
	}
	return(0);
}

// 0x4809D0
int ProxyRObj::Add_Sub_Object_To_Bone(RenderObjClass *subobj, int bone_index)
{
	if (Proxied != NULL) {
		return(Proxied->Add_Sub_Object_To_Bone(subobj, bone_index));
	}
	return(0);
}

// 0x480A10
int ProxyRObj::Remove_Sub_Objects_From_Bone(const char *bname)
{
	if (Proxied != NULL) {
		return(Proxied->Remove_Sub_Objects_From_Bone(bname));
	}
	return(0);
}

// 0x480A40 (recursive; Render inlines one level of it)
static void Unknown_480A40(RenderObjClass *robj, RenderInfoClass &rinfo)
{
	if (robj->Class_ID() == RenderObjClass::CLASSID_PARTICLEEMITTER) {
		((ParticleEmitterClass *)robj)->Peek_Buffer()->Render(rinfo);
		return;
	}
	int count = robj->Get_Num_Sub_Objects();
	for (int i = 0; i < count; i++) {
		RenderObjClass *sub = robj->Get_Sub_Object(i);
		Unknown_480A40(sub, rinfo);
		sub->Release_Ref();
	}
}

/*
**	Notes
**
**	Placement: operator= (0x47FC90) pushes this file's __FILE__ 0xB71AEC, line 56; the constructor, the deleting
**	destructor and the copy constructor before it install the same table 0xAE9D88, and the object runs to the
**	adjustors 0x480AD0 / 0x480AE0 (the slots of 0xAE9D70 / 0xAE9D6C) and Class_ID 0x480AF0. Get_Sort_Level /
**	Set_Sort_Level (0x47F920 / 0x47F940) are inlines in the table of ro_offset.h's class too, so the client kept
**	their copies in the object before this one.
*/
