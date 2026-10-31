// Earth & Beyond client (reconstruction)
#include <windows.h>
#include "eff_o.h"
#include "sclient.h"
#include "..\shared\unknown_debug.h"
#include "..\..\ww3d2\scene.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\assetmgr.h"
#include "..\..\ww3d2\dx8renderer.h"
#include "..\..\ww3d2\dx8wrapper.h"
#include "..\..\ww3d2\camera.h"
#include "..\..\ww3d2\light.h"
#include "..\..\ww3d2\dazzle.h"
#include "..\shared\stars.h"
#include "wwmath.h"
#include <stdlib.h>
#include <string.h>
// E&B's asset manager at 0xBDADC0 (co.cpp's UnknownBDADC0Class): the asserts name it AssetManager
class UnknownBDADC0Class : public WW3DAssetManager	// NAME 1: neutral (D10)
{
};
extern UnknownBDADC0Class *Unknown_BDADC0;	// NAME 1 [Unknown_BDADC0]: 0xBDADC0
#define AssetManager Unknown_BDADC0	// NAME 3 [AssetManager]: the asserts' text 'AssetManager' (0x802D30, the destructor)

// the screen fade and the letterbox (0x7FF830-0x7FFC70, the file before this one: its globals at 0xBE8B60..)
void Unknown_7FF830(bool on, float time);							// 0x7FF830: the letterbox  // NAME 1 [on, time]: what Enable_Letterbox passes
void Unknown_7FFA30(float red, float green, float blue, float time);	// 0x7FFA30: the fade's colour  // NAME 1 [red, green, blue, time]: what Set_Screen_Fade_Color passes
void Unknown_810750(RenderObjClass *model, bool on);					// 0x810750 (shiputils.cpp; co.h declares it so): a ship model's engine trails  // NAME 1 [model, on]: what co.cpp passes
void Unknown_7FFBE0(float opacity, float time);						// 0x7FFBE0: the fade's opacity  // NAME 1 [opacity, time]: what Set_Screen_Fade_Opacity passes

// 0x7FFEA0: by frame; within a frame the objects and lights are made first
bool operator < (const ScriptedMovieLineClass &a, const ScriptedMovieLineClass &b)
{
	if (a.Frame < b.Frame) {
		return(true);
	}
	if (a.Frame == b.Frame) {
		if (strncmp(a.Text, "CREATE_OBJECT", 13) == 0 && strncmp(b.Text, "CREATE_OBJECT", 13) != 0) {
			return(true);
		}
		if (strncmp(a.Text, "CREATE_OBJECT", 13) != 0 && strncmp(b.Text, "CREATE_OBJECT", 13) == 0) {
			return(false);
		}
		if (strncmp(a.Text, "CREATE_SHIP_OBJECT", 18) == 0 && strncmp(b.Text, "CREATE_SHIP_OBJECT", 18) != 0) {
			return(true);
		}
		if (strncmp(a.Text, "CREATE_SHIP_OBJECT", 18) != 0 && strncmp(b.Text, "CREATE_SHIP_OBJECT", 18) == 0) {
			return(false);
		}
		if (strncmp(a.Text, "CREATE_POINT_LIGHT", 18) == 0 && strncmp(b.Text, "CREATE_POINT_LIGHT", 18) != 0) {
			return(true);
		}
		if (strncmp(a.Text, "CREATE_POINT_LIGHT", 18) != 0 && strncmp(b.Text, "CREATE_POINT_LIGHT", 18) == 0) {
			return(false);
		}
		if (strncmp(a.Text, "CREATE_DIRECTIONAL_LIGHT", 24) == 0 && strncmp(b.Text, "CREATE_DIRECTIONAL_LIGHT", 24) != 0) {
			return(true);
		}
		if (strncmp(a.Text, "CREATE_DIRECTIONAL_LIGHT", 24) != 0 && strncmp(b.Text, "CREATE_DIRECTIONAL_LIGHT", 24) == 0) {
			return(false);
		}
	}
	return(false);
}

// 0x7FFD30: the line starts with name (any case): the cursor past it and its comma
bool ScriptedMovieParamsClass::Unknown_7FFD30(const char *name)
{
	int len = strlen(name);
	if (strnicmp(Unknown08, name, len) == 0) {
		Unknown08 += len;
		while (*Unknown08 != 0 && *Unknown08 != ',') {
			Unknown08++;
		}
		if (*Unknown08 == ',') {
			Unknown08++;
		}
		return(true);
	}
	return(false);
}

// 0x7FFDC0: the next parameter (up to a comma; leading blanks skipped, a quoted one unquoted, trailing blanks cut)
char *ScriptedMovieParamsClass::Unknown_7FFDC0()
{
	if (Unknown08 == NULL || *Unknown08 == 0) {
		return(NULL);
	}
	char *param = Unknown08;
	while (*param != 0 && *param <= ' ') {
		param++;
	}
	if (*param == '"') {
		param++;
		Unknown08 = param;
		while (*Unknown08 != 0 && *Unknown08 != '"') {
			Unknown08++;
		}
	}
	while (*Unknown08 != 0 && *Unknown08 != ',') {
		Unknown08++;
	}
	if (*Unknown08 != 0) {
		*Unknown08 = 0;
		Unknown08++;
	}
	unsigned int len = strlen(param);
	while (len != 0 && param[len - 1] <= ' ') {
		param[--len] = 0;
	}
	if (len != 0 && param[len - 1] == '"') {
		param[len - 1] = 0;
	}
	return(param);
}
// the render passes' depth ranges and clip planes (0xB07C3C-0xB07C54)
static float Unknown_B07C3C = 1.0f;	// NAME 1: neutral (D10)
static float Unknown_B07C40 = 1.0f;	// NAME 1: neutral (D10)
static float Unknown_B07C44 = 1.0f;	// NAME 1: neutral (D10)
static float Unknown_B07C48 = 100000.0f;	// NAME 1: neutral (D10)
static float Unknown_B07C4C = 20.0f;	// NAME 1: neutral (D10)
static float Unknown_B07C50 = 150.0f;	// NAME 1: neutral (D10)
static float Unknown_B07C54 = 50.0f;	// NAME 1: neutral (D10)
static float Unknown_B07C58 = 1.0f;	// NAME 1: neutral (D10)
static float Unknown_B07C5C = 0.95f;	// NAME 1: neutral (D10)
static float Unknown_B07C60 = 1.0f;	// NAME 1: neutral (D10)

































































































































































































































































































































































































































// 0x801860: the Unknown120 scene through the scripted camera at its own depth range and clip planes, then the camera's back
void ScriptedMovieViewClass::Unknown_801860()
{
	if (Unknown120) {
		G_ASSERT(ScriptedCamera);
		float znear;
		float zfar;
		ScriptedCamera->Get_Clip_Planes(znear, zfar);
		float zmin;
		float zmax;
		ScriptedCamera->Get_Depth_Range(&zmin, &zmax);
		ScriptedCamera->Set_Depth_Range(Unknown_B07C44, Unknown_B07C40);
		ScriptedCamera->Set_Clip_Planes(Unknown_B07C4C, Unknown_B07C48);
		WW3D::Render(Unknown120, ScriptedCamera);
		ScriptedCamera->Set_Clip_Planes(znear, zfar);
		ScriptedCamera->Set_Depth_Range(zmin, zmax);
	}
}























