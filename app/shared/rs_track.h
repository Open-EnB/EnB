#if defined(_MSC_VER)
#pragma once
#endif

#ifndef RS_TRACK_H
#define RS_TRACK_H

#include "rendobj.h"
#include "sphere.h"
#include "aabox.h"
#include "listnode.h"
#include "proj_man.h"

class HAnimComboClass;
class HAnimComboDataClass;
class CullableClass;
class RsTrackClass;
class RenderStateListClass;

// the flag groups rs.cpp's asserts name (0x458370-0x458460)
#define RS_MUTABLE_FLAG_MASK	0x300										// NAME 3 [RS_MUTABLE_FLAG_MASK]: the asserts' text
#define RS_THREAD_FLAG_MASK		0x3000										// NAME 3 [RS_THREAD_FLAG_MASK]: the asserts' text (the loader thread's bits 12 / 13)
#define RS_DATA_PRESENT			9											// NAME 3 [RS_DATA_PRESENT]: rs_robj.cpp's assert text (Test_Flag(RS_DATA_PRESENT): bit 9, 0x200, set with the model)

// rs.cpp's render state and rs_list.cpp's render state list, declared as far as rs.cpp and rs_track.cpp use them
// (interim placement: rs.h / rs_list.cpp are not in the tree yet; D137)
class RenderStateClass : public Node<RenderStateClass *>	// NAME 2: rs_track.cpp's assert names State and Get_Render_State_List()
{
public:
	RenderStateClass(RenderStateListClass *list, int id, int link_id, unsigned int flags);	// 0x457B60 (rs_robj.cpp's constructors call it)  // NAME 1 [list, id, link_id, flags]: the members they set
	RenderStateClass(RenderStateListClass *list, const RenderStateClass &source);	// 0x457BE0 (the derived states' copies)  // NAME 1 [list, source]: neutral
	virtual ~RenderStateClass();									// 0x457D20 (deleting 0x457BB0)
	virtual RenderStateClass *Unknown_04(RenderStateListClass *list) = 0;	// +0x04 (pure: E&B's _purecall 0x9CCD10; 0x458970 clones into list)  // NAME 1 [list]: neutral
	virtual void Unknown_08(RsTrackClass *owner, int link_id) {}	// +0x08 0x458590 (INIT_RENDER_STATE 47)  // NAME 1 [owner, link_id]: neutral; what 0x460880 passes
	virtual void Unknown_0C() {}									// +0x0C 0x4585B0  // NAME 1: neutral (D10)
	virtual bool Unknown_10() { return(true); }						// +0x10 0x4585D0 (the loader thread 0x458150 runs it)  // NAME 1: neutral (D10)
	virtual void Unknown_14(RsTrackClass *owner) = 0;				// +0x14 (pure; 0x460440 activates through it)  // NAME 1 [owner]: neutral
	virtual void Unknown_18(RsTrackClass *owner);					// +0x18 0x4582E0 (0x460710 deactivates through it)  // NAME 1 [owner]: neutral
	virtual void Unknown_1C(RsTrackClass *owner);					// +0x1C 0x458330  // NAME 1 [owner]: neutral
	virtual RenderObjClass *Peek_Render_Object() { return(NULL); }	// +0x20 0x4585F0  // NAME 2: the assert 'Peek_Render_Object()' at rs_track.cpp line 236
	virtual void Unknown_24(void *value, bool recursive);			// +0x24 0x4582C0 (rs_robj.cpp's 0x45EC90 passes them to Set_User_Data)  // NAME 1 [value, recursive]: Set_User_Data's
	virtual void Unknown_28(int a, int b) {}						// +0x28 0x458610  // NAME 1 [a, b]: neutral
	virtual void Unknown_2C(int a, int b) {}						// +0x2C 0x458630  // NAME 1 [a, b]: neutral
	virtual void Unknown_30(RsTrackClass *owner, float seconds);	// +0x30 0x458350 (rs_robj.cpp's 0x45ECD0 adds seconds to its timer)  // NAME 1 [owner, seconds]: Unknown_45A870's
	virtual void Unknown_34(float seconds) {}						// +0x34 0x458650 (0x4584B0 passes the time left over)  // NAME 1 [seconds]: neutral
	virtual void Unknown_38(int a) {}								// +0x38 0x458670  // NAME 1 [a]: neutral
	virtual void Unknown_3C() {}									// +0x3C 0x458690 (ACTIVATE_NEXT_RENDER_STATE 49)  // NAME 1: neutral (D10)
	virtual void Unknown_40(float scale, float *hsv) {}				// +0x40 0x4586B0 (rs_robj.cpp's 0x45F280 remakes its model at scale / hsv)  // NAME 1 [scale, hsv]: what the asset manager's Unknown_78 takes
	virtual void Unknown_44(int arg) {}								// +0x44 0x4586D0  // NAME 1 [arg]: neutral
	virtual void Unknown_48(int arg) {}								// +0x48 0x4586F0  // NAME 1 [arg]: neutral
	void Unknown_457E10();											// 0x457E10 (flag 4 on, into the list's +0x20 chain)  // NAME 1: neutral (D10)
	void Unknown_457E90();											// 0x457E90 (flag 4 off, unlinked)  // NAME 1: neutral (D10)
	void Unknown_457EE0(const char *text);							// 0x457EE0 (copies text into Unknown14)  // NAME 1 [text]: neutral
	void Unknown_457F60(RenderStateListClass *list);				// 0x457F60 (pushes the chain onto list's Head)  // NAME 1 [list]: neutral
	void Unknown_458010();											// 0x458010 (resolves Link from LinkID)  // NAME 1: neutral (D10)
	void Unknown_458040();											// 0x458040 (waits for the loader thread)  // NAME 1: neutral (D10)
	void Unknown_4580D0();											// 0x4580D0 (starts the loader thread 0x458150)  // NAME 1: neutral (D10)
	void Unknown_458370(int flag);									// 0x458370 (sets a RS_MUTABLE_FLAG_MASK flag)  // NAME 1 [flag]: the asserts' text
	void Unknown_4583C0(int flag);									// 0x4583C0 (clears it)  // NAME 1 [flag]: the asserts' text
	void Unknown_458410(int flag);									// 0x458410 (sets a RS_THREAD_FLAG_MASK flag)  // NAME 1 [flag]: the asserts' text
	void Unknown_458460(int flag);									// 0x458460 (clears it)  // NAME 1 [flag]: the asserts' text
	void Unknown_4584B0(RsTrackClass *owner, float seconds, RenderStateClass *next);	// 0x4584B0  // NAME 1 [owner, seconds, next]: neutral; 0x45ECD0 passes the time left over
	void Unknown_458540(const char *text);							// 0x458540 (Unknown24 from text)  // NAME 1 [text]: neutral
	RenderStateListClass *Get_Render_State_List() { return(List); }	// the assert at rs_track.cpp line 191
	bool Unknown_Flag4() const { return((Flags >> 4) & 1); }		// NAME 1: neutral (D10)
	bool Unknown_Flag10() const { return((Flags >> 10) & 1); }		// NAME 1: neutral (D10)
	bool Unknown_Flag12() const { return((Flags >> 12) & 1); }		// NAME 1: neutral (D10)
	bool Unknown_Flag13() const { return((Flags >> 13) & 1); }		// NAME 1: neutral (D10)
	bool Unknown_Flag16() const { return((Flags >> 16) & 1); }		// NAME 1: neutral (D10)
	bool Test_Flag(int flag) const { return((Flags >> flag) & 1); }	// NAME 2: rs_robj.cpp's assert text
	const char *Get_Source_Filename() const { return(Unknown14); }	// NAME 2: rs_robj.cpp's assert text (0x45EF80 loads it when the model is missing)

