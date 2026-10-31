#include "rs_track.h"
#include "hanim.h"
#include "cullsys.h"
#include "scene.h"
#include "ww3d.h"
#include <string.h>
#include <float.h>
#include "..\shared\unknown_debug.h"

// 0x45FDA0
// NOT EXACT: shape (0.82): the client stores the vtables before loading RenderStateList's +0x2C byte and compares the inlined assert's pointer with ebx
RsTrackClass::RsTrackClass(RenderStateListClass *list) :
	Unknown8C(0),
	RenderStateList(list),
	UnknownC0(false),
	UnknownC4(NULL),
	UnknownC8(NULL),
	UnknownCC(NULL),
	CullingObject(NULL),
	UnknownD4(true),
	UnknownD8(Vector3(0.0f, 0.0f, 0.0f), 0.0f),
	UnknownE8(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f))
{
	Unknown8C = RenderStateList.Unknown2C;
	RenderStateList.Current = NULL;
	for (RenderStateClass *state = list->Head; state != NULL; state = state->Next) {
		if (state->Unknown_Flag16() && state->ID != -1) {
			Unknown_4604D0(state->ID);
		}
	}
}

// 0x45FFB0
// NOT EXACT: shape (0.83): as 0x45FDA0
RsTrackClass::RsTrackClass(RenderStateListClass *list, float scale, float *hsv) :
	Unknown8C(0),
	RenderStateList(list),
	UnknownC0(false),
	UnknownC4(NULL),
	UnknownC8(NULL),
	UnknownCC(NULL),
	CullingObject(NULL),
	UnknownD4(true),
	UnknownD8(Vector3(0.0f, 0.0f, 0.0f), 0.0f),
	UnknownE8(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f))
{
	RenderStateList.Unknown_04(scale, hsv);
	Unknown8C = RenderStateList.Unknown2C;
	RenderStateList.Current = NULL;
	for (RenderStateClass *state = list->Head; state != NULL; state = state->Next) {
		if (state->Unknown_Flag16() && state->ID != -1) {
			Unknown_4604D0(state->ID);
		}
	}
}

// 0x4601D0
RenderObjClass *RsTrackClass::Clone(void) const
{
	return(new RsTrackClass(RenderStateList.Source));
}

// 0x460260
void RsTrackClass::Unknown_460260(RenderObjClass *robj)
{
	if (robj != NULL) {
		robj->Add_Ref();
	}
	RenderObjClass *old = Peek_Render_Object();
	if (old != NULL) {
		if (Is_In_Scene()) {
			old->Notify_Removed(Scene);
		}
		old->Set_Container(NULL);
		old->Release_Ref();
	}
	if (robj != NULL) {
		robj->Set_Container(NULL);
		robj->Set_Container(this);
		if (Is_In_Scene()) {
			robj->Notify_Added(Scene);
		}
		robj->Set_User_Data(Get_User_Data(), true);
		SphereClass sphere;
		robj->Get_Obj_Space_Bounding_Sphere(sphere);
		UnknownD8.Center = sphere.Center;
		UnknownD8.Radius = sphere.Radius;
		robj->Get_Obj_Space_Bounding_Box(UnknownE8);
	} else {
		UnknownD8.Center.Set(0.0f, 0.0f, 0.0f);
		UnknownD8.Radius = 0.0f;
		UnknownE8.Center.Set(0.0f, 0.0f, 0.0f);
		UnknownE8.Extent.Set(0.0f, 0.0f, 0.0f);
	}
	Unknown8C &= ~2;
}

// 0x460410
RenderObjClass *RsTrackClass::Unknown_460410()
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) {
		return(robj->Clone());
	}
	return(NULL);
}

#pragma auto_inline(off)











































































// 0x460440
// NOT EXACT: shape (0.52): ebx / edi swapped and the loop's two decrement paths; never auto-inlined (#pragma auto_inline(off))
void RsTrackClass::Unknown_460440(RenderStateClass *state, int depth)
{
	if (depth < 0) {
		return;
	}
	G_ASSERT(state->Get_Render_State_List() == &RenderStateList);
	state->Unknown_14(this);
	if (state->LinkID != -1) {
		if (state->Link == NULL) {
			state->Unknown_458010();
			if (state->Link == NULL) {
				return;
			}
		}
		Unknown_460440(state->Link, depth - 1);
	}
}
#pragma auto_inline(on)

























