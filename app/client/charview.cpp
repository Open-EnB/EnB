// Earth & Beyond client (reconstruction): charview.cpp's CharacterViewClass (charview.h). Notes at the end.
#include <string.h>
#include <math.h>
#include "charview.h"
#include "tasks\tsk_login.h"
#include "ui_avsel.h"
#include "uiview.h"
#include "..\shared\g_assert.h"
#include "..\audio\soundlib.h"
#include "..\..\wwaudio\WWAudio.h"
#include "..\..\wwaudio\AudibleSound.h"
#include "..\..\wwlib\ini.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\scene.h"
#include "..\..\ww3d2\camera.h"
#include "..\..\ww3d2\light.h"
#include "..\..\ww3d2\hanim.h"
#include "..\..\ww3d2\assetmgr.h"
#include "..\..\ww3d2\layer.h"
#include "..\..\ww3d2\dazzle.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\..\ww3d2\predlod.h"
#include "..\..\ww3d2\dx8renderer.h"
#include "..\..\ww3d2\matinfo.h"
#include "matrix3d.h"
#include "..\..\wwlib\ref_ptr.h"
#include "wwmath.h"
#include <vector>

// The frame's platform turn (0x679930): a file inline the frame expands (as its own code, the client's Matrix3D's
// Vector4 constructor goes through ??_H and Rotate_Z is called out of line, ViewRObjs[4] and +0x1A0 read again)
inline void CharacterViewClass::Unknown_Inline_Platform()
{
	Matrix3D platform(true);
	int bone = ViewRObjs[4]->Get_Bone_Index("platform");
	platform.Rotate_Z(Unknown1A0);
	ViewRObjs[4]->Capture_Bone(bone);
	ViewRObjs[4]->Control_Bone(bone, platform, false);
}

// The environment maps +0x3A4 read through an inline accessor: the avatars' constructor calls then load and push
// it before 'this' is set, as the client does (CV2: a plain member read schedules both loads first, 0.79)
static inline EnvironmentMapManagerClass *Unknown_Inline_Env_Maps(CharacterViewClass *view)
{
	return view->Unknown3A4;
}








// 0xBFAE52 is WW3D::OverbrightModifyOnLoad (MeshModelClass::post_process 0x9BCC40 reads it): FE_ROOM loads overbright

// sclient.cpp's env map object (0x24 bytes): its implicit destructor is out of line here (0x679820)
class Unknown67D360Class		// NAME 1: neutral (D10): 0x14 bytes (env_map_manager.cpp's); its destructor out of line here (0x67D360)
{
public:
	~Unknown67D360Class() { if (Unknown10) { Unknown10->Release_Ref(); Unknown10 = NULL; } }
	std::string Unknown00;								// +0x00
	RefCountClass *Unknown10;							// +0x10 (released by the destructor)
};
class EnvironmentMapManagerClass	// NAME 3: by convention (D24) from the client's env_map_manager.cpp
{
public:
	EnvironmentMapManagerClass() : Unknown10(2) {}
	void Load(const char *ini);							// 0x760360 (env_map_manager.cpp): 'envmaps.ini'  // NAME 2: what it does
	void Unknown_760770(int base_asset, bool a);		// 0x760770 (env_map_manager.cpp)  // NAME 1 [base_asset, a]: sclient.cpp's
	std::vector<Unknown67D360Class> Unknown00;			// +0x00
	int Unknown10;										// +0x10 (2)
	INIClass INI;										// +0x14  // NAME 2: what it is
};

// The mode's screens (0x679160), declared as far as this file makes them
class LoginInterface : public InterfaceClass	// NAME 5: the client's RTTI (ui_login.cpp's)
{
public:
	LoginInterface(UIViewClass *view, LayerClass *layer);	// 0x5ACB80
	char Unknown84[0xDC - 0x84];
};
class Unknown5DB670InterfaceClass : public InterfaceClass	// NAME 1: neutral (D10): mode 0 CHARACTER_POPUP_MESSAGE
{
public:
	Unknown5DB670InterfaceClass(UIViewClass *view, LayerClass *layer);	// 0x5DB670  // NAME 1 [view, layer]: as LoginInterface's
	char Unknown84[0xD4 - 0x84];
};
#include "ui_class.h"
#include "ui_shipcust.h"




class AvatarCreationInterface : public InterfaceClass	// NAME 3: the client's RTTI (ui_avatr.cpp's window, mode 4), as far as this file makes it
{
public:
	AvatarCreationInterface(UIViewClass *view, LayerClass *layer, bool keep);	// 0x553AF0 (ui_avatr.cpp)  // NAME 1 [view, layer, keep]: as ui_avatr.cpp's
	char Unknown84[0x2F8 - 0x84];
};
class Unknown615A50InterfaceClass : public InterfaceClass	// NAME 1: neutral (D10): mode 5
{
public:
	Unknown615A50InterfaceClass(UIViewClass *view, LayerClass *layer);	// 0x615A50  // NAME 1 [view, layer]: as LoginInterface's
	char Unknown84[0xEC - 0x84];
};
// mode 6: ShipCustomizeInterface (ui_shipcust.h; the lines it held kept blank, so the FAIL_IFs below keep the client's __LINE__)





class RegistrationInterface : public InterfaceClass	// NAME 1: neutral (D10): modes 9-12
{
public:
	RegistrationInterface(UIViewClass *view, LayerClass *layer, int unknown);	// 0x5EF0B0 (3, 20, 3, 0)  // NAME 1 [view, layer, unknown]: neutral
	char Unknown84[0x5B8 - 0x84];
};

// three colours (the static initializer 0x67C3C0)
static Vector3 Unknown_BE6018[3] = {	// NAME 1: neutral (D10)
	Vector3(0.8f, 0.1f, 0.05f),
	Vector3(0.95f, 0.51f, 0.13f),
	Vector3(0.0f, 0.52f, 0.45f)
};