	RenderStateClass *Next;											// +0x0C  // NAME 2: the list walk
	RenderStateListClass *List;										// +0x10  // NAME 2: Get_Render_State_List()
	char *Unknown14;												// +0x14 (0x457EE0's copy)  // NAME 1: neutral (D10)
	int ID;															// +0x18  // NAME 2: the render state id the packets name
	int LinkID;														// +0x1C  // NAME 2: -1 ends the chain (INIT_RENDER_STATE's LinkID)
	RenderStateClass *Link;											// +0x20  // NAME 2: the next state of the chain
	char *Unknown24;												// +0x24 (nstrdup'd from the source's)  // NAME 1: neutral (D10)
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
	RenderStateClass *Unknown_45A7E0(int id);						// 0x45A7E0 (rs_robj.cpp's NextRenderState and source state)  // NAME 1 [id]: neutral
	void Unknown_45A870(RsTrackClass *owner, float seconds);		// 0x45A870  // NAME 1 [owner, seconds]: neutral; WW3D's frame time in s
	void Unknown_45A8F0(RsTrackClass *owner, RenderInfoClass &rinfo);	// 0x45A8F0  // NAME 1 [owner, rinfo]: neutral
	void Unknown_45A970(RsTrackClass *owner, RenderInfoClass &rinfo);	// 0x45A970  // NAME 1 [owner, rinfo]: neutral
	void Unknown_45A9F0(RsTrackClass *owner);						// 0x45A9F0  // NAME 1 [owner]: neutral
	void Unknown_45AA40(void *value, bool recursive);				// 0x45AA40  // NAME 1 [value, recursive]: Set_User_Data's

