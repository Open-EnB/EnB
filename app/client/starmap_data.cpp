// starmap_data.cpp: the icon and the top-view mesh a client object shows on the starmap (0x7BE600-0x7BF810); notes at the end
#include <string>
#include "starmap_data.h"
#include "radar_system_data_cache.h"
#include "navigation_data.h"
#include "client.h"
#include "co.h"
#include "assetmgr.h"
#include "rendobj.h"
#include "dynamesh.h"
#include "shader.h"
#include "matrix3d.h"
#include "sphere.h"
#include "vector2i.h"
#include "..\shared\g_assert.h"

extern const int Unknown_B9F38C;	// the mesh's polys  // NAME 1: neutral
extern const int Unknown_B9F390;	// the mesh's vertices  // NAME 1: neutral
class CameraManagerClass { public: static float Unknown_79B6B0(); };	// camera_manager.h's 0x79B6B0 (a constant), as far as this file calls it
const char *Unknown_7BF4C0(ClientObjectClass *co);
float Unknown_7BF630();

// RObj2Texture (the client's profile name for 0x497700's Render_TextureMap; not rebuilt): renders an object into a texture
class RObj2Texture
{
public:
	RObj2Texture(const Vector2i &size, const Vector2 &a, const Vector2 &b);	// 0x4973E0  // NAME 2 [size]: 0x80 or 0x100
	~RObj2Texture();														// 0x4976B0
	TextureClass *Render_TextureMap(RenderObjClass *robj, const SphereClass &sphere);	// 0x497700

	int Unknown00;				// +0x00
	bool Unknown04;				// +0x04
	int Unknown08;				// +0x08
	char Unknown0C[0x2C - 0x0C];	// +0x0C
};

static Vector2i SmallTextureSize(0x80, 0x80);	// 0xBE8778  // NAME 2: what 0x7BEF50 uses it for
static Vector2i LargeTextureSize(0x100, 0x100);	// 0xBE8780  // NAME 2: what 0x7BEF50 uses it for
static float PCIconSize = 0.0f;					// 0xBE8788  // NAME 2: what it holds

// @0x7BE640
StarmapDataClass::StarmapDataClass(ClientObjectClass *co) :
	RadarIconDataClass(co, NULL),
	WorldPosition(0.0f, 0.0f, 0.0f),
	Mesh(NULL),
	Texture(NULL),
	Radius(0.0f),
	MeshSize(-1.0f),
	Unknown38(false)
{
	Set_Icon(Unknown_7BF4C0(co));
	Get_Client()->ObjectsChanged = true;
}

// @0x7BE780
StarmapDataClass::~StarmapDataClass()
{
	Remove_From_Scene();
}

// @0x7BE860
void StarmapDataClass::Set_Icon(const char *icon_name)
{
	RadarIconDataClass::Set_Icon(icon_name);
	RefCountPtr<RenderObjClass> robj = Get_Render_Obj();
	if (robj) {
		robj->Set_Sort_Level(3);
	}
}

// @0x7BE900
bool StarmapDataClass::Remove_From_Scene()
{
	if (Mesh && Mesh->Peek_Scene()) {
		Mesh->Remove();
	}
	return(RadarIconDataClass::Remove_From_Scene());
}

// @0x7BE940
RefCountPtr<RenderObjClass> StarmapDataClass::Get_Render_Obj()
{
	return(Mesh);
}

// @0x7BE970
bool StarmapDataClass::Update(RadarSystemDataCacheClass *cache, const Vector3 &pos, const RefCountPtr<SceneClass> &scene)
{
	bool changed = false;
	if (WorldPosition != pos) {
		changed = true;
		WorldPosition = pos;
	}
	ClientObjectClass *co = Peek_Client_Object();




















































	G_ASSERT(co);
	if (Add_To_Scene(scene)) {
		changed = true;
		Radius = co ? co->Unknown_30() : 0.0f;
	}
	NavigationDataClass *nav = cache->Get_Navigation_Data();
	if (nav) {
		if (nav->Unknown0C) {
			if (Unknown38) {
				Set_Icon(Unknown_7BF4C0(co));
				Get_Client()->ObjectsChanged = true;
				Unknown38 = false;
			}
		} else if (!Unknown38) {
			Set_Icon("m_unknown");
			Get_Client()->ObjectsChanged = true;
			Unknown38 = true;
		}
	}
	Get_Color();
	return(changed);
}

