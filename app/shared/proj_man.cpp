// Earth & Beyond client (reconstruction): notes at the end of the file
#define ENABLE_WWPROFILE
#include "proj_man.h"
#include "camera.h"
#include "vertmaterial.h"
#include "shader.h"
#include "texture.h"
#include "assetmgr.h"
#include "rinfo.h"
#include "dx8wrapper.h"
#include "matrix3d.h"
#include "rendobj.h"
#include "wwprofile.h"
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
		ShadowCamera->Add_Ref();
		return ShadowCamera;
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
		ShadowMaterial->Add_Ref();
		return ShadowMaterial;
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
		DynamicProjectorCullingSystem->Remove_Object(dit.Peek_Obj());
		DynamicProjectorList.Remove(dit.Peek_Obj());
	}

	RefMultiListIterator<TexProjectClass> sit(&StaticProjectorList);
	for (sit.First(); !sit.Is_Done(); sit.Next()) {
		StaticProjectorCullingSystem->Remove_Object(sit.Peek_Obj());
		StaticProjectorList.Remove(sit.Peek_Obj());
	}
}

bool ProjectorManagerClass::Contains(TexProjectClass * projector)
{
	return (DynamicProjectorList.Is_In_List(projector) || StaticProjectorList.Is_In_List(projector));
}

