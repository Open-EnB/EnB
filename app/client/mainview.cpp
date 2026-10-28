// Earth & Beyond client (reconstruction): app\client\mainview.cpp (the client's __FILE__). Notes at the end of the file.
#include <stdio.h>
#include <string>
#include <vector>
#include "sclient.h"
#include "..\shared\unknown_debug.h"
#include "..\shared\land.h"
#include "..\..\wwlib\ini.h"
#include "..\..\ww3d2\scene.h"
#include "..\shared\stars.h"
#include "..\..\ww3d2\light.h"
#include "..\..\ww3d2\camera.h"
#include <math.h>
#include "..\..\ww3d2\ww3d.h"
#include "..\..\ww3d2\predlod.h"
#include "..\..\ww3d2\dazzle.h"
#include "..\..\wwdebug\wwprofile.h"
#include "..\..\ww3d2\colorspace.h"

bool Unknown_C404F8;			// NAME 1: neutral (D10): the client's 0xC404F8, 0x514480 copies 'Doppler_Enabled' to it (its home file is unknown)

// the +0x10C scene (0x75ECF0, 0xF4 bytes)
class MainViewSceneClass : public SimpleSceneClass	// NAME 1: neutral (D10)
{
public:
	MainViewSceneClass();							// 0x75ECF0
	char Unknown98[0xF4 - 0x98];
};

// enbdazzle.cpp's visibility handler (0x75DD00, 8 bytes)
class MainViewDazzleHandlerClass : public DazzleVisibilityClass	// NAME 1: neutral (D10)
{
public:
	MainViewDazzleHandlerClass(SClient *client);	// 0x75DD00  // NAME 1 [client]: what 0x50BA60 passes
	SClient *Client;								// +0x04  // NAME 2: what it holds
};

// the +0x84 object (0x7B6BD0, 0x24 bytes; gutter_data.cpp's neighbourhood)
class MainViewUnknown84Class	// NAME 1: neutral (D10)
{
public:
	MainViewUnknown84Class(SClient *client, const RefCountPtr<RefCountClass> &scene);	// 0x7B6BD0  // NAME 1 [client, scene]: what 0x50BA60 passes
	char Unknown00[0x24];
};

// the orbitals' callback (table 0xAED01C: 0x50C2B0 / 0x50C2D0)
class MainViewOrbitalCallbackClass	// NAME 1: neutral (D10)
{
public:
	MainViewOrbitalCallbackClass(MainViewClass *view) : View(view) {}	// NAME 1 [view]: neutral
	virtual SimpleSceneClass *Unknown_00() { return(View->Unknown_74()); }	// 0x50C2B0
	virtual unsigned Unknown_04() { return(View->Unknown1C8); }			// 0x50C2D0 (the sector)
	MainViewClass *View;							// +0x04  // NAME 2: what it holds
};






































































































// NOT EXACT: scheduling only (the array-ctor pushes, two float constants, one register); attempts.csv 0x50B360
MainViewClass::MainViewClass(ClientTaskClass *ctask) :
	ViewClass(ctask, 0),
	GraphicsDefault(false), GraphicsGamma(0), GraphicsLandPoly(0), GraphicsLandDetail(0), GraphicsSpacePoly(0), GraphicsTextureSize(0), GraphicsChanged(false), DefaultChanged(false),
	Gamma(0), LandPolyBudget(0), LandLOD(0), PolyBudget(0), TextureReduction(0), Radar(NULL), Unknown84(0),
	Scene(NULL), Unknown110(NULL), Unknown114(NULL), Unknown118(NULL), Unknown11C(NULL), Unknown120(NULL),
	DazzleLayer(NULL), Unknown128(0), Camera(NULL), Unknown130(true),
	Unknown138(NULL), Land(NULL), Orbitals(NULL),
	Unknown168(false), Unknown16C(0.0f, 0.0f, 0.0f),
	Unknown178(0.5f, 0.5f, 0.5f), Unknown184(0.0f, 0.0f, 1.0f), Unknown190(0),
	Unknown194(0.05f), Unknown198(0.1f), Unknown19C(0.1f), Unknown1A0(0.5f), Unknown1A4(true),
	SeaFogNear(0), SeaFogFar(0), SeaColor(1.0f, 1.0f, 1.0f), Wind(0.0f, 0.0f, 0.0f), Unknown1C8(0), Unknown1CC(false),
	Cpit(NULL), Unknown1D4(NULL), Group(NULL), Unknown1DC(NULL), Unknown1E0(0), Unknown1E4(NULL), ProfileIterator(NULL)
{
	// (the members above are the client's order: 0x50B360 stores them before the table)
	// (the base 0x65B810 clears +0x48; the view's helper is made last)
	// (the assert names the parameter)
	// (0x50B360 sets the table after the members)
	// (line 176 next)
	G_ASSERT(ctask);
	Unknown48 = new MainViewUnknown48Class(this, ctask->Client);
}

