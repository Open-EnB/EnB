// Earth & Beyond client (reconstruction): app\client\ui_mesh.cpp, the animated mesh (UIMeshClass, 0x5B8120-0x5BB240). Notes at the end.
#include "ui_proc_impl.h"
#include <stdlib.h>
#include <string.h>
#include "sclient.h"
#include "client.h"
#include "gadget.h"
#include "..\shared\rs_track.h"
#include "..\shared\g_assert.h"
#include "..\audio\soundlib.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\..\ww3d2\assetmgr.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\hanim.h"
#include "..\..\ww3d2\matinfo.h"
#include "..\..\ww3d2\vertmaterial.h"
#include "..\..\ww3d2\scene.h"

bool Unknown_495FD0(RenderObjClass *robj, TextureClass *tex, int index);

UIMeshClass::UIMeshClass(const char *name, int count) : SubjectClass(2), InterfaceMesh(NULL), Unknown2C(NULL), Unknown6C(0), Unknown70(0)
{
	Unknown_5B8850(name, count);
}










// NOT EXACT: 0x5B8330 (0.78): the client calls GenericNode() / Link out of line for the running list (inline budget) and stores the hsv zeros inside the new
UIMeshClass::UIMeshClass(const char *name, ClientClass *client) : SubjectClass(2), Unknown6C(1), Unknown70(0)
{
	G_ASSERT(client);
	RenderStateListClass *list = client->Unknown18->Unknown_764060(name);
	G_ASSERT(list);
	float hsv[3] = { 0.0f, 0.0f, 0.0f };
	RsTrackClass *track = new RsTrackClass(list, 1.0f, hsv);
	track->Unknown_4604D0(0);
	InterfaceMesh = track;


	G_ASSERT((InterfaceMesh != 0) && "Can't find mesh");
	Unknown2C = new HAnimComboClass;
	Unknown4C.Resize(Unknown6C);
	Unknown5C.Resize(Unknown6C);
	for (int i = 0; i < Unknown6C; i++) {
		Unknown4C[i] = new UIProcListClass;
		Unknown5C[i] = false;
	}
}

UIMeshClass::~UIMeshClass()
{
	Unknown_5B8850(NULL, 0);
}

// NOT EXACT: 0x5B8850 (0.79): the client's FAIL_IF path jumps to the shared epilogue, ours repeats it
void UIMeshClass::Unknown_5B8850(const char *name, int count)
{
	if (InterfaceMesh != NULL) {
		InterfaceMesh->Remove();
		InterfaceMesh->Release_Ref();
		InterfaceMesh = NULL;
		delete Unknown2C;
		Unknown2C = NULL;
		for (int i = 0; i < Unknown6C; i++) {
			for (DataNode<UIProcClass *> *node = Unknown4C[i]->First_Valid(); node != NULL; node = Unknown4C[i]->First_Valid()) {
				delete node->Get();
			}
			delete Unknown4C[i];
			Unknown4C[i] = NULL;
			Unknown5C[i] = false;
		}
		for (DataNode<UIProcClass *> *node = Unknown30.First_Valid(); node != NULL; node = Unknown30.First_Valid()) {
			delete node->Get();
		}
		Unknown6C = 0;
		Unknown70 = 0;
	}
	if (name != NULL) {
		InterfaceMesh = WW3DAssetManager::Get_Instance()->Create_Render_Obj(name);














		FAIL_IF(!InterfaceMesh) {
		} else {
			Unknown2C = new HAnimComboClass;
			Unknown6C = count;
			Unknown4C.Resize(count);
			Unknown5C.Resize(count);
			for (int i = 0; i < Unknown6C; i++) {
				Unknown4C[i] = new UIProcListClass;
				Unknown5C[i] = false;
			}
		}
	}
}

RenderObjClass *UIMeshClass::Peek_Object()
{
	return(InterfaceMesh);
}

RenderObjClass *UIMeshClass::Unknown_5B8B20()
{
	InterfaceMesh->Add_Ref();
	return(InterfaceMesh);
}

RenderObjClass *UIMeshClass::Unknown_5B8B40(const char *name)
{
	RenderObjClass *robj = InterfaceMesh->Get_Sub_Object_By_Name(name);
	if (robj == NULL) {
		char *copy = strdup(name);
		char *dot = strchr(copy, '.');
		if (dot != NULL && dot[1] != '\0') {
			*dot = '\0';
			robj = InterfaceMesh->Get_Sub_Object_By_Name(copy);
			if (robj != NULL) {
				RenderObjClass *mesh = InterfaceMesh;
				InterfaceMesh = robj;
				robj = Unknown_5B8B40(dot + 1);
				InterfaceMesh->Release_Ref();
				InterfaceMesh = mesh;
			}
		}
		free(copy);
	}
	return(robj);
}

