// Earth & Beyond client (reconstruction): app\Shared\enbassetmgr.h (no string names it; interim placement beside
// enbassetmgr.cpp, D137). E&B's asset manager, the object at *0xBDADC0.
#ifndef ENBASSETMGR_H
#define ENBASSETMGR_H

#include "assetmgr.h"

class MeshClass;
class VertexMaterialClass;
class Vector3;

// E&B's asset manager, 0x408C bytes (ENBAssetManager in the client's strings, table 0xAEA5C8; renamed from the
// stand-in UnknownBDADC0Class in P-MC-3, together with kernel.cpp's own declaration of Unknown_BDADC0, whose
// mangled name carries the class name). Its own slots: +0x10 Release_Unused_Assets 0x48F2C0, +0x14 Create_Render_Obj 0x48B0F0,
// +0x38 Release_Unused_Textures 0x48F1A0, +0x78 0x48B220.
class ENBAssetManager : public WW3DAssetManager	// NAME 5: the client's profile string 'ENBAssetManager::Create_Render_Obj'
{
public:
	ENBAssetManager();
	virtual ~ENBAssetManager();

	virtual void Release_Unused_Assets(void);									// +0x10
	virtual RenderObjClass *Create_Render_Obj(const char *name);				// +0x14
	virtual void Release_Unused_Textures(void);									// +0x38
	virtual RenderObjClass *Create_Render_Obj(const char *name, float scale, Vector3 &hsv_shift);	// +0x78  // NAME 5: EA hueshift enbassetmgr.h

	RenderObjClass *Create_Render_Obj(const char *name, float scale, float *hsv) { return Create_Render_Obj(name, scale, *(Vector3 *)hsv); }
	RenderObjClass *Unknown_78(const char *name, float scale, float *hsv) { return Create_Render_Obj(name, scale, *(Vector3 *)hsv); }	// NAME 1 [Unknown_78]: compatibility alias for Create_Render_Obj

	void Make_Mesh_Unique(RenderObjClass *robj, bool geometry, bool colors);	// NAME 5: EA hueshift enbassetmgr.h
	void Make_HLOD_Unique(RenderObjClass *robj, bool geometry, bool colors);	// NAME 5: EA hueshift enbassetmgr.h
	void Make_Unique(RenderObjClass *robj, bool geometry, bool colors);			// NAME 5: EA hueshift enbassetmgr.h
	void Recolor_Vertex_Material(VertexMaterialClass *vmat, Vector3 &hsv_shift);	// NAME 5: EA hueshift enbassetmgr.h
	void Recolor_Vertices(unsigned int *color, int count, Vector3 &hsv_shift);		// NAME 5: EA hueshift enbassetmgr.h
	TextureClass *Recolor_Texture(TextureClass *texture, float *hsv);				// NAME 5: EA hueshift enbassetmgr.h
	TextureClass *Unknown_48CF90(const char *name, float *hsv);						// NAME 1 [Unknown_48CF90, name, hsv]: neutral (D10)
	void Recolor_Mesh(RenderObjClass *robj, float *hsv);							// NAME 5: EA hueshift enbassetmgr.h
	void Recolor_HLOD(RenderObjClass *robj, float *hsv);							// NAME 5: EA hueshift enbassetmgr.h
	void Recolor_ParticleEmitter(RenderObjClass *robj, float *hsv);					// NAME 5: EA hueshift enbassetmgr.h
	void Recolor_Sphere(RenderObjClass *robj, float *hsv);							// NAME 3 [Recolor_Sphere]: EA hueshift style for CLASSID_SPHERE
	void Recolor_Ring(RenderObjClass *robj, float *hsv);							// NAME 3 [Recolor_Ring]: EA hueshift style for CLASSID_RING
	void Recolor_Asset(RenderObjClass *robj, float *hsv);							// NAME 5: EA hueshift enbassetmgr.h
	void Unknown_48ECA0(TextureClass **textures, unsigned count);					// NAME 1 [Unknown_48ECA0, textures, count]: neutral (D10)
};
extern ENBAssetManager *Unknown_BDADC0;	// NAME 1 [Unknown_BDADC0]: 0xBDADC0

#endif
