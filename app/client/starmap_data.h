// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\starmap_data.h (name assumed from starmap_data.cpp)
**
**	A client object's starmap data (vtable 0xB06980, base radar_icon_data.cpp's 0x7BC040), bound to the
**	object through co_data.cpp; declared as far as co_data.cpp needs it (its constructor and size).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __STARMAP_DATA_H  // NAME 3 [starmap_data.h]: header name assumed from the client's starmap_data.cpp
#define __STARMAP_DATA_H

#include "radar_icon_data.h"
#include "vector3.h"

class RadarSystemDataCacheClass;
class DynamicMeshClass;
class TextureClass;

class StarmapDataClass : public RadarIconDataClass  // NAME 5: the client's typeid name .PAVStarmapDataClass@@ (co_data.cpp's registry)
{
public:
	StarmapDataClass(ClientObjectClass *co);			// 0x7BE640 (starmap_data.cpp)
	virtual ~StarmapDataClass();						// +0x04 0x7BE750 (0x7BE780)

	virtual void Set_Icon(const char *icon_name);		// +0x08 0x7BE860
	virtual bool Remove_From_Scene();					// +0x10 0x7BE900
	virtual RefCountPtr<RenderObjClass> Get_Render_Obj();	// +0x14 0x7BE940: the mesh

	bool Update(RadarSystemDataCacheClass *cache, const Vector3 &pos, const RefCountPtr<SceneClass> &scene);	// 0x7BE970 (0x7C0620)  // NAME 2 [Update, cache, pos, scene]: what it does, what 0x7C06D9 passes
	void Update_Transform(const Vector3 &pos, const Vector2 &position, float angle, float size, bool rotate_icon);	// 0x7BEAD0 (0x7C0620)  // NAME 2 [Update_Transform, pos, position, angle, size, rotate_icon]: what it does
	Vector3 Get_Position();								// 0x7BEF20 (0x7B9DC0, 0x7C3830)  // NAME 2: the icon's position

	Vector3 WorldPosition;								// +0x1C  // NAME 2: what it holds
	RefCountPtr<DynamicMeshClass> Mesh;					// +0x28  // NAME 2: what it holds
	RefCountPtr<TextureClass> Texture;					// +0x2C  // NAME 2: what it holds
	float Radius;										// +0x30  // NAME 2: the object's slot +0x30
	float MeshSize;										// +0x34  // NAME 2: what it holds
	bool Unknown38;										// +0x38 the icon is 'm_unknown'

private:
	void Create_Mesh();									// 0x7BEF50  // NAME 2: what it does
};

#endif //__STARMAP_DATA_H
