#include <string.h>
#include <stdio.h>
#include <math.h>
#include "rs_hanim.h"
#include "assetmgr.h"
#include "htree.h"
#include "ww3d.h"
#include "..\..\wwlib\nstrdup.h"
#include "..\shared\g_assert.h"
























Unknown4596A0Class::Unknown4596A0Class(RenderStateListClass *list, int id, int link_id, unsigned int flags, const char *hAnimName, const char *suffix, int next_id, HAnimComboDataClass *combo) :
	RenderStateClass(list, id, link_id, flags),
	Combo(true),
	NextID(next_id),
	NextRenderState(NULL),
	HTree(NULL),
	Unknown58(-1.0f),
	Unknown5C(0.0f)
{
	G_ASSERT(hAnimName);
	HAnimName = nstrdup(hAnimName);
	HAnimSuffix = nstrdup(suffix);
	Combo.Copy(combo);
}

Unknown4596A0Class::Unknown4596A0Class(RenderStateListClass *list, const Unknown4596A0Class &source) :
	RenderStateClass(list, source),
	Combo(true),
	NextID(source.NextID),
	NextRenderState(NULL),
	HTree(NULL),
	Unknown58(-1.0f),
	Unknown5C(0.0f)
{
	HAnimName = nstrdup(source.Get_HAnim_Name());
	HAnimSuffix = nstrdup(source.Get_HAnim_Suffix());
	Combo.Copy(&source.Combo);
}

RenderStateClass *Unknown4596A0Class::Unknown_04(RenderStateListClass *list)
{
	return(new Unknown4596A0Class(list, *this));
}

Unknown4596A0Class::~Unknown4596A0Class()
{
	if (HAnimName != NULL) {
		delete [] HAnimName;
	}
	if (HAnimSuffix != NULL) {
		delete [] HAnimSuffix;
	}
}

RenderStateClass *Unknown4596A0Class::Get_Next_Render_State()
{
	if (NextID == -1) {
		return(NULL);
	}
	if (NextRenderState == NULL) {
		NextRenderState = List->Unknown_45A7E0(NextID);
		G_ASSERT(NextRenderState);
	}
	return(NextRenderState);
}

void Unknown4596A0Class::Unknown_44(float seconds)
{
	if (Unknown58 < 0.0f || Unknown58 > 1.0f) {
		Unknown58 = 1.0f;
	}
	Unknown5C = -1.0f;
	if (seconds >= 0.0001f) {
		Unknown5C = -1.0f / seconds;
	}
}

void Unknown4596A0Class::Unknown_48(float seconds)
{
	if (Unknown58 < 0.0f || Unknown58 > 1.0f) {
		Unknown58 = 0.0f;
	}
	Unknown5C = 1.0f;
	if (seconds >= 0.0001f) {
		Unknown5C = 1.0f / seconds;
	}
}

void Unknown4596A0Class::Unknown_30(RsTrackClass *owner, float seconds)
{
	HAnimClass *anim = Combo.Peek_HAnim();
	if (anim == NULL) {
		return;
	}
	float frame = Combo.Get_Frame();
	float rate = anim->Get_Frame_Rate();
	if (Unknown58 < 0.0f || Unknown58 > 1.0f) {
		Combo.Set_Weight(1.0f);
	} else {
		Combo.Set_Weight(Unknown58);
		Unknown58 += WW3D::Get_Frame_Time() * Unknown5C;
		if (Unknown58 < 0.0f) {
			owner->Unknown_460710(this, 50);
		}
	}
	int frames = anim->Get_Num_Frames();
	float new_frame = rate * seconds + frame;
	float last_frame = frames - 1;
	if (new_frame < last_frame) {
		Combo.Set_Frame(new_frame);
		return;
	}
	float num_frames = frames;
	seconds -= (num_frames - frame) / rate;
	Unknown_4584B0(owner, seconds, Get_Next_Render_State());
	if (Flags & 0x10) {
		if (!(Flags & 0x44)) {
			Combo.Set_Frame((float)fmod(new_frame, num_frames));
		} else {
			Combo.Set_Frame(last_frame);
		}
	} else {
		Combo.Set_Frame(0.0f);
	}
}

