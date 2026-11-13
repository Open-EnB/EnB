// Earth & Beyond client (reconstruction): app\shared\unknown_490900.cpp (PROVISIONAL, D137). Notes at the end of the file.
#include <math.h>
#include "unknown_490900.h"
#include "light.h"
#include "ini.h"
#include "matrix3d.h"
#include "quat.h"
#include "ww3d.h"

float Unknown_B721A0 = 1.0f;	// 0xB721A0: the device width / 640  // NAME 1: neutral (D10)
float Unknown_B721A4 = 1.0f;	// 0xB721A4: the device height / 480  // NAME 1: neutral (D10)
bool Unknown_BE34D0;			// 0xBE34D0: set, the scales stay as they are  // NAME 1: neutral (D10)

// 0x490900
ScriptedMovieViewerSettingsStruct::ScriptedMovieViewerSettingsStruct() :
	Unknown00(NULL),
	BackgroundColor(0.0f, 0.0f, 0.0f),
	Unknown10(0.0f, 0.0f, 0.0f)
{
	Unknown00 = new LightClass;
}

// 0x4909A0
ScriptedMovieViewerSettingsStruct::~ScriptedMovieViewerSettingsStruct()
{
	REF_PTR_RELEASE(Unknown00);
}

// 0x4909D0
// NOT EXACT: stack slots only (the client's frame is 8 bytes larger: its r / g at -0x40 / -0x3C, the quaternion at -0x50); every instruction otherwise matches
bool ScriptedMovieViewerSettingsStruct::Unknown_4909D0(const char *name, const Vector3 &position)
{
	INIClass ini;
	if (!ini.Load(name)) {
		return(false);
	}

	Unknown00->Set_Intensity(1.0f);
	Unknown00->Set_Force_Visible(true);
	Unknown00->Set_Flag(LightClass::FAR_ATTENUATION, false);
	Unknown00->Set_Far_Attenuation_Range(1000000.0, 1000000.0);
	Unknown00->Set_Ambient(Vector3(0.0f, 0.0f, 0.0f));
	Unknown00->Set_Specular(Vector3(1.0f, 1.0f, 1.0f));
	Unknown00->Set_Diffuse(Vector3(1.0f, 1.0f, 1.0f));
	BackgroundColor.Set(0.0f, 0.0f, 0.0f);
	Unknown10.Set(0.33f, 0.33f, 0.33f);
	Unknown00->Set_Position(Vector3(0.0f, 5000.0f, 3000.0f));

	if (ini.Is_Present("Settings", "AmbientLightR") && ini.Is_Present("Settings", "AmbientLightG") && ini.Is_Present("Settings", "AmbientLightB")) {
		float r = ini.Get_Float("Settings", "AmbientLightR", 0.0f);
		float g = ini.Get_Float("Settings", "AmbientLightG", 0.0f);
		float b = ini.Get_Float("Settings", "AmbientLightB", 0.0f);
		Unknown10.Set(r, g, b);
	}

	if (ini.Is_Present("Settings", "SceneLightR") && ini.Is_Present("Settings", "SceneLightG") && ini.Is_Present("Settings", "SceneLightB")) {
		float r = ini.Get_Float("Settings", "SceneLightR", 0.0f);
		float g = ini.Get_Float("Settings", "SceneLightG", 0.0f);
		float b = ini.Get_Float("Settings", "SceneLightB", 0.0f);
		Unknown00->Set_Diffuse(Vector3(r, g, b));
		Unknown00->Set_Specular(Vector3(r, g, b));
	}

	if (ini.Is_Present("Settings", "SceneLightX") && ini.Is_Present("Settings", "SceneLightY") && ini.Is_Present("Settings", "SceneLightZ") && ini.Is_Present("Settings", "SceneLightW")) {
		Quaternion q;
		q.X = ini.Get_Float("Settings", "SceneLightX", 0.0f);
		q.Y = ini.Get_Float("Settings", "SceneLightY", 0.0f);
		q.Z = ini.Get_Float("Settings", "SceneLightZ", 0.0f);
		q.W = ini.Get_Float("Settings", "SceneLightW", 0.0f);
		Vector3 pos = position;
		Vector3 light_pos = Unknown00->Get_Position();
		float distance = (light_pos - pos).Length();
		Matrix3D tm(true);
		tm.Set_Translation(pos);
		Matrix3D::Multiply(tm, Build_Matrix3D(q), &tm);
		tm.Translate_Z(distance);
		Unknown00->Set_Transform(tm);
	}

	if (ini.Is_Present("Settings", "SceneLightDistance") && ini.Is_Present("Settings", "SceneLightIntensity") && ini.Is_Present("Settings", "SceneLightAttenStart") && ini.Is_Present("Settings", "SceneLightAttenEnd") && ini.Is_Present("Settings", "SceneLightAttenOn")) {
		float distance = ini.Get_Float("Settings", "SceneLightDistance", 0.0f);
		float intensity = ini.Get_Float("Settings", "SceneLightIntensity", 0.0f);
		float start = ini.Get_Float("Settings", "SceneLightAttenStart", 0.0f);
		float end = ini.Get_Float("Settings", "SceneLightAttenEnd", 0.0f);
		int on = ini.Get_Int("Settings", "SceneLightAttenOn", 0);
		Unknown00->Set_Intensity(intensity);
		Unknown00->Set_Far_Attenuation_Range(start, end);
		Unknown00->Set_Flag(LightClass::FAR_ATTENUATION, on == 1);
		Vector3 pos = position;
		Vector3 dir = Unknown00->Get_Position() - pos;
		dir.Normalize();
		Unknown00->Set_Position(dir * distance);
	}

	if (ini.Is_Present("Settings", "BackgroundR") && ini.Is_Present("Settings", "BackgroundG") && ini.Is_Present("Settings", "BackgroundB")) {
		float r = ini.Get_Float("Settings", "BackgroundR", 0.0f);
		float g = ini.Get_Float("Settings", "BackgroundG", 0.0f);
		float b = ini.Get_Float("Settings", "BackgroundB", 0.0f);
		BackgroundColor.Set(r, g, b);
	}
	return(true);
}