	RenderStateClass *Head;											// +0x04  // NAME 2: the list walk
	char Unknown08[0x20 - 0x08];									// NAME 1: neutral (D10)
	GenericNode *Unknown20;											// +0x20 (the active states' chain 0x457E10 links into)  // NAME 1: neutral (D10)
	RenderStateClass *Current;										// +0x24  // NAME 2: Peek_Render_Object's state
	RenderStateListClass *Source;									// +0x28  // NAME 2: what Clone passes on
	unsigned char Unknown2C;										// +0x2C  // NAME 1: neutral (D10)
};

// rs_robj.cpp's render-object state (vtable 0xAE8138, 0x44 bytes): a state that shows a model (made by name from
// the asset manager or handed in), and after Unknown34 seconds moves on to the state NextID names
// (interim, D137: rs_robj.cpp's header is not in the tree yet)
class Unknown45E850Class : public RenderStateClass	// NAME 1: neutral (D10)
{
public:
	Unknown45E850Class(RenderStateListClass *list, int id, int link_id, unsigned int flags, const char *name, int next_id, float duration);	// 0x45E5F0  // NAME 1 [name, next_id, duration]: the members they set
	Unknown45E850Class(RenderStateListClass *list, int id, int link_id, unsigned int flags, RenderObjClass *robj, int next_id, float duration);	// 0x45E880  // NAME 1 [robj, next_id, duration]: the members they set
	Unknown45E850Class(RenderStateListClass *list, const Unknown45E850Class &source);	// 0x45E9F0  // NAME 1 [list, source]: neutral
	virtual ~Unknown45E850Class();									// 0x45EC00 (deleting 0x45E850)
	virtual RenderStateClass *Unknown_04(RenderStateListClass *list);	// +0x04 0x45EB70
	virtual void Unknown_0C();										// +0x0C 0x45EF10 (drops a model it can load again)  // NAME 1: neutral (D10)
	virtual bool Unknown_10();										// +0x10 0x45EF80 (loads the model)  // NAME 1: neutral (D10)
	virtual void Unknown_14(RsTrackClass *owner);					// +0x14 0x45EE50
	virtual void Unknown_18(RsTrackClass *owner);					// +0x18 0x45EED0
	virtual RenderObjClass *Peek_Render_Object();					// +0x20 0x45EDC0
	virtual void Unknown_24(void *value, bool recursive);			// +0x24 0x45EC90
	virtual void Unknown_30(RsTrackClass *owner, float seconds);	// +0x30 0x45ECD0
	virtual void Unknown_40(float scale, float *hsv);				// +0x40 0x45F280
	RenderStateClass *Get_Next_Render_State();						// 0x45ED70 (no caller; inlined into 0x45ECD0)  // NAME 1: the assert's NextRenderState

