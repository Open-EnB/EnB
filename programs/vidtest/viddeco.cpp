// Earth & Beyond VIDTEST (reconstruction, D45, D46): the land's decorations (viddeco.h). Notes at the end.
// NOT EXACT, the whole file: functional, after VIDTEST's 2002 copy (0x414B50, 0x415C40, 0x416170, 0x410B90).
#include "viddeco.h"
#include "land.h"
#include "heightfield.h"
#include "vector2.h"
#include "ini.h"
#include "texture.h"
#include "assetmgr.h"
#include "vertmaterial.h"
#include "dx8wrapper.h"
#include "dx8vertexbuffer.h"
#include "dx8indexbuffer.h"
#include "dx8fvf.h"
#include "rinfo.h"
#include "random.h"
#include "matrix3d.h"
#include "mesh.h"
#include "meshmdl.h"
#include "wwmath.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

static inline int Int_Min(int a, int b) { return a < b ? a : b; }	// NAME 2: ours
static inline int Int_Max(int a, int b) { return a > b ? a : b; }

VidtestDecorationsClass::VidtestDecorationsClass(LandClass *land) :
	Land(land),
	Field(NULL),
	PatchSize(0),
	GridWidth(0)
{
	if (Land != NULL) {
		Land->Add_Ref();
	}
}

VidtestDecorationsClass::~VidtestDecorationsClass(void)
{
	for (int i = 0; i < DecoBatches.Count(); i++) {
		DecoBatchStruct *batch = DecoBatches[i];
		REF_PTR_RELEASE(batch->Texture);
		REF_PTR_RELEASE(batch->Material);
		REF_PTR_RELEASE(batch->VertexBuffer);
		REF_PTR_RELEASE(batch->IndexBuffer);
		delete batch;
	}
	DecoBatches.Delete_All();
	REF_PTR_RELEASE(Land);
}

/*
** The land's bounds (LandClass keeps RenderObjClass's: the scene must not cull the trees)
*/
void VidtestDecorationsClass::Get_Obj_Space_Bounding_Sphere(SphereClass &sphere) const
{
	float half = (Field != NULL) ? Field->Get_Extent() * 0.5f : 0.0f;
	sphere.Init(Vector3(0.0f, 0.0f, 0.0f), half * 1.5f);
}

void VidtestDecorationsClass::Get_Obj_Space_Bounding_Box(AABoxClass &box) const
{
	float half = (Field != NULL) ? Field->Get_Extent() * 0.5f : 0.0f;
	box.Init(Vector3(0.0f, 0.0f, 0.0f), Vector3(half, half, half));
}

/*
/*
** Decorations (VIDTEST 0x415C40, client 0x479D00): decoration.ini [set]: NUM_DECOS, MIN_SCALE,
** MAX_SCALE, NUM_PROTOS, PROTO%d (default TDTREE01A); Random4Class(0x1105) picks a prototype, a scale
** and a place (u, v) on the field, and a rotation about z. As VIDTEST (0x416170 = client 0x47A2F0),
** the prototypes' meshes are copied, transformed, into shared buffers: one batch per texture /
** shader / material, split where a buffer would pass 65535 vertices or indices.
*/
struct DecoPartStruct	// NAME 2: ours (VIDTEST 0x416170 batches the prototypes)
{
	MeshModelClass *			Model;
	Matrix3D						Transform;		// in the prototype
	TextureClass *				Texture;
	ShaderClass					Shader;
	VertexMaterialClass *	Material;
	int							Batch;
	bool							Shadow;
	bool operator == (const DecoPartStruct &that) const	{ return Model == that.Model && Batch == that.Batch; }
	bool operator != (const DecoPartStruct &that) const	{ return !(*this == that); }
};

struct DecoVertexStruct	// NAME 2: ours
{
	float	X, Y, Z;
	float	NX, NY, NZ;
	float	U, V;
};

struct DecoInstanceStruct	// NAME 2: ours
{
	int		Proto;
	Matrix3D	Transform;
	Matrix3D	Shadow;			// the shadow mesh's: on the ground (VIDTEST 0x416170)
};