CharacterViewSoundClass::~CharacterViewSoundClass()
{
	if (Sound) {
		Sound->Release_Ref();
		Sound = NULL;
	}
}


// 0x678450
CharacterViewClass::CharacterViewClass(LoginTaskClass *task, int mode) :
	UIViewClass((ClientTaskClass *)task, 2),
	// +0x4C: the camera's own constructor (0, 0, 0; 0, 0, 0; 45)


	Unknown068(NULL),
	Unknown06C(NULL),
	Unknown070(NULL),
	Unknown074(NULL),
	Unknown078(NULL),
	Unknown07C(NULL),
	Unknown080(NULL),
	Unknown084(NULL),
	AvatarRObj(NULL),
	NarratorRObj(NULL),
	Unknown19C(false),
	Unknown19D(false),
	Unknown19E(false),
	Unknown1A0(0.0f),
	Mode(mode),
	Unknown1B0(mode),
	Unknown1B4(NULL),
	Unknown224(NULL),
	Unknown228(false),
	Unknown22C(0),
	Unknown230(NULL),
	Unknown234(0),
	Unknown238(0),
	Unknown23C(0),
	Unknown240(0),
	Unknown3A4(NULL),
	Unknown4C8(0.0f, 0.0f, 0.0f),
	Unknown4D4(0.0f, 0.0f, 0.0f)
{
	int i;
	for (i = 0; i < 5; i++) {
		ViewRObjs[i] = NULL;
		Unknown1CC[i] = false;
	}
	for (i = 0; i < 2; i++) {
		Unknown1A4[i] = NULL;
		Unknown3A8[i] = 0;
		Unknown3B0[i] = 0;
	}
	for (i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			Unknown3B8[i][j][0] = NULL;
			Unknown3B8[i][j][1] = NULL;
		}
	}
	for (i = 0; i < 10; i++) {
		Unknown400[i][0].Set(0.5f, 0.5f); Unknown400[i][1].Set(0.5f, 0.5f);
	}
	for (i = 0; i < 2; i++) {
		Unknown4A0[i].Set(0.5f, 0.5f);
		Unknown4B0[i].Set(0.5f, 0.5f, 1.0f);
	}
	Unknown_04(); Unknown4E0.size(); Unknown4E0.size(); Unknown4E0.size(); Unknown4E0.size(); Unknown4E0.size(); Unknown4E0.size(); Unknown4E0.size();	// dead: VC6's inline budget (the client calls 0x6191F0 and 0x67D300 out of line)
}

// 0x678970: the implicit destructor (charview.h)





// 0x678B70
void CharacterViewClass::Unknown_04()
{
	UIViewClass::Unknown_04();

	Unknown06C = new SimpleSceneClass;
	Unknown06C->Set_Ambient_Light(Vector3(0.0f, 0.0f, 0.0f));
	Unknown068 = new CameraClass;
	Unknown070 = new LayerClass(Unknown06C, Unknown068, false, false, Vector3(0.0f, 0.0f, 0.0f));
	Unknown068->Set_Depth_Range(0.5f, 1.0f);
	Unknown068->Set_View_Plane(DEG_TO_RADF(45.0f), -1.0f);
	Unknown068->Set_Clip_Planes(1.0f, 1000.0f);

	Unknown074 = new LayerClass(Unknown1C, Unknown20.Peek(), false, false, Vector3(0.0f, 0.0f, 0.0f));
	Unknown1C->Set_Ambient_Light(Vector3(1.0f, 1.0f, 1.0f));

	Unknown07C = new SimpleSceneClass;
	Unknown07C->Set_Ambient_Light(Vector3(1.0f, 1.0f, 1.0f));
	Unknown078 = new CameraClass;
	Unknown078->Set_Position(Vector3(0.0f, 0.0f, 640.0f));
	Unknown078->Set_View_Plane(DEG_TO_RADF(45.0f), -1.0f);
	Unknown078->Set_Clip_Planes(1.0f, 10000.0f);

	Unknown084 = new SimpleSceneClass;
	Unknown084->Set_Ambient_Light(Vector3(0.0f, 0.0f, 0.0f));
	Unknown080 = new CameraClass;
	Unknown080->Set_Position(Vector3(0.0f, 0.0f, 640.0f));
	Unknown080->Set_View_Plane(DEG_TO_RADF(45.0f), -1.0f);
	Unknown080->Set_Clip_Planes(1.0f, 1000.0f);
	Unknown080->Set_Depth_Range(0.25f, 0.5f);

	Unknown230 = new DazzleLayerClass;
	DazzleRenderObjClass::Install_Dazzle_Visibility_Handler(NULL);

	if (Unknown224) {
		Unknown224->Stop(true);
		Unknown224->Release_Ref(); Unknown224 = NULL;
	}
	Unknown224 = SoundLibraryClass::Play_2D_Sound("fe_music.mp3", 1);
	if (Unknown224) {
		Unknown224->Set_Loop_Count(0);
	}

	Unknown3A4 = new EnvironmentMapManagerClass;
	Unknown3A4->Load("envmaps.ini");
	Unknown3A4->Unknown_760770(100000, true);
	Unknown_67BF20();
	Unknown4D4 = Unknown_BE6018[0];
	Unknown4C8 = Unknown_BE6018[0];
}