// 0x4604D0
void RsTrackClass::Unknown_4604D0(int render_state_id)
{
	if (render_state_id == -1) {
		return;
	}
	if (RenderStateList.Current != NULL && RenderStateList.Current->ID == render_state_id) {
		G_ASSERT(Peek_Render_Object());
		Peek_Render_Object()->Set_Hidden(false);
	} else {
		RenderStateClass *state = RenderStateList.Unknown_45A750(render_state_id);
		if (state != NULL && !state->Unknown_Flag4()) {
			Unknown_460440(state, 50);
			UnknownC0 = true;
		}
	}
}

// 0x4605A0
void RsTrackClass::Unknown_4605A0(int render_state_id, int arg)
{
	if (render_state_id != -1) {
		RenderStateClass *state = RenderStateList.Unknown_45A720(render_state_id);
		if (state != NULL) {
			state->Unknown_44(arg);
		}
	}
}

// 0x4605E0
void RsTrackClass::Unknown_4605E0(int render_state_id, int arg)
{
	if (render_state_id != -1) {
		RenderStateClass *state = RenderStateList.Unknown_45A720(render_state_id);
		if (state != NULL) {
			Unknown_4604D0(render_state_id);
			state->Unknown_48(arg);
		}
	}
}















// 0x460710
void RsTrackClass::Unknown_460710(RenderStateClass *state, int depth)
{
	if (depth < 0) {
		return;
	}
	G_ASSERT(state->Get_Render_State_List() == &RenderStateList);
	state->Unknown_18(this);
	if (state->Link != NULL) {
		Unknown_460710(state->Link, depth);
	}
}

// 0x460780
void RsTrackClass::Unknown_460780(int render_state_id)
{
	if (render_state_id != -1) {
		RenderStateClass *state = RenderStateList.Unknown_45A720(render_state_id);
		if (state != NULL) {
			Unknown_460710(state, 50);
			UnknownC0 = true;
		}
	}
}

// 0x460810
bool RsTrackClass::Unknown_460810(int render_state_id)
{
	if (render_state_id != -1) {
		RenderStateClass *state = RenderStateList.Unknown_45A720(render_state_id);
		if (state != NULL) {
			return(state->Unknown_Flag4());
		}
	}
	return(false);
}

// 0x460850
void RsTrackClass::Unknown_460850(int render_state_id)
{
	RenderStateClass *state = RenderStateList.Unknown_45A750(render_state_id);
	if (state != NULL) {
		state->Unknown_3C();
	}
}

// 0x460880
void RsTrackClass::Unknown_460880(int render_state_id, int link_id)
{
	RenderStateClass *state = RenderStateList.Unknown_45A750(render_state_id);
	if (state != NULL) {
		bool active = false;
		if (state->Unknown_Flag4()) {
			Unknown_460710(state, 50);
			active = true;
		}
		state->Unknown_08(this, link_id);
		if (active) {
			Unknown_460440(state, 50);
		}
	}
}

// 0x460940
void RsTrackClass::Unknown_1CC(RenderStateClass *state, RenderObjClass *robj)
{
	Unknown_460260(robj);
	if (RenderStateList.Current != NULL) {
		RenderStateList.Current->Unknown_18(this);
	}
	RenderStateListClass *list = &RenderStateList;
	list->Current = state;
	for (RenderStateClass *s = RenderStateList.Head; s != NULL; s = s->Next) {
		if (s->Unknown_Flag4()) {
			s->Unknown_1C(this);
		}
	}
}





