void ProjectorManagerClass::Apply_Projectors
(
	CameraClass &						camera,
	ProjectorReceiverListClass &	dynamic_receivers,
	ProjectorReceiverListClass &	static_receivers,
	ReceiverAABTreeCullSystemClass *	static_objs,
	ReceiverGridCullSystemClass *		dynamic_objs
)
{
	WWPROFILE("pscene::Apply_Projectors");

	if ((static_objs == NULL) && (dynamic_objs == NULL)) {
		return;
	}

	Vector3 view_pos;
	Vector3 view_dir;
	camera.Get_Transform().Get_Translation(&view_pos);
	camera.Get_Transform().Get_Z_Vector(&view_dir);
	view_dir = -view_dir;

	if (StaticProjectorsEnabled) {

		/*
		** collect the visible static projectors
		*/
		StaticProjectorCullingSystem->Reset_Collection();
		StaticProjectorCullingSystem->Collect_Objects(camera.Get_Frustum());

		for (	TexProjectClass * static_projector = StaticProjectorCullingSystem->Get_First_Collected_Object();
				static_projector != NULL;
				static_projector = StaticProjectorCullingSystem->Get_Next_Collected_Object(static_projector))
		{
			if (!static_projector->Is_Intensity_Zero()) {
				bool first = true;

				if ((static_objs != NULL) && static_projector->Is_Affect_Static_Objects_Enabled()) {
					static_objs->Reset_Collection();
					static_objs->Collect_Objects(static_projector->Get_Bounding_Volume());
					for (	ProjectorReceiverCullClass * obj = static_objs->Get_First_Collected_Object();
							obj != NULL;
							obj = static_objs->Get_Next_Collected_Object(obj))
					{
						ProjectorReceiverClass * receiver = obj->Receiver;
						if (	(receiver != NULL) && static_receivers.Is_In_List(receiver) &&
								receiver->Intersects(static_projector->Get_Bounding_Volume()))
						{
							if (static_projector->Get_Projection_Object_ID() != receiver->Get_Projection_Object_ID()) {
								receiver->Add_Projector(static_projector);
								if (first) {
									static_projector->Pre_Render_Update(camera.Get_Transform());
									first = false;
								}
							}
						}
					}
				}

				if ((dynamic_objs != NULL) && static_projector->Is_Affect_Dynamic_Objects_Enabled()) {
					dynamic_objs->Reset_Collection();
					dynamic_objs->Collect_Objects(static_projector->Get_Bounding_Volume());
					for (	ProjectorReceiverCullClass * obj = dynamic_objs->Get_First_Collected_Object();
							obj != NULL;
							obj = dynamic_objs->Get_Next_Collected_Object(obj))
					{
						ProjectorReceiverClass * receiver = obj->Receiver;
						if ((receiver != NULL) && dynamic_receivers.Is_In_List(receiver)) {
							if (static_projector->Get_Projection_Object_ID() != receiver->Get_Projection_Object_ID()) {
								receiver->Add_Projector(static_projector);
								if (first) {
									static_projector->Pre_Render_Update(camera.Get_Transform());
									first = false;
								}
							}
						}
					}
				}
			}
		}
	}

	if (DynamicProjectorsEnabled) {

		/*
		** Collect the visible dynamic projectors
		*/
		DynamicProjectorCullingSystem->Reset_Collection();
		DynamicProjectorCullingSystem->Collect_Objects(camera.Get_Frustum());

		for (	TexProjectClass * dynamic_projector = DynamicProjectorCullingSystem->Get_First_Collected_Object();
				dynamic_projector != NULL;
				dynamic_projector = DynamicProjectorCullingSystem->Get_Next_Collected_Object(dynamic_projector))
		{
			if (!dynamic_projector->Is_Intensity_Zero()) {
				bool first = true;

				if ((static_objs != NULL) && dynamic_projector->Is_Affect_Static_Objects_Enabled()) {
					static_objs->Reset_Collection();
					static_objs->Collect_Objects(dynamic_projector->Get_Bounding_Volume());
					for (	ProjectorReceiverCullClass * obj = static_objs->Get_First_Collected_Object();
							obj != NULL;
							obj = static_objs->Get_Next_Collected_Object(obj))
					{
						ProjectorReceiverClass * receiver = obj->Receiver;
						if (	(receiver != NULL) && static_receivers.Is_In_List(receiver) &&
								receiver->Intersects(dynamic_projector->Get_Bounding_Volume()))
						{
							if (dynamic_projector->Get_Projection_Object_ID() != receiver->Get_Projection_Object_ID()) {
								receiver->Add_Projector(dynamic_projector);
								if (first) {
									dynamic_projector->Pre_Render_Update(camera.Get_Transform());
									first = false;
								}
							}
						}
					}
				}

				if ((dynamic_objs != NULL) && dynamic_projector->Is_Affect_Dynamic_Objects_Enabled()) {
					dynamic_objs->Reset_Collection();
					dynamic_objs->Collect_Objects(dynamic_projector->Get_Bounding_Volume());
					for (	ProjectorReceiverCullClass * obj = dynamic_objs->Get_First_Collected_Object();
							obj != NULL;
							obj = dynamic_objs->Get_Next_Collected_Object(obj))
					{
						ProjectorReceiverClass * receiver = obj->Receiver;
						if ((receiver != NULL) && dynamic_receivers.Is_In_List(receiver)) {
							if ((receiver->Get_Projection_Object_ID() != dynamic_projector->Get_Projection_Object_ID()) && (dynamic_projector->Get_Projection_Object_ID() != receiver->Get_Projection_Object_ID())) {
								receiver->Add_Projector(dynamic_projector);
								if (first) {
									dynamic_projector->Pre_Render_Update(camera.Get_Transform());
									first = false;
								}
							}
						}
					}
				}
			}
		}
	}
}



























StaticShadowTexMgrClass * ProjectorManagerClass::Get_Static_Shadow_Tex_Mgr(void)
{
	if (StaticShadowTexMgr == NULL) {
		StaticShadowTexMgr = new StaticShadowTexMgrClass;
		G_ASSERT(StaticShadowTexMgr != NULL);
	}
	StaticShadowTexMgr->Add_Ref();
	return StaticShadowTexMgr;
}