	char *RenderObjName;											// +0x2C (nstrdup'd name)  // NAME 2: the asserts' text
	RenderObjClass *RenderObj;										// +0x30  // NAME 2: the asserts' text
	float Unknown34;												// +0x34 (seconds before the next state)  // NAME 1: neutral (D10)
	float Unknown38;												// +0x38 (seconds active)  // NAME 1: neutral (D10)
	int NextID;														// +0x3C (the next state's id)  // NAME 1: after NextRenderState
	RenderStateClass *NextRenderState;								// +0x40  // NAME 2: the assert's text
};

// rs.cpp's render-object state on a bone path (vtable 0xAE7D58, 0x4C bytes): its model goes onto the
// owner's bone path while it is active
class Unknown458710Class : public Unknown45E850Class	// NAME 1: neutral (D10)
{
public:
	Unknown458710Class(RenderStateListClass *list, int id, int link_id, unsigned int flags, const char *name, int next_id, float duration, const char *bone_path, int arg);	// 0x458710  // NAME 1 [bone_path, arg]: what 0x458B20 passes on
	Unknown458710Class(RenderStateListClass *list, int id, int link_id, unsigned int flags, RenderObjClass *robj, int next_id, float duration, const char *bone_path, int arg);	// 0x4588A0
	Unknown458710Class(RenderStateListClass *list, const Unknown458710Class &source);	// 0x458810
	virtual ~Unknown458710Class();									// 0x458A60 (deleting 0x4587E0)
	virtual RenderStateClass *Unknown_04(RenderStateListClass *list);	// +0x04 0x458970
	virtual void Unknown_14(RsTrackClass *owner);					// +0x14 0x458B20
	virtual void Unknown_18(RsTrackClass *owner);					// +0x18 0x458B70
	virtual void Unknown_1C(RsTrackClass *owner);					// +0x1C 0x458BC0
	void Unknown_458AE0(const char *bone_path);						// 0x458AE0  // NAME 1 [bone_path]: neutral

	char *Unknown44;												// +0x44 (the bone path RsTrackClass::Unknown_1D0 / Unknown_461D60 take)  // NAME 1: neutral (D10)
	int Unknown48;													// +0x48 (Unknown_1D0's last argument)  // NAME 1: neutral (D10)
};

// rs_proj_sup.cpp's projector caster (0x45D9E0, vtable 0xAE80B0) and receiver (0x45DC80, vtable 0xAE80EC) RsTrackClass
// makes (0x462470, 0x462530); declared here for now (interim, D137: no rs_proj_sup.h is known)
class Unknown45D9E0Class : public ProjectorCasterClass	// NAME 1: neutral (D10)
{
public:
	Unknown45D9E0Class(RsTrackClass *owner, int def, int manager);	// 0x45D9E0 (0x24 bytes)  // NAME 1 [owner]: neutral; NAME 2 [def, manager]: the base's (int as RsTrackClass::Unknown_462470 passes them)

	virtual RenderObjClass *Peek_Render_Obj(void);					// 0x45DBB0
	virtual Matrix3D Get_Transform(void);							// 0x45DBE0
	virtual uint32 Get_Type_ID(void);								// 0x45DC60

	RsTrackClass *Owner;											// +0x20  // NAME 2: what it points at
};
class Unknown45DC80Class : public ProjectorReceiverClass	// NAME 1: neutral (D10)
{
public:
	Unknown45DC80Class(RsTrackClass *owner, CullableClass *cull, int a);	// 0x45DC80 (0x34 bytes)  // NAME 1 [owner, cull, a]: neutral

	virtual ~Unknown45DC80Class();									// 0x45DDF0 (deleting 0x45DD90)
	virtual void Add_Projector(TexProjectClass *texProject);		// 0x45DF20 (the assert's texProject)
	virtual bool Intersects(const OBBoxClass &box);					// 0x45DF90
	virtual void *Get_Projection_Object_ID(void);					// 0x45E0B0

	void Unknown_45DFC0(RenderInfoClass &rinfo);					// 0x45DFC0: pushes the projectors' material passes  // NAME 1 [rinfo]: the Render argument
	void Unknown_45E030(RenderInfoClass &rinfo);					// 0x45E030: releases them, popping the passes  // NAME 1 [rinfo]: the Render argument

	TexProjListClass ProjectorList;									// +0x18  // NAME 2: what it holds
	RsTrackClass *Owner;											// +0x30  // NAME 2: what it points at
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
