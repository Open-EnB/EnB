// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\interface_static_sort_list.h (name assumed from
**	interface_static_sort_list.cpp, whose __FILE__ is 'e:\build\beta\G\app\Client\interface_static_sort_list.cpp';
**	the client holds no string of this header)
**
**	The interface scene's static sort list (InterfaceSceneClass +0x9C, the asserts' 'ssl'; 0x74 bytes, vtable
**	0xAFF314): one DefaultStaticSortListClass per interface (InterfaceRenderClass, 0x34C bytes, vtables 0xAFF304 /
**	0xAFF2F8), kept in InterfaceVector in drawing order between the back and the front list; the render objects
**	are sorted into the list of the interface that was current when they were added (RenderObjMap), and each
**	interface's list draws its picture-in-picture scenes (PIPScenes) between its sort levels.
**	The member names are the asserts' (InterfaceVector, InterfaceMap, RenderObjMap, PIPScenes, PIPPolyBudget,
**	FrontInterface, CurrentInterface, LastInterface); the classes' and the rest are neutral (D10, D24).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __INTERFACE_STATIC_SORT_LIST_H  // NAME 3 [interface_static_sort_list.h]: header name assumed from the client's interface_static_sort_list.cpp
#define __INTERFACE_STATIC_SORT_LIST_H

#include <vector>
#include <hash_map.h>
#include "gadget_list_manager.h"
#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\ww3d2\static_sort_list.h"

class InterfaceClass;
class RenderObjClass;
class SimpleSceneClass;
class SceneClass;
class CameraClass;
struct PIPSceneStruct;

__STL_BEGIN_NAMESPACE
__STL_TEMPLATE_NULL struct hash<RenderObjClass *> {  // the key's address (as co_data.h's)
	size_t operator()(RenderObjClass *robj) const { return((size_t)robj); }
};
__STL_TEMPLATE_NULL struct hash<SimpleSceneClass *> {  // the key's address (as co_data.h's)
	size_t operator()(SimpleSceneClass *scene) const { return((size_t)scene); }
};
__STL_END_NAMESPACE

// One interface's sort list (the asserts' 'ui_render'; 0x34C bytes)
class InterfaceRenderClass : public DefaultStaticSortListClass, public RefCountClass  // NAME 2: the asserts' ui_render
{
public:
	InterfaceRenderClass(InterfaceClass *ui, int index) :	// inline (0x6DA930, 0x6DB380)  // NAME 2 [ui]: the asserts'  // NAME 1 [index]: neutral
		Interface(ui),
		Index(index),
		MinSortLevel(MAX_SORT_LEVEL),
		MaxSortLevel(0)
	{
	}
	virtual ~InterfaceRenderClass() {}						// 0x6DAD40 (scalar deleting 0x6DAD10)

	virtual void Add_To_List(RenderObjClass *robj, unsigned int sort_level);	// 0x6DC8B0 (the levels used)

	void Render(const RefCountPtr<SceneClass> &scene, const RefCountPtr<CameraClass> &camera);	// 0x6DC8F0 (the levels used, around the PIP scenes)  // NAME 1: neutral
	void Add_PIP(PIPSceneStruct *ps);						// 0x6DCAD0 (FAIL_IF '!ps', by the sort level)  // NAME 1: neutral
	void Remove_PIP(PIPSceneStruct *ps);					// 0x6DCD50 (FAIL_IF '!ps', 'find == PIPScenes.end()')  // NAME 1: neutral

	InterfaceClass *Interface;								// +0x32C (the key of InterfaceMap)  // NAME 1: neutral
	std::vector<PIPSceneStruct *> PIPScenes;				// +0x330 (by the sort level, highest first)  // NAME 2: the asserts' 'find == PIPScenes.end()'
	int Index;												// +0x340 (the place in InterfaceVector)  // NAME 1: neutral
	unsigned int MinSortLevel;								// +0x344 (the lowest level added since the last render)  // NAME 1: neutral
	unsigned int MaxSortLevel;								// +0x348 (the highest)  // NAME 1: neutral
};

// A picture-in-picture scene (the asserts' 'pip_scene', 'ps'; 0x14 bytes)
struct PIPSceneStruct  // NAME 2: the asserts' pip_scene
{
	PIPSceneStruct(RefCountPtr<InterfaceRenderClass> owner, const RefCountPtr<SimpleSceneClass> &scene,	// 0x6DC2F0  // NAME 1 [owner, scene]: neutral
		const RefCountPtr<CameraClass> &camera, float poly_budget, unsigned int sort_level) :	// NAME 1 [camera, poly_budget, sort_level]: neutral
		Owner(owner),
		Scene(scene),
		Camera(camera),
		PolyBudget(poly_budget),
		SortLevel(sort_level)
	{
	}

	void Render(const RefCountPtr<SceneClass> &scene, const RefCountPtr<CameraClass> &camera);	// 0x6DC7C0 (InterfaceRenderClass::Render's arguments, unused)  // NAME 1: neutral

