// Earth & Beyond client (reconstruction): app\Shared\enbassetmgr.h (no string names it; interim placement beside
// enbassetmgr.cpp, D137). E&B's asset manager, the object at *0xBDADC0.
#ifndef ENBASSETMGR_H
#define ENBASSETMGR_H

#include "assetmgr.h"

class MeshClass;

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

	void Unknown_48BCF0(MeshClass *mesh, bool geometry, bool colors);	// NAME 1 [Unknown_48BCF0, geometry, colors]: neutral (D10), what each flag makes unique
	void Unknown_48BDC0(RenderObjClass *robj, bool geometry, bool colors);	// NAME 1 [Unknown_48BDC0]: neutral (D10)
	void Unknown_48BE60(RenderObjClass *robj, bool geometry, bool colors);	// NAME 1 [Unknown_48BE60]: neutral (D10)
	void Unknown_48D1F0(RenderObjClass *robj, float *hsv);	// 0x48D1F0: a mesh's HSV shift (not written)  // NAME 1 [Unknown_48D1F0, robj, hsv]: neutral (D10), what 0x48B220 passes
	void Unknown_48D710(RenderObjClass *robj, float *hsv);	// NAME 1 [Unknown_48D710]: neutral (D10)
	void Unknown_48D850(RenderObjClass *robj, float *hsv);	// 0x48D850: a particle emitter's (not written)  // NAME 1 [Unknown_48D850]: neutral (D10)
	void Unknown_48DD00(RenderObjClass *robj, float *hsv);	// 0x48DD00: a sphere's (not written)  // NAME 1 [Unknown_48DD00]: neutral (D10)
	void Unknown_48E0F0(RenderObjClass *robj, float *hsv);	// 0x48E0F0: a ring's (not written)  // NAME 1 [Unknown_48E0F0]: neutral (D10)
	void Unknown_48E4E0(RenderObjClass *robj, float *hsv);	// NAME 1 [Unknown_48E4E0]: neutral (D10)
	void Unknown_48ECA0(TextureClass **textures, unsigned count);	// NAME 1 [Unknown_48ECA0, textures, count]: neutral (D10)
	virtual RenderObjClass *Unknown_78(const char *name, float scale, float *hsv);	// +0x78 (also 'ENBAssetManager::Create_Render_Obj' in its profile string)  // NAME 1 [name, scale, hsv]: what 0x6F51D0 passes
};
extern ENBAssetManager *Unknown_BDADC0;	// NAME 1 [Unknown_BDADC0]: 0xBDADC0

#endif
