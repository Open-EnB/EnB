#if defined(_MSC_VER)
#pragma once
#endif

#ifndef RS_TRACK_H
#define RS_TRACK_H

#include "rendobj.h"
#include "sphere.h"
#include "aabox.h"

class HAnimComboClass;
class HAnimComboDataClass;
class CullableClass;
class RsTrackClass;
class RenderStateListClass;

// rs_list.cpp's render state and render state list, declared as far as rs_track.cpp uses them
// (interim placement: rs_list.cpp / its header are not in the tree yet; D137)
class RenderStateClass	// NAME 2: rs_track.cpp's assert names State and Get_Render_State_List()
{
public:
	virtual ~RenderStateClass();									// +0x00
	virtual void Unknown_04();										// +0x04  // NAME 1: neutral (D10)
	virtual void Unknown_08(RsTrackClass *owner, int link_id);		// +0x08 (INIT_RENDER_STATE 47)  // NAME 1 [owner, link_id]: neutral; what 0x460880 passes
	virtual void Unknown_0C();										// +0x0C  // NAME 1: neutral (D10)
	virtual void Unknown_10();										// +0x10  // NAME 1: neutral (D10)
	virtual void Unknown_14(RsTrackClass *owner);					// +0x14 (0x460440 activates through it)  // NAME 1 [owner]: neutral
	virtual void Unknown_18(RsTrackClass *owner);					// +0x18 (0x460710 deactivates through it)  // NAME 1 [owner]: neutral
	virtual void Unknown_1C(RsTrackClass *owner);					// +0x1C  // NAME 1 [owner]: neutral
	virtual RenderObjClass *Peek_Render_Object();					// +0x20  // NAME 2: the assert 'Peek_Render_Object()' at rs_track.cpp line 236
	virtual void Unknown_24();										// +0x24  // NAME 1: neutral (D10)
	virtual void Unknown_28();										// +0x28  // NAME 1: neutral (D10)
	virtual void Unknown_2C();										// +0x2C  // NAME 1: neutral (D10)
	virtual void Unknown_30();										// +0x30  // NAME 1: neutral (D10)
	virtual void Unknown_34();										// +0x34  // NAME 1: neutral (D10)
	virtual void Unknown_38();										// +0x38  // NAME 1: neutral (D10)
	virtual void Unknown_3C();										// +0x3C (ACTIVATE_NEXT_RENDER_STATE 49)  // NAME 1: neutral (D10)
	virtual void Unknown_40();										// +0x40  // NAME 1: neutral (D10)
	virtual void Unknown_44(int arg);								// +0x44  // NAME 1 [arg]: neutral
	virtual void Unknown_48(int arg);								// +0x48  // NAME 1 [arg]: neutral
	void Unknown_458010();											// 0x458010 (resolves Link from LinkID)  // NAME 1: neutral (D10)
	RenderStateListClass *Get_Render_State_List() { return(List); }	// the assert at rs_track.cpp line 191
	bool Unknown_Flag4() const { return((Flags >> 4) & 1); }		// NAME 1: neutral (D10)
	bool Unknown_Flag10() const { return((Flags >> 10) & 1); }		// NAME 1: neutral (D10)
	bool Unknown_Flag16() const { return((Flags >> 16) & 1); }		// NAME 1: neutral (D10)

	char Unknown04[0x0C - 0x04];									// NAME 1: neutral (D10)
	RenderStateClass *Next;											// +0x0C  // NAME 2: the list walk
	RenderStateListClass *List;										// +0x10  // NAME 2: Get_Render_State_List()
	int Unknown14;													// +0x14  // NAME 1: neutral (D10)
	int ID;															// +0x18  // NAME 2: the render state id the packets name
	int LinkID;														// +0x1C  // NAME 2: -1 ends the chain (INIT_RENDER_STATE's LinkID)
	RenderStateClass *Link;											// +0x20  // NAME 2: the next state of the chain
	int Unknown24;													// +0x24  // NAME 1: neutral (D10)
	unsigned int Flags;												// +0x28  // NAME 2: bit tests
};

