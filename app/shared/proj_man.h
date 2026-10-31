// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\proj_man.h (the client's asserts name proj_man.cpp)
**
**	The projector / shadow manager: E&B's copy of Renegade's PhysicsSceneClass texture projection
**	code (wwphys/pscene_projectors.cpp) moved into a class of its own: the static and dynamic
**	projector lists and culling systems, the blob shadow texture, the shadow camera, material and
**	render context, and the static shadow texture manager (Renegade's StaticShadowTexMgrClass, here a
**	RefCountClass the manager creates on demand). The asserts name the members (StaticProjectorList,
**	StaticProjectorCullingSystem, ShadowBlobTexture, StaticShadowTexMgr, ...).
**
**	THE CLIENT'S LAYOUT (0x74 bytes), from the constructor 0x491DD0 and the destructor 0x492020; one
**	virtual (the destructor, vtable 0xAEA74C). StaticShadowTexMgrClass (0x20 bytes, vtable 0xAEA68C):
**	the constructor 0x491860, destructor 0x491910.
*/

#ifndef PROJ_MAN_H
#define PROJ_MAN_H

#include "refcount.h"
#include "vector.h"
#include "quat.h"
#include "vector3.h"
#include "texproject.h"
#include "texture.h"
#include "aabtreecull.h"
#include "gridcull.h"

class CameraClass;
class VertexMaterialClass;
class SpecialRenderInfoClass;
class ShaderClass;

/*
** StaticShadowTexMgrClass: the unique shadow textures of the static shadow projectors (Renegade's,
** with a RefCountClass base in E&B)
*/
class StaticShadowTexMgrClass : public RefCountClass
{
public:
	StaticShadowTexMgrClass(void) {}
	virtual ~StaticShadowTexMgrClass(void);
	void						Reset(void);

	TextureClass *			Peek_Shadow_Texture(uint32 obj_type_id,const Quaternion & orientation);
	void						Add_Shadow_Texture(uint32 obj_type_id,const Quaternion & orientation,TextureClass * tex);
	void						Remove_Shadow_Texture(TextureClass * tex);

private:

	class	 ShadowTexClass
	{
	public:
		ShadowTexClass(void) : ObjectTypeID(0), ObjectOrientation(1), Texture(NULL) {}
		ShadowTexClass(uint32 obj_type_id,const Quaternion & orientation,TextureClass * tex) :
			ObjectTypeID(obj_type_id), ObjectOrientation(orientation), Texture(NULL) { REF_PTR_SET(Texture,tex); }
		ShadowTexClass(const ShadowTexClass & that) : ObjectTypeID(0), ObjectOrientation(1), Texture(NULL) { *this = that; }
		~ShadowTexClass(void) { REF_PTR_RELEASE(Texture); }
		const ShadowTexClass & operator = (const ShadowTexClass & that)
		{
			ObjectTypeID = that.ObjectTypeID;
			ObjectOrientation = that.ObjectOrientation;
			REF_PTR_SET(Texture,that.Texture);
			return *this;
		}

		bool					operator == (const ShadowTexClass &)						{ return false; }
		bool					operator != (const ShadowTexClass & that)					{ return true; }
		uint32				ObjectTypeID;			// +0x00
		Quaternion			ObjectOrientation;	// +0x04
		TextureClass *		Texture;					// +0x14
	};

	DynamicVectorClass<ShadowTexClass>	ShadowTextures;	// +0x08
};


/*
** ProjectorManagerClass
*/
class ProjectorManagerClass	// NAME 3: class-from-file (D24) for app\shared\proj_man.cpp
{
public:
	ProjectorManagerClass(void);
	virtual ~ProjectorManagerClass(void);

	void							Release_Projector_Resources(void);				// 0x492200  // NAME 2: Renegade's PhysicsSceneClass method
	CameraClass *				Get_Shadow_Camera(void);							// 0x492290  // NAME 2: Renegade's (shadow_cam)
	VertexMaterialClass *	Get_Shadow_Material(void);							// 0x492380  // NAME 2: Renegade's Get_Shadow_Material_Pass, without the pass
	ShaderClass					Get_Shadow_Shader(void);							// 0x492470  // NAME 2: Renegade's Get_Shadow_Material_Pass's shader

