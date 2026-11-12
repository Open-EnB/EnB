// Earth & Beyond client (reconstruction): app\client\mv3dview.cpp, the 3-D movie view (Movie3DViewClass,
// 0x7EEF60-0x7F0F50; the asserts sit on the client's lines). Notes at the end of the file (L16, 2026-11-11).
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <string>
#include "ui_proc_impl.h"
#include "sclient.h"
#include "mv3dview.h"
#include "ui_mesh.h"
#include "co.h"
#include "unknown_490900.h"
#include "random.h"
#include "strtok_r.h"
#include "wwmath.h"
#include "..\shared\g_assert.h"
#include "..\shared\ro_proxy.h"
#include "..\..\ww3d2\camera.h"
#include "..\..\ww3d2\scene.h"
#include "..\..\ww3d2\light.h"
#include "..\..\ww3d2\ww3d.h"
#include "interface_scene.h"

static const char *Delimiters = ", \t";					// 0xBA1D4C  // NAME 1: strtok's
static char TestSection[256] = "LONDON";				// 0xBA1D50: movie3d.ini's [TEST] Section  // NAME 1: what it holds
static Movie3DStruct Movies[14] = {						// 0xBA1E50 (0x70 bytes, six entries set)  // NAME 1: what it holds
	{ TestSection, 0 },
	{ "LONDON", 0 },
	{ "LONDON", 1 },
	{ "LONDON", 2 },
	{ "JOVE", 0 },
	{ "MARS", 0 },
};
static RandomClass Random(time(NULL));					// 0xBE8B34 (the initialiser 0x7EEF60)  // NAME 1: what it is

// @0x7EEF90
Movie3DViewClass::Movie3DViewClass(ClientTaskClass *task, int movie) :
	Unknown65B810ViewClass(task, 3),
	Meshes(5),
	Settings(new ScriptedMovieViewerSettingsStruct),
	Light(new LightClass(LightClass::POINT)),
	Ship(NULL),
	CameraMesh(-1),
	Frames(0),
	Movie(&Movies[movie]),
	MovieIndex(movie),
	Loaded(false),
	Started(false)
{
	INI.Load("movie3d.ini");
	Ship = new ProxyRObj(NULL);
}

// @0x7EF220
void Movie3DViewClass::Unknown_04()
{
	Scene = new SimpleSceneClass;
	Camera = new CameraClass;
	Camera->Set_Clip_Planes(20.0f, 100000.0f);
	Camera->Set_View_Plane(0.97738439f, 0.71558499f);
	Camera->Set_Viewport(Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f));
	UIViewClass::Unknown_04();
	if (MovieIndex == 0) {
		INI.Get_String("TEST", "Section", "", TestSection, 255);
		Movies[0].Movie = INI.Get_Int("TEST", "Play", -1);
		if (!TestSection[0] || Movies[0].Movie == -1) {
			MovieIndex = Random(1, 5);
			Movie = &Movies[MovieIndex];
		}
	}
	if (Unknown_7EFD00()) {
		Unknown_7F0460();
		int ship = INI.Get_Int(Movie->Section, "Ship", 0);
		if (ship >= 0) {
			RenderObjClass *model = Client->Get_Avatar()->Unknown_6F3BC0();
			Ship->Unknown_47FF70(model);
			Ship->Set_LOD_Level(model->Get_LOD_Count() - 1);
			model->Release_Ref();
			Meshes[ship]->Unknown_5B9100("Ship", Ship);
		}
		CameraMesh = INI.Get_Int(Movie->Section, "Camera", 0);
		char section[256];
		strcpy(section, Movie->Section);
		strcat(section, "\\");
		strcat(section, "SFX");
		Sounds = new Movie3DSoundClass(INI, section);
		Loaded = true;
	}
}

// @0x7EF5D0
void Movie3DViewClass::Unknown_08()
{
}

// @0x7EF5F0
Movie3DViewClass::~Movie3DViewClass()
{
	int i = Meshes.Count();
	while (i--) {
		UIMeshClass *mesh = Meshes[i];
		Meshes.Delete(i);
		mesh->Detach(this);
		mesh->Unknown_5B92B0();
		delete mesh;
	}
	delete Sounds;
	Light->Release_Ref();
	delete Settings;
	Ship->Unknown_47FF70(NULL);
	Ship->Release_Ref();
}

// @0x7EF7D0
void Movie3DViewClass::Unknown_10()
{
	Settings->Unknown00->Remove();
	Light->Remove();
	if (Scene) {
		Scene->Release_Ref();
		Scene = NULL;
	}
	if (Camera) {
		Camera->Release_Ref();
		Camera = NULL;
	}
	UIViewClass::Unknown_10();
}

