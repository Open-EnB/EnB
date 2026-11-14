// Earth & Beyond client (reconstruction): app\client\influence.cpp (notes at the end of the file)
#include "influence.h"
#include "sclient.h"
#include "co.h"
#include "hlod.h"
#include "collect.h"
#include "meshmdl.h"
#include "matinfo.h"
#include "vertmaterial.h"
#include "scene.h"
#include "ini.h"
#include "nstrdup.h"
#include <stdio.h>
#include <string.h>
#include "..\shared\g_assert.h"


Unknown86CRecordClass::Unknown86CRecordClass(ClientClass *client, unsigned id) :
	Client(client),
	ID(id)
{
	Node.Set(this);
}


void Unknown86CRecordClass::Unknown_04(int)
{
}


UnknownObjectInfluenceClass *Unknown86CRecordClass::Unknown_0C()
{
	return(NULL);
}


Unknown86CRecordClass::~Unknown86CRecordClass()
{
}


UnknownObjectInfluenceClass::UnknownObjectInfluenceClass(ClientClass *client, unsigned id) :
	Unknown86CRecordClass(client, id),
	Object(NULL),
	ObjectNode(NULL),
	Sphere(Vector3(0.0f, 0.0f, 0.0f), 0.0f),
	SubObjectName(NULL)
{
}


UnknownObjectInfluenceClass *UnknownObjectInfluenceClass::Unknown_0C()
{
	return(this);
}


UnknownObjectInfluenceClass::~UnknownObjectInfluenceClass()
{
	if (SubObjectName) {
		delete [] SubObjectName;
		SubObjectName = NULL;
	}
}


bool UnknownObjectInfluenceClass::Unknown_08(const Vector3 &)
{
	Update_Sphere();
	return(true);
}


void UnknownObjectInfluenceClass::Unknown_04(int arg)
{
	if ((DataNode<UnknownAF0DD0Class *> *)arg == &ObjectNode) {
		Object = NULL;
	}
}


void UnknownObjectInfluenceClass::Unknown_10(int game_id)
{
	Object = Client->Find_Object(game_id);
	Unknown_73FDB0(&ObjectNode, Object);
	Update_Sphere();
}


void UnknownObjectInfluenceClass::Set_Sphere(const SphereClass &sphere)
{
	Sphere = sphere;
}


void UnknownObjectInfluenceClass::Set_Sub_Object_Name(const char *name)
{
	if (SubObjectName) {
		delete [] SubObjectName;
		SubObjectName = NULL;
	}
	SubObjectName = nstrdup(name);
	Update_Sphere();
}


void UnknownObjectInfluenceClass::Update_Sphere()
{
	if (Object) {
		RenderObjClass *model = Object->Unknown_6F3BC0();
		if (SubObjectName) {
			RenderObjClass *sub = model->Get_Sub_Object_By_Name(SubObjectName);
			if (sub) {
				sub->Validate_Transform();
				model->Release_Ref();
				model = sub;
			}
		}
		Sphere = model->Get_Bounding_Sphere();
		model->Release_Ref();
	}
}


