// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\radar_icon_data.h (name assumed from radar_icon_data.cpp)
**
**	The base of the gutter and starmap data (constructor 0x7BC040, which calls ClientObjectDataClass's);
**	its own size and members are not rebuilt: declared only as their base.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __RADAR_ICON_DATA_H  // NAME 3 [radar_icon_data.h]: header name assumed from the client's radar_icon_data.cpp
#define __RADAR_ICON_DATA_H

#include "co_data.h"
#include "ref_ptr.h"
#include "vector2.h"

class RenderObjClass;
class SceneClass;
class Vector3;
class VertexMaterialClass;

class RadarIconDataClass : public ClientObjectDataClass  // NAME 3: by convention (D24) from the client's radar_icon_data.cpp
{
public:
	RadarIconDataClass(ClientObjectClass *co, const char *icon_name);	// 0x7BC040 (radar_icon_data.cpp)  // NAME 2 [icon_name]: gutter_data.cpp's 'gutter_icon'
	virtual ~RadarIconDataClass();							// +0x04 0x7BC0D0 (0x7BC100)

	virtual void Set_Icon(const char *icon_name);			// +0x08 0x7BC320  // NAME 2: what it does
	virtual bool Add_To_Scene(const RefCountPtr<SceneClass> &scene);	// +0x0C 0x7BC1F0: false when already in one  // NAME 2: what it does
	virtual bool Remove_From_Scene();						// +0x10 0x7BC260 (gutter_data.cpp's 0x7B5A60)  // NAME 2: what it does
	virtual RefCountPtr<RenderObjClass> Get_Render_Obj();	// +0x14 0x7BC2B0 (gutter_data.cpp's 0x7B5AD0): mouse_targeting.cpp's 0x7B7880 intersects it  // NAME 2: what it returns

	RefCountPtr<RenderObjClass> &Get_Icon_RObj();			// 0x7BC6D0 (the FAIL_IF's name '!Get_Icon_RObj()')
	SceneClass *Peek_Scene();								// 0x7BC2E0  // NAME 2: what it returns
	bool Is_In_Scene();										// 0x7BC5C0  // NAME 2: what it returns
	const Vector3 &Get_Color();								// 0x7BC5E0: the radar data's, through the icon's material  // NAME 2: what it returns

	Vector2 Position;										// +0x08: 0..1 of the screen  // NAME 2: mouse_targeting.cpp's 0x7B7880 compares it with the mouse
	RefCountPtr<RenderObjClass> IconRObj;					// +0x10 (the FAIL_IF's name)
	RefCountPtr<VertexMaterialClass> VMat;					// +0x14 (the FAIL_IF's name)
	int SortLevel;											// +0x18: the icon's, from its slot +0x168 at 0x7BC320  // NAME 2: what it holds
};

#endif //__RADAR_ICON_DATA_H
