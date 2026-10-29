#include "sclient.h"
#include "..\shared\unknown_debug.h"
#include "..\..\ww3d2\scene.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\assetmgr.h"
#include "..\..\ww3d2\dx8renderer.h"
#include "..\..\ww3d2\dx8wrapper.h"
#include "..\..\ww3d2\camera.h"
#include "..\..\ww3d2\light.h"
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

// one line of the script (std::sort orders them, 0x8099B0)
struct ScriptedMovieLineStruct	// NAME 1: neutral (D10)
{
	unsigned int Frame;	// +0x00  // NAME 2: what the order compares
	char *Text;			// +0x04: the command line  // NAME 2: what it holds
};

// 0x7FFEA0: by frame; within a frame the objects and lights are made first
bool operator < (const ScriptedMovieLineStruct &a, const ScriptedMovieLineStruct &b)
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
// NOT EXACT: the client multiplies by a float in memory (fmul dword, 0xAFDF14: pi / 180); every constant form here
// folds to a double (fmul qword): a const float variable, probably
bool ScriptedMovieViewClass::Set_Camera_View_Plane(ScriptedMovieParamsClass *params)
{
	const char *param = params->Unknown_7FFDC0();
	FAIL_IF((param == NULL) && "User error: Set_Camera_View_Plane: Need x view plane value") {return(false);}
	float x = atof(param) * (float)(WWMATH_PI / 180.0f);

	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Set_Camera_View_Plane: Need y view plane value") {return(false);}
	float y = atof(param) * (float)(WWMATH_PI / 180.0f);
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