// 0x679160
void CharacterViewClass::Unknown_08()
{
	switch (Mode) {
		case 0:
			Unknown1B4 = new Unknown5DB670InterfaceClass(this, Unknown074);
			break;
		case 1:
			Unknown1B4 = new LoginInterface(this, Unknown074);
			break;
		case 2:
			Unknown1B4 = new AvatarSelectionInterface(this, Unknown074);
			break;
		case 3:
			Unknown1B4 = new Unknown574710InterfaceClass(this, Unknown074);
			break;
		case 4:
			Unknown1B4 = new AvatarCreationInterface(this, Unknown074, 0);
			break;
		case 5:
			Unknown1B4 = new Unknown615A50InterfaceClass(this, Unknown074);
			break;
		case 6:
			Unknown1B4 = new Unknown622180InterfaceClass(this, Unknown074, Unknown1B0 == 5, 0);
			break;
		case 7:
		case 8:
			return;
		case 12:
			Unknown1B4 = new RegistrationInterface(this, Unknown074, 0);
			break;
		case 10:
			Unknown1B4 = new RegistrationInterface(this, Unknown074, 20);
			break;
		case 11:
			Unknown1B4 = new RegistrationInterface(this, Unknown074, 3);
			break;
		case 9:
			Unknown1B4 = new RegistrationInterface(this, Unknown074, 3);
			break;
	}
	Unknown1B4->Unknown_54();
	Unknown_6B4940(Unknown1B4);
	((LoginTaskClass *)Unknown14->Unknown_08())->Unknown0018->Unknown_515400();
	LoginTaskClass *task = (LoginTaskClass *)Unknown14->Unknown_08(); task->Unknown0018->Unknown_515120(2);
}

// 0x679510: Shutdown, as calls to the file's own helpers (inlined; the client calls the deque's pop_front
// 0x67CEF0 out of line from 0x67C7B0's copy)
void CharacterViewClass::Unknown_10()
{
	int i;
	for (i = 0; i < 5; i++) {
		Unknown_67ACE0(i);
		Unknown_67A9B0(i);
	}
	Unknown_67C7B0();
	if (Unknown3A4) {
		delete Unknown3A4;
		Unknown3A4 = NULL;
	}
	Unknown_67BEB0();
	Unknown_67B220();
	Unknown_67A3F0();
	Unknown_67A670();
	Unknown_67B320();
	Unknown_67A300();
	UIViewClass::Unknown_10();
	if (Unknown06C) { Unknown06C->Release_Ref(); Unknown06C = NULL; }
	if (Unknown068) { Unknown068->Release_Ref(); Unknown068 = NULL; }
	if (Unknown07C) { Unknown07C->Release_Ref(); Unknown07C = NULL; }
	if (Unknown078) { Unknown078->Release_Ref(); Unknown078 = NULL; }
	if (Unknown084) { Unknown084->Release_Ref(); Unknown084 = NULL; }
	if (Unknown080) { Unknown080->Release_Ref(); Unknown080 = NULL; }
	if (Unknown070) {
		delete Unknown070;
		Unknown070 = NULL;
	}
	if (Unknown074) {
		delete Unknown074;
		Unknown074 = NULL;
	}
	delete Unknown230;
	Unknown230 = NULL;
}























// 0x679930: the avatars' +0x1C4 is their first own virtual (Null3DObjClass's table ends at +0x1C0)
int CharacterViewClass::Unknown_0C(bool on)
{
	Unknown_67CBD0();

	Matrix3D tm = Unknown068->Get_Transform();
	tm.Rotate_X(-WWMATH_PI / 2.0f);
	tm.Rotate_Z(WWMATH_PI / 2.0f);
	SoundLibraryClass::Set_Listener_Transform(tm);
	WWAudioClass::Get_Instance()->On_Frame_Update(0);

	float time = (float)WW3D::Get_Frame_Time() * 0.001f;
	Unknown1A0 += time * 0.2f;
	if (ViewRObjs[4]) {
		Unknown_Inline_Platform();
	}




	Unknown_67AF70(time);
	Unknown_67C530();

	if (AvatarRObj) {
		AvatarRObj->Unknown_1C4();
	}
	if (NarratorRObj) {
		NarratorRObj->Unknown_1C4();
		Unknown_67B090();
		Unknown_67B120();
























	}
	for (int i = 0; i < 2; i++) {
		if (Unknown1A4[i]) {
			Unknown1A4[i]->Unknown_1C4();
		}
	}

	if (ViewRObjs[1]) {
		Matrix3D camera = ViewRObjs[1]->Get_Bone_Transform("CAMERA");
		camera.Rotate_X(DEG_TO_RAD(Unknown04C.Rotation.X));
		camera.Rotate_Y(DEG_TO_RAD(Unknown04C.Rotation.Y));
		camera.Rotate_Z(DEG_TO_RAD(Unknown04C.Rotation.Z));
		camera.Translate_X(Unknown04C.Position.X);
		camera.Translate_Y(Unknown04C.Position.Y);
		camera.Translate_Z(Unknown04C.Position.Z);
		Unknown068->Set_Transform(camera);
	}

	WW3D::Begin_Render(on, true, Vector3(0.0f, 0.0f, 0.0f));
	PredictiveLODOptimizerClass::Clear();
	Unknown07C->Visibility_Check(Unknown078);
	Unknown06C->Visibility_Check(Unknown068);
	Unknown084->Visibility_Check(Unknown080);
	Unknown1C->Visibility_Check(Unknown20.Peek());
	Unknown24->Visibility_Check(Unknown28);
	DazzleRenderObjClass::Set_Current_Dazzle_Layer(Unknown230);
	WW3D::Render(Unknown07C, Unknown078, false, false, Vector3(0.0f, 0.0f, 0.0f));
	WW3D::Render(Unknown06C, Unknown068, false, false, Vector3(0.0f, 0.0f, 0.0f));
	Unknown230->Render(Unknown068);
	WW3D::Render(Unknown084, Unknown080, false, false, Vector3(0.0f, 0.0f, 0.0f));
	((InterfaceSceneClass *)Unknown1C)->Render_With_PIP_Scenes(Unknown20);
	WW3D::Render(Unknown24, Unknown28, false, false, Vector3(0.0f, 0.0f, 0.0f));
	WW3D::End_Render(true);
	return(0);
}