// 0x8019A0: the StarsScene scene through the scripted camera at its own depth range and clip planes, then the camera's back
void ScriptedMovieViewClass::Unknown_8019A0()
{
	if (StarsScene) {
		G_ASSERT(ScriptedCamera);
		float znear;
		float zfar;
		ScriptedCamera->Get_Clip_Planes(znear, zfar);
		float zmin;
		float zmax;
		ScriptedCamera->Get_Depth_Range(&zmin, &zmax);
		ScriptedCamera->Set_Depth_Range(Unknown_B07C3C, Unknown_B07C3C);
		ScriptedCamera->Set_Clip_Planes(Unknown_B07C54, Unknown_B07C50);
		WW3D::Render(StarsScene, ScriptedCamera);
		ScriptedCamera->Set_Clip_Planes(znear, zfar);
		ScriptedCamera->Set_Depth_Range(zmin, zmax);
	}
}


















// 0x801AE0: the orbital scene at depth 0.95..1 between the given clip planes, then the camera's back
void ScriptedMovieViewClass::Unknown_801AE0(float znear, float zfar)
{
	G_ASSERT(OrbitalScene);
	G_ASSERT(ScriptedCamera);
	float zmin;
	float zmax;
	ScriptedCamera->Get_Depth_Range(&zmin, &zmax);
	float old_znear;
	float old_zfar;
	ScriptedCamera->Get_Clip_Planes(old_znear, old_zfar);
	ScriptedCamera->Set_Depth_Range(Unknown_B07C5C, Unknown_B07C58);
	ScriptedCamera->Set_Clip_Planes(znear, zfar);
	WW3D::Render(OrbitalScene, ScriptedCamera);
	ScriptedCamera->Set_Depth_Range(zmin, zmax);
	ScriptedCamera->Set_Clip_Planes(old_znear, old_zfar);
}




















// 0x801C40: the background dazzle layer at depth 1 between the given clip planes, then the camera's back
void ScriptedMovieViewClass::Unknown_801C40(float znear, float zfar)
{
	G_ASSERT(ScriptedCamera);
	G_ASSERT(BackgroundDazzleLayer);
	float old_znear;
	float old_zfar;
	ScriptedCamera->Get_Clip_Planes(old_znear, old_zfar);
	float zmin;
	float zmax;
	ScriptedCamera->Get_Depth_Range(&zmin, &zmax);
	ScriptedCamera->Set_Depth_Range(Unknown_B07C60, Unknown_B07C60);
	ScriptedCamera->Set_Clip_Planes(znear, zfar);
	BackgroundDazzleLayer->Render(ScriptedCamera);
	ScriptedCamera->Set_Clip_Planes(old_znear, old_zfar);
	ScriptedCamera->Set_Depth_Range(zmin, zmax);
}
























// 0x801D70: the orbitals' near plane scaled for the camera's FOV, and their far plane (the main view's 0x50CCF0)
void ScriptedMovieViewClass::Unknown_801D70(float *znear, float *zfar)
{
	G_ASSERT(ScriptedCamera);
	G_ASSERT(OrbitalManager);
	float v = tan(ScriptedCamera->Get_Vertical_FOV() * 0.5f);
	float h = tan(ScriptedCamera->Get_Horizontal_FOV() * 0.5f);
	*znear = (1.0f / WWMath::Sqrt(h * h + v * v + 1.0f)) * OrbitalManager->Unknown50;
	*zfar = OrbitalManager->Get_Unknown54();
}























































































#include "..\packets\pkcolorization.h"
void Unknown_80EFF0(RenderObjClass *model, std::vector<ColorizationRegionStruct> &regions, SClientUnknown1338Class *unknown);	// 0x80EFF0 (shiputils.cpp; cpacket.cpp declares it so)
// NOT EXACT: shape (0.38): the client copies b's half into each region through an out-of-line 16-byte copy (0x780AC0, avatar.cpp's) three times, frame 0x40 to our 0x30
// 0x8020B0: the ship model's four colourization regions, each a then b, in a metal (shiputils.cpp's 0x80EFF0)
void ScriptedMovieViewClass::Unknown_8020B0(RenderObjClass *model, Vector3 a, Vector3 b, int metal)
{
	std::vector<ColorizationRegionStruct> regions;
	ColorizationHalfStruct second;
	second.Unknown00 = metal;
	second.Unknown04 = b;
	ColorizationRegionStruct region;
	region.Unknown00.Unknown00 = metal; region.Unknown00.Unknown04 = a; region.Unknown10 = second;
	regions.push_back(region);
	region.Unknown00.Unknown00 = metal; region.Unknown00.Unknown04 = a; region.Unknown10 = second;
	regions.push_back(region);
	region.Unknown00.Unknown00 = metal; region.Unknown00.Unknown04 = a; region.Unknown10 = second;
	regions.push_back(region);
	region.Unknown00.Unknown00 = metal; region.Unknown00.Unknown04 = a; region.Unknown10 = second;
	regions.push_back(region);
	Unknown_80EFF0(model, regions, Unknown140);
}












// 0x8022C0: the ambient light's fade one step on
void ScriptedMovieViewClass::Unknown_8022C0(float time)
{
	FAIL_IF(ScriptedScene == NULL) {return;}
	Unknown88.Unknown_80A5A0(time);
	ScriptedScene->Set_Ambient_Light(Unknown88.Current);
}
















// 0x802330: the fog colour's fade one step on
void ScriptedMovieViewClass::Unknown_802330(float time)
{
	FAIL_IF(ScriptedScene == NULL) {return;}
	UnknownAC.Unknown_80A5A0(time);
	ScriptedScene->Set_Fog_Color(UnknownAC.Current);
}















// 0x8023A0
void ScriptedMovieViewClass::Unknown_8023A0(bool on)
{
	G_ASSERT(ClientInterface);
	if (on) {
		ClientInterface->Unknown_04(ScriptedCamera);
		if (Unknown6C && !Unknown6D) {
			Unknown6D = true;
		}
	} else {
		ClientInterface->Unknown_04(0);
	}
}
















































































// 0x8027C0: the camera onto the next of CameraList (its view plane too, when it has one)
void ScriptedMovieViewClass::Unknown_8027C0()
{
	int size = CameraList.size();
	if (size != 0) {
		G_ASSERT(CurrentCameraIndex < size);
		G_ASSERT(CameraList[CurrentCameraIndex].RenderObject);
		if (ScriptedCamera->Get_Container()) {
			ScriptedCamera->Remove();
		}
		if (CameraList[CurrentCameraIndex].HFov != 0.0f) {
			ScriptedCamera->Set_View_Plane(CameraList[CurrentCameraIndex].HFov, CameraList[CurrentCameraIndex].VFov);
		}
		CameraList[CurrentCameraIndex].RenderObject->Add_Sub_Object_To_Bone(ScriptedCamera, CameraList[CurrentCameraIndex].Bone);
		CurrentCameraIndex++;
		if (CurrentCameraIndex >= size) {
			CurrentCameraIndex = 0;
		}
	}
}








// 0x802900: +0x6D set once +0x6C is
void ScriptedMovieViewClass::Unknown_802900()
{
	if (Unknown6C && !Unknown6D) {
		Unknown6D = true;
	}
}

// 0x802930: SClient::Do_Frame ends the movie when it is true (also before +0x6C / +0x6D are set)
// NOT EXACT: the client normalizes the last test (setne) and returns the early true through mov al, 1; ours jumps
// to one mov eax, 1 (forms: the && chain, an if with != false, this || chain)
bool ScriptedMovieViewClass::Unknown_802930()
{
	return(!Unknown6C || !Unknown6D || Unknown6E);
}