// NOT EXACT (0.68, attempts.py 0x7E87C0): inline budget: the client calls DataNode<UnknownAF0DD0Class *>(T) 0x7E9910 out of line inside the inlined base constructor, ours inlines it; frame 0x34C against 0x340 (the client's three 256-byte buffers at function scope and a hsv temporary sharing the base's Vector3 slot reach 0.76 but push SphereClass::Init's copy and 0x7E9910 out: kept this form)
NebulaClass::NebulaClass(ClientClass *client, unsigned id, RenderObjClass *model) :
	UnknownObjectInfluenceClass(client, id),
	CurrentRoom(NULL),
	DefaultRoom(NULL),
	Initialized(false),
	SkyBox(NULL),
	TempRoomPtrs(NULL)
{
	RoomCount = model->Get_Num_Bones();
	if (RoomCount && model->Class_ID() == RenderObjClass::CLASSID_HLOD) {
		HLodClass *hlod = (HLodClass *)model;
		TempRoomPtrs = new RoomClass *[RoomCount];
		for (int i = 0; i < RoomCount; i++) {
			TempRoomPtrs[i] = new RoomClass;
			TempRoomPtrs[i]->Model = new CollectionClass;
			char name[228];
			sprintf(name, "Room%02d", i);
			TempRoomPtrs[i]->Model->Set_Name(name);
		}


























































		G_ASSERT(hlod->Get_Lod_Count() == 1);
		int count = hlod->Get_Lod_Model_Count(0);
		const char *htree_name = hlod->Get_HTree()->Get_Name();
		char root_name[256];
		sprintf(root_name, "%s.%s", htree_name, "NEBULA_ROOT");
		for (int j = 0; j < count; j++) {
			int bidx = hlod->Get_Lod_Model_Bone(0, j);
			RenderObjClass *robj = hlod->Peek_Lod_Model(0, j)->Clone();
			TempRoomPtrs[bidx]->Model->Add_Sub_Object(robj);
			char bone_name[256];
			sprintf(bone_name, "%s.%s", htree_name, hlod->Get_Bone_Name(bidx));
			const char *robj_name = robj->Get_Name();
			if (stricmp(robj_name, bone_name) == 0 || stricmp(robj_name, root_name) == 0) {
				if (robj->Class_ID() == RenderObjClass::CLASSID_MESH) {
					G_ASSERT(TempRoomPtrs[bidx]->BoundaryMesh == NULL);
					TempRoomPtrs[bidx]->BoundaryMesh = (MeshClass *)robj;
					TempRoomPtrs[bidx]->BoundaryMesh->Add_Ref();
				} else {
					G_ASSERT(0);
				}
			}
			robj->Release_Ref();
		}
		int extra = hlod->Get_Additional_Model_Count();
		for (int k = 0; k < extra; k++) {
			int bidx = hlod->Get_Additional_Model_Bone(k);
			RenderObjClass *robj = hlod->Peek_Additional_Model(k)->Clone();
			if (TempRoomPtrs[bidx]->Model->Class_ID() != RenderObjClass::CLASSID_HLOD) {
				RenderObjClass *old = TempRoomPtrs[bidx]->Model;
				TempRoomPtrs[bidx]->Model = new HLodClass(old->Get_Name(), &old, 1);
				REF_PTR_RELEASE(old);
			}
			TempRoomPtrs[bidx]->Model->Add_Sub_Object(robj);
			robj->Release_Ref();
		}
		for (int r = 0; r < RoomCount; r++) {
			const Matrix3D &tm = hlod->Get_Bone_Transform(r);
			TempRoomPtrs[r]->Model->Set_Transform(tm);
			TempRoomPtrs[r]->BoundaryMesh->Set_Transform(tm);
			TempRoomPtrs[r]->Set_Cull_Box(TempRoomPtrs[r]->BoundaryMesh->Get_Bounding_Box());
			RoomTree.Add_Object(TempRoomPtrs[r]);
		}
		DefaultRoom = TempRoomPtrs[0];
		DefaultRoom->Add_Ref();
		DefaultRoom->Inverted = true;
		RoomTree.Re_Partition();
	} else {































		G_ASSERT(0);
	}
	float hsv[3] = { 0.0f, 0.0f, 0.0f };
	SkyBox = ClientObjectClass::Unknown_6F5140(0x137, client, 1.0f, hsv);
	INIClass ini;
	ini.Load("nebula.ini");
	int red = ini.Get_Int("Room", "FogColorR", 0);
	int green = ini.Get_Int("Room", "FogColorG", 0);
	int blue = ini.Get_Int("Room", "FogColorB", 0);
	RoomFogColor = Vector3(red * (1.0f / 255.0f), green * (1.0f / 255.0f), blue * (1.0f / 255.0f));
	RoomFogNear = ini.Get_Float("Room", "FogNearRange", 0.0f);
	RoomFogFar = ini.Get_Float("Room", "FogFarRange", 1.0f);
	red = ini.Get_Int("Between", "SkyBoxColorR", 0);
	green = ini.Get_Int("Between", "SkyBoxColorG", 0);
	blue = ini.Get_Int("Between", "SkyBoxColorB", 0);
	Vector3 sky_color(red * (1.0f / 255.0f), green * (1.0f / 255.0f), blue * (1.0f / 255.0f));
	red = ini.Get_Int("Between", "FogColorR", 0);
	green = ini.Get_Int("Between", "FogColorG", 0);
	blue = ini.Get_Int("Between", "FogColorB", 0);
	BetweenFogColor = Vector3(red * (1.0f / 255.0f), green * (1.0f / 255.0f), blue * (1.0f / 255.0f));
	BetweenFogNear = ini.Get_Float("Between", "FogNearRange", 0.0f);
	BetweenFogFar = ini.Get_Float("Between", "FogFarRange", 1.0f);
	MeshClass *mesh = (MeshClass *)SkyBox->Get_Sub_Object(0);
	mesh->Get_Material_Info()->Get_Vertex_Material(0)->Set_Emissive(sky_color);
	if (mesh->Class_ID() == RenderObjClass::CLASSID_MESH) {
		MeshModelClass *mesh_model = mesh->Get_Model();
		ShaderClass shader = mesh_model->Get_Single_Shader();
		shader.Set_Primary_Gradient(ShaderClass::GRADIENT_MODULATE);
		mesh_model->Set_Single_Shader(shader);
		mesh_model->Release_Ref();
	}
	mesh->Release_Ref();
}