MainViewClass::~MainViewClass()
{
}

// NOT EXACT: similarity 0.84, registers only (the first light's and the stars' RefCountPtr assignments); attempts.csv 0x50BA60
void MainViewClass::Unknown_04()
{
	ViewClass::Unknown_04();
	Unknown1C->Set_Fog_Enable(false);
	Unknown110 = new SimpleSceneClass;
	Unknown114 = new SimpleSceneClass;
	Scene = new MainViewSceneClass;
	Unknown118 = new SimpleSceneClass;
	Unknown11C = new SimpleSceneClass;
	Unknown120 = new SimpleSceneClass;
	DazzleLayer = new DazzleLayerClass;
	Unknown128 = new MainViewDazzleHandlerClass(Client);
	DazzleRenderObjClass::Install_Dazzle_Visibility_Handler(Unknown128);
	Radar = new RadarSystemClass((SClient *)Client->Unknown_18());
	Unknown84 = new MainViewUnknown84Class((SClient *)Client->Unknown_18(), Create_Peek((RefCountClass *)Unknown_50()));
	Unknown110->Set_Fog_Enable(false);
	Unknown114->Set_Fog_Enable(false);
	Scene->Set_Fog_Enable(false);
	Unknown118->Set_Fog_Enable(false);
	Orbitals = new OrbitalManager(Client, new MainViewOrbitalCallbackClass(this));
	for (int i = 0; i < 4; i++) {
		Unknown144[i] = Create_NEW(new LightClass(LightClass::DIRECTIONAL));
		Unknown144[i]->Set_Ambient(Vector3(0.0f, 0.0f, 0.0f));
		Unknown144[i]->Set_Diffuse(Vector3(0.0f, 0.0f, 0.0f));
		Unknown144[i]->Set_Specular(Vector3(0.0f, 0.0f, 0.0f));
		Unknown154[i] = Create_NEW(new LightClass(LightClass::DIRECTIONAL));
		Unknown154[i]->Set_Ambient(Vector3(0.0f, 0.0f, 0.0f));
		Unknown154[i]->Set_Diffuse(Vector3(0.0f, 0.0f, 0.0f));
		Unknown154[i]->Set_Specular(Vector3(0.0f, 0.0f, 0.0f));
		Unknown120->Add_Render_Object(Unknown154[i].Peek());
	}
	Unknown164 = Create_NEW(new LightClass(LightClass::DIRECTIONAL));
	Unknown164->Set_Ambient(Vector3(0.0f, 0.0f, 0.0f));
	Unknown164->Set_Diffuse(Vector3(0.0f, 0.0f, 0.0f));
	Unknown164->Set_Specular(Vector3(0.0f, 0.0f, 0.0f));
	WWProfileManager::Reset();
	ProfileIterator = WWProfileManager::Get_Iterator();
	StarField = Create_NEW(new StarFieldClass(Client->StarBudget));
	Unknown114->Add_Render_Object(StarField.Peek());
	Unknown_514480();
	GraphicsChanged = true;
	DefaultChanged = true;
	Unknown_5146D0();
}
































































































































































int MainViewClass::Unknown_0C(bool on)
{
	float znear, zfar;
	Camera->Get_Clip_Planes(znear, zfar);
	if (Client->Unknown1198) {
		return(Render_Initializing());
	}
	switch (Client->ViewMode) {
		case 0:
			return(Render_Space_Mode(znear, zfar));
		case 1:
			return(Render_Planet_Mode(znear, zfar));
		case 5:
			return(Render_Gas_Giant_Mode(znear, zfar));
	}
	G_ASSERT(0);
	Camera->Set_Clip_Planes(znear, zfar);
	Camera->Set_Depth_Range(0.0f, 1.0f);
	return(WW3D_ERROR_GENERIC);
}