// 0x67A210
void CharacterViewClass::Unknown_18(bool on)
{
	if (Unknown1B4) {
		if (on) {
			Unknown1B4->Unknown_58();
		} else {
			Unknown1B4->Unknown_5C();
		}
	}
}

// 0x67A250
void CharacterViewClass::Unknown_67A250()
{
	for (int i = 0; i < 5; i++) {
		if (Unknown1CC[i]) {
			Unknown_67A9B0(i);
		}
		Unknown1CC[i] = false;
	}
}

// 0x67A290
void CharacterViewClass::Unknown_67A290(const Vector3 &position, const Vector3 &rotation, float fov)
{
	Unknown04C.Position = position;
	Unknown04C.Rotation = rotation;
	Unknown04C.Fov = fov;
	Unknown068->Set_View_Plane(DEG_TO_RAD(fov), -1.0f);
}

// 0x67A300
void CharacterViewClass::Unknown_67A300()
{
	if (Unknown1B4) {
		Unknown_18(false);
		Unknown1B4->Unknown_7C(NULL, 0);
		REF_PTR_RELEASE(Unknown1B4);
	}
}








// 0x67A370
void CharacterViewClass::Unknown_67A370(int mode)
{
	FAIL_IF(mode < CHARACTER_POPUP_MESSAGE || mode >= CHARACTER_LASTVIEWMODE) return;
	Unknown1B0 = Mode;
	Mode = mode;
	Unknown_08();
	Unknown_18(true);
}

// 0x67A3F0
void CharacterViewClass::Unknown_67A3F0()
{
	if (AvatarRObj) {
		AvatarRObj->Remove();
		AvatarRObj->Release_Ref();
		AvatarRObj = NULL;
	}
}















// 0x67A440 (the environment maps read through Unknown_Inline_Env_Maps: this in esi, the copy in edi, as the
// client has them; CV2)
void CharacterViewClass::Unknown_67A440(AvatarDescClass *desc, bool copy)
{
	if (AvatarRObj == NULL) {
		if (copy) {
			AvatarDescClass *fresh = new AvatarDescClass(desc->Race, desc->Profession, desc->Gender);
			AvatarRObj = new CharacterViewAvatarClass(fresh, 1, 1, Unknown_Inline_Env_Maps(this));
			delete fresh;
		} else {
			AvatarRObj = new CharacterViewAvatarClass(desc, 1, 1, Unknown_Inline_Env_Maps(this));
		}
		G_ASSERT(AvatarRObj);
		if (AvatarRObj) {
			AvatarRObj->Unknown_77E3D0(1);
			AvatarRObj->Unknown_Inline_231();

			AvatarRObj->Unknown232 = true;
		}
		TheDX8MeshRenderer.Invalidate();
	}
}











// 0x67A670
void CharacterViewClass::Unknown_67A670()
{
	if (NarratorRObj) {
		NarratorRObj->Unknown_784E00();
		NarratorRObj->Remove();
		NarratorRObj->Release_Ref();
		NarratorRObj = NULL;
	}
}


// 0x67A6C0
void CharacterViewClass::Unknown_67A6C0()
{
	if (NarratorRObj == NULL && ViewRObjs[4] != NULL) {
		AvatarDescClass *desc = new AvatarDescClass(1, (const char *)NULL);
		NarratorRObj = new CharacterViewAvatarClass(desc, 1, 0, NULL);
		delete desc;
		G_ASSERT(NarratorRObj);
		if (NarratorRObj) {
			ViewRObjs[4]->Add_Sub_Object_To_Bone(NarratorRObj, "NARRATOR");
			NarratorRObj->Unknown_77E3D0(1);
		}
		Unknown19C = true;
		Unknown19D = true;
		NarratorRObj->Unknown_786370(true);
		if (NarratorRObj && Unknown19C && Unknown19C == Unknown19D) {
			NarratorRObj->Unknown_784A50("/walk_on", 1, 1);
			Unknown22C = 3;
			Unknown19D = false;
		}
	}
}


// 0x67A8D0
void CharacterViewClass::Unknown_67A8D0(bool off)
{
	if (NarratorRObj && Unknown19C != off && Unknown19C == Unknown19D) {
		if (off) {
			NarratorRObj->Unknown_784A50("/walk_off", 1, 0);
		} else {
			NarratorRObj->Unknown_784A50("/walk_on", 1, 1);
			Unknown22C = 3;
		}
		Unknown19D = off;
	}
}


// 0x67A960
void CharacterViewClass::Unknown_67A960()
{
	if (NarratorRObj && Unknown19C == true) {
		NarratorRObj->Unknown_784C90("Idle0", 1);
		Unknown22C = 3;
		Unknown19E = true;
	}
}










// 0x67A9B0
void CharacterViewClass::Unknown_67A9B0(int type)
{
	if (ViewRObjs[type] == NULL) {
		return;
	}
	int i;
	switch (type) {
		case 0:
			Unknown1C->Remove_Render_Object(ViewRObjs[type]);
			break;
		case 3:
			Unknown07C->Remove_Render_Object(ViewRObjs[type]);
			break;
		case 4:
			for (i = 0; i < 2; i++) {
				if (Unknown3B0[i]) {
					Unknown3B0[i]->Remove();
					Unknown3B0[i]->Release_Ref();
					Unknown3B0[i] = NULL;
				}
			}
			Unknown084->Remove_Render_Object(ViewRObjs[type]);
			break;
		case 1:
		case 2:
			for (i = 0; i < 2; i++) {
				if (Unknown3A8[i]) {
					Unknown3A8[i]->Remove();
					Unknown3A8[i]->Release_Ref();
					Unknown3A8[i] = NULL;
				}
			}
			Unknown06C->Remove_Render_Object(ViewRObjs[type]);
			break;
	}
	ViewRObjs[type]->Release_Ref();
	ViewRObjs[type] = NULL;
	if (Unknown1D4[type].Anim) {
		Unknown1D4[type].Anim->Release_Ref();
		Unknown1D4[type].Anim = NULL;
	}
}