class RenderStateListClass	// NAME 2: the member RenderStateList of rs_track.cpp's assert
{
public:
	RenderStateListClass(RenderStateListClass *source);				// 0x45C1B0  // NAME 1 [source]: neutral
	~RenderStateListClass();										// 0x45C310
	virtual void Unknown_00();										// +0x00  // NAME 1: neutral (D10)
	virtual void Unknown_04(float scale, float *hsv);				// +0x04  // NAME 1 [scale, hsv]: what 0x45FFB0 passes
	RenderStateClass *Unknown_45A720(int id);						// 0x45A720  // NAME 1 [id]: neutral
	RenderStateClass *Unknown_45A750(int id);						// 0x45A750  // NAME 1 [id]: neutral
	void Unknown_45A870(RsTrackClass *owner, float seconds);		// 0x45A870  // NAME 1 [owner, seconds]: neutral; WW3D's frame time in s
	void Unknown_45A8F0(RsTrackClass *owner, RenderInfoClass &rinfo);	// 0x45A8F0  // NAME 1 [owner, rinfo]: neutral
	void Unknown_45A970(RsTrackClass *owner, RenderInfoClass &rinfo);	// 0x45A970  // NAME 1 [owner, rinfo]: neutral
	void Unknown_45A9F0(RsTrackClass *owner);						// 0x45A9F0  // NAME 1 [owner]: neutral
	void Unknown_45AA40(void *value, bool recursive);				// 0x45AA40  // NAME 1 [value, recursive]: Set_User_Data's

	RenderStateClass *Head;											// +0x04  // NAME 2: the list walk
	char Unknown08[0x24 - 0x08];									// NAME 1: neutral (D10)
	RenderStateClass *Current;										// +0x24  // NAME 2: Peek_Render_Object's state
	RenderStateListClass *Source;									// +0x28  // NAME 2: what Clone passes on
	unsigned char Unknown2C;										// +0x2C  // NAME 1: neutral (D10)
};

// proj_man's helpers RsTrackClass makes (0x462470, 0x462530), declared as far as it calls them
class Unknown45D9E0Class : public RefCountClass	// NAME 1: neutral (D10)
{
public:
	Unknown45D9E0Class(RsTrackClass *owner, int a, int b);			// 0x45D9E0  // NAME 1 [owner, a, b]: neutral
	void Unknown_493FE0();											// 0x493FE0 (proj_man.cpp)  // NAME 1: neutral (D10)
	char Unknown08[0x24 - 0x08];									// 0x24 bytes (0x462470's new)  // NAME 1: neutral (D10)
};
class Unknown45DC80Class : public RefCountClass	// NAME 1: neutral (D10)
{
public:
	Unknown45DC80Class(RsTrackClass *owner, CullableClass *cull, int a);	// 0x45DC80  // NAME 1 [owner, cull, a]: neutral
	void Unknown_45DFC0(RenderInfoClass &rinfo);					// 0x45DFC0  // NAME 1 [rinfo]: the Render argument
	void Unknown_45E030(RenderInfoClass &rinfo);					// 0x45E030  // NAME 1 [rinfo]: the Render argument
	char Unknown08[0x34 - 0x08];									// 0x34 bytes (0x462530's new)  // NAME 1: neutral (D10)
};

// rs_track.cpp's render object (0x100 bytes, Class_ID 0x10001): a render object that shows one render state's
// model at a time
class RsTrackClass : public RenderObjClass	// NAME 3: class-from-file (D24) for app\shared\rs_track.cpp
{
public:
	RsTrackClass(RenderStateListClass *list);						// 0x45FDA0  // NAME 1 [list]: neutral
	RsTrackClass(RenderStateListClass *list, float scale, float *hsv);	// 0x45FFB0 (list: ClientClass +0x18's 0x764030)  // NAME 1 [list, scale, hsv]: what 0x6F51D0 passes
	virtual ~RsTrackClass();										// 0x460CB0