static const float Unknown110Near = 20.0f;		// NAME 1: neutral (D10): the +0x110 scene's clip planes (client 0xAECF38 / 0xAECF34)
static const float Unknown110Far = 100000.0f;	// NAME 1: neutral (D10)
static const float Unknown114Near = 50.0f;		// NAME 1: neutral (D10): the +0x114 scene's (0xAECF40 / 0xAECF3C)
static const float Unknown114Far = 150.0f;		// NAME 1: neutral (D10)
static const float Unknown_AECEE8 = 1.0f, Unknown_AECEEC = 1.0f, Unknown_AECEF0 = 1.0f, Unknown_AECEF4 = 0.96f, Unknown_AECEF8 = 0.96f, Unknown_AECEFC = 0.96f, Unknown_AECF00 = 0.95f, Unknown_AECF04 = 1.0f, Unknown_AECF08 = 1.0f, Unknown_AECF0C = 1.0f, Unknown_AECF10 = 0.95f, Unknown_AECF14 = 1.0f, Unknown_AECF18 = 0.975f, Unknown_AECF1C = 0.975f, Unknown_AECF20 = 0.95f, Unknown_AECF24 = 0.95f, Unknown_AECF28 = 0.0f;	// NAME 1: neutral (D10): the depth ranges, by client address

float MainViewClass::Unknown_50CC10()
{
	PredictiveLODOptimizerClass::Clear();
	float znear, zfar;
	Camera->Get_Clip_Planes(znear, zfar);
	Camera->Set_Clip_Planes(Unknown110Near, Unknown110Far);
	Unknown110->Visibility_Check(Camera);
	Camera->Set_Clip_Planes(Unknown114Near, Unknown114Far);
	Unknown114->Visibility_Check(Camera);
	Camera->Set_Clip_Planes(znear, zfar);
	Unknown1C->Visibility_Check(Unknown20);
	Unknown24->Visibility_Check(Unknown28);
	return(PredictiveLODOptimizerClass::Get_Total_Cost());
}















void MainViewClass::Unknown_50CCF0(float *znear, float *zfar)
{
	FAIL_IF(!znear) {return;}
	FAIL_IF(!zfar) {return;}
	float v = tan(Camera->Get_Vertical_FOV() * 0.5f);
	float h = tan(Camera->Get_Horizontal_FOV() * 0.5f);
	*znear = (1.0f / WWMath::Sqrt(h * h + v * v + 1.0f)) * Orbitals->Unknown50;
	*zfar = Orbitals->Get_Unknown54();
}

void MainViewClass::Unknown_50CE00(float znear, float zfar)
{
	float old_near, old_far;
	Camera->Get_Clip_Planes(old_near, old_far);
	float zmin, zmax;
	Camera->Get_Depth_Range(&zmin, &zmax);
	if (Client->ViewMode == 0) {
		Camera->Set_Depth_Range(Unknown_AECF10, Unknown_AECF0C);
	} else if (Client->ViewMode == 1) {
		Camera->Set_Depth_Range(Unknown_AECEF4, Unknown_AECEF0);
	} else if (Client->ViewMode == 5) {
		Camera->Set_Depth_Range(Unknown_AECF18, Unknown_AECF14);
	}
	Camera->Set_Clip_Planes(znear, zfar);
	WW3D::Render(Unknown11C, Camera);
	Camera->Set_Clip_Planes(old_near, old_far);
	Camera->Set_Depth_Range(zmin, zmax);
}

void MainViewClass::Unknown_50CF70()
{
	float old_near, old_far;
	Camera->Get_Clip_Planes(old_near, old_far);
	float zmin, zmax;
	Camera->Get_Depth_Range(&zmin, &zmax);
	Camera->Set_Depth_Range(Unknown_AECEF8, Unknown_AECEF8);
	Camera->Set_Clip_Planes(0.0f, 1.0f);
	Unknown_510D40(Unknown16C);
	Camera->Set_Clip_Planes(old_near, old_far);
	Camera->Set_Depth_Range(zmin, zmax);
}