// 0x4609C0
bool RsTrackClass::Unknown_1D0(int arg, const char *bone_path, RenderObjClass *obj, int arg2)
{
	bool result = false;
	if (obj != NULL && obj->Get_Container() == NULL) {
		RenderObjClass *model = Peek_Render_Object();
		if (model == NULL) {
			return(false);
		}
		if (bone_path == NULL || *bone_path == '\0') {
			model->Add_Sub_Object(obj);
			result = true;
		} else {
			RenderObjClass *robj = model;
			char path[256];
			strcpy(path, bone_path);
			char *name = strtok(path, "\\");
			while (name != NULL) {
				char *next = strtok(NULL, "\\");
				if (next == NULL) {
					if (robj->Get_Bone_Index(name) != 0) {
						robj->Add_Sub_Object_To_Bone(obj, name);
						result = true;
					}
					break;
				}
				int idx = robj->Get_Bone_Index(name);
				if (idx == 0) {
					break;
				}
				G_ASSERT(robj->Get_Num_Sub_Objects_On_Bone(idx) <= 1);
				robj = robj->Get_Sub_Object_On_Bone(0, idx);
				if (robj == NULL) {
					break;
				}
				robj->Release_Ref();
				name = next;
			}
		}
		if (result) {
			obj->Set_User_Data(Get_User_Data(), true);
		}
	}
	return(result);
}

// 0x460BA0
void RsTrackClass::Unknown_1D8(int a, int b)
{
}

// 0x460BC0
void RsTrackClass::Unknown_460BC0()
{
	UnknownD4 = true;
	Invalidate_Cached_Bounding_Volumes();
}

// 0x460BF0
void RsTrackClass::Validate_Transform(void) const
{
	_isnan(Transform[0][0]);
	if (UnknownD4 || RenderStateList.Current->Unknown_Flag10()) {
		RenderObjClass *robj = Peek_Render_Object();
		if (robj != NULL) {
			robj->Set_Transform(Transform);
			((RsTrackClass *)this)->UnknownD4 = false;
			if (CullingObject != NULL) {
				CullingObject->Set_Cull_Box(Get_Bounding_Box());
			}
			if (UnknownC8 != NULL) {
				UnknownC8->Unknown_493FE0();
			}
		}
	}
}

// 0x460CB0
RsTrackClass::~RsTrackClass()
{
	for (RenderStateClass *state = RenderStateList.Head; state != NULL; state = state->Next) {
		if (state != RenderStateList.Current) {
			state->Unknown_18(this);
		}
	}
	if (RenderStateList.Current != NULL) {
		Unknown_460260(NULL);
		RenderStateList.Current->Unknown_18(this);
	}
	if (UnknownC4 != NULL) {
		delete UnknownC4;
		UnknownC4 = NULL;
	}
	if (UnknownC8 != NULL) {
		UnknownC8->Release_Ref();
		UnknownC8 = NULL;
	}
	if (UnknownCC != NULL) {
		UnknownCC->Release_Ref();
		UnknownCC = NULL;
	}
	if (CullingObject != NULL) {
		CullingObject->Release_Ref();
		CullingObject = NULL;
	}
}

// 0x460E80
// NOT EXACT: shape (0.75): the client saves edi after the early returns; the last call loads UnknownC4 into ecx
bool RsTrackClass::Unknown_1C8()
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj == NULL) {
		return(false);
	}
	if (robj->Is_Hidden()) {
		return(false);
	}
	RenderStateList.Unknown_45A870(this, (float)WW3D::Get_Frame_Time() * 0.001f);
	if ((Unknown8C & 2) == 0) {
		if (UnknownC4 != NULL) {
			UnknownC4->Reset();
		}
		RenderStateList.Unknown_45A9F0(this);
		Unknown8C |= 2;
		if (UnknownC4 != NULL && UnknownC4->Get_Num_Anims() == 0 && (Unknown8C & 4) == 0) {
			delete UnknownC4;
			UnknownC4 = NULL;
		} else if (UnknownC4 != NULL && UnknownC4->Get_Num_Anims() > 0) {
			UnknownC4->Normalize_Weights();
			robj->Set_Animation(UnknownC4);
			return(true);
		}
		robj->Set_Animation();
	}
	return(true);
}

// 0x460FF0
void RsTrackClass::Render(RenderInfoClass &rinfo)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL && !robj->Is_Hidden()) {
		Validate_Transform();
		RenderStateList.Unknown_45A8F0(this, rinfo);
		if (UnknownCC != NULL) {
			UnknownCC->Unknown_45DFC0(rinfo);
		}
		robj->Render(rinfo);
		if (UnknownCC != NULL) {
			UnknownCC->Unknown_45E030(rinfo);
		}
		RenderStateList.Unknown_45A970(this, rinfo);
	}
}

