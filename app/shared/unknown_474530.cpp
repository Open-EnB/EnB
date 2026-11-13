// Earth & Beyond client (reconstruction): app\shared\ terrain, LandPatchClass, Render and the decorations (file name unknown, interim: D137). Notes at the end.
#define ENABLE_WWPROFILE
#include "land.h"
#include "heightfield.h"
#include "texture.h"
#include "vertmaterial.h"
#include "dx8wrapper.h"
#include "dx8indexbuffer.h"
#include "vector2.h"
#include "surfaceclass.h"
#include "dx8vertexbuffer.h"
#include <windows.h>
#include "wwprofile.h"
#include "wwmath.h"
#include <math.h>
#include <queue>
#include "ini.h"
#include "targa.h"
#include "ffactory.h"
#include "wwfile.h"
#include "assetmgr.h"
#include "textureloader.h"
#include "mesh.h"
#include "meshmdl.h"
#include "ww3d.h"
#include "pot.h"
#include "camera.h"
#include "matrix4.h"
#include "aabox.h"
#include "matrix3d.h"
#include "matpass.h"
#include "obbox.h"
#include "rinfo.h"
#include "dx8fvf.h"
#include "vector.h"
#include "landdmap.h"
#include "texproject.h"
#include "quat.h"
#include "random.h"
#include "matinfo.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern LandUnknown466BE0Class *Unknown_BDC0D8;	// NAME 1: unknown
	// NAME 1: unknown; deletes a global object (0xBDC0D8) and clears it; called by the land's dtor only


// NAME 1: unknown; draws triangles of the land under a projector (0x477E20, its vector by value)
void Unknown_477E20(std::vector<Vector3> verts);

// NAME 2: a quad's four corners at a height into the sea's vertex list. A macro: Render 0x474B30 inlines all 36 Adds and
// calls the DX8 setters at its end out of line; through an inline function the extra level leaves the Adds out of line
#define Add_Sea_Quad(quads, x, y, dx, dy, z) do { \
	float qx_ = (x); float qy_ = (y); float qdx_ = (dx); float qdy_ = (dy); float qz_ = (z); \
	(quads).Add(Vector3(qx_, qy_, qz_)); \
	(quads).Add(Vector3(qx_ + qdx_, qy_, qz_)); \
	(quads).Add(Vector3(qx_ + qdx_, qy_ + qdy_, qz_)); \
	(quads).Add(Vector3(qx_, qy_ + qdy_, qz_)); \
} while (0)

