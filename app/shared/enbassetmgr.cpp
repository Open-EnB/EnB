// Earth & Beyond client (reconstruction): app\Shared\enbassetmgr.cpp (__FILE__ in the assert at 0xB721AC). Notes at the end of the file.
#include "enbassetmgr.h"
#include "rendobj.h"
#include "proto.h"
#include "wwprofile.h"
#include "g_assert.h"
#include <string.h>
#include <stdio.h>
#pragma warning(disable: 4786)
#include <vector>

#include "dx8wrapper.h"
#include "dx8caps.h"
#include "mesh.h"
#include "meshmdl.h"
#include "coltype.h"
#include "matinfo.h"
#include "vertmaterial.h"
#include "colorspace.h"
#include "texture.h"
#include "vector3.h"
#include "wwmath.h"
#include "..\..\wwlib\nstrdup.h"
#include "part_emt.h"
#include "sphereobj.h"
#include "ringobj.h"

void Unknown_48E9D0(RenderObjClass *robj);	// 0x48E9D0: Post_Process' walk of a model (recursive)  // NAME 1 [Unknown_48E9D0, robj]: neutral (D10)
bool Unknown_48E620(RenderObjClass *robj);	// 0x48E620: Post_Process' step for the 'plt' models (recursive)  // NAME 1 [Unknown_48E620, robj]: neutral (D10)

const float ident_scale(1.0f);
const Vector3 ident_HSV(0,0,0);

class ENBPrototypeClass : public PrototypeClass	// NAME 5: EA hueshift enbassetmgr.cpp
{
public:
	ENBPrototypeClass(RenderObjClass *proto, const char *name);
	virtual ~ENBPrototypeClass(void);

	virtual const char *Get_Name(void) const;
	virtual int Get_Class_ID(void) const;
	virtual RenderObjClass *Create(void);

	RenderObjClass *Proto;
	char *Name;
};

// @0x48AED0
ENBPrototypeClass::ENBPrototypeClass(RenderObjClass *proto, const char *name)
{
	Name = nstrdup(name);
	Proto = proto;
	Proto->Add_Ref();
}

// @0x48AF80
ENBPrototypeClass::~ENBPrototypeClass(void)
{
	delete [] Name;
	if (Proto != NULL) {
		Proto->Release_Ref();
	}
}

// @0x48B010
const char *ENBPrototypeClass::Get_Name(void) const
{
	return Name;
}

// @0x48B030
int ENBPrototypeClass::Get_Class_ID(void) const
{
	return Proto->Class_ID();
}

// @0x48B050
RenderObjClass *ENBPrototypeClass::Create(void)
{
	return Proto->Clone();
}

// @0x48B070
ENBAssetManager::ENBAssetManager()
{
}

// @0x48B0D0
ENBAssetManager::~ENBAssetManager()
{
}



// @0x48B0F0
RenderObjClass *ENBAssetManager::Create_Render_Obj(const char *name)
{
	WWPROFILE("ENBAssetManager::Create_Render_Obj()");
	RenderObjClass *robj = WW3DAssetManager::Create_Render_Obj(name);
	if (robj != NULL) {
		Post_Process(robj);
	} else {
		robj = NULL;
	}
	return(robj);
}

