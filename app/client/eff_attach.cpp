// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\eff_attach.cpp (the client's __FILE__)
**
**	The attach effect (client 0x6E4750-0x6E5030): see eff_attach.h. Notes at the end.
*/
#include "eff_attach.h"
#include "co.h"
#include "rendobj.h"
#include "mapper.h"
#include "nstrdup.h"
#include "..\shared\g_assert.h"
#include <string.h>

// 0x6E4750
AttachEffectClass::AttachEffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, const RefCountPtr<RenderObjClass> &robj, const char *bone_prefix) :
	UnaryEffectClass(client, id, start_time, duration, cycle_time),
	RObj(robj),
	BonePrefix(NULL),
	BonePrefixLength(0)
{
	BonePrefix = nstrdup(bone_prefix);
	if (BonePrefix) BonePrefixLength = strlen(BonePrefix);
	else BonePrefixLength = 0;
}

// 0x6E4A30
AttachEffectClass::~AttachEffectClass()
{
	if (BonePrefix) { delete [] BonePrefix; BonePrefix = NULL; }
}

// 0x6E4BA0
Vector3 AttachEffectClass::Unknown_08(const Vector3 &)
{
	return(Transform.Get_Translation());
}

// 0x6E4BE0
void AttachEffectClass::Unknown_18()
{
	UnaryEffectClass::Unknown_18();
	if (Get_Flag(10)) {
		Reset_All_Texture_Mappers(RObj.Peek(), false);
	}
	if (SourceObject) {
		RefCountPtr<RenderObjClass> model = Create_Get(SourceObject->Unknown_6F3BC0());
		if (model) {
			Unknown_6E4D00(model);
		}
	}
}

// 0x6E4CC0
void AttachEffectClass::Unknown_20()
{
	for (std::vector<RefCountPtr<RenderObjClass> >::iterator it = Copies.begin(); it != Copies.end(); it++) (*it)->Remove();
}

// 0x6E4D00
void AttachEffectClass::Unknown_6E4D00(const RefCountPtr<RenderObjClass> &robj)
{
	FAIL_IF(!robj) return;
	int bones = robj->Get_Num_Bones();
	for (int i = 0; i < bones; i++) {
		if (BonePrefix == NULL || strnicmp(BonePrefix, robj->Get_Bone_Name(i), BonePrefixLength) == 0) {
			RefCountPtr<RenderObjClass> copy = Create_Get(RObj->Clone());
			if (Get_Flag(9) && robj->Get_Num_Sub_Objects_On_Bone(i) > 0) {
				RefCountPtr<RenderObjClass> sub_obj = Create_Get(robj->Get_Sub_Object_On_Bone(0, i));






				FAIL_IF(!sub_obj) return;
				if (sub_obj->Is_Hidden()) {
					copy->Set_Hidden(true);
				}
			}
			robj->Add_Sub_Object_To_Bone(copy.Peek(), i);
			Copies.push_back(copy);
		}
	}
	int count = robj->Get_Num_Sub_Objects();
	for (i = 0; i < count; i++) {
		RefCountPtr<RenderObjClass> sub = Create_Get(robj->Get_Sub_Object(i));
		Unknown_6E4D00(sub);
	}
}

/*
**	Reconstruction notes (E1, 2026-11-14)
**
**	The window: the factory's helper 0x6EC570 makes the effect; its constructor 0x6E4750 is the object's first
**	own function, and the guarded <string> static 0x6E5010 ends it right before eff_line.cpp's 0x6E5040. The
**	object also first emits EffectObjectClass::Unknown_14 0x6E4860, UnaryEffectClass::Unknown_10 0x6E4880
**	and Unknown_2C 0x6E48A0 and the unary end's implicit destructor 0x6E4970. The +0xD8 byte the constructor
**	copies from its fifth argument's stack slot is VC6's std::vector allocator (the default argument's temporary).
**	The two FAIL_IFs fix the layout: 0x6E4D00's '!robj' on line 64, its '!sub_obj' on line 77; the functions
**	above it are written compactly to fit.
*/