// NOT EXACT: written from the pseudocode (0x474B30); VIDTEST's land scene runs it (D101). The world transforms are the
// client's Set_Transform(D3DTS_WORLD, identity) (its ??_H arrays and the flags' and ~WORLD_IDENTITY | WORLD_CHANGED), not
// Set_World_Identity. Left (inline budget): the patch transform's Matrix3D rows inline (the client constructs them
// through ??_H), the second buffer pass's textures out of line and its shader inline (the client the reverse)
void LandClass::Render(RenderInfoClass &rinfo)
{
	DX8Wrapper::Set_Texture(0, LightmapTextures[FrontLightmap]);
	DX8Wrapper::Set_Texture(1, DetailTexture);
	DX8Wrapper::Set_Shader(Shader);
	DX8Wrapper::Set_Material(Material);
	static DynamicVectorClass<Vector3> sea_quads(10);
	sea_quads.Delete_All();
	bool companion = Unknown_BDC0D8->Unknown11;
	float extent_x = Field->Get_Spacing() * (float)(unsigned int)PatchSize;
	float extent_y = (float)(unsigned int)PatchSize * Field->Get_YSpacing();
	Unknown_BDC0D8->Unknown_466F60(rinfo);
	int level, i;
	for (level = 0; level < MAX_LEVELS; level++) {
		LandPatchClass **patch = Patches;
		for (i = PatchCount; i != 0; i--, patch++) {
			if (!(*patch)->Visible || (*patch)->Level != level) {
				continue;
			}
			if (!companion) {
				Matrix3D tm(Vector3(Field->Get_Spacing(), 0.0f, 0.0f), Vector3(0.0f, Field->Get_YSpacing(), 0.0f),
					Vector3(0.0f, 0.0f, Field->Get_VScale()), Vector3((*patch)->X, (*patch)->Y, (*patch)->Z));
				DX8Wrapper::Set_Transform(D3DTS_WORLD, tm);
				Set_Strip_Buffer(level);
				(*patch)->Draw_Strip();
				if (level != 0 && (*patch)->Quarters != 0) {
					Set_Edge_Buffer(level);
					(*patch)->Draw();
				}
			}
			if (HasSea) {
				Add_Sea_Quad(sea_quads, (*patch)->X, (*patch)->Y, extent_x, extent_y, SeaLevel);
			}
		}
	}

	if (HasSea) {
		// ten rings of sea quads around the field, and the corners
		float half = (float)(Field->Get_Size() >> 1) * Field->Get_Spacing();
		int ring, c;
		for (ring = 0; ring < 10; ring++) {
			for (c = 0; c < GridWidth; c++) {
				Add_Sea_Quad(sea_quads, c * extent_x - half, ring * extent_y + half, extent_x, extent_y, SeaLevel);
				Add_Sea_Quad(sea_quads, ring * extent_x + half, c * extent_y - half, extent_x, extent_y, SeaLevel);
				Add_Sea_Quad(sea_quads, c * extent_x - half, -half - (ring + 1) * extent_y, extent_x, extent_y, SeaLevel);
				Add_Sea_Quad(sea_quads, -half - (ring + 1) * extent_x, c * extent_y - half, extent_x, extent_y, SeaLevel);
			}
			for (c = 0; c < 10; c++) {
				Add_Sea_Quad(sea_quads, (c - 10) * extent_x - half, ring * extent_y + half, extent_x, extent_y, SeaLevel);
				Add_Sea_Quad(sea_quads, ring * extent_x + half, c * extent_y + half, extent_x, extent_y, SeaLevel);
				Add_Sea_Quad(sea_quads, c * extent_x + half, -half - (ring + 1) * extent_y, extent_x, extent_y, SeaLevel);
				Add_Sea_Quad(sea_quads, -half - (ring + 1) * extent_x, -half - (c + 1) * extent_y, extent_x, extent_y, SeaLevel);
			}
		}
		int count = sea_quads.Count();
		int triangles = count / 2;
		DynamicIBAccessClass ib(BUFFER_TYPE_DYNAMIC_DX8, (unsigned short)(triangles * 3));
		{
			DynamicIBAccessClass::WriteLockClass lock(&ib);
			unsigned short *index = lock.Get_Index_Array();
			for (i = 0; i < count / 4; i++) {
				unsigned short base = (unsigned short)(i * 4);
				*index++ = base;
				*index++ = base + 1;
				*index++ = base + 2;
				*index++ = base;
				*index++ = base + 2;
				*index++ = base + 3;
			}
		}
		DX8Wrapper::Set_Index_Buffer(ib, 0);
		DX8Wrapper::Set_Transform(D3DTS_WORLD, Matrix4(true));
		for (int sea = 0; sea < SeaCount; sea++) {
			DynamicVBAccessClass vb(BUFFER_TYPE_DYNAMIC_DX8, dynamic_fvf_type, (unsigned short)count);
			{
				DynamicVBAccessClass::WriteLockClass lock(&vb);
				VertexFormatXYZNDUV2 *vertex = lock.Get_Formatted_Vertex_Array();
				float scale = 1.0f / (float)Field->Get_Size();
				for (i = 0; i < count; i++, vertex++) {
					const Vector3 &pos = sea_quads[i];
					vertex->x = pos.X;
					vertex->y = pos.Y;
					vertex->z = pos.Z;
					vertex->nx = 0.0f;
					vertex->ny = 0.0f;
					vertex->nz = 1.0f;
					float u, v;
					Field->World_To_Grid(Vector2(pos.X, pos.Y), &u, &v);
					u *= scale;
					v *= scale;
					vertex->u1 = u * SeaDetailScales[sea][0];
					vertex->v1 = v * SeaDetailScales[sea][0];
					vertex->u2 = SeaDetailScales[sea][1] * u;
					vertex->v2 = SeaDetailScales[sea][1] * v;
				}
			}
			DX8Wrapper::Set_Vertex_Buffer(vb);
			DX8Wrapper::Set_Texture(0, SeaTextures[sea][0]);
			DX8Wrapper::Set_Texture(1, SeaTextures[sea][1]);
			DX8Wrapper::Set_Shader(SeaShaders[sea]);
			DX8Wrapper::Set_Material(SeaMaterials[sea]);
			DX8Wrapper::Draw_Triangles(0, (unsigned short)triangles, 0, (unsigned short)count);
		}
	}

	// the projectors on the land (+0x1218): their boxes' triangles drawn with their material passes
	static std::vector<Vector3> projected;
	DX8Wrapper::Set_Transform(D3DTS_WORLD, Matrix3D(true));
	for (std::vector< RefCountPtr<TexProjectClass> >::iterator it = Unknown1218.begin(); it != Unknown1218.end(); it++) {
		TexProjectClass *projector = (*it).Peek();
		projected.clear();
		Field->Get_Triangles(projector->Get_Bounding_Volume(), projected);
		projector->Peek_Material_Pass()->Install_Materials();
		Unknown_477E20(projected);
	}

	// the patch records' extra buffers (+0x00 / +0x04 and +0x08 / +0x0C), up to level 2
	if (Unknown192) {
		if (Unknown190) {
			DX8Wrapper::Set_Texture(0, Unknown178.Peek());
			DX8Wrapper::Set_Texture(1, NULL);
			DX8Wrapper::Set_Shader(Unknown17C);
			DX8Wrapper::Set_Material(Unknown180.Peek());
			for (i = 0; i < PatchCount; i++) {
				LandPatchRecordClass &record = PatchRecords[i];
				if (record.Visible && record.Unknown10 > 0 && (unsigned int)record.Level < 3) {
					DX8Wrapper::Set_Vertex_Buffer((VertexBufferClass *)record.Unknown00.Peek());
					DX8Wrapper::Set_Index_Buffer((IndexBufferClass *)record.Unknown04.Peek(), 0);
					DX8Wrapper::Draw_Triangles(0, (unsigned short)(record.Unknown14 / 3), 0, (unsigned short)record.Unknown10);
				}
			}
		}
		if (Unknown191) {
			DX8Wrapper::Set_Texture(0, Unknown178.Peek());
			DX8Wrapper::Set_Texture(1, NULL);
			DX8Wrapper::Set_Shader(Unknown184);
			DX8Wrapper::Set_Material(Unknown188.Peek());
			DX8Wrapper::Set_DX8_Render_State(D3DRS_ZBIAS, 1);
			for (i = 0; i < PatchCount; i++) {
				LandPatchRecordClass &record = PatchRecords[i];
				if (record.Visible && record.Unknown18 > 0 && (unsigned int)record.Level < 3) {
					DX8Wrapper::Set_Vertex_Buffer((VertexBufferClass *)record.Unknown08.Peek());
					DX8Wrapper::Set_Index_Buffer((IndexBufferClass *)record.Unknown0C.Peek(), 0);
					DX8Wrapper::Draw_Triangles(0, (unsigned short)(record.Unknown1C / 3), 0, (unsigned short)record.Unknown18);
				}
			}
			DX8Wrapper::Set_DX8_Render_State(D3DRS_ZBIAS, 0);
		}
	}
}