// @0x48B220
RenderObjClass *ENBAssetManager::Create_Render_Obj(const char *name, float Scale, Vector3 &HSV_shift)
{
	bool scale = (WWMath::Fabs(Scale - ident_scale) > 0.01f);
	bool hsv_shift = (WWMath::Fabs(HSV_shift.X - ident_HSV.X) > 1.0f) ||
	                 (WWMath::Fabs(HSV_shift.Y - ident_HSV.Y) > 0.01f) ||
	                 (WWMath::Fabs(HSV_shift.Z - ident_HSV.Z) > 0.01f);

	if (!scale && !hsv_shift) {
		return WW3DAssetManager::Create_Render_Obj(name);
	}

	char lower_case_name[256];
	strcpy(lower_case_name, name);
	_strlwr(lower_case_name);

	char newname[512];
	sprintf(newname, "#%s!%gH%gS%gV%g", lower_case_name, Scale, HSV_shift.X, HSV_shift.Y, HSV_shift.Z);

	RenderObjClass *rendobj = WW3DAssetManager::Create_Render_Obj(newname);
	if (rendobj != NULL) {
		Post_Process(rendobj);
		return rendobj;
	}

	rendobj = WW3DAssetManager::Create_Render_Obj(name);
	if (rendobj == NULL) {
		return NULL;
	}

	Make_Unique(rendobj, scale, hsv_shift);

	if (scale) {
		rendobj->Scale(Scale);
	}

	if (hsv_shift) {
		switch (rendobj->Class_ID()) {
			case RenderObjClass::CLASSID_MESH:
				Recolor_Mesh(rendobj, (float *)&HSV_shift);
				break;
			case RenderObjClass::CLASSID_HLOD:
				Recolor_HLOD(rendobj, (float *)&HSV_shift);
				break;
			case RenderObjClass::CLASSID_PARTICLEEMITTER:
				Recolor_ParticleEmitter(rendobj, (float *)&HSV_shift);
				break;
			case RenderObjClass::CLASSID_SPHERE:
				Recolor_Sphere(rendobj, (float *)&HSV_shift);
				break;
			case RenderObjClass::CLASSID_RING:
				Recolor_Ring(rendobj, (float *)&HSV_shift);
				break;
		}
	}

	Post_Process(rendobj);

	ENBPrototypeClass *proto = new ENBPrototypeClass(rendobj, newname);
	rendobj->Release_Ref();
	Add_Prototype(proto);

	return proto->Create();
}

// @0x48F2C0
void ENBAssetManager::Release_Unused_Assets(void)
{
	int count = Prototypes.Count();
	while (count-- > 0) {
		PrototypeClass *proto = Prototypes[count];
		Prototypes.Delete(count);
		if (proto != NULL) {
			delete proto;
		}
	}
	memset(PrototypeHashTable, 0, sizeof(PrototypeClass *) * PROTOTYPE_HASH_TABLE_SIZE);
	HAnimManager.Free_All_Anims();
	HTreeManager.Free_All_Trees();
	WW3DAssetManager::Release_Unused_Assets();
}

// @0x48BCF0
void ENBAssetManager::Make_Mesh_Unique(RenderObjClass *robj, bool geometry, bool colors)	// NAME 5: EA hueshift enbassetmgr.h
{
	MeshClass *mesh = (MeshClass *)robj;
	mesh->Make_Unique();
	MeshModelClass *model = mesh->Get_Model();
	if (colors) {
		MaterialInfoClass *matinfo = mesh->Get_Material_Info();
		for (int i = 0; i < matinfo->Vertex_Material_Count(); ++i) {
			matinfo->Peek_Vertex_Material(i)->Make_Unique();
		}
		REF_PTR_RELEASE(matinfo);
		model->Make_Color_Array_Unique(0);
		model->Make_Color_Array_Unique(1);
	}
	if (geometry) {
		model->Make_Geometry_Unique();
	}
	REF_PTR_RELEASE(model);
}

// @0x48BDC0
void ENBAssetManager::Make_HLOD_Unique(RenderObjClass *robj, bool geometry, bool colors)	// NAME 5: EA hueshift enbassetmgr.h
{
	int count = robj->Get_Num_Sub_Objects();
	for (int i = 0; i < count; ++i) {
		RenderObjClass *sub = robj->Get_Sub_Object(i);
		switch (sub->Class_ID()) {
			case RenderObjClass::CLASSID_MESH:
				Make_Mesh_Unique(sub, geometry, colors);
				break;
			case RenderObjClass::CLASSID_HLOD:
				Make_HLOD_Unique(sub, geometry, colors);
				break;
		}
		if (sub != NULL) {
			sub->Release_Ref();
		}
	}
}

