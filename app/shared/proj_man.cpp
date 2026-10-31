// Earth & Beyond client (reconstruction): notes at the end of the file
#include "proj_man.h"
#include "camera.h"
#include "vertmaterial.h"
#include "shader.h"
#include "texture.h"
#include "assetmgr.h"
#include "rinfo.h"
#include "wwmath.h"
#include <math.h>
#include "..\shared\unknown_debug.h"


static float	MIN_STATIC_SHADOW_COS_HALF_THETA		= cos(DEG_TO_RADF(10.0f)/2.0f);	// angle to allow shadow tex re-use


/************************************************************************************
**
** StaticShadowTexMgrClass Implemenation
**
************************************************************************************/
StaticShadowTexMgrClass::~StaticShadowTexMgrClass(void)
{
}

TextureClass * StaticShadowTexMgrClass::Peek_Shadow_Texture
(
	uint32					obj_type_id,
	const Quaternion &	orientation
)
{
	for (int i=0; i<ShadowTextures.Count(); i++) {
		Quaternion deltaq = ShadowTextures[i].ObjectOrientation * Inverse(orientation);
		if (	(ShadowTextures[i].ObjectTypeID == obj_type_id) &&
				(deltaq.W > MIN_STATIC_SHADOW_COS_HALF_THETA))
		{
			return ShadowTextures[i].Texture;
		}
	}
	return NULL;
}

























































































void StaticShadowTexMgrClass::Add_Shadow_Texture
(
	uint32					obj_type_id,
	const Quaternion &	orientation,
	TextureClass *			tex
)
{
	G_ASSERT(Peek_Shadow_Texture(obj_type_id,orientation) == NULL);
	ShadowTexClass record(obj_type_id,orientation,tex);
	ShadowTextures.Add(record);
}

void StaticShadowTexMgrClass::Remove_Shadow_Texture
(
	TextureClass *			tex
)
{
	for (int i=0; i<ShadowTextures.Count(); i++) {
		if (ShadowTextures[i].Texture == tex) {
			ShadowTextures.Delete(i);
			return;
		}
	}
}

void StaticShadowTexMgrClass::Reset(void)
{
	ShadowTextures.Delete_All();
}




















/************************************************************************************
**
** ProjectorManagerClass Implementation
**
************************************************************************************/
ProjectorManagerClass::ProjectorManagerClass(void) :
	ShadowNormalIntensity(0.75f),
	ShadowBlobTexture(NULL),
	StaticShadowTexMgr(NULL),
	ShadowCamera(NULL),
	ShadowMaterial(NULL),
	ShadowRenderContext(NULL),
	Unknown2C(false),
	StaticProjectorCullingSystem(NULL),
	DynamicProjectorCullingSystem(NULL),
	ShadowLightDirection(0.0f,0.0f,1.0f)
{
	StaticProjectorCullingSystem = new TypedAABTreeCullSystemClass<TexProjectClass>;
	DynamicProjectorCullingSystem = new TypedGridCullSystemClass<TexProjectClass>;
}

ProjectorManagerClass::~ProjectorManagerClass(void)
{
	Remove_All_Texture_Projectors();
	delete StaticProjectorCullingSystem;
	delete DynamicProjectorCullingSystem;
	Release_Projector_Resources();
}

void ProjectorManagerClass::Release_Projector_Resources(void)
{
	REF_PTR_RELEASE(ShadowCamera);
	REF_PTR_RELEASE(ShadowMaterial);
	REF_PTR_RELEASE(StaticShadowTexMgr);
	if (ShadowRenderContext != NULL) {
		delete ShadowRenderContext;
		ShadowRenderContext = NULL;
	}
}

CameraClass * ProjectorManagerClass::Get_Shadow_Camera(void)
{
	if (ShadowCamera == NULL) {
		ShadowCamera = new CameraClass;
		ShadowCamera->Set_Clip_Planes(0.2f,500.0f);
		ShadowCamera->Set_View_Plane(DEG_TO_RAD(90.0f),DEG_TO_RAD(90.0f));
		ShadowCamera->Set_Viewport(Vector2(0,0),Vector2(1,1));
	}
	ShadowCamera->Add_Ref();
	return ShadowCamera;
}