// 0x461090
void RsTrackClass::Prepare_LOD(CameraClass &camera)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL && !robj->Is_Hidden()) {
		robj->Prepare_LOD(camera);
	}
}

// 0x4610E0
void RsTrackClass::Recalculate_Static_LOD_Factors(void)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) robj->Recalculate_Static_LOD_Factors();
}

// 0x461110
void RsTrackClass::Increment_LOD(void)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) robj->Increment_LOD();
}

// 0x461140
void RsTrackClass::Decrement_LOD(void)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) robj->Decrement_LOD();
}

// 0x461170
float RsTrackClass::Get_Cost(void) const
{
	RenderObjClass *robj = Peek_Render_Object();
	return(robj != NULL ? robj->Get_Cost() : 1.0e-7f);
}

// 0x4611B0
float RsTrackClass::Get_Value(void) const
{
	RenderObjClass *robj = Peek_Render_Object();
	return(robj != NULL ? robj->Get_Value() : 0.0f);
}

// 0x4611F0
float RsTrackClass::Get_Post_Increment_Value(void) const
{
	RenderObjClass *robj = Peek_Render_Object();
	return(robj != NULL ? robj->Get_Post_Increment_Value() : 0.0f);
}

// 0x461230
void RsTrackClass::Set_LOD_Level(int lod)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) robj->Set_LOD_Level(lod);
}

// 0x461270
int RsTrackClass::Get_LOD_Level(void) const
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) return(robj->Get_LOD_Level());
	return(0);
}

// 0x4612A0
int RsTrackClass::Get_LOD_Count(void) const
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) return(robj->Get_LOD_Count());
	return(0);
}

// 0x4612D0
void RsTrackClass::Set_LOD_Bias(float bias)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) robj->Set_LOD_Bias(bias);
}

// 0x461310
int RsTrackClass::Calculate_Cost_Value_Arrays(float screen_area, float *values, float *costs) const
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) return(robj->Calculate_Cost_Value_Arrays(screen_area, values, costs));
	return(0);
}

// 0x461360
void RsTrackClass::Create_Decal(DecalGeneratorClass *generator)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) robj->Create_Decal(generator);
}

// 0x4613A0
void RsTrackClass::Delete_Decal(uint32 decal_id)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) robj->Delete_Decal(decal_id);
}

// 0x4613E0
void RsTrackClass::Scale(float scale)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) robj->Scale(scale);
}

// 0x461420
void RsTrackClass::Scale(float scalex, float scaley, float scalez)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) robj->Scale(scalex, scaley, scalez);
}

// 0x461460
void RsTrackClass::On_Frame_Update(void)
{
	if (Unknown_1C8() && Peek_Render_Object() != NULL) {
		Validate_Transform();
	}
}

// 0x4614A0
void RsTrackClass::Set_User_Data(void *value, bool recursive)
{
	User_Data = value;
	if (recursive) {
		RenderStateList.Unknown_45AA40(value, true);
	}
}

// 0x4614E0
const char *RsTrackClass::Get_Name(void) const
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) return(robj->Get_Name());
	return("UNNAMED");
}

// 0x461510
const char *RsTrackClass::Get_Base_Model_Name(void) const
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) return(robj->Get_Base_Model_Name());
	return(NULL);
}

// 0x461540
void RsTrackClass::Set_Animation(HAnimClass *motion, float frame, int anim_mode)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) robj->Set_Animation(motion, frame, anim_mode);
}

// 0x461580
void RsTrackClass::Set_Transform(const Matrix3D &m)
{
	RenderObjClass::Set_Transform(m);
	Invalidate_Cached_Bounding_Volumes();
	UnknownD4 = true;
	Validate_Transform();
}

// 0x4615C0
void RsTrackClass::Set_Position(const Vector3 &v)
{
	RenderObjClass::Set_Position(v);
	Invalidate_Cached_Bounding_Volumes();
	UnknownD4 = true;
	Validate_Transform();
}

