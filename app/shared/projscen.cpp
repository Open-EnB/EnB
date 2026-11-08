// Earth & Beyond client (reconstruction): notes at the end of the file
#include "projscen.h"
#include "rs_track.h"
#include "camera.h"
#include "argv.h"
#include "..\shared\g_assert.h"

// @0x494EE0
ProjSceneClass::ProjSceneClass(void) :
	ProjectorManager(NULL),
	ProjectionDef(NULL),
	StaticCullingSystem(NULL),
	DynamicCullingSystem(NULL)
{
}

// @0x494F50
ProjSceneClass::~ProjSceneClass(void)
{
	ProjSceneClass::Remove_All_Render_Objects();
	Delete_Projector_Manager();
	Delete_Static_Culling_System();
	Delete_Dynamic_Culling_System();
}

// @0x495070
void ProjSceneClass::Add_Render_Object(RenderObjClass * obj)
{
	Create_Projectors(obj);
	SimpleSceneClass::Add_Render_Object(obj);

	if ((obj->Class_ID() == 0x10001) && ((StaticCullingSystem != NULL) || (DynamicCullingSystem != NULL))) {
		RsTrackClass * track = (RsTrackClass *)obj;
		if (track->Unknown8C & 0x08) {
			CullableClass * cull = track->CullingObject;
			if (cull == NULL) {
				track->Unknown_462690();
				cull = track->CullingObject;















































				G_ASSERT(cull);
			}
			if ((track->Unknown8C & 0x10) == 0x10) {
				if (StaticCullingSystem != NULL) {
					StaticCullingSystem->Add_Object((ProjectorReceiverCullClass *)cull);
				}
			} else {
				if (DynamicCullingSystem != NULL) {
					DynamicCullingSystem->Add_Object((ProjectorReceiverCullClass *)cull);
				}
			}
		}
	}
}

// @0x495160
void ProjSceneClass::Remove_Render_Object(RenderObjClass * obj)
{
	SimpleSceneClass::Remove_Render_Object(obj);

	if ((obj->Class_ID() == 0x10001) && ((StaticCullingSystem != NULL) || (DynamicCullingSystem != NULL))) {
		RsTrackClass * track = (RsTrackClass *)obj;
		CullableClass * cull = track->CullingObject;
		if (cull != NULL) {
			if ((track->Unknown8C & 0x10) == 0x10) {
				if (StaticCullingSystem != NULL) {
					StaticCullingSystem->Remove_Object((ProjectorReceiverCullClass *)cull);
				}
			} else {
				if (DynamicCullingSystem != NULL) {
					DynamicCullingSystem->Remove_Object((ProjectorReceiverCullClass *)cull);
				}
			}
		}
		Unknown45D9E0Class * caster = track->UnknownC8;
		if ((caster != NULL) && (caster->Projection != NULL)) {
			if (track->Unknown8C & 0x10) {
				caster->Manager->Remove_Static_Texture_Projector(caster->Projection);
			} else {
				caster->Manager->Remove_Dynamic_Texture_Projector(caster->Projection);
			}
		}
	}
}

// @0x495230
void ProjSceneClass::Remove_All_Render_Objects(void)
{
	RenderObjClass * obj;
	while ((obj = RenderList.Remove_Head()) != NULL) {
		Remove_Render_Object(obj);
		obj->Release_Ref();
	}
}

// @0x495280
void ProjSceneClass::Create_Projector_Manager(void)
{
	if (ProjectorManager == NULL) {
		ArgvClass args(false, true);
		if (args.Find("-NOPROJECTORS") == NULL) {
			ProjectorManager = new ProjectorManagerClass;
			ProjectionDef = new ProjectionDefClass;

			RefRenderObjListIterator it(&RenderList);
			for (it.First(); !it.Is_Done(); it.Next()) {
				Create_Projectors(it.Peek_Obj());
			}
		}
	}
}

// @0x4953E0
void ProjSceneClass::Delete_Projector_Manager(void)
{
	if (ProjectorManager != NULL) {
		delete ProjectorManager;
		ProjectorManager = NULL;
		ProjectionDef->Release_Ref();
		ProjectionDef = NULL;
	}
}

// @0x495430
void ProjSceneClass::Create_Projectors(RenderObjClass * obj)
{
	if (ProjectorManager == NULL) {
		return;
	}
	if (obj->Class_ID() == 0x10001) {
		RsTrackClass * track = (RsTrackClass *)obj;
		if (track->Unknown8C & 0x20) {
			Unknown45D9E0Class * shadowSource = track->UnknownC8;
			if (shadowSource == NULL) {
				track->Unknown_462470((int)ProjectionDef, (int)ProjectorManager);
				shadowSource = track->UnknownC8;




































				G_ASSERT(shadowSource);
			}
			if (shadowSource->Projection != NULL) {
				if (track->Unknown8C & 0x10) {
					ProjectorManager->Add_Static_Texture_Projector(shadowSource->Projection);
				} else {
					ProjectorManager->Add_Dynamic_Texture_Projector(shadowSource->Projection);
				}
			}
		}
		if ((track->Unknown8C & 0x40) && (track->UnknownCC == NULL)) {
			track->Unknown_462690();
			track->Unknown_462530((int)ProjectorManager);
			Unknown45DC80Class * projectionTarget = track->UnknownCC;









			G_ASSERT(projectionTarget);
		}
	}
}