// 0x67AB20
void CharacterViewClass::Unknown_67AB20(int type, const char *name)
{
	Unknown_67A9B0(type);
	if (type == 2) {
		WW3D::Enable_Overbright_Modify_On_Load(true);
	}
	if (ViewRObjs[type] == NULL) {
		ViewRObjs[type] = WW3DAssetManager::Get_Instance()->Create_Render_Obj(name);
		G_ASSERT(ViewRObjs[type]);
		if (ViewRObjs[type]) {
			switch (type) {
				case 0:
					Unknown1C->Add_Render_Object(ViewRObjs[type]);
					break;
				case 3:
					Unknown07C->Add_Render_Object(ViewRObjs[type]);
					break;
				case 4:
					Unknown084->Add_Render_Object(ViewRObjs[type]);
					Unknown_67BBE0();
					break;
				case 1:
				case 2: {
					Unknown06C->Add_Render_Object(ViewRObjs[type]);
					for (int i = 0; i < ViewRObjs[type]->Get_Num_Sub_Objects(); i++) {
						RenderObjClass *sub = ViewRObjs[type]->Get_Sub_Object(i);
						sub->Set_LOD_Level(sub->Get_LOD_Count() - 1);
						sub->Release_Ref();
					}
					break;
				}
			}
		}
	}
	WW3D::Enable_Overbright_Modify_On_Load(false);
}

// 0x67ACE0
void CharacterViewClass::Unknown_67ACE0(int type)
{
	if (Unknown1D4[type].Anim) {
		Unknown1D4[type].Anim->Release_Ref();
		Unknown1D4[type].Anim = NULL;
	}
}

// 0x67AD30
bool CharacterViewClass::Unknown_67AD30(int type, const char *name)
{
	bool found = false;
	HAnimClass *anim = WW3DAssetManager::Get_Instance()->Get_HAnim(name);
	if (anim) {
		if (Unknown1D4[type].Anim) {
			Unknown1D4[type].Anim->Release_Ref();
			Unknown1D4[type].Anim = NULL;
		}
		Unknown1D4[type].Anim = anim;
		found = true;
	}
	Unknown1D4[type].Frame = -1.0f;
	Unknown1D4[type].Unknown0C = false;
	return(found);
}

// 0x67ADD0
void CharacterViewClass::Unknown_67ADD0(int type)
{
	if (Unknown1D4[type].Anim) {
		Unknown1D4[type].Mode = 1;
		ViewRObjs[type]->Set_Animation(Unknown1D4[type].Anim, 0.0f, 1);
	}
}

// 0x67AE20
void CharacterViewClass::Unknown_67AE20(int type)
{
	if (Unknown1D4[type].Anim) {
		Unknown1D4[type].Mode = 0;
		Unknown1D4[type].Unknown0D = false;
		Unknown1D4[type].Frame = 0.0f;
	}
}

// 0x67AE60
void CharacterViewClass::Unknown_67AE60(int type)
{
	if (Unknown1D4[type].Anim) {
		Unknown1D4[type].Mode = 0;
		Unknown1D4[type].Unknown0D = true;
		Unknown1D4[type].Frame = 0.0f;
	}
}

// 0x67AEA0
bool CharacterViewClass::Unknown_67AEA0(int type)
{
	if (Unknown1D4[type].Unknown0C) {
		Unknown1D4[type].Unknown0C = false;
		return(true);
	}
	return(false);
}

// 0x67AED0
void CharacterViewClass::Unknown_67AED0(int type, bool first)
{
	if (ViewRObjs[type]) {
		if (Unknown1D4[type].Anim) {
			if (first) {
				ViewRObjs[type]->Set_Animation(Unknown1D4[type].Anim, 0.0f, 0);
				return;
			}
			ViewRObjs[type]->Set_Animation(Unknown1D4[type].Anim, (float)(Unknown1D4[type].Anim->Get_Num_Frames() - 1), 0);
		}
	}
}


// 0x67B090
void CharacterViewClass::Unknown_67B090()
{
	if (Unknown19C != Unknown19D) {
		if (Unknown19D == true && NarratorRObj->Unknown220) {
			NarratorRObj->Unknown_786370(true);
			Unknown19C = Unknown19D;
		}
		if (Unknown19D == false) {
			if (Unknown22C) {
				Unknown22C--;
				return;
			}
			NarratorRObj->Unknown_786370(false);
			Unknown19C = Unknown19D;
		}
	}
}

// 0x67B120
void CharacterViewClass::Unknown_67B120()
{
	if (Unknown19E) {
		if (NarratorRObj->Unknown_7863E0()) {
			if (Unknown22C) {
				Unknown22C--;
				return;
			}
			NarratorRObj->Unknown_786370(false);
			return;
		}
		if (NarratorRObj->Unknown220) {
			NarratorRObj->Unknown_786370(true);
			Unknown19E = false;
		}
	}
}

// 0x67B1A0
void CharacterViewClass::Unknown_67B1A0(const char *name)
{
	if (Unknown224) {
		Unknown224->Stop(true);
		Unknown224->Release_Ref(); Unknown224 = NULL;
	}
	Unknown224 = SoundLibraryClass::Play_2D_Sound(name, 1);
	if (Unknown224) {
		Unknown224->Set_Loop_Count(0);
	}
}

// 0x67B220
void CharacterViewClass::Unknown_67B220()
{
	if (Unknown224) {
		Unknown224->Stop(true);
		Unknown224->Release_Ref(); Unknown224 = NULL;
	}
}

// 0x67B270
void CharacterViewClass::Unknown_67B270()
{
	Unknown34->Unknown_588A70("YES_NO", 0, "Are you sure you want to quit?", 320, 0, 0);
	Unknown228 = true;
}

