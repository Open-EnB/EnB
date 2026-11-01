// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\interface_scene.h (name assumed from interface_scene.cpp,
**	whose __FILE__ is 'e:\build\beta\G\app\Client\interface_scene.cpp'; the client holds no string of this header)
**
**	The views' 2-D scene (UIViewClass +0x1C; 0xA0 bytes, vtable 0xAFF1F4): a SimpleSceneClass that draws its
**	render objects through its static sort list (+0x9C, interface_static_sort_list.h), one list per interface
**	in the interfaces' order, with the picture-in-picture scenes between their sort levels.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __INTERFACE_SCENE_H  // NAME 3 [interface_scene.h]: header name assumed from the client's interface_scene.cpp
#define __INTERFACE_SCENE_H

#include "..\..\ww3d2\scene.h"
#include "..\..\wwlib\ref_ptr.h"

class CameraClass;
class RenderObjClass;
class RenderInfoClass;
class InterfaceStaticSortListClass;

class InterfaceSceneClass : public SimpleSceneClass  // NAME 3: class-from-file (D24) for app\client\interface_scene.cpp
{
public:
	InterfaceSceneClass();									// 0x6DA4D0
	virtual ~InterfaceSceneClass();							// 0x6DA5C0 (scalar deleting 0x6DA590)

	virtual int Get_Scene_ID(void) { return(SCENE_ID_LAST + 1); }	// 0x6DA570 (0x10000)

	virtual void Add_Render_Object(RenderObjClass *obj);	// 0x6DA650 (and to the sort list)
	virtual void Remove_Render_Object(RenderObjClass *obj);	// 0x6DA680
	virtual void Visibility_Check(CameraClass *camera);		// 0x6DA6B0 (the PIP scenes' budget to the LOD optimizer's cost)

	bool Add_PIP_Scene(const RefCountPtr<SimpleSceneClass> &scene, const RefCountPtr<CameraClass> &camera, float poly_budget, unsigned int sort_level);	// 0x6DA6F0 (UIViewClass's 0x65B7A0)  // NAME 1: the sort list's
	bool Remove_PIP_Scene(const RefCountPtr<SimpleSceneClass> &scene);	// 0x6DA730 (UIViewClass's 0x65B7E0)  // NAME 1: the sort list's
	void Render_With_PIP_Scenes(const RefCountPtr<CameraClass> &camera);	// 0x6DA760 (UIViewClass::Render and the main view's 0x50D520 pass their +0x20 camera)  // NAME 2: after the assert's InsideRenderWithPIPScenes

protected:
	virtual void Customized_Render(RenderInfoClass &rinfo);	// 0x6DA880 (FAIL_IF '!InsideRenderWithPIPScenes'; only outside the sort list's render)

public:
	bool InsideSortListRender;								// +0x98 (set while the sort list draws: the scene's own render skips)  // NAME 1: neutral
	bool InsideRenderWithPIPScenes;							// +0x99  // NAME 2: the assert's
	InterfaceStaticSortListClass *SSL;						// +0x9C (the asserts' 'ssl')  // NAME 2: the asserts'
};

#endif //__INTERFACE_SCENE_H