void UIMeshClass::Unknown_5B8C10(RenderObjClass *robj, float r, float g, float b)
{
	MaterialInfoClass *info = robj->Get_Material_Info();
	for (int i = 0; i < info->Vertex_Material_Count(); i++) {
		VertexMaterialClass *vmat = info->Get_Vertex_Material(i);
		if (vmat != NULL) {
			vmat->Set_Emissive(r, g, b);
			vmat->Release_Ref();
		}
	}
	info->Release_Ref();
}

void UIMeshClass::Unknown_5B8CB0(RenderObjClass *robj)
{
	MaterialInfoClass *info = robj->Get_Material_Info();
	for (int i = 0; i < info->Vertex_Material_Count(); i++) {
		VertexMaterialClass *vmat = info->Get_Vertex_Material(i);
		if (vmat != NULL) {
			vmat->Set_Emissive(0.35f, 0.35f, 0.35f);
			vmat->Release_Ref();
		}
	}
	info->Release_Ref();
}

void UIMeshClass::Unknown_5B8D50(RenderObjClass *robj)
{
	MaterialInfoClass *info = robj->Get_Material_Info();
	for (int i = 0; i < info->Vertex_Material_Count(); i++) {
		VertexMaterialClass *vmat = info->Get_Vertex_Material(i);
		if (vmat != NULL) {
			vmat->Set_Emissive(1.0f, 1.0f, 1.0f);
			vmat->Release_Ref();
		}
	}
	info->Release_Ref();
}

void UIMeshClass::Unknown_5B8DF0(int id, const char *anim_name, bool loop, int a, int b, int c, int d)
{
	UIProcClass *proc = new UIProc5DCD20Class(this, anim_name, loop, a, b, c, d);
	proc->Id = id;
	Unknown30.Add_Head(&proc->Node);
}

// NOT EXACT: 0x5B8EE0 (0.50): register choice (slot in edx, the node through lea ecx)
void UIMeshClass::Unknown_5B8EE0(int slot, int id, const char *anim_name, bool loop, int a, int b, int c, int d)
{
	UIProcClass *proc = new UIProc5DCD20Class(this, anim_name, loop, a, b, c, d);
	proc->Slot = slot;
	proc->Id = id;
	Unknown4C[slot]->Add_Tail(&proc->Node);
}

// NOT EXACT: 0x5B8FF0 (0.50): as 0x5B8EE0
void UIMeshClass::Unknown_5B8FF0(int slot, int id, const char *morph_anim, bool loop, int a, int b, int c, int d)
{
	UIProcClass *proc = new UIProc5DCD20Class(this, (HAnimClass *)morph_anim, loop, a, b, c, d);
	proc->Slot = slot;
	proc->Id = id;
	Unknown4C[slot]->Add_Tail(&proc->Node);
}

void UIMeshClass::Unknown_5B9100(const char *bone_name, RenderObjClass *object)
{
	InterfaceMesh->Add_Sub_Object_To_Bone(object, bone_name);
}

void UIMeshClass::Unknown_5B9130(const char *bone_name, UIMeshClass *child)
{
	InterfaceMesh->Add_Sub_Object_To_Bone(child->InterfaceMesh, bone_name);
}

int UIMeshClass::Unknown_5B9160(const char *bone_name)
{
	return(InterfaceMesh->Get_Bone_Index(bone_name));
}

Matrix3D UIMeshClass::Unknown_5B9190(int bone)
{
	return(InterfaceMesh->Get_Bone_Transform(bone));
}

Matrix3D UIMeshClass::Unknown_5B9210(const char *bone_name)
{
	return(InterfaceMesh->Get_Bone_Transform(bone_name));
}

void UIMeshClass::Unknown_5B9290(SimpleSceneClass *scene)
{
	InterfaceMesh->Add(scene);
}

void UIMeshClass::Unknown_5B92B0()
{
	InterfaceMesh->Remove();
}