/*
** VIDTEST 0x418400: the first mesh under robj whose name holds ".D" or ".d" (tree) or ".S" or ".s"
** (its shadow), depth first; add-reffed
*/
static RenderObjClass *Find_Deco_Mesh(RenderObjClass *robj, bool tree)	// NAME 2: ours
{
	if (robj == NULL) return NULL;
	if (robj->Class_ID() == RenderObjClass::CLASSID_MESH) {
		const char *name = robj->Get_Name();
		if (name != NULL && (strstr(name, tree ? ".d" : ".s") != NULL || strstr(name, tree ? ".D" : ".S") != NULL)) {
			robj->Add_Ref();
			return robj;
		}
		return NULL;
	}
	for (int i = 0; i < robj->Get_Num_Sub_Objects(); i++) {
		RenderObjClass *sub = robj->Get_Sub_Object(i);
		RenderObjClass *found = Find_Deco_Mesh(sub, tree);
		REF_PTR_RELEASE(sub);
		if (found != NULL) return found;
	}
	return NULL;
}

bool VidtestDecorationsClass::Create_Decorations(int set)
{
	if (Land == NULL || Land->Peek_Height_Field() == NULL) {
		return false;
	}
	Field = Land->Peek_Height_Field();
	PatchSize = Land->Get_Patch_Size();
	GridWidth = Land->Get_Grid_Width();
	INIClass ini;
	if (!ini.Load("decoration.ini")) {
		return false;
	}
	char section[16];
	sprintf(section, "%d", set);
	int count = ini.Get_Int(section, "NUM_DECOS", 5000);
	float min_scale = ini.Get_Float(section, "MIN_SCALE", 1.0f);
	float max_scale = ini.Get_Float(section, "MAX_SCALE", 1.0f);
	int protos = ini.Get_Int(section, "NUM_PROTOS", 1);
	if (protos < 1) protos = 1;
	WW3DAssetManager *assets = WW3DAssetManager::Get_Instance();

	// the prototypes' meshes
	DynamicVectorClass<DecoPartStruct> parts;
	DynamicVectorClass<int> first_part;
	DynamicVectorClass<RenderObjClass *> keep;
	for (int p = 0; p < protos; p++) {
		char key[16];
		char name[64];
		sprintf(key, "PROTO%d", p);
		ini.Get_String(section, key, "TDTREE01A", name, sizeof(name));
		first_part.Add(parts.Count());
		RenderObjClass *robj = assets->Create_Render_Obj(name);
		if (robj == NULL) continue;
		robj->Set_Transform(Matrix3D(true));
		keep.Add(robj);
		// the tree is the first mesh named ".D"/".d", its shadow the first ".S"/".s" (VIDTEST 0x418400:
		// TDTREE01A.DTREE, 6 polygons, and TDTREE01A.STREE, one triangle)
		RenderObjClass *meshes[2];
		meshes[0] = Find_Deco_Mesh(robj, true);
		meshes[1] = Find_Deco_Mesh(robj, false);
		for (int m = 0; m < 2; m++) {
			if (meshes[m] == NULL) continue;
			MeshModelClass *model = ((MeshClass *)meshes[m])->Peek_Model();
			if (model != NULL && model->Get_Vertex_Count() > 0 && model->Get_Polygon_Count() > 0) {
				DecoPartStruct part;
				part.Model = model;
				part.Model->Add_Ref();
				part.Transform = meshes[m]->Get_Transform();
				part.Texture = model->Peek_Single_Texture(0, 0);
				if (part.Texture == NULL) part.Texture = model->Peek_Texture(0, 0, 0);
				part.Shader = model->Has_Shader_Array(0) ? model->Get_Shader(0, 0) : model->Get_Single_Shader(0);
				part.Material = model->Peek_Single_Material(0);
				part.Batch = -1;
				part.Shadow = (m == 1);
				parts.Add(part);
			}
			meshes[m]->Release_Ref();
		}
	}
	first_part.Add(parts.Count());

	// the batches: one per texture / shader / material
	DynamicVectorClass<int> batch_of_kind;
	DynamicVectorClass<DecoPartStruct *> kinds;
	for (int i = 0; i < parts.Count(); i++) {
		for (int k = 0; k < kinds.Count(); k++) {
			if (kinds[k]->Texture == parts[i].Texture && kinds[k]->Material == parts[i].Material
				&& kinds[k]->Shader == parts[i].Shader && kinds[k]->Shadow == parts[i].Shadow) {
				parts[i].Batch = k;
				break;
			}
		}
		if (parts[i].Batch < 0) {
			parts[i].Batch = kinds.Count();
			kinds.Add(&parts[i]);
		}
	}

	// the instances
	Random4Class random(0x1105);
	DecoInstanceStruct *instances = new DecoInstanceStruct[count];
	for (int d = 0; d < count; d++) {
		// VIDTEST 0x415C40's order: the prototype (Pick_Random_Number, no draw for one prototype), the
		// scale, then v before u (the second draw is the sample's first coordinate), the angle
		int p = random(0, protos - 1);
		float scale = min_scale + (max_scale - min_scale) * random.Get_Float();
		float v = random.Get_Float();
		float u = random.Get_Float();
		float angle = random.Get_Float() * 2.0f * WWMATH_PI;
		Vector3 pos;
		Field->Sample(&pos, Vector2(u, v));		// the client's 0x463BF0 = VIDTEST 0x402AE0: the cell's two triangles (VIDTEST 0x403400), not bilinear
		Matrix3D tm(true);
		tm.Set_Translation(pos);
		tm.Rotate_Z(angle);

		// the shadow (VIDTEST 0x416170): the matrix turned so that its z axis follows the ground's
		// normal n, by the quaternion (z x n) sin(a/2), cos(a/2) with a = acos(z.n), not normalised
		// (as VIDTEST's), each column v going to q v q*; lifted 3 units along n
		Matrix3D shadow = tm;
		{
			float spacing = Field->Get_Spacing();
			float half_extent = Field->Get_Extent() * 0.5f;
			int ni = Int_Max(0, Int_Min(Field->Get_Size() - 1, (int)((pos.X + half_extent) / spacing + 0.5f)));
			int nj = Int_Max(0, Int_Min(Field->Get_Size() - 1, (int)((pos.Y + half_extent) / spacing + 0.5f)));
			Vector3 n = Field->Get_Normal(ni, nj);
			Vector3 z(tm[0][2], tm[1][2], tm[2][2]);
			Vector3 axis;
			Vector3::Cross_Product(z, n, &axis);
			if (axis.Length2() > 0.0001f) {
				float a = (float)acos(WWMath::Clamp(Vector3::Dot_Product(z, n), -1.0f, 1.0f)) * 0.5f;
				float w = (float)cos(a);
				Vector3 q = axis * (float)sin(a);
				for (int c = 0; c < 3; c++) {
					Vector3 col(tm[0][c], tm[1][c], tm[2][c]);
					Vector3 t;
					Vector3::Cross_Product(q, col, &t);
					t += col * w;									// the vector part of q v
					float tw = -Vector3::Dot_Product(q, col);		// its scalar part
					Vector3 x;
					Vector3::Cross_Product(t, -q, &x);
					Vector3 r = (-q) * tw + t * w + x;				// (q v) q*
					shadow[0][c] = r.X;
					shadow[1][c] = r.Y;
					shadow[2][c] = r.Z;
				}
			}
			shadow.Set_Translation(pos + n * 3.0f);
		}
		for (int r = 0; r < 3; r++) {
			tm[r][0] *= scale;
			tm[r][1] *= scale;
			tm[r][2] *= scale;
			shadow[r][0] *= scale;
			shadow[r][1] *= scale;
			shadow[r][2] *= scale;
		}
		instances[d].Proto = p;
		instances[d].Transform = tm;
		instances[d].Shadow = shadow;
	}

	// the patch each decoration stands on: VIDTEST keeps them per patch and draws a patch's only while
	// its level (the land's patch record +0x24) is below 3 (0x410B90; the original draws no trees at Land LOD 1, one draw per visible
	// patch at LOD 7: d3d8shot draw dump, M2.4)
	float patch_size = PatchSize * Field->Get_Spacing();
	float half = Field->Get_Extent() * 0.5f;
	int *patch_of = new int[count];
	for (d = 0; d < count; d++) {
		Vector3 t = instances[d].Transform.Get_Translation();
		int px = Int_Max(0, Int_Min(GridWidth - 1, (int)((t.X + half) / patch_size)));
		int py = Int_Max(0, Int_Min(GridWidth - 1, (int)((t.Y + half) / patch_size)));
		patch_of[d] = py * GridWidth + px;
	}

	// fill the buffers, patch by patch, kind by kind
	for (int patch = 0; patch < GridWidth * GridWidth; patch++) {
		for (int k = 0; k < kinds.Count(); k++) {
			int verts = 0, polys = 0;
			for (int d = 0; d < count; d++) {
				if (patch_of[d] != patch) continue;
				for (int i = first_part[instances[d].Proto]; i < first_part[instances[d].Proto + 1]; i++) {
					if (parts[i].Batch != k) continue;
					verts += parts[i].Model->Get_Vertex_Count();
					polys += parts[i].Model->Get_Polygon_Count();
				}
			}
			if (polys > 0 && verts <= 65000 && polys * 3 <= 65000) {
				DecoBatchStruct *batch = new DecoBatchStruct;
				batch->Texture = kinds[k]->Texture;
				if (batch->Texture) batch->Texture->Add_Ref();
				batch->Material = kinds[k]->Material;
				if (batch->Material) batch->Material->Add_Ref();
				// VIDTEST's trees are alpha tested and write depth, without blending (its draws:
				// ALPHATESTENABLE, ALPHAREF 0x60, ZWRITEENABLE, ALPHABLENDENABLE off)
				// (the shadows keep their mesh's shader: blended, no depth writes)
				batch->Shader = kinds[k]->Shader;
				batch->Shadow = kinds[k]->Shadow;
				if (!batch->Shadow) {
					batch->Shader.Set_Alpha_Test(ShaderClass::ALPHATEST_ENABLE);
					batch->Shader.Set_Src_Blend_Func(ShaderClass::SRCBLEND_ONE);
					batch->Shader.Set_Dst_Blend_Func(ShaderClass::DSTBLEND_ZERO);
					batch->Shader.Set_Depth_Mask(ShaderClass::DEPTH_WRITE_ENABLE);
				}
				batch->Vertices = verts;
				batch->Polys = polys;
				batch->Patch = patch;
				batch->VertexBuffer = new DX8VertexBufferClass(DX8_FVF_XYZNUV1, (unsigned short)verts);
				batch->IndexBuffer = new DX8IndexBufferClass((unsigned short)(polys * 3));
				{
					VertexBufferClass::WriteLockClass vlock(batch->VertexBuffer);
					IndexBufferClass::WriteLockClass ilock(batch->IndexBuffer);
					DecoVertexStruct *out = (DecoVertexStruct *)vlock.Get_Vertex_Array();
					unsigned short *idx = ilock.Get_Index_Array();
					int base = 0;
					for (int n = 0; n < count; n++) {
						if (patch_of[n] != patch) continue;
						for (int i = first_part[instances[n].Proto]; i < first_part[instances[n].Proto + 1]; i++) {
							if (parts[i].Batch != k) continue;
							MeshModelClass *model = parts[i].Model;
							Matrix3D tm;
							Matrix3D::Multiply(parts[i].Shadow ? instances[n].Shadow : instances[n].Transform,
								parts[i].Transform, &tm);
							const Vector3 *pos = model->Get_Vertex_Array();
							const Vector3 *nrm = model->Get_Vertex_Normal_Array();
							const Vector2 *uv = model->Get_UV_Array(0, 0);
							int vc = model->Get_Vertex_Count();
							for (int v = 0; v < vc; v++) {
								Vector3 p = tm * pos[v];
								Vector3 q = nrm ? tm.Rotate_Vector(nrm[v]) : Vector3(0.0f, 0.0f, 1.0f);
								q.Normalize();
								out->X = p.X; out->Y = p.Y; out->Z = p.Z;
								out->NX = q.X; out->NY = q.Y; out->NZ = q.Z;
								out->U = uv ? uv[v].U : 0.0f;
								out->V = uv ? uv[v].V : 0.0f;
								out++;
							}
							const TriIndex *tri = model->Get_Polygon_Array();
							int tc = model->Get_Polygon_Count();
							for (int t = 0; t < tc; t++) {
								*idx++ = (unsigned short)(base + tri[t].I);
								*idx++ = (unsigned short)(base + tri[t].J);
								*idx++ = (unsigned short)(base + tri[t].K);
							}
							base += vc;
						}
					}
				}
				DecoBatches.Add(batch);
			}
		}
	}
	delete [] patch_of;
	delete [] instances;
	for (i = 0; i < parts.Count(); i++) {
		parts[i].Model->Release_Ref();
	}
	for (i = 0; i < keep.Count(); i++) {
		keep[i]->Release_Ref();
	}
	return true;
}