// 0x461600
void RsTrackClass::Notify_Added(SceneClass *scene)
{
	RenderObjClass::Notify_Added(scene);
	scene->Register(this, SceneClass::ON_FRAME_UPDATE);
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) robj->Notify_Added(scene);
}

// 0x461650
void RsTrackClass::Notify_Removed(SceneClass *scene)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) robj->Notify_Removed(scene);
	scene->Unregister(this, SceneClass::ON_FRAME_UPDATE);
	RenderObjClass::Notify_Removed(scene);
}





























































































































// 0x4616A0
const SphereClass &RsTrackClass::Get_Bounding_Sphere(void) const
{
	RenderObjClass *primary = Peek_Render_Object();
	FAIL_IF(!primary) {
		static SphereClass sphere;
		return(sphere);
	}
	return(primary->Get_Bounding_Sphere());
}

// 0x461740
void RsTrackClass::Get_Obj_Space_Bounding_Sphere(SphereClass &sphere) const
{
	sphere = UnknownD8;
}

// 0x461780
void RsTrackClass::Get_Obj_Space_Bounding_Box(AABoxClass &box) const
{
	box = UnknownE8;
}

// 0x4617D0
int RsTrackClass::Get_Num_Polys(void) const
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) return(robj->Get_Num_Polys());
	return(0);
}

// 0x461800
void RsTrackClass::Update_Obj_Space_Bounding_Volumes(void)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) {
		SphereClass sphere;
		robj->Get_Obj_Space_Bounding_Sphere(sphere);
		UnknownD8.Center = sphere.Center;
		UnknownD8.Radius = sphere.Radius;
		robj->Get_Obj_Space_Bounding_Box(UnknownE8);
	} else {
		UnknownD8.Center.Set(0.0f, 0.0f, 0.0f);
		UnknownD8.Radius = 0.0f;
		UnknownE8.Center.Set(0.0f, 0.0f, 0.0f);
		UnknownE8.Extent.Set(0.0f, 0.0f, 0.0f);
	}
	Invalidate_Cached_Bounding_Volumes();
	RenderObjClass *container = Get_Container();
	if (container != NULL) {
		container->Update_Obj_Space_Bounding_Volumes();
	}
}

// 0x461910
void RsTrackClass::Update_Cached_Bounding_Volumes(void) const
{
	CachedBoundingSphere.Center = Get_Transform() * UnknownD8.Center;
	CachedBoundingSphere.Radius = UnknownD8.Radius;
	CachedBoundingBox = UnknownE8;
	CachedBoundingBox.Transform(Get_Transform());
	Validate_Cached_Bounding_Volumes();
}

// 0x461A90
void RsTrackClass::Update_Sub_Object_Transforms(void)
{
	RenderObjClass::Update_Sub_Object_Transforms();
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) {
		robj->Set_Transform(Transform);
		robj->Update_Sub_Object_Transforms();
	}
	Set_Sub_Object_Transforms_Dirty(false);
}

// 0x461B00
bool RsTrackClass::Cast_Ray(RayCollisionTestClass &raytest)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) { Validate_Transform(); return(robj->Cast_Ray(raytest)); }
	return(false);
}

// 0x461B50
bool RsTrackClass::Cast_AABox(AABoxCollisionTestClass &boxtest)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) { Validate_Transform(); return(robj->Cast_AABox(boxtest)); }
	return(false);
}

// 0x461BA0
bool RsTrackClass::Cast_OBBox(OBBoxCollisionTestClass &boxtest)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) { Validate_Transform(); return(robj->Cast_OBBox(boxtest)); }
	return(false);
}

// 0x461BF0
bool RsTrackClass::Intersect_AABox(AABoxIntersectionTestClass &boxtest)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) { Validate_Transform(); return(robj->Intersect_AABox(boxtest)); }
	return(false);
}

// 0x461C40
bool RsTrackClass::Intersect_OBBox(OBBoxIntersectionTestClass &boxtest)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) { Validate_Transform(); return(robj->Intersect_OBBox(boxtest)); }
	return(false);
}