// 0x477E20: the projected triangles as a dynamic FVF 0x252 list (at most 30000 vertices), raised 3 units where the device
// has no z bias, drawn with z bias 1
void Unknown_477E20(std::vector<Vector3> verts)
{
	int vertex_count = (int)WWMath::Min((float)verts.size(), 30000.0f);
	int polygon_count = vertex_count / 3;
	float z_offset = 0.0f;
	if (!DX8Wrapper::Get_Current_Caps()->Support_ZBias()) {
		z_offset = 3.0f;
	}
	DynamicIBAccessClass ib(BUFFER_TYPE_DYNAMIC_DX8, vertex_count);
	{
		DynamicIBAccessClass::WriteLockClass lock(&ib);
		unsigned short *indices = lock.Get_Index_Array();
		for (int i = 0; i < vertex_count; i++) {
			*indices++ = (unsigned short)i;
		}
	}
	DynamicVBAccessClass vb(BUFFER_TYPE_DYNAMIC_DX8, 0x252, vertex_count);
	{
		DynamicVBAccessClass::WriteLockClass lock(&vb);
		unsigned char *vertices = (unsigned char *)lock.Get_Formatted_Vertex_Array();
		const FVFInfoClass &fi = vb.FVF_Info();
		int n = 0;
		for (std::vector<Vector3>::iterator it = verts.begin(); it != verts.end(); it++) {
			*(Vector3 *)(vertices + fi.Get_Location_Offset()) = Vector3((*it).X, (*it).Y, (*it).Z + z_offset);
			vertices += fi.Get_FVF_Size();
			if (++n >= vertex_count) break;
		}
	}
	DX8Wrapper::Set_Index_Buffer(ib, 0);
	DX8Wrapper::Set_Vertex_Buffer(vb);
	DX8Wrapper::Set_DX8_Render_State(D3DRS_ZBIAS, 1);
	DX8Wrapper::Draw_Triangles(0, (unsigned short)polygon_count, 0, (unsigned short)vertex_count);
	DX8Wrapper::Set_DX8_Render_State(D3DRS_ZBIAS, 0);
}

LandPatchClass::LandPatchClass(LandClass *land) :
	VertexBuffer(NULL),
	Land(land),
	Level(0),
	Quarters(0),
	X(0),
	Y(0),
	Z(0)
{
}

// The client calls it out of line from the land's destructor (0x46A656); defined here it inlines there (the empty-loop
// lever doesn't stop it): auto_inline(off) keeps it out of line
#pragma auto_inline(off)
LandPatchClass::~LandPatchClass(void)
{
	REF_PTR_RELEASE(VertexBuffer);
}
#pragma auto_inline(on)

LandPatchRecordClass::LandPatchRecordClass(void) :
	Unknown10(0),
	Unknown14(0),
	Unknown18(0),
	Unknown1C(0),
	Visible(false),
	Level(0)
{
}

// The client releases the four pointers in the body, then again as members (0x4749D0); with an inline candidate here the
// land's destructor gains three EH states (its delete [] through __ehvec_dtor): auto_inline(off) as the patch's
#pragma auto_inline(off)
LandPatchRecordClass::~LandPatchRecordClass(void)
{
	Unknown00 = NULL;
	Unknown04 = NULL;
	Unknown08 = NULL;
	Unknown0C = NULL;
}
#pragma auto_inline(on)

void LandClass::Set_Strip_Buffer(int level)
{
	DX8Wrapper::Set_Index_Buffer(StripBuffers[level], 0);
}

void LandClass::Set_Edge_Buffer(int level)
{
	DX8Wrapper::Set_Index_Buffer(EdgeBuffers[level], 0);
}

void LandClass::Add_Unknown1218(const RefCountPtr<TexProjectClass> &object)
{
	Unknown1218.push_back(object);
}

void LandClass::Clear_Unknown1218(void)
{
	Unknown1218.clear();
}

#include "unknown_4a1590.h"

// NAME 2: the .dcb's first word (0x4795D0, 0x479A50)
#define LAND_DECORATION_ID			0xDEC0DEC0

// NAME 2: a decoration as the .dcb keeps it, 0x20 bytes (0x479A50): no height, the reader puts it on the ground
struct LandDecorationRecordStruct
{
	unsigned char				Proto;
	Quaternion					Rotation;
	float							X;
	float							Y;
	float							Scale;
};

void LandClass::Load_Prototype_Names(std::vector<std::string> &names, INIClass &ini, const char *section)
{
	int count = ini.Get_Int(section, "NUM_PROTOS", 0);
	for (int i = 0; i < count; i++) {
		char entry[256];
		char name[1024];
		sprintf(entry, "PROTO%d", i);
		ini.Get_String(section, entry, "TDTREE01A", name, sizeof(name));
		names.push_back(std::string(name));
	}
}

