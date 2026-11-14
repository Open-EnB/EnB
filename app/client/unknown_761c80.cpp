// Earth & Beyond client (reconstruction): app\client\unknown_761c80.cpp, the simple scene that lights its render
// objects through light environments of their own (the starbase view's +0x70 scene). No string names the file (no
// asserts): the name is an interim one (D137). Notes at the end.
#include "unknown_761c80.h"
#include "light.h"
#include "lightenvironment.h"
#include "rendobj.h"
#include "rinfo.h"
#include "camera.h"

// @0x761C80
Unknown761C80Class::Unknown761C80Class() :
	Light(NULL),
	DefaultLightEnvironment(NULL),
	FillIntensity(0.5f),
	LightEnvironments(100)
{
	Light = new LightClass(LightClass::POINT);
	Light->Set_Far_Attenuation_Range(1000000.0, 1000000.0);
	Light->Set_Ambient(Vector3(1.0f, 1.0f, 1.0f));
	Light->Set_Diffuse(Vector3(0.0f, 0.0f, 0.0f));
	Light->Set_Specular(Vector3(0.0f, 0.0f, 0.0f));

	DefaultLightEnvironment = new LightEnvironmentClass;
	DefaultLightEnvironment->Reset(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f));
	DefaultLightEnvironment->Add_Light(*Light);

	EnvironmentAmbient.Set(0.0f, 0.0f, 0.0f);
}

// @0x761F70
Unknown761C80Class::~Unknown761C80Class()
{
	if (Light) {
		Light->Release_Ref();
		Light = NULL;
	}
	if (DefaultLightEnvironment) {
		delete DefaultLightEnvironment;
		DefaultLightEnvironment = NULL;
	}
	std::hash_map<RenderObjClass *, LightEnvironmentClass *>::iterator it;
	for (it = LightEnvironments.begin(); it != LightEnvironments.end(); ++it) {
		delete (*it).second;
	}
	LightEnvironments.clear();
}

// @0x7621A0
void Unknown761C80Class::Add_Light_Environment(RenderObjClass *obj)
{
	LightEnvironmentClass *env = new LightEnvironmentClass;
	env->Set_Fill_Intensity(0.66f);
	LightEnvironments[obj] = env;
}

// @0x7623D0
void Unknown761C80Class::Add_Render_Object(RenderObjClass *obj)
{
	SceneClass::Add_Render_Object(obj);
	RenderList.Add(obj);
}

// @0x762410
// NOT EXACT: inline budget: the client calls the bucket vector's size() (0x762C30) in the inlined erase, this
// inlines it (9 bytes longer; the rest is the client's)
void Unknown761C80Class::Remove_Render_Object(RenderObjClass *obj)
{
	SimpleSceneClass::Remove_Render_Object(obj);
	Remove_Light_Environment(obj);
}

// @0x762520
void Unknown761C80Class::Customized_Render(RenderInfoClass &rinfo)
{
	if (!Visibility_Checked) {
		Visibility_Check(&rinfo.Camera);
	}
	Visibility_Checked = false;

	RefRenderObjListIterator it(&UpdateList);
	for (it.First(); !it.Is_Done(); it.Next()) {
		it.Peek_Obj()->On_Frame_Update();
	}

	for (it.First(&RenderList); !it.Is_Done(); it.Next()) {
		RenderObjClass *robj = it.Peek_Obj();
		if (robj->Is_Really_Visible()) {
			LightEnvironmentClass *lenv = Peek_Light_Environment(robj);
			if (lenv != NULL) {
				lenv->Reset(robj->Get_Transform().Get_Translation(), EnvironmentAmbient);
				lenv->Set_Fill_Intensity(FillIntensity);
				RefRenderObjListIterator light(&LightList);
				for (light.First(); !light.Is_Done(); light.Next()) {
					lenv->Add_Light(*(LightClass *)light.Peek_Obj());
				}
				lenv->Pre_Render_Update(rinfo.Camera.Get_Transform());
				rinfo.light_environment = lenv;
			} else {
				DefaultLightEnvironment->Pre_Render_Update(rinfo.Camera.Get_Transform());
				rinfo.light_environment = DefaultLightEnvironment;
			}

			if (robj->Get_Render_Hook()) {
				if (robj->Get_Render_Hook()->Pre_Render(robj, rinfo)) {
					robj->Render(rinfo);
				}
				robj->Get_Render_Hook()->Post_Render(robj, rinfo);
			} else {
				robj->Render(rinfo);
			}
		}
	}
}

// @0x762790
LightEnvironmentClass *Unknown761C80Class::Peek_Light_Environment(RenderObjClass *obj)
{
	std::hash_map<RenderObjClass *, LightEnvironmentClass *>::const_iterator it = LightEnvironments.find(obj);
	if (it == LightEnvironments.end()) {
		return(NULL);
	}
	return((*it).second);
}

// @0x762800
void Unknown761C80Class::Remove_Light_Environment(RenderObjClass *obj)
{
	LightEnvironmentClass *env = Peek_Light_Environment(obj);
	if (env != NULL) {
		delete env;
		LightEnvironments.erase(obj);
	}
}

// @0x762920
void Unknown761C80Class::Set_Fill_Intensity(float intensity)
{
	FillIntensity = intensity;
}

// @0x762940
void Unknown761C80Class::Set_Environment_Ambient(const Vector3 &color)
{
	EnvironmentAmbient = color;
}

// Notes (reconstruction)
// * The object is 0x761C80-0x76304F: the constructor follows env_map_manager.cpp's vector helpers (its last,
//   0x761BC0); after the class's own code come its hash_map's instances (0x761EE0 the hash_map's destructor, called
//   from the constructor's and the destructor's unwind code; 0x762980 the const_iterator from an iterator; 0x7629B0
//   the table's constructor (100 buckets); 0x762B80 find; 0x762BF0 / 0x762C30 / 0x762C60 / 0x762F90 / 0x763010 the
//   bucket vector's destructor, size, insert, constructor, swap; 0x762F00 _Construct; 0x762F60 _M_bkt_num_key), then
//   the guarded ctype<unsigned short>::id static 0x762F30; proj_decal_manager.cpp starts at 0x763050.
// * The vtable 0xB03E4C is SimpleSceneClass's with slots +0x04 (deleting destructor), +0x08 (Get_Scene_ID const:
//   0x10001), +0x0C / +0x10 (Add / Remove_Render_Object) and +0x50 (Customized_Render) replaced.
// * Customized_Render is SimpleSceneClass's without the four global lights: a render object with a light environment
//   (Peek_Light_Environment, inlined, its const_iterator conversions out of line as in the client) gets it reset to
//   its position and EnvironmentAmbient, FillIntensity, every light of LightList; the others get
//   DefaultLightEnvironment. The destructor's loop iterator is declared before the loop (the client's form).
// * ui_starbase.cpp calls Add_Light_Environment for its avatars and ships, Set_Fill_Intensity and
//   Set_Environment_Ambient from the starbase's lights (0x640E90 / 0x640EC0); Peek_ and Remove_Light_Environment
//   have no caller in the client (Remove_Render_Object inlines the latter).
