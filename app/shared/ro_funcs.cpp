// Earth & Beyond client (reconstruction): app\shared\ro_funcs.cpp (__FILE__ 0xB724F0). Notes at the end of the file.
#include "rendobj.h"
#include "mesh.h"
#include "meshmdl.h"
#include "matinfo.h"
#include "vertmaterial.h"
#include "intersec.h"
#include "layer.h"
#include "unknown_debug.h"



































// 0x495FD0
bool Unknown_495FD0(RenderObjClass *robj, TextureClass *tex, int index)	// NAME 1 [Unknown_495FD0, robj, tex, index]: neutral (D10)
{
	if (robj == NULL || tex == NULL) {
		return false;
	}
	MaterialInfoClass *matinfo = robj->Get_Material_Info();
	if (matinfo == NULL) {
		return false;
	}
	G_ASSERT(robj->Class_ID()==RenderObjClass::CLASSID_MESH);
	TextureClass *old_tex = matinfo->Peek_Texture(index);
	if (old_tex == tex) {
		matinfo->Release_Ref();
		return true;
	}
	((MeshClass *)robj)->Replace_Texture(old_tex, tex);
	matinfo->Replace_Texture(index, tex);
	matinfo->Release_Ref();
	return false;
}
// 0x4960C0
TextureClass *Unknown_4960C0(RenderObjClass *robj, int index)	// NAME 1 [Unknown_4960C0, robj, index]: neutral (D10)
{
	if (robj == NULL) {
		return NULL;
	}
	TextureClass *tex = NULL;
	MaterialInfoClass *matinfo = robj->Get_Material_Info();
	if (matinfo != NULL) {
		tex = matinfo->Peek_Texture(index);
		matinfo->Release_Ref();
	}
	return tex;
}
// 0x496110
// NOT EXACT: shape
bool Unknown_496110(RenderObjClass *robj, float r, float g, float b)	// NAME 1 [Unknown_496110, robj, r, g, b]: neutral (D10)
{
	if (robj == NULL) {
		return false;
	}
	bool res = false;
	MaterialInfoClass *matinfo = robj->Get_Material_Info();
	if (matinfo != NULL) {
		for (int i = 0; i < matinfo->Vertex_Material_Count(); i++) {
			VertexMaterialClass *vmat = matinfo->Get_Vertex_Material(i);
			if (vmat != NULL) {
				vmat->Set_Emissive(r, g, b);
				vmat->Release_Ref();
				res = true;
			}
		}
		matinfo->Release_Ref();
		return res;
	}
	int count = robj->Get_Num_Sub_Objects();
	for (int i = 0; i < count; i++) {
		RenderObjClass *sub = robj->Get_Sub_Object(i);
		if (Unknown_496110(sub, r, g, b) || res) {
			res = true;
		}
		if (sub != NULL) {
			sub->Release_Ref();
		}
	}
	return res;
}
// 0x496240
// NOT EXACT: shape
bool Unknown_496240(RenderObjClass *robj, float opacity)	// NAME 1 [Unknown_496240, robj, opacity]: neutral (D10)
{
	if (robj == NULL) {
		return false;
	}
	MaterialInfoClass *matinfo = robj->Get_Material_Info();
	if (matinfo != NULL) {
		bool res = false;
		for (int i = 0; i < matinfo->Vertex_Material_Count(); i++) {
			VertexMaterialClass *vmat = matinfo->Get_Vertex_Material(i);
			if (vmat != NULL) {
				vmat->Set_Opacity(opacity);
				vmat->Release_Ref();
				res = true;
			}
		}
		matinfo->Release_Ref();
		return res;
	}
	bool res = false;
	int count = robj->Get_Num_Sub_Objects();
	for (int i = 0; i < count; i++) {
		RenderObjClass *sub = robj->Get_Sub_Object(i);
		if (Unknown_496240(sub, opacity) || res) {
			res = true;
		}
		if (sub != NULL) {
			sub->Release_Ref();
		}
	}
	return res;
}
// 0x496360
// NOT EXACT: shape
bool Unknown_496360(RenderObjClass *robj, float r, float g, float b)	// NAME 1 [Unknown_496360, robj, r, g, b]: neutral (D10)
{
	if (robj == NULL) {
		return false;
	}
	bool res = false;
	MaterialInfoClass *matinfo = robj->Get_Material_Info();
	if (matinfo != NULL) {
		int count = matinfo->Vertex_Material_Count();
		for (int i = 0; i < count; i++) {
			VertexMaterialClass *vmat = matinfo->Get_Vertex_Material(i);
			if (vmat != NULL) {
				vmat->Set_Diffuse(r, g, b);
				vmat->Release_Ref();
				res = true;
			}
		}
		matinfo->Release_Ref();
		return res;
	}
	int count = robj->Get_Num_Sub_Objects();
	for (int i = 0; i < count; i++) {
		RenderObjClass *sub = robj->Get_Sub_Object(i);
		if (Unknown_496360(sub, r, g, b)) {
			res = true;
		}
		if (sub != NULL) {
			sub->Release_Ref();
		}
	}
	return res;
}
// 0x496490
// NOT EXACT: shape
bool Unknown_496490(RenderObjClass *robj, bool half)	// NAME 1 [Unknown_496490, robj, half]: neutral (D10)
{
	if (robj == NULL) {
		return false;
	}
	bool res = false;
	MaterialInfoClass *matinfo = robj->Get_Material_Info();
	if (matinfo != NULL) {
		int count = matinfo->Vertex_Material_Count();
		for (int i = 0; i < count; i++) {
			VertexMaterialClass *vmat = matinfo->Get_Vertex_Material(i);
			if (vmat != NULL) {
				Vector3 diff;
				vmat->Get_Diffuse(&diff);
				if (half) {
					Vector3 em(diff.X * 0.5f, diff.Y * 0.5f, diff.Z * 0.5f);
					vmat->Set_Emissive(em);
				} else {
					vmat->Set_Emissive(diff);
				}
				vmat->Release_Ref();
				res = true;
			}
		}
		matinfo->Release_Ref();
		return res;
	}
	int count = robj->Get_Num_Sub_Objects();
	for (int i = 0; i < count; i++) {
		RenderObjClass *sub = robj->Get_Sub_Object(i);
		if (Unknown_496490(sub, half)) {
			res = true;
		}
		if (sub != NULL) {
			sub->Release_Ref();
		}
	}
	return res;
}

