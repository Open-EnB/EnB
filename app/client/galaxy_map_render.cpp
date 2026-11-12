// Earth & Beyond client (reconstruction): app\Client\galaxy_map_render.cpp (notes at the end)
#include <math.h>
#include <algorithm>
#include "galaxy_map_render.h"
#include "galaxy_map_client.h"
#include "fontdatabase.h"
#include "fontmanager.h"
#include "..\common\galaxy_map.h"
#include "..\..\wwlib\ini.h"
#include "..\..\ww3d2\scene.h"
#include "..\..\ww3d2\camera.h"
#include "..\..\ww3d2\assetmgr.h"
#include "..\..\ww3d2\coltest.h"
#include "colmath.h"
#include "lineseg.h"
#include "aabox.h"
#include "wwmath.h"
#include "matrix3d.h"

extern FontStringFilesClass *Unknown_BE67BC;	// NAME 1: neutral (D10)
bool Unknown_496110(RenderObjClass *robj, float a, float b, float c);	// 0x496110 (ro_funcs.cpp): a model's highlight  // NAME 1 [a, b, c]: neutral
void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// ro_funcs.cpp's
RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj);	// 0x497300  // NAME 1 [robj]: as ui_avatr.cpp's
unsigned int Unknown_Get_Ticks();	// (unknown_root.h's)

static int Unknown_B9E934 = 9;	// .data 0xB9E934: the sector name's sort level  // NAME 1: neutral (D10)
static float Unknown_B9E93C = 1.5f;	// .data 0xB9E93C: how near a gate line picks  // NAME 1: neutral (D10)

typedef std::hash_map<int, RefCountPtr<GalaxyMapRenderSystemClass> > RenderSystemsType;	// NAME 1: what it holds
typedef std::hash_map<int, RefCountPtr<GalaxyMapRenderSectorClass> > RenderSectorsType;	// NAME 1: what it holds
typedef std::hash_map<int, RefCountPtr<GalaxyMapRenderGateClass> > RenderGatesType;		// NAME 1: what it holds
static GalaxyMapRenderSystemClass *New_Item(const RefCountPtr<GalaxyMapSystemClass> &data);	// 0x7A9370 (static: called without a thunk)  // NAME 1: what it does
static GalaxyMapRenderSectorClass *New_Item(const RefCountPtr<GalaxyMapSectorClass> &data);	// 0x7A93F0 (static)




















































































template <class CONTAINER, class T>
static bool Find_Item(RefCountPtr<T> &result, CONTAINER &container, int id)	// NAME 1: what it does
{
	CONTAINER::iterator it = container.find(id);
	if (it == container.end()) { return(false); }
	result = (*it).second;
	FAIL_IF(!result) { return(false); }
	return(true);
}

// NOT EXACT: written from the client's code (see attempts.csv)
template <class CONTAINER, class T, class DATA>
static bool Create_Item(RefCountPtr<T> &result, CONTAINER &container, const RefCountPtr<DATA> &data, const RefCountPtr<SceneClass> &scene)	// NAME 1: what it does
{
	int id = data->Get_Hash_ID();
	if (Find_Item(result, container, id)) { return(false); }
	result = Create_NEW(new T(data));
	container[id] = result;
	if (scene) { result->Add_To_Scene(scene); }
	return(true);
}









template <class CONTAINER>
static void Remove_Item(CONTAINER &container, const RefCountPtr<GalaxyMapRenderItemClass> &item)	// NAME 1: what it does
{
	CONTAINER::iterator it = container.find(item->Get_Data()->Get_Hash_ID());
	FAIL_IF(it == container.end()) { return; }
	item->Release_Item();
	container.erase(it);
}

template <class CONTAINER>
static void Clear_Items(CONTAINER &container)	// NAME 1: what it does
{
	for (CONTAINER::iterator it = container.begin(); it != container.end(); ) {
		CONTAINER::iterator here = it;
		++it;
		(*here).second->Release_Item();
		container.erase(here);
	}
}

template <class CONTAINER>
static void Remove_Stale_Items(CONTAINER &container)	// NAME 1: what it does
{
	for (CONTAINER::iterator it = container.begin(); it != container.end(); ) {
		RefCountPtr<GalaxyMapDataClass> data = (*it).second->Get_Data();
		if (data->Is_In_Galaxy_Map()) {
			++it;
		} else {
			CONTAINER::iterator here = it;
			++it;
			(*here).second->Release_Item();
			container.erase(here);
		}
	}
}







template <class CONTAINER>
static void Add_Items(CONTAINER &container, const RefCountPtr<SceneClass> &scene)	// NAME 1: what it does
{

	FAIL_IF(!scene) { return; }
	for (CONTAINER::iterator it = container.begin(); it != container.end(); ++it) {
		(*it).second->Add_To_Scene(scene);
	}
}

template <class CONTAINER>
static void Remove_Items(CONTAINER &container)	// NAME 1: what it does
{
	for (CONTAINER::iterator it = container.begin(); it != container.end(); ++it) {
		(*it).second->Remove_From_Scene();
	}
}

template <class T, class CONTAINER>
static void Find_Nearest(RefCountPtr<T> &result, CONTAINER &container, const LineSegClass &ray, const Vector3 &pos)	// NAME 1: what it does
{
	float best = FLT_MAX;
	result = NULL;
	for (CONTAINER::iterator it = container.begin(); it != container.end(); ++it) {
		RefCountPtr<T> item = *it;
		Vector3 contact;
		if (item->Intersect(ray, &contact)) {
			float d = (contact - pos).Length2();
			if (best > d) { best = d; result = item; }
		}
	}
}

