// 0x67B2B0
bool CharacterViewClass::Unknown_67B2B0()
{
	if (Unknown228) {
		int button = Unknown34->Unknown_5890B0("YES_NO");
		if (button == 1) {
			Unknown34->Unknown_5C();
			((LoginTaskClass *)Unknown14->Unknown_08())->Unknown_6C();
		} else if (button > 1) {
			Unknown34->Unknown_5C();
			Unknown228 = false;
			return(true);
		}
	}
	return(false);
}



// 0x67B750
void CharacterViewClass::Unknown_67B750()
{
	for (int i = 0; i < 2; i++) {
		if (Unknown3A8[i]) {
			Unknown3A8[i]->Remove();
			Unknown3A8[i]->Release_Ref();
			Unknown3A8[i] = NULL;
		}
	}
}


// 0x67B7A0
void CharacterViewClass::Unknown_67B7A0(int set)
{
	Vector3 direction[2] = { Vector3(-0.66f, -1.0f, 1.0f), Vector3(-1.0f, -0.4f, 0.0f) };
	Vector3 diffuse[2] = { Vector3(1.0f, 1.0f, 1.0f), Vector3(1.0f, 1.0f, 1.0f) };
	Vector3 back[2] = { Vector3(0.0f, 0.0f, 0.0f), Vector3(0.8f, 0.3f, 0.3f) };
	float intensity[2] = { 3.0f, 1.0f };
	float back_intensity[2] = { 1.0f, 1.0f };
	if (Unknown06C) {
		for (int i = 0; i < 2; i++) {
			if (Unknown3A8[i]) {
				Unknown3A8[i]->Remove();
				Unknown3A8[i]->Release_Ref();
				Unknown3A8[i] = NULL;
			}
		}
		Unknown3A8[0] = new LightClass(LightClass::DIRECTIONAL);
		Unknown3A8[0]->Set_Intensity(intensity[set]);
		Unknown3A8[0]->Set_Force_Visible(true);
		Unknown3A8[0]->Set_Far_Attenuation_Range(1000000.0, 1000000.0);
		Unknown3A8[0]->Set_Ambient(Vector3(0.0f, 0.0f, 0.0f));
		Unknown3A8[0]->Set_Specular(Vector3(0.0f, 0.0f, 0.0f));
		Unknown3A8[0]->Set_Diffuse(diffuse[set]);
		Unknown3A8[0]->Set_Transform(Matrix3D::Identity);
		Unknown3A8[0]->Set_Spot_Direction(direction[set]);
		Unknown3A8[1] = new LightClass(LightClass::DIRECTIONAL);
		Unknown3A8[1]->Set_Intensity(back_intensity[set]);
		Unknown3A8[1]->Set_Force_Visible(true);
		Unknown3A8[1]->Set_Far_Attenuation_Range(1000000.0, 1000000.0);
		Unknown3A8[1]->Set_Ambient(Vector3(0.0f, 0.0f, 0.0f));
		Unknown3A8[1]->Set_Specular(Vector3(0.0f, 0.0f, 0.0f));
		Unknown3A8[1]->Set_Diffuse(back[set]);
		Unknown3A8[1]->Set_Transform(Matrix3D::Identity);
		Unknown3A8[1]->Set_Spot_Direction(-direction[set]);
		Unknown3A8[0]->Add(Unknown06C);
		Unknown3A8[1]->Add(Unknown06C);
	}
}

// 0x67BB90
void CharacterViewClass::Unknown_67BB90()
{
	for (int i = 0; i < 2; i++) {
		if (Unknown3B0[i]) {
			Unknown3B0[i]->Remove();
			Unknown3B0[i]->Release_Ref();
			Unknown3B0[i] = NULL;
		}
	}
}

// 0x67BBE0
void CharacterViewClass::Unknown_67BBE0()
{
	if (Unknown084) {
		for (int i = 0; i < 2; i++) {
			if (Unknown3B0[i]) {
				Unknown3B0[i]->Remove();
				Unknown3B0[i]->Release_Ref();
				Unknown3B0[i] = NULL;
			}
		}
		Unknown3B0[0] = new LightClass(LightClass::DIRECTIONAL);
		Unknown3B0[0]->Set_Intensity(1.0f);
		Unknown3B0[0]->Set_Force_Visible(true);
		Unknown3B0[0]->Set_Far_Attenuation_Range(1000000.0, 1000000.0);
		Unknown3B0[0]->Set_Ambient(Vector3(0.03f, 0.03f, 0.03f));
		Unknown3B0[0]->Set_Specular(Vector3(0.0f, 0.0f, 0.0f));
		Unknown3B0[0]->Set_Diffuse(Vector3(1.0f, 1.0f, 1.0f));
		Unknown3B0[0]->Set_Transform(Matrix3D::Identity);
		Unknown3B0[0]->Set_Spot_Direction(Vector3(1.0f, 0.0f, -0.75f));
		Unknown3B0[1] = new LightClass(LightClass::DIRECTIONAL);
		Unknown3B0[1]->Set_Intensity(1.0f);
		Unknown3B0[1]->Set_Force_Visible(true);
		Unknown3B0[1]->Set_Far_Attenuation_Range(1000000.0, 1000000.0);
		Unknown3B0[1]->Set_Ambient(Vector3(0.0f, 0.0f, 0.0f));
		Unknown3B0[1]->Set_Specular(Vector3(0.0f, 0.0f, 0.0f));
		Unknown3B0[1]->Set_Diffuse(Vector3(0.8f, 0.3f, 0.3f));
		Unknown3B0[1]->Set_Transform(Matrix3D::Identity);
		Unknown3B0[1]->Set_Spot_Direction(Vector3(-1.0f, 0.0f, 0.75f));
		Unknown3B0[0]->Add(Unknown084);
		Unknown3B0[1]->Add(Unknown084);
	}
}

