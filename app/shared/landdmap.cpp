// Earth & Beyond client (reconstruction): app\shared\ the land's displacement-mapped companion, client 0x466BE0-0x468E70
// (file name unknown: NAME 1). Notes at the end.
#include "landdmap.h"
#include "heightfield.h"
#include "registry.h"
#include <d3dx8.h>

LandUnknown466BE0Class *Unknown_BDC0D8;	// NAME 1: unknown

LandUnknown466BE0Class::LandUnknown466BE0Class(HeightFieldClass *field, float detail_u, float detail_v) :
	m_Unknown04(NULL),
	Enabled(false),
	m_Unknown11(true),
	m_Unknown12(false),
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
	m_Unknown0C = 1;
	if (Enabled) {
		DX8CALL(CreateTexture(DisplacementMapSize, DisplacementMapSize, 0, 0, (D3DFORMAT)0x102B0001, D3DPOOL_DEFAULT, &DisplacementMap));
		DX8_ErrorCode(D3DXCreateTexture(DX8Wrapper::_Get_D3D_Device8(), DisplacementMapSize, DisplacementMapSize, 0, 0, D3DFMT_L8, D3DPOOL_MANAGED, &HeightTexture));
		Unknown_4677B0(Field);
		Unknown_467910();
		Unknown_467A50(detail_u, detail_v);
		Unknown_467E40();
		return;
	}
	m_Unknown11 = false;
}

LandUnknown466BE0Class::~LandUnknown466BE0Class(void)
{
	if (Enabled) {
		if (DisplacementMap != NULL) {
			DisplacementMap->Release();
			DisplacementMap = NULL;
		}
		if (HeightTexture != NULL) {
			HeightTexture->Release();
			HeightTexture = NULL;
		}
		if (VertexShader != 0) {
			DX8CALL(DeleteVertexShader(VertexShader));
			VertexShader = 0;
		}
		delete [] m_Unknown04;
		m_Unknown04 = NULL;
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

/*
**	Notes
**
**	The object Load_Assets 0x4691D0 creates through 0x466E70 and the land's destructor deletes through 0x466F10: the
**	land drawn with displacement maps where the hardware has them (a Matrox adapter, pixel shader 1.3; the DMap
**	registry key, read only). Separate from land.cpp: the land's destructor calls 0x466F10 out of line. Rebuilt: the
**	constructor 0x466BE0, the destructor 0x466DE0 / 0x466D40, 0x466E70, 0x466F10, the empty 0x466F40 and the registry
**	reader 0x468120. Not rebuilt (Matrox only): 0x4677B0, 0x467910, 0x467A50, 0x467E40, the draw 0x466F60 and its helpers.
*/