	RefCountPtr<InterfaceRenderClass> Owner;				// +0x00  // NAME 1: neutral
	RefCountPtr<SimpleSceneClass> Scene;					// +0x04  // NAME 1: neutral
	RefCountPtr<CameraClass> Camera;						// +0x08  // NAME 1: neutral
	float PolyBudget;										// +0x0C (summed in PIPPolyBudget)  // NAME 1: neutral
	unsigned int SortLevel;									// +0x10  // NAME 1: neutral
};

class InterfaceStaticSortListClass : public StaticSortListClass  // NAME 3: class-from-file (D24) for app\client\interface_static_sort_list.cpp
{
public:
	typedef std::hash_map<InterfaceClass *, RefCountPtr<InterfaceRenderClass> > InterfaceMapType;	// NAME 1: neutral
	typedef std::hash_map<RenderObjClass *, RefCountPtr<InterfaceRenderClass> > RenderObjMapType;	// NAME 1: neutral
	typedef std::hash_map<SimpleSceneClass *, PIPSceneStruct *> PIPSceneMapType;					// NAME 1: neutral

	InterfaceStaticSortListClass() :						// 0x6DA930 (inline: interface_scene.cpp's)
		LastRenderObj(NULL),
		PIPPolyBudget(0)
	{
		FrontInterface = Create_NEW(new InterfaceRenderClass(NULL, 0));
		BackInterface = Create_NEW(new InterfaceRenderClass(NULL, 0));
		InterfaceVector.push_back(BackInterface);
		InterfaceVector.push_back(FrontInterface);
	}
	virtual ~InterfaceStaticSortListClass();				// 0x6DB010 (scalar deleting 0x6DADB0)

	virtual void Add_To_List(RenderObjClass *robj, unsigned int sort_level);	// 0x6DB2C0 (FAIL_IF '!LastInterface')
	virtual void Render_And_Clear(RenderInfoClass &rinfo) {}					// 0x6DACF0 (the lists render in Render)

	void Render(const RefCountPtr<SceneClass> &scene, const RefCountPtr<CameraClass> &camera);	// 0x6DB330 (interface_scene.cpp's 0x6DA760)  // NAME 1: neutral
	void Add_Interface(InterfaceClass *ui);					// 0x6DB380 (intrface.cpp's)  // NAME 1: neutral
	void Remove_Interface(InterfaceClass *ui);				// 0x6DB6D0  // NAME 1: neutral
	void Interface_To_Front(InterfaceClass *ui);			// 0x6DB950  // NAME 1: neutral
	InterfaceClass *Set_Current_Interface(InterfaceClass *ui);	// 0x6DBC00 (local_interface_sort.cpp's; the previous one)  // NAME 1: neutral
	InterfaceClass *Set_Current_Front();					// 0x6DBD70  // NAME 1: neutral
	InterfaceClass *Set_Current_Back();						// 0x6DBDE0  // NAME 1: neutral
	void Add_Render_Object(RenderObjClass *robj);			// 0x6DBE50 (interface_scene.cpp's 0x6DA650)  // NAME 1: neutral
	void Remove_Render_Object(RenderObjClass *robj);		// 0x6DBF40 (interface_scene.cpp's 0x6DA680)  // NAME 1: neutral
	bool Add_PIP_Scene(const RefCountPtr<SimpleSceneClass> &scene, const RefCountPtr<CameraClass> &camera, float poly_budget, unsigned int sort_level);	// 0x6DC0A0  // NAME 1: neutral
	bool Remove_PIP_Scene(const RefCountPtr<SimpleSceneClass> &scene);	// 0x6DC380  // NAME 1: neutral
	InterfaceClass *Find_Interface(RenderObjClass *robj);	// 0x6DC5F0 (sets LastInterface)  // NAME 1: neutral

	RefCountPtr<InterfaceRenderClass> CurrentInterface;		// +0x04  // NAME 2: the asserts'
	RefCountPtr<InterfaceRenderClass> FrontInterface;		// +0x08  // NAME 2: the asserts'
	RefCountPtr<InterfaceRenderClass> BackInterface;		// +0x0C  // NAME 1: neutral (FrontInterface's opposite)
	std::vector<RefCountPtr<InterfaceRenderClass> > InterfaceVector;	// +0x10  // NAME 2: the asserts'
	InterfaceMapType InterfaceMap;							// +0x20  // NAME 2: the asserts'
	RenderObjMapType RenderObjMap;							// +0x38  // NAME 2: the asserts'
	RenderObjClass *LastRenderObj;							// +0x50 (Find_Interface's last top container)  // NAME 1: neutral
	RefCountPtr<InterfaceRenderClass> LastInterface;		// +0x54  // NAME 2: the asserts'
	PIPSceneMapType PIPScenes;								// +0x58  // NAME 2: the asserts'
	float PIPPolyBudget;									// +0x70  // NAME 2: the asserts'
};

#endif