// 0x802960: the keys while the movie plays (SClient::Do_Frame): Esc ends it, 0x22 the next camera, space
// pauses, 0x6B five seconds on
void ScriptedMovieViewClass::Unknown_802960(KernelEventClass event)
{
	if (!(event.Flags & 0x1000)) {
		switch (event.Key) {
			case 0x1B:
				Unknown6E = true;
				return;
			case 0x22:
				Unknown_8027C0();
				return;
			case 0x20:
				if (Unknown171 && ClientInterface->Unknown_00() && ClientInterface->Unknown_00()->Unknown_1C()) {
					Unknown168 = !Unknown168;
					return;
				}
				break;
			case 0x6B:
				if (Unknown171 && ClientInterface->Unknown_00() && ClientInterface->Unknown_00()->Unknown_1C()) {
					Unknown164 += 5000;
				}
				break;
		}
	}
}









































































// 0x802AD0: every sound of +0xF0
void ScriptedMovieViewClass::Unknown_802AD0(int arg)
{
	for (std::vector<ScriptedMovieSoundClass *>::iterator i = UnknownF0.begin(); i != UnknownF0.end(); i++) {
		(*i)->Unknown_7ED6C0(arg, arg);
	}
}
















// 0x802B20: the movie's time on, the effects that are over ended, then every line due run (a line that is not a
// command ends the movie)
// NOT EXACT: the inlined effect sweep (0x807520) swaps ebx / ebp with the client's (0.65); one form
void ScriptedMovieViewClass::Unknown_802B20()
{
	Unknown80 += WW3D::Get_Frame_Time();
	Unknown_807520(Unknown80);
	std::vector<ScriptedMovieLineClass *>::iterator itor = Lines.begin();
	while (itor != Lines.end()) {
		G_ASSERT(*itor);
		if (Unknown80 < (*itor)->Frame) {
			return;
		}
		if (!Unknown_803310(*itor)) {
			Unknown6E = true;
			MessageBox(NULL, (*itor)->Text, "Script Error", MB_OK);
			G_ASSERT(!"Invalid command.  Ending Movie");
			return;
		}
		delete *itor;
		Lines.erase(itor);
	}
}



















// 0x802D30: the unused assets released, the mesh renderer's buffers invalidated
void ScriptedMovieViewClass::Unknown_802D30()
{
	G_ASSERT(AssetManager);
	AssetManager->Release_Unused_Assets();
	TheDX8MeshRenderer.Invalidate();
	DX8Wrapper::Flush_DX8_Resource_Manager(0);
}



















































































































































































































































































































// 0x8046E0: the slot's object deleted and taken off the list
bool ScriptedMovieViewClass::Destroy_Object(ScriptedMovieParamsClass *params)
{
	const char *param = params->Unknown_7FFDC0();
	FAIL_IF((param == NULL) && "User error: Destroy_Object: Need slot number") {return(false);}
	int slot = atoi(param);
	for (std::vector<ScriptedMovieObjectStruct>::iterator itor = Objects.begin(); itor != Objects.end(); itor++) {
		if ((*itor).Slot == slot) {
			UIMeshClass *mesh = (*itor).Mesh;


			G_ASSERT(mesh);
			mesh->Unknown_5B92B0();
			delete mesh;
			Objects.erase(itor);
			return(true);
		}
	}
	return(false);
}











































































// 0x804A70: the star field in a scene of its own (the system's stars.ini, its polygon budget)
bool ScriptedMovieViewClass::Unknown_804A70(ScriptedMovieParamsClass *params)
{
	int system = 800;
	const char *param = params->Unknown_7FFDC0();
	if (param) {
		system = atoi(param);
	}
	unsigned int budget = 1000;
	param = params->Unknown_7FFDC0();
	if (param) {
		budget = atoi(param);
	}
	G_ASSERT(StarsScene == NULL);
	G_ASSERT(Stars == NULL);
	StarsScene = new SimpleSceneClass;
	Stars = Create_NEW(new StarFieldClass(budget));
	Stars->Init(system);
	StarsScene->Add_Render_Object(Stars.Peek());
	StarsScene->Set_Fog_Enable(false);
	return(true);
}






























































// 0x805000: the slot's animation: its name, then loop (non-zero), and two numbers (0 and -1 by default)
bool ScriptedMovieViewClass::Play_Animation(ScriptedMovieParamsClass *params)
{
	const char *param = params->Unknown_7FFDC0();
	FAIL_IF((param == NULL) && "User error: Play_Animation: Need slot number") {return(false);}
	int slot = atoi(param);
	for (std::vector<ScriptedMovieObjectStruct>::iterator itor = Objects.begin(); itor != Objects.end(); itor++) {
		if ((*itor).Slot == slot) {


			G_ASSERT((*itor).Mesh);
			const char *anim_name = params->Unknown_7FFDC0();

			FAIL_IF((anim_name == NULL) && "User error: Play_Animation: Need animation name") {return(false);}
			bool loop = false;
			param = params->Unknown_7FFDC0();
			if (param && atoi(param)) {
				loop = true;
			}
			int first = 0;
			param = params->Unknown_7FFDC0();
			if (param) {
				first = atoi(param);
			}
			int last = -1;
			param = params->Unknown_7FFDC0();
			if (param) {
				last = atoi(param);
			}
			(*itor).Mesh->Unknown_5B9550(0);
			(*itor).Mesh->Unknown70 &= ~1;
			(*itor).Mesh->Unknown_5B8DF0(0, anim_name, loop, 0, 0, first, last);
			(*itor).Mesh->Unknown70 &= ~2;
			return(true);
		}
	}
	return(false);
}




















// 0x8051B0: the slot's animation, held (its flag bit 1 set)
bool ScriptedMovieViewClass::Unknown_8051B0(ScriptedMovieParamsClass *params)
{
	const char *param = params->Unknown_7FFDC0();
	FAIL_IF((param == NULL) && "User error: Play_Animation: Need slot number") {return(false);}
	int slot = atoi(param);
	for (std::vector<ScriptedMovieObjectStruct>::iterator itor = Objects.begin(); itor != Objects.end(); itor++) {
		if ((*itor).Slot == slot) {


			G_ASSERT((*itor).Mesh);
			const char *anim_name = params->Unknown_7FFDC0();

			FAIL_IF((anim_name == NULL) && "User error: Play_Animation: Need animation name") {return(false);}
			(*itor).Mesh->Unknown_5B9550(0);
			(*itor).Mesh->Unknown70 &= ~1;
			(*itor).Mesh->Unknown_5B8DF0(0, anim_name, false, 0, 0, 0, -1);
			(*itor).Mesh->Unknown70 |= 2;
			return(true);
		}
	}
	return(false);
}


























// 0x8052E0: the camera onto a bone of the slot's mesh (out of its container first)
bool ScriptedMovieViewClass::Attach_Camera(ScriptedMovieParamsClass *params)
{
	const char *param = params->Unknown_7FFDC0();
	FAIL_IF((param == NULL) && "User error: Attach_Camera: Need slot number") {return(false);}
	int slot = atoi(param);
	for (std::vector<ScriptedMovieObjectStruct>::iterator itor = Objects.begin(); itor != Objects.end(); itor++) {
		if ((*itor).Slot == slot) {



			G_ASSERT((*itor).Mesh);
			param = params->Unknown_7FFDC0();
			FAIL_IF((param == NULL) && "User error: Attach_Camera: Need bone name") {return(false);}

			G_ASSERT(ScriptedCamera);
			if (ScriptedCamera->Get_Container()) {
				ScriptedCamera->Remove();
			}
			(*itor).Mesh->Unknown_5B9100(param, ScriptedCamera);
			return(true);
		}
	}
	return(false);
}












































































