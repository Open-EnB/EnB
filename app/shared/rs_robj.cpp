// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\rs_robj.cpp (its asserts, lines 92-416); client
**	0x45E550-0x45F343. Notes at the end.
**
**	The render-object state (Unknown45E850Class, declared in rs_track.h for now): a render state that
**	shows a model, made by name from the asset manager or handed in, and moves on to the next state.
*/
#include <string.h>
#include "rs_track.h"
#include "assetmgr.h"
#include "mesh.h"
#include "meshmdl.h"
#include "nullrobj.h"
#include "..\..\wwlib\nstrdup.h"
#include "..\shared\unknown_debug.h"

#define RSTRACK_CLASSID	0x10001	// NAME 2: RsTrackClass::Class_ID

// E&B's asset manager (WW3DAssetManager::Get_Instance()): its first own slot makes a model at a scale and HSV shift
#include "enbassetmgr.h"





// the copy constructor's read of the source's time (its fld / fstp)
static inline float Unknown_Duration(const Unknown45E850Class &state) { return(state.Unknown34); }	// NAME 1 [Unknown_Duration, state]: neutral (D10)

// 0x45E550: marks the meshes of a model and its sub-objects
static void Unknown_45E550(RenderObjClass *robj)	// NAME 1 [Unknown_45E550, robj]: neutral (D10)
{
	if (robj->Class_ID() == RenderObjClass::CLASSID_MESH) {
		MeshModelClass *model = ((MeshClass *)robj)->Get_Model();
		if (model != NULL) {
			model->Set_Flag(MeshGeometryClass::CAST_SHADOW, true);
			model->Release_Ref();
		}
	}
	int count = robj->Get_Num_Sub_Objects();
	while (count--) {
		RenderObjClass *sub = robj->Get_Sub_Object(count);
		Unknown_45E550(sub);
		sub->Release_Ref();
	}
}





























// 0x45E5F0 (deleting destructor 0x45E850)
Unknown45E850Class::Unknown45E850Class(RenderStateListClass *list, int id, int link_id, unsigned int flags, const char *name, int next_id, float duration) :
	RenderStateClass(list, id, link_id, flags),
	RenderObjName(NULL),
	RenderObj(NULL),
	Unknown34(duration),
	Unknown38(0),
	NextID(next_id),
	NextRenderState(NULL)
{
	RenderObjName = nstrdup(name);
	if (Test_Flag(8)) {
		if (RenderObjName != NULL) {
			RenderObj = WW3DAssetManager::Get_Instance()->Create_Render_Obj(RenderObjName);
			if (RenderObj == NULL) {
				G_ASSERT(0);
			} else {
				if (RenderObj->Class_ID() == RSTRACK_CLASSID) {
					((RsTrackClass *)RenderObj)->Unknown_1C4((int)Unknown24);
				}
				if (Test_Flag(11)) {
					WW3DAssetManager::Get_Instance()->Remove_Prototype(RenderObjName);
				}
				Flags |= (1 << RS_DATA_PRESENT);
				Unknown_45E550(RenderObj);
			}
		}












	} else {
		G_ASSERT(RenderObjName);
		RenderObj = NULL;
	}






	G_ASSERT(((RenderObj == 0) && !Test_Flag(RS_DATA_PRESENT) && RenderObjName !=0 && strlen(RenderObjName) > 0) || ((RenderObj != 0) && Test_Flag(RS_DATA_PRESENT)));
}









