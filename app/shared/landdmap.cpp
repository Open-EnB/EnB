// Earth & Beyond client (reconstruction): app\shared\ the land's displacement-mapped companion, client 0x466BE0-0x468E70
// (file name unknown: NAME 1). Notes at the end.
#include "landdmap.h"
#include "heightfield.h"
#include "registry.h"
#include "camera.h"
#include "rinfo.h"
#include "matrix3d.h"
#include <d3dx8.h>

LandUnknown466BE0Class *Unknown_BDC0D8;	// NAME 1: unknown

// NAME 1: unknown; the displacement vertex shader (0xB71650)
static const char Unknown_B71650[] =
	"\n"
	"\n"
	"\n"
	"vs.1.1\n"
	"\n"
	"\n"
	"\n"
	"\n"
	"\n"
	"\n"
	"\n"
	"\n"
	"\n"
	"\n"
	"\n"
	"\n"
	"\n"
	"\n"
	"\n"
	"\n"
	"\n"
	"\n"
	"\n"
	"\n"
	"\n"
	"\n"
	"\n"
	"\n"
	"\n"
	"mov r0, v0\n"
	"\n"
	"mov r2, v3\n"
	"dp3 r2.w,r2,r2\n"
	"rsq r2.w,r2.w\n"
	"mul r2,r2, r2.w\n"
	"\n"
	"mul r1, r2, c5.x\n"
	"mov r3.x,v3.w\n"
	"\n"
	"mul r1.xyz, r1.xyz, r3.x\n"
	"add r0.xyz, r0.xyz, r1.xyz\n"
	"\n"
	"m4x4 r1, r0, c6\n"
	"mov oPos, r1\n"
	"\n"
	"mov oT0.xy, v7.xy\n"
	"mov oT1.xy, v8.xy\n"
	"\n"
	"add r1, r1, -c10.x\t\n"
	"rcp r1.y, c10.z\t\n"
	"\n"
	"\n"
	"mad oFog.x, -r1.z, r1.y, c11.y\n"
	"";


LandUnknown466BE0Class::LandUnknown466BE0Class(HeightFieldClass *field, float detail_u, float detail_v) :
	Unknown04(NULL),
	Enabled(false),
	Unknown11(true),
	Unknown12(false),
	Field(field),
	TessellationRate(80000.0f),
	DisplacementMap(NULL),
	HeightTexture(NULL)
{
	Read_Registry();
	if (Enabled && DX8Wrapper::Get_Current_Adapter_Identifier().VendorId == 0x102B
		&& DX8Wrapper::Get_Current_Caps()->Get_Pixel_Shader_Major_Version() == 1
		&& DX8Wrapper::Get_Current_Caps()->Get_Pixel_Shader_Minor_Version() == 3) {
		Enabled = true;
	} else {
		Enabled = false;
	}
	Unknown0C = 1;
	if (Enabled) {
		Create_Displacement_Map();
		Create_Height_Texture();
		Unknown_4677B0(Field);
		Unknown_467910();
		Unknown_467A50(detail_u, detail_v);
		Unknown_467E40();
		return;
	}
	Unknown11 = false;
}

LandUnknown466BE0Class::~LandUnknown466BE0Class(void)
{
	if (Enabled) {
		Release_Displacement_Map();
		Release_Height_Texture();
		if (VertexShader != 0) {
			DX8CALL(DeleteVertexShader(VertexShader));
			VertexShader = 0;
		}
		delete [] Unknown04;
		Unknown04 = NULL;
	}
}

void Unknown_466E70(HeightFieldClass *field, float detail_u, float detail_v)
{
	Unknown_BDC0D8 = new LandUnknown466BE0Class(field, detail_u, detail_v);
}

void Unknown_466F10(void)
{
	if (Unknown_BDC0D8 != NULL) {
		delete Unknown_BDC0D8;
	}
	Unknown_BDC0D8 = NULL;
}

void LandUnknown466BE0Class::Unknown_466F40(CameraClass &camera, int lod, const Matrix4 &scale)
{
}

