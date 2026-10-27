// Earth & Beyond client (reconstruction): app\shared\ the land's displacement-mapped companion (file name unknown).
#ifndef __LANDDMAP_H
#define __LANDDMAP_H

#include "dx8wrapper.h"

class HeightFieldClass;
class CameraClass;
class Matrix4;
class RenderInfoClass;

void Unknown_466E70(HeightFieldClass *field, float detail_u, float detail_v);	// NAME 1: unknown; news the global object (0x90 bytes, 0xBDC0D8) Unknown_466F10 deletes; called by Load_Assets only
void Unknown_466F10(void);
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
	void							Read_Registry(void);	// NAME 2: reads the DMap key's four values (0x468120)
	void							Unknown_4677B0(HeightFieldClass *field);	// NAME 1: unknown; the height field into the L8 texture (+0x34)
	void							Unknown_467910(void);	// NAME 1: unknown; the L8 texture's levels into the displacement map (+0x30)
	void							Unknown_467A50(float detail_u, float detail_v);	// NAME 1: unknown; the base mesh's index and vertex buffers
	void							Unknown_467E40(void);	// NAME 1: unknown; the vertex shader (+0x2C)

	unsigned char *			m_Unknown04;							// NAME 1: +0x04, deleted by the dtor and 0x467A50
	int							m_Unknown08;
	int							m_Unknown0C;							// NAME 1: +0x0C, 1 after the ctor
	bool							Enabled;									// NAME 5: +0x10, DisplacementMappingEnabled (registry), cleared without the hardware
	bool							m_Unknown11;							// NAME 1: +0x11, the land uses the object (cleared when not Enabled)
	bool							m_Unknown12;
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
	char							m_Unknown40[0x50];
};

#endif //__LANDDMAP_H