// 0x45E880
Unknown45E850Class::Unknown45E850Class(RenderStateListClass *list, int id, int link_id, unsigned int flags, RenderObjClass *robj, int next_id, float duration) :
	RenderStateClass(list, id, link_id, flags),
	RenderObjName(NULL),
	RenderObj(NULL),
	Unknown34(duration),
	Unknown38(0),
	NextID(next_id),
	NextRenderState(NULL)
{
	G_ASSERT(RenderObj != 0);
	if (Test_Flag(8) && !Test_Flag(10)) {
		RenderObj = robj->Clone();
		if (RenderObj->Class_ID() == RSTRACK_CLASSID) {
			((RsTrackClass *)RenderObj)->Unknown_1C4((int)Unknown24);
		}









	} else {
		RenderObj = robj;
		G_ASSERT(RenderObj);
		RenderObj->Add_Ref();
	}

	G_ASSERT(RenderObj != 0);
	if (RenderObj != 0) {
		Flags |= (1 << RS_DATA_PRESENT);
	}
}
// 0x45E9F0
Unknown45E850Class::Unknown45E850Class(RenderStateListClass *list, const Unknown45E850Class &source) :
	RenderStateClass(list, source),
	RenderObjName(NULL),
	RenderObj(NULL),
	Unknown34(Unknown_Duration(source)),
	Unknown38(0),
	NextID(source.NextID),
	NextRenderState(NULL)
{
	RenderObjName = nstrdup(source.RenderObjName);
	if (!Test_Flag(8) || Test_Flag(10)) {
		RenderObj = ((Unknown45E850Class &)source).Peek_Render_Object();
		if (RenderObj != NULL) {
			RenderObj->Add_Ref();
			Flags |= (1 << RS_DATA_PRESENT);
		}
	} else {
		RenderObjClass *robj = ((Unknown45E850Class &)source).Peek_Render_Object();
		if (robj != NULL) {
			RenderObj = robj->Clone();
			if (RenderObj->Class_ID() == RSTRACK_CLASSID) {
				((RsTrackClass *)RenderObj)->Unknown_1C4((int)Unknown24);
			}
			Flags |= (1 << RS_DATA_PRESENT);
		} else {
			RenderObj = NULL;
		}
	}
















	G_ASSERT(((RenderObj == 0) && !Test_Flag(RS_DATA_PRESENT) && RenderObjName !=0 && strlen(RenderObjName) > 0) || ((RenderObj != 0) && Test_Flag(RS_DATA_PRESENT)));
}

// 0x45EB70: the copy into another list
RenderStateClass *Unknown45E850Class::Unknown_04(RenderStateListClass *list)
{
	return(new Unknown45E850Class(list, *this));
}

// 0x45EC00
Unknown45E850Class::~Unknown45E850Class()
{
	if (RenderObj != NULL) {
		RenderObj->Release_Ref();
	}
	if (RenderObjName != NULL) {
		delete RenderObjName;
	}
}

// 0x45EC90
void Unknown45E850Class::Unknown_24(void *value, bool recursive)
{
	if (Test_Flag(8) && RenderObj != NULL) {
		RenderObj->Set_User_Data(value, recursive);
	}
}

// 0x45ECD0: the time, and on to the next state when it is up
void Unknown45E850Class::Unknown_30(RsTrackClass *owner, float seconds)
{
	seconds += Unknown38;
	Unknown38 = seconds;
	if ((Flags & 7) && seconds > Unknown34) {
		seconds -= Unknown34;
		Unknown_4584B0(owner, seconds, Get_Next_Render_State());
	}
}


// 0x45ED70
RenderStateClass *Unknown45E850Class::Get_Next_Render_State()
{
	if (NextRenderState == NULL) {
		RenderStateListClass *list = List;
		NextRenderState = list->Unknown_45A7E0(NextID);
		G_ASSERT(NextRenderState);
	}
	return(NextRenderState);
}








// 0x45EDC0: the model (loaded; a copy of its own unless flag 8 or 10)
RenderObjClass *Unknown45E850Class::Peek_Render_Object()
{
	Unknown_10();
	if (!Test_Flag(8) && !Test_Flag(10)) {
		G_ASSERT(RenderObj);
		if (RenderObj == NULL) {
			return(NULL);
		}
		RenderObjClass *robj = RenderObj;
		RenderObj = robj->Clone();
		robj->Release_Ref();
		Unknown_458370(8);
	}
	return(RenderObj);
}

// 0x45EE50: activation: the model shown, its animation reset, the owner switched to it
void Unknown45E850Class::Unknown_14(RsTrackClass *owner)
{
	Unknown_457E10();
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) {
		robj->Set_Hidden(false);
		robj->Set_Animation();
	}
	owner->Unknown_1CC(this, robj);
	owner->Unknown_460BC0();
	Unknown38 = 0;
}

// 0x45EED0: deactivation: the model hidden
void Unknown45E850Class::Unknown_18(RsTrackClass *owner)
{
	Unknown_457E90();
	RenderObjClass *robj = Peek_Render_Object();
	if (robj != NULL) {
		robj->Set_Hidden(true);
	}
}