// 0x461C90
bool RsTrackClass::Intersect(IntersectionClass *Intersection, IntersectionResultClass *Final_Result)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) { Validate_Transform(); return(robj->Intersect(Intersection, Final_Result)); }
	return(false);
}

// 0x461CE0
bool RsTrackClass::Intersect_Sphere(IntersectionClass *Intersection, IntersectionResultClass *Final_Result)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) return(robj->Intersect_Sphere(Intersection, Final_Result));
	return(false);
}

// 0x461D20
bool RsTrackClass::Intersect_Sphere_Quick(IntersectionClass *Intersection, IntersectionResultClass *Final_Result)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) return(robj->Intersect_Sphere_Quick(Intersection, Final_Result));
	return(false);
}











































// 0x461D60
int RsTrackClass::Unknown_461D60(RenderObjClass *obj, const char *bone_path)
{
	if (obj != NULL && obj->Get_Container() != NULL) {
		RenderObjClass *model = Peek_Render_Object();
		if (model != NULL) {
			if (bone_path == NULL || *bone_path == '\0') {
				Remove_Sub_Object(obj);
			} else {
				RenderObjClass *robj = model;
				char path[256];
				strcpy(path, bone_path);
				char *name = strtok(path, "\\");
				while (name != NULL) {
					char *next = strtok(NULL, "\\");
					if (next == NULL) {
						robj->Remove_Sub_Object(obj);
						return(1);
					}
					int idx = robj->Get_Bone_Index(name);
					if (idx == -1) {
						return(0);
					}
					G_ASSERT(robj->Get_Num_Sub_Objects_On_Bone(idx) <= 1);
					robj = robj->Get_Sub_Object_On_Bone(0, idx);
					if (robj == NULL) {
						return(0);
					}
					robj->Release_Ref();
					name = next;
				}
			}
		}
	}
	return(0);
}

// 0x461EF0
int RsTrackClass::Remove_Sub_Object(RenderObjClass *robj)
{
	RenderObjClass *primary = Peek_Render_Object();
	if (primary != NULL) return(primary->Remove_Sub_Object(robj));
	return(0);
}

// 0x461F30
RenderObjClass *RsTrackClass::Get_Sub_Object_By_Name(const char *name) const
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) return(robj->Get_Sub_Object_By_Name(name));
	return(NULL);
}

// 0x461F70
int RsTrackClass::Get_Num_Sub_Objects_On_Bone(int boneindex) const
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) return(robj->Get_Num_Sub_Objects_On_Bone(boneindex));
	return(0);
}

// 0x461FB0
RenderObjClass *RsTrackClass::Get_Sub_Object_On_Bone(int index, int boneindex) const
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) return(robj->Get_Sub_Object_On_Bone(index, boneindex));
	return(NULL);
}

// 0x461FF0
int RsTrackClass::Get_Sub_Object_Bone_Index(RenderObjClass *subobj) const
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) return(robj->Get_Sub_Object_Bone_Index(subobj));
	return(0);
}

// 0x462030
int RsTrackClass::Remove_Sub_Objects_From_Bone(const char *bname)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) return(robj->Remove_Sub_Objects_From_Bone(bname));
	return(0);
}

// 0x462070
void RsTrackClass::Unknown_1D4(int a, int b, int c)
{
}

// 0x462090
int RsTrackClass::Get_Num_Sub_Objects(void) const
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) return(robj->Get_Num_Sub_Objects());
	return(0);
}

// 0x4620C0
RenderObjClass *RsTrackClass::Get_Sub_Object(int index) const
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) return(robj->Get_Sub_Object(index));
	return(NULL);
}

// 0x462100
int RsTrackClass::Add_Sub_Object(RenderObjClass *subobj)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) return(robj->Add_Sub_Object(subobj));
	return(0);
}

// 0x462140
int RsTrackClass::Add_Sub_Object_To_Bone(RenderObjClass *subobj, int bone_index)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) return(robj->Add_Sub_Object_To_Bone(subobj, bone_index));
	return(0);
}

// 0x462180
void RsTrackClass::Unknown_462180(HAnimComboDataClass *data)
{
	if (UnknownC4 == NULL) {
		UnknownC4 = new HAnimComboClass;
	}
	UnknownC4->Append_Anim_Combo_Data(data);
}