// NOT EXACT: register / order only (the client reloads ViewMode after the +0x10FA test; ours keeps it in eax); attempts.csv 0x50D060
void MainViewClass::Unknown_50D060()
{
	float old_near, old_far;
	Camera->Get_Clip_Planes(old_near, old_far);
	float zmin, zmax;
	Camera->Get_Depth_Range(&zmin, &zmax);
	if (Client->ViewMode == 0 || (Client->ViewMode == 1 && Client->Unknown10FA)) {
		Camera->Set_Depth_Range(Unknown_AECF08, Unknown_AECF04);
	} else if (Client->ViewMode == 1) {
		Camera->Set_Depth_Range(Unknown_AECF00, Unknown_AECEFC);
	} else if (Client->ViewMode == 5) {
		Camera->Set_Depth_Range(Unknown_AECF20, Unknown_AECF1C);
	}
	Camera->Set_Clip_Planes(Unknown110Near, Unknown110Far);
	WW3D::Render(Unknown110, Camera);
	Camera->Set_Clip_Planes(old_near, old_far);
	Camera->Set_Depth_Range(zmin, zmax);
}

void MainViewClass::Unknown_50D1E0()
{
	if (Unknown130) {
		float old_near, old_far;
		Camera->Get_Clip_Planes(old_near, old_far);
		float zmin, zmax;
		Camera->Get_Depth_Range(&zmin, &zmax);
		Camera->Set_Depth_Range(Unknown_AECEE8, Unknown_AECEE8);
		Camera->Set_Clip_Planes(Unknown114Near, Unknown114Far);
		WW3D::Render(Unknown114, Camera);
		Camera->Set_Clip_Planes(old_near, old_far);
		Camera->Set_Depth_Range(zmin, zmax);
	}
}

void MainViewClass::Unknown_50D310(float znear, float zfar)
{
	float old_near, old_far;
	Camera->Get_Clip_Planes(old_near, old_far);
	float zmin, zmax;
	Camera->Get_Depth_Range(&zmin, &zmax);
	Camera->Set_Depth_Range(Unknown_AECF28, Unknown_AECF24);
	Camera->Set_Clip_Planes(znear, zfar);
	WW3D::Render(Scene, Camera);
	Camera->Set_Clip_Planes(old_near, old_far);
	Camera->Set_Depth_Range(zmin, zmax);
}

void MainViewClass::Unknown_50D430(float znear, float zfar)
{
	float old_near, old_far;
	Camera->Get_Clip_Planes(old_near, old_far);
	float zmin, zmax;
	Camera->Get_Depth_Range(&zmin, &zmax);
	Camera->Set_Depth_Range(Unknown_AECEEC, Unknown_AECEEC);
	Camera->Set_Clip_Planes(znear, zfar);
	DazzleLayer->Render(Camera);
	Camera->Set_Clip_Planes(old_near, old_far);
	Camera->Set_Depth_Range(zmin, zmax);
}





















































WW3DErrorType MainViewClass::Render_Initializing()
{
	WWPROFILE("MainView::Render_Initializing");
	WW3DErrorType error = WW3D::Begin_Render(false, true);
	if (error != WW3D_ERROR_OK) {
		return(error);
	}
	Unknown_50D520();
	WW3D::End_Render(true);
	return(WW3D_ERROR_OK);
}



// NOT EXACT: similarity 0.84 (the inlined 0x50CCF0 stores zfar first in the client; scheduling); attempts.csv 0x50DB20
WW3DErrorType MainViewClass::Render_Gas_Giant_Mode(float znear, float zfar)
{
	WWPROFILE("MainView::Render_Gas_Giant_Mode");
	Update(Camera, false);
	WW3DErrorType error = WW3D::Begin_Render(false, true);
	if (error != WW3D_ERROR_OK) {
		return(error);
	}
	DazzleRenderObjClass::Set_Current_Dazzle_Layer(DazzleLayer);
	Unknown_50CC10(); float cost = PredictiveLODOptimizerClass::Get_Total_Cost();
	PredictiveLODOptimizerClass::Clear();
	float old_near, old_far;
	Camera->Get_Clip_Planes(old_near, old_far);
	float near2, far2;
	Unknown_50CCF0(&near2, &far2);
	Camera->Set_Clip_Planes(near2, far2);
	Unknown11C->Visibility_Check(Camera);
	Camera->Set_Clip_Planes(znear, zfar);
	Scene->Visibility_Check(Camera);
	Camera->Set_Clip_Planes(old_near, old_far);
	float budget = (float)PolyBudget - cost;
	if (budget <= 0.0f) budget = 0.0f;
	PredictiveLODOptimizerClass::Optimize_LODs(budget);
	Unknown_50CE00(near2, far2);
	Unknown_50D060();
	Unknown_50D310(znear, zfar);
	Unknown_50D430(near2, far2);
	Unknown_50D520();
	WW3D::End_Render(true);
	return(WW3D_ERROR_OK);
}