void ProjectorManagerClass::Generate_Static_Shadow_Projectors(RefMultiListClass<ProjectorCasterClass> & list)
{
	/*
	** Collect the casters that want a static shadow; each drops its projector
	*/
	RefMultiListClass<ProjectorCasterClass> shadow_gen_list;
	RefMultiListIterator<ProjectorCasterClass> it(&list);
	for (it.First(); !it.Is_Done(); it.Next()) {
		ProjectorCasterClass * obj = it.Peek_Obj();
		if (obj != NULL) {
			ProjectionDefClass * def = obj->ProjectionDef;
			if (def && def->Unknown08) {
				obj->Set_Projection(NULL);
				shadow_gen_list.Add(obj);
			}
		}
	}

	/*
	** Release all of the textures we were using for static shadows
	*/
	StaticShadowTexMgrClass * texmgr = Get_Static_Shadow_Tex_Mgr();
	G_ASSERT(texmgr != NULL);
	texmgr->Reset();
	REF_PTR_RELEASE(texmgr);

	/*
	** Generate a new shadow for each one
	*/
	RefMultiListIterator<ProjectorCasterClass> shadow_gen_iterator(&shadow_gen_list);
	for (shadow_gen_iterator.First(); !shadow_gen_iterator.Is_Done(); shadow_gen_iterator.Next()) {
		ProjectorCasterClass * obj = shadow_gen_iterator.Peek_Obj();
		Vector3 sunvector;
		Get_Shadow_Light_Direction(&sunvector);
		Setup_Static_Directional_Shadow(obj,sunvector);
	}
}




















// NOT EXACT: 0x493620, 1 register row in the inlined Get_Static_Shadow_Tex_Mgr (mov eax,esi / test eax,eax)
void ProjectorManagerClass::Setup_Static_Directional_Shadow(ProjectorCasterClass * caster,const Vector3 & light_dir)
{
	ProjectionDefClass * def = caster->ProjectionDef;
	if (def == NULL) {
		return;
	}

	uint32 type_id = caster->Get_Type_ID();
	Quaternion obj_orientation(1);
	if (def->Unknown09 == false) {
		obj_orientation = Build_Quaternion(caster->Get_Transform());
	}

	TexProjectClass * shadow_projector = new TexProjectClass;
	shadow_projector->Set_Texture_Size(128);
	if (def->Is_Additive()) {
		shadow_projector->Init_Additive();
	} else {
		shadow_projector->Init_Multiplicative();
	}
	shadow_projector->Set_Intensity(def->Get_Intensity(),true);
	shadow_projector->Compute_Ortho_Projection(caster->Peek_Render_Obj(),light_dir,def->Get_NearZ(),def->Get_FarZ());
	shadow_projector->Enable_Attenuation(true);
	shadow_projector->Enable_Depth_Gradient(true);

	StaticShadowTexMgrClass * texmgr = Get_Static_Shadow_Tex_Mgr();
	G_ASSERT(texmgr != NULL);
	TextureClass * existing_texture = texmgr->Peek_Shadow_Texture(type_id,obj_orientation);
	if (existing_texture != NULL) {
		shadow_projector->Set_Texture(existing_texture);
	} else {

		/*
		** No texture was available: render one into a render target (the blob texture when
		** the device has none), then add it to the cache so others can use it
		** (Renegade copies the render target into a new texture first)
		*/
		TextureClass * render_target = Create_Projector_Render_Target(128,128);
		if (render_target == NULL) {
			TextureClass * blob = Get_Shadow_Blob_Texture();
			shadow_projector->Set_Texture(blob);
			REF_PTR_RELEASE(blob);
		} else {
			render_target->Set_Mag_Filter(TextureClass::FILTER_TYPE_BEST);
			shadow_projector->Set_Render_Target(render_target);
		}

		CameraClass * shadow_cam = Get_Shadow_Camera();
		G_ASSERT(shadow_cam != NULL);
		SpecialRenderInfoClass * context = new SpecialRenderInfoClass(*shadow_cam,SpecialRenderInfoClass::RENDER_SHADOW);
		DX8Wrapper::Set_Shader(Get_Shadow_Shader());
		VertexMaterialClass * vmat = Get_Shadow_Material();
		DX8Wrapper::Set_Material(vmat);
		shadow_projector->Compute_Texture(caster->Peek_Render_Obj(),ShadowRenderContext);
		texmgr->Add_Shadow_Texture(type_id,obj_orientation,shadow_projector->Peek_Texture());
		REF_PTR_RELEASE(vmat);
		REF_PTR_RELEASE(shadow_cam);
	}

	caster->Set_Projection(shadow_projector);
	REF_PTR_RELEASE(shadow_projector);
	REF_PTR_RELEASE(texmgr);
}