// NOT EXACT: frame (identical once the stack offsets are masked: the client has the ini at esp+0x18 and the section
// counter above it at esp+0x28, ours the counter at esp+0x18 and the ini above it. Load_Prototype_Names 0x478970,
// which nothing calls in the client, is inlined here: that is the level that leaves the string's assign and the
// names' insert out of line; attempts.csv 0x478B50)
void LandClass::Load_Decorations_Ini(std::vector<std::string> &names, std::vector<LandDecorationStruct> &decorations, const char *filename)
{
	INIClass ini;
	char section[256];
	ini.Load(filename);
	if (ini.Is_Loaded()) {
		Load_Prototype_Names(names, ini, "Prototypes");
		unsigned int proto_count = names.size();
		if (proto_count != 0) {
			int n = 0;
			sprintf(section, "DECO%d", n);
			while (ini.Find_Section(section) != NULL) {
				LandDecorationStruct decoration;
				decoration.Proto = ini.Get_Int(section, "DecID", 0);
				if ((unsigned int)decoration.Proto >= proto_count) {
					decoration.Proto = 0;
				}
				Matrix3D tm(true);
				char buffer[256];
				ini.Get_String(section, "M", "", buffer, sizeof(buffer));
				char *token = strtok(buffer, ",");
				for (int column = 0; column < 4; column++) {
					for (int row = 0; row < 3; row++) {
						if (token != NULL) {
							tm[row][column] = (float)atof(token);
							token = strtok(NULL, ",");
						}
					}
				}
				decoration.Transform = tm;
				decoration.Scale = ini.Get_Float(section, "Scale", 1.0f);
				decorations.push_back(decoration);
				n++;
				sprintf(section, "DECO%d", n);
			}
		}
	}
}

// 0x4791D0: the prototypes and decorations as an ini (Load_Decorations_Ini's layout; M is the matrix column by column,
// "%f" then ",%f"); nothing when either list is empty
void LandClass::Save_Decorations_Ini(std::vector<std::string> &names, std::vector<LandDecorationStruct> &decorations, const char *filename)
{
	INIClass ini;
	unsigned int proto_count = names.size();
	unsigned int count = decorations.size();
	if (proto_count == 0 || count == 0) {
		return;
	}
	ini.Put_Int("Prototypes", "NUM_PROTOS", proto_count);
	char section[256];
	unsigned int i;
	for (i = 0; i < proto_count; i++) {
		sprintf(section, "PROTO%d", i);
		ini.Put_String("Prototypes", section, names[i].c_str());
	}
	for (i = 0; i < count; i++) {
		sprintf(section, "DECO%d", i);
		LandDecorationStruct &decoration = decorations[i];
		ini.Put_Int(section, "DecID", decoration.Proto);
		const Matrix3D &tm = decoration.Transform;
		std::string m;
		bool first = true;
		for (int column = 0; column < 4; column++) {
			for (int row = 0; row < 3; row++) {
				char buffer[512];
				if (first) {
					first = false;
					sprintf(buffer, "%f", tm[row][column]);
				} else {
					sprintf(buffer, ",%f", tm[row][column]);
				}
				m += buffer;
			}
		}
		ini.Put_String(section, "M", m.c_str());
		ini.Put_Float(section, "Scale", decoration.Scale);
	}
	ini.Save(filename);
}

void LandClass::Load_Decorations(std::vector<std::string> &names, std::vector<LandDecorationStruct> &decorations, const char *filename)
{
	if (Field == NULL) {
		return;
	}
	file_auto_ptr file(_TheFileFactory, filename);
	if (!file->Is_Available()) {
		return;
	}
	file->Open(FileClass::READ);
	unsigned int id;
	if (file->Read(&id, 4) != 4 || id != LAND_DECORATION_ID) {
		return;
	}
	unsigned int count;
	if (file->Read(&count, 4) != 4) {
		return;
	}
	for (id = 0; id < count; id++) {
		char name[32];
		memset(name, 0, sizeof(name));
		std::string string;
		if (file->Read(name, sizeof(name)) != sizeof(name)) {
			return;
		}
		string = name;
		names.push_back(string);
	}
	unsigned int decoration_count;
	if (file->Read(&decoration_count, 4) != 4) {
		return;
	}
	for (id = 0; id < decoration_count; id++) {
		LandDecorationRecordStruct record;
		if (file->Read(&record, sizeof(record)) != sizeof(record)) {
			return;
		}
		Quaternion rotation(record.Rotation.X, record.Rotation.Y, record.Rotation.Z, record.Rotation.W);
		Matrix3D tm = Build_Matrix3D(rotation);
		Vector3 pos;
		pos.X = record.X;
		pos.Y = record.Y;
		pos.Z = Field->Get_Height(pos);
		tm.Set_Translation(pos);
		LandDecorationStruct decoration;
		decoration.Proto = record.Proto;
		if ((unsigned int)decoration.Proto > count) {
			decoration.Proto = 0;
		}
		decoration.Transform = tm;
		decoration.Scale = record.Scale;
		decorations.push_back(decoration);
	}
	file->Close();
}

// NAME 2: a decoration vertex, FVF 0x112 (0x47C6A0; the patch records' buffers, 0x47A2F0); plain floats: 0x47DE10 copies it as one block
struct LandDecorationVertexStruct
{
	float						Position[3];
	float						Normal[3];
	float						UV[2];
};