// @0x7BEAD0
void StarmapDataClass::Update_Transform(const Vector3 &pos, const Vector2 &position, float angle, float size, bool rotate_icon)
{
	Matrix3D tm(pos);
	if (rotate_icon) {
		tm.Rotate_Z(angle);
	}
	Get_Icon_RObj()->Set_Transform(tm);
	Position = position;
	if (!Unknown38) {
		float s = size * Radius;
		s = s + s;
		if (Unknown_7BF630() < s) {
			if (Mesh == NULL) {
				Create_Mesh();
			}
			if (Mesh == NULL) {
				return;
			}
			if (Mesh->Peek_Scene() == NULL) {
				Mesh->Add(Peek_Scene());
			}
			if (size != MeshSize) {
				float half = size * Radius;
				Mesh->Resize(Unknown_B9F38C, Unknown_B9F390);
				Mesh->Set_Texture(Texture.Peek());
				Mesh->Set_Shader(ShaderClass::_PresetAlpha2DShader);
				float neg = -half;
				Mesh->Begin_Tri_Strip();
				Mesh->Vertex(neg, neg, 0, 0.0f, 1.0f);
				Mesh->Vertex(half, neg, 0, 1.0f, 1.0f);
				Mesh->Vertex(neg, half, 0, 0.0f, 0.0f);
				Mesh->Vertex(half, half, 0, 1.0f, 0.0f);
				Mesh->End_Tri_Strip();
				MeshSize = size;
			}
			if (!rotate_icon) {
				tm.Rotate_Z(angle);
			}
			Mesh->Set_Transform(tm);
			return;
		}
	}
	if (Mesh && Mesh->Peek_Scene()) {
		Mesh->Remove();
	}
}

// @0x7BEF20
Vector3 StarmapDataClass::Get_Position()
{
	return(Get_Icon_RObj()->Get_Position());
}

class StarmapMeshClass : public DynamicMeshClass  // NAME 1: neutral (0xF4 bytes; vtable 0xB069B8)
{
public:
	StarmapMeshClass(int max_polys, int max_verts) : DynamicMeshClass(max_polys, max_verts) {}
	virtual bool Cast_Ray(RayCollisionTestClass &raytest) { return(false); }	// +0xD0 0x7BF450 (not hit by the mouse)
};

// NOT EXACT: frame and registers (the client keeps 0 in ebx and is_ship on the stack, the old transform above the rotation)
// @0x7BEF50
void StarmapDataClass::Create_Mesh()
{
	if (Mesh == NULL) {
	ClientObjectClass *co = Peek_Client_Object();



















	if (!co) { G_ASSERT(co); } else {
	RenderObjClass *robj = co->RenderObject;
	if (!robj) { G_ASSERT(robj); } else {
	bool is_ship = false;
	if (co->Unknown_6F5A40()) {
		is_ship = co->Unknown_7008B0();
	}
	Matrix3D old_tm = robj->Get_Transform();
	Matrix3D rotation(true);
	rotation.Rotate_Z(1.5707963705062866);
	robj->Set_Transform(rotation);
	SphereClass sphere;
	if (is_ship) {
		float radius = CameraManagerClass::Unknown_79B6B0();
		sphere.Center.Set(0, 0, 0);
		sphere.Radius = radius * 0.70710677f * 1.5;
	} else {
		sphere = robj->Get_Bounding_Sphere();
		sphere.Radius *= 0.70710677f;
	}
	NavigationDataClass *nav = Get_Navigation_Data(Peek_Client_Object(), false);
	const Vector2i &size = (nav && nav->Unknown14) ? LargeTextureSize : SmallTextureSize;
	RObj2Texture r2t(size, Vector2(0, 0), Vector2(0, 0));
	r2t.Unknown04 = true;
	r2t.Unknown08 = 4;
	Texture = Create_Peek(r2t.Render_TextureMap(robj, sphere));
	Mesh = Create_Peek((DynamicMeshClass *)new StarmapMeshClass(Unknown_B9F38C, Unknown_B9F390));
	Mesh->Disable_Sort();
	Mesh->Set_Sort_Level(4);
	MeshSize = -1.0f;
	robj->Set_Transform(old_tm);
	}
	}
	}
}

// @0x7BF4C0
const char *Unknown_7BF4C0(ClientObjectClass *co)
{






















	FAIL_IF(!co) return("m_dead");
	switch (co->Type) {
		case 0: return("m_npc");
		case 1: return("m_pc");
		case 2: return("m_cap");
		case 3: return("m_planet");
		case 9: return("m_misile");
		case 4: return("m_item");
		case 10: return("m_nebula");
		case 0x22:
		case 0x26: return("m_resrce");
		case 11: return("m_gate");
		case 12: return("m_sbase");
		case 0x1B: return("m_mine");
		case 0x23: return("m_drone");
		case 0x25: return("m_deco");
		case 0x18: return("m_build");
		case 0x19: break;
		default:






			G_ASSERT(false);
			break;
	}
	return("m_dead");
}

// @0x7BF630
float Unknown_7BF630()
{
	if (PCIconSize == 0.0f) {
		RefCountPtr<RenderObjClass> robj = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj("m_pc"));

		FAIL_IF(!robj) return(0.0f);
		PCIconSize = robj->Get_Bounding_Sphere().Radius * 2.0f;
	}
	return(PCIconSize);
}

const int Unknown_B9F38C = 2;
const int Unknown_B9F390 = 4;

/*
**	Notes
**
**	The client's __FILE__ 0xB9F3A4. The object opens with the two Vector2i statics' initialisers at 0x7BE600 and
**	0x7BE620; 0x7BE5A0, before them, closes radar_system_data_cache.cpp (it numbers two locale facets from 0xBE3B44).
*/