// NOT EXACT: similarity 0.75 (as Render_Gas_Giant_Mode, and the inline choices of 0x50D1E0 / 0x50E6A0); attempts.csv 0x50E090
WW3DErrorType MainViewClass::Render_Space_Mode(float znear, float zfar)
{
	WWPROFILE("MainView::Render_Space_Mode");
	Update(Camera, false);
	WW3DErrorType error = WW3D::Begin_Render(Client->Unknown10FA, true);
	if (error != WW3D_ERROR_OK) {
		return(error);
	}
	DazzleRenderObjClass::Set_Current_Dazzle_Layer(DazzleLayer);
	Unknown_50CC10(); float cost = PredictiveLODOptimizerClass::Get_Total_Cost();
	PredictiveLODOptimizerClass::Clear();
	float old_near, old_far;
	Camera->Get_Clip_Planes(old_near, old_far);
	float near2, far2;
	Unknown_50CCF0(&near2, &far2);
	Camera->Set_Clip_Planes(near2, far2);
	Unknown11C->Visibility_Check(Camera);
	Camera->Set_Clip_Planes(znear, zfar);
	Scene->Visibility_Check(Camera);
	Camera->Set_Clip_Planes(old_near, old_far);
	float budget = (float)PolyBudget - cost;
	if (budget <= 0.0f) budget = 0.0f;
	PredictiveLODOptimizerClass::Optimize_LODs(budget);
	if (!Client->Unknown10FA) {
		Unknown_50D060();
		Unknown_50D1E0();
		Unknown_50CE00(near2, far2);
	} else {
		Unknown_50D1E0();
		Unknown_50CE00(near2, far2);
		Unknown_50D060();
	}
	Unknown_50D310(znear, zfar);
	Unknown_50D430(near2, far2);
	Unknown_50D520();
	WW3D::End_Render(true);
	return(WW3D_ERROR_OK);
}

// NOT EXACT: similarity 0.67 (functional from the pseudocode; the inline choices differ); attempts.csv 0x50E6E0
WW3DErrorType MainViewClass::Render_Planet_Mode(float znear, float zfar)
{
	WWPROFILE("MainView::Render_Planet_Mode");
	Vector3 position = Camera->Get_Position();
	bool under = false;
	if (Land) {
		under = Land->Has_Sea() && Land->Get_Sea_Level() > position.Z;
		Update(Camera, under);
	}
	WW3DErrorType error = WW3D::Begin_Render(!Client->Unknown10FA, true);
	if (error != WW3D_ERROR_OK) {
		return(error);
	}
	DazzleRenderObjClass::Set_Current_Dazzle_Layer(DazzleLayer);
	Unknown_50CC10(); float cost = PredictiveLODOptimizerClass::Get_Total_Cost();
	PredictiveLODOptimizerClass::Clear();
	float old_near, old_far;
	Camera->Get_Clip_Planes(old_near, old_far);
	float near2, far2;
	Unknown_50CCF0(&near2, &far2);
	Camera->Set_Clip_Planes(near2, far2);
	Unknown11C->Visibility_Check(Camera);
	Camera->Set_Clip_Planes(znear, zfar);
	Scene->Visibility_Check(Camera);
	Camera->Set_Clip_Planes(old_near, old_far);
	float budget = (float)LandPolyBudget - cost;
	if (budget <= 0.0f) budget = 0.0f;
	PredictiveLODOptimizerClass::Optimize_LODs(budget);
	if (!Client->Unknown10FA) {
		if (Unknown16C.Length2() < 0.3f && !under) {
			Unknown_50D1E0();
		}
		Unknown_50CE00(near2, far2);
		if (!under) {
			Unknown_50CF70();
			Unknown_50D060();
		}
	} else if (!under) {
		Unknown_50D060();
		Unknown_50D1E0();
		Unknown_50CE00(near2, far2);
	}
	Unknown_50D310(znear, zfar);
	Unknown_50D430(near2, far2);
	Unknown_50D520();
	WW3D::End_Render(true);
	return(WW3D_ERROR_OK);
}
































