GalaxyMapRenderClass::GalaxyMapRenderClass(GalaxyMapClientClass *map) :
	GalaxyMap(map), SectorNum(0), Unknown14(FLT_MAX, FLT_MAX), Unknown1C(-FLT_MAX, -FLT_MAX), Version(0), Unknown28(0),
	Systems(100), Sectors(100), Gates(100), Point(0.0f, 0.0f, 0.0f), UnknownB8(1.0f), SystemZoomMinRadius(10.0f)
{
	Unknown_BE67BC->Load("fc_radar.ini");



	G_ASSERT(GalaxyMap);
	HomeSectorRObj = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj("sm_sector_home"));
	Load_Ini();
}

GalaxyMapRenderClass::~GalaxyMapRenderClass()
{
	Unknown_7A6750();
	Unknown_BE67BC->Unload("fc_radar.ini");
}

void GalaxyMapRenderClass::Unknown_7A6750()
{
	Unknown_7A9840();
	Updates.clear();
	Clear_Items(Gates);
	Clear_Items(Sectors);
	Clear_Items(Systems);
	Version = 0;
}

void GalaxyMapRenderClass::Load_Ini()
{
	INIClass ini("galaxy_map_c.ini");
	SystemZoomMinRadius = ini.Get_Float("Render", "SystemZoomMinRadius", SystemZoomMinRadius);
}

bool GalaxyMapRenderClass::Unknown_7A6A70()
{
	if (PickedSystem || PickedSector || PickedGate) { return(true); }
	return(false);
}



































void GalaxyMapRenderClass::Unknown_7A6AB0(const RefCountPtr<SceneClass> &scene, const RefCountPtr<CameraClass> &camera)
{




	FAIL_IF(!scene) { return; }
	FAIL_IF(!camera) { return; }
	Scene = scene;
	Camera = camera;
	Add_Items(Systems, Scene);
	Add_Items(Sectors, Scene);
	Add_Items(Gates, Scene);
}














void GalaxyMapRenderClass::Unknown_7A6DD0()
{
	FAIL_IF(!HomeSectorRObj) { return; }
	if (HomeSectorRObj->Peek_Scene()) { HomeSectorRObj->Remove(); }
	Unknown_7A7450();
	Remove_Items(Systems);
	Remove_Items(Sectors);
	Remove_Items(Gates);
	Scene = NULL;
	Camera = NULL;
}
















bool GalaxyMapRenderClass::Unknown_7A7070(bool arg)
{

	FAIL_IF(!GalaxyMap) { return(false); }
	if (arg) {
		Load_Ini();
		Unknown28 = 0;
		Version = 0;
	}
	int version = GalaxyMap->Version;
	if (version != Version && Unknown28 < Unknown_Get_Ticks()) {
		Unknown_7A8B70();
		Version = version;
		Unknown28 = Unknown_Get_Ticks() + 1000;
		Unknown_7A8770(SectorNum);
		Unknown_7A7D70(true);
		return(true);
	}
	for (std::vector<GalaxyMapRenderUpdateStruct>::iterator it = Updates.begin(); it != Updates.end(); ++it) {
		(*it).Item->Update();
	}
















	FAIL_IF(!HomeSectorRObj) { return(false); }
	Matrix3D tm = HomeSectorRObj->Get_Transform();
	tm.Rotate_Z(0.01f);
	HomeSectorRObj->Set_Transform(tm);
	return(false);
}
// NOT EXACT: written from the client's code (see attempts.csv)















void GalaxyMapRenderClass::Unknown_7A72C0(float x, float y)
{
	FAIL_IF(!Camera) { return; }
	Vector3 from;
	Vector3 to;
	Camera->Un_Project(to, Vector2((x + x) - 1.0f, -((y + y) - 1.0f)));
	Vector3 pos = Camera->Get_Position();
	Vector3 dir = to - pos;
	if (fabs(dir.Z) < WWMATH_EPSILON) {
		Point = pos;
	} else {
		float t = -pos.Z; t /= dir.Z;
		Point = pos + dir * t;
	}
	bool changed = Unknown_7A7560();
	Unknown_7A7D70(changed);
}

void GalaxyMapRenderClass::Unknown_7A7450()
{
	Unknown_7A7D70(Unknown_7A9840());
}