// @0x495560
void ProjSceneClass::Create_Static_Culling_System(void)
{
	if (StaticCullingSystem == NULL) {
		StaticCullingSystem = new ReceiverAABTreeCullSystemClass;

		RefRenderObjListIterator it(&RenderList);
		for (it.First(); !it.Is_Done(); it.Next()) {
			Add_Static_Receiver(it.Peek_Obj());
		}
	}
}

// @0x495680
void ProjSceneClass::Create_Dynamic_Culling_System(const Vector3 & min,const Vector3 & max,float objdim)
{
	if (DynamicCullingSystem == NULL) {
		DynamicCullingSystem = new ReceiverGridCullSystemClass;

		RefRenderObjListIterator it(&RenderList);
		for (it.First(); !it.Is_Done(); it.Next()) {
			Add_Dynamic_Receiver(it.Peek_Obj());
		}
	}
}

#pragma auto_inline(off)
// @0x4957A0
void ProjSceneClass::Add_Static_Receiver(RenderObjClass * obj)
{



















	G_ASSERT(StaticCullingSystem);
	if ((obj != NULL) && (obj->Class_ID() == 0x10001)) {
		RsTrackClass * track = (RsTrackClass *)obj;
		if ((track->Unknown8C & 0x08) && ((track->Unknown8C & 0x10) == 0x10)) {
			CullableClass * cull = track->CullingObject;
			if (cull == NULL) {
				track->Unknown_462690();
				cull = track->CullingObject;


				G_ASSERT(cull);
			}
			StaticCullingSystem->Add_Object((ProjectorReceiverCullClass *)cull);
		}
	}
}
// @0x495870
void ProjSceneClass::Add_Dynamic_Receiver(RenderObjClass * obj)
{
	G_ASSERT(DynamicCullingSystem);
	if ((obj != NULL) && (obj->Class_ID() == 0x10001)) {
		RsTrackClass * track = (RsTrackClass *)obj;
		if ((track->Unknown8C & 0x08) && ((track->Unknown8C & 0x10) == 0)) {
			CullableClass * cull = track->CullingObject;
			if (cull == NULL) {
				track->Unknown_462690();
				cull = track->CullingObject;


				G_ASSERT(cull);
			}
			DynamicCullingSystem->Add_Object((ProjectorReceiverCullClass *)cull);
		}
	}
}

#pragma auto_inline(on)

// @0x495930
void ProjSceneClass::Delete_Static_Culling_System(void)
{
	if (StaticCullingSystem != NULL) {
		delete StaticCullingSystem;
		StaticCullingSystem = NULL;
	}
}

// @0x495960
void ProjSceneClass::Delete_Dynamic_Culling_System(void)
{
	if (DynamicCullingSystem != NULL) {
		delete DynamicCullingSystem;
		DynamicCullingSystem = NULL;
	}
}

// @0x495990
void ProjSceneClass::Apply_Projectors(CameraClass * camera)
{
	if (ProjectorManager == NULL) return;
	ProjectorReceiverListClass dynamic_receivers;
	dynamic_receivers.Is_Empty();	// a dead read: it sends this list's constructor out of line, as the client has it (inline budget)
	if (DynamicCullingSystem != NULL) {
		DynamicCullingSystem->Reset_Collection();
		DynamicCullingSystem->Collect_Objects(camera->Get_Frustum());
		for (ProjectorReceiverCullClass * obj = DynamicCullingSystem->Get_First_Collected_Object();
			obj != NULL;
			obj = DynamicCullingSystem->Get_Next_Collected_Object(obj)) {
			ProjectorReceiverClass * receiver = obj->Receiver;
			if (receiver != NULL) {
				dynamic_receivers.Add(receiver);
			}
		}
	}
	ProjectorReceiverListClass static_receivers;
	if (StaticCullingSystem != NULL) {
		StaticCullingSystem->Reset_Collection();
		StaticCullingSystem->Collect_Objects(camera->Get_Frustum());
		for (ProjectorReceiverCullClass * obj = StaticCullingSystem->Get_First_Collected_Object();
			obj != NULL;
			obj = StaticCullingSystem->Get_Next_Collected_Object(obj)) {
			ProjectorReceiverClass * receiver = obj->Receiver;
			if (receiver != NULL) {
				static_receivers.Add(receiver);
			}
		}
	}
	ProjectorManager->Apply_Projectors(*camera, dynamic_receivers, static_receivers, StaticCullingSystem, DynamicCullingSystem);
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\projscen.cpp (its asserts, lines 86-324); client
**	0x494EE0-0x495FD0 with the template and inline copies this object emits: the receiver culling systems'
**	0x495630-0x495660, 0x495750-0x495780, 0x495C80-0x495CF0, the receiver list's 0x495C50 / 0x495D10,
**	ProjectionDefClass's 0x495F80 / 0x495FB0, Get_Scene_ID 0x495F60. The next object starts at ro_funcs' 0x495FD0.
**
**	The asserts sit on the client's __LINE__ through the blank lines (D25), so the notes are here.
*/
