// Earth & Beyond client (reconstruction): app\Shared\enbassetmgr.cpp (__FILE__ in the assert at 0xB721AC). Notes at the end of the file.
#include "enbassetmgr.h"
#include "rendobj.h"
#include "proto.h"
#include "wwprofile.h"
#include "unknown_debug.h"
#include <string.h>
#include <stdio.h>
#pragma warning(disable: 4786)
#include <vector>

#include "dx8wrapper.h"
#include "dx8caps.h"
#include "mesh.h"
#include "meshmdl.h"
#include "coltype.h"
#include "matinfo.h"
#include "vertmaterial.h"

void Unknown_48E9D0(RenderObjClass *robj);	// 0x48E9D0: Post_Process' walk of a model (recursive)  // NAME 1 [Unknown_48E9D0, robj]: neutral (D10)
bool Unknown_48E620(RenderObjClass *robj);	// 0x48E620: Post_Process' step for the 'plt' models (recursive)  // NAME 1 [Unknown_48E620, robj]: neutral (D10)

// @0x48B070
ENBAssetManager::ENBAssetManager()
{
}

// @0x48B0D0
ENBAssetManager::~ENBAssetManager()
{
}

// 'ENBAssetManager::Post_Process' (0x48EB90, its FAIL_IF(!robj) on line 635; not rebuilt): Create_Render_Obj's copy
static inline RenderObjClass *Post_Process(RenderObjClass *robj)	// NAME 3 [Post_Process]: its profile string
{
	WWPROFILE("ENBAssetManager::Post_Process");
	FAIL_IF(!robj) return(NULL);
	Unknown_48E9D0(robj);
	const char *name = robj->Get_Name();
	if (!DX8Wrapper::Get_Current_Caps()->Support_TnL() && strnicmp(name, "plt", 3) == 0) {
		Unknown_48E620(robj);
	}
	return(robj);
}

// @0x48B0F0
RenderObjClass *ENBAssetManager::Create_Render_Obj(const char *name)
{
	WWPROFILE("ENBAssetManager::Create_Render_Obj()");
	RenderObjClass *robj = WW3DAssetManager::Create_Render_Obj(name);
	if (robj != NULL) {
		robj = Post_Process(robj);
	} else {
		robj = NULL;
	}
	return(robj);
}

// @0x48F2C0
void ENBAssetManager::Release_Unused_Assets(void)
{
	int count = Prototypes.Count();
	while (count-- > 0) {
		PrototypeClass *proto = Prototypes[count];
		Prototypes.Delete(count);
		if (proto != NULL) {
			delete proto;
		}
	}
	memset(PrototypeHashTable, 0, sizeof(PrototypeClass *) * PROTOTYPE_HASH_TABLE_SIZE);
	HAnimManager.Free_All_Anims();
	HTreeManager.Free_All_Trees();
	WW3DAssetManager::Release_Unused_Assets();
}

// @0x48BCF0
void ENBAssetManager::Unknown_48BCF0(MeshClass *mesh, bool geometry, bool colors)	// NAME 1 [Unknown_48BCF0, geometry, colors]: neutral (D10), what each flag makes unique
{
	mesh->Make_Unique();
	MeshModelClass *model = mesh->Get_Model();
	if (colors) {
		MaterialInfoClass *matinfo = mesh->Get_Material_Info();
		for (int i = 0; i < matinfo->Vertex_Material_Count(); ++i) {
			matinfo->Peek_Vertex_Material(i)->Make_Unique();
		}
		REF_PTR_RELEASE(matinfo);
		model->Make_Color_Array_Unique(0);
		model->Make_Color_Array_Unique(1);
	}
	if (geometry) {
		model->Make_Geometry_Unique();
	}
	REF_PTR_RELEASE(model);
}

// @0x48BDC0
void ENBAssetManager::Unknown_48BDC0(RenderObjClass *robj, bool geometry, bool colors)	// NAME 1 [Unknown_48BDC0]: neutral (D10)
{
	int count = robj->Get_Num_Sub_Objects();
	for (int i = 0; i < count; ++i) {
		RenderObjClass *sub = robj->Get_Sub_Object(i);
		switch (sub->Class_ID()) {
			case RenderObjClass::CLASSID_MESH:
				Unknown_48BCF0((MeshClass *)sub, geometry, colors);
				break;
			case RenderObjClass::CLASSID_HLOD:
				Unknown_48BDC0(sub, geometry, colors);
				break;
		}
		if (sub != NULL) {
			sub->Release_Ref();
		}
	}
}