// 0x45EF10: drops a model it can make again (from the source list, or by name from its file)
void Unknown45E850Class::Unknown_0C()
{
	if (Test_Flag(RS_DATA_PRESENT) && RenderObj != NULL && (List->Source != NULL || (Get_Source_Filename() != NULL && RenderObjName != NULL))) {
		RenderObj->Release_Ref();
		RenderObj = NULL;
		Flags &= ~(1 << RS_DATA_PRESENT);
	}
}





















// 0x45EF80: loads the model (the source list's state's, or by name, loading its file when needed)
bool Unknown45E850Class::Unknown_10()
{
	if (Test_Flag(RS_DATA_PRESENT)) {
		return(true);
	}
	RenderStateListClass *list = List;
	if (list->Source != NULL) {
		RenderStateClass *state = list->Source->Unknown_45A7E0(ID);
		G_ASSERT(state);
		RenderObj = state->Peek_Render_Object();
		G_ASSERT(RenderObj);
		Unknown_4583C0(8);
		return(true);
	}


	G_ASSERT(RenderObj == 0);




	G_ASSERT(RenderObjName);
	RenderObj = WW3DAssetManager::Get_Instance()->Create_Render_Obj(RenderObjName);
	if (RenderObj == NULL) {


		G_ASSERT(Get_Source_Filename());
		if (Get_Source_Filename() != NULL) {
			WW3DAssetManager::Get_Instance()->Load_3D_Assets(Get_Source_Filename());
			RenderObj = WW3DAssetManager::Get_Instance()->Create_Render_Obj(RenderObjName);
		}









		FAIL_IF(RenderObj == 0) {
			RenderObj = new Null3DObjClass("NULL");
		}
	}
	if (RenderObj->Class_ID() == RSTRACK_CLASSID) {
		((RsTrackClass *)RenderObj)->Unknown_1C4((int)Unknown24);
	}
	if (Test_Flag(11)) {
		WW3DAssetManager::Get_Instance()->Remove_Prototype(RenderObjName);
	}
	Unknown_458370(RS_DATA_PRESENT);
	Unknown_45E550(RenderObj);
	return(true);
}










// 0x45F280: the model made again at a scale and HSV shift
void Unknown45E850Class::Unknown_40(float scale, float *hsv)
{
	RenderObjClass *newrobj = ((UnknownBDADC0Class *)WW3DAssetManager::Get_Instance())->Unknown_78(RenderObjName, scale, hsv);
	FAIL_IF(!newrobj) {
		newrobj = new Null3DObjClass("NULL");
	}
	REF_PTR_SET(RenderObj, newrobj);
	REF_PTR_RELEASE(newrobj);
}
/*
**	Reconstruction notes
**
**	client.exe 0x45E550-0x45F343, appfile.py match 15 of 15 exact (and the deleting destructor 0x45E850):
**	the mesh marker 0x45E550 (inlined once into 0x45E5F0 / 0x45EF80, the rest recursive), the constructors
**	by name 0x45E5F0 / by render object 0x45E880 / copy 0x45E9F0 (its time read through a file-static
**	inline accessor for the client's fld / fstp), the clone 0x45EB70, the destructor 0x45EC00 and the
**	vtable 0xAE8138's own slots +0x0C 0x45EF10, +0x10 0x45EF80, +0x14 0x45EE50, +0x18 0x45EED0, +0x20
**	0x45EDC0, +0x24 0x45EC90, +0x30 0x45ECD0, +0x40 0x45F280; 0x45ED70 (no caller) is inlined into 0x45ECD0.
**	0x45E880 asserts RenderObj != 0 right after its member list zeroes it: the client's call is unconditional.
**	0x45EF80 and 0x45ED70 read List into a local: the client's registers (List->Source / List->... directly
**	give ecx for List and other registers in 0x45ED70's inlined copy in 0x45ECD0).
**	The flags it tests: 8 (own copy made: Peek_Render_Object clones the model once when 8 and 10 are clear;
**	the constructors load by name only with 8), 9 RS_DATA_PRESENT, 10 (share the model), 11 (drop the
**	asset manager's prototype after loading).
*/
