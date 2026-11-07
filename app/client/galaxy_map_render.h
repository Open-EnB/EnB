// Earth & Beyond client (reconstruction): app\Client\galaxy_map_render.h (notes at the end)
/*
**	The galaxy map's renderer and the items it draws: one per system, sector and gate of the
**	map (galaxy_map.h's data objects), each holding its render object. galaxy_map_render.cpp's
**	classes; GalaxyMapRObjClass's and GalaxyMapLineRObjClass's are galaxy_map_robj.cpp's, declared
**	here as far as this file calls them.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __GALAXY_MAP_RENDER_H
#define __GALAXY_MAP_RENDER_H

#include <vector>
#include <hash_map.h>
#include "vector2.h"
#include "vector3.h"
#include "sphere.h"
#include "..\common\galaxy_map.h"
#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\segline.h"
#include "..\shared\g_assert.h"

class SceneClass;
class CameraClass;
class LineSegClass;
class FontSys;
class INIClass;
class GalaxyMapClientClass;
class GalaxyMapSystemIteratorClass;

// galaxy_map_robj.cpp's system marker, as far as this file calls it. 0x120 bytes, table 0xB05A44
// PLACEMENT: provisional (D137): galaxy_map_robj.cpp is not in the tree yet
class GalaxyMapRObjClass : public RenderObjClass  // NAME 1: what it is
{
public:
	GalaxyMapRObjClass(const RefCountPtr<GalaxyMapSystemClass> &system);	// 0x7B2B30  // NAME 2 [system]: AA420 passes its data
	virtual RenderObjClass *Clone(void) const;
	virtual void Render(RenderInfoClass &rinfo);

	void Unknown_7B3440(std::vector<Vector3> &points);	// 0x7B3440  // NAME 1 [points]: what it walks
	void Unknown_7B3690(bool on);						// 0x7B3690  // NAME 1 [on]: neutral
	void Unknown_7B3700(bool on);						// 0x7B3700  // NAME 1 [on]: neutral

	char Unknown_Pad[0x120 - sizeof(RenderObjClass)];
};

// galaxy_map_robj.cpp's line between two sectors, as far as this file calls it. 0xFC bytes, table 0xB05CAC
// PLACEMENT: provisional (D137): galaxy_map_robj.cpp is not in the tree yet
class GalaxyMapLineRObjClass : public SegmentedLineClass  // NAME 1: what it is
{
public:
	GalaxyMapLineRObjClass();								// 0x7B3E30
	void Unknown_7B3F00(INIClass &ini, const char *name, const RefCountPtr<RenderObjClass> &from, const RefCountPtr<RenderObjClass> &to, const Vector3 &color);	// 0x7B3F00 (FAIL_IF '!from')  // NAME 1 [ini, name, to, color]: what 0x7ACEF0 passes

	char Unknown_PadE8[0xF4 - sizeof(SegmentedLineClass)];
	bool Hidden;											// +0xF4  // NAME 1: 0x7AD090, 0x7AD850
	char Unknown_PadF5[0xFC - 0xF5];
};

// The base of the map's items (table 0xB0593C; 8 bytes; its destructor 0x7A9CC0 is implicit)
class GalaxyMapRenderItemClass : public RefCountClass  // NAME 1: what it is
{
public:
	virtual void Release_Item() { Remove_From_Scene(); }	// +0x08 0x7A9920  // NAME 1: what it does
	virtual RenderObjClass *Peek_RObj() = 0;				// +0x0C  // NAME 2: 0x7A9940 adds it to the scene
	virtual RefCountPtr<GalaxyMapDataClass> Get_Data() = 0;	// +0x10  // NAME 2: 0x7B1090 reads its Get_Hash_ID
	virtual bool Needs_Update() { return(false); }			// +0x14 0x7A9B60  // NAME 1: 0x7A8B70 lists those that do

























































































































	virtual void Update() { G_ASSERT(false); }				// +0x18 0x7A9B80  // NAME 1: 0x7A7070 calls it per frame
	virtual bool Add_To_Scene(const RefCountPtr<SceneClass> &scene);	// +0x1C 0x7A9940 (FAIL_IF '!scene')  // NAME 2: what it does
	virtual SceneClass *Remove_From_Scene();				// +0x20 0x7A99B0  // NAME 2: what it does
};

// An item with a mesh and a scale (table 0xB0588C; 0x18 bytes)
class GalaxyMapRenderMeshClass : public GalaxyMapRenderItemClass  // NAME 1: what it is
{
public:
	GalaxyMapRenderMeshClass() : Scale(1.0f, 1.0f, 1.0f) {}	// 0x7A9B20
	virtual ~GalaxyMapRenderMeshClass() { GalaxyMapRenderMeshClass::Release_Item(); }	// 0x7A9C00 (deleting 0x7A9BD0)
	virtual void Release_Item() { Remove_From_Scene(); RObj = NULL; }	// +0x08 0x7A9CE0
	virtual RenderObjClass *Peek_RObj() { return(RObj.Peek()); }	// +0x0C 0x7A9BB0

	bool Set_RObj(const char *name);						// 0x7A9D20
	bool Set_RObj(const RefCountPtr<RenderObjClass> &robj);	// 0x7A9EA0
	bool Intersect(const LineSegClass &ray, Vector3 *contact);	// 0x7A99F0  // NAME 2: CollisionMath::Collide
	void Set_Scale(const Vector3 &scale);					// 0x7A9F80  // NAME 3: the asserts'
	void Set_Color(const Vector3 &color);					// 0x7AA110  // NAME 1: ro_funcs.cpp's 0x496110
	void Get_Position(Vector3 &position);					// 0x7AA140  // NAME 2: RenderObjClass::Get_Position

	RefCountPtr<RenderObjClass> RObj;						// +0x08  // NAME 3: the FAIL_IF's '!RObj'
	Vector3 Scale;											// +0x0C  // NAME 3: the asserts'
};

class GalaxyMapRenderSectorClass;

// A system (table 0xB058E4; 0x30 bytes)
class GalaxyMapRenderSystemClass : public GalaxyMapRenderMeshClass  // NAME 1: what it holds
{
public:
	GalaxyMapRenderSystemClass(const RefCountPtr<GalaxyMapSystemClass> &data);	// 0x7AA420
	virtual ~GalaxyMapRenderSystemClass();					// 0x7AA660 (deleting 0x7AA630)
	virtual void Release_Item();							// +0x08 0x7AA7E0
	virtual RefCountPtr<GalaxyMapDataClass> Get_Data() { return(Data); }	// +0x10 0x7AA600

	void Unknown_7AA8C0();									// 0x7AA8C0
	void Unknown_7AA950(const RefCountPtr<GalaxyMapRenderSectorClass> &sector);	// 0x7AA950  // NAME 1 [sector]: what it stores
	const SphereClass &Unknown_7AAB60(GalaxyMapClientClass *map);	// 0x7AAB60 (FAIL_IF '!map')  // NAME 3 [map]: the FAIL_IF's
	void Unknown_7AADA0();									// 0x7AADA0
	void Unknown_7AAE10(RefCountPtr<GalaxyMapRenderSectorClass> &result, const LineSegClass &ray, const Vector3 &pos);	// 0x7AAE10  // NAME 1 [result, ray, pos]: neutral
	void Unknown_7AAE90();									// 0x7AAE90
	void Unknown_7AAED0(bool on);							// 0x7AAED0  // NAME 1 [on]: neutral

	RefCountPtr<GalaxyMapSystemClass> Data;					// +0x18  // NAME 2: what it holds
	std::vector<RefCountPtr<GalaxyMapRenderSectorClass> > Sectors;	// +0x1C  // NAME 2: what it holds
	bool Unknown2C;											// +0x2C
};

class GalaxyMapRenderGateClass;

// A sector (table 0xB05910; 0x3C bytes)
class GalaxyMapRenderSectorClass : public GalaxyMapRenderMeshClass  // NAME 1: what it holds
{
public:
	GalaxyMapRenderSectorClass(const RefCountPtr<GalaxyMapSectorClass> &data);	// 0x7AAF20
	virtual ~GalaxyMapRenderSectorClass();					// 0x7AB3A0 (deleting 0x7AB370)
	virtual void Release_Item();							// +0x08 0x7AB5B0
	virtual RefCountPtr<GalaxyMapDataClass> Get_Data() { return(Data); }	// +0x10 0x7AB340
	virtual bool Needs_Update() { return(Planet); }			// +0x14 0x7AB320
	virtual void Update();									// +0x18 0x7ABC60 (FAIL_IF '!robj')
	virtual bool Add_To_Scene(const RefCountPtr<SceneClass> &scene);	// +0x1C 0x7ABB80 (FAIL_IF '!scene')
	virtual SceneClass *Remove_From_Scene();				// +0x20 0x7ABC10

	bool Unknown_7AB6D0(std::hash_map<int, RefCountPtr<GalaxyMapRenderSystemClass> > &systems, bool explored);	// 0x7AB6D0  // NAME 1 [systems, explored]: what it finds; 0x7A5400's
	void Unknown_7AB8B0(GalaxyMapClientClass *map, const Vector3 &offset, Vector3 *position);	// 0x7AB8B0  // NAME 1 [map, offset, position]: neutral
	Vector3 Unknown_7ABA70(bool invert);					// 0x7ABA70 (FAIL_IF '!sys_data': the colour)  // NAME 1 [invert]: 1 - colour
	void Unknown_7ABDD0(const RefCountPtr<GalaxyMapRenderGateClass> &gate);	// 0x7ABDD0  // NAME 1 [gate]: what it stores
	void Unknown_7ABFE0();									// 0x7ABFE0
	void Unknown_7AC070(bool invert);						// 0x7AC070  // NAME 1 [invert]: as 0x7ABA70's
	void Unknown_7AC130(bool visible, bool show_gates, bool show_name, bool place_name);	// 0x7AC130 (FAIL_IF '!robj')  // NAME 1 [visible, show_gates, show_name, place_name]: what it does with them
	const RefCountPtr<GalaxyMapRenderGateClass> &Unknown_7AC7B0(const RefCountPtr<GalaxyMapRenderSectorClass> &to);	// 0x7AC7B0  // NAME 1 [to]: what it compares

	RefCountPtr<GalaxyMapSectorClass> Data;					// +0x18  // NAME 2: what it holds
	std::vector<RefCountPtr<GalaxyMapRenderGateClass> > Gates;	// +0x1C  // NAME 2: what it holds
	RefCountPtr<GalaxyMapRenderSystemClass> System;			// +0x2C  // NAME 1: 0x7AB6D0 looks it up by the sector's Unknown10
	RefCountPtr<FontSys> Name;								// +0x30 (GALAXY_MAP_SECTOR_NAME)  // NAME 2: the string template
	RefCountPtr<RenderObjClass> Dot;						// +0x34 (sm_dot)  // NAME 2: the asset's
	bool Planet;											// +0x38 (the data's types 7, 8)  // NAME 2: sm_planet
	bool Explored;											// +0x39  // NAME 1: 0x7A5400's
};

// A line's base (table 0xB058B8; 0xC bytes)
class GalaxyMapRenderLineClass : public GalaxyMapRenderItemClass  // NAME 1: what it is
{
public:
	GalaxyMapRenderLineClass();								// 0x7AA1B0
	virtual ~GalaxyMapRenderLineClass() { GalaxyMapRenderLineClass::Release_Item(); }	// 0x7AA300 (deleting 0x7AA2D0)
	virtual void Release_Item() { Remove_From_Scene(); Line = NULL; }	// +0x08 0x7AA3C0
	virtual RenderObjClass *Peek_RObj() { return(Line.Peek()); }	// +0x0C 0x7AA400

	RefCountPtr<SegmentedLineClass> Line;					// +0x08  // NAME 3: the FAIL_IF's '!line'
	RefCountPtr<SegmentedLineClass> &Get_Line() { return(Line); }	// 0x7AD070
};

// A gate (table 0xB05974; 0x1C bytes)
class GalaxyMapRenderGateClass : public GalaxyMapRenderLineClass  // NAME 1: what it holds
{
public:
	GalaxyMapRenderGateClass(const RefCountPtr<GalaxyMapGateClass> &data);	// 0x7AC870
	virtual ~GalaxyMapRenderGateClass() {}					// 0x7ACA00 (deleting 0x7AC9D0)
	virtual void Release_Item();							// +0x08 0x7ACB20
	virtual RefCountPtr<GalaxyMapDataClass> Get_Data() { return(Data); }	// +0x10 0x7AC9A0

	bool Unknown_7ACBB0(std::hash_map<int, RefCountPtr<GalaxyMapRenderSectorClass> > &sectors);	// 0x7ACBB0  // NAME 1 [sectors]: what it finds
	void Unknown_7ACEF0(INIClass &ini);						// 0x7ACEF0 (FAIL_IF '!From', '!To')  // NAME 1 [ini]: the caller's
	float Unknown_7AD090(bool hide_inter, bool hide_intra, bool place);	// 0x7AD090 (FAIL_IF '!line')  // NAME 1 [hide_inter, hide_intra, place]: what it does with them
	float Unknown_7AD5B0(const Vector3 &point);				// 0x7AD5B0: the distance to the line, -1 off it  // NAME 1 [point]: neutral
	void Unknown_7AD850(const Vector3 &point);				// 0x7AD850 (FAIL_IF '!line', '!to_name', '!from_name')  // NAME 3 [to_name, from_name]: the FAIL_IFs'
	void Unknown_7ADFC0(const Vector3 &a, const Vector3 &b);	// 0x7ADFC0  // NAME 1 [a, b]: neutral
	const RefCountPtr<GalaxyMapRenderSectorClass> &Unknown_7AE220() { return(Flipped ? From : To); }	// 0x7AE220

	RefCountPtr<GalaxyMapGateClass> Data;					// +0x0C  // NAME 2: what it holds
	RefCountPtr<GalaxyMapRenderSectorClass> From;			// +0x10  // NAME 3: the FAIL_IF's
	RefCountPtr<GalaxyMapRenderSectorClass> To;				// +0x14  // NAME 3: the FAIL_IF's
	bool Intra;												// +0x18 (both sectors in one system)  // NAME 2: 'IntraGateLine'
	bool Flipped;											// +0x19  // NAME 1: which end is the near one
};

// One entry of the renderer's update list: _Destroy calls its implicit destructor through ??_G (0x7B18A0)
struct GalaxyMapRenderUpdateStruct  // NAME 1: what it is
{
	GalaxyMapRenderUpdateStruct(const RefCountPtr<GalaxyMapRenderItemClass> &item) : Item(item) {}
	RefCountPtr<GalaxyMapRenderItemClass> Item;			// NAME 1: what it holds
};

// The renderer (GalaxyMapPipClass's, 0xC0 bytes)
class GalaxyMapRenderClass  // NAME 3: class-from-file (D24) for app\Client\galaxy_map_render.cpp
{
public:
	GalaxyMapRenderClass(GalaxyMapClientClass *map);	// 0x7A6020 (G_ASSERT 'GalaxyMap')  // NAME 2 [map]: the assert's
	~GalaxyMapRenderClass();							// 0x7A64F0

	void Unknown_7A6750();								// 0x7A6750
	void Load_Ini();									// 0x7A69D0  // NAME 2: reads galaxy_map_c.ini's [Render]
	bool Unknown_7A6A70();								// 0x7A6A70
	void Unknown_7A6AB0(const RefCountPtr<SceneClass> &scene, const RefCountPtr<CameraClass> &camera);	// 0x7A6AB0  // NAME 1 [scene, camera]: what the caller passes
	void Unknown_7A6DD0();								// 0x7A6DD0
	bool Unknown_7A7070(bool arg);						// 0x7A7070 (FAIL_IF '!GalaxyMap')  // NAME 1 [arg]: neutral
	void Unknown_7A72C0(float x, float y);				// 0x7A72C0 (FAIL_IF '!Camera')  // NAME 1 [x, y]: neutral
	void Unknown_7A7450();								// 0x7A7450
	bool Unknown_7A7560();								// 0x7A7560
	void Unknown_7A7D70(bool changed);					// 0x7A7D70  // NAME 1 [changed]: what the callers pass
	SphereClass Unknown_7A83D0(bool arg);					// 0x7A83D0  // NAME 1 [arg]: neutral
	bool Unknown_7A8770(int arg);						// 0x7A8770 (the aux data's SectorNum)  // NAME 1 [arg]: what the caller passes
	SphereClass Unknown_7A89A0();							// 0x7A89A0
	void Unknown_7A8B70();								// 0x7A8B70 (FAIL_IF '!GalaxyMap')
	void Unknown_7A9500();								// 0x7A9500 (FAIL_IF '!sys_rend')
	void Unknown_7A97A0(const SphereClass &sphere);		// 0x7A97A0  // NAME 1 [sphere]: what it reads
	bool Unknown_7A9840();								// 0x7A9840

	GalaxyMapClientClass *GalaxyMap;					// +0x00  // NAME 3: the FAIL_IF's
	int SectorNum;										// +0x04  // NAME 2: the aux data's, 0x7A8770
	RefCountPtr<RenderObjClass> HomeSectorRObj;			// +0x08  // NAME 3: the FAIL_IF's
	RefCountPtr<SceneClass> Scene;						// +0x0C  // NAME 2: the FAIL_IF's '!scene'
	RefCountPtr<CameraClass> Camera;					// +0x10  // NAME 3: the FAIL_IF's
	Vector2 Unknown14;									// +0x14 (0x53FD10)
	Vector2 Unknown1C;									// +0x1C (0x53FD10)
	int Version;										// +0x24  // NAME 1: GalaxyMapClientClass's Version, last seen
	unsigned int Unknown28;								// +0x28: the time of the next update
	std::hash_map<int, RefCountPtr<GalaxyMapRenderSystemClass> > Systems;	// +0x2C  // NAME 2: what it holds
	std::hash_map<int, RefCountPtr<GalaxyMapRenderSectorClass> > Sectors;	// +0x44  // NAME 2: what it holds
	std::hash_map<int, RefCountPtr<GalaxyMapRenderGateClass> > Gates;		// +0x5C  // NAME 2: what it holds
	std::vector<GalaxyMapRenderUpdateStruct> Updates;			// +0x74  // NAME 2: 0x7A7070 calls Update on them
	Vector3 Point;										// +0x84: the pointer on the map's plane
	RefCountPtr<GalaxyMapRenderSystemClass> PickedSystem;	// +0x90  // NAME 2: 0x7A7560
	RefCountPtr<GalaxyMapRenderSectorClass> PickedSector;	// +0x94  // NAME 2: 0x7A7560
	RefCountPtr<GalaxyMapRenderGateClass> PickedGate;		// +0x98  // NAME 2: 0x7A7560
	Vector3 PickedGatePoint;							// +0x9C  // NAME 2: 0x7A7560
	SphereClass Bounds;									// +0xA8 (0x7A9500; 0x53FCA0 writes the radius)  // NAME 2: what 0x7A9500 stores
	float UnknownB8;									// +0xB8 (1.0f; 0x7A9500: the largest system radius)
	float SystemZoomMinRadius;							// +0xBC (10.0f)  // NAME 3: the ini's
};

#endif // __GALAXY_MAP_RENDER_H