// @0x48BE60
void ENBAssetManager::Make_Unique(RenderObjClass *robj, bool geometry, bool colors)	// NAME 5: EA hueshift enbassetmgr.h
{
	switch (robj->Class_ID()) {
		case RenderObjClass::CLASSID_MESH:
			Make_Mesh_Unique(robj, geometry, colors);
			break;
		case RenderObjClass::CLASSID_HLOD:
		{
			int count = robj->Get_Num_Sub_Objects();
			for (int i = 0; i < count; ++i) {
				RenderObjClass *sub = robj->Get_Sub_Object(i);
				Make_Unique(sub, geometry, colors);
				if (sub != NULL) {
					sub->Release_Ref();
				}
			}
			break;
		}
	}
}

// @0x48BF20
void ENBAssetManager::Recolor_Vertex_Material(VertexMaterialClass *vmat, Vector3 &hsv_shift)	// NAME 5: EA hueshift enbassetmgr.h
{
	Vector3 rgb;

	vmat->Get_Ambient(&rgb);
	Recolor(rgb, hsv_shift);
	vmat->Set_Ambient(rgb);

	vmat->Get_Diffuse(&rgb);
	Recolor(rgb, hsv_shift);
	vmat->Set_Diffuse(rgb);

	vmat->Get_Emissive(&rgb);
	Recolor(rgb, hsv_shift);
	vmat->Set_Emissive(rgb);

	vmat->Get_Specular(&rgb);
	Recolor(rgb, hsv_shift);
	vmat->Set_Specular(rgb);
}

// DX8Wrapper::Convert_Color_Clamp's inline assembly is the client's (dx8wrapper.h, D70)
void ENBAssetManager::Recolor_Vertices(unsigned int *color, int count, Vector3 &hsv_shift)	// NAME 5: EA hueshift enbassetmgr.h
{
	int i;
	Vector4 rgba;

	for (i = 0; i < count; i++) {
		rgba = DX8Wrapper::Convert_Color(color[i]);
		Recolor(reinterpret_cast<Vector3 &>(rgba), hsv_shift);
		color[i] = DX8Wrapper::Convert_Color_Clamp(rgba);
	}
}

// @0x48CF90
TextureClass *ENBAssetManager::Unknown_48CF90(const char *name, float *hsv)
{
	char lower_case_name[256];
	char newname[512];
	strcpy(lower_case_name, name);
	_strlwr(lower_case_name);
	sprintf(newname, "#%s!H%gS%gV%g", lower_case_name, hsv[0], hsv[1], hsv[2]);
	TextureClass *newtex = TextureHash.Get(newname);
	if (newtex) newtex->Add_Ref();
	return(newtex);
}

// @0x48CB40
// NOT EXACT: register-only (explain.py: identical once registers are normalised; the client loads the mip count after
// pushing the format, ours before; 19 argument forms, attempts.py show 0x48CB40); the logic is the client's (D64)
TextureClass *ENBAssetManager::Recolor_Texture(TextureClass *texture, float *hsv)	// NAME 5: EA hueshift enbassetmgr.h
{
	WWPROFILE("Recolor_Texture()");
	if (texture == NULL) return(NULL);

	// if texture is procedural return NULL
	const char *name = texture->Get_Texture_Name();
	if (name && name[0] == '!') return(NULL);

	char lower_case_name[256];
	char newname[512];
	strcpy(lower_case_name, name);
	_strlwr(lower_case_name);
	sprintf(newname, "#%s!H%gS%gV%g", lower_case_name, hsv[0], hsv[1], hsv[2]);

	// a new texture under the same file, shifted when it loads
	TextureClass *newtex = W3DNEW TextureClass(name, NULL, (MipCountType)texture->Get_Mip_Level_Count(),
		texture->Get_Texture_Format(), texture->Is_Compression_Allowed());
	newtex->Set_Mag_Filter(texture->Get_Mag_Filter());
	newtex->Set_Min_Filter(texture->Get_Min_Filter());
	newtex->Set_Mip_Mapping(texture->Get_Mip_Mapping());
	newtex->Set_U_Addr_Mode(texture->Get_U_Addr_Mode());
	newtex->Set_V_Addr_Mode(texture->Get_V_Addr_Mode());
	newtex->Set_HSV_Shift(*(Vector3 *)hsv);

	TextureHash.Insert(newname, newtex);
	newtex->Add_Ref();
	return(newtex);
}

