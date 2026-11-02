// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (PROVISIONAL: app\Client\splash.cpp, D137; notes at the end)
*/

#include <stdio.h>
#include <time.h>
#include "splash.h"
#include "..\..\ww3d2\scene.h"
#include "..\..\ww3d2\camera.h"
#include "..\..\ww3d2\assetmgr.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\..\wwdebug\wwprofile.h"



Splash::Splash(int type) :
	Camera(NULL),
	Scene(NULL),
	Model(NULL),
	Rating(NULL)
{
	char name[64];
	if (type == 0 || type == 2) {
		_snprintf(name, sizeof(name), "install%02da", time(NULL) % 7);
	} else if (type == 1) {
		_snprintf(name, sizeof(name), "starmap%02da", time(NULL) % 1);
	}

	Scene = new SimpleSceneClass;
	Scene->Set_Ambient_Light(Vector3(0.33f, 0.33f, 0.33f));

	Camera = new CameraClass;
	Camera->Set_View_Plane(Vector2(-0.5f, -0.375f), Vector2(0.5f, 0.375f));
	Camera->Set_Position(Vector3(0.0f, 0.0f, 640.0f));
	Camera->Set_Clip_Planes(0.1f, 5000.0f);

	WW3DAssetManager::Get_Instance()->Set_WW3D_Load_On_Demand(true);
	Model = WW3DAssetManager::Get_Instance()->Create_Render_Obj(name);
	if (type == 2) {
		Rating = WW3DAssetManager::Get_Instance()->Create_Render_Obj("demo-esrb01");
		if (Rating) {
			Model->Add_Sub_Object(Rating);
		}
	}
	if (Model) {
		Model->Add(Scene);
	}
}



Splash::~Splash()
{
	if (Scene) {
		Scene->Release_Ref();
		Scene = NULL;
	}
	if (Camera) {
		Camera->Release_Ref();
		Camera = NULL;
	}
	if (Rating) {
		Rating->Release_Ref();
		Rating = NULL;
	}
	if (Model) {
		Model->Release_Ref();
		Model = NULL;
	}
}



void Splash::Render()
{
	WWPROFILE("Splash::Render");
	if (Model == NULL) {
		return;
	}
	for (int i = 0; i < 2; i++) {
		WW3D::Begin_Render(true, true, Vector3(0.0f, 0.0f, 0.0f));
		Scene->Visibility_Check(Camera);
		WW3D::Render(Scene, Camera, false, false, Vector3(0.0f, 0.0f, 0.0f));
		WW3D::End_Render(true);
	}
}



/*
**	Reconstruction notes (GAD4, 2026-11-01). Client 0x6A9B60-0x6A9F39: the constructor 0x6A9B60, the destructor
**	0x6A9DB0 and Render 0x6A9E40, byte-exact (appfile.py match). Its literals 0xB917F8-0xB91836 ('demo-esrb01',
**	'install%02da', 'starmap%02da', 'Splash::Render') sit between the previous object's ('!serial_string') and
**	textgadg.cpp's (0xB91840 on); textgadg.cpp's code starts at 0x6AA000.
**
**	Placement (D137, interim): no __FILE__ or assert names the file. The class name comes from the profile string;
**	the file name is assumed from it. The neighbouring app\Client objects run in alphabetical order (loadingview,
**	sbaseview, slot_data_render?, [this], textgadg, tutorial_manager, view), which a 'splash.cpp' fits.
**
**	The 'starmap' branch calls time() and prints 0: the source's modulo was 1 (one starmap model), which VC6
**	folds away while keeping the call.
*/