void LandUnknown466BE0Class::Read_Registry(void)
{
	RegistryClass registry("Software\\Westwood Studios\\Earth and Beyond\\Render\\DMap");
	if (registry.Is_Valid()) {
		Enabled = registry.Get_Int("DisplacementMappingEnabled", 1) != 0;
		BaseTessellationSize = registry.Get_Int("BaseTessellationSize", 16);
		DisplacementMapSize = registry.Get_Int("DisplacementMapSize", 256);
		TessellationRate = (float)registry.Get_Int("TessellationRate", 80000);
	} else {
		Enabled = true;
		BaseTessellationSize = 16;
		DisplacementMapSize = 256;
		TessellationRate = 80000.0f;
	}
}


// NOT EXACT: functional, from the disassembly and pseudocode of 0x466F60
void LandUnknown466BE0Class::Unknown_466F60(RenderInfoClass &rinfo)
{
	if (!Unknown11) {
		return;
	}
	IDirect3DDevice8 *device = DX8Wrapper::_Get_D3D_Device8();
	float size = (float)Field->Get_Size();
	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, Field->Get_Spacing() * size, Field->Get_YSpacing() * size, Field->Get_VScale() * size);

	D3DXMATRIX proj;
	D3DXMATRIX view;
	D3DXMATRIX world;
	DX8Wrapper::_Get_D3D_Device8()->GetTransform(D3DTS_PROJECTION, &proj);
	DX8Wrapper::_Get_D3D_Device8()->GetTransform(D3DTS_VIEW, &view);
	DX8Wrapper::_Get_D3D_Device8()->GetTransform(D3DTS_WORLD, &world);
	DX8Wrapper::Apply_Render_State_Changes();

	// the camera's view as a D3D matrix (row vectors); the client builds it in the same local it read D3DTS_VIEW into,
	// so the "restore" at the end sets this matrix, not the one read back
	Matrix3D camview = rinfo.Camera.Get_View_Matrix();
	for (int r = 0; r < 4; r++) {
		for (int c = 0; c < 3; c++) {
			view.m[r][c] = camview[c][r];
		}
	}
	view.m[0][3] = 0.0f;
	view.m[1][3] = 0.0f;
	view.m[2][3] = 0.0f;
	view.m[3][3] = 1.0f;

	D3DXMATRIX flip(1.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 1.0f, 0.0f, 0.0f,
						0.0f, 0.0f, -1.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 1.0f);
	D3DXMATRIX wvp;
	D3DXMatrixMultiply(&wvp, &scale, &view);
	D3DXMatrixMultiply(&wvp, &wvp, &proj);
	D3DXMatrixTranspose(&wvp, &wvp);
	D3DXMATRIX d3dview;
	D3DXMatrixMultiply(&d3dview, &view, &flip);
	device->SetTransform(D3DTS_VIEW, &d3dview);
	device->SetVertexShaderConstant(6, &wvp, 4);

	DWORD fog;
	device->GetRenderState(D3DRS_FOGENABLE, &fog);
	device->GetRenderState(D3DRS_FOGSTART, (DWORD *)&FogStart);
	device->GetRenderState(D3DRS_FOGEND, (DWORD *)&FogEnd);
	FogRange = FogEnd - FogStart;
	device->SetVertexShaderConstant(10, &FogStart, 1);

	// the client's is a function-local static with a destructor registered through atexit (0x4676B0; guard 0xBDC0C0,
	// data 0xBDC0C8); D3DXVECTOR4 has none, so this registers nothing
	static D3DXVECTOR4 consts(0.0f, 1.0f, 0.5f, 2.0f);
	device->SetVertexShaderConstant(11, &consts, 1);

	if (Unknown0C) {
		Unknown_468240(rinfo.Camera, scale);
	}
	Unknown_468D50();

	float tess[4];
	tess[0] = TessellationRate;
	tess[1] = 0.0f;
	tess[2] = 32.0f;
	*(DWORD *)&tess[3] = 1;	// the client stores the integer 1, not 1.0f
	device->SetVertexShaderConstant(4, tess, 1);
	D3DXVECTOR4 c5(1.0f, 0.0f, 0.0f, 0.0f);
	device->SetVertexShaderConstant(5, &c5, 1);

	device->SetTextureStageState(4, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	device->SetTextureStageState(4, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	device->SetTextureStageState(4, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
	device->SetTextureStageState(4, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	device->SetTextureStageState(4, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	device->SetTexture(4, DisplacementMap);
	device->SetVertexShader(VertexShader);
	if (Unknown12) {
		device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}
	device->SetStreamSource(0, VertexBuffer, 0x34);
	device->SetIndices(IndexBuffer, 0);
	device->SetTransform(D3DTS_WORLD, &scale);
	DX8Wrapper::_Get_D3D_Device8()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, VertexCount, 0, IndexCount / 3);
	if (Unknown12) {
		DX8Wrapper::_Get_D3D_Device8()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}
	DX8Wrapper::_Get_D3D_Device8()->SetTransform(D3DTS_VIEW, &view);
	DX8Wrapper::_Get_D3D_Device8()->SetTransform(D3DTS_WORLD, &world);
}

void LandUnknown466BE0Class::Create_Displacement_Map(void)
{
	DX8CALL(CreateTexture(DisplacementMapSize, DisplacementMapSize, 0, 0, (D3DFORMAT)0x102B0001, D3DPOOL_DEFAULT, &DisplacementMap));
}

void LandUnknown466BE0Class::Create_Height_Texture(void)
{
	DX8_ErrorCode(D3DXCreateTexture(DX8Wrapper::_Get_D3D_Device8(), DisplacementMapSize, DisplacementMapSize, 0, 0, D3DFMT_L8, D3DPOOL_MANAGED, &HeightTexture));
}

void LandUnknown466BE0Class::Release_Displacement_Map(void)
{
	if (DisplacementMap != NULL) {
		DisplacementMap->Release();
		DisplacementMap = NULL;
	}
}

void LandUnknown466BE0Class::Release_Height_Texture(void)
{
	if (HeightTexture != NULL) {
		HeightTexture->Release();
		HeightTexture = NULL;
	}
}

void LandUnknown466BE0Class::Unknown_4677B0(HeightFieldClass *field)
{
	int size = field->Get_Size();
	D3DSURFACE_DESC desc;
	DX8_ErrorCode(HeightTexture->GetLevelDesc(0, &desc));
	float step = (float)size;
	step /= desc.Width;
	D3DLOCKED_RECT rect;
	HeightTexture->LockRect(0, &rect, NULL, 0);
	unsigned char *bits = (unsigned char *)rect.pBits;
	for (unsigned int y = 0; y < desc.Height; y++) {
		for (unsigned int x = 0; x < desc.Width; x++) {
			bits[rect.Pitch * y + x] = field->Get_Height_Byte((int)(x * step), (int)(y * step));
		}
	}
	DX8_ErrorCode(HeightTexture->UnlockRect(0));
	DX8_ErrorCode(D3DXFilterTexture(HeightTexture, NULL, 0, D3DX_FILTER_BOX));
}

void LandUnknown466BE0Class::Unknown_467910(void)
{
	int levels = HeightTexture->GetLevelCount();
	for (int level = 0; level < levels; level++) {
		D3DSURFACE_DESC desc;
		DX8_ErrorCode(HeightTexture->GetLevelDesc(level, &desc));
		D3DLOCKED_RECT src;
		D3DLOCKED_RECT dst;
		HeightTexture->LockRect(level, &src, NULL, 0);
		DisplacementMap->LockRect(level, &dst, NULL, 0);
		unsigned char *from = (unsigned char *)src.pBits;
		unsigned char *to = (unsigned char *)dst.pBits;
		for (unsigned int y = 0; y < desc.Height; y++) {
			for (unsigned int x = 0; x < desc.Width; x++) {
				to[dst.Pitch * y + x] = from[src.Pitch * y + x];
			}
		}
		DX8_ErrorCode(DisplacementMap->UnlockRect(level));
		DX8_ErrorCode(HeightTexture->UnlockRect(level));
	}
}

// NOT EXACT: functional, from the disassembly and pseudocode of 0x467A50
void LandUnknown466BE0Class::Unknown_467A50(float detail_u, float detail_v)
{
	int size = BaseTessellationSize;
	IndexCount = (size * 6 - 6) * (size - 1);
	VertexCount = size * size;
	DX8Wrapper::_Get_D3D_Device8()->CreateIndexBuffer(IndexCount * 2, D3DUSAGE_WRITEONLY | D3DUSAGE_NPATCHES | D3DUSAGE_DYNAMIC,
		D3DFMT_INDEX16, D3DPOOL_DEFAULT, &IndexBuffer);
	unsigned short *indices;
	IndexBuffer->Lock(0, IndexCount * 2, (BYTE **)&indices, 0);
	unsigned int y;
	for (y = 1; y < (unsigned int)BaseTessellationSize; y++) {
		for (unsigned int x = 1; x < (unsigned int)BaseTessellationSize; x++) {
			unsigned short n = (unsigned short)BaseTessellationSize;
			indices[0] = (unsigned short)((y - 1) * n + x);
			indices[1] = (unsigned short)(y * n + x - 1);
			indices[2] = (unsigned short)((y - 1) * n + x - 1);
			indices[3] = (unsigned short)(y * n + x);
			indices[4] = (unsigned short)(y * n + x - 1);
			indices[5] = (unsigned short)((y - 1) * n + x);
			indices += 6;
		}
	}
	IndexBuffer->Unlock();

	// 0x34-byte vertices, no FVF (the vertex shader's declaration: position, float4 at v3, float4 at v7, float2 at v8)
	DX8Wrapper::_Get_D3D_Device8()->CreateVertexBuffer(VertexCount * 0x34, D3DUSAGE_WRITEONLY | D3DUSAGE_NPATCHES | D3DUSAGE_DYNAMIC,
		0, D3DPOOL_DEFAULT, &VertexBuffer);
	float *v;
	VertexBuffer->Lock(0, VertexCount * 0x34, (BYTE **)&v, 0);
	float last = (float)(unsigned int)BaseTessellationSize - 1.0f;
	for (y = 0; y < (unsigned int)BaseTessellationSize; y++) {
		float fv = (float)y / last;
		for (unsigned int x = 0; x < (unsigned int)BaseTessellationSize; x++) {
			float fu = (float)x / last;
			v[0] = fu - 0.5f;
			v[1] = fv - 0.5f;
			v[2] = 0.0f;
			v[3] = 0.0f;
			v[4] = 0.0f;
			v[5] = 1.0f;
			v[6] = 0.0f;
			v[7] = fu;
			v[8] = fv;
			v[9] = 1.0f;
			v[10] = 1.0f;
			v[11] = fu * detail_u;
			v[12] = fv * detail_v;
			v += 13;
		}
	}
	VertexBuffer->Unlock();
	delete [] Unknown04;
	Unknown04 = NULL;
	Unknown_468920();
}

// NOT EXACT: functional, from the disassembly and pseudocode of 0x467E40
void LandUnknown466BE0Class::Unknown_467E40(void)
{
	// .rdata 0xB71650: the vertex shader's source (read with strlen; the text itself is in the client, not copied here)
	LPD3DXBUFFER shader = NULL;
	LPD3DXBUFFER errors = NULL;
	DWORD decl[] = {
		0x88000000,	// D3DVSD_CONST(0, 4), then four constants as raw bits
		0x7FF4614D, 0x7FF86F72, 0x7FF47845, 0x7FD02E31,
		0x7FFF0001, 0x00000001, 0x00000011, 0x00000000,
		0x7FFF0002, 0x7FFE0001, 0x00000004, 0x00000000,
		0xFFF4614D, 0xFFF86F72, 0xFFF47845, 0xFFD02E31,
		D3DVSD_STREAM(0),
		D3DVSD_REG(0, D3DVSDT_FLOAT3),
		D3DVSD_REG(3, D3DVSDT_FLOAT4),
		D3DVSD_REG(7, D3DVSDT_FLOAT4),
		D3DVSD_REG(8, D3DVSDT_FLOAT2),
		D3DVSD_END()
	};
	if (D3DXAssembleShader(Unknown_B71650, strlen(Unknown_B71650), 0, NULL, &shader, &errors) != D3D_OK) {
		OutputDebugString((const char *)errors->GetBufferPointer());	// import 0xC43E54 (unnamed in client.py; assumed OutputDebugStringA)
	}
	DX8Wrapper::_Get_D3D_Device8()->CreateVertexShader(decl, (DWORD *)shader->GetBufferPointer(), &VertexShader, 0);
	if (shader != NULL) {
		shader->Release();
	}
}

// NOT EXACT: functional, from the disassembly and pseudocode of 0x468240
void LandUnknown466BE0Class::Unknown_468240(CameraClass &camera, const D3DXMATRIX &scale)
{
	D3DXMATRIX proj;
	D3DXMATRIX view;
	DX8Wrapper::_Get_D3D_Device8()->GetTransform(D3DTS_PROJECTION, &proj);
	DX8Wrapper::_Get_D3D_Device8()->GetTransform(D3DTS_VIEW, &view);
	D3DXMATRIX flip(1.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 1.0f, 0.0f, 0.0f,
						0.0f, 0.0f, -1.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 1.0f);
	D3DXMatrixMultiply(&view, &view, &flip);
	D3DXMATRIX wv;
	D3DXMatrixMultiply(&wv, &scale, &view);
	D3DXMATRIX m;
	D3DXMatrixMultiply(&m, &wv, &proj);
	D3DXMatrixTranspose(&m, &m);
	Planes[0] = -D3DXPLANE(m._41 - m._11, m._42 - m._12, m._43 - m._13, m._44 - m._14);
	Planes[1] = -D3DXPLANE(m._41 + m._11, m._42 + m._12, m._43 + m._13, m._44 + m._14);
	Planes[2] = -D3DXPLANE(m._41 - m._21, m._42 - m._22, m._43 - m._23, m._44 - m._24);
	Planes[3] = -D3DXPLANE(m._41 + m._21, m._42 + m._22, m._43 + m._23, m._44 + m._24);
}

// NOT EXACT: functional, from the disassembly and pseudocode of 0x468760
int LandUnknown466BE0Class::Unknown_468760(const Vector3 &center, const Vector3 &extent)
{
	Vector3 lo = center - extent;
	Vector3 hi = center + extent;
	for (int i = 0; i < 4; i++) {
		const D3DXPLANE &p = Planes[i];
		// near: the corner with the smallest distance along the plane's normal; far: the largest
		float nx = (p.a < 0.0f) ? hi.X : lo.X;
		float fx = (p.a < 0.0f) ? lo.X : hi.X;
		float ny = (p.b < 0.0f) ? hi.Y : lo.Y;
		float fy = (p.b < 0.0f) ? lo.Y : hi.Y;
		float nz = (p.c < 0.0f) ? hi.Z : lo.Z;
		float fz = (p.c < 0.0f) ? lo.Z : hi.Z;
		if (nz * p.c + nx * p.a + ny * p.b + p.d > 0.0f) {
			return 0;
		}
		if (fz * p.c + fx * p.a + fy * p.b + p.d >= 0.0f) {
			return 1;
		}
	}
	return 2;
}

// NOT EXACT: functional, from the disassembly and pseudocode of 0x468920
void LandUnknown466BE0Class::Unknown_468920(void)
{
	delete [] Unknown04;
	Unknown04 = NULL;
	Unknown08 = (BaseTessellationSize - 1) * (BaseTessellationSize - 1);
	Unknown04 = new LandUnknown466BE0CellStruct[Unknown08];
	int cell = 0;
	for (unsigned int y = 1; y < (unsigned int)BaseTessellationSize; y++) {
		for (unsigned int x = 1; x < (unsigned int)BaseTessellationSize; x++) {
			unsigned short n = (unsigned short)BaseTessellationSize;
			Unknown04[cell].Corner[0] = (unsigned short)(n * (y - 1) + x - 1);
			Unknown04[cell].Corner[1] = (unsigned short)(n * y + x - 1);
			Unknown04[cell].Corner[2] = (unsigned short)(n * (y - 1) + x);
			Unknown04[cell].Corner[3] = (unsigned short)(n * y + x);
			cell++;
		}
	}
	float *verts;
	VertexBuffer->Lock(0, VertexCount * 0x34, (BYTE **)&verts, 0);
	for (int i = 0; i < Unknown08; i++) {
		// the four corners, then the four one unit up
		Vector3 points[8];
		for (int k = 0; k < 4; k++) {
			const float *v = verts + Unknown04[i].Corner[k] * 13;
			points[k].Set(v[0], v[1], v[2]);
			points[k + 4].Set(v[0], v[1], v[2] + 1.0f);
		}
		Unknown_468BE0(Unknown04[i].Box, points);
	}
	VertexBuffer->Unlock();
}

// NOT EXACT: functional, from the disassembly and pseudocode of 0x468BE0
void LandUnknown466BE0Class::Unknown_468BE0(AABoxClass &box, const Vector3 *points)
{
	Vector3 lo = points[0];
	Vector3 hi = points[0];
	for (int i = 1; i < 8; i++) {
		if (points[i].X < lo.X) lo.X = points[i].X;
		if (points[i].X > hi.X) hi.X = points[i].X;
		if (points[i].Y < lo.Y) lo.Y = points[i].Y;
		if (points[i].Y > hi.Y) hi.Y = points[i].Y;
		if (points[i].Z < lo.Z) lo.Z = points[i].Z;
		if (points[i].Z > hi.Z) hi.Z = points[i].Z;
	}
	box.Center.X = (hi.X - lo.X) * 0.5f + lo.X;
	box.Center.Y = (hi.Y - lo.Y) * 0.5f + lo.Y;
	box.Center.Z = (hi.Z - lo.Z) * 0.5f + lo.Z;
	box.Extent.X = (float)fabs(box.Center.X - lo.X);
	box.Extent.Y = (float)fabs(box.Center.Y - lo.Y);
	box.Extent.Z = (float)fabs(box.Center.Z - lo.Z);
}

// NOT EXACT: functional, from the disassembly and pseudocode of 0x468D50
void LandUnknown466BE0Class::Unknown_468D50(void)
{
	float *verts;
	VertexBuffer->Lock(0, VertexCount * 0x34, (BYTE **)&verts, 0);
	for (int i = 0; i < VertexCount; i++) {
		verts[i * 13 + 10] = 0.0f;
	}
	for (int c = 0; c < Unknown08; c++) {
		if (Unknown_468760(Unknown04[c].Box.Center, Unknown04[c].Box.Extent) >= 1) {
			verts[Unknown04[c].Corner[0] * 13 + 10] = 1.0f;
			verts[Unknown04[c].Corner[1] * 13 + 10] = 1.0f;
			verts[Unknown04[c].Corner[2] * 13 + 10] = 1.0f;
			verts[Unknown04[c].Corner[3] * 13 + 10] = 1.0f;
		}
	}
	VertexBuffer->Unlock();
}

/*
**	Notes
**
**	The object Load_Assets 0x4691D0 creates through 0x466E70 and the land's destructor deletes through 0x466F10: the
**	land drawn with displacement maps where the hardware has them (a Matrox adapter, pixel shader 1.3; the DMap
**	registry key, read only). Separate from land.cpp: the land's destructor calls 0x466F10 out of line. Rebuilt: the
**	constructor 0x466BE0, the destructor 0x466DE0 / 0x466D40, 0x466E70, 0x466F10, the empty 0x466F40 and the registry
**	reader 0x468120. Exact: 0x467910 (the L8 levels into the displacement map). Functional, NOT EXACT (Matrox only, never
**	run): 0x4677B0, 0x467A50, 0x467E40, the draw
**	0x466F60 and its helpers 0x468240-0x468D50.
*/