// 0x805810: the object slot's mesh onto a bone of the host slot's
bool ScriptedMovieViewClass::Attach_To_Bone(ScriptedMovieParamsClass *params)
{
	const char *param = params->Unknown_7FFDC0();
	FAIL_IF((param == NULL) && "User error: Attach_To_Bone: Need object slot") {return(false);}
	int object_slot = atoi(param);
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Attach_To_Bone: Need host slot") {return(false);}
	int host_slot = atoi(param);
	UIMeshClass *parent = NULL;
	UIMeshClass *child = NULL;
	for (std::vector<ScriptedMovieObjectStruct>::iterator itor = Objects.begin(); itor != Objects.end(); itor++) {
		if ((*itor).Slot == host_slot) {
			parent = (*itor).Mesh;
		} else if ((*itor).Slot == object_slot) {
			child = (*itor).Mesh;
		}
		if (child && parent) {
			break;
		}
	}






	G_ASSERT(child);
	G_ASSERT(parent);
	child->Unknown_5B92B0();
	param = params->Unknown_7FFDC0();




	FAIL_IF((param == NULL) && "User error: Attach_To_Bone: Need bone name") {return(false);}
	parent->Unknown_5B9130(param, child);
	return(true);
}
// 0x8059A0: the light slot's light onto a bone of the host slot's mesh
// NOT EXACT: after the loop the client knows parent is NULL (no second test before the G_ASSERT); forms: the
// slot and the mesh tested together, the mesh tested after the assignment (this one)
bool ScriptedMovieViewClass::Attach_Light_To_Bone(ScriptedMovieParamsClass *params)
{
	const char *param = params->Unknown_7FFDC0();
	FAIL_IF((param == NULL) && "User error: Attach_Light_To_Bone: Need light slot") {return(false);}
	int objectslot = atoi(param);

	G_ASSERT(objectslot < MAX_LIGHTS_PER_SCENE);
	G_ASSERT(Lights[objectslot]);
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Attach_Light_To_Bone: Need host slot") {return(false);}
	int host_slot = atoi(param);
	UIMeshClass *parent = NULL;
	for (std::vector<ScriptedMovieObjectStruct>::iterator itor = Objects.begin(); itor != Objects.end(); itor++) {
		if ((*itor).Slot == host_slot) {
			parent = (*itor).Mesh;
			if (parent) {
				break;
			}
		}
	}






	G_ASSERT(parent);
	Lights[objectslot]->Remove();
	param = params->Unknown_7FFDC0();




	FAIL_IF((param == NULL) && "User error: Attach_Light_To_Bone: Need bone name") {return(false);}
	parent->Unknown_5B9100(param, Lights[objectslot]);
	return(true);
}






































































































































// 0x806100: Lights[slot]'s intensity
bool ScriptedMovieViewClass::Set_Light_Intensity(ScriptedMovieParamsClass *params)
{
	const char *param = params->Unknown_7FFDC0();
	FAIL_IF((param == NULL) && "User error: Need light slot") {return(false);}
	int lightslot = atoi(param);

	FAIL_IF((lightslot < 0) && "User error: Set_Light_Intensity: Invalid light slot") {return(false);}
	FAIL_IF((lightslot >= MAX_LIGHTS_PER_SCENE) && "User error: Set_Light_Intensity: Exceeding 4 lights per scene") {return(false);}
	FAIL_IF((Lights[lightslot] == NULL) && "User error: Set_Light_Intensity: Can't change intensity if light doesn't exist") {return(false);}
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Set_Light_Intensity: Need light intensity") {return(false);}
	Lights[lightslot]->Set_Intensity(atof(param));
	return(true);
}


















// 0x806280: Lights[slot]'s far attenuation (the end defaults to the start)
bool ScriptedMovieViewClass::Set_Light_Far_Attenuation(ScriptedMovieParamsClass *params)
{
	const char *param = params->Unknown_7FFDC0();
	FAIL_IF((param == NULL) && "User error: Set_Light_Far_Attenuation: Need light slot") {return(false);}
	int lightslot = atoi(param);

	FAIL_IF((lightslot < 0) && "User error: Set_Light_Far_Attenuation: Invalid light slot") {return(false);}
	FAIL_IF((lightslot >= MAX_LIGHTS_PER_SCENE) && "User error: Set_Light_Far_Attenuation: Exceeding 4 lights per scene") {return(false);}
	FAIL_IF((Lights[lightslot] == NULL) && "User error: Set_Light_Far_Attenuation: Can't change attenuation if light doesn't exist") {return(false);}
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Set_Light_Far_Attenuation: Need start of far attenuation") {return(false);}
	float start = atof(param);
	float end = start;
	param = params->Unknown_7FFDC0();
	if (param) {
		end = atof(param);
	}
	Lights[lightslot]->Set_Far_Attenuation_Range(start, end);
	return(true);
}


















// 0x806440: Lights[slot]'s ambient color
bool ScriptedMovieViewClass::Set_Light_Ambient_Color(ScriptedMovieParamsClass *params)
{
	const char *param = params->Unknown_7FFDC0();
	FAIL_IF((param == NULL) && "User error: Set_Light_Ambient_Color: Need light slot") {return(false);}
	int lightslot = atoi(param);

	FAIL_IF((lightslot < 0) && "User error: Set_Light_Ambient_Color: Invalid light slot") {return(false);}
	FAIL_IF((lightslot >= MAX_LIGHTS_PER_SCENE) && "User error: Set_Light_Ambient_Color: Exceeding 4 lights per scene") {return(false);}
	FAIL_IF((Lights[lightslot] == NULL) && "User error: Set_Light_Ambient_Color: Can't change ambient color if light doesn't exist") {return(false);}
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Set_Light_Ambient_Color: Need red") {return(false);}
	float red = atof(param);
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Set_Light_Ambient_Color: Need green") {return(false);}
	float green = atof(param);
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Set_Light_Ambient_Color: Need blue") {return(false);}
	float blue = atof(param);
	Lights[lightslot]->Set_Ambient(Vector3(red, green, blue));
	return(true);
}

















// 0x806670: Lights[slot]'s specular color
bool ScriptedMovieViewClass::Set_Light_Specular_Color(ScriptedMovieParamsClass *params)
{
	const char *param = params->Unknown_7FFDC0();
	FAIL_IF((param == NULL) && "User error: Set_Light_Specular_Color: Need light slot") {return(false);}
	int lightslot = atoi(param);

	FAIL_IF((lightslot < 0) && "User error: Set_Light_Specular_Color: Invalid light slot") {return(false);}
	FAIL_IF((lightslot >= MAX_LIGHTS_PER_SCENE) && "User error: Set_Light_Specular_Color: Exceeding 4 lights per scene") {return(false);}
	FAIL_IF((Lights[lightslot] == NULL) && "User error: Set_Light_Specular_Color: Can't change specular color if light doesn't exist") {return(false);}
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Set_Light_Specular_Color: Need red") {return(false);}
	float red = atof(param);
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Set_Light_Specular_Color: Need green") {return(false);}
	float green = atof(param);
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Set_Light_Specular_Color: Need blue") {return(false);}
	float blue = atof(param);
	Lights[lightslot]->Set_Specular(Vector3(red, green, blue));
	return(true);
}

















// 0x8068A0: Lights[slot]'s diffuse color
bool ScriptedMovieViewClass::Set_Light_Diffuse_Color(ScriptedMovieParamsClass *params)
{
	const char *param = params->Unknown_7FFDC0();
	FAIL_IF((param == NULL) && "User error: Set_Light_Diffuse_Color: Need light slot") {return(false);}
	int lightslot = atoi(param);

	FAIL_IF((lightslot < 0) && "User error: Set_Light_Diffuse_Color: Invalid light slot") {return(false);}
	FAIL_IF((lightslot >= MAX_LIGHTS_PER_SCENE) && "User error: Set_Light_Diffuse_Color: Exceeding 4 lights per scene") {return(false);}
	FAIL_IF((Lights[lightslot] == NULL) && "User error: Set_Light_Diffuse_Color: Can't change diffuse color if light doesn't exist") {return(false);}
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Set_Light_Diffuse_Color: Need red") {return(false);}
	float red = atof(param);
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Set_Light_Diffuse_Color: Need green") {return(false);}
	float green = atof(param);
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Set_Light_Diffuse_Color: Need blue") {return(false);}
	float blue = atof(param);
	Lights[lightslot]->Set_Diffuse(Vector3(red, green, blue));
	return(true);
}