void Unknown4596A0Class::Unknown_34(float seconds)
{
	if (Combo.Peek_HAnim() != NULL) {
		Combo.Set_Frame(Combo.Peek_HAnim()->Get_Frame_Rate() * seconds);
	}
}





































void Unknown4596A0Class::Unknown_14(RsTrackClass *owner)
{
	Unknown_457E10();
	Unknown58 = -1.0f;
	const HTreeClass *htree = owner->Peek_Render_Object()->Get_HTree();
	if (HTree != htree) {
		HTree = htree;
		if (htree != NULL) {
			HAnimClass *anim;
			char name[MAX_PATH + 1];
			const char *c = htree->Get_Name();
			if (strlen(HAnimName) > 0) {
				if (strchr(HAnimName, '.') != NULL) {
					anim = WW3DAssetManager::Get_Instance()->Get_HAnim(HAnimName);
				} else {
					G_ASSERT((strlen(c)+strlen(HAnimName) + 2) < MAX_PATH);
					sprintf(name, "%s.%s", c, HAnimName);
					anim = WW3DAssetManager::Get_Instance()->Get_HAnim(name);
				}
			} else {
				G_ASSERT((strlen(c) * 2 + strlen(HAnimSuffix) * 2) < MAX_PATH);
				sprintf(name, "%s.%s%s", c, c, HAnimSuffix);
				anim = WW3DAssetManager::Get_Instance()->Get_HAnim(name);
			}
			Combo.Give_HAnim(anim);
			if (Combo.Peek_Pivot_Map() != NULL) {
				NamedPivotMapClass *map = Combo.Peek_Pivot_Map()->As_Named_Pivot_Map();
				if (map != NULL) {
					map->Update_Pivot_Map(HTree);
				} else {
					Combo.Build_Active_Pivot_Map();
				}
			}
		} else {
			Combo.Set_HAnim(NULL);
		}
	}
	Flags &= ~0x80;
	owner->Unknown8C &= ~2;
}

void Unknown4596A0Class::Unknown_18(RsTrackClass *owner)
{
	Unknown_457E90();
	Unknown58 = -1.0f;
	Combo.Set_Frame(0.0f);
	owner->Unknown8C &= ~2;
}

void Unknown4596A0Class::Unknown_38(RsTrackClass *owner)
{
	if (Combo.Peek_HAnim() != NULL) {
		owner->Unknown_462180(&Combo);
	}
}

void Unknown4596A0Class::Unknown_3C()
{
	if (Flags & 1) {
		RenderStateClass *next = Get_Next_Render_State();
		if (next != NULL) {
			next->Unknown_3C();
		}
	} else {
		Flags |= 0x20;
	}
}


/*
** rs_hanim.cpp: reconstruction notes
**
** The client's window 0x4596A0-0x45A010 (13 functions and the scalar deleting destructor 0x4597B0): the
** asserts push 'e:\build\beta\G\app\shared\rs_hanim.cpp' (0xB70ED8) at lines 43 ('hAnimName', the constructor's
** parameter), 85 ('NextRenderState', Get_Next_Render_State, also inlined into 0x459B10 and 0x45A010), 208 and
** 213 (Unknown_14's name buffers). The vtable 0xAE7E78 gives the slots; the next object starts at 0x45A090
** (the 'link' state, vtable 0xAE7ED4).
**
** Forms that matter:
**   the copy constructor 0x4597E0 reads the name and suffix through inline getters (plain member reads
**   load the name into edx, the client's eax);
**   Unknown_14's name buffer is MAX_PATH + 1 bytes (the client's frame is 0x108; MAX_PATH gives 0x104), and
**   the htree == NULL branch is the else (the client lays out Set_HAnim(NULL) last);
**   Unknown_30 subtracts the time used into 'seconds' before Get_Next_Render_State (the client stores it back
**   into the parameter's slot).
**
** The render state's slots +0x38, +0x44 and +0x48 take an RsTrackClass * and floats (this file's overrides):
** rs_track.h's base declarations were retyped to match (their empty bodies compile alike).
*/
