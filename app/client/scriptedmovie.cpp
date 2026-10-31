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
#pragma auto_inline(off)
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
#pragma auto_inline(on)
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

#include "co.h"
#include "..\..\ww3d2\render2d.h"
#include "font3d.h"
#include "..\audio\soundlib.h"
#include "ini.h"
// the members' classes as far as the constructor builds them (sclient.cpp defines them so)
class Unknown67D360Class { public: ~Unknown67D360Class(); char Unknown00[0x14]; };	// 0x67D360  // NAME 1: neutral (D10)
class Unknown619280Class { public: Unknown619280Class(); ~Unknown619280Class(); char Unknown00[0x44]; };	// 0x619280  // NAME 1: neutral (D10)
// env_map_manager.cpp's object at the view's +0x140 (sclient.cpp's SClientUnknown1338Class), as far as Create_Skybox calls it
class SClientUnknown1338Class { public: SClientUnknown1338Class() : Unknown10(2) {} ~SClientUnknown1338Class(); void Load(const char *ini); void Unknown_760770(int base_asset, bool a); std::vector<Unknown67D360Class> Unknown00; int Unknown10; INIClass INI; };	// destructor 0x679820, Load 0x760360, 0x760770  // NAME 1 [base_asset, a]: sclient.cpp's
// the skybox's interface (0x8C bytes, constructor 0x53AA30; its file unknown), as far as Create_Skybox calls it
class Unknown53AA30InterfaceClass : public InterfaceClass	// NAME 1: neutral (D10)
{
public:
	Unknown53AA30InterfaceClass(UIViewClass *view, SimpleSceneClass *scene);	// 0x53AA30  // NAME 1 [view, scene]: what Create_Skybox passes
	virtual void Unknown_88(RenderObjClass *model);	// +0x88 (Create_Skybox: the skybox model)  // NAME 1 [model]: neutral
	char Unknown84[0x8C - 0x84];						// +0x84
};
// decal.cpp's decal INIs at the view's +0x144 (sclient.h's SClientUnknown1334Class), as far as the shutdown deletes it
class SClientUnknown1334Class { public: ~SClientUnknown1334Class(); void Load(const char *textures, const char *swatches, const char *secret_textures); std::vector<int> Unknown00; std::vector<int> Unknown10; std::vector<int> Unknown20; Unknown619280Class Unknown30; Unknown619280Class Unknown74; };	// 0x618A50, Load 0x75BE00 (decal.cpp)  // NAME 3 [textures, swatches, secret_textures]: the INI names (sclient.cpp's)
// ViewerSettings' first member (its slot +0x38: the shutdown)
class ScriptedMovieUnknown68Class { public: virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C(); virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C(); virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C(); virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); };	// NAME 1: neutral (D10)
struct ScriptedMovieViewerSettingsStruct { ScriptedMovieViewerSettingsStruct(); ~ScriptedMovieViewerSettingsStruct(); ScriptedMovieUnknown68Class *Unknown00; Vector3 BackgroundColor; char Unknown10[0x1C - 0x10]; };	// what +0x68 points to (0x1C bytes, constructor 0x490900, destructor 0x4909A0; the render 0x8012C0 clears to +0x04)  // NAME 1: neutral (D10); NAME 2 [BackgroundColor]: what Begin_Render gets
void Unknown_7FF230();	// 0x7FF230 (the file before this one: the screen fade and the letterbox), the constructor calls it
extern bool Unknown_BB9654;	// NAME 1 [Unknown_BB9654]: neutral (D10) (tsk_login.cpp's); the constructor clears it, the destructor sets it
extern int Unknown_C01290;	// NAME 1: neutral (D10): 300000 while the scripted movie view is up (the constructor), else 0 (the destructor)
extern RectClass Unknown_C07550;	// NAME 1: neutral (D10) (sclient.cpp's): the text's coordinate range
#define EnvironmentMapManager Unknown1338	// NAME 3 [EnvironmentMapManager]: the assert's 'sclient->EnvironmentMapManager' (the constructor, l.231)