// 0x806AD0
bool ScriptedMovieViewClass::Unknown_806AD0()
{
	Unknown6E = true;
	return(true);
}


















// 0x806AF0
bool ScriptedMovieViewClass::Set_Camera_View_Plane(ScriptedMovieParamsClass *params)
{
	const char *param = params->Unknown_7FFDC0();
	FAIL_IF((param == NULL) && "User error: Set_Camera_View_Plane: Need x view plane value") {return(false);}
	float x = DEG_TO_RADF(atof(param));

	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Set_Camera_View_Plane: Need y view plane value") {return(false);}
	float y = DEG_TO_RADF(atof(param));
	ScriptedCamera->Set_View_Plane(x, y);
	return(true);
}







































































































// 0x807020: the sound of that name stopped
bool ScriptedMovieViewClass::Stop_Audio(ScriptedMovieParamsClass *params)
{
	const char *audio_name = params->Unknown_7FFDC0();
	FAIL_IF((audio_name == NULL) && "User error: Stop_Audio: Need audio name") {return(false);}
	for (std::vector<ScriptedMovieSoundClass *>::iterator i = UnknownF0.begin(); i != UnknownF0.end(); i++) {
		if ((*i)->Unknown_7EDA60(audio_name)) {
			(*i)->Unknown_7ED630(0);
			return(true);
		}
	}
	return(false);
}

































































































// 0x807520: the effects whose end time has come ended and taken off the list
void ScriptedMovieViewClass::Unknown_807520(unsigned int time)
{
	for (std::vector<ScriptedMovieEffectStruct>::iterator itor = Effects.begin(); itor != Effects.end(); ) {
		if ((*itor).EndTime <= time) {
			G_ASSERT(Get_Client());
			EffectObjectClass *effect = Get_Client()->Get_Effect((*itor).ID);
			if (effect) {
				effect->Unknown_6E6E30();
			}
			itor = Effects.erase(itor);
		} else {
			itor++;
		}
	}
}














// NOT EXACT: the effect == NULL return gets its own epilogue; the client's shares the first FAIL_IF's (pop, pop, pop, xor al, al)
// 0x807600: a special effect from the source slot's object to the target slot's (CREATE_SFX_BETWEEN_OBJECTS)
bool ScriptedMovieViewClass::Create_Special_Effect_Between_Objects(ScriptedMovieParamsClass *params)
{
	ClientClass *client = Get_Client();


	G_ASSERT(client);
	const char *param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Create_Special_Effect_Between_Objects: Need source slot") {return(false);}
	int source = atoi(param);
	bool sourceset = false;
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Create_Special_Effect_Between_Objects: Need target slot") {return(false);}
	int target = atoi(param);
	bool targetset = false;
	EffectObjectClass *effect = Unknown_8070C0(client, params);
	if (effect == NULL) {
		return(false);
	}
	UnknownB0009CEffectClass *binary_effect = effect->Unknown_14();

	FAIL_IF(binary_effect == NULL) {return(false);}
	for (std::vector<ScriptedMovieObjectStruct>::iterator itor = Objects.begin(); itor != Objects.end(); itor++) {
		if ((*itor).Slot == source && !sourceset) {


			G_ASSERT((*itor).Mesh);
			if ((*itor).Unknown08 == 0) {
				(*itor).Unknown08 = Unknown_803180((*itor).Mesh->Peek_Object(), 0);
			}
			binary_effect->Unknown_28((*itor).Unknown08);
			sourceset = true;
		}
		if ((*itor).Slot == target && !targetset) {


			G_ASSERT((*itor).Mesh);
			if ((*itor).Unknown08 == 0) {
				(*itor).Unknown08 = Unknown_803180((*itor).Mesh->Peek_Object(), 0);
			}
			binary_effect->Unknown_34((*itor).Unknown08);
			targetset = true;
		}
		if (sourceset && targetset) {
			break;
		}
	}
	client->Add_Effect(effect, (int)Unknown_5C(), (int)ScriptedCamera);





	FAIL_IF(!sourceset || !targetset) {return(false);}
	return(true);
}

















// NOT EXACT: the effect == NULL return gets its own epilogue; the client's shares the first FAIL_IF's (pop, pop, pop, xor al, al)
// 0x8078D0: a special effect from a bone of the source slot's object to one of the target slot's (CREATE_SFX_BETWEEN_BONES)
bool ScriptedMovieViewClass::Create_Special_Effect_Between_Bones(ScriptedMovieParamsClass *params)
{
	ClientClass *client = Get_Client();


	G_ASSERT(client);
	const char *param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Create_Special_Effect_Between_Bones: Need source slot") {return(false);}
	int source = atoi(param);
	bool sourceset = false;
	const char *source_bone = params->Unknown_7FFDC0();


	FAIL_IF((param == NULL) && "User error: Create_Special_Effect_Between_Bones: Need source bone name") {return(false);}
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Create_Special_Effect_Between_Bones: Need target slot") {return(false);}
	int target = atoi(param);
	bool targetset = false;
	const char *target_bone = params->Unknown_7FFDC0();


	FAIL_IF((param == NULL) && "User error: Create_Special_Effect_Between_Bones: Need target bone name") {return(false);}
	EffectObjectClass *effect = Unknown_8070C0(client, params);
	if (effect == NULL) {
		return(false);
	}
	UnknownB0009CEffectClass *binary_effect = effect->Unknown_14();


	FAIL_IF(binary_effect == NULL) {return(false);}
	for (std::vector<ScriptedMovieObjectStruct>::iterator itor = Objects.begin(); itor != Objects.end(); itor++) {
		if ((*itor).Slot == source && !sourceset) {
			int id = Unknown_802D80((*itor).Mesh->Peek_Object(), source_bone);
			binary_effect->Unknown_28(id);
			sourceset = true;
		}
		if ((*itor).Slot == target && !targetset) {
			int id = Unknown_802D80((*itor).Mesh->Peek_Object(), target_bone);
			binary_effect->Unknown_34(id);
			targetset = true;
		}
		if (sourceset && targetset) {
			break;
		}
	}





	FAIL_IF((!sourceset || !targetset) && "User error: Create_Special_Effect_Between_Bones: Can't find source and/or target") {return(false);}
	client->Add_Effect(effect, (int)Unknown_5C(), (int)ScriptedCamera);
	return(true);
}

















