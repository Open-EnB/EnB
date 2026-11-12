static INIClass _MetalDatabase;	// 0xBE8D30  // NAME 2: metal_equivalent_rgbs.ini, as 0x8117D0's _BloodDatabase
// @0x80F470
static void Unknown_80F470(RenderObjClass *robj, std::vector<ColorizationRegionStruct> &reg_mat_defs, unsigned current_region)	// NAME 3 [reg_mat_defs, current_region]: the FAIL_IF; NAME 1 [Unknown_80F470]: neutral (D10); The region's first colour (a metal's from metal_equivalent_rgbs.ini) onto the '~s1' vertex materials once they differ
{
	FAIL_IF(current_region >= reg_mat_defs.size()) {	/*@L157*/
		return;
	}
	if (!_MetalDatabase.Is_Loaded()) {
		_MetalDatabase.Load("metal_equivalent_rgbs.ini");
	}
	Vector3 color = reg_mat_defs[current_region].Unknown00.Unknown04;
	if (reg_mat_defs[current_region].Unknown00.Unknown00 >= 0) {
		char section[256];
		sprintf(section, "Metal-%02d", reg_mat_defs[current_region].Unknown00.Unknown00);
		float r = _MetalDatabase.Get_Float(section, "R", 1.0f);
		float g = _MetalDatabase.Get_Float(section, "G", 1.0f);
		color.Z = _MetalDatabase.Get_Float(section, "B", 1.0f);
		color.X = r;
		color.Y = g;
	}
	MeshClass *mesh = (MeshClass *)robj;
	RefCountPtr<MaterialInfoClass> minfo = Create_Get(mesh->Get_Material_Info());
	FAIL_IF(!minfo) return;	/*@L180*/
	int vmat_count = minfo->Vertex_Material_Count();
	for (int i = 0; i < vmat_count; i++) {
		{
			RefCountPtr<VertexMaterialClass> vmat = Create_Get(minfo->Get_Vertex_Material(i));
			FAIL_IF(!vmat) {	/*@L186*/
				return;
			}
			if (strnicmp(vmat->Get_Name(), "~s1", 3) != 0) {
				continue;
			}
			Vector3 diffuse;
			Vector3 ambient;
			vmat->Get_Diffuse(&diffuse);
			vmat->Get_Ambient(&ambient);
			if (fabs(diffuse.X - color.X) <= 0.004f && fabs(diffuse.Y - color.Y) <= 0.004f && fabs(diffuse.Z - color.Z) <= 0.004f
				&& fabs(ambient.X - color.X) <= 0.004f && fabs(ambient.Y - color.Y) <= 0.004f && fabs(ambient.Z - color.Z) <= 0.004f) {
				continue;
			}
		}
			mesh->Make_Unique(false);	// the client passes a test of the model's list at +0x88 (unknown)
			minfo = Create_Get(mesh->Get_Material_Info());
			FAIL_IF(!minfo) return;	/*@L217*/
			FAIL_IF(vmat_count != minfo->Vertex_Material_Count()) {	/*@L218*/
				return;
			}
			if (vmat_count == 1) {
				RefCountPtr<VertexMaterialClass> vmat = Create_Get(minfo->Get_Vertex_Material(0));
				FAIL_IF(!vmat) {	/*@L223*/
					return;
				}
				vmat->Set_Ambient(color);
				vmat->Set_Diffuse(color);
			} else {
				for (int j = 0; j < vmat_count; j++) {
					RefCountPtr<VertexMaterialClass> vmat = Create_Get(minfo->Get_Vertex_Material(j));
					FAIL_IF(!vmat) return;	/*@L235*/
					if (strnicmp(vmat->Get_Name(), "~s1", 3) == 0) { vmat->Set_Ambient(color); vmat->Set_Diffuse(color); }
				}
			}
		break;
	}
}
