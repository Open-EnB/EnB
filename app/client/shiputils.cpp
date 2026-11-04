// Earth & Beyond client (reconstruction): shiputils.cpp (the client's 'e:\build\beta\G\app\Client\shiputils.cpp', its
// FAIL_IF / assert path), the colorization of a model's '~S1' / '~S2' / '~SK' vertex materials: the region's colours,
// its environment-map mapper on stage 1 and the post-detail colour function of the shaders that use them. The avatar
// (avatar.cpp's 0x780810) runs it on its body and head; co.cpp's ships run the same file's 0x80F220 (not rebuilt).
#include <vector>
#include <string.h>
#include "..\shared\unknown_debug.h"
#include "..\packets\pkcolorization.h"
#include "env_map_manager.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\mesh.h"
#include "..\..\ww3d2\meshmdl.h"
#include "..\..\ww3d2\matinfo.h"
#include "..\..\ww3d2\vertmaterial.h"
#include "..\..\ww3d2\mapper.h"
#include "..\..\ww3d2\shader.h"
#include "..\..\wwlib\ref_ptr.h"
#include "shiputils.h"
#include "..\..\ww3d2\texture.h"
#include "..\shared\enbassetmgr.h"
#include "font32bit.h"
#include "fontmanager.h"

void Unknown_80FCE0(RenderObjClass *robj, std::vector<ColorizationRegionStruct> &regions,
	EnvironmentMapManagerClass *env_maps, const Vector3 *skin);
void Unknown_80FD10(RenderObjClass *robj, std::vector<ColorizationRegionStruct> &regions, int axis, const Vector3 *skin);
void Unknown_812390(RenderObjClass *robj, std::vector<ColorizationRegionStruct> &regions, int region, int axis,
	const Vector3 *skin);

// '~S1' / '~S2' / '~SK' (the S and K in either case): 1, 2, 3; anything else 0
static int Unknown_Region_Material(VertexMaterialClass *vmat)
{
	const char *name = vmat->Get_Name();
	if (name[0] == '~' && (name[1] == 'S' || name[1] == 's')) {
		if (name[2] == '1') return 1;
		if (name[2] == '2') return 2;
		if (name[2] == 'K' || name[2] == 'k') return 3;
	}
	return 0;
}

// One half of a region: a metal (an environment-map grid offset) or -1 / -2 (the two plain looks) and its colour.
// Metal >= 0: GridWSEnvironmentMapper (grid 2, the metal's offset), SCALE2X, the colour a quarter grey;
// -1: GridWSEnvironmentMapper (grid 1, offset 3), MODALPHAADDCOLOR; -2: GridWSClassicEnvironmentMapper (grid 4,
// offset 0xFF), SCALE; the colour then the region's own.
static bool Unknown_Region_Half(const ColorizationHalfStruct &half, int axis, TextureMapperClass **mapper,
	Vector3 &color, ShaderClass::DetailColorFuncType &func)
{
	color = half.Unknown04;
	if (half.Unknown00 >= 0) {
		func = ShaderClass::DETAILCOLOR_SCALE2X;
		*mapper = new GridWSEnvironmentMapperClass(0.0f, 2, 0, half.Unknown00, (GridWSEnvMapperClass::AxisType)axis, 1);
		color.Set(0.25f, 0.25f, 0.25f);
	} else if (half.Unknown00 == -2) {
		func = ShaderClass::DETAILCOLOR_SCALE;
		*mapper = new GridWSClassicEnvironmentMapperClass(0.0f, 4, 0, 0xFF, (GridWSEnvMapperClass::AxisType)axis, 1);
	} else if (half.Unknown00 == -1) {
		func = ShaderClass::DETAILCOLOR_MODALPHAADDCOLOR;
		*mapper = new GridWSEnvironmentMapperClass(0.0f, 1, 0, 3, (GridWSEnvMapperClass::AxisType)axis, 1);
	} else {
		return false;
	}
	return true;
}

// 0x80FCE0: 0x80FD10 with the environment-map manager's axis (+0x10)
// NOT EXACT: functional for POC (D64), not compared
void Unknown_80FCE0(RenderObjClass *robj, std::vector<ColorizationRegionStruct> &regions,
	EnvironmentMapManagerClass *env_maps, const Vector3 *skin)
{
	Unknown_80FD10(robj, regions, env_maps->Unknown10, skin);
}