// 0x67AF70
void CharacterViewClass::Unknown_67AF70(float time)
{
	for (int i = 0; i < 5; i++) {
		CharacterViewUnknown1D4Struct &anim = Unknown1D4[i];
		if (anim.Anim && anim.Mode == 0 && anim.Frame >= 0.0f) {
			float rate = anim.Anim->Get_Frame_Rate();
			float last = (float)(anim.Anim->Get_Num_Frames() - 1);
			if (anim.Frame < last) {
				if (anim.Frame != 0.0f) {
					anim.Frame = rate * time + anim.Frame;
				} else {
					anim.Frame = 1.0f;
				}
				if (anim.Frame >= last) {
					anim.Frame = last;
					anim.Unknown0C = true;
				}
			}
			if (anim.Unknown0D) {
				ViewRObjs[i]->Set_Animation(anim.Anim, last - anim.Frame, 0);
			} else {
				ViewRObjs[i]->Set_Animation(anim.Anim, anim.Frame, 0);
			}
		}
	}
}

// 0x67B320
void CharacterViewClass::Unknown_67B320()
{
	for (int i = 0; i < 2; i++) {
		if (Unknown1A4[i]) {
			if (ViewRObjs[4]) {
				ViewRObjs[4]->Remove_Sub_Object(Unknown1A4[i]);
			}
			Unknown1A4[i]->Unknown_788EF0();
			Unknown1A4[i]->Release_Ref();
			Unknown1A4[i] = NULL;
		}
	}
}

// 0x67B390
void CharacterViewClass::Unknown_67B390(int race, int profession)
{
	const char *bones[2] = { "male", "female" };
	if (ViewRObjs[4]) {
		int i;
		for (i = 0; i < 2; i++) {
			if (Unknown1A4[i]) {
				if (ViewRObjs[4]) {
					ViewRObjs[4]->Remove_Sub_Object(Unknown1A4[i]);
				}
				Unknown1A4[i]->Unknown_788EF0();
				Unknown1A4[i]->Release_Ref();
				Unknown1A4[i] = NULL;
			}
		}
		for (i = 0; i < 2; i++) {
			Unknown1A4[i] = Unknown3B8[race][profession][i];
			Unknown1A4[i]->Add_Ref();
			if (Unknown1A4[i]) {
				ViewRObjs[4]->Add_Sub_Object_To_Bone(Unknown1A4[i], bones[i]);
				Unknown1A4[i]->Unknown_77E3D0(1);
				Unknown1A4[i]->Unknown_7896B0(4);
				Unknown1A4[i]->Unknown_Inline_231();
				Unknown1A4[i]->Unknown232 = true;
			}
		}
	}
}


// 0x67B4F0
void CharacterViewClass::Unknown_67B4F0(bool on)
{
	if (ViewRObjs[4]) {
		RefCountPtr<RenderObjClass> platform = Create_Get(ViewRObjs[4]->Get_Sub_Object_By_Name("platform"));
		if (platform.Peek() != NULL) {
			platform->Set_Hidden(!on);
		}
	}
}

// 0x67B5C0 (no caller in the client: the frame's platform turn as a member of its own, ViewRObjs[4] tested)
void CharacterViewClass::Unknown_67B5C0()
{
	if (ViewRObjs[4]) {
		Matrix3D platform(true);
		int bone = ViewRObjs[4]->Get_Bone_Index("platform");
		platform.Rotate_Z(Unknown1A0);
		ViewRObjs[4]->Capture_Bone(bone);
		ViewRObjs[4]->Control_Bone(bone, platform, false);
	}
}

// 0x67C440
void CharacterViewClass::Unknown_67C440(int index, bool now)
{
	Unknown4D4 = Unknown_BE6018[index];
	if (now) {
		Unknown4C8 = Unknown_BE6018[index];
	}
}

// 0x67C4C0 (no caller in the client: 0x67C440 with the colour given)
void CharacterViewClass::Unknown_67C4C0(const Vector3 &color, bool now)
{
	Unknown4D4 = color;
	if (now) {
		Unknown4C8 = color;
	}
}



































































































































































// 0x67C1E0 (no caller in the client: empty)
void CharacterViewClass::Unknown_67C1E0()
{
}

#define MAX_OFFSET 10	// the offset pairs at +0x400 (0x67C200 / 0x67C270)  // NAME 3 [MAX_OFFSET]: the FAIL_IFs'

// 0x67C200
void CharacterViewClass::Unknown_67C200(Vector2 *offsets, int size, int which)
{
	FAIL_IF(size > MAX_OFFSET) return;
	for (int i = 0; i < size; i++) {
		offsets[i] = Unknown400[i][which];
	}
}



// 0x67C270
void CharacterViewClass::Unknown_67C270(const Vector2 *offsets, int size, int which)
{
	FAIL_IF(size > MAX_OFFSET) return;
	for (int i = 0; i < size; i++) {
		Unknown400[i][which] = offsets[i];
	}
}

// 0x67C2E0
void CharacterViewClass::Unknown_67C2E0(Vector2 *out, int index)
{
	*out = Unknown4A0[index];
}

// 0x67C310
void CharacterViewClass::Unknown_67C310(const Vector2 *in, int index)
{
	Unknown4A0[index] = *in;
}

// 0x67C340
void CharacterViewClass::Unknown_67C340(Vector3 *out, int index)
{
	*out = Unknown4B0[index];
}

// 0x67C380
void CharacterViewClass::Unknown_67C380(const Vector3 *in, int index)
{
	Unknown4B0[index] = *in;
}

// 0x67BEB0
void CharacterViewClass::Unknown_67BEB0()
{
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (Unknown3B8[i][j][0]) {
				Unknown3B8[i][j][0]->Release_Ref();
				Unknown3B8[i][j][0] = NULL;
			}
			if (Unknown3B8[i][j][1]) {
				Unknown3B8[i][j][1]->Release_Ref();
				Unknown3B8[i][j][1] = NULL;
			}
		}
	}
}