NebulaClass::~NebulaClass()
{
	if (CurrentRoom) {
		CurrentRoom->Model->Remove();
		CurrentRoom->Release_Ref();
		CurrentRoom = NULL;
	}
	REF_PTR_RELEASE(DefaultRoom);
	REF_PTR_RELEASE(SkyBox);
	if (TempRoomPtrs) {
		for (int i = 0; i < RoomCount; i++) {
			REF_PTR_RELEASE(TempRoomPtrs[i]);
		}
		delete [] TempRoomPtrs;
		TempRoomPtrs = NULL;
	}
}


// NOT EXACT (0.86, attempts.py 0x7E9560): shape: the client saves ebx / ebp / esi after the Object == NULL return, ours saves ebp / esi at entry
bool NebulaClass::Unknown_08(const Vector3 &position)
{
	if (Object == NULL) {
		if (CurrentRoom) {
			CurrentRoom->Model->Remove();
		}
		return(false);
	}
	if (!Initialized) {









		G_ASSERT(TempRoomPtrs);
		const Matrix3D &parent = Object->RenderObject->Get_Transform();
		for (int i = 0; i < RoomCount; i++) {
			Matrix3D tm = TempRoomPtrs[i]->Model->Get_Transform();
			Matrix3D result;
			Matrix3D::Multiply(parent, tm, &result);
			TempRoomPtrs[i]->Model->Set_Transform(result);
			TempRoomPtrs[i]->BoundaryMesh->Set_Transform(result);
			REF_PTR_RELEASE(TempRoomPtrs[i]);
		}
		delete [] TempRoomPtrs;
		TempRoomPtrs = NULL;
		Initialized = true;
	}
	if (CurrentRoom == NULL || !CurrentRoom->Contains(position)) {
		if (CurrentRoom) {
			CurrentRoom->Model->Remove();
		}
		RoomTree.Reset_Collection();
		RoomTree.Collect_Objects(position);
		RoomClass *found = NULL;
		RoomClass *room = RoomTree.Peek_First_Collected_Object();
		if (room != NULL) {
			while (room) {
				if (room->Contains(position)) {
					found = room;
					break;
				}
				room = RoomTree.Peek_Next_Collected_Object(room);
			}
		} else {
			found = DefaultRoom;
		}
		if (found) {
			found->Model->Add(Object->RenderObject->Peek_Scene());
			found->Add_Ref();
		}
		REF_PTR_RELEASE(CurrentRoom);
		CurrentRoom = found;
	}
	if (CurrentRoom != DefaultRoom) {
		MainViewClass *view = ((SClient *)Client->As_Space_Client())->MainView;
		view->Unknown_513600((int)SkyBox);
		view->Unknown130 = false;
		SimpleSceneClass *scene = view->Unknown_64();
		scene->Set_Fog_Enable(true);
		if (CurrentRoom) {
			scene->Set_Fog_Range(RoomFogNear, RoomFogFar);
			scene->Set_Fog_Color(RoomFogColor);
			view->Unknown_513CD0(1);
			return(true);
		}
		scene->Set_Fog_Range(BetweenFogNear, BetweenFogFar);
		scene->Set_Fog_Color(BetweenFogColor);
		view->Unknown_513CD0(2);
	}
	return(true);
}

/*
**	Notes (reconstruction)
**
**	The client's object (0x7E81B0-0x7E9B20, __FILE__ 'e:\build\beta\G\app\Client\influence.cpp',
**	asserts on lines 203, 217, 221, 281 and 354): the base of ClientClass's third list (client.h's
**	Unknown86CRecordClass, vtable 0xB07730), the influence that follows an object (vtable 0xB07748),
**	the nebula (vtable 0xB07778) with its rooms (vtable 0xB0776C) in a TypedAABTreeCullSystemClass
**	(vtable 0xB0779C), then the template copies this object emits: HashNodeClass<Unknown86CRecordClass *>
**	0x7E9940 / 0x7E99D0 / 0x7E9A00, DataNode<UnknownAF0DD0Class *> 0x7E9910, the tree's Add_Object /
**	Remove_Object 0x7E9980 / 0x7E99B0 and ProxyClass's destructors 0x7E9A50 / 0x7E9AD0 (collect.h).
**	The base's slot +0x08 and UnknownAF0DD0Class's slot +0x04 are the client's pure-virtual function
**	0x9CCD10 (580 vtable references).
*/