// @0x7EF850
void Movie3DViewClass::Unknown_00(SubjectClass *subject, void *event)
{
	UIProcEventStruct *info = (UIProcEventStruct *)event;
	int id = info->Proc->Id;
	switch (info->Event) {
		case 0:
			Sounds->Unknown_7EE770(id, info->Arg);
			break;
		case 1:
			Sounds->Unknown_7EE690(id, 0, info->Proc->UIMesh->InterfaceMesh);
			break;
		case 2:
			Sounds->Unknown_7EE700(id);
			break;
	}
}

// @0x7EF8C0
int Movie3DViewClass::Unknown_0C(bool on)
{
	WW3D::Begin_Render(on, true, Vector3(Settings->BackgroundColor));
	Scene->Visibility_Check(Camera);
	Unknown1C->Visibility_Check(Unknown20.Peek());
	Unknown24->Visibility_Check(Unknown28);
	WW3D::Render(Scene, Camera);
	((InterfaceSceneClass *)Unknown1C)->Render_With_PIP_Scenes(Unknown20);
	WW3D::Render(Unknown24, Unknown28);
	WW3D::End_Render();
	if (Frames++ == 4) {
		int i = Meshes.Count();
		while (i--) {
			Meshes[i]->Unknown70 &= ~1;
		}
	}
	Sounds->Unknown_7EE7E0(WW3D::Get_Frame_Time());
	return(0);
}

// @0x7EFA40
void Movie3DViewClass::Unknown_18(bool arg)
{
	Client->As_Space_Client();
	if (arg) {
		((SClient *)Client->As_Space_Client())->Unknown_74C370(Camera);
		Unknown_7F0410();
	} else {
		((SClient *)Client->As_Space_Client())->Unknown_74C370(NULL);
	}
}

// @0x7EFAE0
void Movie3DViewClass::Unknown_14()
{
	int i = Meshes.Count();
	while (i--) {
		Meshes[i]->Unknown_5B9410();
	}
	UIViewClass::Unknown_14();
	Unknown_7F06C0();
}

// @0x7EFCC0
bool Movie3DViewClass::Unknown_7EFCC0()
{
	if (Loaded && Started) {
		return(Meshes[0]->Unknown_5BA2A0(0));
	}
	return(true);
}











































































































































































































































































// @0x7EFD00
bool Movie3DViewClass::Unknown_7EFD00()
{
	G_ASSERT(!Started);
	char movie[256];
	INI.Get_String(Movie->Section, Unknown_7F09B0("Movie%d", Movie->Movie), "", movie, 255);
	if (!movie[0]) {
		return(false);
	}
	char *movie_next = NULL;
	char *name = strtok_r(movie, Delimiters, &movie_next);
	while (name) {
		DynamicVectorClass<char *> channels(32);
		int count;
		for (count = 0; INI.Is_Present(Unknown_7F0880(1, name), Unknown_7F09B0("Channel%d", count)); count++) {
			channels.Add(INI.Get_Alloc_String(SectionName, EntryName, ""));
		}
		if (INI.Is_Present(SectionName, "Mesh")) {
			char mesh_name[256];
			INI.Get_String(SectionName, "Mesh", "", mesh_name, 255);
			UIMeshClass *mesh = new UIMeshClass(mesh_name, count);
			Add_Mesh(mesh);
			for (int channel = 0; channel < channels.Count(); channel++) {
				char *channel_next = NULL;
				for (char *anim = strtok_r(channels[channel], Delimiters, &channel_next); anim; anim = strtok_r(NULL, Delimiters, &channel_next)) {
					Unknown_7F0880(2, "ANIM", anim);
					char ids[256];
					char copy[256];
					INI.Get_String(SectionName, "AnimID", "0", ids, 255);
					strcpy(copy, ids);
					int num = 0;
					char *token;
					for (token = strtok(ids, Delimiters); token; token = strtok(NULL, Delimiters)) {
						num++;
					}
					strcpy(ids, copy);
					int pick = Random(0, num - 1);
					token = strtok(ids, Delimiters);
					while (pick--) {
						token = strtok(NULL, Delimiters);
					}
					int id = atoi(token);
					char anim_name[256];
					if (!INI.Get_String(SectionName, Unknown_7F09B0("Anim%d", id), NULL, anim_name, 255)) {
						break;
					}
					bool cycle = INI.Get_Bool(SectionName, "Cycle", false);
					bool del = INI.Get_Bool(SectionName, "Delete", false);
					bool backward = INI.Get_Bool(SectionName, "Backward", false);
					mesh->Unknown_5B8EE0(channel, id, anim_name, cycle, del, backward, 0, -1);
				}
			}
			int i = channels.Count();
			while (i--) {
				free(channels[i]);
				channels.Delete(i);
			}
		}
		name = strtok_r(NULL, Delimiters, &movie_next);
	}
	return(true);
}