// 0x80FD10: a mesh named (after its '.') SHIRT<digit>, ARM<digit> or HEAD... takes region 0, PANT<digit> region 1;
// anything else that is not a mesh: each sub-object in turn
// NOT EXACT: functional for POC (D64), not compared
void Unknown_80FD10(RenderObjClass *robj, std::vector<ColorizationRegionStruct> &regions, int axis, const Vector3 *skin)
{
	FAIL_IF(!robj) {
		return;
	}
	if (robj->Class_ID() == RenderObjClass::CLASSID_MESH) {
		const char *name = strchr(robj->Get_Name(), '.');
		name = (name != NULL) ? name + 1 : robj->Get_Name();
		int region;
		if (strncmp(name, "SHIRT", 5) == 0 && name[5] >= '0' && name[5] <= '9') {
			region = 0;
		} else if (strncmp(name, "ARM", 3) == 0 && name[3] >= '0' && name[3] <= '9') {
			region = 0;
		} else if (strlen(name) > 4 && name[0] == 'H' && name[1] == 'E' && name[2] == 'A' && name[3] == 'D') {
			region = 0;
		} else if (strncmp(name, "PANT", 4) == 0 && name[4] >= '0' && name[4] <= '9') {
			region = 1;
		} else {
			return;
		}
		Unknown_812390(robj, regions, region, axis, skin);
		return;
	}
	int count = robj->Get_Num_Sub_Objects();
	for (int i = 0; i < count; i++) {
		RenderObjClass *sub = robj->Get_Sub_Object(i);
		Unknown_80FD10(sub, regions, axis, skin);
		sub->Release_Ref();
	}
}

// 0x812390: the region's two halves onto the mesh's '~S1' / '~S2' vertex materials (colour, stage 1 mapper, the
// shaders' post-detail colour function) and the skin colour onto '~SK'
// NOT EXACT: functional for POC (D64), not compared
void Unknown_812390(RenderObjClass *robj, std::vector<ColorizationRegionStruct> &regions, int region, int axis,
	const Vector3 *skin)
{
	FAIL_IF(regions.size() == 0 || (unsigned)region >= regions.size()) {
		return;
	}
	MeshClass *mesh = (MeshClass *)robj;
	TextureMapperClass *mapper1 = NULL;
	TextureMapperClass *mapper2 = NULL;
	Vector3 color1, color2;
	ShaderClass::DetailColorFuncType func1 = ShaderClass::DETAILCOLOR_DISABLE, func2 = ShaderClass::DETAILCOLOR_DISABLE;
	if (!Unknown_Region_Half(regions[region].Unknown00, axis, &mapper1, color1, func1) ||
		!Unknown_Region_Half(regions[region].Unknown10, axis, &mapper2, color2, func2)) {
		REF_PTR_RELEASE(mapper1);
		REF_PTR_RELEASE(mapper2);
		return;
	}

	MaterialInfoClass *minfo = mesh->Get_Material_Info();
	FAIL_IF(!minfo) {
		REF_PTR_RELEASE(mapper1);
		REF_PTR_RELEASE(mapper2);
		return;
	}
	int count = minfo->Vertex_Material_Count();
	bool s1 = false, s2 = false, sk = false;
	int i;
	for (i = 0; i < count; i++) {
		VertexMaterialClass *vmat = minfo->Peek_Vertex_Material(i);
		if (vmat) {
			int kind = Unknown_Region_Material(vmat);
			if (kind == 1) s1 = true;
			if (kind == 2) s2 = true;
			if (kind == 3) sk = true;
		}
	}
	minfo->Release_Ref();
	if (s1 || s2 || sk) {
		// the client forces the model's clone on a test of the model's own (+0x8C against +0x88), not rebuilt: a
		// clone's mesh shares its prototype's model, so it is cloned anyway
		mesh->Make_Unique(false);
		MeshModelClass *model;
		minfo = mesh->Get_Material_Info();
		model = mesh->Get_Model();
		if (minfo && model && minfo->Vertex_Material_Count() == count) {
			if (count == 1) {
				VertexMaterialClass *vmat = minfo->Peek_Vertex_Material(0);
				if (vmat && !sk) {
					ShaderClass::DetailColorFuncType func = s1 ? func1 : func2;
					vmat->Set_Mapper(s1 ? mapper1 : mapper2, 1);
					vmat->Set_Ambient(s1 ? color1 : color2);
					vmat->Set_Diffuse(s1 ? color1 : color2);
					ShaderClass shader = model->Get_Single_Shader(0);
					shader.Set_Post_Detail_Color_Func(func);
					model->Set_Single_Shader(shader, 0);
					if (model->Has_Shader_Array(0)) {
						for (int p = 0; p < model->Get_Polygon_Count(); p++) {
							ShaderClass ps = model->Get_Shader(p, 0);
							ps.Set_Post_Detail_Color_Func(func);
							model->Set_Shader(p, ps, 0);
						}
					}
				} else if (vmat) {
					vmat->Set_Ambient(*skin);
					vmat->Set_Diffuse(*skin);
				}
			} else {
				for (i = 0; i < count; i++) {
					VertexMaterialClass *vmat = minfo->Peek_Vertex_Material(i);
					if (!vmat) {
						continue;
					}
					int kind = Unknown_Region_Material(vmat);
					if (kind == 1) {
						vmat->Set_Mapper(mapper1, 1);
						vmat->Set_Ambient(color1);
						vmat->Set_Diffuse(color1);
					} else if (kind == 2) {
						vmat->Set_Mapper(mapper2, 1);
						vmat->Set_Ambient(color2);
						vmat->Set_Diffuse(color2);
					} else if (kind == 3) {
						vmat->Set_Ambient(*skin);
						vmat->Set_Diffuse(*skin);
					}
				}
				if (!model->Has_Shader_Array(0)) {
					if (s1 && s2) {
						FAIL_IF("primary material present & secondary material present, but no shader array") {
						}
					} else if (s1 || s2) {
						ShaderClass shader = model->Get_Single_Shader(0);
						shader.Set_Post_Detail_Color_Func(s1 ? func1 : func2);
						model->Set_Single_Shader(shader, 0);
					}
				} else {
					const TriIndex *polys = model->Get_Polygon_Array();
					for (int p = 0; p < model->Get_Polygon_Count(); p++) {
						VertexMaterialClass *vmat = model->Get_Material(polys[p].I, 0);
						if (vmat) {
							int kind = Unknown_Region_Material(vmat);
							if (kind == 1 || kind == 2) {
								ShaderClass ps = model->Get_Shader(p, 0);
								ps.Set_Post_Detail_Color_Func(kind == 1 ? func1 : func2);
								model->Set_Shader(p, ps, 0);
							}
							vmat->Release_Ref();
						}
					}
				}
			}
		} else {
			FAIL_IF(minfo == NULL || model == NULL || minfo->Vertex_Material_Count() != count) {
			}
		}
		REF_PTR_RELEASE(model);
		REF_PTR_RELEASE(minfo);
	}
	REF_PTR_RELEASE(mapper1);
	REF_PTR_RELEASE(mapper2);
}