	void							Enable_Static_Projectors(bool onoff);			// 0x4924A0  // NAME 2: Renegade's
	bool							Are_Static_Projectors_Enabled(void);			// 0x4924C0  // NAME 2: Renegade's
	void							Enable_Dynamic_Projectors(bool onoff);			// 0x4924E0  // NAME 2: Renegade's
	bool							Are_Dynamic_Projectors_Enabled(void);			// 0x492500  // NAME 2: Renegade's
	void							Set_Shadow_Normal_Intensity(float normal_intensity);	// 0x492520  // NAME 2: Renegade's
	float							Get_Shadow_Normal_Intensity(void);				// 0x492570  // NAME 2: Renegade's
	bool							Set_Shadow_Blob_Texture(const char * name);	// 0x492590  // NAME 2: what it sets
	TextureClass *				Get_Shadow_Blob_Texture(void);					// 0x492630  // NAME 2: ShadowBlobTexture's getter

	void							Add_Static_Texture_Projector(TexProjectClass * newprojector);	// 0x4926C0  // NAME 2: Renegade's
	void							Remove_Static_Texture_Projector(TexProjectClass * projector);	// 0x492780  // NAME 2: Renegade's
	void							Add_Dynamic_Texture_Projector(TexProjectClass * newprojector);	// 0x492840  // NAME 2: Renegade's
	void							Remove_Dynamic_Texture_Projector(TexProjectClass * projector);	// 0x492900  // NAME 2: Renegade's
	void							Remove_Texture_Projector(TexProjectClass * projector);			// 0x4929C0  // NAME 2: Renegade's
	void							Remove_All_Texture_Projectors(void);			// 0x492AD0  // NAME 2: what it does
	bool							Contains(TexProjectClass * projector);			// 0x492C00  // NAME 2: Renegade's

	void							Apply_Projectors(const CameraClass & camera);	// 0x492C70 (profile label pscene::Apply_Projectors)

	StaticShadowTexMgrClass *	Get_Static_Shadow_Tex_Mgr(void);				// 0x493260  // NAME 2: StaticShadowTexMgr's getter
	void							Get_Shadow_Light_Direction(Vector3 * set_vector);	// 0x493CB0  // NAME 2: set_vector; the sun (mainview passes minus its direction)
	void							Set_Shadow_Light_Direction(const Vector3 & dir);	// 0x493D00  // NAME 2: as the getter

	bool							StaticProjectorsEnabled;							// +0x04
	bool							DynamicProjectorsEnabled;							// +0x05
	int							Unknown08;												// +0x08  // NAME 1: neutral (D10)
	float							Unknown0C;												// +0x0C  // NAME 1: neutral (D10)
	float							Unknown10;												// +0x10  // NAME 1: neutral (D10)
	float							ShadowNormalIntensity;								// +0x14  // NAME 2: Renegade's (0x492520 clamps it)
	TextureClass *				ShadowBlobTexture;									// +0x18
	StaticShadowTexMgrClass *	StaticShadowTexMgr;								// +0x1C
	CameraClass *				ShadowCamera;											// +0x20  // NAME 2: Renegade's
	VertexMaterialClass *	ShadowMaterial;										// +0x24  // NAME 2: Renegade's ShadowMaterialPass
	SpecialRenderInfoClass *ShadowRenderContext;									// +0x28  // NAME 2: Renegade's
	bool							Unknown2C;												// +0x2C  // NAME 1: neutral (D10)
	TypedAABTreeCullSystemClass<TexProjectClass> *	StaticProjectorCullingSystem;	// +0x30
	TypedGridCullSystemClass<TexProjectClass> *		DynamicProjectorCullingSystem;	// +0x34
	Vector3						ShadowLightDirection;								// +0x38  // NAME 2: as its getter
	TexProjListClass			StaticProjectorList;									// +0x44
	TexProjListClass			DynamicProjectorList;								// +0x5C
};

#endif