// NOT EXACT: written from the client's code (see attempts.csv)
bool GalaxyMapRenderClass::Unknown_7A7560()
{
	bool changed = false;
	Matrix3D tm = Camera->Get_Transform();
	if (fabs(tm[2][3]) < 25.0f) {
		tm.Translate_Z(25.0f);
	}
	Vector3 from = tm.Get_Translation();
	Vector3 to = from + (Point - from) * 2.0f;
	LineSegClass ray(from, to);
	RefCountPtr<GalaxyMapRenderSystemClass> system;
	float best = FLT_MAX;
	system = NULL;
	for (RenderSystemsType::iterator it = Systems.begin(); it != Systems.end(); ++it) {
		RefCountPtr<GalaxyMapRenderSystemClass> s = (*it).second;
		Vector3 contact;
		if (s->Intersect(ray, &contact)) {
			float d = (contact - Point).Length2();
			if (d < best) {
				best = d;
				system = s;
			}
		}
	}
	if (system != PickedSystem) {
		if (PickedSystem) {
			PickedSystem->Unknown_7AAE90();
			PickedSystem = NULL;
		}
		if (PickedSector) { PickedSector = NULL; }
		if (PickedGate) { PickedGate = NULL; }
		PickedSystem = system;
		changed = true;
	}
	if (PickedSystem) {
		RefCountPtr<GalaxyMapRenderSectorClass> sector;
		PickedSystem->Unknown_7AAE10(sector, ray, Point);
		if (sector != PickedSector) {
			changed = true;
			PickedSector = sector;
		}
	}
	if (PickedSystem) {
		PickedGate = NULL;
	} else if (PickedGate) {
		float d = PickedGate->Unknown_7AD5B0(Point);
		if (d <= 0.0f || d >= Unknown_B9E93C) {
			RefCountPtr<GalaxyMapRenderGateClass> gate;
			float nearest = Unknown_B9E93C;
			for (RenderGatesType::iterator g = Gates.begin(); g != Gates.end(); ++g) {
				if (!(*g).second->Intra) {
					float dist = (*g).second->Unknown_7AD5B0(Point);
					if (dist > 0.0f && dist < nearest) {
						nearest = dist;
						gate = (*g).second;
					}
				}
			}
			if (PickedGate != gate) {
				PickedGate = gate;
				PickedGatePoint = Point;
				changed = true;
			}
		}
	}
	return(changed);
}

// NOT EXACT: written from the client's code (see attempts.csv)
void GalaxyMapRenderClass::Unknown_7A7D70(bool changed)
{
	if (changed) {
		bool show = true;
		if (PickedSector) {
			for (RenderSectorsType::iterator it = Sectors.begin(); it != Sectors.end(); ++it) {
				if ((*it).second != PickedSector) { (*it).second->Unknown_7AC130(false, false, false, false); }
			}
			PickedSector->Unknown_7AC130(true, true, true, true);
			show = false;
		} else if (PickedSystem) {
			for (RenderSectorsType::iterator it = Sectors.begin(); it != Sectors.end(); ++it) {
				(*it).second->Unknown_7AC130(true, false, false, false);
			}
			PickedSystem->Unknown_7AADA0();
		} else if (PickedGate) {
			for (RenderSectorsType::iterator it = Sectors.begin(); it != Sectors.end(); ++it) {
				(*it).second->Unknown_7AC130(false, false, false, false);
			}
			PickedGate->Unknown_7AD850(Point);
		} else {
			for (RenderSectorsType::iterator it = Sectors.begin(); it != Sectors.end(); ++it) {
				(*it).second->Unknown_7AC130(true, true, false, false);
			}
		}
		for (RenderSystemsType::iterator s = Systems.begin(); s != Systems.end(); ++s) {
			(*s).second->Unknown_7AAED0(show);
		}
	} else if (PickedGate) {
		PickedGate->Unknown_7ADFC0(Point, PickedGatePoint);
	}
}
// NOT EXACT: written from the client's code (see attempts.csv)






















































































































































void GalaxyMapRenderClass::Unknown_7A83D0(SphereClass &sphere, bool arg)
{
	if (PickedSystem) {
		RenderObjClass *robj = PickedSystem->Peek_RObj();
		if (robj) {
			sphere = robj->Get_Bounding_Sphere();
		} else {

			G_ASSERT(false);
			sphere.Center = Point;
			sphere.Radius = UnknownB8;
		}
		sphere.Radius = (sphere.Radius < SystemZoomMinRadius) ? SystemZoomMinRadius : sphere.Radius;
		if (arg) { sphere.Radius = sphere.Radius + sphere.Radius; }
	} else if (GalaxyMapRenderGateClass *gate = PickedGate.Peek()) {
		if (arg) {
			Vector3 a;
			Vector3 b;
			gate->From->Get_Position(a);
			gate->To->Get_Position(b);
			Vector3 d = a - b;
			sphere.Center = d * 0.5f + b;
			sphere.Radius = d.Length() * 0.5f;
		} else {
			RenderObjClass *robj = PickedGate->Unknown_7AE220()->Peek_RObj();








			FAIL_IF(!robj) { return; }
			sphere = robj->Get_Bounding_Sphere();
			sphere.Radius = UnknownB8;
		}
	} else if (arg) {
		sphere = Bounds;
	} else {
		sphere.Center = Point;
		sphere.Radius = UnknownB8;
	}
	sphere.Radius = WWMath::Max(sphere.Radius, SystemZoomMinRadius);
}

























bool GalaxyMapRenderClass::Unknown_7A8770(int arg)
{
	SectorNum = arg;
	RefCountPtr<GalaxyMapRenderSectorClass> sector;
	Find_Item(sector, Sectors, arg);
	if (!sector) { return(false); }
	Vector3 pos;
	sector->Get_Position(pos);
	if (Scene) {
		FAIL_IF(!HomeSectorRObj) { return(false); }
		if (!HomeSectorRObj->Peek_Scene()) { HomeSectorRObj->Add(Scene.Peek()); }
		HomeSectorRObj->Set_Position(pos);
		Vector3 color; sector->Unknown_7ABA70(color, true);
		Unknown_496110(HomeSectorRObj.Peek(), color.X, color.Y, color.Z);
	}
	return(true);
}

bool GalaxyMapRenderClass::Unknown_7A89A0(SphereClass &sphere)
{
	RefCountPtr<GalaxyMapRenderSectorClass> sector;
	Find_Item(sector, Sectors, SectorNum);
	if (sector) {
		const RefCountPtr<GalaxyMapRenderSystemClass> &saved = PickedSystem;
		PickedSystem = sector->System;
		Unknown_7A83D0(sphere, false);
		PickedSystem = saved;
		return(true);
	}
	sphere = Bounds;
	return(false);
}

