// Earth & Beyond client (reconstruction): the galaxy map picture-in-picture (notes at the end)
#include <math.h>
#include "galaxy_map_pip.h"
#include "sclient.h"
#include "galaxy_map_client.h"
#include "fontdatabase.h"
#include "local_interface_sort.h"
#include "aux_roots.h"
#include "..\shared\g_assert.h"
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\auxdata\delta_template.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\wwlib\argv.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\scene.h"
#include "..\..\ww3d2\camera.h"
#include "..\..\ww3d2\assetmgr.h"
#include "..\..\ww3d2\matinfo.h"
#include "..\..\ww3d2\mapper.h"
#include "..\..\ww3d2\vertmaterial.h"
#include "..\..\ww3d2\ww3d.h"
#include "wwmath.h"

void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// ro_funcs.cpp's (avatar.cpp declares it the same)

// .data 0xB7923C / 0xB79240: the two angles 0x53FDA0 chooses between  // NAME 1: neutral (D10)
static float Unknown_B7923C = DEG_TO_RADF(45.0f);
static float Unknown_B79240 = DEG_TO_RADF(20.0f);

// .rdata 0xAEEE48: the ease's speed curve, a step per 0.1 of the ease; 0x53F9E0 reads it up to [10]  // NAME 1: neutral (D10)
static const float Unknown_AEEE48[19] = {
	0.2f, 0.8f, 0.9f, 1.0f, 1.0f, 1.0f, 0.9f, 0.6f, 0.4f, 0.1f, 0.05f
};

// 0x5404E0 reads the field through this: the registers after the call match only so (the test folds away)
static inline int Unknown_Sector_Value(DeltaTemplateClass<int> *sector)	// NAME 1: neutral (D10)
{
	if (!sector) return(0);
	return(sector->Value);
}

static inline GalaxyMapClientClass *Unknown_Map(SClient *client) { return(client->GalaxyMap); }	// NAME 1: neutral (D10)
template<class C> inline C *Unknown_Find_Valid(DeltaCompoundClass *root, const NameClass &name) { C *t = Unknown_Find_Field<C>(root, name); if (!t) return(NULL); if (!t->Unknown70) return(NULL); return(t); }	// NAME 1: neutral (D10): avatar_pip.cpp's

// @0x53F390
GalaxyMapPipClass::GalaxyMapPipClass(SClient *client) :
	MapPipClass(client, false),
	Debug(false),
	Render(NULL),
	Ease(-1.0f),
	EaseRate(-1.0f)
{
	Unknown_BE67BC->Load("fc_radar.ini");
	Render = new GalaxyMapRenderClass(Unknown_Map(client));
	ArgvClass args(false, false);
	if (args.Find("-GALAXY_MAP_DEBUG")) {
		Debug = true;
	}
}

// @0x53F4F0
void GalaxyMapPipClass::Unknown_20(int arg)
{
	Unknown_5404E0(false);
}

// @0x53F540
GalaxyMapPipClass::~GalaxyMapPipClass()
{
	if (Grid) {
		Grid->Remove();
		Grid = NULL;
	}
	if (GalaxyBackground) {
		GalaxyBackground->Remove();
		GalaxyBackground = NULL;
	}
	delete Render;
	Render = NULL;
	Unknown_BE67BC->Unload("fc_radar.ini");
}

// @0x53F6F0
GadgetClass *GalaxyMapPipClass::Unknown_18()
{
	return(new GadgetGalaxyMapClass(this));
}

// @0x53F770
bool GalaxyMapPipClass::Unknown_04(const RefCountPtr<InterfaceClass> &iface_ptr, const RefCountPtr<RenderObjClass> &vp, int arg)
{
	if (!MapPipClass::Unknown_04(iface_ptr, vp, arg)) {
		return(false);
	}
	Unknown_53FE30();
	Unknown_10(vp);
	Camera->Set_Clip_Planes(0.01f, 1000.0f);
	return(true);
}

// @0x53F7E0
bool GalaxyMapPipClass::Unknown_08()
{
	return(MapPipClass::Unknown_08() ? true : false);
}

// @0x53F800
bool GalaxyMapPipClass::Unknown_0C(bool on)
{
	if (!MapPipClass::Unknown_0C(on)) {
		return(false);
	}
	if (on) {
		if (Debug) {
			Unknown_53FE30();
			Render->Unknown_7A7070(true);
		} else {
			Render->Unknown_7A7070(false);
		}
		Render->Unknown_7A6AB0(Scene, Camera);
		Unknown_5404E0(true);
	} else {
		Render->Unknown_7A6DD0();
	}
	return(true);
}

