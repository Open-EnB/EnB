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