void ProjectorManagerClass::Get_Shadow_Light_Direction(Vector3 * set_vector)
{
	G_ASSERT(set_vector != NULL);
	*set_vector = ShadowLightDirection;
}

void ProjectorManagerClass::Set_Shadow_Light_Direction(const Vector3 & dir)
{
	ShadowLightDirection = dir;
}


























void ProjectorCasterClass::Set_Projection(TexProjectClass * projection)
{
	if (Projection != NULL) {
		Projection->Release_Ref();
	}
	if (projection != NULL) {
		projection->Add_Ref();
	}
	Projection = projection;
}

void ProjectorCasterClass::Create_Projection(void)
{
	if (ProjectionDef != NULL) {
		CasterTexProjectClass * projection = new CasterTexProjectClass(this);
		int size = ProjectionDef->TextureSize;
		projection->Set_Intensity(ProjectionDef->Get_Intensity(),true);
		projection->Enable_Attenuation(true);
		projection->Enable_Depth_Gradient(false);
		projection->Set_Texture_Size(size);
		if (ProjectionDef->Is_Additive()) {
			projection->Init_Additive();
		} else {
			projection->Init_Multiplicative();
		}
		TextureClass * render_target = Create_Projector_Render_Target(size,size);
		if (render_target == NULL) {
			render_target = Manager->Get_Shadow_Blob_Texture();
			projection->Set_Texture(render_target);
			REF_PTR_RELEASE(render_target);
		} else {
			render_target->Set_Mag_Filter(TextureClass::FILTER_TYPE_BEST);
			projection->Set_Render_Target(render_target);
		}
		projection->Enable_Depth_Gradient(true);
		Set_Projection(projection);
		REF_PTR_RELEASE(projection);
		REF_PTR_RELEASE(render_target);
	}
}

void ProjectorCasterClass::Update_Projection(void)
{
	Vector3 light_dir;
	Manager->Get_Shadow_Light_Direction(&light_dir);
	G_ASSERT(ProjectionDef);
	G_ASSERT(Projection);
	Projection->Compute_Ortho_Projection(Peek_Render_Obj(),light_dir);
	if (Projection->Get_Culling_System() != NULL) {
		Projection->Get_Culling_System()->Update_Culling(Projection);
	}
}
CasterTexProjectClass::CasterTexProjectClass(ProjectorCasterClass * owner) :
	Owner(owner),
	Unknown140(false)
{
}

// CasterTexProjectClass's destructor 0x494180 is the compiler's (a jump to
// TexProjectClass's: an empty user destructor would reset the vtables first)

void CasterTexProjectClass::Pre_Render_Update(const Matrix3D & camera)
{
	if (!Unknown140) {
		if (Owner->UseBlob) {
			Set_Texture(Owner->Manager->Get_Shadow_Blob_Texture());
		} else {
			CameraClass * shadow_cam = Owner->Manager->Get_Shadow_Camera();
			G_ASSERT(shadow_cam != NULL);
			SpecialRenderInfoClass * context = new SpecialRenderInfoClass(*shadow_cam,SpecialRenderInfoClass::RENDER_SHADOW);
			DX8Wrapper::Set_Shader(Owner->Manager->Get_Shadow_Shader());
			VertexMaterialClass * vmat = Owner->Manager->Get_Shadow_Material();
			DX8Wrapper::Set_Material(vmat);
			Compute_Texture(Owner->Peek_Render_Obj(),context);
			delete context;
			REF_PTR_RELEASE(vmat);
			REF_PTR_RELEASE(shadow_cam);
		}
	}
	TexProjectClass::Pre_Render_Update(camera);
}

void * CasterTexProjectClass::Get_Projection_Object_ID(void) const
{
	return (void *)Owner->Get_Type_ID();
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