// The cached copy under the HSV name, else a new one
static inline TextureClass *Recolored_Texture(ENBAssetManager *mgr, TextureClass *texture, float *hsv)	// NAME 1 [Recolored_Texture]: descriptive (inlined)
{
	TextureClass *newtex = mgr->Unknown_48CF90(texture->Get_Texture_Name(), hsv);
	if (newtex) return(newtex);
	return(mgr->Recolor_Texture(texture, hsv));
}

// @0x48D1F0
void ENBAssetManager::Recolor_Mesh(RenderObjClass *robj, float *hsv)	// NAME 5: EA hueshift enbassetmgr.h
{
	Vector3 &hsv_shift = *(Vector3 *)hsv;
	int i;

	MeshClass *mesh = (MeshClass *)robj;
	MeshModelClass *model = mesh->Get_Model();
	MaterialInfoClass *material = mesh->Get_Material_Info();

	// recolor vertex material
	for (i = 0; i < material->Vertex_Material_Count(); i++) {
		Recolor_Vertex_Material(material->Peek_Vertex_Material(i), hsv_shift);
	}

	// recolor color arrays
	unsigned int *color;
	color = model->Get_Color_Array(0, false);
	if (color) Recolor_Vertices(color, model->Get_Vertex_Count(), hsv_shift);
	color = model->Get_Color_Array(1, false);
	if (color) Recolor_Vertices(color, model->Get_Vertex_Count(), hsv_shift);

	// recolor textures: a cached copy first
	TextureClass *newtex, *oldtex;
	for (i = 0; i < material->Texture_Count(); i++) {
		oldtex = material->Peek_Texture(i);
		if (oldtex) {
			newtex = Recolored_Texture(this, oldtex, hsv);
			if (newtex) {
				model->Replace_Texture(oldtex, newtex);
				material->Replace_Texture(i, newtex);
				newtex->Release_Ref();
			}
		}
	}

	REF_PTR_RELEASE(material);
	REF_PTR_RELEASE(model);
}

// @0x48D710
void ENBAssetManager::Recolor_HLOD(RenderObjClass *robj, float *hsv)	// NAME 5: EA hueshift enbassetmgr.h
{
	int count = robj->Get_Num_Sub_Objects();
	for (int i = 0; i < count; ++i) {
		RenderObjClass *sub = robj->Get_Sub_Object(i);
		switch (sub->Class_ID()) {
			case RenderObjClass::CLASSID_MESH:
				Recolor_Mesh(sub, hsv);
				break;
			case RenderObjClass::CLASSID_HLOD:
				Recolor_HLOD(sub, hsv);
				break;
			case RenderObjClass::CLASSID_PARTICLEEMITTER:
				Recolor_ParticleEmitter(sub, hsv);
				break;
			case RenderObjClass::CLASSID_SPHERE:
				Recolor_Sphere(sub, hsv);
				break;
			case RenderObjClass::CLASSID_RING:
				Recolor_Ring(sub, hsv);
				break;
		}
		if (sub != NULL) {
			sub->Release_Ref();
		}
	}
}

// @0x48D850
void ENBAssetManager::Recolor_ParticleEmitter(RenderObjClass *robj, float *hsv)	// NAME 5: EA hueshift enbassetmgr.h
{
	unsigned int i;
	ParticleEmitterClass *emit = (ParticleEmitterClass *)robj;
	ParticlePropertyStruct<Vector3> colors;

	emit->Get_Color_Key_Frames(colors);
	Recolor(colors.Start, *(const Vector3 *)hsv);
	Recolor(colors.Rand, *(const Vector3 *)hsv);
	for (i = 0; i < colors.NumKeyFrames; i++) {
		Recolor(colors.Values[i], *(const Vector3 *)hsv);
	}
	emit->Reset_Colors(colors);

	if (colors.KeyTimes != NULL) {
		delete colors.KeyTimes;
	}
	if (colors.Values != NULL) {
		delete colors.Values;
	}

	TextureClass *tex = emit->Get_Texture();
	if (tex != NULL) {
		TextureClass *newtex = Recolored_Texture(this, tex, hsv);
		if (newtex != NULL) {
			emit->Set_Texture(newtex);
			newtex->Release_Ref();
		}
		tex->Release_Ref();
	}
}