// @0x7F0410
void Movie3DViewClass::Unknown_7F0410()
{
	if (Loaded && !Started) {
		int i = Meshes.Count();
		while (i--) {
			Meshes[i]->Unknown70 &= ~2;
		}
		Started = true;
	}
}

// @0x7F0460
void Movie3DViewClass::Unknown_7F0460()
{
	strcpy(SectionName, Movie->Section);
	int lights = INI.Get_Int(SectionName, "Lights", 1);
	char name[256];
	if (!INI.Get_String(SectionName, Unknown_7F09B0("Light%d", Random(0, lights - 1)), NULL, name, 255)) {
		INI.Get_String(SectionName, Unknown_7F09B0("Light%d", 0), NULL, name, 255);
	}
	Light->Set_Intensity(1.0f);
	Light->Set_Force_Visible(true);
	Light->Set_Far_Attenuation_Range(1000000.0f, 1000000.0f);
	Light->Set_Ambient(Vector3(0.0f, 1.0f, 0.0f));
	Light->Set_Specular(Vector3(1.0f, 1.0f, 1.0f));
	Light->Set_Diffuse(Vector3(1.0f, 1.0f, 1.0f));
	Settings->Unknown00->Set_Force_Visible(true);
	Settings->Unknown00->Set_Far_Attenuation_Range(1000000.0f, 1000000.0f);
	UIMeshClass *mesh = Meshes[INI.Get_Int(Movie->Section, "Light", 0)];
	if (Settings->Unknown_4909D0(name, mesh->Peek_Mesh_Object()->Get_Position())) {
		Scene->Set_Ambient_Light(Vector3(Settings->Unknown10));
	} else {
		Scene->Set_Ambient_Light(Vector3(0.33f, 0.33f, 0.33f));
	}
	Settings->Unknown00->Add(Scene);
}

// @0x7F06C0
void Movie3DViewClass::Unknown_7F06C0()
{
	if (CameraMesh >= 0) {
		Matrix3D tm = Meshes[CameraMesh]->Unknown_5B9210("camera");
		tm.Rotate_Y(DEG_TO_RADF(-90.0f));
		tm.Rotate_Z(DEG_TO_RADF(-90.0f));
		Camera->Set_Transform(tm);
		Light->Set_Transform(tm);
	}
}












































































































// @0x7F0880
char *Movie3DViewClass::Unknown_7F0880(int count, ...)
{
	va_list args;
	va_start(args, count);
	strcpy(SectionName, Movie->Section);
	while (count--) {
		const char *str = va_arg(args, const char *);
		G_ASSERT(str && *str);
		strcat(SectionName, "\\");
		strcat(SectionName, str);
	}
	va_end(args);
	return(SectionName);
}

// @0x7F09B0
char *Movie3DViewClass::Unknown_7F09B0(const char *format, int number)
{
	sprintf(EntryName, format, number);
	return(EntryName);
}

// Reconstruction notes (L16, 2026-11-11)
// - The object: __FILE__ e:\build\beta\G\app\Client\mv3dview.cpp (0xBA1F58) and its two asserts, '!Started' on line 472
//   (0x7EFD00) and 'str && *str' on line 697 (0x7F0880). It starts at 0x7EEF60, the initialiser of Random (the CRT
//   table's slot 0xB6CE24, right after mv3dsnd.cpp's guarded <string> initialiser 0x7EEF30, slot 0xB6CE20), and ends
//   with its own guarded <string> initialiser 0x7F0F30; option.cpp's OptionClass follows at 0x7F0F60.
// - After 0x7F0880 come the inline members and the template instances the object emits: 0x7F09B0 (Unknown_7F09B0),
//   DynamicVectorClass<UIMeshClass *>'s members (tables 0xB07B24 / 0xB07B08; 0x7EF1E0 is its base's destructor),
//   0x7F0A50 and 0x7F0E70 (DynamicVectorClass<ObserverClass *>'s Add and Delete(int): SubjectClass::Attach's and the
//   destructor's nested calls, out of line here), and 0x7F0EC0 (VectorClass<char *>'s constructor).
// - Add_Mesh is an inline member: 0x7EFD00 keeps a second copy of this for it, and with it the observers' Add is called
//   out of line while the meshes' Add is inlined (the direct statements inline both).
// - Movies has six entries set in 0x70 bytes (0xBA1E50-0xBA1EC0): the count 14 is the room, not evidence.
// - movie3d.ini: [TEST] Section / Play; [<section>] Movie%d (mesh sections), Ship, Camera, Lights, Light%d, Light;
//   [<section>\<mesh>] Channel%d (animation names), Mesh; [<section>\ANIM\<anim>] AnimID, Anim%d, Cycle, Delete,
//   Backward; [<section>\SFX] the sounds (mv3dsnd.cpp).