	virtual RenderObjClass *Clone(void) const;						// 0x4601D0
	virtual int Class_ID(void) const { return(0x10001); }			// 0x4633C0
	virtual const char *Get_Name(void) const;						// 0x4614E0
	virtual const char *Get_Base_Model_Name(void) const;			// 0x461510
	virtual int Get_Num_Polys(void) const;							// 0x4617D0
	virtual void Render(RenderInfoClass &rinfo);					// 0x460FF0
	virtual void On_Frame_Update(void);								// 0x461460
	virtual void Validate_Transform(void) const;					// 0x460BF0
	virtual void Set_Transform(const Matrix3D &m);					// 0x461580
	virtual void Set_Position(const Vector3 &v);					// 0x4615C0
	virtual void Notify_Added(SceneClass *scene);					// 0x461600
	virtual void Notify_Removed(SceneClass *scene);					// 0x461650
	virtual int Get_Num_Sub_Objects(void) const;					// 0x462090
	virtual RenderObjClass *Get_Sub_Object(int index) const;		// 0x4620C0
	virtual int Add_Sub_Object(RenderObjClass *subobj);				// 0x462100
	virtual int Remove_Sub_Object(RenderObjClass *robj);			// 0x461EF0
	virtual RenderObjClass *Get_Sub_Object_By_Name(const char *name) const;	// 0x461F30
	virtual int Get_Num_Sub_Objects_On_Bone(int boneindex) const;	// 0x461F70
	virtual RenderObjClass *Get_Sub_Object_On_Bone(int index, int boneindex) const;	// 0x461FB0
	virtual int Get_Sub_Object_Bone_Index(RenderObjClass *subobj) const;	// 0x461FF0
	virtual int Add_Sub_Object_To_Bone(RenderObjClass *subobj, int bone_index);	// 0x462140
	virtual int Remove_Sub_Objects_From_Bone(const char *bname);	// 0x462030
	virtual void Update_Sub_Object_Transforms(void);				// 0x461A90
	virtual void Set_Animation(HAnimClass *motion, float frame, int anim_mode = ANIM_MODE_MANUAL);	// 0x461540
	virtual int Get_Num_Bones(void);								// 0x462220
	virtual const char *Get_Bone_Name(int bone_index);				// 0x462250
	virtual int Get_Bone_Index(const char *bonename);				// 0x462290
	virtual const Matrix3D &Get_Bone_Transform(const char *bonename);	// 0x4622D0
	virtual const Matrix3D &Get_Bone_Transform(int boneindex);		// 0x462320
	virtual void Capture_Bone(int bindex);							// 0x462370
	virtual void Release_Bone(int bindex);							// 0x4623B0
	virtual bool Is_Bone_Captured(int bindex) const;				// 0x4623F0
	virtual void Control_Bone(int bindex, const Matrix3D &objtm, bool world_space_translation = false);	// 0x462430
	virtual bool Cast_Ray(RayCollisionTestClass &raytest);			// 0x461B00
	virtual bool Cast_AABox(AABoxCollisionTestClass &boxtest);		// 0x461B50
	virtual bool Cast_OBBox(OBBoxCollisionTestClass &boxtest);		// 0x461BA0
	virtual bool Intersect_AABox(AABoxIntersectionTestClass &boxtest);	// 0x461BF0
	virtual bool Intersect_OBBox(OBBoxIntersectionTestClass &boxtest);	// 0x461C40
	virtual bool Intersect(IntersectionClass *Intersection, IntersectionResultClass *Final_Result);	// 0x461C90
	virtual bool Intersect_Sphere(IntersectionClass *Intersection, IntersectionResultClass *Final_Result);	// 0x461CE0
	virtual bool Intersect_Sphere_Quick(IntersectionClass *Intersection, IntersectionResultClass *Final_Result);	// 0x461D20
	virtual const SphereClass &Get_Bounding_Sphere(void) const;		// 0x4616A0
	virtual void Get_Obj_Space_Bounding_Sphere(SphereClass &sphere) const;	// 0x461740
	virtual void Get_Obj_Space_Bounding_Box(AABoxClass &box) const;	// 0x461780
	virtual void Update_Obj_Space_Bounding_Volumes(void);			// 0x461800
	virtual void Prepare_LOD(CameraClass &camera);					// 0x461090
	virtual void Recalculate_Static_LOD_Factors(void);				// 0x4610E0
	virtual void Increment_LOD(void);								// 0x461110
	virtual void Decrement_LOD(void);								// 0x461140
	virtual float Get_Cost(void) const;								// 0x461170
	virtual float Get_Value(void) const;							// 0x4611B0
	virtual float Get_Post_Increment_Value(void) const;				// 0x4611F0
	virtual void Set_LOD_Level(int lod);							// 0x461230
	virtual int Get_LOD_Level(void) const;							// 0x461270
	virtual int Get_LOD_Count(void) const;							// 0x4612A0
	virtual void Set_LOD_Bias(float bias);							// 0x4612D0
	virtual int Calculate_Cost_Value_Arrays(float screen_area, float *values, float *costs) const;	// 0x461310
	virtual void Create_Decal(DecalGeneratorClass *generator);		// 0x461360
	virtual void Delete_Decal(uint32 decal_id);						// 0x4613A0
	virtual void Set_User_Data(void *value, bool recursive = false);	// 0x4614A0
	virtual void Scale(float scale);								// 0x4613E0
	virtual void Scale(float scalex, float scaley, float scalez);	// 0x461420
	virtual void Update_Cached_Bounding_Volumes(void) const;		// 0x461910