void VidtestDecorationsClass::Render(RenderInfoClass &rinfo)
{
	Matrix3D identity(true);
	DX8Wrapper::Set_Transform(D3DTS_WORLD, identity);
	DX8Wrapper::Set_Texture(1, NULL);		// the land leaves its detail map on stage 1 (its own record draws clear it, 0x474B30)
	// the trees of every patch, then their shadows with a depth bias (VIDTEST 0x410B90's two loops;
	// the original's shadow draws: D3DRS_ZBIAS 1)
	for (int pass = 0; pass < 2; pass++) {
		if (pass == 1) DX8Wrapper::Set_DX8_Render_State(D3DRS_ZBIAS, 1);
		for (int i = 0; i < DecoBatches.Count(); i++) {
			DecoBatchStruct *batch = DecoBatches[i];
			if (batch->Shadow != (pass == 1)) continue;
			const LandPatchRecordClass &record = Land->Peek_Patch_Record(batch->Patch);
			if (!record.Visible || (unsigned int)record.Level >= 3) continue;		// as the land's own record draws (0x474B30)
			DX8Wrapper::Set_Material(batch->Material);
			DX8Wrapper::Set_Shader(batch->Shader);
			DX8Wrapper::Set_Texture(0, batch->Texture);
			DX8Wrapper::Set_Vertex_Buffer(batch->VertexBuffer);
			DX8Wrapper::Set_Index_Buffer(batch->IndexBuffer, 0);
			DX8Wrapper::Draw_Triangles(0, (unsigned short)batch->Polys, 0, (unsigned short)batch->Vertices);
		}
	}
	DX8Wrapper::Set_DX8_Render_State(D3DRS_ZBIAS, 0);
}