// @0x48DD00
void ENBAssetManager::Recolor_Sphere(RenderObjClass *robj, float *hsv)	// NAME 3 [Recolor_Sphere]: EA hueshift style for CLASSID_SPHERE
{
	SphereRenderObjClass *sphere = (SphereRenderObjClass *)robj;
	SphereColorChannelClass &channel = sphere->Get_Color_Channel();
	int count = channel.Get_Key_Count();
	if (count > 0) {
		for (int i = 0; i < count; ++i) {
			SphereColorChannelClass::KeyClass key = channel.Get_Key(i);
			Vector3 color = key.Get_Value();
			Recolor(color, *(const Vector3 *)hsv);
			channel.Set_Key_Value(i, color);
		}
	} else {
		Vector3 color = sphere->Get_Color();
		Recolor(color, *(const Vector3 *)hsv);
		sphere->Set_Color(color);
	}
	TextureClass *tex = sphere->Peek_Texture();
	if (tex != NULL) {
		TextureClass *newtex = Recolored_Texture(this, tex, hsv);
		if (newtex != NULL) {
			sphere->Set_Texture(newtex);
			newtex->Release_Ref();
		}
	}
}

// @0x48E0F0
void ENBAssetManager::Recolor_Ring(RenderObjClass *robj, float *hsv)	// NAME 3 [Recolor_Ring]: EA hueshift style for CLASSID_RING
{
	RingRenderObjClass *ring = (RingRenderObjClass *)robj;
	RingColorChannelClass &channel = ring->Get_Color_Channel();
	int count = channel.Get_Key_Count();
	if (count > 0) {
		for (int i = 0; i < count; ++i) {
			RingColorChannelClass::KeyClass key = channel.Get_Key(i);
			Vector3 color = key.Get_Value();
			Recolor(color, *(const Vector3 *)hsv);
			channel.Set_Key_Value(i, color);
		}
	} else {
		Vector3 color = ring->Get_Color();
		Recolor(color, *(const Vector3 *)hsv);
		ring->Set_Color(color);
	}
	TextureClass *tex = ring->Peek_Texture();
	if (tex != NULL) {
		TextureClass *newtex = Recolored_Texture(this, tex, hsv);
		if (newtex != NULL) {
			ring->Set_Texture(newtex);
			newtex->Release_Ref();
		}
	}
}

// @0x48E4E0
void ENBAssetManager::Recolor_Asset(RenderObjClass *robj, float *hsv)	// NAME 5: EA hueshift enbassetmgr.h
{
	switch (robj->Class_ID()) {
		case RenderObjClass::CLASSID_MESH:
			Recolor_Mesh(robj, hsv);
			break;
		case RenderObjClass::CLASSID_HLOD:
		{
			int count = robj->Get_Num_Sub_Objects();
			for (int i = 0; i < count; ++i) {
				RenderObjClass *sub = robj->Get_Sub_Object(i);
				Recolor_Asset(sub, hsv);
				if (sub != NULL) {
					sub->Release_Ref();
				}
			}
			break;
		}
		case RenderObjClass::CLASSID_PARTICLEEMITTER:
			Recolor_ParticleEmitter(robj, hsv);
			break;
		case RenderObjClass::CLASSID_SPHERE:
			Recolor_Sphere(robj, hsv);
			break;
		case RenderObjClass::CLASSID_RING:
			Recolor_Ring(robj, hsv);
			break;
	}
}