// the day / night colour: the value a quarter of the source's, plus a half (0x50EE60, out of line; Update inlines it)
Vector3 Unknown_50EE60(const Vector3 &source)
{
	Vector3 hsv;
	RGB_To_HSV(hsv, source);
	hsv.Z = hsv.Z * 0.25f + 0.5f;
	Vector3 rgb;
	HSV_To_RGB(rgb, hsv);
	return(rgb);
}














































































































































































































































































































































































































































































































































































































void MainViewClass::Set_Camera(CameraClass *camera)
{
	if (Camera) Camera->Release_Ref();
	Camera = camera;
	if (Camera) Camera->Add_Ref();
}


































































































































































































void MainViewClass::Unknown_14()
{
	Unknown_512400();
	ViewClass::Unknown_14();
	Unknown48->Unknown_34();
}






























































































void MainViewClass::Unknown_512B10(int x, int y, float *fx, float *fy)
{
	int width, height, bits;
	bool windowed;
	WW3D::Get_Device_Resolution(width, height, bits, windowed);
	if (x < 0) x = 0; else if (x > width) x = width;
	if (y < 0) y = 0; else if (y > height) y = height;
	*fx = (float)x / width;
	*fy = (float)y / height;
}




















































































































































































































void MainViewClass::Unknown_513630(int arg1, int arg2, int arg3, int arg4)
{
	if (Radar) Radar->Unknown_7BD6E0(arg1);
	if (Cpit) Cpit->Scan->Unknown_60E540(arg1, arg2, arg3, arg4);
}

UIScanClass *MainViewClass::Unknown_5136A0()
{
	return(Cpit ? Cpit->Scan : NULL);
}

ClientObjectClass *MainViewClass::Unknown_5C()
{
	return((Cpit && Cpit->Scan) ? Cpit->Scan->Unknown88 : NULL);
}


















// 0x513700: the sector's stars and orbital%04d.ini (cpacket.cpp: SERVER_PARAMETERS 66)
void MainViewClass::Unknown_513700(unsigned sector)
{
	Unknown1C8 = sector;
	StarField->Init(sector);
	Orbitals->Unknown_4F3310();
	char filename[100];
	sprintf(filename, "orbital%04d.ini", sector);
	INIClass ini(filename);
	if (ini.Is_Loaded()) {
		Orbitals->Unknown_4F34C0(ini);
	}
}

// 0x5137F0: the land of sector `sector` (cpacket.cpp: SERVER_PARAMETERS 66 in view mode 1): land%04d.ini through
// LandClass::Create into the scene, the sea fog and colour from its [GENERAL], and the decorations from land%04d.dcb, or
// made from decoration.ini when the .dcb has none
void MainViewClass::Unknown_5137F0(unsigned sector)
{
	FAIL_IF(Land!=NULL) {return;}
	char filename[100];
	sprintf(filename, "land%04d.ini", sector);


	SClient *sclient = (SClient *)Client->Unknown_18();

	FAIL_IF(!sclient) {return;}
	bool flag = sclient->Unknown1320; Land = LandClass::Create(filename, flag);	// the flag through a local, as the client (one line: the FAIL_IFs)
	FAIL_IF(NULL==Land) {return;}
	Scene->Add_Render_Object(Land);
	INIClass ini(filename);
	SeaFogNear = ini.Get_Float("GENERAL", "SEAFOGNEAR", 100.0f);
	SeaFogFar = ini.Get_Float("GENERAL", "SEAFOGFAR", 3000.0f);
	SeaColor.X = ini.Get_Float("GENERAL", "SEACOLORR", 1.0f);
	SeaColor.Y = ini.Get_Float("GENERAL", "SEACOLORG", 1.0f);
	SeaColor.Z = ini.Get_Float("GENERAL", "SEACOLORB", 1.0f);
	std::vector<std::string> names;
	std::vector<LandDecorationStruct> decorations;
	sprintf(filename, "land%04d.dcb", sector);
	Land->Load_Decorations(names, decorations, filename);
	if (names.size() != 0 && decorations.size() != 0) {
		Land->Build_Decorations(names, decorations);
	} else {
		Land->Create_Decorations(sector);
	}
}