// NOT EXACT: written from the client's code (see attempts.csv)




































void GalaxyMapRenderClass::Unknown_7A8B70()
{


	GalaxyMapClientClass *map = GalaxyMap;
	FAIL_IF(!GalaxyMap) { return; }
	Unknown_7A9840();
	Updates.clear();
	Remove_Stale_Items(Sectors);
	Remove_Stale_Items(Systems);
	Remove_Stale_Items(Gates);
	GalaxyMapSystemIteratorClass systems(GalaxyMap);
	while (systems.Current) {
		RefCountPtr<GalaxyMapRenderSystemClass> render;
		int id = systems.Current->Get_Hash_ID();
		if (Find_Item(render, Systems, id)) {
			render->Sectors.clear();
		} else {
			render = Create_NEW(New_Item(systems.Current));
			Systems[id] = render;
			if (Scene) { render->Add_To_Scene(Scene); }
		}
		systems.Next();
	}
	GalaxyMapSectorIteratorClass sectors(GalaxyMap);
	while (sectors.Current) {
		RefCountPtr<GalaxyMapRenderSectorClass> render;
		int id = sectors.Current->Get_Hash_ID();
		if (Find_Item(render, Sectors, id)) {
			render->Gates.clear();
		} else {
			render = Create_NEW(New_Item(sectors.Current));
			Sectors[id] = render;
			if (Scene) { render->Add_To_Scene(Scene); }
		}
		if (render->Unknown_7AB6D0(Systems, GalaxyMap->Unknown_7A5400(sectors.Current))) {
			if (render->Needs_Update()) {
				Updates.push_back(RefCountPtr<GalaxyMapRenderItemClass>(render));
			}
		} else {
			Remove_Item(Sectors, RefCountPtr<GalaxyMapRenderItemClass>(render));
		}
		sectors.Next();
	}
	GalaxyMapGateIteratorClass gates(GalaxyMap);
	while (gates.Current) {
		RefCountPtr<GalaxyMapRenderGateClass> render;
		Create_Item(render, Gates, gates.Current, Scene);
		if (!render->Unknown_7ACBB0(Sectors)) {
			Remove_Item(Gates, RefCountPtr<GalaxyMapRenderItemClass>(render));
		}
		gates.Next();
	}
	Unknown_7A9500();
}

static GalaxyMapRenderSystemClass *New_Item(const RefCountPtr<GalaxyMapSystemClass> &data)
{
	return(new GalaxyMapRenderSystemClass(data));
}

static GalaxyMapRenderSectorClass *New_Item(const RefCountPtr<GalaxyMapSectorClass> &data)
{
	return(new GalaxyMapRenderSectorClass(data));
}
// NOT EXACT: written from the client's code (see attempts.csv)



















void GalaxyMapRenderClass::Unknown_7A9500()
{
	Unknown14.Set(FLT_MAX, FLT_MAX);
	Unknown1C.Set(-FLT_MAX, -FLT_MAX);
	UnknownB8 = 1.0f;
	for (RenderSystemsType::iterator it = Systems.begin(); it != Systems.end(); ++it) {


		GalaxyMapRenderSystemClass *sys_rend = (*it).second.Peek();
		FAIL_IF(!sys_rend) { continue; }
		const SphereClass &sphere = sys_rend->Unknown_7AAB60(GalaxyMap);
		Unknown_7A97A0(sphere);
		if (sphere.Radius > UnknownB8) { UnknownB8 = sphere.Radius; }
	}
	float width = (Unknown1C.X - Unknown14.X) * 0.5f;
	float height = (Unknown1C.Y - Unknown14.Y) * 0.5f;
	float y = height + Unknown14.Y;
	float x = width + Unknown14.X;
	float radius = (width > height) ? width : height;
	Bounds.Center.Y = y; Bounds.Center.X = x;
	Bounds.Radius = radius; Bounds.Center.Z = 0.0f;
	INIClass ini("galaxy_map_c.ini");
	for (RenderGatesType::iterator g = Gates.begin(); g != Gates.end(); ++g) {
		(*g).second->Unknown_7ACEF0(ini);
	}
}

void GalaxyMapRenderClass::Unknown_7A97A0(const SphereClass &sphere)
{
	float v = (sphere.Center.X - sphere.Radius) - SystemZoomMinRadius;
	if (v < Unknown14.X) { Unknown14.X = v; }
	v = (sphere.Center.X + sphere.Radius); v += SystemZoomMinRadius;
	if (Unknown1C.X < v) { Unknown1C.X = v; }
	v = (sphere.Center.Y - sphere.Radius) - SystemZoomMinRadius;
	if (v < Unknown14.Y) { Unknown14.Y = v; }
	v = (sphere.Center.Y + sphere.Radius); v += SystemZoomMinRadius;
	if (Unknown1C.Y < v) { Unknown1C.Y = v; }
}

bool GalaxyMapRenderClass::Unknown_7A9840()
{
	bool result = false;
	if (PickedSystem) {
		PickedSystem->Unknown_7AAE90();
		PickedSystem = NULL;
		result = true;
	}
	if (PickedSector) {
		PickedSector = NULL;
		result = true;
	}
	if (PickedGate) {
		PickedGate = NULL;
		result = true;
	}
	return(result);
}






















































