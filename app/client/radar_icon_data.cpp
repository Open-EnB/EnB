// radar_icon_data.cpp: the icon a client object shows on a radar (0x7BC040-0x7BC6D0); notes at the end
#include <string>
#include "radar_icon_data.h"
#include "radar_data.h"
#include "co_data.h"
#include "assetmgr.h"
#include "rendobj.h"
#include "matinfo.h"
#include "vertmaterial.h"
#include "..\shared\g_assert.h"

void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl)

// @0x7BC040
RadarIconDataClass::RadarIconDataClass(ClientObjectClass *co, const char *icon_name) :
	ClientObjectDataClass(co),
	Position(0.0f, 0.0f),
	IconRObj(NULL),
	VMat(NULL),
	SortLevel(0)
{
	Set_Icon(icon_name);
}

// @0x7BC100
RadarIconDataClass::~RadarIconDataClass()
{
	Remove_From_Scene();
}

// @0x7BC1F0
bool RadarIconDataClass::Add_To_Scene(const RefCountPtr<SceneClass> &scene)
{






























	FAIL_IF(!IconRObj) return(false);
	if (!IconRObj->Peek_Scene()) {
		IconRObj->Add(scene.Peek());
		return(true);
	}
	return(false);
}

// @0x7BC260
bool RadarIconDataClass::Remove_From_Scene()
{
	FAIL_IF(!IconRObj) return(false);
	if (IconRObj->Peek_Scene()) {
		IconRObj->Remove();
		return(true);
	}
	return(false);
}

// @0x7BC2B0
RefCountPtr<RenderObjClass> RadarIconDataClass::Get_Render_Obj()
{
	return(NULL);
}

// @0x7BC2E0
SceneClass *RadarIconDataClass::Peek_Scene()
{
	FAIL_IF(!IconRObj) return(NULL);
	return(IconRObj->Peek_Scene());
}

// @0x7BC320
void RadarIconDataClass::Set_Icon(const char *icon_name)
{
	SceneClass *scene = NULL;
	if (IconRObj) {
		scene = IconRObj->Peek_Scene();
		Remove_From_Scene();
		IconRObj = NULL;
		VMat = NULL;
	}
	if (icon_name && *icon_name) {
		IconRObj = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj(icon_name));





		FAIL_IF(!IconRObj) return;
		Unknown_496AE0(IconRObj.Peek(), false);
		SortLevel = IconRObj->Get_Sort_Level();
		RefCountPtr<MaterialInfoClass> matinfo = Create_NEW(IconRObj->Get_Material_Info());
		VMat = Create_NEW(matinfo->Get_Vertex_Material(0));
		if (scene) {
			IconRObj->Add(scene);
		}
	}
}

// @0x7BC5C0
bool RadarIconDataClass::Is_In_Scene()
{
	return(IconRObj->Peek_Scene() != NULL);
}

// @0x7BC5E0
const Vector3 &RadarIconDataClass::Get_Color()
{
	RadarDataClass *rdata = Get_Radar_Data(Get_Client_Object(), false);




	FAIL_IF(!rdata || !VMat) {
		static Vector3 black(0.0f, 0.0f, 0.0f);
		return(black);
	}
	const Vector3 &color = rdata->Color;
	VMat->Set_Emissive(color.X, color.Y, color.Z);
	return(color);
}

// @0x7BC6D0
RefCountPtr<RenderObjClass> &RadarIconDataClass::Get_Icon_RObj()
{
	return(IconRObj);
}

/*
**	Notes
**
**	The object: 0x7BC040 (the client's __FILE__ 0xB9F220, FAIL_IFs at lines 64-138) to 0x7BC6F0, <string>'s guarded
**	initialiser; radar_system.cpp's constructor 0x7BC770 opens the next.
*/
