// Earth & Beyond client (reconstruction): app\shared\ro_proxy.h (the client's strings name ro_proxy.cpp, not
// this header; the file name follows the object's, as ro_offset.h beside it)
#ifndef __RO_PROXY_H
#define __RO_PROXY_H

#include "always.h"
#include "matrix3d.h"
#include "..\..\ww3d2\rendobj.h"

// A render object that stands in for another, its "proxied" object: nearly every virtual forwards to it.
// Table 0xAE9D88; 0xC4 bytes. ro_offset.h's class (constructor 0x47F560, table 0xAE9B48, 0xD0 bytes) derives from it.
class ProxyRObj : public RenderObjClass	// the client's profile name 'ProxyRObj::Render' (0xB71B34)
{
public:
	ProxyRObj(bool arg);									// 0x47FA70  // NAME 1 [arg]: neutral
	ProxyRObj(const ProxyRObj &src);						// 0x47FB60
	const ProxyRObj &operator=(const ProxyRObj &src);		// 0x47FC90
	virtual ~ProxyRObj(void);								// 0x47FCD0

	virtual RenderObjClass *Clone(void) const;				// 0x47FDE0
	virtual int Class_ID(void) const { return(0x10002); }	// 0x480AF0
	virtual const char *Get_Name(void) const;				// 0x47FFF0
	virtual int Get_Num_Polys(void) const;					// 0x480070
	virtual void Render(RenderInfoClass &rinfo);			// 0x4800A0

	virtual int Get_Num_Sub_Objects(void) const;			// 0x480840
	virtual RenderObjClass *Get_Sub_Object(int index) const;	// 0x480870
	virtual int Add_Sub_Object(RenderObjClass *subobj);		// 0x4808A0
	virtual int Remove_Sub_Object(RenderObjClass *robj);	// 0x4808D0
	virtual RenderObjClass *Get_Sub_Object_By_Name(const char *name) const;	// 0x480900
	virtual int Get_Num_Sub_Objects_On_Bone(int boneindex) const;	// 0x480930
	virtual RenderObjClass *Get_Sub_Object_On_Bone(int index, int boneindex) const;	// 0x480960
	virtual int Get_Sub_Object_Bone_Index(RenderObjClass *subobj) const;	// 0x4809A0
	virtual int Add_Sub_Object_To_Bone(RenderObjClass *subobj, int bone_index);	// 0x4809D0
	virtual int Remove_Sub_Objects_From_Bone(const char *bname);	// 0x480A10

	virtual bool Cast_Ray(RayCollisionTestClass &raytest);	// 0x4803B0
	virtual bool Cast_AABox(AABoxCollisionTestClass &boxtest);	// 0x4803E0
	virtual bool Cast_OBBox(OBBoxCollisionTestClass &boxtest);	// 0x480410
	virtual bool Intersect_AABox(AABoxIntersectionTestClass &boxtest);	// 0x480440
	virtual bool Intersect_OBBox(OBBoxIntersectionTestClass &boxtest);	// 0x480470
	virtual bool Intersect(IntersectionClass *Intersection, IntersectionResultClass *Final_Result);	// 0x4802F0
	virtual bool Intersect_Sphere(IntersectionClass *Intersection, IntersectionResultClass *Final_Result);	// 0x480330
	virtual bool Intersect_Sphere_Quick(IntersectionClass *Intersection, IntersectionResultClass *Final_Result);	// 0x480370

	virtual void Get_Obj_Space_Bounding_Sphere(SphereClass &sphere) const;	// 0x4804A0
	virtual void Get_Obj_Space_Bounding_Box(AABoxClass &box) const;	// 0x4804F0

	virtual void Prepare_LOD(CameraClass &camera);			// 0x480540
	virtual void Increment_LOD(void);						// 0x480570
	virtual void Decrement_LOD(void);						// 0x4805A0
	virtual float Get_Cost(void) const;						// 0x4805D0
	virtual float Get_Value(void) const;					// 0x480600
	virtual float Get_Post_Increment_Value(void) const;		// 0x480630
	virtual void Set_LOD_Level(int lod);					// 0x480660
	virtual int Get_LOD_Level(void) const;					// 0x480690
	virtual int Get_LOD_Count(void) const;					// 0x4806C0
	virtual void Set_LOD_Bias(float bias);					// 0x4806F0
	virtual RenderObjClass *Get_Current_LOD(void);			// 0x480720

	virtual MaterialInfoClass *Get_Material_Info(void);		// 0x480750

	virtual int Get_Sort_Level(void) const { return(SortLevel); }	// 0x47F920
	virtual void Set_Sort_Level(int level) { SortLevel = level; }	// 0x47F940

	virtual int Is_Translucent(void) const;					// 0x480780
	virtual void Set_Translucent(int onoff);				// 0x4807B0
	virtual bool Is_Complete(void);							// 0x4807E0

	virtual void Unknown_480810(float scale);				// 0x480810 (slot +0x1C4, new here: the proxied object's Scale)  // NAME 1: neutral

	bool Unknown_47FF70(RenderObjClass *obj);				// 0x47FF70 (the proxied object, referenced; false when it is already there)  // NAME 1 [obj]: neutral

	RenderObjClass *Proxied;								// +0x8C (ro_offset.h's assert 'Proxied', 0xB71A94)
	Matrix3D Unknown090;									// +0x90 identity; the proxied object's transform as it was before Render set this object's
	char SortLevel;											// +0xC0 (Get_Sort_Level / Set_Sort_Level; Render's static sort list)  // NAME 3: what it holds
	bool Unknown0C1;										// +0xC1 (Render: the proxied object's particle buffers drawn as well)
};

#endif //__RO_PROXY_H