// NOT EXACT: the effect == NULL return gets its own epilogue; the client's shares the first FAIL_IF's (pop, pop, pop, xor al, al)
// 0x807BE0: a special effect from the source slot's object to a bone of the target slot's (CREATE_SFX_BETWEEN_OBJECT_AND_BONE)
bool ScriptedMovieViewClass::Create_Special_Effect_Between_Object_And_Bone(ScriptedMovieParamsClass *params)
{
	ClientClass *client = Get_Client();


	G_ASSERT(client);
	const char *param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Create_Special_Effect_Between_Object_And_Bone: Need source slot") {return(false);}
	int source = atoi(param);
	bool sourceset = false;
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Create_Special_Effect_Between_Object_And_Bone: Need target slot") {return(false);}
	int target = atoi(param);
	bool targetset = false;
	const char *target_bone = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Create_Special_Effect_Between_Object_And_Bone: Need target bone name") {return(false);}
	EffectObjectClass *effect = Unknown_8070C0(client, params);
	if (effect == NULL) {
		return(false);
	}
	UnknownB0009CEffectClass *binary_effect = effect->Unknown_14();


	FAIL_IF(binary_effect == NULL) {return(false);}
	for (std::vector<ScriptedMovieObjectStruct>::iterator itor = Objects.begin(); itor != Objects.end(); itor++) {
		if ((*itor).Slot == source && !sourceset) {
			if ((*itor).Unknown08 == 0) {
				(*itor).Unknown08 = Unknown_803180((*itor).Mesh->Peek_Object(), 0);
			}
			binary_effect->Unknown_28((*itor).Unknown08);
			sourceset = true;
		}
		if ((*itor).Slot == target && !targetset) {
			int id = Unknown_802D80((*itor).Mesh->Peek_Object(), target_bone);
			binary_effect->Unknown_34(id);
			targetset = true;
		}
		if (sourceset && targetset) {
			break;
		}
	}





	FAIL_IF((!sourceset || !targetset) && "User error: Create_Special_Effect_Between_Object_And_Bone: Can't find source and/or target") {return(false);}
	client->Add_Effect(effect, (int)Unknown_5C(), (int)ScriptedCamera);
	return(true);
}

















// 0x807EB0: a special effect on a bone of the source slot's object (CREATE_SFX_ON_BONE)
bool ScriptedMovieViewClass::Create_Special_Effect_On_Bone(ScriptedMovieParamsClass *params)
{
	ClientClass *client = Get_Client();


	G_ASSERT(client);
	const char *param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Create_Special_Effect_On_Bone: Need source slot") {return(false);}
	int source = atoi(param);
	const char *bone_name = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Create_Special_Effect_On_Bone: Need bone name for source") {return(false);}
	EffectObjectClass *effect = Unknown_8070C0(client, params);
	if (effect == NULL) {
		return(false);
	}
	UnknownB00060EffectClass *unary_effect = effect->Unknown_10();


	FAIL_IF(unary_effect == NULL) {return(false);}
	for (std::vector<ScriptedMovieObjectStruct>::iterator itor = Objects.begin(); itor != Objects.end(); itor++) {
		if ((*itor).Slot == source) {


			G_ASSERT((*itor).Mesh);
			int id = Unknown_802D80((*itor).Mesh->Peek_Object(), bone_name);
			unary_effect->Unknown_28(id);
			client->Add_Effect(effect, (int)Unknown_5C(), (int)ScriptedCamera);
			return(true);
		}
	}
	return(false);
}

















// 0x808090: a special effect on the source slot's object (CREATE_SFX_ON_OBJECT)
bool ScriptedMovieViewClass::Create_Special_Effect_On_Object(ScriptedMovieParamsClass *params)
{
	ClientClass *client = Get_Client();



	G_ASSERT(client);
	const char *param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Create_Special_Effect_On_Object: Need source slot") {return(false);}
	int source = atoi(param);
	EffectObjectClass *effect = Unknown_8070C0(client, params);
	if (effect == NULL) {
		return(false);
	}
	UnknownB00060EffectClass *unary_effect = effect->Unknown_10();


	FAIL_IF(unary_effect == NULL) {return(false);}
	for (std::vector<ScriptedMovieObjectStruct>::iterator itor = Objects.begin(); itor != Objects.end(); itor++) {
		if ((*itor).Slot == source) {


			G_ASSERT((*itor).Mesh);
			if ((*itor).Unknown08 == 0) {
				(*itor).Unknown08 = Unknown_803180((*itor).Mesh->Peek_Object(), 0);
			}
			unary_effect->Unknown_28((*itor).Unknown08);
			client->Add_Effect(effect, (int)Unknown_5C(), (int)ScriptedCamera);
			return(true);
		}
	}
	return(false);
}




















// 0x808220: the camera shaken by an amount over a time
bool ScriptedMovieViewClass::Shake_Camera(ScriptedMovieParamsClass *params)
{
	const char *param = params->Unknown_7FFDC0();
	FAIL_IF((param == NULL) && "User error: Shake_Camera: Need amount") {return(false);}
	float amount = atof(param);
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Shake_Camera: Need time") {return(false);}
	float time = atof(param);

	G_ASSERT(CameraEffects);
	CameraEffects->Unknown_6E32E0(3, amount, time);
	return(true);
}
















// 0x808330: the scene's ambient light, at once or (with a time) faded from the current one
bool ScriptedMovieViewClass::Set_Ambient_Light(ScriptedMovieParamsClass *params)
{
	FAIL_IF(ScriptedScene == NULL) {return(false);}
	const char *param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Set_Ambient_Light: Need red") {return(false);}
	float red = atof(param);
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Set_Ambient_Light: Need green") {return(false);}
	float green = atof(param);
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Set_Ambient_Light: Need blue") {return(false);}
	float blue = atof(param);
	param = params->Unknown_7FFDC0();
	if (param == NULL) {
		ScriptedScene->Set_Ambient_Light(Vector3(red, green, blue));
		return(true);
	}
	float time = atof(param);
	if (time < 0.0001f) {
		ScriptedScene->Set_Ambient_Light(Vector3(red, green, blue));
		return(true);
	}
	Vector3 current = ScriptedScene->Get_Ambient_Light();
	Unknown88.Unknown_80A730(time, current, Vector3(red, green, blue));
	return(true);
}






















// 0x808590: the scene's fog on, its colour (faded over a time, else at once) and range
bool ScriptedMovieViewClass::Set_Fog(ScriptedMovieParamsClass *params)
{

	FAIL_IF(ScriptedScene == NULL) {return(false);}
	Vector3 color;
	const char *param = params->Unknown_7FFDC0();
	FAIL_IF((param == NULL) && "User error: Set_Fog: Need red") {return(false);}
	color.X = atof(param);
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Set_Fog: Need green") {return(false);}
	color.Y = atof(param);
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Set_Fog: Need blue") {return(false);}
	color.Z = atof(param);
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Set_Fog: Need start") {return(false);}
	float start = atof(param);
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Set_Fog: Need end") {return(false);}
	float end = atof(param);
	ScriptedScene->Set_Fog_Enable(true);
	ScriptedScene->Set_Fog_Range(start, end);
	param = params->Unknown_7FFDC0();
	float time = 0.0f;
	if (param != NULL) {
		time = atof(param);
	}
	Vector3 current = ScriptedScene->Get_Fog_Color();
	UnknownAC.Unknown_80A730(time, current, color);
	return(true);
}






















// 0x808840: the scene's fog off
bool ScriptedMovieViewClass::Unknown_808840()
{
	FAIL_IF(ScriptedScene == NULL) {return(false);}
	ScriptedScene->Set_Fog_Enable(false);
	return(true);
}
















// 0x808880: the screen fade's colour over a time
bool ScriptedMovieViewClass::Set_Screen_Fade_Color(ScriptedMovieParamsClass *params)
{
	const char *param = params->Unknown_7FFDC0();
	FAIL_IF((param == NULL) && "User error: Set_Screen_Fade_Color: Need red") {return(false);}
	float red = atof(param);
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Set_Screen_Fade_Color: Need green") {return(false);}
	float green = atof(param);
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Set_Screen_Fade_Color: Need blue") {return(false);}
	float blue = atof(param);
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Set_Screen_Fade_Color: Need time") {return(false);}
	float time = atof(param);
	Unknown_7FFA30(red, green, blue, time);
	return(true);
}

