// 0x513B90: the sector's wind from gas%04d.ini (cpacket.cpp: SERVER_PARAMETERS 66, view mode 5), per hour
void MainViewClass::Unknown_513B90(unsigned sector)
{
	char filename[100];
	sprintf(filename, "gas%04d.ini", sector);
	INIClass ini(filename);
	Wind.X = ini.Get_Float("GENERAL", "WINDX", 0.0f);
	Wind.Y = ini.Get_Float("GENERAL", "WINDY", 0.0f);
	Wind.Z = ini.Get_Float("GENERAL", "WINDZ", 0.0f);
	Wind *= 3600.0f;
}

int MainViewClass::Unknown_513D00() { return(Unknown138->Unknown9C); }


void MainViewClass::Unknown_513D50() { Unknown138->Unknown_53E0B0(); }
void MainViewClass::Unknown_513D70() { Unknown138->Unknown_53E0E0(); }
void MainViewClass::Unknown_513D90(int arg) { Unknown138->Unknown_53E110(arg); }
void MainViewClass::Unknown_513DC0() { Unknown138->Unknown_53E160(); }
void MainViewClass::Unknown_513DE0(int arg) { Unknown138->Unknown_53E190(arg); }
void MainViewClass::Unknown_513E10(int arg) { Unknown138->Unknown_53E1E0(arg); }
void MainViewClass::Unknown_513E40(int arg) { Unknown138->Unknown_53E220(arg); }
void MainViewClass::Unknown_513E70() { Unknown138->Unknown_53E260(); }
void MainViewClass::Unknown_513E90(int arg) { Unknown138->Unknown_53E290(arg); }
void MainViewClass::Unknown_513EC0() { Unknown138->Unknown_53E2C0(); }
void MainViewClass::Unknown_513EE0(int arg) { Unknown138->Unknown_53E2F0(arg); }
void MainViewClass::Unknown_513F10() { Unknown138->Unknown_53E3C0(); }
void MainViewClass::Unknown_513F30(int arg) { Unknown138->Unknown_53E3F0(arg); }
void MainViewClass::Unknown_513F60(FileClass *file, int indent) {}
void MainViewClass::Unknown_513F80(int arg) {}
void MainViewClass::Unknown_513FA0() {}

void MainViewClass::Unknown_513FC0()
{
	if (Land) {
		Scene->Remove_Render_Object(Land);
		REF_PTR_RELEASE(Land);
	}
}





















































































































































void MainViewClass::Unknown_5141A0()
{
	Cpit->Unknown_58();
	((ViewUnknown34Class *)Unknown1D4)->Unknown_58();
	((ViewUnknown34Class *)Group)->Unknown_58();
}

void MainViewClass::Unknown_5141E0()
{
	Cpit->Unknown_5C();
	((ViewUnknown34Class *)Unknown1D4)->Unknown_5C();
	((ViewUnknown34Class *)Group)->Unknown_5C();
}

void MainViewClass::Unknown_514220()
{
	Cpit->Unknown_5C();
	((ViewUnknown34Class *)Unknown1D4)->Unknown_5C();
	((ViewUnknown34Class *)Group)->Unknown_5C();
	Unknown1E4->Unknown_5C();
}

























void MainViewClass::Add_Object(ClientObjectClass *object)
{
	if (object == NULL) {


		G_ASSERT(object);
		return;
	}
	RenderObjClass *model = object->Model;
	if (model) {
		model->Set_User_Data(object);
		if (!object->Unknown_6F5D90()) model->Add((SceneClass *)Unknown_64());
		model->Set_Transform(Matrix3D(true));
	}
}

void MainViewClass::Remove_Object(ClientObjectClass *object)
{
	if (object == NULL) {






		G_ASSERT(object);
		return;
	}
	if (object->Model && !object->Unknown_6F5D90()) object->Model->Remove();
}

