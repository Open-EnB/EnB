// Earth & Beyond client (reconstruction): app\client\unknown_761c80.h, a simple scene that lights each of its render
// objects through a light environment of its own: the starbase view's +0x70 scene (sbaseview.cpp). No string names
// the file: unknown_761c80.cpp and this header are interim names (D137), the object 0x761C80-0x76304F (after
// env_map_manager.cpp's vector helpers, before proj_decal_manager.cpp's first function 0x763050).
#ifndef UNKNOWN_761C80_H  // NAME 1 [unknown_761c80.h]: interim header name (D137)
#define UNKNOWN_761C80_H

#include <hash_map.h>
#include "unknown_robj_hash.h"	// hash<RenderObjClass *>
#include "scene.h"
#include "vector3.h"

class LightClass;
class LightEnvironmentClass;
class RenderObjClass;
class RenderInfoClass;

// The scene (0xC8 bytes; constructor 0x761C80, vtable 0xB03E4C, destructor 0x761F70, deleting 0x761EB0). Its render
// objects given a light environment (Add_Light_Environment) are lit by it: reset to the object's position and
// EnvironmentAmbient each frame, with every light of the scene's LightList; the others share DefaultLightEnvironment
// (a white ambient Light only).
class Unknown761C80Class : public SimpleSceneClass	// NAME 1: neutral (D10)
{
public:
	Unknown761C80Class();								// 0x761C80
	virtual ~Unknown761C80Class();						// 0x761F70

	virtual int Get_Scene_ID() const { return(0x10001); }	// +0x08 0x761E90

	virtual void Add_Render_Object(RenderObjClass *obj);		// +0x0C 0x7623D0
	virtual void Remove_Render_Object(RenderObjClass *obj);		// +0x10 0x762410 (its light environment deleted)

	void Add_Light_Environment(RenderObjClass *obj);			// 0x7621A0 (ui_starbase.cpp: the avatars and ships)  // NAME 2: what it does
	LightEnvironmentClass *Peek_Light_Environment(RenderObjClass *obj);	// 0x762790 (no caller)  // NAME 2: what it does
	void Remove_Light_Environment(RenderObjClass *obj);		// 0x762800 (no caller)  // NAME 2: what it does
	void Set_Fill_Intensity(float intensity);					// 0x762920 (each environment's FillIntensity)  // NAME 2: LightEnvironmentClass's
	void Set_Environment_Ambient(const Vector3 &color);		// 0x762940 (each environment's scene ambient)  // NAME 2: what it does

protected:
	virtual void Customized_Render(RenderInfoClass &rinfo);	// +0x50 0x762520

	LightClass *Light;									// +0x98 (white ambient, DefaultLightEnvironment's light)  // NAME 2: its type
	LightEnvironmentClass *DefaultLightEnvironment;		// +0x9C  // NAME 2: what it holds
	Vector3 EnvironmentAmbient;							// +0xA0  // NAME 2: what it holds
	float FillIntensity;								// +0xAC (0.5)  // NAME 2: LightEnvironmentClass's
	std::hash_map<RenderObjClass *, LightEnvironmentClass *> LightEnvironments;	// +0xB0 (100 buckets)  // NAME 2: what it holds
};

#endif // UNKNOWN_761C80_H