VertexMaterialClass * ProjectorManagerClass::Get_Shadow_Material(void)
{
	if (ShadowMaterial == NULL) {
		ShadowMaterial = new VertexMaterialClass;
		ShadowMaterial->Set_Ambient(0,0,0);
		ShadowMaterial->Set_Diffuse(0,0,0);
		ShadowMaterial->Set_Specular(0,0,0);
		ShadowMaterial->Set_Emissive(0,0,0);
		ShadowMaterial->Set_Opacity(1.0f);
		ShadowMaterial->Set_Shininess(0.0f);
	}
	ShadowMaterial->Add_Ref();
	return ShadowMaterial;
}

ShaderClass ProjectorManagerClass::Get_Shadow_Shader(void)
{
	ShaderClass shader = ShaderClass::_PresetOpaqueShader;
	shader.Set_Depth_Compare(ShaderClass::PASS_ALWAYS);
	shader.Set_Depth_Mask(ShaderClass::DEPTH_WRITE_DISABLE);
	shader.Set_Primary_Gradient(ShaderClass::GRADIENT_MODULATE);
	shader.Set_Secondary_Gradient(ShaderClass::SECONDARY_GRADIENT_DISABLE);
	return shader;
}

void ProjectorManagerClass::Enable_Static_Projectors(bool onoff)
{
	StaticProjectorsEnabled = onoff;
}

bool ProjectorManagerClass::Are_Static_Projectors_Enabled(void)
{
	return StaticProjectorsEnabled;
}

void ProjectorManagerClass::Enable_Dynamic_Projectors(bool onoff)
{
	DynamicProjectorsEnabled = onoff;
}

bool ProjectorManagerClass::Are_Dynamic_Projectors_Enabled(void)
{
	return DynamicProjectorsEnabled;
}

void ProjectorManagerClass::Set_Shadow_Normal_Intensity(float normal_intensity)
{
	if (normal_intensity < 0.0f) {
		normal_intensity = 0.0f;
	}
	if (normal_intensity > 1.0f) {
		normal_intensity = 1.0f;
	}
	ShadowNormalIntensity = normal_intensity;
}

float ProjectorManagerClass::Get_Shadow_Normal_Intensity(void)
{
	return ShadowNormalIntensity;
}

bool ProjectorManagerClass::Set_Shadow_Blob_Texture(const char * name)
{
	if (name == NULL) {
		return false;
	}
	TextureClass * tex = WW3DAssetManager::Get_Instance()->Get_Texture(name);
	if (tex == NULL) {
		return false;
	}
	REF_PTR_SET(ShadowBlobTexture,tex);
	REF_PTR_RELEASE(tex);
	ShadowBlobTexture->Set_U_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);
	ShadowBlobTexture->Set_V_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);
	return true;
}

TextureClass * ProjectorManagerClass::Get_Shadow_Blob_Texture(void)
{
	if (ShadowBlobTexture == NULL) {
		ShadowBlobTexture = WW3DAssetManager::Get_Instance()->Get_Texture("shadow.tga");
		ShadowBlobTexture->Set_U_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);
		ShadowBlobTexture->Set_V_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);
		G_ASSERT(ShadowBlobTexture != NULL);
	}
	ShadowBlobTexture->Add_Ref();
	return ShadowBlobTexture;
}
void ProjectorManagerClass::Add_Static_Texture_Projector(TexProjectClass * newprojector)
{
	G_ASSERT(newprojector);
	G_ASSERT(!StaticProjectorList.Is_In_List(newprojector));

	StaticProjectorList.Add(newprojector);
	StaticProjectorCullingSystem->Add_Object(newprojector);
}

void ProjectorManagerClass::Remove_Static_Texture_Projector(TexProjectClass * projector)
{
	G_ASSERT(projector);
	G_ASSERT(projector->Get_Culling_System() == StaticProjectorCullingSystem);

	StaticProjectorList.Remove(projector);
	StaticProjectorCullingSystem->Remove_Object(projector);
}