// 0x808A00: the screen fade's opacity over a time
bool ScriptedMovieViewClass::Set_Screen_Fade_Opacity(ScriptedMovieParamsClass *params)
{
	const char *param = params->Unknown_7FFDC0();
	FAIL_IF((param == NULL) && "User error: Set_Screen_Fade_Opacity: Need opacity") {return(false);}
	float opacity = atof(param);
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Set_Screen_Fade_Opacity: Need time") {return(false);}
	float time = atof(param);
	Unknown_7FFBE0(opacity, time);
	return(true);
}

















// 0x808AD0
bool ScriptedMovieViewClass::Set_Clip_Plane(ScriptedMovieParamsClass *params)
{
	const char *param = params->Unknown_7FFDC0();
	FAIL_IF((param == NULL) && "User error: Set_Clip_Plane: Need near plane") {return(false);}
	float znear = atof(param);
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Set_Clip_Plane: Need far plane") {return(false);}
	float zfar = atof(param);
	ScriptedCamera->Set_Clip_Planes(znear, zfar);
	return(true);
}

















// 0x808BB0: the letterbox in or out over a time
bool ScriptedMovieViewClass::Enable_Letterbox(ScriptedMovieParamsClass *params, bool on)
{
	const char *param = params->Unknown_7FFDC0();
	FAIL_IF((param == NULL) && "User error: Enable_Letterbox: Need time") {return(false);}
	float time = atof(param);
	Unknown_7FF830(on, time);
	return(true);
}














// 0x808C30: the slot's ship in two colours (0x8020B0, no metal)
bool ScriptedMovieViewClass::Change_Ship_Color(ScriptedMovieParamsClass *params)
{
	const char *param = params->Unknown_7FFDC0();


	FAIL_IF((param == NULL) && "User error: Change_Ship_Color: Need slot number") {return(false);}
	int slot = atoi(param);
	for (std::vector<ScriptedMovieObjectStruct>::iterator itor = Objects.begin(); itor != Objects.end(); itor++) {
		if ((*itor).Slot == slot) {


			G_ASSERT((*itor).Mesh);
			Vector3 primary;
			Vector3 secondary;
			param = params->Unknown_7FFDC0();

			FAIL_IF((param == NULL) && "User error: Change_Ship_Color: Need red") {return(false);}
			primary.X = atof(param);
			param = params->Unknown_7FFDC0();

			FAIL_IF((param == NULL) && "User error: Change_Ship_Color: Need green") {return(false);}
			primary.Y = atof(param);
			param = params->Unknown_7FFDC0();

			FAIL_IF((param == NULL) && "User error: Change_Ship_Color: Need blue") {return(false);}
			primary.Z = atof(param);
			param = params->Unknown_7FFDC0();

			FAIL_IF((param == NULL) && "User error: Change_Ship_Color: Need red") {return(false);}
			secondary.X = atof(param);
			param = params->Unknown_7FFDC0();

			FAIL_IF((param == NULL) && "User error: Change_Ship_Color: Need green") {return(false);}
			secondary.Y = atof(param);
			param = params->Unknown_7FFDC0();

			FAIL_IF((param == NULL) && "User error: Change_Ship_Color: Need blue") {return(false);}
			secondary.Z = atof(param);
			Unknown_8020B0((*itor).Mesh->Unknown_5E26D0(), primary, secondary, -1);
			return(true);
		}
	}
	return(false);
}

















// 0x808F00: the slot's engine trails on or off
bool ScriptedMovieViewClass::Toggle_Engine_Trails(ScriptedMovieParamsClass *params, bool on)
{
	const char *param = params->Unknown_7FFDC0();
	FAIL_IF((param == NULL) && "User error: Toggle_Engine_Trails: Need slot number") {return(false);}
	int slot = atoi(param);
	for (std::vector<ScriptedMovieObjectStruct>::iterator itor = Objects.begin(); itor != Objects.end(); itor++) {
		if ((*itor).Slot == slot) {


			G_ASSERT((*itor).Mesh);
			Unknown_810750((*itor).Mesh->Unknown1C, on);
			return(true);
		}
	}
	return(false);
}












int Unknown_810CF0(RenderObjClass *model, int *levels, int hull_level, ClientClass *client, unsigned id, int unknown);	// 0x810CF0 (shiputils.cpp; co.cpp declares it so)
// the damage levels Turn_On_Damage_Particles takes (its FAIL_IF names the ends; shiputils.cpp's 0x810CF0 gets them)
enum { DAMAGE_PARTICLES_NO_CHANGE = 0, DAMAGE_PARTICLES_3 = 4 };	// NAME 3 [DAMAGE_PARTICLES_NO_CHANGE, DAMAGE_PARTICLES_3]: the FAIL_IF's text (0x808FD0); values from its tests
// 0x808FD0: the slot's damage particles, every hull section or one
bool ScriptedMovieViewClass::Turn_On_Damage_Particles(ScriptedMovieParamsClass *params)
{
	const char *param = params->Unknown_7FFDC0();


	FAIL_IF((param == NULL) && "User error: Turn_On_Damage_Particles: Need slot number") {return(false);}
	int slot = atoi(param);
	for (std::vector<ScriptedMovieObjectStruct>::iterator itor = Objects.begin(); itor != Objects.end(); itor++) {
		if ((*itor).Slot == slot) {


			G_ASSERT((*itor).Mesh);
			param = params->Unknown_7FFDC0();

			FAIL_IF((param == NULL) && "User error: Turn_On_Damage_Particles: Need damage") {return(false);}
			int damage_amount = atoi(param);

			FAIL_IF(((damage_amount < DAMAGE_PARTICLES_NO_CHANGE) || (damage_amount > DAMAGE_PARTICLES_3)) && "User error: Turn_On_Damage_Particles: Damage_Amount is incorrect") {return(false);}
			int levels[4];
			param = params->Unknown_7FFDC0();
			int section;
			if (param != NULL && (section = atoi(param)) >= 0 && section < 4) {
				for (int i = 0; i < 4; i++) {
					levels[i] = (i == section) ? damage_amount : 0;
				}
			} else {
				levels[0] = damage_amount; levels[1] = damage_amount; levels[2] = damage_amount; levels[3] = damage_amount;
			}
			Unknown_810CF0((*itor).Mesh->Unknown1C, levels, levels[0], NULL, (unsigned)-1, 0);
			return(true);
		}
	}
	return(false);
}





























void Unknown_80FF20(RenderObjClass *model, const char *name, float *rgb);	// 0x80FF20 (shiputils.cpp; cpacket.cpp declares it so)

// 0x8091B0: a name decal on the slot's ship, in a colour
bool ScriptedMovieViewClass::Add_Name_Decal(ScriptedMovieParamsClass *params)
{
	const char *param = params->Unknown_7FFDC0();


	FAIL_IF((param == NULL) && "User error: Add_Name_Decal: Need slot number") {return(false);}
	int slot = atoi(param);
	for (std::vector<ScriptedMovieObjectStruct>::iterator itor = Objects.begin(); itor != Objects.end(); itor++) {
		if ((*itor).Slot == slot) {


			G_ASSERT((*itor).Mesh);
			const char *name = params->Unknown_7FFDC0();

			FAIL_IF((name == NULL) && "User error: Add_Name_Decal: Need a name") {return(false);}
			float rgb[3];
			param = params->Unknown_7FFDC0();


			FAIL_IF((param == NULL) && "User error: Add_Name_Decal: Need red") {return(false);}
			rgb[0] = atof(param);
			param = params->Unknown_7FFDC0();

			FAIL_IF((param == NULL) && "User error: Add_Name_Decal: Need green") {return(false);}
			rgb[1] = atof(param);
			param = params->Unknown_7FFDC0();

			FAIL_IF((param == NULL) && "User error: Add_Name_Decal: Need blue") {return(false);}
			rgb[2] = atof(param);
			Unknown_80FF20((*itor).Mesh->Unknown1C, name, rgb);
			return(true);
		}
	}
	return(false);
}