	virtual void Unknown_1C4(int arg) {}							// +0x1C4 0x4633A0  // NAME 1 [arg]: neutral
	virtual bool Unknown_1C8();										// +0x1C8 0x460E80 (On_Frame_Update: the animation step)  // NAME 1: neutral (D10)
	virtual void Unknown_1CC(RenderStateClass *state, RenderObjClass *robj);	// +0x1CC 0x460940 (the state switch)  // NAME 1 [state, robj]: neutral
	virtual bool Unknown_1D0(int arg, const char *bone_path, RenderObjClass *robj, int arg2);	// +0x1D0 0x4609C0 (adds robj on a bone path; ret 0x10)  // NAME 1 [arg .. arg2]: neutral
	virtual void Unknown_1D4(int a, int b, int c);					// +0x1D4 0x462070  // NAME 1 [a, b, c]: neutral
	virtual void Unknown_1D8(int a, int b);							// +0x1D8 0x460BA0  // NAME 1 [a, b]: neutral

	void Unknown_460260(RenderObjClass *robj);						// 0x460260 (the shown model)  // NAME 1 [robj]: neutral
	RenderObjClass *Unknown_460410();								// 0x460410  // NAME 1: neutral (D10)
	void Unknown_460440(RenderStateClass *state, int depth);		// 0x460440 (activates a state and its links)  // NAME 1 [state, depth]: neutral
	void Unknown_4604D0(int render_state_id);						// 0x4604D0 (ACTIVATE_RENDER_STATE 48)
	void Unknown_4605A0(int render_state_id, int arg);				// 0x4605A0  // NAME 1 [arg]: neutral
	void Unknown_4605E0(int render_state_id, int arg);				// 0x4605E0  // NAME 1 [arg]: neutral
	void Unknown_460710(RenderStateClass *state, int depth);		// 0x460710 (deactivates a state chain)  // NAME 1 [state, depth]: neutral
	void Unknown_460780(int render_state_id);						// 0x460780 (DEACTIVATE_RENDER_STATE 50)
	bool Unknown_460810(int render_state_id);						// 0x460810  // NAME 1: neutral (D10)
	void Unknown_460850(int render_state_id);						// 0x460850 (ACTIVATE_NEXT_RENDER_STATE 49)
	void Unknown_460880(int render_state_id, int link_id);			// 0x460880 (INIT_RENDER_STATE 47)
	void Unknown_460BC0();											// 0x460BC0  // NAME 1: neutral (D10)
	int Unknown_461D60(RenderObjClass *robj, const char *bone_path);	// 0x461D60 (removes robj from a bone path)  // NAME 1 [robj, bone_path]: neutral
	void Unknown_462180(HAnimComboDataClass *data);					// 0x462180  // NAME 1 [data]: neutral
	void Unknown_462470(int a, int b);								// 0x462470  // NAME 1 [a, b]: neutral
	void Unknown_462530(int a);										// 0x462530  // NAME 1 [a]: neutral
	void Unknown_462610();											// 0x462610  // NAME 1: neutral (D10)
	void Unknown_462650();											// 0x462650  // NAME 1: neutral (D10)
	void Unknown_462690();											// 0x462690  // NAME 1: neutral (D10)

	RenderObjClass *Peek_Render_Object() const { return(RenderStateList.Current == NULL ? NULL : RenderStateList.Current->Peek_Render_Object()); }

	unsigned char Unknown8C;										// +0x8C  // NAME 1: neutral (D10)
	RenderStateListClass RenderStateList;							// +0x90 (the assert's)
	bool UnknownC0;													// +0xC0  // NAME 1: neutral (D10)
	HAnimComboClass *UnknownC4;										// +0xC4  // NAME 1: neutral (D10)
	Unknown45D9E0Class *UnknownC8;									// +0xC8  // NAME 1: neutral (D10)
	Unknown45DC80Class *UnknownCC;									// +0xCC  // NAME 1: neutral (D10)
	CullableClass *CullingObject;									// +0xD0 (the assert's)
	bool UnknownD4;													// +0xD4  // NAME 1: neutral (D10)
	SphereClass UnknownD8;											// +0xD8 (object space)  // NAME 1: neutral (D10)
	AABoxClass UnknownE8;											// +0xE8 (object space)  // NAME 1: neutral (D10)
};

#endif