// @0x48BE60
void ENBAssetManager::Unknown_48BE60(RenderObjClass *robj, bool geometry, bool colors)	// NAME 1 [Unknown_48BE60]: neutral (D10)
{
	switch (robj->Class_ID()) {
		case RenderObjClass::CLASSID_MESH:
			Unknown_48BCF0((MeshClass *)robj, geometry, colors);
			break;
		case RenderObjClass::CLASSID_HLOD:
		{
			int count = robj->Get_Num_Sub_Objects();
			for (int i = 0; i < count; ++i) {
				RenderObjClass *sub = robj->Get_Sub_Object(i);
				Unknown_48BE60(sub, geometry, colors);
				if (sub != NULL) {
					sub->Release_Ref();
				}
			}
			break;
		}
	}
}

// @0x48D710
void ENBAssetManager::Unknown_48D710(RenderObjClass *robj, float *hsv)
{
	int count = robj->Get_Num_Sub_Objects();
	for (int i = 0; i < count; ++i) {
		RenderObjClass *sub = robj->Get_Sub_Object(i);
		switch (sub->Class_ID()) {
			case RenderObjClass::CLASSID_MESH:
				Unknown_48D1F0(sub, hsv);
				break;
			case RenderObjClass::CLASSID_HLOD:
				Unknown_48D710(sub, hsv);
				break;
			case RenderObjClass::CLASSID_PARTICLEEMITTER:
				Unknown_48D850(sub, hsv);
				break;
			case RenderObjClass::CLASSID_SPHERE:
				Unknown_48DD00(sub, hsv);
				break;
			case RenderObjClass::CLASSID_RING:
				Unknown_48E0F0(sub, hsv);
				break;
		}
		if (sub != NULL) {
			sub->Release_Ref();
		}
	}
}

// @0x48E4E0
void ENBAssetManager::Unknown_48E4E0(RenderObjClass *robj, float *hsv)
{
	switch (robj->Class_ID()) {
		case RenderObjClass::CLASSID_MESH:
			Unknown_48D1F0(robj, hsv);
			break;
		case RenderObjClass::CLASSID_HLOD:
		{
			int count = robj->Get_Num_Sub_Objects();
			for (int i = 0; i < count; ++i) {
				RenderObjClass *sub = robj->Get_Sub_Object(i);
				Unknown_48E4E0(sub, hsv);
				if (sub != NULL) {
					sub->Release_Ref();
				}
			}
			break;
		}
		case RenderObjClass::CLASSID_PARTICLEEMITTER:
			Unknown_48D850(robj, hsv);
			break;
		case RenderObjClass::CLASSID_SPHERE:
			Unknown_48DD00(robj, hsv);
			break;
		case RenderObjClass::CLASSID_RING:
			Unknown_48E0F0(robj, hsv);
			break;
	}
}

// @0x48E620
bool Unknown_48E620(RenderObjClass *robj)
{
	bool found = false;
	if (robj->Class_ID() == RenderObjClass::CLASSID_MESH) {
		MeshModelClass *model = ((MeshClass *)robj)->Get_Model();
		for (int pass = 0; pass < model->Get_Pass_Count(); ++pass) {
			for (int stage = 0; stage < MeshMatDescClass::MAX_TEX_STAGES; ++stage) {
				TextureClass *tex = model->Peek_Single_Texture(pass, stage);
				if (tex != NULL) {
					StringClass name((const char *)tex->Get_Texture_Name());
					_strlwr(name.Peek_Buffer());
					if (	strncmp(name, "plt_at", 6) == 0 || strncmp(name, "plt3505", 7) == 0 ||
							strncmp(name, "plt9601", 7) == 0 || strncmp(name, "plt9703", 7) == 0) {
						found = true;
					}
				}
				if (found) break;
			}
			if (found) break;
		}
		REF_PTR_RELEASE(model);
	} else {
		int count = robj->Get_Num_Sub_Objects();
		std::vector<RenderObjClass *> remove_list;
		for (int i = 0; i < count; ++i) {
			RenderObjClass *sub = robj->Get_Sub_Object(i);
			if (Unknown_48E620(sub)) {
				remove_list.push_back(sub);
			}
			REF_PTR_RELEASE(sub);
		}
		for (unsigned j = 0; j < remove_list.size(); ++j) {
			robj->Remove_Sub_Object(remove_list[j]);
		}
	}
	return(found);
}