// 0x67BF20 (Unknown_04: an avatar of each race, profession and gender, the body weights set)
void CharacterViewClass::Unknown_67BF20()
{
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			AvatarDescClass *male = new AvatarDescClass(i, j, 0);
			AvatarDescClass *female = new AvatarDescClass(i, j, 1);
			male->Body_Weight[0] = 0.5f;
			male->Body_Weight[4] = 0.25f;
			male->Body_Weight[3] = 0.25f;
			female->Body_Weight[0] = 0.5f;
			male->Body_Weight[2] = 0.25f;
			female->Body_Weight[1] = 0.25f;
			Unknown3B8[i][j][0] = new CharacterViewAvatarClass(male, 1, 0, Unknown_Inline_Env_Maps(this));
			Unknown3B8[i][j][1] = new CharacterViewAvatarClass(female, 1, 0, Unknown_Inline_Env_Maps(this));
			delete male;
			delete female;
		}
	}
}

// 0x67C7B0
void CharacterViewClass::Unknown_67C7B0()
{
	SoundLibraryClass::Stop_Dialog();
	while (Unknown4E0.size() != 0) {
		Unknown4E0.pop_front();
	}
}

// The queue's push_back one inline level down, as Unknown_Inline_Pop below: 0x67C920 then calls the element's
// _Construct 0x433700 out of line on all three paths and _Buyback 0x67CFE0 on the second, as the client does
// (CV2: a direct push_back inlines one _Construct; dead reads after it send _Buyback out on every path).
static inline void Unknown_Inline_Push(std::deque<std::string> &queue, const std::string &name)
{
	queue.push_back(name);
}

// 0x67C920 (the screens queue a dialog; an empty name is dropped)
void CharacterViewClass::Unknown_67C920(std::string name)
{
	if (name.length()) {
		Unknown_Inline_Push(Unknown4E0, name);
	}
}

// The queue's pop_front one inline level down: 0x67CBD0 then expands pop_front but calls its _Freefront 0x67D1A0
// and the element's destructor (??_G 0x433A10) out of line, as the client does (CV2: a direct pop_front
// inlines both, dead reads after it push the string copy out of line first).
static inline void Unknown_Inline_Pop(std::deque<std::string> &queue)
{
	queue.pop_front();
}

// 0x67CBD0
void CharacterViewClass::Unknown_67CBD0()
{
	SoundLibraryClass::Update_Dialog();
	if (SoundLibraryClass::Is_Dialog_Playing() || Unknown4E0.size() == 0) {
		return;
	}
	std::string name = Unknown4E0.front();
	Unknown_Inline_Pop(Unknown4E0);
	if (name.length()) {
		SoundLibraryClass::Play_Dialog(name.c_str(), 1);
	}
}

// 0x67CD40 (no caller in the client: 0x67C7B0's loop without the dialog stop)
void CharacterViewClass::Unknown_67CD40()
{
	while (Unknown4E0.size() != 0) {
		Unknown4E0.pop_front();
	}
}


// 0x67C530
void CharacterViewClass::Unknown_67C530()
{
	if (ViewRObjs[3]) {
		Unknown4C8 = Unknown4D4 * 0.05f + Unknown4C8 * 0.95f;
		RefCountPtr<RenderObjClass> box = Create_Get(ViewRObjs[3]->Get_Sub_Object_By_Name("box02"));
		if (box.Peek() != NULL) {
			RefCountPtr<MaterialInfoClass> info = Create_Get(box->Get_Material_Info());
			if (info.Peek() != NULL && info->Peek_Vertex_Material(0) != NULL) {
				info->Peek_Vertex_Material(0)->Set_Emissive(Unknown4C8);
			}
			if (ViewRObjs[4] && Unknown3B0[1]) {
				float r = (Unknown4C8.X < 0.5f) ? (Unknown4C8.X + 0.5f) * 0.5f : Unknown4C8.X;
				float g = (Unknown4C8.Y < 0.5f) ? (Unknown4C8.Y + 0.5f) * 0.5f : Unknown4C8.Y;
				float b = (Unknown4C8.Z < 0.5f) ? (Unknown4C8.Z + 0.5f) * 0.5f : Unknown4C8.Z;
				Unknown3B0[1]->Set_Diffuse(Vector3(r, g, b));
			}
		}
	}
}














/*
**	Reconstruction notes (client.exe 0x678450-0x67D300; vtable 0xAFBD5C, slots in charview.h).
**
**	The constructor 0x678450 passes the task to UIViewClass with the view's kind 2 and calls Unknown_04
**	(0x678B70) itself. Its +0x08 0x679160 makes the mode's screen over the +0x74 layer (mode 1: ui_login.cpp's
**	LoginInterface 0x5ACB80, 2: ui_avsel.cpp's AvatarSelectionInterface 0x55C650), activates it (+0x54),
**	adds it to the view (0x6B4940) and resets the login task's software mouse (0x515400, 0x515120(2)).
**	The frame 0x679930: the listener from the 3-D camera, the platform's turn, the avatars, the camera from
**	the model's CAMERA bone, then the backdrop (+0x7C / +0x78), the 3-D scene (+0x6C / +0x68) and its dazzles,
**	+0x84 / +0x80, and the base's 2-D scenes.
**	Written functional first for the proof-of-concept client (D64, D179); the asserts' lines: 0x67A370's
**	FAIL_IF on 505, 0x67A440 'AvatarRObj' 548, 0x67A6C0 'NarratorRObj' 588, 0x67AB20 'ViewRObjs[type]' 693,
**	0x67C200 / 0x67C270 'size > MAX_OFFSET' 1274 / 1285.
*/