#include "..\packets\pknamedecal.h"
void Unknown_80E8C0(RenderObjClass *model, std::vector<DecalStruct> &decals, SClientUnknown1334Class *decal_ini);	// 0x80E8C0 (shiputils.cpp; cpacket.cpp declares it so)











// 0x8093D0: decal number on the slot's ship, in both of its places (1 and 2), white
bool ScriptedMovieViewClass::Add_Decal_To_Ship(ScriptedMovieParamsClass *params)
{
	const char *param = params->Unknown_7FFDC0();


	FAIL_IF((param == NULL) && "User error: Add_Decal_To_Ship: Need slot number") {return(false);}
	int slot = atoi(param);
	for (std::vector<ScriptedMovieObjectStruct>::iterator itor = Objects.begin(); itor != Objects.end(); itor++) {
		if ((*itor).Slot == slot) {


			G_ASSERT((*itor).Mesh);
			param = params->Unknown_7FFDC0();

			FAIL_IF((param == NULL) && "User error: Add_Decal_To_Ship: Need decal number") {return(false);}
			int number = atoi(param);
			G_ASSERT(DecalMaterialManager);
			std::vector<DecalStruct> decals;
			DecalStruct decal;
			decal.Unknown00 = 1; decal.Unknown04 = number; decal.Unknown08.Set(1.0f, 1.0f, 1.0f, 1.0f);
			decals.push_back(decal);
			decal.Unknown00 = 2; decal.Unknown04 = number; decal.Unknown08.Set(1.0f, 1.0f, 1.0f, 1.0f);
			decals.push_back(decal);
			Unknown_80E8C0((*itor).Mesh->Unknown1C, decals, DecalMaterialManager);
			return(true);
		}
	}
	return(false);
}


















// 0x809600: the slot's ship in a metal (MAKE_SHIP_CHROME 0, MAKE_SHIP_BRONZE 1, MAKE_SHIP_COPPER 2)
bool ScriptedMovieViewClass::Unknown_809600(ScriptedMovieParamsClass *params, int metal)
{
	const char *param = params->Unknown_7FFDC0();


	FAIL_IF((param == NULL) && "User error: Toggle_Engine_Trails: Need slot number") {return(false);}
	int slot = atoi(param);
	for (std::vector<ScriptedMovieObjectStruct>::iterator itor = Objects.begin(); itor != Objects.end(); itor++) {
		if ((*itor).Slot == slot) {


			G_ASSERT((*itor).Mesh);
			Unknown_8020B0((*itor).Mesh->Unknown_5E26D0(), Vector3(0.9f, 0.9f, 0.9f), Vector3(0.9f, 0.9f, 0.9f), metal);
			return(true);
		}
	}
	return(false);
}



































// 0x809710
bool ScriptedMovieViewClass::Unknown_809710()
{
	Unknown7C = false;
	return(true);
}

// 0x809730
bool ScriptedMovieViewClass::Unknown_809730()
{
	Unknown7C = true;
	return(true);
}













// 0x809750: +0x6F, and while it is set an angle in degrees into +0x78
bool ScriptedMovieViewClass::Unknown_809750(bool value, ScriptedMovieParamsClass *params)
{
	Unknown6F = value;
	if (params != NULL) {


		G_ASSERT(value == true);
		const char *param = params->Unknown_7FFDC0();
		if (param != NULL) {
			Unknown78 = DEG_TO_RADF(atof(param));
		}
	}
	return(true);
}





















// 0x8097D0
bool ScriptedMovieViewClass::Skip_Frames(ScriptedMovieParamsClass *params)
{
	if (Unknown171 && ClientInterface->Unknown_00()->Unknown_1C()) {
		const char *param = params->Unknown_7FFDC0();
		FAIL_IF((param == NULL) && "User error: Skip_Frames: Need number of frames") {return(false);}
		Unknown164 += (atoi(param) * 1000) / 30;
	}
	return(true);
}



















// 0x809890: the slot's model hidden or shown, its engine trails the other way
bool ScriptedMovieViewClass::Hide_Object(ScriptedMovieParamsClass *params, bool hide)
{
	const char *param = params->Unknown_7FFDC0();
	FAIL_IF((param == NULL) && "User error: Hide_Object: Need slot number") {return(false);}
	int slot = atoi(param);
	for (std::vector<ScriptedMovieObjectStruct>::iterator itor = Objects.begin(); itor != Objects.end(); itor++) {
		if ((*itor).Slot == slot) {


			G_ASSERT((*itor).Mesh);
			G_ASSERT((*itor).Mesh->Peek_Object());
			(*itor).Mesh->Peek_Object()->Set_Hidden(hide);
			Unknown_810750((*itor).Mesh->Unknown1C, !hide);
			return(true);
		}
	}
	return(false);
}






























// 0x80A570
ScriptedMovieFadeClass::ScriptedMovieFadeClass() :
	Current(0.0f, 0.0f, 0.0f),
	Target(0.0f, 0.0f, 0.0f),
	Rate(0.0f, 0.0f, 0.0f)
{
}

// 0x80A5A0: one step on: time times the rate, each component stopping at its target
void ScriptedMovieFadeClass::Unknown_80A5A0(float time)
{
	if (Rate.X == 0.0f && Rate.Y == 0.0f && Rate.Z == 0.0f) {
		return;
	}
	float step = time * Rate.X;
	if (WWMath::Fabs(step) > WWMath::Fabs(Target.X - Current.X)) {
		Current.X = Target.X;
		Rate.X = 0.0f;
	} else {
		Current.X += step;
	}
	step = time * Rate.Y;
	if (WWMath::Fabs(step) > WWMath::Fabs(Target.Y - Current.Y)) {
		Current.Y = Target.Y;
		Rate.Y = 0.0f;
	} else {
		Current.Y += step;
	}
	step = time * Rate.Z;
	if (WWMath::Fabs(step) > WWMath::Fabs(Target.Z - Current.Z)) {
		Current.Z = Target.Z;
		Rate.Z = 0.0f;
	} else {
		Current.Z += step;
	}
}

// 0x80A730: from to to over time
void ScriptedMovieFadeClass::Unknown_80A730(float time, Vector3 from, Vector3 to)
{
	Current = from;
	Target = to;
	Rate.X = (to.X - from.X) / time;
	Rate.Y = (to.Y - from.Y) / time;
	Rate.Z = (to.Z - from.Z) / time;
}

/*
**	Notes (UI1, 2026-10-29)
**
**	scriptedmovie.cpp is the scripted movie view (ScriptedMovieViewClass, sclient.h: 0x178 bytes over uiview.cpp's
**	UIViewClass; SClient 0x74BD90 makes it), client 0x7FFD30-0x80A730 (appfile.py show app\client\scriptedmovie.cpp;
**	__FILE__ e:\build\beta\G\app\Client\scriptedmovie.cpp, 254 asserts on lines 206-3533). Written so far: a few
**	short members, the asserting ones on the client's lines (D25: 832, 855, 877, 1178); the script commands
**	(0x806AD0 near line 2318's 0x806AF0, 0x809710 / 0x809730 before line 3533's 0x809750) placed by their
**	neighbours. Not rebuilt: the constructor 0x8000C0, the destructor 0x8008B0 (it inlines 0x802D30) and the rest.
*/