/*
**	Reconstruction notes
**
**	VIDTEST's own 2002 code (D45, D46): VIDTEST.EXE's land drew its decorations itself (Create_Decorations
**	0x415C40, the prototypes 0x414B50, the batching 0x416170 = the client's 0x47A2F0, drawn by its render
**	0x410B90 per patch while the patch's level is below 3). The client's LandClass draws them from its patch
**	records (+0x18C: per patch a tree buffer pair +0x00 / +0x04 and a shadow pair +0x08 / +0x0C, drawn by
**	Render 0x474B30 with one texture +0x178 and the shader / material pairs +0x17C / +0x180, +0x184 / +0x188),
**	which the zone loader fills (0x478970-0x479D00, called from 0x5137F0), not LandClass. Until that is rebuilt,
**	VIDTEST's decorations are this render object, added to the land scene after the land (D101): the batches
**	vidland.cpp (the former stand-in, 2026-10-27, builder L4) built, drawn while the land's patch record is
**	visible and its level below 3, as the land's own record draws.
**
**	NOT EXACT: written functionally (D42). Kept from VIDTEST: the ini keys and defaults, Random4Class(0x1105)
**	and the order of its draws (prototype, scale, v, u, angle), the trees alpha tested with depth writes, the
**	shadows on the ground's normal, lifted 3 units, drawn with D3DRS_ZBIAS 1.
*/