// 0x8000C0: the view: its scene, camera, environment maps and decals (SClient's, or its own without one), the script
ScriptedMovieViewClass::ScriptedMovieViewClass(ClientTaskClass *task, int arg, SClientMovieCallbackClass *callback) :
	UIViewClass(task, 6),
	ClientInterface((ScriptedMovieInterfaceClass *)callback),
	ScriptedCamera(NULL),
	ScriptedScene(NULL),
	ViewerSettings(new ScriptedMovieViewerSettingsStruct),
	Unknown6C(false),
	Unknown6D(false),
	Unknown6E(false),
	Unknown6F(false),
	Unknown70(-1.0f),
	Unknown74(-1.0f),
	Unknown78(1.0f),
	Unknown7C(true),
	Unknown80(0),
	CurrentCameraIndex(0),
	BackgroundScene(NULL),
	BackInterface(NULL),
	StarsScene(NULL),
	OrbitalManager(NULL),
	OrbitalScene(NULL),
	Unknown138(-1),
	BackgroundDazzleLayer(NULL),
	Unknown140(NULL),
	DecalMaterialManager(NULL),
	Unknown158(false),
	Unknown15C(NULL),
	Unknown164(0),
	Unknown168(false),
	Unknown16C(0),
	Unknown170(false),
	Unknown171(false),
	Unknown172(false),
	Unknown174(NULL)
{













	G_ASSERT(ClientInterface);
	CameraEffects = new CameraEffectsClass((MainViewClass *)this, (SClient *)Get_Client());
	ScriptedScene = new SimpleSceneClass;
	ScriptedScene->Set_Ambient_Light(Vector3(0.0f, 0.0f, 0.0f));
	ScriptedCamera = new CameraClass;
	ScriptedCamera->Set_Clip_Planes(20.0f, 100000.0f);
	ScriptedCamera->Set_View_Plane(DEG_TO_RADF(56.0f), DEG_TO_RADF(41.0f));
	ScriptedCamera->Set_Viewport(Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f));
	for (int i = 0; i < MAX_LIGHTS_PER_SCENE; i++) Lights[i] = NULL;
	UIViewClass::Unknown_04();
	Unknown_7FF230();
	SClient *sclient = (SClient *)ClientInterface->Unknown_00()->Unknown_18();
	if (sclient != NULL) {












		G_ASSERT(sclient->EnvironmentMapManager);
		Unknown140 = sclient->EnvironmentMapManager;
		DecalMaterialManager = sclient->Unknown1334;
	} else {
		Unknown140 = new SClientUnknown1338Class;
		Unknown140->Load("envmaps.ini");
		DecalMaterialManager = new SClientUnknown1334Class;
		DecalMaterialManager->Load("decal_textures.ini", "decal_swatches.ini", "secret_decal_textures.ini");
	}
	Unknown_BB9654 = false;
	Unknown_C01290 = 300000;
	Unknown_8099B0((const char *)arg);
	if (Unknown16C != 0) {
		Unknown_7FFA30(0.0f, 0.0f, 0.0f, 0.0f);
		Unknown_7FFBE0(1.0f, 0.0f);
	}
	Unknown160 = Create_NEW(new Font3DInstanceClass("font12x16.tga"));
	Unknown15C = new Render2DTextClass(Unknown160.Peek());
	Unknown15C->Set_Coordinate_Range(Unknown_C07550);
	Unknown15C->Enable_Alpha(true);
	Unknown15C->Reset();
	SoundLibraryClass::_PlayUndefined = true;
}




























































































































































// 0x8010A0: the view's shutdown (slot +0x10): its scenes, camera, stars, orbitals and skybox released; the client keeps
void ScriptedMovieViewClass::Unknown_10()
{
	ClientClass *client = Get_Client();
	G_ASSERT(client);
	ViewerSettings->Unknown00->Unknown_38();
	REF_PTR_RELEASE(ScriptedScene);
	REF_PTR_RELEASE(ScriptedCamera);
	REF_PTR_RELEASE(Stars);
	REF_PTR_RELEASE(StarsScene);
	REF_PTR_RELEASE(BackgroundScene);
	REF_PTR_RELEASE(BackInterface);
	REF_PTR_RELEASE(OrbitalScene);
	if (OrbitalManager) {
		delete OrbitalManager;
		OrbitalManager = NULL;
	}
	if (BackgroundDazzleLayer) {
		delete BackgroundDazzleLayer;
		BackgroundDazzleLayer = NULL;
	}
	SClient *sclient = (SClient *)client->Unknown_18();
	if (sclient) {
		sclient->Unknown_745050();
	} else {
		delete Unknown140;
		delete DecalMaterialManager;
	}
	Unknown140 = NULL;
	DecalMaterialManager = NULL;
	UIViewClass::Unknown_10();
}










#include "..\..\ww3d2\predlod.h"
#include "..\..\ww3d2\render2d.h"
#include "..\..\ww3d2\statistics.h"
extern unsigned int Unknown_BFAE3C; extern unsigned int Unknown_BFAE40;	// (declared again with the frame 0x801E60)
void Unknown_7FF810();					// 0x7FF810 (the file before this one: the screen fade and the letterbox), their render