// NOT EXACT: frame and inline-budget (the client's calls and frame size 0x109C; the slots differ (the decorations at
// esp+0x2C, the client's at +0x34) and the decorations' destructor calls _Destroy where the client's is inline).
// Read from the client: Load_Prototype_Names 0x478970 inlined, the prototype count in a local, a counted loop, the
// transform as an identity Matrix3D turned by Rotate_Z (c + 0, 0 - s, s + 0, c - 0 sign for sign) and copied, the sea
// test through the accessors (docs/attempts/479D00/l20_decode.txt, attempts.csv 0x479D00)
void LandClass::Create_Decorations(int set)
{
	INIClass ini;
	ini.Load("decoration.ini");
	char section[256];
	sprintf(section, "%d", set);
	int count = ini.Get_Int(section, "NUM_DECOS", 0);
	if (count <= 0) {
		return;
	}
	float min_scale = ini.Get_Float(section, "MIN_SCALE", 1.0f);
	float max_scale = ini.Get_Float(section, "MAX_SCALE", 2.0f);
	std::vector<std::string> names;
	Load_Prototype_Names(names, ini, section);
	int proto_count = names.size();
	if (proto_count <= 0) {
		return;
	}
	std::vector<LandDecorationStruct> decorations;
	Random4Class random(0x1105);
	for (int i = 0; i < count; i++) {
		LandDecorationStruct decoration;
		decoration.Proto = Pick_Random_Number(random, 0, proto_count - 1);
		decoration.Scale = random.Get_Float() * (max_scale - min_scale) + min_scale;
		Vector3 pos;
		do {
			Vector2 uv(random.Get_Float(), random.Get_Float());
			pos = Field->Sample(uv);
		} while (Has_Sea() && pos.Z <= Get_Sea_Level());
		float angle = random.Get_Float() * (2.0f * WWMATH_PI);
		Matrix3D tm(true);
		tm.Rotate_Z(angle);
		tm.Set_Translation(pos);
		decoration.Transform = tm;
		decorations.push_back(decoration);
	}
	Build_Decorations(names, decorations);
}

// 0x47C580 (VIDTEST 0x418400): the first mesh under robj, depth first, whose name holds ".d" or ".D" (a tree) or ".s" or
// ".S" (its shadow); add-reffed
static RenderObjClass *Land_Find_Decoration_Mesh(RenderObjClass *robj, bool tree)	// NAME 2: after what it does
{
	RenderObjClass *found = NULL;
	if (robj->Class_ID() == RenderObjClass::CLASSID_MESH) {
		const char *name = robj->Get_Name();
		if (tree) {
			if (name != NULL && (strstr(name, ".d") != NULL || strstr(name, ".D") != NULL)) {
				found = robj;
				robj->Add_Ref();
			}
		} else {
			if (name != NULL && (strstr(name, ".s") != NULL || strstr(name, ".S") != NULL)) {
				found = robj;
				robj->Add_Ref();
			}
		}
	} else {
		int count = robj->Get_Num_Sub_Objects();
		for (int i = 0; i < count; i++) {
			RenderObjClass *sub = robj->Get_Sub_Object(i);
			found = Land_Find_Decoration_Mesh(sub, tree);
			REF_PTR_RELEASE(sub);
			if (found != NULL) {
				break;
			}
		}
	}
	return found;
}

// 0x47C6A0: a mesh's vertices (scaled, then through tm; normals rotated, not normalised; no normals or uvs give zeros) and its
// triangles' indices plus base, appended; returns the largest index the triangles use
// NOT EXACT: order-only (42 masked rows: the client reads tri.J and tri.K into di before the previous maximum's ftol, ours into cx
// after it. The triangle through a reference, the two returns and `polys && points` give the client's inlining: one
// more or fewer comparison against NULL moves the third push_back's _Ucopy in or out of line; attempts.csv 0x47C6A0)
static unsigned short Land_Append_Decoration_Mesh(MeshClass *mesh, std::vector<LandDecorationVertexStruct> &vertices,
	std::vector<unsigned short> &indices, unsigned short base, const Matrix3D &tm, float scale)	// NAME 2: after what it does
{
	unsigned short largest = 0;
	MeshModelClass *model = mesh->Peek_Model();
	const Vector2 *uv = model->Get_UV_Array(0, 0);
	const TriIndex *polys = model->Get_Polygon_Array();
	const Vector3 *points = model->Get_Vertex_Array();
	const Vector3 *normals = model->Get_Vertex_Normal_Array();
	if (polys && points) {
		int i;
		int vertex_count = model->Get_Vertex_Count();
		int poly_count = model->Get_Polygon_Count();
		for (i = 0; i < vertex_count; i++) {
			Vector3 pos = tm * (scale * points[i]);
			LandDecorationVertexStruct vertex;
			vertex.Position[0] = pos.X;
			vertex.Position[1] = pos.Y;
			vertex.Position[2] = pos.Z;
			Vector3 normal(0.0f, 0.0f, 0.0f);
			if (normals != NULL) {
				normal = tm.Rotate_Vector(normals[i]);
			}
			vertex.Normal[0] = normal.X;
			vertex.Normal[1] = normal.Y;
			vertex.Normal[2] = normal.Z;
			Vector2 tex(0.0f, 0.0f);
			if (uv != NULL) {
				tex = uv[i];
			}
			vertex.UV[0] = tex.X;
			vertex.UV[1] = tex.Y;
			vertices.push_back(vertex);
		}
		for (i = 0; i < poly_count; i++) {
			const TriIndex &tri = polys[i];
			largest = (unsigned short)WWMath::Max((float)tri.I, (float)largest);
			indices.push_back((unsigned short)(base + tri.I));
			largest = (unsigned short)WWMath::Max((float)tri.J, (float)largest);
			indices.push_back((unsigned short)(base + tri.J));
			largest = (unsigned short)WWMath::Max((float)tri.K, (float)largest);
			indices.push_back((unsigned short)(base + tri.K));
		}
		return largest;
	}
	return 0;
}

