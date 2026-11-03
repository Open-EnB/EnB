// Earth & Beyond client (reconstruction): app\shared\ro_funcs.cpp (__FILE__ 0xB724F0). Notes at the end of the file.
#include "rendobj.h"
#include "mesh.h"
#include "meshmdl.h"
#include "matinfo.h"
#include "vertmaterial.h"

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

/*
**	Reconstruction notes (MSG2, 2026-11-02; rof)
**
**	client.exe: ro_funcs.cpp's window is 0x495F60-0x497330 (appfile.py show: asserts on lines 55-411, the
**	model helpers 0x496110 / 0x496240 / ... the pick 0x496960). Rebuilt:
**	- 0x496730 (exact): recursive Set_Sort_Level walker over RenderObjClass sub-objects, decrementing sort_level
**	  when matching if dec_if_match.
**	- 0x496AE0 (exact): recursive Make_Unique helper for meshes (Make_Unique(false) + Make_Vertex_Materials_Unique,
**	  and Make_Geometry_Unique if make_geometry_unique) and sub-objects.
**	- 0x497240 (exact): ALLOW_NPATCHES flag set or cleared on every mesh's model through sub-objects.
**	None of these three functions contain asserts; the asserts in the file are on lines 55-411.
*/