// 0x8012C0: the view's render (slot +0x0C): the background, the stars, the orbitals and the dazzles, the scene, the fade;
// with +0x171 and +0x158 the frame, the frame rate and the polygon / vertex counts as text
// NOT EXACT: shape (0.83): the client reads the background colour as three dwords and the inlined 0x801D70 copies zfar as an integer before znear
int ScriptedMovieViewClass::Unknown_0C(bool on)
{





	G_ASSERT(ViewerSettings);
	G_ASSERT(ScriptedScene);
	int result = WW3D::Begin_Render(on, true, Vector3(ViewerSettings->BackgroundColor));
	Unknown_801860();
	Unknown_8019A0();
	if (OrbitalScene) {
		DazzleRenderObjClass::Set_Current_Dazzle_Layer(BackgroundDazzleLayer);
		float znear, zfar;
		Unknown_801D70(&znear, &zfar);
		Unknown_801AE0(znear, zfar);
		Unknown_801C40(znear, zfar);
	}
	PredictiveLODOptimizerClass::Clear();
	ScriptedScene->Visibility_Check(ScriptedCamera);
	PredictiveLODOptimizerClass::Optimize_LODs(50000.0f);
	WW3D::Render(ScriptedScene, ScriptedCamera);
	Unknown_7FF810();
	if (Unknown171 && Unknown158) {
		unsigned int frame = (Unknown80 * 30 - Unknown16C * 30) / 1000;
		StringClass line(255, true);
		StringClass text(1024, true);
		unsigned int ms = Unknown_BFAE3C - Unknown_BFAE40;
		if (ms <= 1) {
			ms = 1;
		}
		float rate = 1.0f / (ms * 0.001f);
		line.Format("Frame: %d  Frame Rate: %f\n", frame, rate);
		text += line;
		line.Format("%d Polys/frame (%dk pps)\n", Debug_Statistics::Get_DX8_Polygons(), (unsigned int)(Debug_Statistics::Get_DX8_Polygons() * rate) / 1000);
		text += line;
		line.Format("%d Verts/frame (%dk vps)\n", Debug_Statistics::Get_DX8_Vertices(), (unsigned int)(Debug_Statistics::Get_DX8_Vertices() * rate) / 1000);
		text += line;
		Unknown15C->Draw_Text(text, 0xFFFF0000);
		Unknown15C->Render();
		Unknown15C->Reset();
	}
	WW3D::End_Render();
	return(result);
}





