bool GalaxyMapRenderItemClass::Add_To_Scene(const RefCountPtr<SceneClass> &scene)
{
	FAIL_IF(!scene) { return(false); }
	RenderObjClass *robj = Peek_RObj();
	if (robj && !robj->Peek_Scene()) {
		robj->Add(scene.Peek());
		return(true);
	}
	return(false);
}

SceneClass *GalaxyMapRenderItemClass::Remove_From_Scene()
{
	RenderObjClass *robj = Peek_RObj();
	if (robj) {
		SceneClass *scene = robj->Peek_Scene();
		if (scene) {
			robj->Remove();
			return(scene);
		}
	}
	return(NULL);
}

bool GalaxyMapRenderMeshClass::Intersect(const LineSegClass &ray, Vector3 *contact)
{
	RenderObjClass *robj = Peek_RObj();
	if (!robj) { return(false); }
	CastResultStruct sphere_result;
	if (CollisionMath::Collide(ray, robj->Get_Bounding_Sphere(), &sphere_result)) {
		CastResultStruct box_result;
		box_result.ComputeContactPoint = (contact != NULL);
		if (CollisionMath::Collide(ray, robj->Get_Bounding_Box(), &box_result)) {
			if (contact) {
				*contact = box_result.ContactPoint;
			}
			return(true);
		}
	}
	return(false);
}

bool GalaxyMapRenderMeshClass::Set_RObj(const char *name)
{
	RefCountPtr<RenderObjClass> robj;
	if (name && *name) {
		robj = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(name));
	}
	return(Set_RObj(robj));
}

bool GalaxyMapRenderMeshClass::Set_RObj(const RefCountPtr<RenderObjClass> &robj)
{
	SceneClass *scene = Remove_From_Scene();
	RObj = robj;
	Unknown_496AE0(RObj.Peek(), true);
	if (scene) { Add_To_Scene(Create_Peek(scene)); }
	return(true);
}



























void GalaxyMapRenderMeshClass::Set_Scale(const Vector3 &scale)
{
	FAIL_IF(!RObj) { return; }


	FAIL_IF(scale.X < WWMATH_EPSILON) { return; }
	FAIL_IF(scale.Y < WWMATH_EPSILON) { return; }
	FAIL_IF(scale.Z < WWMATH_EPSILON) { return; }
	if (Scale.X != scale.X || Scale.Y != scale.Y || Scale.Z != scale.Z) {
		Vector3 ratio = scale; ratio.X /= Scale.X; ratio.Y /= Scale.Y; ratio.Z /= Scale.Z;
		RObj->Scale(ratio.X, ratio.Y, ratio.Z);
		Scale = scale;
	}
}

void GalaxyMapRenderMeshClass::Set_Color(const Vector3 &color)
{
	Unknown_496110(RObj.Peek(), color.X, color.Y, color.Z);
}







void GalaxyMapRenderMeshClass::Get_Position(Vector3 &position)
{
	RenderObjClass *robj = Peek_RObj();
	FAIL_IF(!robj) { return; }
	position = robj->Get_Position();
}

GalaxyMapRenderLineClass::GalaxyMapRenderLineClass()
{
	Line = Create_NEW((SegmentedLineClass *)new GalaxyMapLineRObjClass());
}

GalaxyMapRenderSystemClass::GalaxyMapRenderSystemClass(const RefCountPtr<GalaxyMapSystemClass> &data) :
	Data(data), Unknown2C(false)
{
	RefCountPtr<GalaxyMapRObjClass> robj = Create_NEW(new GalaxyMapRObjClass(Data));
	Set_RObj(robj);
}


GalaxyMapRenderSystemClass::~GalaxyMapRenderSystemClass()
{
	Release_Item();


}

inline void GalaxyMapRenderSystemClass::Release_Item()
{
	GalaxyMapRenderMeshClass::Release_Item();
	Sectors.clear();
	Data = NULL;
}

void GalaxyMapRenderSystemClass::Unknown_7AA8C0()
{
	Sectors.clear();
}

void GalaxyMapRenderSystemClass::Unknown_7AA950(const RefCountPtr<GalaxyMapRenderSectorClass> &sector)
{
	Sectors.push_back(sector);
}





























const SphereClass &GalaxyMapRenderSystemClass::Unknown_7AAB60(GalaxyMapClientClass *map)
{
	static SphereClass sphere;
	RenderObjClass *robj = Peek_RObj();

	FAIL_IF(!robj) { return(sphere); }
	FAIL_IF(!map) { return(sphere); }
	Vector3 pos = Data->Unknown2C; pos *= Data->Unknown38;
	pos = map->Unknown_7A5470(pos);
	robj->Set_Position(pos);
	std::vector<Vector3> points;
	for (std::vector<RefCountPtr<GalaxyMapRenderSectorClass> >::iterator it = Sectors.begin(); it != Sectors.end(); ++it) {









		FAIL_IF(!*it) { continue; }
		Vector3 point;
		(*it)->Unknown_7AB8B0(map, pos, &point);
		points.push_back(point);
	}
	((GalaxyMapRObjClass *)robj)->Unknown_7B3440(points);
	return(robj->Get_Bounding_Sphere());
}







void GalaxyMapRenderSystemClass::Unknown_7AADA0()
{
	for (std::vector<RefCountPtr<GalaxyMapRenderSectorClass> >::iterator it = Sectors.begin(); it != Sectors.end(); ++it) {

		FAIL_IF(!*it) { continue; }
		(*it)->Unknown_7AC130(true, false, true, false);
	}
}