// @0x48E620
bool Unknown_48E620(RenderObjClass *robj)
{
	bool found = false;
	if (robj->Class_ID() == RenderObjClass::CLASSID_MESH) {
		MeshModelClass *model = ((MeshClass *)robj)->Get_Model();
		for (int pass = 0; pass < model->Get_Pass_Count(); ++pass) {
			for (int stage = 0; stage < MeshMatDescClass::MAX_TEX_STAGES; ++stage) {
				TextureClass *tex = model->Peek_Single_Texture(pass, stage);
				if (tex != NULL) {
					StringClass name((const char *)tex->Get_Texture_Name());
					_strlwr(name.Peek_Buffer());
					if (	strncmp(name, "plt_at", 6) == 0 || strncmp(name, "plt3505", 7) == 0 ||
							strncmp(name, "plt9601", 7) == 0 || strncmp(name, "plt9703", 7) == 0) {
						found = true;
					}
				}
				if (found) break;
			}
			if (found) break;
		}
		REF_PTR_RELEASE(model);
	} else {
		int count = robj->Get_Num_Sub_Objects();
		std::vector<RenderObjClass *> remove_list;
		for (int i = 0; i < count; ++i) {
			RenderObjClass *sub = robj->Get_Sub_Object(i);
			if (Unknown_48E620(sub)) {
				remove_list.push_back(sub);
			}
			REF_PTR_RELEASE(sub);
		}
		for (unsigned j = 0; j < remove_list.size(); ++j) {
			robj->Remove_Sub_Object(remove_list[j]);
		}
	}
	return(found);
}

// @0x48E9D0
void Unknown_48E9D0(RenderObjClass *robj)
{
	if (robj->Class_ID() == RenderObjClass::CLASSID_MESH) {
		MeshClass *mesh = (MeshClass *)robj;
		if (mesh->Get_Num_Polys() > 30) {
			MeshModelClass *model = mesh->Peek_Model();
			if (!model->Has_Cull_Tree() && !model->Get_Flag(MeshGeometryClass::SKIN)) {
				WWPROFILE("Generate Culling Tree");
				mesh->Generate_Culling_Tree();
			}
		}
		if (!robj->Is_Translucent() && (robj->Get_Collision_Type() & (COLLISION_TYPE_PHYSICAL | COLLISION_TYPE_PROJECTILE)) == 0) {
			robj->Set_Collision_Type(COLLISION_TYPE_3);
		}
	} else {
		int count = robj->Get_Num_Sub_Objects();
		for (int i = 0; i < count; ++i) {
			WWPROFILE("Post_Process_Sub");
			RenderObjClass *sub = robj->Get_Sub_Object(i);
			Unknown_48E9D0(sub);
			if (sub != NULL) {
				sub->Release_Ref();
			}
		}
	}
}


































// @0x48EB90
void ENBAssetManager::Post_Process(RenderObjClass *robj)
{
	WWPROFILE("ENBAssetManager::Post_Process");
	FAIL_IF(!robj) return;
	Unknown_48E9D0(robj);
	const char *name = robj->Get_Name();
	if (!DX8Wrapper::Get_Current_Caps()->Support_TnL() && strnicmp(name, "plt", 3) == 0) {
		Unknown_48E620(robj);
	}
}

// @0x48ECA0
void ENBAssetManager::Unknown_48ECA0(TextureClass **textures, unsigned count)
{
	for (unsigned i = 0; i < count; ++i) {
		TextureClass *tex = textures[i];
		const char *name = tex->Get_Texture_Name();
		char lower[256];
		strcpy(lower, name);
		_strlwr(lower);
		char hsv_name[256];
		const Vector3 &hsv = tex->Get_HSV_Shift();
		sprintf(hsv_name, "#%s!H%gS%gV%g", lower, hsv.X, hsv.Y, hsv.Z);
		if (TextureHash.Get(hsv_name)) {
			TextureHash.Remove(hsv_name);
		} else {
			if (TextureHash.Get(name)) {
				TextureHash.Remove(name);
			}
		}
		textures[i]->Release_Ref();
	}
}

// @0x48F1A0
void ENBAssetManager::Release_Unused_Textures(void)
{
	for (;;) {
		unsigned count = 0;
		TextureClass *temp_textures[256];
		HashTemplateIterator<StringClass,TextureClass*> ite(TextureHash);
		for (ite.First(); !ite.Is_Done(); ite.Next()) {
			TextureClass *tex = ite.Peek_Value();
			if (tex->Num_Refs() == 1) {
				temp_textures[count++] = tex;
				if (count == 256) {
					break;
				}
			}
		}
		if (count == 0) {
			return;
		}
		Unknown_48ECA0(temp_textures, count);
	}
}