// the model's +0x84 list, here false (the ship customisation's model is its own)
// @0x810130
static void Unknown_810130(RenderObjClass *robj, TextureClass *texture, const Vector3 &color)	// NAME 1 [Unknown_810130]: neutral (D10); NAME 1 [robj, texture, color]: as 0x80FF20's
{


	FAIL_IF(!robj) return;
	if (robj->Class_ID() == RenderObjClass::CLASSID_MESH) {
		const char *robj_name = robj->Get_Name();

		FAIL_IF(!robj_name) return;
		const char *name = strrchr(robj_name, '.');
		if (name != NULL && *name != 0) {
			name++;
		}
		if (name == NULL) {
			name = robj_name;
		}
		if (strlen(name) >= 9 && strnicmp(name, "namedecal", 9) == 0) {
			MeshClass *mesh = (MeshClass *)robj;
			mesh->Make_Unique(false);
			MeshModelClass *model = mesh->Peek_Model();


			FAIL_IF(!model) return;
			FAIL_IF(model->Has_Material_Array(0)) return;
			FAIL_IF(model->Has_Texture_Array(0, 0)) return;
			FAIL_IF(model->Has_Shader_Array(0)) return;
			MaterialInfoClass *minfo = robj->Get_Material_Info();
			FAIL_IF(!minfo) return;
			minfo->Reset();
			model->Set_Single_Texture(texture, 0, 0);
			minfo->Add_Texture(texture);
			ShaderClass shader = ShaderClass::_PresetOpaqueShader;
			shader.Set_Alpha_Test(ShaderClass::ALPHATEST_ENABLE);
			model->Set_Single_Shader(shader, 0);
			VertexMaterialClass *vmat = model->Get_Single_Material(0);
			FAIL_IF(!vmat) return;
			vmat->Set_Diffuse(color);
			vmat->Set_Ambient(color);
			minfo->Add_Vertex_Material(vmat);
			vmat->Release_Ref();
			minfo->Release_Ref();
		}
	} else {
		int count = robj->Get_Num_Sub_Objects();
		for (int i = 0; i < count; i++) {
			RenderObjClass *sub = robj->Get_Sub_Object(i);
			Unknown_810130(sub, texture, color);
			sub->Release_Ref();
		}
	}
}