void GalaxyMapRenderSystemClass::Unknown_7AAE10(RefCountPtr<GalaxyMapRenderSectorClass> &result, const LineSegClass &ray, const Vector3 &pos)
{
	RenderObjClass *robj = Peek_RObj();
	FAIL_IF(!robj) { return; }
	Find_Nearest(result, Sectors, ray, pos);
	((GalaxyMapRObjClass *)robj)->Unknown_7B3690(true);
}


void GalaxyMapRenderSystemClass::Unknown_7AAE90()
{
	RenderObjClass *robj = Peek_RObj();
	FAIL_IF(!robj) { return; }
	((GalaxyMapRObjClass *)robj)->Unknown_7B3690(false);
}

void GalaxyMapRenderSystemClass::Unknown_7AAED0(bool on)
{
	RenderObjClass *robj = Peek_RObj();
	FAIL_IF(!robj) { return; }
	((GalaxyMapRObjClass *)robj)->Unknown_7B3700(on);
}



GalaxyMapRenderSectorClass::GalaxyMapRenderSectorClass(const RefCountPtr<GalaxyMapSectorClass> &data) :
	Data(data), Planet(false), Explored(false)
{
	const char *name;
	bool big = false;
	switch (data->Unknown24) {
		case 6:
		case 11:
			name = "sm_sector";
			break;
		case 7:
		case 8:
			name = "sm_planet";
			Planet = true;
			big = true;
			break;
		default:











			G_ASSERT(false);
			return;
	}
	Set_RObj(name);
	if (big) { Set_Scale(Vector3(1.5f, 1.5f, 1.5f)); }
	Dot = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj("sm_dot"));
	Dot->Scale(0.25f);
	Name = Create_NEW(new FontSys("GALAXY_MAP_SECTOR_NAME", 0, NULL));
	Name->Change_Text(Data->Name.c_str(), 0);
	Name->Unknown_6D8940(0.1f);
	Name->Set_Sort_Level(Unknown_B9E934);
	Name->Unknown450 = 2;
	Name->Unknown_6D88F0(0xff000001, 0xffffff, 0xffffff, 0xff000001);
}

GalaxyMapRenderSectorClass::~GalaxyMapRenderSectorClass()
{
	Release_Item();
}

inline void GalaxyMapRenderSectorClass::Release_Item()
{
	GalaxyMapRenderMeshClass::Release_Item();
	Dot = NULL;
	Name = NULL;
	System = NULL;
	Gates.clear();
	Data = NULL;
}

// NOT EXACT: order-only (the frame is the client's with the colour at function scope; the key's copy and the iterator sit in each other's slots)
bool GalaxyMapRenderSectorClass::Unknown_7AB6D0(RenderSystemsType &systems, bool explored)
{
	Explored = explored;
	RefCountPtr<GalaxyMapRenderSystemClass> system; Vector3 color;
	if (Find_Item(system, systems, Data->Unknown10)) {
		System = system;
		system->Sectors.push_back(Create_Peek(this));
		Set_Color(Unknown_7ABA70(color, false));
		return(true);
	}
	return(false);
}




















void GalaxyMapRenderSectorClass::Unknown_7AB8B0(GalaxyMapClientClass *map, const Vector3 &offset, Vector3 *position)
{
	RenderObjClass *robj = Peek_RObj();



	FAIL_IF(!robj) { return; }
	Vector3 pos = map->Unknown_7A54F0(Data->Unknown28);
	if (position) { *position = pos; }
	pos += offset;
	Matrix3D tm(pos);
	tm.Rotate_Z(DEG_TO_RADF(15.0f));
	robj->Set_Transform(tm);
	Dot->Set_Position(pos);
	pos.Y += 1.0f;
	pos.Z = 2.0f;
	Name->Set_Position(pos);
}











Vector3 &GalaxyMapRenderSectorClass::Unknown_7ABA70(Vector3 &color, bool invert)
{
	color.Set(0.35f, 0.35f, 0.35f);
	if (Explored && System) {
		GalaxyMapSystemClass *sys_data = System->Data.Peek();




		FAIL_IF(!sys_data) { return(color); }
		color = sys_data->Unknown20;
		color.X = (1.0f - color.X) * 0.2f + color.X;
		color.Y = (1.0f - color.Y) * 0.2f + color.Y;
		color.Z = (1.0f - color.Z) * 0.2f + color.Z;
	}
	if (invert) {
		color.X = 1.0f - color.X;
		color.Y = 1.0f - color.Y;
		color.Z = 1.0f - color.Z;
	}
	return(color);
}

bool GalaxyMapRenderSectorClass::Add_To_Scene(const RefCountPtr<SceneClass> &scene)
{
	if (GalaxyMapRenderMeshClass::Add_To_Scene(scene)) {
		if (Dot) { Dot->Add(scene.Peek()); }
		return(true);
	}
	return(false);
}

SceneClass *GalaxyMapRenderSectorClass::Remove_From_Scene()
{
	SceneClass *scene = GalaxyMapRenderMeshClass::Remove_From_Scene();
	if (scene) {
		if (Dot) { Dot->Remove(); }
	}
	return(scene);
}








void GalaxyMapRenderSectorClass::Update()
{
	RenderObjClass *robj = Peek_RObj();


	FAIL_IF(!robj) { return; }
	Matrix3D tm = robj->Get_Transform();
	tm.Rotate_Z(0.01f);
	robj->Set_Transform(tm);
}