/*
**	Notes
**
**	0x48B070 is the constructor WinMain (0x526DE0) calls after new (0x408C bytes). The destructor 0x48B0D0 is declared (it stores
**	the table before the base's; the compiler's would only jump); the deleting destructor 0x48B0A0 is the compiler's.
**	0x48B0F0 (slot +0x14) inlines Post_Process (0x48EB90: its FAIL_IF(!robj) on line 635 drops out after the NULL test); the
**	if / else with robj = NULL is what puts the zero in ebx and joins both paths (a return NULL or a void Post_Process
**	duplicates the epilogue). 0x48F2C0 (slot +0x10) is Free_Assets' prototype loop without its profile, then the base's
**	Release_Unused_Assets. 0x48F1A0 (slot +0x38) is the base's Release_Unused_Textures, except that it hands each batch of
**	256 textures with one reference to 0x48ECA0 and starts over until a pass finds none; 0x48ECA0 removes each texture
**	under its HSV name ('#name!H%gS%gV%g', lower case) or else under its plain name, then releases it (exact with the
**	HSV-name branch first and bare `if (Get(...))` tests: `== NULL` / `!= NULL` tests or a named result miss). Post_Process' helpers:
**	0x48E9D0 builds a culling tree for each mesh over 30 polygons (not skins) and gives the opaque meshes without physical or
**	projectile collision COLLISION_TYPE_3, recursing into sub-objects; 0x48E620 returns true for a mesh with a 'plt_at' /
**	'plt3505' / 'plt9601' / 'plt9703' texture and removes such meshes from their containers (only without hardware T&L). The
**	single return(found) is what keeps found in memory for the container path.
**	ENBPrototypeClass (0x48AED0 ctor, 0x48AF50/0x48AF80 dtors, 0x48B010 Get_Name, 0x48B030 Get_Class_ID, 0x48B050 Create)
**	and +0x78 Create_Render_Obj(name, scale, hsv) 0x48B220 reconstructed (EAM4): the prototype class exact (table 0xAEA5A0);
**	Create_Render_Obj calls Post_Process on cached or created objects, calls Make_Unique (VC6 inlines it one level and
**	keeps its recursive call out of line; a hand-copied switch there inlined it twice: 40 calls for 35), scales, recolors
**	via Recolor_Mesh / Recolor_HLOD / Recolor_ParticleEmitter / Recolor_Sphere / Recolor_Ring, Post_Processes, creates the
**	prototype, releases the object reference and returns proto->Create().
**	Its helpers Make_Mesh_Unique 0x48BCF0 / Make_HLOD_Unique 0x48BDC0 / Make_Unique 0x48BE60 are: make a mesh's
**	geometry and / or colours (vertex materials, both colour arrays) unique, for a mesh or recursively for an HLOD's meshes;
**	a switch on Class_ID is what puts the HLOD test first.
**	0x48D710 (an HLOD's sub-objects) and 0x48E4E0 (one object, recursing into HLODs) hand the HSV shift to the per-class
**	helpers 0x48D1F0 (mesh), 0x48D850 (particle emitter), 0x48DD00 (sphere), 0x48E0F0 (ring); 0x48D1F0 is written (EAM3).
**	The mesh helper is Renegade's ww3d2/hueshift/enbassetmgr.cpp Recolor_Mesh (EA's published code: Recolor_Vertex_Material
**	0x48BF20 and Recolor_Vertices 0x48C590 inlined into it, Recolor out of line) except that it first asks 0x48CF90 for a
**	cached copy under '#name!H%gS%gV%g' (lower case, with a reference) and only then 0x48CB40 ('Recolor_Texture()'), which
**	makes a new texture from the same file with the old one's format, filters and address modes and Set_HSV_Shift (shifted
**	when it loads, not copied from a surface as in Renegade), and caches it. The cache-or-new choice is an inline helper
**	with two returns: that single merge is what lets the client drop REF_PTR_SET's null test. 0x48CF90 must be defined
**	before 0x48CB40 in this file, or it loses its EH frame.
*/