int MainViewClass::Unknown_514400()
{
	return(Cpit ? Cpit->UnknownDC : 0);
}

MainViewUnknown64Class *MainViewClass::Unknown_64()
{
	return((MainViewUnknown64Class *)Scene);
}

void MainViewClass::Unknown_514450()
{
	if (Land) Land->Set_Dirty();
}

void MainViewClass::Unknown_514480()
{
	bool graphics_default = Unknown_BE8B38->Get_Bool("Graphics_Default");
	float gamma = Unknown_BE8B38->Get_Float("Graphics_Gamma");
	float land_poly = Unknown_BE8B38->Get_Float("Graphics_Land_Poly");
	float land_detail = Unknown_BE8B38->Get_Float("Graphics_Land_Detail");
	float space_poly = Unknown_BE8B38->Get_Float("Graphics_Space_Poly");
	float texture_size = Unknown_BE8B38->Get_Float("Graphics_Texture_Size");
	DefaultChanged = false;
	GraphicsChanged = false;
	if (graphics_default != GraphicsDefault) {
		GraphicsDefault = graphics_default;
		DefaultChanged = true;
	}
	if (WWMath::Fabs(gamma - GraphicsGamma) > 1.0f) {
		GraphicsChanged = true;
		GraphicsGamma = gamma;
	}
	if (WWMath::Fabs(land_poly - GraphicsLandPoly) > 1.0f) {
		GraphicsChanged = true;
		GraphicsLandPoly = land_poly;
	}
	if (WWMath::Fabs(land_detail - GraphicsLandDetail) > 1.0f) {
		GraphicsChanged = true;
		GraphicsLandDetail = land_detail;
	}
	if (WWMath::Fabs(space_poly - GraphicsSpacePoly) > 1.0f) {
		GraphicsChanged = true;
		GraphicsSpacePoly = space_poly;
	}
	if (WWMath::Fabs(texture_size - GraphicsTextureSize) > 1.0f) {
		GraphicsTextureSize = texture_size;
		GraphicsChanged = true;
	}
	Unknown_C404F8 = Unknown_BE8B38->Get_Bool("Doppler_Enabled");
}

void MainViewClass::Unknown_18(int arg)
{
	if (Radar) Radar->Unknown_7BCA40(arg);
}

/*
**	Notes (LD3, 2026-10-28)
**
**	mainview.cpp is the main view (MainViewClass, sclient.h; constructor 0x50B360, client 0x50B2C0-0x5146xx,
**	97 functions, appfile.py show app\client\mainview.cpp). The zone loader 0x5137F0 sits on the
**	client's FAIL_IF lines 1974 / 1981 / 1983 (the lines above are left empty for the functions not yet rebuilt), with its
**	neighbour 0x513B90 (gas%04d.ini's wind) after it, and 0x513700 (the stars, orbital%04d.ini) before it (HN2:
**	clienttest's fake main view carries a StarField and an OrbitalManager for it).
**	Client->Unknown_18() is ClientClass's slot +0x18 (0 in the base, 0x7268E0); SClient's override returns the
**	SClient ('!sclient'). Its +0x1320 byte is LandClass::Create's flag.
**
**	MV1 (2026-10-28): the class's layout from the constructor 0x50B360 (its stores in member order before the
**	table, sclient.h), the destructor 0x50B950, the table 0xAECF78 (34 slots: +0x5C..+0x84 are the main view's own;
**	+0x1C..+0x54 are the base's inline slots, whose copies the linker took from this object: 0x50B680-0x50B7E0;
**	the base's own table is 0xAF9854, 0x65B810's). Add_Object 0x514270 / Remove_Object 0x514390: the model's
**	Set_User_Data, Add to the scene slot +0x64 returns (the +0x10C scene) unless the object's 0x6F5D90 says
**	otherwise, an identity transform; Remove only in the same case. The constructor is not exact (scheduling)
**	(attempts.csv 0x50B360). Not rebuilt yet because clienttest drives their stand-ins: 0x512A00, 0x513540,
**	0x513600, 0x513CD0, 0x513D20, 0x514010 (expected.txt 'gen MainViewClass::').
*/