void ProjectorManagerClass::Add_Dynamic_Texture_Projector(TexProjectClass * newprojector)
{
	G_ASSERT(newprojector);
	G_ASSERT(!DynamicProjectorList.Is_In_List(newprojector));

	DynamicProjectorList.Add(newprojector);
	DynamicProjectorCullingSystem->Add_Object(newprojector);
}

void ProjectorManagerClass::Remove_Dynamic_Texture_Projector(TexProjectClass * projector)
{
	G_ASSERT(projector);
	G_ASSERT(projector->Get_Culling_System() == DynamicProjectorCullingSystem);

	DynamicProjectorList.Remove(projector);
	DynamicProjectorCullingSystem->Remove_Object(projector);
}

void ProjectorManagerClass::Remove_Texture_Projector(TexProjectClass * projector)
{
	G_ASSERT(projector);

	if (DynamicProjectorList.Is_In_List(projector)) {
		DynamicProjectorCullingSystem->Remove_Object(projector);
		DynamicProjectorList.Remove(projector);
	} else if (StaticProjectorList.Is_In_List(projector)) {
		StaticProjectorCullingSystem->Remove_Object(projector);
		StaticProjectorList.Remove(projector);
	} else {
		G_ASSERT(0);
	}
}

void ProjectorManagerClass::Remove_All_Texture_Projectors(void)
{
	RefMultiListIterator<TexProjectClass> dit(&DynamicProjectorList);
	for (dit.First(); !dit.Is_Done(); dit.Next()) {
		TexProjectClass * projector = dit.Peek_Obj();
		DynamicProjectorCullingSystem->Remove_Object(projector);
		DynamicProjectorList.Remove(projector);
	}

	RefMultiListIterator<TexProjectClass> sit(&StaticProjectorList);
	for (sit.First(); !sit.Is_Done(); sit.Next()) {
		TexProjectClass * projector = sit.Peek_Obj();
		StaticProjectorCullingSystem->Remove_Object(projector);
		StaticProjectorList.Remove(projector);
	}
}

bool ProjectorManagerClass::Contains(TexProjectClass * projector)
{
	return (DynamicProjectorList.Is_In_List(projector) || StaticProjectorList.Is_In_List(projector));
}

// Apply_Projectors 0x492C70 (Renegade's PhysicsSceneClass::Apply_Projectors) is not rebuilt yet.





































































































































































StaticShadowTexMgrClass * ProjectorManagerClass::Get_Static_Shadow_Tex_Mgr(void)
{
	if (StaticShadowTexMgr == NULL) {
		StaticShadowTexMgr = new StaticShadowTexMgrClass;
		G_ASSERT(StaticShadowTexMgr != NULL);
	}
	StaticShadowTexMgr->Add_Ref();
	return StaticShadowTexMgr;
}

// 0x493340 (texmgr != NULL at line 592, set_vector at 703) and 0x493620 (Renegade's
// Setup_Static_Directional_Shadow: texmgr != NULL at 654, shadow_cam != NULL at 676) are not rebuilt yet.


































































































































void ProjectorManagerClass::Get_Shadow_Light_Direction(Vector3 * set_vector)
{
	G_ASSERT(set_vector != NULL);
	*set_vector = ShadowLightDirection;
}

void ProjectorManagerClass::Set_Shadow_Light_Direction(const Vector3 & dir)
{
	ShadowLightDirection = dir;
}


/*
**	Reconstruction notes (kept here so the asserts stay on the client's lines)
**
**	E&B's copy of Renegade's wwphys/pscene_projectors.cpp, its projector code moved out of
**	PhysicsSceneClass into ProjectorManagerClass (proj_man.h). The client's functions, in order:
**	StaticShadowTexMgrClass: scalar deleting dtor 0x4918E0, dtor 0x491910, Peek_Shadow_Texture
**	0x4919B0, Add_Shadow_Texture 0x491A80 (line 138), Remove_Shadow_Texture 0x491CB0, Reset 0x491DA0.
**	ProjectorManagerClass: ctor 0x491DD0, scalar deleting dtor 0x491F50 (the typed culling systems'
**	0x491F80 / 0x491FB0 and dtors 0x491FE0 / 0x492000), dtor 0x492020, then the methods in proj_man.h.
*/