void GalaxyMapRenderSectorClass::Unknown_7ABDD0(const RefCountPtr<GalaxyMapRenderGateClass> &gate)
{
	Gates.push_back(gate);
}

void GalaxyMapRenderSectorClass::Unknown_7ABFE0()
{
	Gates.clear();
}

void GalaxyMapRenderSectorClass::Unknown_7AC070(bool invert)
{
	Vector3 color;
	Set_Color(Unknown_7ABA70(color, invert));
	Name->Set_Color(color);
}
// NOT EXACT: written from the client's code (see attempts.csv)


void GalaxyMapRenderSectorClass::Unknown_7AC130(bool visible, bool show_gates, bool show_name, bool place_name)
{
	RenderObjClass *robj = Peek_RObj();



	FAIL_IF(!robj) { return; }
	robj->Set_Hidden(!visible);
	Vector3 color;
	Set_Color(Unknown_7ABA70(color, place_name));
	Name->Set_Color(color);
	std::vector<float> angles;
	for (std::vector<RefCountPtr<GalaxyMapRenderGateClass> >::iterator it = Gates.begin(); it != Gates.end(); ++it) {
		float angle = (*it)->Unknown_7AD090(visible, show_gates, place_name);
		if (place_name) { angles.push_back(angle); }
	}
	if (place_name) {
		Matrix3D tm(true);
		if (angles.empty()) {
			Vector3 pos = robj->Get_Position();
			tm.Set_Translation(tm.Get_Y_Vector() * 2.0f + pos);
			Name->Set_Transform(tm);
			Name->Set_Justify(2, 0);
		} else {
			std::sort(angles.begin(), angles.end());
			float first = angles.back() - (2.0f * WWMATH_PI);
			float last = angles.front() + (2.0f * WWMATH_PI);
			angles.insert(angles.begin(), 1, first);
			angles.push_back(last);
			float gap = 0.0f;
			float start = first;
			float prev = first;
			for (std::vector<float>::iterator f = angles.begin(); f != angles.end(); ++f) {
				float d = *f - prev;
				if (gap < d) { gap = d; start = prev; }
				prev = *f;
			}
			float angle = gap * 0.5 + start;
			if (angle >= WWMATH_PI) { angle -= (2.0f * WWMATH_PI); }
			if (angle < -WWMATH_PI) { angle += (2.0f * WWMATH_PI); }
			angle = WWMath::Clamp(angle, -WWMATH_PI, WWMATH_PI);
			float dist = 2.0f;
			Matrix3D rot(true);
			if (fabs(angle) <= DEG_TO_RADF(90.0f)) {
				rot.Rotate_Z(angle);
				Name->Set_Justify(0, 3);
			} else {
				dist = -2.0f;
				rot.Rotate_Z(angle - WWMATH_PI);
				Name->Set_Justify(0, 4);
			}
			Vector3 pos = robj->Get_Position();
			rot.Set_Translation(rot.Get_X_Vector() * dist + pos);
			Name->Set_Transform(rot);
		}
		Name->Unknown_6CE7D0(0);
		if (!Name->Peek_Scene()) { Name->Add(robj->Peek_Scene()); }
	} else if (Name->Peek_Scene()) {
		Name->Unknown_6CE7D0(2);
	}
}

const RefCountPtr<GalaxyMapRenderGateClass> &GalaxyMapRenderSectorClass::Unknown_7AC7B0(const RefCountPtr<GalaxyMapRenderSectorClass> &to)
{
	for (std::vector<RefCountPtr<GalaxyMapRenderGateClass> >::iterator it = Gates.begin(); it != Gates.end(); ++it) {
		if ((*it)->To == to) { return(*it); }
	}
	static RefCountPtr<GalaxyMapRenderGateClass> none;
	return(none);
}

GalaxyMapRenderGateClass::GalaxyMapRenderGateClass(const RefCountPtr<GalaxyMapGateClass> &data) :
	Data(data), Intra(false), Flipped(false)
{
}

void GalaxyMapRenderGateClass::Release_Item()
{
	Remove_From_Scene();
	Line = NULL;
	From = NULL;
	To = NULL;
	Data = NULL;
}

bool GalaxyMapRenderGateClass::Unknown_7ACBB0(RenderSectorsType &sectors)
{
	From = NULL;
	To = NULL;
	RefCountPtr<GalaxyMapRenderSectorClass> from;
	int from_id = Data->Unknown0C;
	if (!Find_Item(from, sectors, from_id)) { return(false); }
	RefCountPtr<GalaxyMapRenderSectorClass> to;
	int to_id = Data->Unknown10;
	if (!Find_Item(to, sectors, to_id)) { return(false); }
	From = from;
	To = to;
	From->Gates.push_back(Create_Peek(this));
	return(true);
}
// NOT EXACT: written from the client's code (see attempts.csv)





































































void GalaxyMapRenderGateClass::Unknown_7ACEF0(INIClass &ini)
{
	Vector3 color;
	FAIL_IF(!From) {
	} else FAIL_IF(!To) {
	} else {
		const char *name = "IntraGateLine";
		Intra = (From->Data->Unknown10 == To->Data->Unknown10);
		if (!Intra) { name = "InterGateLine"; }
		((GalaxyMapLineRObjClass *)Get_Line().operator->())->Unknown_7B3F00(ini, name, To->Unknown_7ABA70(color, Intra), Unknown_497300(From->Peek_RObj()), Unknown_497300(To->Peek_RObj()));
	}
}