UIProcClass *UIMeshClass::Unknown_5B92D0(int slot)
{
	if (slot == -1 || Unknown5C[slot]) {
		return(NULL);
	}
	DataNode<UIProcClass *> *node;
	while ((node = Unknown4C[slot]->First_Valid()) != NULL) {
		UIProcClass *proc = node->Get();
		if (proc->Unknown_04()) {
			Unknown5C[slot] = true;
			node->Unlink();
			Unknown30.Add_Head(node);
			return(proc);
		}
		delete proc;
	}
	return(NULL);
}

void UIMeshClass::Unknown_00()
{
	Unknown_5B9410();
}

// NOT EXACT: 0x5B9410 (0.82): the client keeps the loop's bool (setne al; test al, al) where ours tests proc directly
void UIMeshClass::Unknown_5B9410()
{
	for (int i = 0; i < Unknown6C; i++) {
		Unknown_5B92D0(i);
	}
	float time = WW3D::Get_Frame_Time() * 0.001f;
	if (Unknown2C->Get_Num_Anims() != 0) {
		Unknown2C->Reset();
	}
	DataNode<UIProcClass *> *node = Unknown30.First_Valid();
	while (node != NULL) {
		DataNode<UIProcClass *> *next = node->Next_Valid();
		UIProcClass *proc = node->Get();
		bool running = true;
		while (running) {
			int slot = proc->Slot;
			if (proc->Unknown_08(time)) {
				break;
			}
			proc->Unknown_0C();
			delete proc;
			running = ((proc = Unknown_5B92D0(slot)) != NULL);
		}
		node = next;
	}
	if (Unknown2C->Get_Num_Anims() != 0) {
		Unknown2C->Normalize_Weights();
		InterfaceMesh->Set_Animation(Unknown2C);
	} else {
		InterfaceMesh->Set_Animation();
	}
}

void UIMeshClass::Unknown_5B9550(int id)
{
	DataNode<UIProcClass *> *node = Unknown30.First_Valid();
	while (node != NULL) {
		DataNode<UIProcClass *> *next = node->Next_Valid();
		UIProcClass *proc = node->Get();
		if (proc->Id == id) {
			if (proc->Slot != -1) {
				Unknown5C[proc->Slot] = false;
			}
			proc->Unknown_0C();
			delete proc;
		}
		node = next;
	}
}

void UIMeshClass::Unknown_5B95E0(int slot, int arg)
{
	UIProcClass *proc = new UIProc5DD7E0Class(this, arg);
	proc->Slot = slot;
	Unknown4C[slot]->Add_Tail(&proc->Node);
}

void UIMeshClass::Unknown_5B9830()
{
	DataNode<UIProcClass *> *node = Unknown30.First_Valid();
	while (node != NULL) {
		DataNode<UIProcClass *> *next = node->Next_Valid();
		UIProcClass *proc = node->Get();
		if (proc->Slot != -1) {
			Unknown5C[proc->Slot] = false;
		}
		proc->Unknown_0C();
		delete proc;
		node = next;
	}
}

void UIMeshClass::Unknown_5B98B0(int slot, int id)
{
	DataNode<UIProcClass *> *node = Unknown4C[slot]->First_Valid();
	while (node != NULL) {
		DataNode<UIProcClass *> *next = node->Next_Valid();
		UIProcClass *proc = node->Get();
		if (id == -1 || proc->Id == id) {
			delete proc;
		}
		node = next;
	}
}

void UIMeshClass::Unknown_5B9930(int id)
{
	for (int i = 0; i < Unknown6C; i++) {
		Unknown_5B98B0(i, id);
	}
}

void UIMeshClass::Unknown_5B99C0(GadgetClass *gadget)
{
	Gadgets.Add_Head(gadget);
}

void UIMeshClass::Unknown_5B99F0(GadgetClass *gadget)
{
	Gadgets.Remove(gadget);
}

void UIMeshClass::Unknown_5B9A20(int id, bool on)
{
	for (SLNode<GadgetClass> *n = Gadgets.Head(); n != NULL; n = n->Next()) {
		if (n->Data()->Get_Id() == id) {
			n->Data()->Unknown_40(!on);
		}
	}
}

void UIMeshClass::Unknown_5B9A70(int slot, int arg, bool on)
{
	UIProcClass *proc = new UIProc5DD770Class(this, arg, on);
	proc->Slot = slot;
	Unknown4C[slot]->Add_Tail(&proc->Node);
}