// 0x801860: the background scene through the scripted camera at its own depth range and clip planes, then the camera's back
void ScriptedMovieViewClass::Unknown_801860()
{
	if (BackgroundScene) {
		G_ASSERT(ScriptedCamera);
		float znear;
		float zfar;
		ScriptedCamera->Get_Clip_Planes(znear, zfar);
		float zmin;
		float zmax;
		ScriptedCamera->Get_Depth_Range(&zmin, &zmax);
		ScriptedCamera->Set_Depth_Range(Unknown_B07C44, Unknown_B07C40);
		ScriptedCamera->Set_Clip_Planes(Unknown_B07C4C, Unknown_B07C48);
		WW3D::Render(BackgroundScene, ScriptedCamera);
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



#include "..\audio\soundlib.h"
extern unsigned int Unknown_BFAE3C;	// NAME 1 [Unknown_BFAE3C]: neutral (D10): the time now in ms (mainview.cpp, ui_line_display.cpp declare it so)
extern unsigned int Unknown_BFAE40;	// NAME 1 [Unknown_BFAE40]: neutral (D10): the time of the last frame (ui_line_display.cpp declares it so)
void Unknown_7FF3D0();					// 0x7FF3D0 (the file before this one: the screen fade and the letterbox), their frame




















// 0x801E60: the view's frame (slot +0x14): the fade-out at the end time, the script, the objects, the sounds, the fades, the orbitals
void ScriptedMovieViewClass::Unknown_14()
{
	unsigned int frame_time = Unknown_BFAE3C - Unknown_BFAE40;
	if (Unknown16C && !Unknown170 && Unknown80 >= Unknown16C) {
		Unknown_7FFBE0(0.0f, 1.0f);
		Unknown170 = true;
	}
	Unknown_802B20();
	if (!Unknown172) {
		if (Unknown174) {
			SoundLibraryClass::Play_2D_Sound(Unknown174, 2);
			delete Unknown174;
			Unknown174 = NULL;
		}
		Unknown172 = true;
		return;
	}
	for (std::vector<ScriptedMovieObjectStruct>::iterator itor = Objects.begin(); itor != Objects.end(); itor++) {
		FAIL_IF((*itor).Mesh == NULL) {return;}
		(*itor).Mesh->Unknown_5B9410();
	}
	UIViewClass::Unknown_14();
	Unknown_802420();
	CameraEffects->Unknown_34();
	Unknown_802AD0(frame_time);
	Unknown_7FF3D0();
	float time = frame_time * 0.001f;
	Unknown_8022C0(time);
	Unknown_802330(time);
	if (OrbitalManager) {
		OrbitalManager->Update(ScriptedCamera, time * (1.0f / 3600.0f), true);
	}
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
















void Unknown_42ECF0(float *x, float *y);	// 0x42ECF0 (cdecl: slot +0x10 of the object at 0xBDAA74): the pointer's position, 0..1  // NAME 1 [x, y]: neutral

// 0x802420: the camera's transform: turned to look down its bone's axis unless +0x7C, then (with +0x6F) by the pointer
void ScriptedMovieViewClass::Unknown_802420()
{
	FAIL_IF(ScriptedCamera == NULL) {return;}
	Matrix3D tm = ScriptedCamera->Get_Transform();
	if (!Unknown7C) {
		tm.Rotate_Y(-WWMATH_PI / 2.0f);
		tm.Rotate_Z(-WWMATH_PI / 2.0f);
	}
	if (Unknown6F) {
		float x, y;
		Unknown_42ECF0(&x, &y);
		if (fabs(Unknown70 - x) > 0.0001 || fabs(Unknown74 - y) > 0.0001) {
			Unknown70 = x;
			Unknown74 = y;
		}
		float yaw, pitch;
		if (Unknown7C) {
			yaw = -((Unknown70 - 0.5f) * Unknown78);
			pitch = -((Unknown74 - 0.5f) * Unknown78);
		} else {
			yaw = (Unknown70 - 0.5f) * -1.0f;
			pitch = (Unknown74 - 0.5f) * -1.0f;
		}
		tm.Rotate_Y(yaw);
		tm.Rotate_X(pitch);
	}
	ScriptedCamera->Set_Transform(tm);
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













// 0x802D80: the client object id of robj's bone: the one BoneClientObjectList holds, else a new one (0x803180) added to it
int ScriptedMovieViewClass::Unknown_802D80(RenderObjClass *robj, const char *bonename)
{



	G_ASSERT(robj);
	G_ASSERT(bonename);
	int boneindex = robj->Get_Bone_Index(bonename);


	G_ASSERT(boneindex >= 0);
	for (unsigned i = 0; i < BoneClientObjectList.size(); i++) {
		if (BoneClientObjectList[i].RenderObject == robj && BoneClientObjectList[i].Bone == boneindex) {


			G_ASSERT(BoneClientObjectList[i].ClientID < 0);
			return(BoneClientObjectList[i].ClientID);
		}
	}
	int id = Unknown_803180(robj, bonename);
	robj->Add_Ref();
	ScriptedMovieBoneObjectStruct entry;
	entry.RenderObject = robj;
	entry.Bone = boneindex;
	entry.ClientID = id;
	BoneClientObjectList.push_back(entry);
	return(id);
}
















#include "postate.h"
// 0x803180: a client object (type 0x29, the client's next negative id) for robj, or for one of its bones; its id
int ScriptedMovieViewClass::Unknown_803180(RenderObjClass *robj, const char *bone_name)
{
	ClientClass *client = Get_Client();




	G_ASSERT(client);
	unsigned id = --client->Unknown113C;

	G_ASSERT(robj);
	ClientObjectClass *object = new ClientObjectClass(client, 0x29, id);
	object->Unknown98 = new Unknown7192A0Class(robj, object->Unknown98, bone_name);
	object->Unknown_6F33F0(robj, -1, 1.0f);
	if (bone_name) {
		object->Unknown_34(100.0f);
	} else {
		SphereClass sphere = robj->Get_Bounding_Sphere();
		object->Unknown_34(sphere.Radius);
	}
	client->ClientClass::Add_Object(object);
	return(id);
}










































































// NOT EXACT: inline budget (0.63): the client inlines Shake_Camera and both Toggle_Engine_Trails too; ours calls them (0x7FFD30 kept out of line by the pragmas)
// 0x803310: one script line run: its command (the line's first field, any case) to the member that takes the rest
bool ScriptedMovieViewClass::Unknown_803310(ScriptedMovieLineClass *line)
{
	ScriptedMovieParamsClass *params = (ScriptedMovieParamsClass *)line;	// (the same 12 bytes: the frame, the text, the cursor)
	if (params->Unknown_7FFD30("CREATE_OBJECT")) return(Create_Object(params, false));
	if (params->Unknown_7FFD30("CREATE_SHIP_OBJECT")) return(Create_Object(params, true));
	if (params->Unknown_7FFD30("CREATE_SFX_ON_OBJECT")) return(Create_Special_Effect_On_Object(params));
	if (params->Unknown_7FFD30("CREATE_SFX_BETWEEN_OBJECTS")) return(Create_Special_Effect_Between_Objects(params));
	if (params->Unknown_7FFD30("CREATE_SFX_BETWEEN_BONES")) return(Create_Special_Effect_Between_Bones(params));
	if (params->Unknown_7FFD30("CREATE_SFX_BETWEEN_OBJECT_AND_BONE")) return(Create_Special_Effect_Between_Object_And_Bone(params));
	if (params->Unknown_7FFD30("CREATE_SFX_ON_BONE")) return(Create_Special_Effect_On_Bone(params));
	if (params->Unknown_7FFD30("CREATE_STARS")) return(Unknown_804A70(params));
	if (params->Unknown_7FFD30("CREATE_SKYBOX")) return(Create_Skybox(params));
	if (params->Unknown_7FFD30("CREATE_ORBITAL")) return(Create_Orbital(params));
	if (params->Unknown_7FFD30("DESTROY_OBJECT")) return(Destroy_Object(params));
	if (params->Unknown_7FFD30("PLAY_ANIMATION")) return(Play_Animation(params));
	if (params->Unknown_7FFD30("LOAD_ANIMATION")) return(Unknown_8051B0(params));
	if (params->Unknown_7FFD30("ATTACH_CAMERA")) return(Attach_Camera(params));
	if (params->Unknown_7FFD30("ADD_CAMERA")) return(Add_Camera(params));
	if (params->Unknown_7FFD30("ATTACH_TO_BONE")) return(Attach_To_Bone(params));
	if (params->Unknown_7FFD30("ATTACH_LIGHT_TO_BONE")) return(Attach_Light_To_Bone(params));
	if (params->Unknown_7FFD30("SET_AMBIENT_LIGHT")) return(Set_Ambient_Light(params));
	if (params->Unknown_7FFD30("CREATE_POINT_LIGHT")) return(Create_Light(params, 0, false));
	if (params->Unknown_7FFD30("CREATE_DIRECTIONAL_LIGHT_FROM_BONE")) return(Create_Light(params, 1, true));
	if (params->Unknown_7FFD30("CREATE_DIRECTIONAL_LIGHT")) return(Create_Light(params, 1, false));
	if (params->Unknown_7FFD30("SET_LIGHT_INTENSITY")) return(Set_Light_Intensity(params));
	if (params->Unknown_7FFD30("SET_LIGHT_FAR_ATTENUATION") || params->Unknown_7FFD30("SET_LIGHT_ATTENUATION")) return(Set_Light_Far_Attenuation(params));
	if (params->Unknown_7FFD30("SET_LIGHT_AMBIENT_COLOR")) return(Set_Light_Ambient_Color(params));
	if (params->Unknown_7FFD30("SET_LIGHT_SPECULAR_COLOR")) return(Set_Light_Specular_Color(params));
	if (params->Unknown_7FFD30("SET_LIGHT_DIFFUSE_COLOR")) return(Set_Light_Diffuse_Color(params));
	if (params->Unknown_7FFD30("SET_CAMERA_VIEW_PLANE")) return(Set_Camera_View_Plane(params));
	if (params->Unknown_7FFD30("SHAKE_CAMERA")) return(Shake_Camera(params));
	if (params->Unknown_7FFD30("SET_SCREEN_FADE_COLOR")) return(Set_Screen_Fade_Color(params));
	if (params->Unknown_7FFD30("SET_SCREEN_FADE_OPACITY")) return(Set_Screen_Fade_Opacity(params));
	if (params->Unknown_7FFD30("PLAY_AUDIO")) return(Play_Audio(params));
	if (params->Unknown_7FFD30("TURN_ON_MAX_CAMERA")) return(Unknown_809730());
	if (params->Unknown_7FFD30("TURN_OFF_MAX_CAMERA")) return(Unknown_809710());
	if (params->Unknown_7FFD30("ENABLE_LETTERBOX")) return(Enable_Letterbox(params, true));
	if (params->Unknown_7FFD30("DISABLE_LETTERBOX")) return(Enable_Letterbox(params, false));
	if (params->Unknown_7FFD30("SET_CLIP_PLANE")) return(Set_Clip_Plane(params));
	if (params->Unknown_7FFD30("TURN_ON_FOG")) return(Set_Fog(params));
	if (params->Unknown_7FFD30("TURN_OFF_FOG")) return(Unknown_808840());
	if (params->Unknown_7FFD30("STOP_AUDIO")) return(Stop_Audio(params));
	if (params->Unknown_7FFD30("TURN_ON_MOUSE_MOVEMENT_OF_CAMERA")) return(Unknown_809750(true, params));
	if (params->Unknown_7FFD30("TURN_OFF_MOUSE_MOVEMENT_OF_CAMERA")) return(Unknown_809750(false, NULL));
	if (params->Unknown_7FFD30("TURN_ON_ENGINE_TRAILS")) return(Toggle_Engine_Trails(params, true));
	if (params->Unknown_7FFD30("TURN_OFF_ENGINE_TRAILS")) return(Toggle_Engine_Trails(params, false));
	if (params->Unknown_7FFD30("TURN_ON_DAMAGE_PARTICLES")) return(Turn_On_Damage_Particles(params));
	if (params->Unknown_7FFD30("CHANGE_SHIP_COLOR")) return(Change_Ship_Color(params));
	if (params->Unknown_7FFD30("MAKE_SHIP_BRONZE")) return(Unknown_809600(params, 1));
	if (params->Unknown_7FFD30("MAKE_SHIP_COPPER")) return(Unknown_809600(params, 2));
	if (params->Unknown_7FFD30("MAKE_SHIP_CHROME")) return(Unknown_809600(params, 0));
	if (params->Unknown_7FFD30("ADD_NAME_DECAL")) return(Add_Name_Decal(params));
	if (params->Unknown_7FFD30("ADD_DECAL")) return(Add_Decal_To_Ship(params));
	if (params->Unknown_7FFD30("END_MOVIE")) return(Unknown_806AD0());
	if (params->Unknown_7FFD30("SHOW_CURRENT_FRAME")) {Unknown158 = true; return(true);}
	if (params->Unknown_7FFD30("DONT_SHOW_CURRENT_FRAME")) {Unknown158 = false; return(true);}
	if (params->Unknown_7FFD30("SKIP_FRAMES")) return(Skip_Frames(params));
	if (params->Unknown_7FFD30("HIDE_OBJECT")) return(Hide_Object(params, true));
	if (params->Unknown_7FFD30("UNHIDE_OBJECT")) return(Hide_Object(params, false));
	if (params->Unknown_7FFD30("DEBUG_MODE")) {Unknown171 = true; return(true);}
	MessageBox(NULL, line->Text, "Script Error", MB_OK);













	G_ASSERT(!"User error: Unrecognized command");
	return(false);
}









extern bool Unknown_BFAE52;	// 0xBFAE52: set while Create_Object makes a mesh  // NAME 1 [Unknown_BFAE52]: neutral (D10)

// NOT EXACT: register / inline budget (0.58): the push_back helpers and registers differ from the client (ebx / ebp swapped, two calls fewer)
// 0x804290: an object of a slot: ui_mesh.cpp's mesh of that name (a ship's with the client) in the scene, shown at its lowest LOD
bool ScriptedMovieViewClass::Create_Object(ScriptedMovieParamsClass *params, bool ship)
{
	const char *param = params->Unknown_7FFDC0();




	FAIL_IF((param == NULL) && "User error: Create_Object: Need slot number") {return(false);}
	int slot = atoi(param);
	ScriptedMovieObjectStruct object;
	param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Create_Object: Need mesh name") {return(false);}
	Unknown_BFAE52 = true;
	UIMeshClass *mesh;
	if (ship) {
		mesh = new UIMeshClass(param, Get_Client());
	} else {
		mesh = new UIMeshClass(param, true);
	}
	Unknown_BFAE52 = false;
	mesh->Unknown_5B9290(ScriptedScene);
	mesh->Unknown70 |= 3;








	G_ASSERT(mesh->Peek_Mesh_Object());
	mesh->Peek_Mesh_Object()->Set_Force_Visible(true);
	RenderObjClass *model = mesh->Peek_Mesh_Object();
	model->Set_LOD_Level(model->Get_LOD_Count() - 1);
	object.Mesh = mesh;
	object.Slot = slot;
	Objects.push_back(object);
	return(true);
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






















// 0x804800: the skybox: the background scene, its interface and the base asset's model (CREATE_SKYBOX; 0x135 by default)
bool ScriptedMovieViewClass::Create_Skybox(ScriptedMovieParamsClass *params)
{
	int skybox = 0x135;
	const char *param = params->Unknown_7FFDC0();
	if (param != NULL) {
		skybox = atoi(param);
	}
	G_ASSERT(BackgroundScene == NULL);
	G_ASSERT(BackInterface == NULL);
	BackgroundScene = new SimpleSceneClass;

	G_ASSERT(BackgroundScene);
	BackgroundScene->Set_Fog_Enable(false);
	BackInterface = new Unknown53AA30InterfaceClass(this, BackgroundScene);


	G_ASSERT(BackInterface);
	BackInterface->Unknown_54();
	BackInterface->Unknown_58();
	Unknown_6B4940(BackInterface);

	G_ASSERT(ClientInterface);
	ClientClass *client = Get_Client();
	float hsv[3] = { 0.0f, 0.0f, 0.0f };
	RenderObjClass *model = ClientObjectClass::Unknown_6F5140(skybox, client, 1.0f, hsv);
	BackInterface->Unknown_88(model);
	model->Release_Ref();
	Unknown140->Unknown_760770(skybox, true);
	return(true);
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






// the scripted movie's orbital callback (8 bytes, table 0xB07CFC), laid out as mainview.cpp's MainViewOrbitalCallbackClass
// (OrbitalManager takes that type: Create_Orbital casts to it)
class ScriptedMovieOrbitalCallbackClass	// NAME 1: neutral (D10)
{
public:
	ScriptedMovieOrbitalCallbackClass(ScriptedMovieViewClass *view) : View(view) {}	// NAME 1 [view]: neutral
	virtual SimpleSceneClass *Unknown_00() { return(View->OrbitalScene); }	// 0x804FC0
	virtual unsigned Unknown_04() { return(View->Unknown138); }			// 0x804FE0 (the orbital%04d.ini number)
	ScriptedMovieViewClass *View;							// +0x04  // NAME 2: what it holds
};

#include "ini.h"
// 0x804CA0: the orbitals: their manager and scene, the asset's orbital%04d.ini, the background dazzle layer (CREATE_ORBITAL)
bool ScriptedMovieViewClass::Create_Orbital(ScriptedMovieParamsClass *params)
{

	G_ASSERT(OrbitalManager == NULL);
	OrbitalManager = new ::OrbitalManager((SClient *)Get_Client(), (MainViewOrbitalCallbackClass *)new ScriptedMovieOrbitalCallbackClass(this));

	G_ASSERT(OrbitalManager != NULL);
	G_ASSERT(OrbitalScene == NULL);
	OrbitalScene = new SimpleSceneClass;
	G_ASSERT(OrbitalScene != NULL);
	OrbitalManager->Unknown_4F3310();
	const char *param = params->Unknown_7FFDC0();
	FAIL_IF((param == NULL) && "User error: Create_Orbital: Need asset id") {return(false);}
	Unknown138 = atoi(param);
	char filename[100];
	sprintf(filename, "orbital%04d.ini", Unknown138);
	INIClass ini(filename);



	FAIL_IF(!ini.Is_Loaded() && "User error: Create_Orbital: Can't load ini file.") {return(false);}
	OrbitalManager->Unknown_4F34C0(ini);

	G_ASSERT(BackgroundDazzleLayer == NULL);
	BackgroundDazzleLayer = new DazzleLayerClass;
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


















// NOT EXACT: frame 0x14, the client 0x18 (hfov in a local of its own, the camera 4 bytes higher) and the push_back's fill 0x80AFA0 inline (0.60)
// 0x805420: a camera on a bone of the slot's model, last in CameraList (optionally its view plane, in degrees)
bool ScriptedMovieViewClass::Add_Camera(ScriptedMovieParamsClass *params)
{
	const char *param = params->Unknown_7FFDC0();


	FAIL_IF((param == NULL) && "User error: Add_Camera: Need slot number") {return(false);}
	int slot = atoi(param);
	for (std::vector<ScriptedMovieObjectStruct>::iterator itor = Objects.begin(); itor != Objects.end(); itor++) {
		if ((*itor).Slot == slot) {



			G_ASSERT((*itor).Mesh);
			param = params->Unknown_7FFDC0();
			FAIL_IF((param == NULL) && "User error: Add_Camera: Need bone name") {return(false);}
			RenderObjClass *robj = (*itor).Mesh->Unknown_5B8B20();

			G_ASSERT(robj);
			ScriptedMovieCameraStruct camera;




			camera.Bone = robj->Get_Bone_Index(param);
			camera.RenderObject = robj;
			param = params->Unknown_7FFDC0();
			if (param != NULL) {
				float hfov = DEG_TO_RADF(atof(param));
				param = params->Unknown_7FFDC0();
				FAIL_IF((param == NULL) && "User error: Add_Camera: Need Y view plane") {return(false);}
				camera.VFov = DEG_TO_RADF(atof(param));
				camera.HFov = hfov;
			}
			CameraList.push_back(camera);
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











#include "..\..\wwaudio\AudibleSound.h"
// NOT EXACT: shape (0.76): the client saves all four registers first and its FAIL_IF exit joins the common one; ours returns early before ebx / ebp are saved
// 0x806BD0: a sound: 2D at once, or (with a slot and a bone) mv3dsnd.cpp's sound on the slot's model, kept on +0xF0
bool ScriptedMovieViewClass::Play_Audio(ScriptedMovieParamsClass *params)
{
	int slot = -1;
	const char *bone_name;
	int loop = 0;
	float volume = 1.0f;
	const char *audio_name = params->Unknown_7FFDC0();


	FAIL_IF((audio_name == NULL) && "User error: Play_Audio: Need audio name") {return(false);}
	const char *param = params->Unknown_7FFDC0();
	if (param != NULL) {
		slot = atoi(param);
	}
	bone_name = params->Unknown_7FFDC0();
	param = params->Unknown_7FFDC0();
	if (param != NULL) {
		loop = atoi(param);
	}
	param = params->Unknown_7FFDC0();
	if (param != NULL) {
		volume = atof(param);
	}
	if (slot == -1 || bone_name == NULL) {
		AudibleSoundClass *sound2d = SoundLibraryClass::Play_2D_Sound(audio_name, 1);









		G_ASSERT(sound2d);
		sound2d->Set_Volume(volume);
		if (loop) {
			sound2d->Set_Loop_Count(0);
		}
		sound2d->Release_Ref();
	} else {
		ScriptedMovieSoundClass *sound = new ScriptedMovieSoundClass(audio_name, bone_name);




		G_ASSERT(sound);
		if (loop) {
			sound->Unknown18 = true;
		} else {
			sound->Unknown18 = false;
		}
		sound->Unknown28 = volume;
		for (std::vector<ScriptedMovieObjectStruct>::iterator itor = Objects.begin(); itor != Objects.end(); itor++) {
			if ((*itor).Slot == slot) {



				G_ASSERT((*itor).Mesh);
				if ((*itor).Mesh) {
					sound->Unknown_7ED410((*itor).Mesh->Peek_Object());
				}
				break;
			}
		}
		UnknownF0.push_back(sound);
	}
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



















// 0x8070C0: the effect of a line's id, duration (seconds), scale and hsv shift (eff_o.cpp's factory 0x6E9490), timed through
EffectObjectClass *ScriptedMovieViewClass::Unknown_8070C0(ClientClass *client, ScriptedMovieParamsClass *params)
{
	const char *param = params->Unknown_7FFDC0();

	FAIL_IF((param == NULL) && "User error: Need special effect id") {return(NULL);}
	int desc_id = atoi(param);
	float duration = 0.0f;
	param = params->Unknown_7FFDC0();
	if (param != NULL) {
		duration = (float)atof(param) * 1000.0f;
	}
	float scale = 1.0f;
	param = params->Unknown_7FFDC0();
	if (param != NULL) {
		scale = atof(param);
	}
	float hue = 0.0f;
	param = params->Unknown_7FFDC0();
	if (param != NULL) {
		hue = atof(param);
	}
	float saturation = 0.0f;
	param = params->Unknown_7FFDC0();
	if (param != NULL) {
		saturation = atof(param);
	}
	float value = 0.0f;
	param = params->Unknown_7FFDC0();
	if (param != NULL) {
		value = atof(param);
	}
	EffectObjectClass *effect;
	unsigned id;
	int time;
	{
		id = --client->Unknown113C;
		Vector3 hsv(hue, saturation, value);
		time = duration;
		effect = Unknown_6E9490(desc_id, id, time, client->Unknown10BC, client, scale, hsv, 1.0f, -1, true);
	}


	G_ASSERT(effect);
	if (!Unknown172) {
		effect->Unknown_6E6E30();
		return(NULL);
	}
	if (duration > 0.0f) {
		ScriptedMovieEffectStruct timed;
		timed.ID = id;
		timed.EndTime = Unknown80 + time;
		Effects.push_back(timed);
	}
	return(effect);
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