// NOT EXACT: functional, from the disassembly and the pseudocode (0x47A2F0 = VIDTEST 0x416170, 7 KB). The prototypes' tree
// and shadow meshes; the first tree's texture, shader and vertex material (+0x178 / +0x17C / +0x180) and the first shadow's
// shader and material (+0x184 / +0x188), every other prototype's must be the same (else no decorations); each decoration
// appended to its patch's lists (the patch from World_To_Grid over the grid width), the shadow turned onto the ground's
// normal (the quaternion from z to the normal, its half angle not normalised) and lifted 3 units along it; then the patch
// records' vertex and index buffers. Nothing when the height field wraps. The client sets a global byte 0xBFAE52 while it
// loads the prototypes when the land's +0x135 is set (not rebuilt).
void LandClass::Build_Decorations(std::vector<std::string> &names, std::vector<LandDecorationStruct> &decorations)
{
	int count = decorations.size();
	if (count < 1) {
		return;
	}
	int proto_count = names.size();
	Unknown190 = false;
	Unknown191 = false;
	if (Field->Is_Wrapping()) {
		return;
	}
	std::vector<MeshClass *> trees;
	std::vector<MeshClass *> shadows;
	int i;
	for (i = 0; i < proto_count; i++) {
		RenderObjClass *robj = WW3DAssetManager::Get_Instance()->Create_Render_Obj(names[i].c_str());
		if (robj == NULL) {
			for (int k = 0; k < (int)trees.size(); k++) {
				if (trees[k] != NULL) trees[k]->Release_Ref();
				if (shadows[k] != NULL) shadows[k]->Release_Ref();
			}
			return;
		}
		trees.push_back((MeshClass *)Land_Find_Decoration_Mesh(robj, true));
		shadows.push_back((MeshClass *)Land_Find_Decoration_Mesh(robj, false));
		robj->Release_Ref();
	}
	bool has_trees = false;
	bool has_shadows = false;
	for (i = 0; i < proto_count; i++) {
		if (trees[i] != NULL) {
			MaterialInfoClass *info = trees[i]->Get_Material_Info();
			TextureClass *texture = info->Get_Texture(0);
			Unknown178 = Create_Get(texture);
			Unknown17C = trees[i]->Peek_Model()->Get_Shader(0, 0);
			Unknown180 = Create_Get(info->Get_Vertex_Material(0));
			has_trees = true;
			info->Release_Ref();
			break;
		}
	}
	for (i = 0; i < proto_count; i++) {
		if (shadows[i] != NULL) {
			MaterialInfoClass *info = shadows[i]->Get_Material_Info();
			Unknown184 = shadows[i]->Peek_Model()->Get_Shader(0, 0);
			Unknown188 = Create_Get(info->Get_Vertex_Material(0));
			has_shadows = true;
			info->Release_Ref();
			break;
		}
	}
	bool same = true;
	for (i = 0; i < proto_count && same; i++) {
		if (has_trees && trees[i] != NULL) {
			MaterialInfoClass *info = trees[i]->Get_Material_Info();
			TextureClass *texture = info->Get_Texture(0);
			ShaderClass shader = trees[i]->Peek_Model()->Get_Shader(0, 0);
			VertexMaterialClass *material = info->Peek_Vertex_Material(0);
			if (texture != Unknown178.Peek() || !(shader == Unknown17C)
				|| material->Get_CRC() != (Unknown180.Peek())->Get_CRC()) {
				same = false;
			}
			REF_PTR_RELEASE(texture);
			info->Release_Ref();
		}
		if (same && has_shadows && shadows[i] != NULL) {
			MaterialInfoClass *info = shadows[i]->Get_Material_Info();
			TextureClass *texture = info->Get_Texture(0);
			ShaderClass shader = shadows[i]->Peek_Model()->Get_Shader(0, 0);
			VertexMaterialClass *material = info->Peek_Vertex_Material(0);
			if (texture != Unknown178.Peek() || !(shader == Unknown184)
				|| material->Get_CRC() != (Unknown188.Peek())->Get_CRC()) {
				same = false;
			}
			REF_PTR_RELEASE(texture);
			info->Release_Ref();
		}
	}
	if (same && (has_trees || has_shadows)) {
		Unknown190 = has_trees;
		Unknown191 = has_shadows;
		std::vector<LandDecorationVertexStruct> *tree_vertices = new std::vector<LandDecorationVertexStruct>[PatchCount];
		std::vector<unsigned short> *tree_indices = new std::vector<unsigned short>[PatchCount];
		std::vector<LandDecorationVertexStruct> *shadow_vertices = new std::vector<LandDecorationVertexStruct>[PatchCount];
		std::vector<unsigned short> *shadow_indices = new std::vector<unsigned short>[PatchCount];
		unsigned short *tree_counts = new unsigned short[PatchCount];
		unsigned short *shadow_counts = new unsigned short[PatchCount];
		memset(tree_counts, 0, PatchCount * sizeof(unsigned short));
		memset(shadow_counts, 0, PatchCount * sizeof(unsigned short));
		for (int d = 0; d < count; d++) {
			const LandDecorationStruct &decoration = decorations[d];
			Matrix3D tm = decoration.Transform;
			Vector3 pos = tm.Get_Translation();
			float gi, gj;
			Field->World_To_Grid(Vector2(pos.X, pos.Y), &gi, &gj);
			int patch = GridWidth * ((int)floor(gj / (float)(unsigned int)GridWidth) & GridMask)
				+ ((int)floor(gi / (float)(unsigned int)GridWidth) & GridMask);
			if (Unknown190 && trees[decoration.Proto] != NULL) {
				unsigned short largest = Land_Append_Decoration_Mesh(trees[decoration.Proto], tree_vertices[patch], tree_indices[patch],
					tree_counts[patch], tm, decoration.Scale);
				tree_counts[patch] += largest + 1;
			}
			if (Unknown191 && shadows[decoration.Proto] != NULL) {
				Matrix3D shadow = tm;
				Vector3 normal;
				Field->Get_Normal(&normal, Vector2(pos.X, pos.Y));
				Vector3 translation = pos + normal * 3.0f;
				Vector3 z(tm[0][2], tm[1][2], tm[2][2]);
				Vector3 axis;
				Vector3::Cross_Product(z, normal, &axis);
				if (axis.Length2() > 0.0001f) {
					float half = (float)acos(Vector3::Dot_Product(z, normal)) * 0.5f;
					float w = (float)cos(half);
					Vector3 q = axis * (float)sin(half);
					for (int c = 0; c < 3; c++) {
						Vector3 col(tm[0][c], tm[1][c], tm[2][c]);
						Vector3 t;
						Vector3::Cross_Product(q, col, &t);
						t += col * w;
						float tw = -Vector3::Dot_Product(q, col);
						Vector3 x;
						Vector3::Cross_Product(t, -q, &x);
						Vector3 r = (-q) * tw + t * w + x;
						shadow[0][c] = r.X;
						shadow[1][c] = r.Y;
						shadow[2][c] = r.Z;
					}
				}
				shadow.Set_Translation(translation);
				unsigned short largest = Land_Append_Decoration_Mesh(shadows[decoration.Proto], shadow_vertices[patch], shadow_indices[patch],
					shadow_counts[patch], shadow, decoration.Scale);
				shadow_counts[patch] += largest + 1;
			}
		}
		for (int p = 0; p < PatchCount; p++) {
			LandPatchRecordClass &record = PatchRecords[p];
			for (int pass = 0; pass < 2; pass++) {
				std::vector<LandDecorationVertexStruct> &verts = pass ? shadow_vertices[p] : tree_vertices[p];
				std::vector<unsigned short> &idx = pass ? shadow_indices[p] : tree_indices[p];
				RefCountPtr<RefCountClass> &vb = pass ? record.Unknown08 : record.Unknown00;
				RefCountPtr<RefCountClass> &ib = pass ? record.Unknown0C : record.Unknown04;
				int &vertex_count = pass ? record.Unknown18 : record.Unknown10;
				int &index_count = pass ? record.Unknown1C : record.Unknown14;
				int nv = verts.size();
				int ni = idx.size();
				if (nv < 1) {
					vertex_count = 0;
					index_count = 0;
					vb = NULL;
					ib = NULL;
					continue;
				}
				vertex_count = nv;
				index_count = ni;
				DX8VertexBufferClass *vertex_buffer = new DX8VertexBufferClass(0x112, (unsigned short)nv);
				vb = Create_NEW(vertex_buffer);
				DX8IndexBufferClass *index_buffer = new DX8IndexBufferClass((unsigned short)ni);
				ib = Create_NEW(index_buffer);
				{
					VertexBufferClass::WriteLockClass lock(vertex_buffer);
					LandDecorationVertexStruct *out = (LandDecorationVertexStruct *)lock.Get_Vertex_Array();
					for (int v = 0; v < nv; v++) {
						out[v] = verts[v];
					}
				}
				{
					IndexBufferClass::WriteLockClass lock(index_buffer);
					unsigned short *out = lock.Get_Index_Array();
					for (int n = 0; n < ni; n++) {
						out[n] = idx[n];
					}
				}
			}
		}
		delete [] tree_vertices;
		delete [] tree_indices;
		delete [] shadow_vertices;
		delete [] shadow_indices;
		delete [] tree_counts;
		delete [] shadow_counts;
	}
	for (i = 0; i < (int)trees.size(); i++) {
		if (trees[i] != NULL) trees[i]->Release_Ref();
		if (shadows[i] != NULL) shadows[i]->Release_Ref();
	}
}