RefCountPtr<SegmentedLineClass> &GalaxyMapRenderLineClass::Get_Line()
{
	return(Line);
}

// NOT EXACT: inline-budget (the client inlines name's ~RefCountPtr and so keeps the pointer in esi; ours calls it); the matrix (identity, Set_Translation, Rotate_Z(s, c)), the branch order and '!line' are the client's




float GalaxyMapRenderGateClass::Unknown_7AD090(bool hide_inter, bool hide_intra, bool place)
{
	float angle = 0.0f;
	Peek_RObj(); GalaxyMapLineRObjClass *line = (GalaxyMapLineRObjClass *)Line.Peek();
	FAIL_IF(!line) { return(angle); }
	if (Intra) { line->Hidden = (hide_intra == false); } else { line->Hidden = (hide_inter == false); }

	RefCountPtr<FontSys> name = To->Name;
	if (place) {
		Vector3 a;
		Vector3 b;
		From->Get_Position(a);
		To->Get_Position(b);
		Vector3 d = b - a;
		d.Normalize();
		Matrix3D tm(true); tm.Set_Translation(a);
		tm.Rotate_Z(d.Y, d.X);
		angle = tm.Get_Z_Rotation();
		if (fabs(angle) > 1.5707964f) {
			tm.Rotate_Z(WWMATH_PI);
			tm.Translate_X(-2.0f);
			name->Set_Justify(0, 4);
		} else {
			tm.Translate_X(2.0f);
			name->Set_Justify(0, 3);
		}
		name->Set_Transform(tm);
		Vector3 color; name->Set_Color(To->Unknown_7ABA70(color, false));
		if (!name->Peek_Scene()) { name->Add(Line->Peek_Scene()); }
		name->Unknown_6CE7D0(0);
	} else {
		name->Unknown_6CE7D0(2);
	}
	return(angle);
}

// NOT EXACT: written from the client's code (see attempts.csv)
float GalaxyMapRenderGateClass::Unknown_7AD5B0(const Vector3 &point)
{
	Vector3 a;
	Vector3 b;
	From->Get_Position(a);
	To->Get_Position(b);
	if (WWMath::Max(a.X, b.X) < point.X - Unknown_B9E93C) { return(-1.0f); }
	if (WWMath::Min(a.X, b.X) > point.X + Unknown_B9E93C) { return(-1.0f); }
	if (WWMath::Max(a.Y, b.Y) < point.Y - Unknown_B9E93C) { return(-1.0f); }
	if (WWMath::Min(a.Y, b.Y) > point.Y + Unknown_B9E93C) { return(-1.0f); }
	float dx = a.X - b.X;
	float dy = a.Y - b.Y;
	float len = sqrtf(dx * dx + dy * dy);
	if (len >= WWMATH_EPSILON) {
		return(fabs(((point.X - b.X) * dy - (point.Y - b.Y) * dx) / len));
	}
	dx = point.X - a.X;
	dy = point.Y - a.Y;
	return(sqrtf(dx * dx + dy * dy));
}
// NOT EXACT: written from the client's code (see attempts.csv)





























































































void GalaxyMapRenderGateClass::Unknown_7AD850(const Vector3 &point)
{



	FAIL_IF(!Line) { return; }
	To->Unknown_7AC070(Flipped);
	From->Unknown_7AC070(!Flipped);
}

























































































































void GalaxyMapRenderGateClass::Unknown_7ADFC0(const Vector3 &a, const Vector3 &b)
{
	Vector3 pos;
	To->Get_Position(pos);
	float da = (a - pos).Length(); float db = (b - pos).Length();
	bool flipped = da > db;
	if (flipped != Flipped) {
		Flipped = flipped;
		To->Unknown_7AC070(!Flipped);
		From->Unknown_7AC070(Flipped);
	}
}

const RefCountPtr<GalaxyMapRenderSectorClass> &GalaxyMapRenderGateClass::Unknown_7AE220()
{
	return(Flipped ? From : To);
}

/*
**	Reconstruction notes
**
**	The object: __FILE__ 0xB9E970 (app\Client\galaxy_map_render.cpp), client 0x7A6020-0x7B2B30. It
**	starts with the constructor right after galaxy_map_focal_point.cpp's object and ends, after its
**	<string> initialiser 0x7B2500, with the late template members it needs (hashtable::resize
**	0x7B2530 / 0x7B2700 / 0x7B28D0 and std::_Construct 0x7B2AA0 / 0x7B2AD0 / 0x7B2B00, called only
**	from here); galaxy_map_robj.cpp starts at 0x7B2B30 (FAIL_IF '_name_scale < WWMATH_EPSILON',
**	line 65). The templates at the top are the FAIL_IFs on lines 124 ('!result'), 152
**	('it == container.end()') and 194 ('!scene') that the members inline.
**
**	Forms: iterators constructed in place, not assigned (an assigned one is kept in registers);
**	the update list's element is a one-member struct, whose implicit destructor _Destroy calls
**	through ??_G (0x7B18A0); the destructors call Release_Item, which reaches Remove_From_Scene through
**	the table just stored; New_Item 0x7A9370 / 0x7A93F0 are static (called without a thunk);
**	0x7ABA70, 0x7A83D0 and 0x7A89A0 write their results through references (ret without eax).
**	What stays NOT EXACT is listed in docs/data/attempts.csv (inline budgets in the long members,
**	0x7A8B70 above all, and three order-only residues).
*/