// 0x496620
bool Unknown_496620(RenderObjClass *robj)	// NAME 1 [Unknown_496620, robj]: neutral (D10)
{
	if (robj == NULL) {
		return false;
	}
	if (robj->Class_ID() == RenderObjClass::CLASSID_MESH) {
		((MeshClass *)robj)->Make_Unique(false);
		MeshModelClass *model = ((MeshClass *)robj)->Get_Model();
		if (model == NULL) {
			return false;
		}
		ShaderClass shader = model->Get_Single_Shader(0);
		shader.Set_Dst_Blend_Func(ShaderClass::DSTBLEND_ONE);
		shader.Set_Src_Blend_Func(ShaderClass::SRCBLEND_ONE);
		model->Set_Single_Shader(shader, 0);
		model->Release_Ref();
		return true;
	}
	int count = robj->Get_Num_Sub_Objects();
	bool res = false;
	for (int i = 0; i < count; i++) {
		RenderObjClass *sub = robj->Get_Sub_Object(i);
		if (Unknown_496620(sub)) {
			res = true;
		}
		if (sub != NULL) {
			sub->Release_Ref();
		}
	}
	return res;
}

// 0x496730
void Unknown_496730(RenderObjClass *robj, int sort_level, bool dec_if_match)	// NAME 1 [Unknown_496730, robj, sort_level, dec_if_match]: neutral (D10)
{
	if (robj == NULL) {
		return;
	}
	robj->Set_Sort_Level(sort_level);
	if (dec_if_match && sort_level != 0) {
		if (robj->Get_Sort_Level() == sort_level) {
			sort_level--;
		}
	}
	int count = robj->Get_Num_Sub_Objects();
	for (int i = 0; i < count; i++) {
		RenderObjClass *sub = robj->Get_Sub_Object(i);
		if (sub) {
			Unknown_496730(sub, sort_level, dec_if_match);
			sub->Release_Ref();
		}
	}
}

// 0x4967F0
// NOT EXACT: shape
bool Unknown_4967F0(RenderObjClass *robj, int delta, RenderObjClass *base_robj)	// NAME 1 [Unknown_4967F0, robj, delta, base_robj]: neutral (D10)
{
	if (robj == NULL) {
		return false;
	}
	if (base_robj == NULL) {
		base_robj = robj;
	}
	int sort_level = base_robj->Get_Sort_Level();
	if (sort_level == 0) {
		sort_level = 32;
		int count = base_robj->Get_Num_Sub_Objects();
		for (int i = 0; i < count; i++) {
			RenderObjClass *sub_obj = base_robj->Get_Sub_Object(i);
			FAIL_IF(!sub_obj) continue;
			int sub_level = sub_obj->Get_Sort_Level();
			if (sub_level != 0 && sub_level < sort_level) sort_level = sub_level;
			sub_obj->Release_Ref();
		}
	}
	sort_level += delta;
	if (sort_level <= 0) return false;
	if (sort_level >= 32) return false;
	robj->Set_Sort_Level(sort_level);
	int level = sort_level;
	if (robj->Get_Sort_Level() == sort_level) level = sort_level - 1;
	int count = robj->Get_Num_Sub_Objects();
	for (int i = 0; i < count; i++) {
		RenderObjClass *sub = robj->Get_Sub_Object(i);
		if (sub != NULL) {
			Unknown_496730(sub, level, true);
			sub->Release_Ref(); }
	}
	return true;
}
// 0x496960
bool Unknown_496960(RenderObjClass *robj, float x, float y, const LayerClass &layer)	// NAME 1 [Unknown_496960, robj, x, y, layer]: neutral (D10)
{
	if (robj == NULL) return false;
	FAIL_IF(!layer.Peek_Scene()) return false;
	FAIL_IF(!layer.Peek_Camera()) return false;
	IntersectionClass intersection;
	IntersectionResultClass result;
	return intersection.Intersect_Sub_Object(x, y, (LayerClass &)layer, robj, &result) != NULL;
}