// NOT EXACT: functional (D64), from the pseudocode
// @0x80FF20
void Unknown_80FF20(RenderObjClass *robj, const char *name, float *rgb)
{
	Vector3 color(rgb[0], rgb[1], rgb[2]);
	RefCountPtr<Font32Class> font_32bit = Create_NEW(new Font32Class("ship_font.tga", 1, 1.0f));


	FAIL_IF(font_32bit.Peek()->Get_Height() != 16) return;
	Unknown_6D8B90Class *text = new Unknown_6D8B90Class;
	text->Unknown_6D8DC0((Font32Class **)&font_32bit, name, 4);
	TextureClass *name_texture = text->Texture;


	FAIL_IF(!name_texture) return;
	name_texture->Add_Ref();
	delete text;
	name_texture->Set_U_Addr_Mode((TextureClass::TxtAddrMode)2);	// the client's 2 (the tree's enum stops at 1)
	name_texture->Set_V_Addr_Mode((TextureClass::TxtAddrMode)2);
	Unknown_810130(robj, name_texture, color);
	name_texture->Release_Ref();
}


// NOT EXACT: functional (D64), from the pseudocode
// @0x810750
void Unknown_810750(RenderObjClass *robj, bool on)
{


	FAIL_IF(!robj) return;
	int bones = robj->Get_Num_Bones();
	int i;
	for (i = 0; i < bones; i++) {
		const char *bone = robj->Get_Bone_Name(i);
		if (strnicmp(bone, "exhaust", 7) == 0) {
			int count = robj->Get_Num_Sub_Objects_On_Bone(i);
			for (int j = 0; j < count; j++) {
				RefCountPtr<RenderObjClass> sub = Create_Get(robj->Get_Sub_Object_On_Bone(j, i));
				sub->Set_Hidden(!on);
			}
		} else if (strlen(bone) == 4 && strnicmp(bone, "eng", 3) == 0 && bone[3] >= '0' && bone[3] <= '9') {
			if (robj->Get_Num_Sub_Objects_On_Bone(i) == 0) {
				RefCountPtr<RenderObjClass> eng1 = Create_Get(AssetManager->Create_Render_Obj("p-eng1"));
				if (eng1) {
					robj->Add_Sub_Object_To_Bone(eng1.Peek(), i);
				}
				RefCountPtr<RenderObjClass> eng2 = Create_Get(AssetManager->Create_Render_Obj("p-eng2"));
				if (eng2) {
					robj->Add_Sub_Object_To_Bone(eng2.Peek(), i);
				}
				RefCountPtr<RenderObjClass> eng3 = Create_Get(AssetManager->Create_Render_Obj("p-eng3"));
				if (eng3) {
					robj->Add_Sub_Object_To_Bone(eng3.Peek(), i);
				}
				RefCountPtr<RenderObjClass> eng4 = Create_Get(AssetManager->Create_Render_Obj("p-eng4"));
				if (eng4) {
					robj->Add_Sub_Object_To_Bone(eng4.Peek(), i);
				}
			}
			int count = robj->Get_Num_Sub_Objects_On_Bone(i);
			for (int j = 0; j < count; j++) {
				RefCountPtr<RenderObjClass> sub = Create_Get(robj->Get_Sub_Object_On_Bone(j, i));
				sub->Set_Hidden(!on);
			}
		}
	}
	int subs = robj->Get_Num_Sub_Objects();
	for (i = 0; i < subs; i++) {
		RefCountPtr<RenderObjClass> sub = Create_Get(robj->Get_Sub_Object(i));
		Unknown_810750(sub.Peek(), on);
	}
}


// The decals by slot (0x18 bytes each, 16 slots: DecalStruct's layout, the slot in +0x00, 0x7FFFFFF in +0x04 for none)
#define MAX_NUM_DECAL_SLOTS 16	// NAME 3: the FAIL_IF's 'slot_id >= MAX_NUM_DECAL_SLOTS'