// @0x48E9D0
void Unknown_48E9D0(RenderObjClass *robj)
{
	if (robj->Class_ID() == RenderObjClass::CLASSID_MESH) {
		MeshClass *mesh = (MeshClass *)robj;
		if (mesh->Get_Num_Polys() > 30) {
			MeshModelClass *model = mesh->Peek_Model();
			if (!model->Has_Cull_Tree() && !model->Get_Flag(MeshGeometryClass::SKIN)) {
				WWPROFILE("Generate Culling Tree");
				mesh->Generate_Culling_Tree();
			}
		}
		if (!robj->Is_Translucent() && (robj->Get_Collision_Type() & (COLLISION_TYPE_PHYSICAL | COLLISION_TYPE_PROJECTILE)) == 0) {
			robj->Set_Collision_Type(COLLISION_TYPE_3);
		}
	} else {
		int count = robj->Get_Num_Sub_Objects();
		for (int i = 0; i < count; ++i) {
			WWPROFILE("Post_Process_Sub");
			RenderObjClass *sub = robj->Get_Sub_Object(i);
			Unknown_48E9D0(sub);
			if (sub != NULL) {
				sub->Release_Ref();
			}
		}
	}
}

// @0x48ECA0
// NOT EXACT: shape (explain.py: 194 masked rows; 0.48), the client keeps one EH state (around the out-of-line Remove of the
// plain name) where ours numbers all four StringClass temporaries; the logic is the client's (D64)
void ENBAssetManager::Unknown_48ECA0(TextureClass **textures, unsigned count)
{
	for (unsigned i = 0; i < count; ++i) {
		TextureClass *tex = textures[i];
		const char *name = tex->Get_Texture_Name();
		char lower[256];
		strcpy(lower, name);
		_strlwr(lower);
		char hsv_name[256];
		const Vector3 &hsv = tex->Get_HSV_Shift();
		sprintf(hsv_name, "#%s!H%gS%gV%g", lower, hsv.X, hsv.Y, hsv.Z);
		if (TextureHash.Get(hsv_name) == NULL) {
			if (TextureHash.Get(name) != NULL) {
				TextureHash.Remove(name);
			}
		} else {
			TextureHash.Remove(hsv_name);
		}
		textures[i]->Release_Ref();
	}
}

// @0x48F1A0
void ENBAssetManager::Release_Unused_Textures(void)
{
	for (;;) {
		unsigned count = 0;
		TextureClass *temp_textures[256];
		HashTemplateIterator<StringClass,TextureClass*> ite(TextureHash);
		for (ite.First(); !ite.Is_Done(); ite.Next()) {
			TextureClass *tex = ite.Peek_Value();
			if (tex->Num_Refs() == 1) {
				temp_textures[count++] = tex;
				if (count == 256) {
					break;
				}
			}
		}
		if (count == 0) {
			return;
		}
		Unknown_48ECA0(temp_textures, count);
	}
}

/*
**	Notes
**
**	0x48B070 is the constructor WinMain (0x526DE0) calls after new (0x408C bytes). The destructor 0x48B0D0 is declared (it stores
**	the table before the base's; the compiler's would only jump); the deleting destructor 0x48B0A0 is the compiler's.
**	0x48B0F0 (slot +0x14) inlines Post_Process (0x48EB90: its FAIL_IF(!robj) on line 635 drops out after the NULL test); the
**	if / else with robj = NULL is what puts the zero in ebx and joins both paths (a return NULL or a void Post_Process
**	duplicates the epilogue). 0x48F2C0 (slot +0x10) is Free_Assets' prototype loop without its profile, then the base's
**	Release_Unused_Assets. 0x48F1A0 (slot +0x38) is the base's Release_Unused_Textures, except that it hands each batch of
**	256 textures with one reference to 0x48ECA0 and starts over until a pass finds none; 0x48ECA0 removes each texture
**	under its HSV name ('#name!H%gS%gV%g', lower case) or else under its plain name, then releases it. Post_Process' helpers:
**	0x48E9D0 builds a culling tree for each mesh over 30 polygons (not skins) and gives the opaque meshes without physical or
**	projectile collision COLLISION_TYPE_3, recursing into sub-objects; 0x48E620 returns true for a mesh with a 'plt_at' /
**	'plt3505' / 'plt9601' / 'plt9703' texture and removes such meshes from their containers (only without hardware T&L). The
**	single return(found) is what keeps found in memory for the container path. Not written: +0x78 0x48B220 (0x590 bytes; its HSV helpers 0x48D1F0 .. 0x48E0F0
**	and the prototype class at table 0xAEA5A0 are not rebuilt). Its helpers 0x48BCF0 / 0x48BDC0 / 0x48BE60 are: make a mesh's
**	geometry and / or colours (vertex materials, both colour arrays) unique, for a mesh or recursively for an HLOD's meshes;
**	a switch on Class_ID is what puts the HLOD test first.
**	0x48D710 (an HLOD's sub-objects) and 0x48E4E0 (one object, recursing into HLODs) hand the HSV shift to the per-class
**	helpers 0x48D1F0 (mesh), 0x48D850 (particle emitter), 0x48DD00 (sphere), 0x48E0F0 (ring), not written yet.
*/