// 0x4914C0
void Unknown_4914C0()
{
	int width;
	int height;
	int bits;
	bool windowed;

	WW3D::Get_Device_Resolution(width, height, bits, windowed);
	if ((float)width > 0.0f && (float)height > 0.0f) {
		Unknown_B721A0 = (float)width / 640.0f;
		Unknown_B721A4 = (float)height / 480.0f;
	}
}

// 0x491550
void Unknown_491550(float *x, float *y)
{
	if (!Unknown_BE34D0) {
		Unknown_4914C0();
	}
	*x = Unknown_B721A0 * *x;
	*y = Unknown_B721A4 * *y;
	*x = (float)floor(*x);
	*y = (float)floor(*y);
	*x = *x / Unknown_B721A0;
	*y = *y / Unknown_B721A4;
}

/*
**	Reconstruction notes
**
**	The object runs from enbassetmgr.cpp's Recolor (0x4904B0) to proj_man.cpp's guarded MIN_STATIC_SHADOW_COS_HALF_THETA
**	initialiser (0x491660, cos at 0xBE34D4); it has no <string> guard, no __FILE__ and no assert. Its data: the 'Settings'
**	key names (0xB72048-0xB7219C) and the two scales (0xB721A0 / 0xB721A4, 1.0), then the flag 0xBE34D0 (bss). The INI
**	loader takes the light's defaults first (intensity 1, forced visible, far attenuation off at 1000000, black ambient,
**	white diffuse / specular, at (0, 5000, 3000)), then each key group only when all its keys are present (INIClass's
**	Is_Present inline: the client keeps its 'entry == 0' test). SceneLightX..W is a rotation about the given position,
**	keeping the light's distance; SceneLightDistance moves the light along its direction from the position. The device
**	scale 0x4914C0 (no caller) is inlined into 0x491550, which snaps a point to whole device pixels.
**	PLACEMENT: provisional (D137): a neutral name between enbassetmgr.cpp and proj_man.cpp (the original name sorts there).
*/
