/*
**	Earth & Beyond VIDTEST (reconstruction, D45, D46): the land's decorations (decoration.ini: trees placed
**	at random on the land, VIDTEST 0x415C40), as a render object drawn after the land (D101). Notes at the
**	end of viddeco.cpp.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __VIDDECO_H
#define __VIDDECO_H

#include "rendobj.h"
#include "vector.h"
#include "sphere.h"
#include "aabox.h"
#include "shader.h"

class LandClass;
class HeightFieldClass;
class TextureClass;
class VertexMaterialClass;
class DX8VertexBufferClass;
class DX8IndexBufferClass;

// NAME 2: ours (VIDTEST's land draws its decorations itself; the client's land from its patch records)
class VidtestDecorationsClass : public RenderObjClass
{
public:
	VidtestDecorationsClass(LandClass *land);
	virtual ~VidtestDecorationsClass(void);

// NAME 2: VIDTEST 0x415C40 (decoration.ini [set])
	bool							Create_Decorations(int set);

	// RenderObjClass
	virtual RenderObjClass *	Clone(void) const							{ return NULL; }
	virtual void				Render(RenderInfoClass &rinfo);
	virtual void				Get_Obj_Space_Bounding_Sphere(SphereClass &sphere) const;
	virtual void				Get_Obj_Space_Bounding_Box(AABoxClass &box) const;

protected:
// NAME 2: ours
	LandClass *					Land;
	HeightFieldClass *		Field;
	int							PatchSize;				// cells per patch side (the land's +0xF0)
	int							GridWidth;				// patches per side (the land's +0xFC)

	struct DecoBatchStruct
	{
		TextureClass *				Texture;
		ShaderClass					Shader;
		VertexMaterialClass *	Material;
		DX8VertexBufferClass *	VertexBuffer;
		DX8IndexBufferClass *	IndexBuffer;
		int							Vertices;
		int							Polys;
		bool							Shadow;					// the trees' shadows, drawn after them (VIDTEST 0x410B90) // NAME 2: ours
		int							Patch;					// drawn while this patch's level is below 3 (VIDTEST 0x410B90: +0x24 < 3) // NAME 2: ours
	};
	DynamicVectorClass<DecoBatchStruct *>	DecoBatches;	// the decorations, batched (VIDTEST 0x416170)
};

#endif //__VIDDECO_H