void Unknown_80EAF0(RenderObjClass *robj, DecalStruct *slots, SClientUnknown1334Class *decal_ini);	// 0x80EAF0 (cdecl, FAIL_IFs '!robj' l.41, '!robj_name' l.45, 'slot_id >= MAX_NUM_DECAL_SLOTS' l.59: each 'decalNN' mesh's slot through decal.cpp's 0x75DA20 / 0x75CB10; not rebuilt yet)  // NAME 1 [all]: what 0x80E8C0 passes

static float Unknown_Clamp01(float value)	// NAME 1: neutral (D10): inline in 0x80E8C0
{
	if (value < 0.0f) {
		return 0.0f;
	}
	if (value > 1.0f) {
		return 1.0f;
	}
	return value;
}

// NOT EXACT: functional (D64), from the pseudocode: the decals sorted into their slots, the coordinates clamped
// @0x80E8C0
void Unknown_80E8C0(RenderObjClass *model, std::vector<DecalStruct> &decals, SClientUnknown1334Class *decal_ini)
{
	DecalStruct slots[MAX_NUM_DECAL_SLOTS];
	int i;
	for (i = 0; i < MAX_NUM_DECAL_SLOTS; i++) {
		slots[i].Unknown04 = 0x7FFFFFF;
	}
	int count = decals.size();
	for (i = 0; i < count; i++) {
		unsigned int slot = decals[i].Unknown00;
		FAIL_IF(slot >= MAX_NUM_DECAL_SLOTS) {	/*@L433*/
			slot = MAX_NUM_DECAL_SLOTS - 1;
		}
		slots[slot] = decals[i];
		slots[slot].Unknown00 = slot;
		slots[slot].Unknown08.X = Unknown_Clamp01(slots[slot].Unknown08.X);
		slots[slot].Unknown08.Y = Unknown_Clamp01(slots[slot].Unknown08.Y);
		slots[slot].Unknown08.Z = Unknown_Clamp01(slots[slot].Unknown08.Z);
		slots[slot].Unknown08.W = Unknown_Clamp01(slots[slot].Unknown08.W);
	}
	Unknown_80EAF0(model, slots, decal_ini);
}


// NOT EXACT: functional (D64), from the disassembly: a mesh takes 0x812390 with the region (the bone '~NN' above it
// sets it), anything else recurses over its bones' objects, or over its sub-objects when it has no bones
// @0x80F020
static void Unknown_80F020(RenderObjClass *robj, std::vector<ColorizationRegionStruct> &regions, int region, int axis)	// NAME 1 [Unknown_80F020]: neutral (D10); NAME 1 [all]: as 0x80FD10's
{
	FAIL_IF(!robj) {	/*@L109*/
		return;
	}
	if (robj->Class_ID() == RenderObjClass::CLASSID_MESH) {
		Vector3 skin(0.0f, 0.0f, 0.0f);
		Unknown_812390(robj, regions, region, axis, &skin);
		return;
	}
	int bones = robj->Get_Num_Bones();
	if (bones <= 0) {
		int count = robj->Get_Num_Sub_Objects();
		for (int i = 0; i < count; i++) {
			RenderObjClass *sub = robj->Get_Sub_Object(i);
			Unknown_80F020(sub, regions, region, axis);
			sub->Release_Ref();
		}
		return;
	}
	for (int bone = 0; bone < bones; bone++) {
		const char *name = robj->Get_Bone_Name(bone);
		int bone_region = region;
		if (name[0] == '~' && name[1] >= '0' && name[1] <= '9' && name[2] >= '0' && name[2] <= '9') {
			bone_region = (name[1] - '0') * 10 + (name[2] - '0');
		}
		int count = robj->Get_Num_Sub_Objects_On_Bone(bone);
		for (int i = 0; i < count; i++) {
			RenderObjClass *sub = robj->Get_Sub_Object_On_Bone(i, bone);
			Unknown_80F020(sub, regions, bone_region, axis);
			sub->Release_Ref();
		}
	}
}


// NOT EXACT: functional (D64): 0x80F020 from region 0 with the environment-map manager's axis (+0x10)
// @0x80EFF0
void Unknown_80EFF0(RenderObjClass *model, std::vector<ColorizationRegionStruct> &regions, EnvironmentMapManagerClass *unknown)
{
	Unknown_80F020(model, regions, 0, unknown->Unknown10);
}
