// Earth & Beyond client (reconstruction): app\shared\ the land's displacement-mapped companion (file name unknown).
#ifndef __LANDDMAP_H
#define __LANDDMAP_H

#include "dx8wrapper.h"
#include "aabox.h"
#include <d3dx8.h>

class HeightFieldClass;
class CameraClass;
class Matrix4;
class RenderInfoClass;

void Unknown_466E70(HeightFieldClass *field, float detail_u, float detail_v);	// NAME 1: unknown; news the global object (0x90 bytes, 0xBDC0D8) Unknown_466F10 deletes; called by Load_Assets only
void Unknown_466F10(void);
// NAME 1: unknown; a cell of the base mesh: its bounds (0x468BE0) and its four corner vertices (0x468920)
struct LandUnknown466BE0CellStruct
{
	AABoxClass					Box;			// +0x00
	unsigned int				Corner[4];	// +0x18, vertex indices (y-1, x-1), (y, x-1), (y-1, x), (y, x)
};

// NAME 1: unknown; the object Unknown_466E70 makes (0x90 bytes, vtable 0xAE8974, ctor 0x466BE0): the land's displacement-mapped
// variant, on only for a Matrox adapter (vendor 0x102B) with pixel shader 1.3 and the registry's DisplacementMappingEnabled
// (Software\Westwood Studios\Earth and Beyond\Render\DMap, 0x468120); Update_Patches and Render use it while its +0x11 is set
class LandUnknown466BE0Class
{
public:
	LandUnknown466BE0Class(HeightFieldClass *field, float detail_u, float detail_v);
	virtual ~LandUnknown466BE0Class(void);
	void							Unknown_466F40(CameraClass &camera, int lod, const Matrix4 &scale);	// NAME 1: unknown; empty
	void							Unknown_466F60(RenderInfoClass &rinfo);	// NAME 1: unknown; called first by Render 0x474B30
	void							Create_Displacement_Map(void);	// NAME 2: CreateTexture into +0x30 (0x4676D0; inlined in the ctor)
	void							Create_Height_Texture(void);	// NAME 2: D3DXCreateTexture into +0x34 (0x467710; inlined in the ctor)
	void							Release_Displacement_Map(void);	// NAME 2: (0x467750; inlined in the dtor)
	void							Release_Height_Texture(void);	// NAME 2: (0x467780; inlined in the dtor)
	void							Read_Registry(void);	// NAME 2: reads the DMap key's four values (0x468120)
	void							Unknown_4677B0(HeightFieldClass *field);	// NAME 1: unknown; the height field into the L8 texture (+0x34)
	void							Unknown_467910(void);	// NAME 1: unknown; the L8 texture's levels into the displacement map (+0x30)
	void							Unknown_467A50(float detail_u, float detail_v);	// NAME 1: unknown; the base mesh's index and vertex buffers
	void							Unknown_467E00(void);	// NAME 1: unknown; empty, no caller (0x467E00: a bare ret)
	void							Unknown_467E20(void);	// NAME 1: unknown; empty, no caller (0x467E20: a bare ret)
	void							Unknown_467E40(void);	// NAME 1: unknown; the vertex shader (+0x2C)
	void							Release_Vertex_Shader(void);	// NAME 2: DeleteVertexShader of +0x2C (0x467FD0; inlined in the dtor)
	DWORD							Create_Vertex_Shader(const char *source, DWORD *declaration);	// NAME 2: assembles a shader into +0x2C (0x468010; never called)
	void							Check_Hardware(void);	// NAME 2: keeps Enabled only on a Matrox (0x102B) with pixel shader 1.3 (0x4680D0; inlined in the ctor)
	void							Unknown_468240(CameraClass &camera, const D3DXMATRIX &scale);	// NAME 1: unknown; the planes (+0x40) from scale * view * proj
	int							Unknown_468760(const Vector3 &center, const Vector3 &extent);	// NAME 1: unknown; 0 outside a plane, 1 crossing, 2 inside
	void							Release_Cells(void);	// NAME 2: deletes the cells (+0x04) (0x4688F0; inlined in the dtor, 0x467A50 and 0x468920)
	void							Unknown_468920(void);	// NAME 1: unknown; the cells (+0x04) and their bounds
	void							Unknown_468BE0(AABoxClass &box, const Vector3 *points);	// NAME 1: unknown; the box around eight points
	void							Unknown_468D50(void);	// NAME 1: unknown; marks the vertices of the cells inside the planes

	LandUnknown466BE0CellStruct *	Unknown04;			// NAME 1: +0x04, the cells (0x468920), deleted by the dtor and 0x467A50
	int							Unknown08;							// NAME 1: +0x08, the cell count, (size - 1) squared (0x468920)
	int							Unknown0C;							// NAME 1: +0x0C, 1 after the ctor
	bool							Enabled;									// NAME 5: +0x10, DisplacementMappingEnabled (registry), cleared without the hardware
	bool							Unknown11;							// NAME 1: +0x11, the land uses the object (cleared when not Enabled)
	bool							Unknown12;
	int							BaseTessellationSize;				// NAME 5: +0x14, BaseTessellationSize (registry, 16)
	int							DisplacementMapSize;					// NAME 5: +0x18, DisplacementMapSize (registry, 256)
	HeightFieldClass *		Field;									// NAME 2: +0x1C, the ctor's argument
	float							TessellationRate;						// NAME 5: +0x20, TessellationRate (registry, 80000)
	IDirect3DVertexBuffer8 *	VertexBuffer;						// NAME 2: +0x24, CreateVertexBuffer (0x467A50)
	IDirect3DIndexBuffer8 *	IndexBuffer;							// NAME 2: +0x28, CreateIndexBuffer (0x467A50)
	DWORD							VertexShader;							// NAME 2: +0x2C, DeleteVertexShader in the dtor
	IDirect3DTexture8 *		DisplacementMap;						// NAME 2: +0x30, format 0x102B0001 (CreateTexture in the ctor)
	IDirect3DTexture8 *		HeightTexture;							// NAME 2: +0x34, D3DFMT_L8, managed (D3DXCreateTexture in the ctor)
	int							IndexCount;								// NAME 2: +0x38, (6 size - 6)(size - 1) (0x467A50)
	int							VertexCount;							// NAME 2: +0x3C, size squared (0x467A50)
	D3DXPLANE					Planes[4];							// NAME 2: +0x40, the side planes of scale * view * proj (0x468240)
	float							FogStart;								// NAME 2: +0x80, D3DRS_FOGSTART read back by 0x466F60
	float							FogEnd;								// NAME 2: +0x84
	float							FogRange;								// NAME 2: +0x88, FogEnd - FogStart
	float							Unknown8C;							// NAME 1: +0x8C, never written
};

#endif //__LANDDMAP_H