// The decorations as the .dec file: the prototype names (32 bytes each), then per decoration its record (the translation through
// Get_Translation's out-parameter form, as the client's frame shows)
void LandClass::Save_Decorations(std::vector<std::string> &names, std::vector<LandDecorationStruct> &decorations, const char *filename)
{
	unsigned int count = names.size();
	unsigned int decoration_count = decorations.size();
	if (count == 0 || decoration_count == 0 || count > 255) {
		return;
	}
	file_auto_ptr file(_TheWritingFileFactory, filename);
	file->Open(FileClass::WRITE);
	unsigned int id = LAND_DECORATION_ID;
	file->Write(&id, 4);
	file->Write(&count, 4);
	for (id = 0; id < count; id++) {
		char name[32];
		memset(name, 0, sizeof(name));
		Bounded_Strcpy(name, sizeof(name), names[id].c_str());
		file->Write(name, sizeof(name));
	}
	file->Write(&decoration_count, 4);
	for (id = 0; id < decoration_count; id++) {
		LandDecorationRecordStruct record;
		record.Proto = (unsigned char)decorations[id].Proto;
		Quaternion rotation = Build_Quaternion(decorations[id].Transform);
		Vector3 pos;
		decorations[id].Transform.Get_Translation(&pos);
		record.Rotation = rotation;
		record.X = pos.X;
		record.Y = pos.Y;
		record.Scale = decorations[id].Scale;
		file->Write(&record, sizeof(record));
	}
	file->Close();
}

