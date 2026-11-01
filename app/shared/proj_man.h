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
#include "obbox.h"

class CameraClass;
class VertexMaterialClass;
class SpecialRenderInfoClass;
class ShaderClass;
class RenderObjClass;
class Matrix3D;
class ProjectorManagerClass;

// The shadow render target maker the manager calls (0x498120, another file: DX8Wrapper::Create_Render_Target
// in the first format the device supports)
TextureClass * Create_Projector_Render_Target(int width,int height);	// NAME 2: Renegade's pscene_projectors.cpp helper

/*
** ProjectionDefClass: how a caster's projector is set up (the asserts' "ProjectionDef"); only the members the
** manager reads
*/
class ProjectionDefClass : public RefCountClass	// NAME 2: the asserts' ProjectionDef
{
public:
	bool							Is_Additive(void) const			{ return Additive; }		// NAME 2: Renegade's Shadow_Is_Additive
	float							Get_Intensity(void) const		{ return Intensity; }	// NAME 2: Renegade's Shadow_Intensity
	float							Get_NearZ(void) const			{ return NearZ; }		// NAME 2: Renegade's Shadow_NearZ
	float							Get_FarZ(void) const				{ return FarZ; }			// NAME 2: Renegade's Shadow_FarZ

	bool							Unknown08;			// +0x08 static: Generate_Static_Shadow_Projectors takes these  // NAME 1: neutral (D10)
	bool							Unknown09;			// +0x09 the shadow ignores the caster's rotation  // NAME 1: neutral (D10)
	bool							Additive;			// +0x0A Init_Additive, else Init_Multiplicative  // NAME 2: Renegade's Shadow_Is_Additive
	float							Intensity;			// +0x0C  // NAME 2: Renegade's Shadow_Intensity
	float							NearZ;				// +0x10  // NAME 2: Renegade's Shadow_NearZ
	float							FarZ;					// +0x14  // NAME 2: Renegade's Shadow_FarZ
	int							TextureSize;		// +0x18 the dynamic projector's  // NAME 2: Set_Texture_Size's argument
};

/*
** ProjectorCasterClass: an object that casts a projector (the asserts' "Projection") built from its ProjectionDef.
** rs_proj_sup.cpp derives its casters from it (0x45D9E0 makes one, 0x20 bytes here); the receivers (0x45DC80)
** share its RefCountClass + MultiListObjectClass start
*/
class ProjectorCasterClass : public RefCountClass, public MultiListObjectClass	// NAME 2: what it holds
{
public:
	virtual RenderObjClass *	Peek_Render_Obj(void) = 0;		// slot 2: the model projected / rendered  // NAME 2: its use
	virtual Matrix3D				Get_Transform(void) = 0;		// slot 3  // NAME 2: Build_Quaternion's argument
	virtual uint32					Get_Type_ID(void) = 0;			// slot 4: the shadow texture key  // NAME 2: Renegade's def->Get_ID()

	void							Set_Projection(TexProjectClass * projection);	// 0x493D30  // NAME 2: Projection's setter
	void							Create_Projection(void);								// 0x493D80  // NAME 2: what it does
	void							Update_Projection(void);								// 0x493FE0  // NAME 2: recomputes the ortho projection

	bool							Unknown10;					// +0x10  // NAME 1: neutral (D10)
	bool							UseBlob;						// +0x11 the blob texture instead of a rendered shadow  // NAME 2: its use
	TexProjectClass *			Projection;					// +0x14
	ProjectionDefClass *		ProjectionDef;				// +0x18
	ProjectorManagerClass *	Manager;						// +0x1C  // NAME 2: what it points at
};

/*
** ProjectorReceiverClass: an object projectors land on (rs_proj_sup.cpp's 0x45DC80 derives from it: its CullObject
** at +0x10 points back at it through +0x2C); Apply_Projectors calls these three slots
*/
class ProjectorReceiverClass : public RefCountClass, public MultiListObjectClass	// NAME 2: what Apply_Projectors does with it
{
public:
	virtual void					Add_Projector(TexProjectClass * projector) = 0;		// slot 2  // NAME 2: its use
	virtual bool					Intersects(const OBBoxClass & box) = 0;				// slot 3: the projector's bounding volume  // NAME 2: its use
	virtual void *					Get_Projection_Object_ID(void) = 0;					// slot 4: compared with the projector's  // NAME 2: TexProjectClass's
};

// The cullable the receivers' culling systems collect (CullableClass + 4 bytes + the receiver at +0x2C)
class ProjectorReceiverCullClass : public CullableClass	// NAME 2: what it holds
{
public:
	int								Unknown28;					// +0x28  // NAME 1: neutral (D10)
	ProjectorReceiverClass *	Receiver;					// +0x2C  // NAME 2: 0x45DC80 stores the receiver here
};

typedef RefMultiListClass<ProjectorReceiverClass>						ProjectorReceiverListClass;		// NAME 2: as TexProjListClass
typedef TypedAABTreeCullSystemClass<ProjectorReceiverCullClass>	ReceiverAABTreeCullSystemClass;	// NAME 2: interim, the static objects' system
typedef TypedGridCullSystemClass<ProjectorReceiverCullClass>		ReceiverGridCullSystemClass;		// NAME 2: interim, the dynamic objects' system

/*
** CasterTexProjectClass: the dynamic projector Create_Projection makes (0x144 bytes, vtables 0xAEA764 / 0xAEA758
** / 0xAEA754): it renders its caster's model before each use
*/
class CasterTexProjectClass : public TexProjectClass	// NAME 2: what it is
{
public:
	CasterTexProjectClass(ProjectorCasterClass * owner);	// 0x494100
	// the destructor (0x494180, a jump to TexProjectClass's) is the compiler's
	virtual void			Pre_Render_Update(const Matrix3D & camera);	// 0x4941A0
	virtual void *			Get_Projection_Object_ID(void) const;			// 0x494410

	ProjectorCasterClass *	Owner;							// +0x13C  // NAME 2: what it points at
	bool							Unknown140;						// +0x140  // NAME 1: neutral (D10)
};

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
		// the client keeps uncalled copies of these three (0x4916B0, 0x491730, 0x4917C0): defined in proj_man.cpp
		ShadowTexClass(uint32 obj_type_id,const Quaternion & orientation,TextureClass * tex);
		ShadowTexClass(const ShadowTexClass & that);
		~ShadowTexClass(void) { REF_PTR_RELEASE(Texture); }
		const ShadowTexClass & operator = (const ShadowTexClass & that);

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

	void							Apply_Projectors(CameraClass & camera,ProjectorReceiverListClass & dynamic_receivers,ProjectorReceiverListClass & static_receivers,
									ReceiverAABTreeCullSystemClass * static_objs,ReceiverGridCullSystemClass * dynamic_objs);	// 0x492C70 (profile label pscene::Apply_Projectors)

	StaticShadowTexMgrClass *	Get_Static_Shadow_Tex_Mgr(void);				// 0x493260  // NAME 2: StaticShadowTexMgr's getter
	void							Generate_Static_Shadow_Projectors(RefMultiListClass<ProjectorCasterClass> & list);	// 0x493340  // NAME 2: Renegade's
	void							Setup_Static_Directional_Shadow(ProjectorCasterClass * caster,const Vector3 & light_dir);	// 0x493620  // NAME 2: Renegade's
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