// @0x53F920
void GalaxyMapPipClass::Unknown_1C(int arg)
{
	if (Grid->Peek_Scene()) {
		Grid->Remove();
		return;
	}
	InterfaceSortClass sort(Interface.Peek());
	Grid->Add(Scene.Peek());
}

// @0x53F9E0
void GalaxyMapPipClass::Unknown_53F9E0()
{
	if (Ease >= 0.0f) {
		float x = Ease * 10.0f;
		int i = (int)WWMath::Clamp(x, 0.0f, 9.0f);
		float a = Unknown_AEEE48[i];
		float b = Unknown_AEEE48[i + 1];
		float t = ((x - (float)i) * (b - a) + a) * WW3D::Get_Frame_Time() * EaseRate + Ease;
		Ease = t;
		if (t >= 1.0f) {
			Ease = -1.0f;
			FocalPoint = Target;
		} else {
			FocalPoint.Unknown_7A5ED0(Start, Target, t);
		}
		FocalPoint.Unknown_7A5890(Camera);
	}
	Render->Unknown_7A7070(false);
	if (Unknown_693020()) {
		const Vector2 &p = ViewportGadget->Position;
		Render->Unknown_7A72C0(p.X, p.Y);
	} else {
		Render->Unknown_7A7450();
	}
	DeltaPlayerClass *aux = ((SClient *)Client->As_Space_Client())->AuxData;
	if (aux) {
		DeltaTemplateClass<int> *sector = Unknown_Find_Field<DeltaTemplateClass<int> >(aux, "SectorNum");
		if (sector && sector->Unknown70 > 1) {
			Unknown_5404E0(false);
		}
	}
}

// @0x53FCA0
void GalaxyMapPipClass::Unknown_53FCA0(float arg)
{
	Ease = -1.0f;
	FocalPoint.MaxDistance = Render->Bounds.Radius * 3.0f;
	FocalPoint.Unknown_7A5E10(arg);
	FocalPoint.Unknown_7A5890(Camera);
}

// @0x53FD10
void GalaxyMapPipClass::Unknown_53FD10(const Vector2 &arg)
{
	Ease = -1.0f;
	FocalPoint.Unknown_7A5980(arg, Render->Unknown14, Render->Unknown1C);
	FocalPoint.Unknown_7A5890(Camera);
}

// @0x53FD60
void GalaxyMapPipClass::Unknown_53FD60(const Vector2 &arg)
{
	Ease = -1.0f;
	FocalPoint.Unknown_7A5AA0(arg);
	FocalPoint.Unknown_7A5890(Camera);
}

// @0x53FDA0
void GalaxyMapPipClass::Unknown_53FDA0(bool arg)
{
	SphereClass v; Render->Unknown_7A83D0(v, arg);
	Unknown_5406A0(v, arg ? Unknown_B79240 : Unknown_B7923C);
}

// @0x53FE10
bool GalaxyMapPipClass::Unknown_53FE10()
{
	return(Render->Unknown_7A6A70());
}

// @0x53FE30
void GalaxyMapPipClass::Unknown_53FE30()
{
	GalaxyMapClientClass *map = Client->GalaxyMap;
	if (Debug) {
		map->Load_Ini();
	}
	if (GalaxyBackground) {
		GalaxyBackground->Remove();
		GalaxyBackground = NULL;
	}
	GalaxyBackground = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj("sm_galaxy"));






































































































































	FAIL_IF(!GalaxyBackground) return;
	Unknown_496AE0(GalaxyBackground.Peek(), true);
	InterfaceSortClass sort(Interface.Peek());
	GalaxyBackground->Add(Scene.Peek());
	if (Grid) {
		Grid->Remove();
		Grid = NULL;
	}
	Grid = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj("sm_grid"));

	FAIL_IF(!Grid) {
		return;
	}
	Unknown_496AE0(Grid.Peek(), true);
	int size = map->Size;
	float scale = (float)size;
	FAIL_IF(scale < WWMATH_EPSILON) {
		scale = 1.0f;
	}
	int count = GalaxyBackground->Get_Num_Sub_Objects();
	for (int i = 0; i < count; i++) {
		RefCountPtr<RenderObjClass> sub = Create_Get(GalaxyBackground->Get_Sub_Object(i));
		if (sub) {
			sub->Scale(scale, scale, scale);
		}
	}
	scale = scale + scale;
	Grid->Scale(scale, scale, 1.0f);
	RefCountPtr<MaterialInfoClass> minfo = Create_Get(Grid->Get_Material_Info());




	G_ASSERT(minfo);
	if (minfo) {
		RefCountPtr<VertexMaterialClass> vmat = Create_Get(minfo->Get_Vertex_Material(0));


		G_ASSERT(vmat);
		if (vmat) {
			RefCountPtr<ScaleTextureMapperClass> mapper = Create_NEW(new ScaleTextureMapperClass(Vector2(scale, scale), 0));
			vmat->Set_Mapper(mapper.Peek());
		}
	}
	Matrix3D tm(true);
	Grid->Set_Transform(tm);
	tm.Translate_Z(scale * -0.025f);
	GalaxyBackground->Set_Transform(tm);
}