// 0x496AE0
void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique)	// NAME 1 [Unknown_496AE0, robj, make_geometry_unique]: neutral (D10)
{
	if (robj == NULL) {
		return;
	}
	if (robj->Class_ID() == RenderObjClass::CLASSID_MESH) {
		((MeshClass *)robj)->Make_Unique(false);
		MaterialInfoClass *matinfo = robj->Get_Material_Info();
		matinfo->Make_Vertex_Materials_Unique();
		if (matinfo) {
			matinfo->Release_Ref();
		}
		if (make_geometry_unique) {
			MeshModelClass *model = ((MeshClass *)robj)->Get_Model();
			model->Make_Geometry_Unique();
			if (model) {
				model->Release_Ref();
			}
		}
		return;
	}
	int count = robj->Get_Num_Sub_Objects();
	for (int i = 0; i < count; i++) {
		RenderObjClass *sub = robj->Get_Sub_Object(i);
		Unknown_496AE0(sub, make_geometry_unique);
		if (sub) {
			sub->Release_Ref();
		}
	}
}

// 0x496DB0: every LOD level to its highest (Get_LOD_Count - 1), each sub-object's too (avatar.cpp's body 0x77E9D0,
// ui_class.cpp's 0x577B80)
// NOT EXACT: functional for POC (D64), inline-budget (the client's RefCountPtr 0x474080 for each sub-object)
void Unknown_496DB0(RenderObjClass *robj)	// NAME 1 [Unknown_496DB0, robj]: neutral (D10)
{
	robj->Set_LOD_Level(robj->Get_LOD_Count() - 1);
	int count = robj->Get_Num_Sub_Objects();
	for (int i = 0; i < count; i++) {
		RenderObjClass *sub = robj->Get_Sub_Object(i);
		Unknown_496DB0(sub);
		if (sub) {
			sub->Release_Ref();
		}
	}
}

// 0x497240
void Unknown_497240(RenderObjClass *robj, bool on)	// NAME 1 [Unknown_497240, robj, on]: neutral (D10)
{
	if (robj == NULL) {
		return;
	}
	if (robj->Class_ID() == RenderObjClass::CLASSID_MESH) {
		((MeshClass *)robj)->Peek_Model()->Set_Flag(MeshGeometryClass::ALLOW_NPATCHES, on);
		return;
	}
	int count = robj->Get_Num_Sub_Objects();
	for (int i = 0; i < count; i++) {
		RenderObjClass *sub = robj->Get_Sub_Object(i);
		Unknown_497240(sub, on);
		if (sub) {
			sub->Release_Ref();
		}
	}
}

// (included here, not at the top, so the FAIL_IFs above keep the client's __LINE__)
#include "texture.h"
#include "textureloader.h"
#include "surfaceclass.h"

// NOT EXACT: functional (D64), from the pseudocode and the disassembly: the colour of the model's first texture
// at (u, v), u and v 0..1 (FAIL_IFs '!material' l.365, l.367, '!surf' l.374); black when it can't
// @0x496C00
void Unknown_496C00(RenderObjClass *robj, float u, float v, Vector3 *color)	// NAME 1 [Unknown_496C00, robj, u, v, color]: neutral (D10)
{
	MaterialInfoClass *material = robj->Get_Material_Info();
	FAIL_IF(!material) {
		color->Set(0.0f, 0.0f, 0.0f);
		return;
	}
	TextureClass *texture = material->Peek_Texture(0);
	FAIL_IF(!texture) {
		color->Set(0.0f, 0.0f, 0.0f);
		material->Release_Ref();
		return;
	}
	if (!texture->Is_Initialized()) {
		TextureLoader::Request_Foreground_Loading(texture);
	}
	SurfaceClass *surf = texture->Get_Surface_Level(0);
	FAIL_IF(!surf) {
		color->Set(0.0f, 0.0f, 0.0f);
		material->Release_Ref();
		return;
	}
	SurfaceClass::SurfaceDescription desc;
	surf->Get_Description(desc);
	surf->Get_Pixel(*color, (int)(desc.Width * u), (int)(desc.Height * v));
	surf->Release_Ref();
	material->Release_Ref();
}

/*
**	Reconstruction notes (MSG2, 2026-11-02; rof; pick)
**
**	client.exe: ro_funcs.cpp's window is 0x495F60-0x497330 (appfile.py show: asserts on lines 55-411, the
**	model helpers 0x496110 / 0x496240 / ... the pick 0x496960). Rebuilt:
**	- 0x495FD0 (exact)
**	- 0x4960C0 (exact)
**	- 0x496110 (functional: shape)
**	- 0x496240 (functional: shape)
**	- 0x496360 (functional: shape)
**	- 0x496490 (functional: shape)
**	- 0x496620 (exact)
**	- 0x496730 (exact)
**	- 0x4967F0 (functional: shape)
**	- 0x496960 (exact)
**	- 0x496AE0 (exact)
**	- 0x497240 (exact)
*/