// 0x462220
int RsTrackClass::Get_Num_Bones(void)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) return(robj->Get_Num_Bones());
	return(0);
}

// 0x462250
const char *RsTrackClass::Get_Bone_Name(int bone_index)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) return(robj->Get_Bone_Name(bone_index));
	return(NULL);
}

// 0x462290
int RsTrackClass::Get_Bone_Index(const char *bonename)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) return(robj->Get_Bone_Index(bonename));
	return(0);
}

// 0x4622D0
const Matrix3D &RsTrackClass::Get_Bone_Transform(const char *bonename)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) return(robj->Get_Bone_Transform(bonename));
	return(Get_Transform());
}

// 0x462320
const Matrix3D &RsTrackClass::Get_Bone_Transform(int boneindex)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) return(robj->Get_Bone_Transform(boneindex));
	return(Get_Transform());
}

// 0x462370
void RsTrackClass::Capture_Bone(int bindex)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) robj->Capture_Bone(bindex);
}

// 0x4623B0
void RsTrackClass::Release_Bone(int bindex)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) robj->Release_Bone(bindex);
}

// 0x4623F0
bool RsTrackClass::Is_Bone_Captured(int bindex) const
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) return(robj->Is_Bone_Captured(bindex));
	return(false);
}

// 0x462430
void RsTrackClass::Control_Bone(int bindex, const Matrix3D &objtm, bool world_space_translation)
{
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) robj->Control_Bone(bindex, objtm);
}

// 0x462470
void RsTrackClass::Unknown_462470(int a, int b)
{
	if (UnknownC8 != NULL) {
		UnknownC8->Release_Ref();
		UnknownC8 = NULL;
	}
	UnknownC8 = new Unknown45D9E0Class(this, a, b);
}
















// 0x462530
void RsTrackClass::Unknown_462530(int a)
{
	if (UnknownCC != NULL) {
		UnknownCC->Release_Ref();
		UnknownCC = NULL;
	}
	G_ASSERT(CullingObject != 0);
	UnknownCC = new Unknown45DC80Class(this, CullingObject, a);
}


// 0x462610
void RsTrackClass::Unknown_462610()
{
	if (UnknownC8 != NULL) {
		UnknownC8->Release_Ref();
		UnknownC8 = NULL;
	}
}


// 0x462650
void RsTrackClass::Unknown_462650()
{
	if (UnknownCC != NULL) {
		UnknownCC->Release_Ref();
		UnknownCC = NULL;
	}
}


// 0x462690's CullingObject (vtable 0xAE871C: RefCountClass's Delete_This, the deleting destructor 0x463410)
class Unknown462690Class : public CullableClass	// NAME 1: neutral (D10)
{
public:
	Unknown462690Class(RsTrackClass *owner) : Owner(owner), Unknown2C(0) {}	// NAME 1 [owner]: neutral

	RsTrackClass *Owner;			// +0x28  // NAME 1: neutral (D10)
	int Unknown2C;					// +0x2C  // NAME 1: neutral (D10)
};


// 0x462690
void RsTrackClass::Unknown_462690()
{
	if (CullingObject == NULL) {
		CullingObject = new Unknown462690Class(this);
		CullingObject->Set_Cull_Box(Get_Bounding_Box());
	}
}


/*
**	Reconstruction notes
**
**	The client's window 0x45FDA0-0x463490: the functions above in address order (0x462610 / 0x462650
**	have no callers; 0x462690's callers are projscen.cpp's 0x495070-0x495870), then WW3D2's inline
**	RenderObjClass copies 0x462760-0x463340, and the header's inline copies 0x4633A0-0x463490
**	(Unknown_1C4, Class_ID, the deleting destructors, Unknown462690Class's 0x463410 / 0x463440, and
**	adjustor thunks).
**	The asserts sit on the client's lines 191, 236, 290, 429, 876 (FAIL_IF), 1077, 1286 through the
**	blank lines; 0x4604D0's body (line 236) is inlined in both constructors and in 0x4605E0.
*/