// NAME 2: the patch vertex, FVF 0x202 (0x4746C0)
struct LandVertexStruct
{
	float	X;
	float	Y;
	float	Z;
	float	U1;
	float	V1;
	float	U2;
	float	V2;
};

void LandPatchClass::Init(int size, int x, int y, float detail_u, float detail_v)
{
	Size = size;
	VertexCount = size * size;
	VertexBuffer = new DX8VertexBufferClass(0x202, VertexCount);
	Build_Vertices(x, y, detail_u, detail_v);
}

// NOT EXACT: order-only (7 masked rows: the client sets xi = x after the inner loop's Size test, ours before; the slots of inv_size / v and i / xi swapped; attempts.csv 0x474790)
void LandPatchClass::Build_Vertices(int x, int y, float detail_u, float detail_v)
{
	float lowest = 2.0f;
	float highest = -1.0f;
	float inv_size = 1.0f / (float)(unsigned int)Land->Field->Get_Size();
	float vscale = Land->Field->Get_VScale();
	float height_scale = 1.0f / vscale;
	{
		VertexBufferClass::WriteLockClass lock(VertexBuffer);
		LandVertexStruct *vertex = (LandVertexStruct *)lock.Get_Vertex_Array();
		for (int j = 0; j < Size; j++) {
			int xi = x;
			for (int i = 0; i < Size; i++) {
				vertex->X = (float)i;
				vertex->Y = (float)j;
				float height = Land->Field->Get_Height_2(xi, j + y) * height_scale;
				if (lowest >= height) lowest = height;
				if (highest <= height) highest = height;
				vertex->Z = height;
				float v = (float)(j + y) * inv_size;
				vertex->V1 = v;
				vertex->V2 = v * detail_v;
				float u = (float)xi * inv_size;
				vertex->U1 = u;
				vertex->U2 = u * detail_u;
				vertex++;
				xi++;
			}
		}
	}
	MidHeight = (highest + lowest) * 0.5f;
}

void LandPatchClass::Draw_Strip(void)
{
	DX8Wrapper::Set_Vertex_Buffer(VertexBuffer);
	DX8Wrapper::Draw_Strip(0, (unsigned short)Land->StripCounts[Level], 0, (unsigned short)VertexCount);
}

void LandPatchClass::Draw(void)
{
	int quarter = Land->EdgeCounts[Level] >> 2;
	if (Quarters & 1) DX8Wrapper::Draw_Triangles(0, quarter / 3, 0, VertexCount);
	if (Quarters & 2) DX8Wrapper::Draw_Triangles(quarter, quarter / 3, 0, VertexCount);
	if (Quarters & 4) DX8Wrapper::Draw_Triangles(quarter * 2, quarter / 3, 0, VertexCount);
	if (Quarters & 8) DX8Wrapper::Draw_Triangles(quarter * 3, quarter / 3, 0, VertexCount);
}

// The strips run forward and back on alternate rows (an int flag); the edge buffers' fans per side
void LandClass::Build_Index_Buffers(int size)
{
	int level = MAX_LEVELS - 1;
	do {
		int step = 1 << level;
		int quads = (size - 1) / step;
		StripCounts[level] = (quads + 2) * quads * 2 - 2;
		StripBuffers[level] = new DX8IndexBufferClass(StripCounts[level] + 2);
		{
			IndexBufferClass::WriteLockClass lock(StripBuffers[level]);
			unsigned short *index = lock.Get_Index_Array();
			int forward = 1;
			for (int row = step; row < size; row += step) {
				if (forward) {
					for (int col = 0; col < size - step; col += step) {
						*index++ = row * size + col;
						*index++ = col + (row - step) * size;
					}
					*index++ = (row + 1) * size - 1;
					*index++ = (row - step + 1) * size - 1;
					*index++ = (row + 1) * size - 1;
					*index++ = (row + 1) * size - 1;
				} else {
					*index++ = (row - step + 1) * size - 1;
					*index++ = (row + 1) * size - 1;
					unsigned short corner = 0;
					for (int col = size - 1; col > 0; col -= step) {
						*index++ = (row - step) * size - step + col;
						corner = col + (row * size - step);
						*index++ = corner;
					}
					*index++ = corner;
					*index++ = corner;
				}
				forward = !forward;
			}
		}
		if (level != 0) {
			EdgeCounts[level] = quads * 12;
			EdgeBuffers[level] = new DX8IndexBufferClass(EdgeCounts[level]);
			IndexBufferClass::WriteLockClass lock(EdgeBuffers[level]);
			unsigned short *index = lock.Get_Index_Array();
			int i;
			for (i = step; i <= size; i += step) {
				*index++ = (i - step) * size;
				*index++ = i * size;
				*index++ = (i - (step >> 1)) * size;
			}
			for (i = step; i < size; i += step) {
				*index++ = (i - step + 1) * size - 1;
				*index++ = (i - (step >> 1) + 1) * size - 1;
				*index++ = (i + 1) * size - 1;
			}
			for (i = step; i < size; i += step) {
				*index++ = i - step;
				*index++ = i - (step >> 1);
				*index++ = i;
			}
			for (i = step; i < size; i += step) {
				*index++ = (size - 1) * size - step + i;
				*index++ = i + (size - 1) * size;
				*index++ = i + ((size - 1) * size - (step >> 1));
			}
		}
	} while (level-- != 0);
}

/*
**	RECONSTRUCTED: LandPatchClass, Render and the decorations (client 0x474530-0x47F520), the third object of the terrain's code:
**	land.cpp's end-of-file notes describe the three and the evidence for the split.
*/