// NOT EXACT: only UnknownRTTIClass::Cast: the client calls it out of line here, ours expands it, and one register follows (see notes)
// @0x5404E0
void GalaxyMapPipClass::Unknown_5404E0(bool arg)
{
	Render->Unknown_7A7070(false);
	DeltaPlayerClass *aux = ((SClient *)Client->As_Space_Client())->AuxData;
	if (aux) {
		DeltaTemplateClass<int> *sector = Unknown_Find_Valid<DeltaTemplateClass<int> >(aux, "SectorNum");
		if (sector) {
			Render->Unknown_7A8770(Unknown_Sector_Value(sector));
		}
	}
	SphereClass v; Render->Unknown_7A89A0(v);
	Unknown_5406A0(v, Unknown_B79240);
	if (arg) {
		Ease = -1.0f;
		FocalPoint = Target;
		FocalPoint.Unknown_7A5890(Camera);
	}
}

// @0x5406A0
void GalaxyMapPipClass::Unknown_5406A0(const SphereClass &sphere, float angle)
{
	Matrix3D tm(sphere.Center);
	tm.Rotate_X(angle);
	CameraClass *camera = Camera.Peek();
	float vfov = camera->Get_Vertical_FOV();
	float hfov = camera->Get_Horizontal_FOV();
	float fov = (hfov > vfov) ? Camera->Get_Vertical_FOV() : Camera->Get_Horizontal_FOV();
	Target.Unknown_7A57A0(tm, sphere.Radius / (float)tan(fov * 0.5f));
	Start = FocalPoint;
	Ease = 0.0f;
	EaseRate = 0.001f;
}

// @0x5408B0
void GalaxyMapPipClass::Unknown_24(int arg)
{
	GalaxyMapClientClass *map = Client->GalaxyMap;

























	FAIL_IF(!map) {
		return;
	}
	if (map->Unknown_7A5540(Client)) {
		((SClient *)Client->As_Space_Client())->Unknown_74D990(0, "Galaxymap PIP will now show the whole galaxy.", 0);
	} else {
		((SClient *)Client->As_Space_Client())->Unknown_74D990(0, "Galaxymap PIP will now show only areas that you have explored.", 0);
	}
}

/*
**	Reconstruction notes
**
**	The object's boundaries in the client: __FILE__ 0xB792D4 ('e:\build\beta\G\app\client\galaxy_map_pip.cpp')
**	in 0x53FE30 and 0x5408B0, with asserts on lines 357-472. Its .data starts at 0xB7923C (the two angles,
**	read by 0x53FDA0 and 0x5404E0; 0xB79238 is the previous object's) and its literals follow in definition
**	order, each function's in reverse: the constructor's '-GALAXY_MAP_DEBUG' 0xB79248 and 'fc_radar.ini' 0xB79260,
**	0x53F9E0's 'SectorNum' 0xB79270, 0x53FE30's 0xB7927C-0xB7930C, 0x5408B0's up to 0xB7939C. So 0x53F9E0, which
**	has no assert of its own, lies inside the object; the functions 0x53FCA0-0x53FE10 share its members
**	(+0x10C, +0x54, +0x58, +0x0C) and sit between it and 0x53FE30, and the atexit initialiser 0x53F330
**	ends the object before. Its .rdata: the curve 0xAEEE48, the table 0xAEEE94, the literals 9.0 0xAEEEC4 and
**	-0.025 0xAEEEC8. The curve's last eight entries (19 floats up to the table) are zero; 0x53F9E0 reads [0]..[10].
**
**	The sphere 0x7A83D0 / 0x7A89A0 return: 16 bytes, 0x5406A0 reads it as a centre and a radius (radius / tan(fov / 2),
**	Matrix3D(const Vector3 &) inlined).
**
**	0x5404E0's residue: the Cast. 0x53F9E0's find of the same field expands it, 0x5404E0's calls it (0x40C1C1); the rest
**	of the function is the client's. Tried (attempts.csv): Find_Valid tail and non-tail, a NameClass local, helpers
**	around the find with early returns (they let callers inline 0x5404E0 instead), helpers before it, dead finds.
*/