void UIMeshClass::Unknown_5B9BC0(int slot, const char *name, int arg)
{
	UIProcClass *proc = new UIProc5DD830Class(this, name, arg);
	proc->Slot = slot;
	Unknown4C[slot]->Add_Tail(&proc->Node);
}















































void UIMeshClass::Unknown_5B9D80(const char *name, TextureClass *texture)
{
	RenderObjClass *robj = Unknown_5B8B40(name);
	G_ASSERT(robj);
	Unknown_495FD0(robj, texture, 0);
	robj->Release_Ref();
}

void UIMeshClass::Unknown_5B9DF0(int slot, const char *name, bool on)
{
	UIProcClass *proc = new UIProc5DD8A0Class(this, name, on);
	proc->Slot = slot;
	Unknown4C[slot]->Add_Tail(&proc->Node);
}

void UIMeshClass::Unknown_5B9FB0(const char *name, bool hide)
{
	RenderObjClass *robj = Unknown_5B8B40(name);
	G_ASSERT(robj);
	if (robj != NULL) {
		robj->Set_Hidden(hide);
		robj->Release_Ref();
	}
}


bool UIMeshClass::Unknown_5BA030(const char *name)
{
	RenderObjClass *robj = Unknown_5B8B40(name);
	G_ASSERT(robj);
	if (robj == NULL) {
		return(true);
	}
	bool hidden = (robj->Is_Hidden() != 0);
	robj->Release_Ref();
	return(hidden);
}

void UIMeshClass::Unknown_5BA0B0(int slot, const char *name)
{
	UIProcClass *proc = new UIProc5DD910Class(this, name);
	proc->Slot = slot;
	Unknown4C[slot]->Add_Tail(&proc->Node);
}

void UIMeshClass::Unknown_5BA270(const char *name)
{
	SoundLibraryClass::Play_2D_Sound(name, 2);
}

bool UIMeshClass::Unknown_5BA2A0(int slot)
{
	if (Unknown5C[slot]) {
		return(false);
	}
	if (Unknown4C[slot]->First_Valid() != NULL) {
		return(false);
	}
	for (DataNode<UIProcClass *> *node = Unknown30.First_Valid(); node != NULL; node = node->Next_Valid()) {
		if (node->Get()->Slot == slot) {
			return(false);
		}
	}
	return(true);
}

bool UIMeshClass::Unknown_5BA320(int index, bool reverse)
{
	HAnimComboDataClass *data = Unknown2C->Peek_Anim_Combo_Data(index);
	if (data == NULL) {
		return(true);
	}
	if (reverse) {
		if (data->Get_Frame() <= 0.0f) {
			return(true);
		}
	} else {
		if (data->Get_Frame() >= (float)(data->Peek_HAnim()->Get_Num_Frames() - 1)) {
			return(true);
		}
	}
	return(false);
}
















int UIMeshClass::Unknown_5BA3B0(int slot)
{
	if (Unknown5C[slot]) {
		for (DataNode<UIProcClass *> *node = Unknown30.First_Valid(); node != NULL; node = node->Next_Valid()) {
			UIProcClass *proc = node->Get();
			if (proc->Slot == slot) {
				return(proc->Id);
			}
		}
		G_ASSERT(false);
	}
	return(-1);
}

void UIMeshClass::Unknown_5BA430(float scale)
{
	InterfaceMesh->Scale(scale);
}

void UIMeshClass::Unknown_5BA460(int arg)
{
}

// Reconstruction notes (UM1, 2026-11-07)
// - The object: after ui_manu_util.cpp's <string> initialiser 0x5B80D0 and the two adjustor thunks 0x5B8100 / 0x5B8110 (into
//   0x5B7B70, ui_manu_util's), up to its own initialiser 0x5BB220; Unknown5BB320Class 0x5BB250 follows.
// - UIMeshClass derives from observer.cpp's SubjectClass at +0x04: the destructor calls ~SubjectClass 0x49E950 through a
//   null-checked this + 4 (a member gave a plain call and a different frame).
// - 0x5BA480-0x5BB1B0 are the wwlib templates this file instantiates (SList<GadgetClass>, DataNode / List of UIProcClass,
//   VectorClass<UIProcListClass *> and VectorClass<bool>), emitted with the tables 0xAF31E0-0xAF3274.
// - UIProcClass's constructor sets Node's value in its body: in the initialiser list the store moved ahead of UIMesh's.
// - 0x5B92D0 is exact only with the node assigned in the while condition (a for loop gave this and slot swapped registers).
